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
#include "GlobalReport.h"
#include "Structures.h"
#include <cstdlib>

GlobalReport::GlobalReport()
        :
        total_power_s(0.0),
        latencyNetwork("network latency"),
        latencyFlit("flit latency"),
        latencyPacket("packet latency ")
{
}

void GlobalReport::reportComplete(const std::string& filename)
{
    ofstream myfile;
    myfile.open(filename+".txt");
    // Writing general simulation data:
    myfile << boost::format("Report file of this simulation run. \n\n");
    myfile << "----------------------------------------------------" << endl;
    myfile << "General Information:" << endl;
    myfile << "----------------------------------------------------" << endl;
    reportPerformance(myfile);
    reportClockCount(myfile);

    // Writing router routing calculations
    myfile << "----------------------------------------------------" << endl;
    myfile << "Router Matrixes:" << endl;
    myfile << "----------------------------------------------------" << endl;
    myfile << "Routing calculations:\n";
    reportRoutingCalculations(myfile);

    // Writing links matrixes
    myfile << "----------------------------------------------------" << endl;
    myfile << "Link Matrixes:" << endl;
    myfile << "----------------------------------------------------" << endl;
    reportLinkMatrices(myfile);
    myfile.close();

    // Generate csv for links matrices
    std::string csvFilename = filename+"_Links.csv";
    ofstream csvfile;
    csvfile.open(csvFilename);
    reportLinkMatricesCSV(csvfile);
    csvfile.close();

    // Generate csv for performance statistics
    csvFilename = filename+"_Performance.csv";
    csvfile.open(csvFilename);
    reportPerformanceCSV(csvfile);
    csvfile.close();

    // Generate csv for routers power statistics
    csvFilename = filename+"_Routers_Power.csv";
    csvfile.open(csvFilename);
    reportRoutersPowerCSV(csvfile);
    csvfile.close();

    reportAllRoutersUsageHist();
}

void GlobalReport::reportPerformance(ostream& stream)
{
    float avgFlitLat = latencyFlit.average()/(float) 1000;
    float avgPacketLat = latencyPacket.average()/(float) 1000;
    float avgNetworkLat = latencyNetwork.average()/(float) 1000;
    stream << boost::format("Average flit latency: %3.2f ns.\n")%avgFlitLat;
    stream << boost::format("Average packet latency: %3.2f ns.\n")%avgPacketLat;
    stream << boost::format("Average network latency: %3.2f ns.\n")%avgNetworkLat;
}

void GlobalReport::reportPerformanceCSV(ostream& stream)
{
    float avgFlitLat = latencyFlit.average()/(float) 1000;
    float avgPacketLat = latencyPacket.average()/(float) 1000;
    float avgNetworkLat = latencyNetwork.average()/(float) 1000;
    stream << boost::format("avgFlitLat, %2.3f\n")%avgFlitLat;
    stream << boost::format("avgPacketLat, %2.3f\n")%avgPacketLat;
    stream << boost::format("avgNetworkLat, %2.3f\n")%avgNetworkLat;
}

void GlobalReport::issueRoutingCalculation(int id)
{
    auto it = routingCalculations.find(id);
    if (it!=routingCalculations.end())
        ++it->second;
    else
        routingCalculations.insert(std::make_pair(id, 1));
}

void GlobalReport::reportRoutingCalculations(ostream& stream)
{
    for (auto it : routingCalculations) {
        stream << boost::format("\tRouter id: %i had %i calculations \n")%it.first%it.second;
    }
}

void GlobalReport::issueLinkMatrixUpdate(int id, int currentTransmissionState, int lastTransmissionState)
{
    ++linkTransmissionMatrices.at(id).at(
            currentTransmissionState+linkTransmissionsMatrixNumberOfStates*lastTransmissionState);
}

