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
#include <boost/algorithm/string.hpp>

#include "GlobalResources.h"
#include "Statistics.h"

class GlobalReport {
public:
    int droppedCounter = 0;             // number of dropped flits.

    Statistics latencyNetwork;
    Statistics latencyFlit;
    Statistics latencyPacket;

    //Link state transmission matrixes
    std::map<int, std::vector<long> > linkTransmissionMatrices;
    int linkTransmissionsMatrixNumberOfStates;

    //Router state vectors and matrices
    std::map<int, int> routingCalculations;

    double averageNetworkLatencySystemLevel = -1;
    int averageNetworkLatencySystemLevelInstances = 0;
    double maxNetworkLatency = 0;

    int numRouters;
    /* 1st dimension is all routers.
     * 2nd dimension is all ports (directions) of a router.
     * 3rd dimension is all possible number of active VCs.
     */
    std::vector<std::vector<std::vector<long>>> VCsUsageHist;
    /* 1st dimension is all routers.
     * 2nd dimension is all ports (directions) of a router.
     * 3rd dimension is all possible number of VCs.
     * 4th dimension is all buffer positions of a VC.
     */
    std::vector<std::vector<std::vector<std::vector<long>>>> bufferUsagePerVCHist;

    void readConfigFile(const std::string& config_path);

    /// VERBOSE ///
    //processing elements
    bool verbose_pe_function_calls = false;
    bool verbose_pe_send_flit = false;
    bool verbose_pe_send_head_flit = false;
    bool verbose_pe_receive_flit = false;
    bool verbose_pe_receive_tail_flit = false;
    bool verbose_pe_throttle = false;
    bool verbose_pe_reset = false;

    //router
    bool verbose_router_function_calls = false;
    bool verbose_router_send_flit = false;
    bool verbose_router_send_head_flit = false;
    bool verbose_router_receive_flit = false;
    bool verbose_router_receive_head_flit = false;
    bool verbose_router_assign_channel = false;
    bool verbose_router_throttle = false;
    bool verbose_router_buffer_overflow = false;
    bool verbose_router_reset = false;

    //netrace
    bool verbose_netrace_inject = false;
    bool verbose_netrace_eject = false;
    bool verbose_netrace_router_receive = false;

    //tasks
    bool verbose_task_function_calls = false;
    bool verbose_task_xml_parse = false;
    bool verbose_task_data_receive = true;
    bool verbose_task_data_send = true;
    bool verbose_task_source_execute = true;

    static GlobalReport& getInstance()
    {
        static GlobalReport instance;
        return instance;
    }

    void reportComplete(const std::string& filename);

    void reportPerformance(ostream& stream);

    void reportPerformanceCSV(ostream& stream);

    // Link state transmission matrixes
    void issueLinkMatrixUpdate(int id, int currentTransmissionState, int lastTransmissionState);

    void reportLinkMatrix(int id, ostream& stream);

    void reportLinkMatrices(ostream& stream);

    void reportLinkMatricesCSV(ostream& stream);

    // Router status vectors and matrices
    void issueRoutingCalculation(int id);

    void reportRoutingCalculations(ostream& stream);

    void updateAverageNetworkLatencySystemLevel(double newLatency);

    void reportAverageNetworkLatencySystemLevel();

    void updateMaxNetworkLatencySystemLevel(double newLatency);

    void reportMaxNetworkLatencySystemLevel();

    void updateVCUsageHist(int routerId, int dir, int numOfActiveVCs);

    void reportVCUsageHist(std::string& csvFileName, int routerId);

    void updateBuffUsagePerVCHist(int routerId, int dir, int vc, int bufferOccupation);

    void reportBuffPerVCUsageHist(std::string& csvFileName, int routerId, int dir);

    void reportAllRoutersUsageHist();

    void resizeMatrices();

    void increaseClockCount(int layer);

    void reportClockCount(ostream& stream);

    void increaseBufferPush(int router_id);

    void increaseBufferPop(int router_id);

    void increaseBufferFront(int router_id);

    void increaseRouting(int router_id);

    void increaseCrossbar(int router_id);

    void reportRoutersPowerCSV(ostream& csvfile);

    const std::vector<int> INNER_ROUTERS = {5, 6, 9, 10, 21, 22, 25, 26, 37, 38, 41, 42};

private:
    GlobalResources& globalResources = GlobalResources::getInstance();

    double total_power_s;               //unused

    //Buffer consumption (power per event is determined by a LUT of buffer depth and flit size)
    std::vector<double> buffer_router_push_pwr_d;    // per flit pushed on buffer
    std::vector<double> buffer_router_pop_pwr_d;     // per flit popped of buffer (not for lookup)
    std::vector<double> buffer_router_front_pwr_d;   // per flit data received from buffer (only count if flit exists)
    std::vector<double> buffer_router_pwr_s;         // leakage per router cycle per buffer

    //Power consumption of routing Algorithm (determined by routing algorithm)
    std::vector<double> routing_pwr_d;                //per routing function called
    std::vector<double> routing_pwr_s;                //Leakage per Router Cycle

    //Routing function gives vector of possible directions/VCs
    //Selection function selects direction/VC
    //Power Consumption of selection Function (determined by selection function)
    //double selection_pwr_d;			//per selection function called
    //double selection_pwr_s; 			//Leakage per Router Cycle
    // not implemented!

    //Power consumption of Crossbar (determined by IOs (5) and Flit size)
    std::vector<double> crossbar_pwr_d;              // per sent flit
    std::vector<double> crossbar_pwr_s;              // Leakage per Router Cycle

    //Power consumption of Network Interface (determined by flit size)
    std::vector<double> ni_pwr_d;                    // per local flit sent or received
    std::vector<double> ni_pwr_s;                    // Leakage per Router Cycle

    // Used to create the buffer axes in the histogram of buffer usage
    const int MAX_BUFFER_DEPTH = 50;
    // Generate VC and buffer histograms for only these routers

    std::vector<long long> clockCounts;

    GlobalReport();
};

