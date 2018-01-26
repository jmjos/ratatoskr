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
#include "NetracePool.h"
#include "../../model/processingElements/ProcessingElementVC3D.h"


NetracePool::NetracePool(sc_module_name name,  std::vector<ProcessingElementVC3D*>* pe) : TrafficPool(name, pe) {
	cout << endl;
	cout << "The following benchmark is running" << endl;
	cout << "            " << global.netraceFile << endl;

	netrace = new Netrace();

	//initialize netrace
	int ignoreDependencies = 0;
	const char* tracefile = global.netraceFile.c_str();

	netrace->nt_open_trfile(tracefile);
	if (ignoreDependencies) {
		netrace->nt_disable_dependencies();
	}
	netrace->nt_print_trheader();
	header = netrace->nt_get_trheader();
	int start_region = global.netraceStartRegion;
	netrace->nt_seek_region(&header->regions[start_region]);
	for (int i = 0; i < start_region; i++) {
		startCycle += header->regions[i].num_cycles;
	}
	cycle += startCycle;

	x_nodes = sqrt(header->num_nodes);
	y_nodes = header->num_nodes / x_nodes;
	waiting = (queue_t**) malloc(header->num_nodes * sizeof(queue_t*));
	inject = (queue_t**) malloc(header->num_nodes * sizeof(queue_t*));
	traverse = (queue_t**) malloc(header->num_nodes * sizeof(queue_t*));
	if ((waiting == NULL) || (inject == NULL) || (traverse == NULL)) {
		printf("malloc fail queues\n");
		exit(0);
	}

	for (int i = 0; i < header->num_nodes; ++i) {
		waiting[i] = queue_new();
		inject[i] = queue_new();
		traverse[i] = queue_new();
	}

	//initialiyse systemc module
	SC_THREAD(netracePoolProcess);
}

NetracePool::~NetracePool(){

};

void NetracePool::netracePoolProcess() {
	cout << "starting NetracePoolProcess" << endl;
	trace_packet = netrace->nt_read_packet();
	netrace->nt_print_packet(trace_packet);
	while (true) {
		wait(1, SC_NS);
		// Reset packets remaining check
		packets_left = 0;


		while ((trace_packet != NULL) && (trace_packet->cycle == cycle)) {
			// Place in appropriate queue
			queue_node_t* new_node = new queue_node_t();
			new_node->packet = trace_packet;
			new_node->cycle = (trace_packet->cycle > cycle) ? trace_packet->cycle : cycle;
			if (netrace->nt_dependencies_cleared(trace_packet)) {
				// Add to inject queue
				queue_push(inject[trace_packet->src], new_node, new_node->cycle);
			} else {
				// Add to waiting queue
				queue_push(waiting[trace_packet->src], new_node, new_node->cycle);
			}
			// Get another packet from trace
			trace_packet = netrace->nt_read_packet();
		}

		// Inject where possible (max one per node)
		for (int i = 0; i < header->num_nodes; ++i) {
			packets_left |= !queue_empty(inject[i]);
			queue_node_t* temp_node = (queue_node_t*) queue_peek_front(inject[i]);
			if (temp_node != NULL) {
				packet = temp_node->packet;
				if ((packet != NULL) && (temp_node->cycle <= cycle)) {
					if (global.verbose_netrace_inject) {
						printf("Inject: %llu ", cycle);
						cout << " @ " << sc_time_stamp();
						netrace->nt_print_packet(packet);
					}
					temp_node = (queue_node_t*) queue_pop_front(inject[i]);

					send_thread(temp_node->packet);

				}
			}
		}

		for (int i = 0; i < header->num_nodes; ++i) {
			packets_left |= !queue_empty(waiting[i]);
			node_t* temp = waiting[i]->head;
			while (temp != NULL) {
				queue_node_t* temp_node = (queue_node_t*) temp->elem;
				packet = temp_node->packet;
				temp = temp->next;
				if (netrace->nt_dependencies_cleared(packet)) {
					// remove from waiting
					queue_remove(waiting[i], temp_node);
					// add to inject
					queue_node_t* new_node = (queue_node_t*) netrace->nt_checked_malloc( sizeof(queue_node_t) );
					new_node->packet = packet;
					new_node->cycle = cycle + L2_LATENCY
					;
					cout << "dep cleared: ";
					netrace->nt_print_packet(packet);
					queue_push(inject[i], new_node, new_node->cycle);
					free(temp_node);
				}
			}
		}

		cycle++;
	}
}

void NetracePool::send_thread(nt_packet_t* ntp) {
	NetracePacket* p = new NetracePacket(ntp->src, ntp->dst, NTtypeEncoding[ntp->type], sc_time_stamp().to_double(), ntp);

	cout << "in send thead:" << endl;
	netrace->nt_print_packet(ntp);
	send(p);
}

void NetracePool::send(Packet* p) {
	pe->at(p->src)->receive(p);
}

void NetracePool::receive(Packet* p) {
	NetracePacket* np = (NetracePacket*)p;

	//cout << "in recieve:";
	//netrace->nt_print_packet(p.netrace_packet);
	//cout << "the time is" << sc_time_stamp().to_double()<< endl;
	//double latency = fabs(np->generationTime - sc_time_stamp().to_double());
	//cout << "the latncy is " << latency << endl;
	//globalReportClass.updateAverageNetworkLatencySystemLevel(latency);
	netrace->nt_clear_dependencies_free_packet(np->netracePacket);

	delete p;
}