void GlobalReport::reportLinkMatrix(int id, ostream& stream)
{
    auto transmissionMatrix = linkTransmissionMatrices.at(id);
    int clockCyclesOfLink = std::accumulate(transmissionMatrix.begin(), transmissionMatrix.end(), 0);
    stream << boost::format("Transmission matrix of links %i:\n")%id;
    int colit = 0, rowit = 0;
    stream << boost::format("from\\to     IDLE     HEAD     HID");
    for (int var = 0; var<globalResources.numberOfTrafficTypes; ++var) {
        stream << boost::format("%7iD%6iID")%var%var;
    }
    stream << boost::format("\n");
    for (auto matrixelement : transmissionMatrix) {
        float field = (float) matrixelement/(float) clockCyclesOfLink;
        if (colit==0) {
            if (rowit==0) {
                stream << boost::format("IDLE\t");
            }
            else if (rowit==1) {
                stream << boost::format("HEAD\t");
            }
            else if (rowit==2) {
                stream << boost::format("HID\t");
            }
            else if (rowit%2==0) {
                int trafficType = (rowit-3)/2;
                stream << boost::format("%iID\t")%trafficType;
            }
            else if (rowit%2==1) {
                int trafficType = (rowit-3)/2;
                stream << boost::format("%iD\t")%trafficType;
            }
            ++rowit;
            stream << boost::format("[%7.4f, ")%field;
        }
        else if (colit==linkTransmissionsMatrixNumberOfStates-1) {
            stream << boost::format("%7.4f ]\n")%field;
        }
        else {
            stream << boost::format("%7.4f,")%field;
        }
        ++colit;
        if (colit==linkTransmissionsMatrixNumberOfStates) {
            colit = 0;
        }
    }
}

void GlobalReport::reportLinkMatrices(ostream& stream)
{
    auto numberOfLinks = linkTransmissionMatrices.size();
    for (unsigned int var = 0; var<numberOfLinks; ++var) {
        reportLinkMatrix(var, stream);
    }
}

void GlobalReport::reportLinkMatricesCSV(ostream& stream)
{
    int numberOfElements = static_cast<int>(pow(linkTransmissionsMatrixNumberOfStates, 2));
    stream << boost::format("link_id");
    for (int var = 0; var<numberOfElements-1; ++var) {
        stream << boost::format(", %i")%var;
    }
    stream << boost::format(", %i\n")%(numberOfElements-1);
    for (auto matrix : linkTransmissionMatrices) {
        stream << boost::format("%i, ")%matrix.first;
        for (auto it = matrix.second.begin(); it!=matrix.second.end(); ++it) {
            int value = static_cast<int>(matrix.second.at(it-matrix.second.begin()));
            if (std::next(it)==matrix.second.end()) {
                stream << boost::format("%i\n")%value;
            }
            else {
                stream << boost::format("%i, ")%value;
            }
        }
    }
}

void GlobalReport::updateAverageNetworkLatencySystemLevel(double newLatency)
{
    if (averageNetworkLatencySystemLevel==-1) {
        averageNetworkLatencySystemLevel = newLatency;
    }
    else {
        double scalingLarge = ((double) averageNetworkLatencySystemLevelInstances/
                ((double) averageNetworkLatencySystemLevelInstances+(double) 1));
        double first = scalingLarge*averageNetworkLatencySystemLevel;
        double scalingSmall = ((double) 1/((double) averageNetworkLatencySystemLevelInstances+(double) 1));
        double second = scalingSmall*newLatency;
        averageNetworkLatencySystemLevel = first+second;
    }
    ++averageNetworkLatencySystemLevelInstances;
}

void GlobalReport::reportAverageNetworkLatencySystemLevel()
{
    cout << "The average network latency was: " << averageNetworkLatencySystemLevel << endl;
}

void GlobalReport::updateMaxNetworkLatencySystemLevel(double newLatency)
{
    if (maxNetworkLatency<newLatency) {
        maxNetworkLatency = newLatency;
    }
}

void GlobalReport::reportMaxNetworkLatencySystemLevel()
{
    cout << "The maximum network latency was: " << maxNetworkLatency << endl;
}

