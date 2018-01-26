////////////////////////////////////////////////////////////////////////////////
// Copyright (C) 2018 Jan Moritz Joseph
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
////////////////////////////////////////////////////////////////////////////////
#include "RouterVC.h"

RouterVC::RouterVC(sc_module_name nm, Node* node) :
		Router(nm, node) {
	int conCount = node->connections.size();
	rInfo = new RoutingInformation(node);
	classicPortContainer.init(conCount);
	buffer.resize(conCount);
	flowControlOut.resize(conCount);
	tagOut.resize(conCount);
	pkgcnt.resize(conCount);
	routedFlits.resize(conCount);
	for (int dir = 0; dir < conCount; dir++) {
		int vcCount = node->connections.at(dir)->getVCCountForNode(node);
		buffer.at(dir) = new std::vector<BufferFIFO<Flit*>*>(vcCount);
		flowControlOut.at(dir) = new std::vector<bool>(vcCount, true);
		tagOut.at(dir) = new std::vector<int>(vcCount, 0);
		pkgcnt.at(dir) = new std::vector<int>(vcCount, 0);
		routedFlits.at(dir) = new std::vector<Flit*>(vcCount);
		for (int vc = 0; vc < vcCount; vc++) {
			BufferFIFO<Flit*>* buf = new BufferFIFO<Flit*>(node->connections.at(dir)->getBufferDepthForNode(node));
			buffer.at(dir)->at(vc) = buf;
			rep.reportAttribute(buf->dbid, "node", std::to_string(node->id));
			rep.reportAttribute(buf->dbid, "dir", std::to_string(dir));
			rep.reportAttribute(buf->dbid, "vc", std::to_string(vc));
			rInfo->tagOut[Channel(dir, vc)]=0;

		}

	}
	if(node->type->routerModel=="XYZ"){
		routing = new RoutingXYZ(node);
	}else if(node->type->routerModel=="DyXYZ"){
		//routing = new RoutingDyXYZ(node);
	}else if(node->type->routerModel=="EDXYZ"){
		//routing = new RoutingEDXYZ(node);
	}else{
		LOG(true, "Router" << id << "\t- Unknown Routing: " << node->type->routerModel);
	}



	selection = new SelectionRoundRobin(node);

}

RouterVC::~RouterVC() {

}

void RouterVC::initialize() {
	checkValid();

	for (int con = 0; con < node->connections.size(); con++) {
		classicPortContainer.at(con).portValidOut.write(false);
		classicPortContainer.at(con).portFlowControlOut.write(flowControlOut.at(con));
		classicPortContainer.at(con).portTagOut.write(tagOut.at(con));
	}

	SC_METHOD(thread);
	sensitive << clk.pos();

	SC_METHOD(negThread);
	sensitive << clk.neg();
	//sc_spawn(sc_bind(&RouterVC::congestionThread, this));
}

void RouterVC::checkValid() {
	for (Node* n : node->connectedNodes) {
		assert(n->type->clockSpeed == node->type->clockSpeed);
	}
}

void RouterVC::bind(Connection* con, SignalContainer* sigContIn, SignalContainer* sigContOut) {
	classicPortContainer.at(node->conToPos.at(con)).bind(sigContIn, sigContOut);
}

void RouterVC::thread() {
	for (int i = 0; i < node->connections.size(); i++) {
		Connection* c = node->connections.at(i);
		std::vector<bool>* fc = classicPortContainer.at(i).portFlowControlIn.read();
		std::vector<int>* tag = classicPortContainer.at(i).portTagIn.read();

		for (int vc = 0; vc < fc->size(); vc++) {
			rInfo->flowIn[Channel(i,vc)]=fc->at(vc);
			rInfo->tagIn[Channel(i,vc)]=tag->at(vc);
			if(tag->at(vc)==-1){
				rInfo->tag[Channel(i,vc)]=-1;
//				Channel c = arbiterTable.at(i).first;
//				if(pkgcnt.at(c.dir)->at(c.vc)==1){
//					rInfo->tagOut[c]=tag->at(vc);
//				}
			}else if (tag->at(vc)==0){
				rInfo->tag[Channel(i,vc)]=tag->at(vc);
			}else if (tag->at(vc)==1){
				rInfo->tag[Channel(i,vc)]=c->vcCount.at(0);
			}
		}
	}

	// #3 send
	send();
	// #2 arbitrate // three stage
	arbitrate();
	// #1 receive & route
	receive();
	route();
	// two stage
	//arbitrate();
}

