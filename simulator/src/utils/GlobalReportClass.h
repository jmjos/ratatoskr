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
#pragma once

#include "systemc.h"
#include <map>
#include <boost/format.hpp>

#include "GlobalInputClass.h"

class GlobalReportClass {
	GlobalInputClass &global = GlobalInputClass::getInstance();
private:
    // s = STATIC (Watt)
    // d = DYNAMIC (Joule)

    double total_power_s; 				//unused

    //Buffer Consumption (power per event is determined by a LUT of buffer depth and flit size)
    double buffer_router_push_pwr_d;	// per flit pushed on buffer
    double buffer_router_pop_pwr_d;		// per flit popped of buffer (not for lookup)
    double buffer_router_front_pwr_d;	// per flit data received from buffer (only count if flit exists)
    double buffer_router_pwr_s;			// leakage per router cycle per buffer

    //Power Consumption of routing Algorithm (determined by routing algorithm)
    double routing_pwr_d; 				//per routing function called
    double routing_pwr_s; 				//Leakage per Router Cycle


    //Routing function gives vector of possible directions/VCs
    //Selection function selects direction/VC
    //Power Consumption of selection Function (determined by selection function)
    //double selection_pwr_d;				//per selection function called
    //double selection_pwr_s; 			//Leakage per Router Cycle
    // not implemented!


    //Power Consumption of Crossbar (determined by IOs (5) and Flit size)
    double crossbar_pwr_d; 				// per sent flit
    double crossbar_pwr_s;				// Leakage per Router Cycle

    //Power consumtion of data links (determined by router to router distance)
    double link_r2r_pwr_d;				// per sent flit
    double link_r2r_pwr_s;				// unused

    //Power Consumtion of Network Interface (determined by flit size)
    double ni_pwr_d;					// per local flit sent or received
    double ni_pwr_s;					// Leakage per Router Cycle

    GlobalReportClass();
public:
	//Link state transmission matrixes
	std::map<int, std::vector<long> > linkTransmissionMatrices;
	int linkTransmissionsMatrixNumberOfStates;

	//Router state vectors and matrices
	std::map<int, int> routingCalulcations;

    //number of resets
	std::map<int, int> numberOfResets;
	std::map<int, int> numberOfAccelerations;

	double averageNetworkLatencySystemLevel = -1;
	int averageNetworkLatencySystemLevelInstances = 0;
	double maxNetworkLatency = 0;


	static GlobalReportClass& getInstance() {
		static GlobalReportClass instance;
		return instance;
	}

	void makeReport(std::string filename);

	//old: can be deleted
	void issueReset(int id);
	void reportReset();
	void reportResetTotal();
	void issueAcceleration(int id);
	void reportAccelerations();
	void reportAccelerationsTotal();

	// Link state transmission matrixes
	void issueLinkMatrixUpdate(int id, int currentTransmissionState, int lastTransmissionState);
	void reportLinkMatrix(int id, ostream& stream);
	void reportLinkMatrices(ostream& stream);
	void reportLinkMatricesCVS(ostream& stream);

	// Router status vectors and matrices
	void issueRoutingCalculation(int id);
	void reportRoutingCalculations(ostream& stream);


	void updateAverageNetworkLatencySystemLevel(double newLatency);
	void reportAverageNetworkLatencySystemLevel();
	void updateMaxNetworkLatencySystemLevel(double newLatency);
	void reportMaxNetworkLatencySystemLevel();
};