void GlobalReport::updateVCUsageHist(int routerId, int dir, int numOfActiveVCs)
{
    int counter = VCsUsageHist.at(routerId).at(dir).at(numOfActiveVCs);
    VCsUsageHist[routerId][dir][numOfActiveVCs] = ++counter;
}

void GlobalReport::reportVCUsageHist(std::string& csvFileName, int routerId)
{
    ofstream csvFile;
    csvFile.open(csvFileName);
    int firstDim = VCsUsageHist[routerId].size();
    for (unsigned int dir = 0; dir<firstDim; ++dir) {
        csvFile << DIR::toString(dir) << ",";
        int secondDim = VCsUsageHist[routerId][dir].size();
        for (unsigned int value = 0; value<secondDim; ++value) {
            if (value<secondDim-1)
                csvFile << VCsUsageHist[routerId][dir][value] << ",";
            else
                csvFile << VCsUsageHist[routerId][dir][value] << "\n";
        }
    }
    csvFile.close();
}

void GlobalReport::updateBuffUsagePerVCHist(int routerId, int dir, int vc, int bufferOccupation)
{
    long counter = bufferUsagePerVCHist.at(routerId).at(dir).at(vc).at(bufferOccupation);
    bufferUsagePerVCHist[routerId][dir][vc][bufferOccupation] = ++counter;
}

void GlobalReport::reportBuffPerVCUsageHist(std::string& csvFileName, int routerId, int dir)
{
    ofstream csvFile;
    csvFile.open(csvFileName);

    // Write the header
    csvFile << "Buffer\\VC,";
    int secondDim = bufferUsagePerVCHist[routerId][dir].size();
    for (unsigned int vc = 0; vc<secondDim; ++vc) { // foreach vc
        csvFile << vc;
        if (vc<secondDim-1)
            csvFile << ",";
        else
            csvFile << "\n";
    }

    // Write the matrix
    for (unsigned int buffer = 1; buffer<=MAX_BUFFER_DEPTH; ++buffer) { // foreach buffer
        for (unsigned int column = 0; column<=secondDim; ++column) { // foreach column
            if (column==0)
                csvFile << buffer << ", ";
            else {
                csvFile << bufferUsagePerVCHist[routerId][dir][column-1][buffer];
                if (column<secondDim)
                    csvFile << ",";
                else
                    csvFile << "\n";
            }
        }
    }
    csvFile.close();
}

void GlobalReport::reportAllRoutersUsageHist()
{
    std::vector<int> routers_ids{};
    if (!bufferReportRouters.empty()) {
        routers_ids = bufferReportRouters;
    }
    else {
        int num_nodes = globalResources.nodes.size();
        for (unsigned int i = 0; i<num_nodes; ++i) {
            if (globalResources.nodes[i].type->model=="RouterVC") {
                routers_ids.push_back(i);
            }
        }
    }
    int num_routers = routers_ids.size();
    for (unsigned int i = 0; i<num_routers; ++i) {
        int router_id = routers_ids[i];
        std::string csvFileName;

        int isFolderCreated = system("mkdir -p ./VCUsage");
        if (isFolderCreated!=0) {
            std::cerr << "VCUsage folder was not created!" << std::endl;
            std::exit(EXIT_FAILURE);
        }
        csvFileName = "VCUsage/"+std::to_string(router_id)+".csv";
        reportVCUsageHist(csvFileName, router_id);

        isFolderCreated = system("mkdir -p ./BuffUsage");
        if (isFolderCreated!=0) {
            std::cerr << "VCUsage folder was not created!" << std::endl;
            std::exit(EXIT_FAILURE);
        }
        if (!bufferUsagePerVCHist.at(router_id).empty()) {
            int nodeConnsSize = globalResources.nodes[router_id].connections.size();
            for (unsigned int conPos = 0; conPos<nodeConnsSize; ++conPos) {
                int dir_int = globalResources.nodes[router_id].getDirOfConPos(conPos);
                std::string dir_str = DIR::toString(globalResources.nodes[router_id].getDirOfConPos(conPos));
                boost::trim(dir_str);
                csvFileName = "BuffUsage/"+std::to_string(router_id)+"_"+dir_str+".csv";
                reportBuffPerVCUsageHist(csvFileName, router_id, dir_int);
            }
        }
    }
}

