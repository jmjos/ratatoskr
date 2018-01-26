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
//-----------------------------------------------------------------------------
//    AdaptNoCSimulator. Simulator for Network on Chip with Support for Task Graph Modelling.
//    Copyright (C) 2014-2014 Jan Moritz Joseph (joseph(at)jmjoseph.de), Universitaet zu Luebeck
//
//    This program is free software: you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation, either version 3 of the License, or
//    (at your option) any later version.
//
//    This program is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with this program.  If not, see <http://www.gnu.org/licenses/>.
//-----------------------------------------------------------------------------

#include "ProcessingElementVC3D.h"
#include "time.h"

ProcessingElementVC3D::ProcessingElementVC3D(sc_module_name mn, Node* node){
		this->id = global.getPeById(node->id);
		this->dbid = rep.registerElement("ProcessingElement", id);
		this->node = node;
		this->flowControlOut = new std::vector<bool>(1, true);
		this->tagOut = new std::vector<int>(1, 0);
		this->classicPortContainer = new ClassicPortContainer(("PE_CONTAINER"+std::to_string(id)).c_str());

		SC_METHOD(send_data_process);
		sensitive << clk.pos();// << reset;

		SC_METHOD(recieve_data_process);
		sensitive << clk_router.pos();// << reset;
	}


void ProcessingElementVC3D::send_data_process(void) {
	LOG(global.verbose_pe_function_calls, "PE" << id <<"(Router"<<node->id<< ")\t- send_data_process()");


	std::vector<bool>* flowControlIn = classicPortContainer->portFlowControlIn.read();
	bool flow_in=true;

	if(flowControlIn){
		flow_in = flowControlIn->at(vc);
	}

	if (!packet_queue.empty() && !flow_in) {
		LOG(global.verbose_pe_throttle, "PE" << id <<"(Router"<<node->id<< ")\t- Waiting for Router!");
	}

	if (!packet_queue.empty() && flow_in) {
		//generate flit and send it.
		//cout << "generating flit" << endl;
		Packet* p = packet_queue.front();

		Flit* current_flit = p->toTransmit.back();
		p->toTransmit.pop_back();
		p->inTransmit.push_back(current_flit);

		rep.reportEvent(dbid, "pe_send_flit", std::to_string(current_flit->id));

		if (p->toTransmit.empty()) {
			packet_queue.pop();
		}

		classicPortContainer->portValidOut.write(true);
		classicPortContainer->portDataOut.write(current_flit);
		classicPortContainer->portVcOut.write(vc);


		LOG((global.verbose_pe_send_head_flit && current_flit->type == HEAD) || global.verbose_pe_send_flit,
				"PE" << id <<"(Router"<<node->id<< ")\t- Send Flit " << *current_flit);

	} else {
		classicPortContainer->portValidOut.write(false);
	}

}

void ProcessingElementVC3D::recieve_data_process(void) {
	LOG(global.verbose_pe_function_calls, "PE" << id <<"(Router"<<node->id<< ")\t- receive_data_process()");


	//to debug: simply print the received flit
	if (classicPortContainer->portValidIn.read()) {

		//if no flit was send: ABP or with NULL-check?
		Flit* received_flit = classicPortContainer->portDataIn.read();
		Packet* p = received_flit->packet;

		std::vector<Flit*>::iterator position = std::find(p->inTransmit.begin(), p->inTransmit.end(), received_flit);
		if (position != p->inTransmit.end())
			p->inTransmit.erase(position);

		p->transmitted.push_back(received_flit);
		rep.reportEvent(dbid, "pe_receive_flit", std::to_string(received_flit->id));


		LOG((global.verbose_pe_receive_tail_flit && received_flit->type == TAIL) || global.verbose_pe_receive_flit,
				"PE" << id <<"(Router"<<node->id<< ")\t- Receive Flit " << *received_flit);

		LOG(received_flit->type == TAIL && (!p->toTransmit.empty() || !p->inTransmit.empty()),
				"PE" << id <<"(Router"<<node->id<< ")\t- Reiceived Tail Flit, but still missing flits! " << *received_flit);

				//if a complete packet is recieved, notify tasks
		if (p->toTransmit.empty() && p->inTransmit.empty()) {
			NetracePacket* np = dynamic_cast<NetracePacket*>(p);

			if (np && np->dst != id) {
				cout << "RECIEVED WRONG FLIT" << endl;
			}
			if (np && global.verbose_netrace_router_receive) {
				cout << "PE " << id << " recieved a packet @ " << sc_time_stamp() << ":";
				Netrace netrace;
				netrace.nt_print_packet(np->netracePacket);
			}
			trafficPool->receive(p);
		}
	}
	// build packet from flit, send packet to application (via TLM?)
}

//implement interface function
void ProcessingElementVC3D::receive(Packet* p) {
	LOG(global.verbose_pe_function_calls, "PE" << id <<"(Router"<<node->id<< ")t- receive()");

	p->toTransmit.resize(global.flitsPerPacket);

	for (int i = 0; i < global.flitsPerPacket ; i++) {
		FlitType type;
		if (i % global.flitsPerPacket == 0) {
			type = FlitType::HEAD;
		} else if (i % global.flitsPerPacket == global.flitsPerPacket - 1) {
			type = FlitType::TAIL;
		} else {
			type = FlitType::BODY;
		}
		Flit* current_flit = new Flit(type, i % global.flitsPerPacket, p, p->ac, p->as, p->trafficTypeId);
		p->toTransmit.at(global.flitsPerPacket-i-1) = current_flit;
	}
	packet_queue.push(p);
}
