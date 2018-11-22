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
#ifndef SRC_MODEL_ROUTER_ROUTER_H_
#define SRC_MODEL_ROUTER_ROUTER_H_

#include <set>
#include "systemc.h"
#include "model/NetworkParticipant.h"
#include "utils/Structures.h"

class Router : public NetworkParticipant, public sc_module{
public:
	int id;
	int dbid;
	Node* node;


	SC_HAS_PROCESS(Router);
	Router(sc_module_name nm, Node* node);
	virtual ~Router();

	virtual void initialize() = 0;
	virtual void bind(Connection*, SignalContainer*, SignalContainer*) = 0;
	virtual void thread() = 0;
	virtual void receive() = 0;
	virtual void updateUsageStats() = 0;

};

#endif /* SRC_MODEL_ROUTER_ROUTER_H_ */
