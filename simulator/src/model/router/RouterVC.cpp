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
	crossbarcount = 0;
	rInfo = new RoutingInformation(node);
	classicPortContainer.init(conCount);
	buffer.resize(conCount);
	flowControlOut.resize(conCount);
	tagOut.resize(conCount);
	emptyOut.resize(conCount);
	pkgcnt.resize(conCount);
	routedFlits.resize(conCount);
	for (int dir = 0; dir < conCount; dir++) {
		int vcCount = node->connections.at(dir)->getVCCountForNode(node);
		buffer.at(dir) = new std::vector<BufferFIFO<Flit*>*>(vcCount);
		flowControlOut.at(dir) = new std::vector<bool>(vcCount, true);
		tagOut.at(dir) = new std::vector<int>(vcCount, 10); //Why 10?
		emptyOut.at(dir) = new std::vector<bool>(vcCount, true);
		pkgcnt.at(dir) = new std::vector<int>(vcCount, 0);
		routedFlits.at(dir) = new std::vector<Flit*>(vcCount);
		for (int vc = 0; vc < vcCount; vc++) {
			//TODO HACK routedFlit is an additional buffer, which must be substracted here.
			BufferFIFO<Flit*>* buf = new BufferFIFO<Flit*>(
					node->connections.at(dir)->getBufferDepthForNode(node) - 1);
			buffer.at(dir)->at(vc) = buf;
			rep.reportAttribute(buf->dbid, "node", std::to_string(node->id));
			rep.reportAttribute(buf->dbid, "dir", std::to_string(dir));
			rep.reportAttribute(buf->dbid, "vc", std::to_string(vc));
			rInfo->tagOut[Channel(dir, vc)] = 0;

		}

	}

	if (node->type->routing == "XYZ") {
		routing = new RoutingXYZ(node);
	} else if (node->type->routing == "TMR") {
		routing = new RoutingTMR(node);
	} else if (node->type->routing == "DPR") {
		routing = new RoutingDPR(node);
	} else if (node->type->routing == "ESPADA") {
		//routing = new RoutingESPADA(node);
	} else {
		FATAL("Router" << id << "\t- Unknown Routing: " << node->type->routing);
	}

	if (node->type->selection == "RoundRobin") {
		selection = new SelectionRoundRobin(node);
	} else if (node->type->selection == "OutputRoundRobin") {
		selection = new SelectionOutputRoundRobin(node);
	} else if (node->type->selection == "DyXYZ") {
		selection = new SelectionDyXYZ(node);
	} else if (node->type->selection == "EDXYZ") {
		selection = new SelectionEDXYZ(node);
	} else if (node->type->selection == "AgRA") {
		//	selection = new SelectionAgRA(node);
	} else if (node->type->selection == "MAFA") {
		// selection = new SelectionMAFA(node);
	} else {
		FATAL(
				"Router" << id << "\t- Unknown selection: " << node->type->selection);
	}

}

RouterVC::~RouterVC() {

}

void RouterVC::initialize() {
	checkValid();

	routing->checkValid();
	//selection->checkValid();

	SC_METHOD(thread);
	sensitive << clk.pos() << clk.neg();

	SC_METHOD(receive);
	for (int con = 0; con < node->connections.size(); con++) {
		classicPortContainer.at(con).portValidOut.write(false);
		classicPortContainer.at(con).portFlowControlOut.write(
				flowControlOut.at(con));
		classicPortContainer.at(con).portTagOut.write(tagOut.at(con));
		classicPortContainer.at(con).portEmptyOut.write(emptyOut.at(con));
		sensitive << classicPortContainer.at(con).portValidIn.pos();
	}

	//sc_spawn(sc_bind(&RouterVC::congestionThread, this));
}

void RouterVC::checkValid() {

}

void RouterVC::bind(Connection* con, SignalContainer* sigContIn,
		SignalContainer* sigContOut) {
	classicPortContainer.at(node->conToPos.at(con)).bind(sigContIn, sigContOut);
}

