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

Link::Link(sc_module_name nm, Connection* c, int globalId){
	this->id = c->id;
	this->globalId = globalId;
	classicPortContainer = new ClassicPortContainer(
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

	this->Cl = c->effectiveCapacityCl;

	//technology constants
	this->linkDepth = c->linkDepth;
	this->linkWidth = c->linkWidth;
	this->Vdd = global.Vdd;
	this->length = c->length;
	this->Cc = c->wireCouplingCapacitanceCc; // capacitance to ground
	this->Cg = c->wireSelfCapacitanceCg; // self capacitance
	this->Cglw = c->wireSelfCapacitancePerUnitLengthCg;
	this->kappaW = (this->Cc / this->Cg * (1 - (1 / this->linkWidth))); // bus ratio

	// vertical link:
	this->C0 = c->tsvarraySelfCapacitanceC0;
	this->Ce = c->tsvarrayEdgeCapacitanceCe;
	this->Cn = c->tsvarrayDiagonalCapacitanceCn;
	this->Cd = c->tsvarrayNeighbourCapacitanceCd;
	this->Ne = 2 * (linkWidth + linkDepth - 2);
	this->Nn = 2 * linkWidth * linkDepth - 3 * linkWidth - 3 * linkDepth - 4;
	this->Nd = 2 * (linkWidth - 1) * (linkDepth - 1);
	this->kappaT = ((float) Ne * Ce + (float) Nn * Cn + (float) Nd * Cd)
			/ ((float) Ne * C0); // bus ratio for vertical link

	SC_METHOD(passthrough_thread);
	sensitive << clk.pos();

	return;
}
Link::~Link() {
	// UNCOMMENT FOR RAW DATA ON LINK (@Lennart)
	//rawDataOutput->close();
}

void Link::passthrough_thread() {
	// Energy Calculations
//	if (classicPortContainer->portValidIn) {
//		if (previousFlit == NULL) {
//			previousFlit = classicPortContainer->portDataIn;
//		} else {
//			currentFlit = classicPortContainer->portDataIn;
//			//report result of:
//			calculateEnergyPackets(previousFlit, currentFlit);
//			//report result of:
//			calculateEnergyStatistics(currentFlit);
//			previousFlit = currentFlit;
//			// TODO report link energy
//		}
//	}

	// UNCOMMENT FOR RAW DATA ON LINK (@Lennart)
	//std::string outputToFile;

	if (!classicPortContainer->portValidIn) {
		currentTransmissionState = 0;
		//outputToFile = "_;";
	} else {
		currentFlit = classicPortContainer->portDataIn;
		if (currentFlit->type == HEAD) {
			//outputToFile = "H;";
			currentTransmissionState = 1;
		} else {
			int offset = 2; // two fields: idle and head
			//outputToFile = std::to_string(currentFlit->trafficTypeId) + ";";
			currentTransmissionState = currentFlit->trafficTypeId + offset;
		}
	}

	// UNCOMMENT FOR RAW DATA ON LINK (@Lennart)
	//rawDataOutput->write(outputToFile.c_str(), 2);
	//rawDataOutput->flush();
	report.issueLinkMatrixUpdate(globalId, currentTransmissionState, previousTransmissionState);

	previousTransmissionState = currentTransmissionState;

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

float Link::calculateEnergyPackets(Flit* previousFlit, Flit* currentFlit) {
	return 0.0f;
}

//TODO implement energy model
float Link::calculateEnergyStatistics(Flit* currentFlit) {
	if (linkType == HORIZONTAL) {
		float Eload = (Vdd * Vdd * Cg * linkWidth * currentFlit->as) / 2;
		float E = (Vdd * Vdd * Cglw * linkWidth) / 2;
		E *= (kappaW * currentFlit ->ac + currentFlit->as);
		E += Eload;
	} else {
		// E = 1/2 Vdd^2 C0 Ne (kw Tc + Ts) + Eload
		float Eload = 0.0f;
		float E = (Vdd * Vdd * C0 * Ne) / 2;
		E *= (kappaT * currentFlit->ac + currentFlit->as);
		E += Eload;
	}
	return 0.0f;
}
