/*******************************************************************************
 * Copyright (C) 2018 Jan Moritz Joseph
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 ******************************************************************************/
#ifndef SRC_NETRACEPOOL_H_
#define SRC_NETRACEPOOL_H_

#include "systemc.h"
#include "netrace.hh"
#include "queue.hh"
#include "NetracePacket.h"

#include "../TrafficPool.h"
#include "../../utils/Structures.h"
#include "../../utils/GlobalInputClass.h"
#include "../../utils/Report.h"


#define L2_LATENCY 5;



typedef struct queue_node queue_node_t;
struct queue_node {
	nt_packet_t* packet;
	unsigned long long int cycle;
};

class NetracePool: public TrafficPool{

private:
	Netrace* netrace;
	unsigned long long int cycle = 0;
	unsigned long long int startCycle = 0;
	nt_header_t* header;
	int x_nodes, y_nodes;

	nt_packet_t* packet = NULL;

	int packets_left=0;

	queue_t** waiting;
	queue_t** inject;
	queue_t** traverse;
	nt_packet_t* trace_packet = NULL;


	const int NTtypeEncoding[31] = {-1,8,72,72,72,8,72,0,0,0,0,0,0,8,8,8,72,0,0,0,0,0,0,0,0,8,0,8,8,8,72};

public:
	NetracePool(sc_module_name, std::vector<ProcessingElementVC3D*>*);
	virtual ~NetracePool();

	void netracePoolProcess();
	void send_thread(nt_packet_t* packet);
	void receive(Packet* p);
	void send(Packet* p);

	SC_HAS_PROCESS(NetracePool);

};

#endif /* SRC_NETRACEPOOL_H_ */