void RouterVC::thread() {
	if (clk.posedge()) {
		readControl();
		routing->beginCycle(rInfo);

		// #3 send
		send();
		// #2 arbitrate // three stage
		arbitrate();
		// #1 receive & route
		//receive();
		route();
		// two stage
		//arbitrate();

	} else if (clk.negedge()) {
		//unset valids
		for (unsigned int con = 0; con < node->connections.size(); con++) {
			classicPortContainer.at(con).portValidOut.write(false);
		}

		// used to set signals that could propagate instantaneously else
		for (int dir = 0; dir < node->connections.size(); dir++) {

			Connection* c = node->connections.at(dir);
			int bufferDepth = c->bufferDepth.at(c->nodePos.at(node));
			c->bufferUtilization.at(c->nodePos.at(node)) = 0;		//TODO
			c->bufferCongestion.at(c->nodePos.at(node)) = 0;

			for (int vc = 0; vc < buffer.at(dir)->size(); vc++) {
				int occupiedSlots = buffer.at(dir)->at(vc)->occupied();
				c->vcBufferUtilization.at(c->nodePos.at(node)).at(vc) =
						occupiedSlots;
				c->vcBufferCongestion.at(c->nodePos.at(node)).at(vc) =
						(float) occupiedSlots / (float) bufferDepth;
				c->bufferUtilization.at(c->nodePos.at(node)) += occupiedSlots;
				c->bufferCongestion.at(c->nodePos.at(node)) +=
						(float) occupiedSlots / (float) bufferDepth;
			}
			c->bufferCongestion.at(c->nodePos.at(node)) /=
					buffer.at(dir)->size();
		}

		node->congestion = (float) crossbarcount
				/ (float) node->connections.size();
		routing->endCycle(rInfo);
		writeControl();

	}

}

void RouterVC::readControl() {
	for (int i = 0; i < node->connections.size(); i++) {
		std::vector<bool>* fc =
				classicPortContainer.at(i).portFlowControlIn.read();
		std::vector<int>* tag = classicPortContainer.at(i).portTagIn.read();
		std::vector<bool>* empty =
				classicPortContainer.at(i).portEmptyIn.read();

		for (int vc = 0; vc < fc->size(); vc++) {
			rInfo->flowIn[Channel(i, vc)] = fc->at(vc);
			rInfo->tagIn[Channel(i, vc)] = tag->at(vc);
			rInfo->emptyIn[Channel(i, vc)] = empty->at(vc);
		}
	}
}

void RouterVC::writeControl() {
	for (int dir = 0; dir < node->connections.size(); dir++) {
		for (int vc = 0; vc < buffer.at(dir)->size(); vc++) {
			//TODO HACK this needs to be 2 due to signal propagation speed to upstream router. Please note, this should be fixed as it may be not safe for all cases. 4 is too large, 3 might be okay
			// the value should be dynamic, i.e. counter in upstream router
			flowControlOut.at(dir)->at(vc) = buffer.at(dir)->at(vc)->free()
					>= 2;
			tagOut.at(dir)->at(vc) = rInfo->tagOut.at(Channel(dir, vc));
			emptyOut.at(dir)->at(vc) = buffer.at(dir)->at(vc)->empty();
		}
		classicPortContainer.at(dir).portFlowControlOut.write(
				flowControlOut.at(dir));
		classicPortContainer.at(dir).portTagOut.write(tagOut.at(dir));
		classicPortContainer.at(dir).portEmptyOut.write(emptyOut.at(dir));
	}
}

