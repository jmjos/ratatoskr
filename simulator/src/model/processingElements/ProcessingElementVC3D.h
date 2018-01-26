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
#ifndef _PROCESSINGELEMENTVC3D_H_
#define _PROCESSINGELEMENTVC3D_H_

#include "systemc.h"
#include <queue>
#include <algorithm>

#include "../../traffic/TrafficPool.h"
#include "../container/ClassicContainer.h"
#include "../../utils/Structures.h"
#include "../../traffic/Flit.h"
#include "../../utils/GlobalInputClass.h"
#include "../../traffic/netrace/NetracePacket.h"
#include "../../utils/Report.h"

class ProcessingElementVC3D : public sc_module{
public:
	GlobalInputClass& global = GlobalInputClass::getInstance();
	Report& rep = Report::getInstance();

	int id;
	int dbid;
	int vc=0;
	Node* node;
	TrafficPool* trafficPool = 0;
	std::queue < Packet* > packet_queue;
	std::vector<bool>* flowControlOut;
	std::vector<int>* tagOut;

	sc_in < bool > clk;
	sc_in < bool > clk_router;
	ClassicPortContainer* classicPortContainer;


	//Functions
	void send_data_process(void);
	void recieve_data_process(void);
	void receive(Packet*);


	SC_HAS_PROCESS(ProcessingElementVC3D);
	ProcessingElementVC3D(sc_module_name mn, Node* node);

};

#endif