void RouterVC::negThread() {
	// used to set signals that could propagate instantaneously else
	for (int dir = 0; dir < node->connections.size(); dir++) {
		for (int vc = 0; vc < buffer.at(dir)->size(); vc++) {
			Connection* c = node->connections.at(dir);
			c->congestion.at(c->nodePos.at(node)).at(vc) = buffer.at(dir)->at(vc)->occupied();

			//set flow control flag
			flowControlOut.at(dir)->at(vc) = buffer.at(dir)->at(vc)->free() >= 4;

			//set tag
			if(buffer.at(dir)->at(vc)->empty()){
				tagOut.at(dir)->at(vc) = 1;
				rInfo->tagOut[Channel(dir,vc)] = 1;
			}else{
				tagOut.at(dir)->at(vc)= rInfo->tagOut.at(Channel(dir,vc));
			}

			if (c->congestion.at(c->nodePos.at(node)).at(vc) >= 15){
				LOG(false, "Router" << id << "[" << DIR::toString(node->conToDir[dir]) << vc << "]\t- BUFFER " << c->congestion.at(c->nodePos.at(node)).at(vc));
			}
		}
		classicPortContainer.at(dir).portFlowControlOut.write(flowControlOut.at(dir));
		classicPortContainer.at(dir).portTagOut.write(tagOut.at(dir));
	}

}


void RouterVC::receive() {


	//read input ports to buffers
	for (int dirIn = 0; dirIn < node->connections.size(); dirIn++) {
		if (classicPortContainer.at(dirIn).portValidIn.read()) {
			Flit* flit = classicPortContainer.at(dirIn).portDataIn.read();
			int vcIn = classicPortContainer.at(dirIn).portVcIn.read();
			BufferFIFO<Flit*>* buf = buffer.at(dirIn)->at(vcIn);

			if(flit->type == HEAD){
				LOG((global.verbose_router_receive_head_flit) || global.verbose_router_receive_flit,
					"Router" << id << "[" << DIR::toString(node->conToDir[dirIn])<<vcIn<< "]\t- Receive Flit " << *flit);
				pkgcnt.at(dirIn)->at(vcIn)++;
			}

			rep.reportEvent(dbid, "router_receive_flit", std::to_string(flit->id));

			if (!buf->enqueue(flit)) {
				LOG(global.verbose_router_buffer_overflow, "Router" << id << "[" << DIR::toString(node->conToDir[dirIn]) << vcIn << "]\t- Buffer Overflow " << *flit);
			} else {
				rep.reportEvent(buf->dbid, "buffer_enqueue_flit", std::to_string(flit->id));
				if(flit->type == HEAD){
					rpInfo[flit->packet] = new RoutingPacketInformation(flit->packet);
					rpInfo[flit->packet]->inputChannel=Channel(dirIn, vcIn);
				}
			}
		}
	}
}

void RouterVC::route() {
	for (int dirIn = 0; dirIn < node->connections.size(); dirIn++) {
		for (int vcIn = 0; vcIn < buffer.at(dirIn)->size(); vcIn++) {
			BufferFIFO<Flit*>* buf = buffer.at(dirIn)->at(vcIn);
			Flit* flit = buf->front();

			if (flit && !routedPackets.count(flit->packet)) {
				rep.reportEvent(buf->dbid, "buffer_front_flit", std::to_string(flit->id));

				if (flit->type == HEAD) {
					routing->route(rInfo, rpInfo.at(flit->packet));
					selection->select(rInfo, rpInfo.at(flit->packet));
					routedPackets.insert(flit->packet);
					rep.reportEvent(dbid, "router_route_packet", std::to_string(flit->id));

				} else {
					LOG(true, "Router" << id << "[" << DIR::toString(node->conToDir[dirIn]) << vcIn << "]\t- Received Body/Tail Flit before Head Flit! " << *flit);
					buf->dequeue();
					rep.reportEvent(buf->dbid, "buffer_dequeue_flit", std::to_string(flit->id));

					continue;
				}
			}
			if (flit && !routedFlits.at(dirIn)->at(vcIn)){

					routedFlits.at(dirIn)->at(vcIn) = buf->dequeue();
					rep.reportEvent(buf->dbid, "buffer_dequeue_flit", std::to_string(flit->id));
				}
		}
	}

}