void GlobalReport::readConfigFile(const std::string& config_path)
{
    pugi::xml_document doc;
    pugi::xml_parse_result result = doc.load_file(config_path.c_str());
    assert(result && "Failed to read simulator config file!");

    //	PE Verbosity
    pugi::xml_node verbose_node = doc.child("configuration").child("verbose").child("processingElements");
    this->verbose_pe_function_calls = verbose_node.child("function_calls").attribute("value").as_bool();
    this->verbose_pe_send_flit = verbose_node.child("send_flit").attribute("value").as_bool();
    this->verbose_pe_send_head_flit = verbose_node.child("send_head_flit").attribute("value").as_bool();
    this->verbose_pe_receive_flit = verbose_node.child("receive_flit").attribute("value").as_bool();
    this->verbose_pe_receive_tail_flit = verbose_node.child("receive_tail_flit").attribute(
            "value").as_bool();
    this->verbose_pe_throttle = verbose_node.child("throttle").attribute("value").as_bool();
    this->verbose_pe_reset = verbose_node.child("reset").attribute("value").as_bool();

    //	Router Verbosity
    verbose_node = doc.child("configuration").child("verbose").child("router");
    this->verbose_router_function_calls = verbose_node.child("function_calls").attribute(
            "value").as_bool();
    this->verbose_router_send_flit = verbose_node.child("send_flit").attribute("value").as_bool();
    this->verbose_router_send_head_flit = verbose_node.child("send_head_flit").attribute(
            "value").as_bool();
    this->verbose_router_receive_flit = verbose_node.child("receive_flit").attribute("value").as_bool();
    this->verbose_router_receive_head_flit = verbose_node.child("receive_head_flit").attribute(
            "value").as_bool();
    this->verbose_router_assign_channel = verbose_node.child("assign_channel").attribute(
            "value").as_bool();
    this->verbose_router_throttle = verbose_node.child("throttle").attribute("value").as_bool();
    this->verbose_router_buffer_overflow = verbose_node.child("buffer_overflow").attribute(
            "value").as_bool();
    this->verbose_router_reset = verbose_node.child("reset").attribute("value").as_bool();

    //	Netrace Verbosity
    verbose_node = doc.child("configuration").child("verbose").child("netrace");
    this->verbose_netrace_inject = verbose_node.child("inject").attribute("value").as_bool();
    this->verbose_netrace_eject = verbose_node.child("eject").attribute("value").as_bool();
    this->verbose_netrace_router_receive = verbose_node.child("router_receive").attribute(
            "value").as_bool();

    //	Task Verbosity
    verbose_node = doc.child("configuration").child("verbose").child("tasks");
    this->verbose_task_xml_parse = verbose_node.child("xml_parse").attribute("value").as_bool();
    this->verbose_task_data_receive = verbose_node.child("data_receive").attribute("value").as_bool();
    this->verbose_task_data_send = verbose_node.child("data_send").attribute("value").as_bool();
    this->verbose_task_source_execute = verbose_node.child("source_execute").attribute(
            "value").as_bool();
    this->verbose_task_function_calls = verbose_node.child("function_calls").attribute(
            "value").as_bool();

    std::string s = doc.child("configuration").child("report").child_value("bufferReportRouters");
    std::string delimiter = " ";
    std::size_t current, previous = 0;
    current = s.find(delimiter);
    while (current!=std::string::npos) {
        try {
            bufferReportRouters.push_back(std::stoi(s.substr(previous, current-previous)));
        }
        catch (char* error) {
            cout << "Error: " << error << endl;
        }
        previous = current+1;
        current = s.find(delimiter, previous);
    }
    bufferReportRouters.push_back(std::stoi(s.substr(previous, current-previous)));
}