void RouterVC::receive() {

	//read input ports to buffers
	for (int dirIn = 0; dirIn < node->connections.size(); dirIn++) {
		if (classicPortContainer.at(dirIn).portValidIn.read()) {
			Flit* flit = classicPortContainer.at(dirIn).portDataIn.read();
			int vcIn = classicPortContainer.at(dirIn).portVcIn.read();
			BufferFIFO<Flit*>* buf = buffer.at(dirIn)->at(vcIn);

			if (flit->type == HEAD) {
				LOG(
						(global.verbose_router_receive_head_flit)
								|| global.verbose_router_receive_flit,
						"Router" << id << "[" << DIR::toString(node->conToDir[dirIn])<<vcIn<< "]\t- Receive Flit " << *flit);
				pkgcnt.at(dirIn)->at(vcIn)++;rInfo
				->tagOut.at(Channel(dirIn, vcIn)) = flit->packet->pkgclass;
			}

			//rep.reportEvent(dbid, "router_receive_flit", std::to_string(flit->id));

			if (!buf->enqueue(flit)) {
				LOG(global.verbose_router_buffer_overflow,
						"Router" << id << "[" << DIR::toString(node->conToDir[dirIn]) << vcIn << "]\t- Buffer Overflow " << *flit);
			} else {
				rep.reportEvent(buf->dbid, "buffer_enqueue_flit",
						std::to_string(flit->id));
				if (flit->type == HEAD) {
					Channel c = Channel(dirIn, vcIn);

					if (rpInfo.count( { flit->packet, c })) {
						FATAL("packet cycle!");
					}

					rpInfo[ { flit->packet, c }] = new RoutingPacketInformation(
							flit->packet);
					rpInfo[ { flit->packet, c }]->inputChannel = c;
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
			Channel c = Channel(dirIn, vcIn);

			if (sc_time_stamp() == sc_time(120, SC_NS) && this->node->id == 2
					&& flit)
				cout << "flit output @ " << sc_time_stamp() << "   " << flit->id
						<< " seq nb" << flit->seq_nb << endl;

			if (flit && !routedPackets.count( { flit->packet, c })) {
				//rep.reportEvent(buf->dbid, "buffer_front_flit", std::to_string(flit->id));

				if (flit->type == HEAD) {
					RoutingPacketInformation* rpi = rpInfo.at(
							{ flit->packet, c });
					routing->route(rInfo, rpi);

					if (rpi->routedChannel.empty()) {
						rpi->dropFlag = true;
						rep.reportEvent(dbid, "router_routefail",
								std::to_string(flit->id));
						//FATAL("Router" << id << "[" << DIR::toString(node->conToDir[dirIn]) << dirIn<<"]\t- Failed Routing! " << *flit);
					}

					selection->select(rInfo, rpi);

					if (rpi->selectedChannel.empty()) {
						rpi->dropFlag = true;
						rep.reportEvent(dbid, "router_selectfail",
								std::to_string(flit->id));
						//FATAL("Router" << id << "[" << DIR::toString(node->conToDir[dirIn]) << dirIn<<"]\t- Failed Selection! " << *flit);
					}

					routedPackets.insert( { flit->packet, c });
					//rep.reportEvent(dbid, "router_route_packet", std::to_string(flit->id));

					flit->packet->numhops++;
					flit->packet->traversedRouter.push_back(node->id);
					flit->packet->routerIDs.insert(node->id);

				} else {
					LOG(true,
							"Router" << id << "[" << DIR::toString(node->conToDir[dirIn]) << vcIn << "]\t- Received Body/Tail Flit before Head Flit! " << *flit);
					buf->dequeue();
					rep.reportEvent(buf->dbid, "buffer_dequeue_flit",
							std::to_string(flit->id));

					continue;
				}
			}
			if (flit && !routedFlits.at(dirIn)->at(vcIn)) {
				routedFlits.at(dirIn)->at(vcIn) = buf->dequeue();
				rep.reportEvent(buf->dbid, "buffer_dequeue_flit",
						std::to_string(flit->id));
			}
		}
	}

}

void RouterVC::arbitrate() {
	std::set<int> arbiterDirs;
	for (unsigned int dirIn = rrDirOff, i = 0; i < buffer.size();
			dirIn = (dirIn + 1) % buffer.size(), i++) {
		for (unsigned int vcIn = 0; vcIn < buffer.at(dirIn)->size(); vcIn++) {
			Flit* flit = routedFlits.at(dirIn)->at(vcIn);
			Channel c = Channel(dirIn, vcIn);

			if (flit) {
				//rep.reportEvent(buf->dbid, "buffer_front_flit", std::to_string(flit->id));
				RoutingPacketInformation* rpi = rpInfo.at( { flit->packet, c });

				if (flit->type == HEAD) {
					routing->makeDecision(rInfo, rpi);

					if (rpInfo.at( { flit->packet, c })->dropFlag) {
						rep.reportEvent(dbid, "router_decisfail",
								std::to_string(flit->id));
					}

					if (rpi->rerouteFlag) {
						rpi->rerouteFlag = false;
						LOG(true,
								"Router" << id << "[" << DIR::toString(node->conToDir[dirIn]) << vcIn<<"]\t- Reroute! " << *flit);
						routing->route(rInfo, rpi);
						selection->select(rInfo, rpi);

						if (rpi->selectedChannel.empty()) {
							rpi->dropFlag = true;
							rep.reportEvent(dbid, "router_select2fail",
									std::to_string(flit->id));
							//FATAL("Router" << id << "[" << DIR::toString(node->conToDir[dirIn]) << dirIn<<"]\t- Failed Selection! " << *flit);
							LOG(true,
									"Router" << id << "[" << DIR::toString(node->conToDir[dirIn]) << vcIn<<"]\t- Reroute Fail! " << *flit);

						} else {
							LOG(true,
									"Router" << id << "[" << DIR::toString(node->conToDir[dirIn]) << vcIn<<"]\t- Reroute Success! " << *flit);
							continue;
						}

					} else if (rpi->delayFlag) {
						rpi->delayFlag = false;
						FATAL(
								"Router" << id << "[" << DIR::toString(node->conToDir[dirIn]) << dirIn<<"]\t- Delay! " << *flit);
						LOG(global.verbose_router_throttle,
								"Router" << id << "[" << DIR::toString(node->conToDir[dirIn]) << dirIn<<"]\t- Delay! " << *flit);
						continue;
					}

				}
				if (rpi->dropFlag) {

					if (flit->type == TAIL) {
						rep.reportEvent(dbid, "pkg_dropped",
								std::to_string(flit->packet->dbid));
						routedPackets.erase( { flit->packet, c });
						rpInfo.erase( { flit->packet, c });
						pkgcnt.at(dirIn)->at(vcIn)--;delete
flit						->packet;
						delete rpi;
						global.droppedCounter++;
					}

					routedFlits.at(dirIn)->at(vcIn) = 0;
					//FATAL("Router" << id << "[" << DIR::toString(node->conToDir[dirIn]) << dirIn<<"]\t- Drop! " << *flit);

					continue;
				}

				if (rpi->outputChannel.dir == -1) {
					rep.reportEvent(dbid, "router_routefail",
							std::to_string(flit->id));
					FATAL(
							"Router" << id << "[" << DIR::toString(node->conToDir[dirIn]) << dirIn<<"]\t- Failed Decision! " << *flit);
				}

				Channel out = rpi->outputChannel;

				if (!classicPortContainer.at(out.dir).portFlowControlIn.read()->at(
						out.vc)) {
					LOG(global.verbose_router_throttle,
							"Router" << id << "[" << DIR::toString(node->conToDir[dirIn]) << dirIn<<"]\t- Flow Control! " << *flit);
					//rep.reportEvent(dbid, "router_flow", std::to_string(flit->id));
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
					if (flit->type == HEAD && !rInfo->occupyTable.count(out)) {
						rInfo->occupyTable[out] = flit->packet;
						LOG(global.verbose_router_assign_channel,
								"Router" << id << "[" << DIR::toString(node->conToDir[out.dir]) << out.vc<<"]\t- Assign to " << *flit);
						//rep.reportEvent(dbid, "router_assigned", std::to_string(flit->id));
					}

					if (rInfo->occupyTable.at(out) == flit->packet) {
						arbitratedFlits.insert( { flit, c });
						arbiterDirs.insert(out.dir);
						routedFlits.at(dirIn)->at(vcIn) = 0;
					}
				}

			}
		}
	}
	rrDirOff = (rrDirOff + 1) % buffer.size();
	crossbarcount = arbiterDirs.size();

}

void RouterVC::send() {
	for (std::pair<Flit*, Channel> p : arbitratedFlits) {
		Flit* flit = p.first;
		RoutingPacketInformation* rpi = rpInfo.at( { flit->packet, p.second });

		Channel out = rpi->outputChannel;
		Channel in = rpi->inputChannel;

		classicPortContainer.at(out.dir).portValidOut.write(true);
		classicPortContainer.at(out.dir).portDataOut.write(flit);
		classicPortContainer.at(out.dir).portVcOut.write(out.vc);

		if (flit->type == TAIL) {
			routedPackets.erase( { flit->packet, p.second });
			rInfo->occupyTable.erase(rpi->outputChannel);
			rpInfo.erase( { flit->packet, p.second });
			pkgcnt.at(in.dir)->at(in.vc)--;delete
rpi			;
		}

		LOG(
				(global.verbose_router_send_head_flit && flit->type == HEAD)
						|| global.verbose_router_send_flit,
				"Router" << id << "[" << DIR::toString(node->conToDir[out.dir]) << out.vc<<"]\t- send flit " << *flit);
		//rep.reportEvent(dbid, "router_send_flit", std::to_string(flit->id));
	}
	arbitratedFlits.clear();

}