void RouterVC::arbitrate() {
	std::set<int> arbiterDirs;
	for (unsigned int dirIn = rrDirOff, i = 0; i < buffer.size(); dirIn = (dirIn + 1) % buffer.size(), i++) {
		for (unsigned int vcIn = 0; vcIn < buffer.at(dirIn)->size(); vcIn++) {
			Flit* flit = routedFlits.at(dirIn)->at(vcIn);

			if (flit) {
				//rep.reportEvent(buf->dbid, "buffer_front_flit", std::to_string(flit->id));



				if(flit->type == HEAD){
					routing->makeDecision(rInfo, rpInfo.at(flit->packet));

					if(rpInfo.at(flit->packet)->rerouteFlag){
						routing->route(rInfo, rpInfo.at(flit->packet));
						selection->select(rInfo, rpInfo.at(flit->packet));

						LOG(true, "Router" << id << "[" << DIR::toString(node->conToDir[dirIn]) << vcIn<<"]\t- Reroute! " << *flit);
						continue;
					}else if (rpInfo.at(flit->packet)->delayFlag) {
						LOG(global.verbose_router_throttle, "Router" << id << "[" << DIR::toString(node->conToDir[dirIn]) << dirIn<<"]\t- Delay! " << *flit);
						continue;
					}
				}
				if(rpInfo.at(flit->packet)->unableFlag){
					FATAL("Router" << id << "[" << DIR::toString(node->conToDir[dirIn]) << vcIn<<"]\t- Unable to route Packet! " << *flit);
				}

				Channel out = rpInfo.at(flit->packet)->outputChannel;


				if (!classicPortContainer.at(out.dir).portFlowControlIn.read()->at(out.vc)) {
					LOG(global.verbose_router_throttle, "Router" << id << "[" << DIR::toString(node->conToDir[dirIn]) << dirIn<<"]\t- Flow Control! " << *flit);
					continue;
				}

//				for (std::pair<int, bool> availableChannel: classicPortContainer.at(dirOut).portFlowControlIn.read()) {
//					if(availableChannel.second){
//						if(routerTable.at(flit).count(Channel(dirOut, availableChannel.first))){
//							routerTable.clear();
//							routerTable[flit] = Channel(dirOut, availableChannel.first);
//							break;
//						}
//					}
//				}


				//make sure router table has only one entry per flit
				if (!arbiterDirs.count(out.dir)) {
					if (flit->type == HEAD && !occupyTable.count(out)) {
						occupyTable[out] = flit->packet;
						LOG(global.verbose_router_assign_channel, "Router" << id << "[" << DIR::toString(node->conToDir[out.dir]) << out.vc<<"]\t- Assign to " << *flit);
					}

					if (occupyTable.at(out) == flit->packet) {
						arbitratedFlits.insert(flit);
						routedFlits.at(dirIn)->at(vcIn)=0;
					}
				}

			}
		}
	}
	rrDirOff = (rrDirOff + 1) % buffer.size();

}

void RouterVC::send() {

	for (unsigned int con = 0; con < node->connections.size(); con++) {
		classicPortContainer.at(con).portValidOut.write(false);
	}

	for (Flit* flit : arbitratedFlits) {
		Channel out = rpInfo.at(flit->packet)->outputChannel;
		Channel in = rpInfo.at(flit->packet)->inputChannel;

		classicPortContainer.at(out.dir).portValidOut.write(true);
		classicPortContainer.at(out.dir).portDataOut.write(flit);
		classicPortContainer.at(out.dir).portVcOut.write(out.vc);

		if (flit->type == TAIL) {
			routedPackets.erase(flit->packet);
			occupyTable.erase(rpInfo.at(flit->packet)->outputChannel);
			rpInfo.erase(flit->packet);
			pkgcnt.at(in.dir)->at(in.vc)--;
		}

		LOG((global.verbose_router_send_head_flit && flit->type == HEAD) || global.verbose_router_send_flit,
				"Router" << id << "[" << DIR::toString(node->conToDir[out.dir]) << out.vc<<"]\t- send flit " << *flit);
		rep.reportEvent(dbid, "router_send_flit", std::to_string(flit->id));
	}
	arbitratedFlits.clear();
}