void GlobalReport::resizeMatrices()
{
    numRouters = static_cast<unsigned int>(globalResources.nodes.size()/2);
    VCsUsageHist.resize(numRouters);
    bufferUsagePerVCHist.resize(numRouters);
    linkTransmissionsMatrixNumberOfStates = (2*globalResources.numberOfTrafficTypes)+3;

    int numOfLinks = globalResources.connections.size()*2;
    for (int link_id = 0; link_id<numOfLinks; ++link_id) {
        int numberElements = static_cast<int>(pow(linkTransmissionsMatrixNumberOfStates, 2));
        std::vector<long> matrix(numberElements, 0);
        linkTransmissionMatrices.insert(std::make_pair(link_id, matrix));
    }

    clockCounts.resize(globalResources.nodeTypes.size()/2, 0.0);
    buffer_router_push_pwr_d.resize(numRouters, 0.0);
    buffer_router_pop_pwr_d.resize(numRouters, 0.0);
    buffer_router_front_pwr_d.resize(numRouters, 0.0);
    buffer_router_pwr_s.resize(numRouters, 0.0);
    routing_pwr_d.resize(numRouters, 0.0);
    routing_pwr_s.resize(numRouters, 0.0);
    crossbar_pwr_d.resize(numRouters, 0.0);
    crossbar_pwr_s.resize(numRouters, 0.0);
    ni_pwr_d.resize(numRouters, 0.0);
    ni_pwr_s.resize(numRouters, 0.0);

    Connection con = globalResources.connections.at(0);
    int vcCount = con.vcsCount.at(0);
    VCsUsageHist.resize(numRouters);
    for (auto& vec_2d:VCsUsageHist) {
        vec_2d.resize(DIR::size);
        for (auto& vec:vec_2d) {
            vec.resize(vcCount+1);
        }
    }
    bufferUsagePerVCHist.resize(numRouters);
    for (auto& vec_3d:bufferUsagePerVCHist) {
        vec_3d.resize(DIR::size);
        for (auto& vec_2d:vec_3d) {
            vec_2d.resize(vcCount);
            for (auto& vec:vec_2d) {
                vec.resize(MAX_BUFFER_DEPTH+1);
            }
        }
    }
}

void GlobalReport::reportClockCount(ostream& stream)
{
    stream << "Clock Counts: [";
    int numOfClocks = clockCounts.size();
    for (int i = 0; i<numOfClocks; ++i) {
        stream << std::fixed << (clockCounts.at(i));
        if (i<clockCounts.size()-1)
            stream << ", ";
        else
            stream << "]\n";
    }
}

void GlobalReport::increaseClockCount(int layer)
{
    this->clockCounts.at(layer) += 1;
}

void GlobalReport::increaseBufferPush(int router_id)
{
    buffer_router_push_pwr_d.at(router_id) += 1;
}

void GlobalReport::increaseBufferPop(int router_id)
{
    buffer_router_pop_pwr_d.at(router_id) += 1;
}

void GlobalReport::increaseBufferFront(int router_id)
{
    buffer_router_front_pwr_d.at(router_id) += 1;
}

void GlobalReport::increaseRouting(int router_id)
{
    routing_pwr_d.at(router_id) += 1;
}

void GlobalReport::increaseCrossbar(int router_id)
{
    crossbar_pwr_d.at(router_id) += 1;
}

void GlobalReport::reportRoutersPowerCSV(ostream& csvfile)
{
    csvfile << "router_id," << "buffer_push," << "buffer_pop," << "buffer_read_front," << "routing," << "crossbar"
            << "\n";
    for (int id = 0; id<buffer_router_push_pwr_d.size(); ++id) {
        csvfile << id << "," << buffer_router_push_pwr_d.at(id) << "," << buffer_router_pop_pwr_d.at(id)
                << "," << buffer_router_front_pwr_d.at(id) << "," << routing_pwr_d.at(id) << ","
                << crossbar_pwr_d.at(id) << "\n";
    }
}

