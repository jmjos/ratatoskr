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
#ifndef SRC_TRAFFIC_TRAFFICPOOL_H_
#define SRC_TRAFFIC_TRAFFICPOOL_H_


#include "../utils/Structures.h"
#include "../utils/GlobalInputClass.h"
#include "../utils/Report.h"
#include "Packet.h"

class ProcessingElementVC3D;

class TrafficPool : public sc_module{
public:
	GlobalInputClass& global = GlobalInputClass::getInstance();
	Report& rep = Report::getInstance();

	int dbid;


	std::vector<ProcessingElementVC3D*>* pe;



	TrafficPool(sc_module_name, std::vector<ProcessingElementVC3D*>*);
	virtual ~TrafficPool();
	virtual void receive(Packet*) = 0;
	virtual void send(Packet*) = 0;

	SC_HAS_PROCESS(TrafficPool);

};




#endif /* SRC_TRAFFIC_TRAFFICPOOL_H_ */
