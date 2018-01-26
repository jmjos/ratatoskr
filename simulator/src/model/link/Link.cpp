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
#include "Link.h"

Link::Link(sc_module_name nm, Connection* c, int globalId) {
	this->id = c->id;
	this->globalId = globalId;
	classicPortContainer = new FlitPortContainer(
			("link_portCont_" + std::to_string(id)).c_str());
	if (c->nodes.size() == 1) {
		this->linkType = HORIZONTAL;
	} else {
		if (c->nodes.at(0)->pos.z == c->nodes.at(1)->pos.z) {
			this->linkType = HORIZONTAL;
		} else {
			this->linkType = VERTICAL;
		}
	}
	this->currentFlit = NULL;
	this->previousFlit = NULL;

	// UNCOMMENT FOR RAW DATA ON LINK (@Lennart)
	//if(id == 4){
	//	cout << "Check file wiht name " << nm << endl;
	//}
	//this->rawDataOutput = new ofstream((std::string) nm + ".txt");

	SC_METHOD(passthrough_thread);
	sensitive << clk.pos();

	return;
}
Link::~Link() {
	// UNCOMMENT FOR RAW DATA ON LINK (@Lennart)
	//rawDataOutput->close();
}

void Link::passthrough_thread() {
	// UNCOMMENT FOR RAW DATA ON LINK (@Lennart)
	//std::string outputToFile;

	int IDLESTATE = 0;
	int HEADSTATE = 1;
	int HEADIDLESTATE = 2;
	int offset = 3; // three fields: idle, head, headidle


	if (!classicPortContainer->portValidIn) {
		// this cycle idle
		if (previousTransmissionState == IDLESTATE){
			// initally, no flits traverse link
			//outputToFile = "__;"
			currentTransmissionState = IDLESTATE;
		} else if (currentFlit->type == HEAD){
			// a head flit traversed previously
			//outputToFile = std::to_string(currentFlit->trafficTypeId) + "_;";
			currentTransmissionState = HEADIDLESTATE;
		} else {
			// a flit already traversed the link
			//outputToFile = std::to_string(currentFlit->trafficTypeId) + "_;";
			currentTransmissionState = (2 * currentFlit->trafficTypeId) + offset + 1;
		}
	} else {
		// this cycle active
		currentFlit = classicPortContainer->portDataIn;
		if (currentFlit->type == HEAD) {
			//received head flit
			//outputToFile = "HD;";
			currentTransmissionState = HEADSTATE;
		} else {
			// received data flit
			//outputToFile = std::to_string(currentFlit->trafficTypeId) + "D;";
			currentTransmissionState = (2 * currentFlit->trafficTypeId) + offset;
		}
	}


	// UNCOMMENT FOR RAW DATA ON LINK (@Lennart)
	//rawDataOutput->write(outputToFile.c_str(), 2);
	//rawDataOutput->flush();
	report.issueLinkMatrixUpdate(globalId, currentTransmissionState,
			previousTransmissionState);

	previousTransmissionState = currentTransmissionState;
	previousFlit = currentFlit;

	classicPortContainer->portValidOut = classicPortContainer->portValidIn;
	classicPortContainer->portFlowControlOut =
			classicPortContainer->portFlowControlIn;
	classicPortContainer->portTagOut = classicPortContainer->portTagIn;
	classicPortContainer->portDataOut = classicPortContainer->portDataIn;
	classicPortContainer->portValidOut = classicPortContainer->portValidIn;
	classicPortContainer->portResetOut = classicPortContainer->portResetIn;
}

void Link::bind(SignalContainer* sigContIn, SignalContainer* sigContOut) {
	classicPortContainer->bind(sigContIn, sigContOut);
}

void Link::bindOpen(SignalContainer* sigContIn) {
	classicPortContainer->bindOpen(sigContIn);
}

