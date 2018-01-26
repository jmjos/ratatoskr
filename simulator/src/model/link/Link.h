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
#ifndef SRC_MODEL_LINK_LINK_H_
#define SRC_MODEL_LINK_LINK_H_


#include "systemc.h"

#include "../../traffic/Flit.h"
#include "../container/ClassicContainer.h"
#include "../../utils/GlobalInputClass.h"
#include "../../utils/GlobalReportClass.h"
#include <iostream>
#include <fstream>


enum LinkType{
	HORIZONTAL=10, VERTICAL = 11
};

class Link: public sc_module{
	GlobalInputClass& global = GlobalInputClass::getInstance();
	GlobalReportClass& report = GlobalReportClass::getInstance();
	int id, globalId;
	Flit* previousFlit;
	Flit* currentFlit;
	int previousTransmissionState = 0;
	int currentTransmissionState = 0;
	LinkType linkType;
	int linkWidth;
	int linkDepth;
	float Cl = 0.0f; // effective Capacity

	// horizontal link:
	float Vdd;
	float length;
	float Cc; // capacitance to ground
	float Cg, Cglw; // self capacitance
	float kappaW;// bus ratio

	// vertical link:
	float C0, Ce, Cn, Cd;
	int Ne = 2*(linkWidth + linkDepth -2);
	int Nn = 2*linkWidth*linkDepth - 3*linkWidth - 3*linkDepth -4;
	int Nd = 2*(linkWidth -1)*(linkDepth -1);
	float kappaT = ((float)Ne*Ce + (float)Nn*Cn + (float)Nd*Cd) / ((float)Ne*C0); // bus ratio for vertical link

	//float Cl;
	//float Eload = (Vdd*Vdd*Cl*linkWidth*currentFlit.as);

	// UNCOMMENT FOR RAW DATA ON LINK (@Lennart)
	//ofstream *rawDataOutput;
  

public:
	sc_in<bool> clk;
	ClassicPortContainer* classicPortContainer;

	SC_HAS_PROCESS(Link);
	Link(sc_module_name nm, Connection* c, int globalId);
	~Link();

	void bind(SignalContainer* sigContIn, SignalContainer* sigContOut);
	void bindOpen(SignalContainer* sigContIn);
	void passthrough_thread();
	float calculateEnergyPackets(Flit* previousFlit, Flit* currentFlit);
	float calculateEnergyStatistics(Flit* currentFlit);

};


#endif /* SRC_MODEL_LINK_LINK_H_ */
