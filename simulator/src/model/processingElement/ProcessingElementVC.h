/*******************************************************************************
 * Copyright (C) 2018 joseph
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
#pragma once

#include "systemc.h"
#include <algorithm>
#include <queue>
#include <random>

#include "model/router/Router.h"
#include "model/traffic/Flit.h"
#include "model/traffic/TrafficPool.h"
#include "utils/GlobalResources.h"
#include "utils/Report.h"
#include "utils/Structures.h"
#include <model/container/PacketContainer.h>

#include "ProcessingElement.h"

class ProcessingElementVC: public ProcessingElement {
public:
	sc_event event;

	PacketPortContainer *packetPortContainer;

	std::map<DataType *, std::set<Task *>> neededFor;
	std::map<std::pair<Task *, DataType *>, int> neededAmount;
	std::map<Task *, std::set<DataType *>> needs;
	std::map<DataType *, int> receivedData;

	std::map<DataDestination *, Task *> destToTask;
	std::map<Task *, std::set<DataDestination *>> taskToDest;

	std::map<Task *, int> taskRepeatLeft;
	std::map<Task *, int> taskStartTime;
	std::map<Task *, int> taskTerminationTime;

	std::map<DataDestination *, int> countLeft;

	std::map<DataDestination *, int> destWait;

	SC_HAS_PROCESS(ProcessingElementVC);
	ProcessingElementVC(sc_module_name mn, Node& node, TrafficPool* tp);
	~ProcessingElementVC();

	void initialize();
	void bind(Connection *, SignalContainer *, SignalContainer *);
	void execute(Task *);
	void receive();
	void thread();

	void startSending(Task *);
	void checkNeed();
};
