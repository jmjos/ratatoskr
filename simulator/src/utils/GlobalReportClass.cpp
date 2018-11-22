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
#include "GlobalReportClass.h"
#include "Structures.h"
#include <cstdlib>

GlobalReportClass::GlobalReportClass() :
        total_power_s(0.0),
        buffer_router_push_pwr_d(0.0),
        buffer_router_pop_pwr_d(0.0),
        buffer_router_front_pwr_d(0.0),
        buffer_router_pwr_s(0.0),
        routing_pwr_d(0.0),
        routing_pwr_s(0.0),
        crossbar_pwr_d(0.0),
        crossbar_pwr_s(0.0),
        link_r2r_pwr_d(0.0),
        link_r2r_pwr_s(0.0),
        ni_pwr_d(0.0),
        ni_pwr_s(0.0),
        latencyNetwork("network latency"),
        latencyFlit("flit latency"),
        latencyPacket("packet latency ") {
    unsigned int numRouters = global.nodes.size() / 2;
    this->VCsUsageHist.resize(numRouters);
    this->bufferUsageHist.resize(numRouters);
    this->bufferUsagePerVCHist.resize(numRouters);
    this->linkTransmissionsMatrixNumberOfStates = (2 * global.application_numberOfTrafficTypes) + 3;
}

void GlobalReportClass::reportComplete(std::string filename) {
    ofstream myfile;
    myfile.open(filename + ".txt");
    // Writing general simulation data:
    myfile << boost::format("Report file of this simulation run. \n\n");
    myfile << "----------------------------------------------------" << endl;
    myfile << "General Information:" << endl;
    myfile << "----------------------------------------------------" << endl;\
    reportPerformance(myfile);

    // Writing router routing calculations
    myfile << "----------------------------------------------------" << endl;
    myfile << "Router Matrixes:" << endl;
    myfile << "----------------------------------------------------" << endl;
    myfile << "Routing calculations:\n";
    reportRoutingCalculations(myfile);

    // Writing link matrixes
    myfile << "----------------------------------------------------" << endl;
    myfile << "Link Matrixes:" << endl;
    myfile << "----------------------------------------------------" << endl;
    reportLinkMatrices(myfile);
    myfile.close();

    // Generate csv for link matrices
    std::string csvFilename = filename + "_Links.csv";
    ofstream csvfile;
    csvfile.open(csvFilename);
    reportLinkMatricesCSV(csvfile);
    csvfile.close();

    // Generate csv for performance statistics
    csvFilename = filename + "_Performance.csv";
    csvfile.open(csvFilename);
    reportPerformanceCSV(csvfile);
    csvfile.close();

    reportAllRoutersUsageHist();
}

void GlobalReportClass::reportPerformance(ostream &stream) {
    float avgFlitLat = latencyFlit.average() / (float) 1000;
    float avgPacketLat = latencyPacket.average() / (float) 1000;
    float avgNetworkLat = latencyNetwork.average() / (float) 1000;
    stream << boost::format("Average flit latency: %3.2f ns.\n") % avgFlitLat;
    stream << boost::format("Average packet latency: %3.2f ns.\n") % avgPacketLat;
    stream << boost::format("Average network latency: %3.2f ns.\n") % avgNetworkLat;
}

void GlobalReportClass::reportPerformanceCSV(ostream &stream) {
    float avgFlitLat = latencyFlit.average() / (float) 1000;
    float avgPacketLat = latencyPacket.average() / (float) 1000;
    float avgNetworkLat = latencyNetwork.average() / (float) 1000;
    stream << boost::format("avgFlitLat, %2.3f\n") % avgFlitLat;
    stream << boost::format("avgPacketLat, %2.3f\n") % avgPacketLat;
    stream << boost::format("avgNetworkLat, %2.3f\n") % avgNetworkLat;
}

void GlobalReportClass::issueRoutingCalculation(int id) {
    std::map<int, int>::iterator it = routingCalulcations.find(id);
    if (it != routingCalulcations.end())
        it->second++;
    else
        routingCalulcations.insert(std::make_pair(id, 1));
}

void GlobalReportClass::reportRoutingCalculations(ostream &stream) {
    for (auto it : routingCalulcations) {
        stream << boost::format("\tRouter id: %i had %i calculations \n") % it.first % it.second;
    }

}

//void GlobalReportClass::issueReset(int id) {
//	std::map<int, int>::iterator it = numberOfResets.find(id);
//	if (it != numberOfResets.end())
//		it->second++;
//	else
//		numberOfResets.insert(std::make_pair(id, 1));
//}
//
//void GlobalReportClass::reportReset() {
//	int totalResets = 0;
//	for (auto element : numberOfResets) {
//		std::cout << "Router " << element.first << " had " << element.second
//				<< " resets." << std::endl;
//		totalResets += element.second;
//	}
//	std::cout << "Complete number of Resets: " << totalResets << endl;
//}
//
//void GlobalReportClass::reportResetTotal() {
//	int totalResets = 0;
//	for (auto element : numberOfResets) {
//		totalResets += element.second;
//	}
//	std::cout << "Complete number of Resets: " << totalResets << endl;
//}

void GlobalReportClass::issueLinkMatrixUpdate(int id, int currentTransmissionState, int lastTransmissionState) {
    std::map<int, std::vector<long> >::iterator it = linkTransmissionMatrices.find(id);
    if (it == linkTransmissionMatrices.end()) {
        // not found, initialization
        int numberElements = pow(linkTransmissionsMatrixNumberOfStates, 2);
        std::vector<long> matrix(numberElements, 0);
        linkTransmissionMatrices.insert(std::make_pair(id, matrix));
    }

    linkTransmissionMatrices.at(id).at(
            currentTransmissionState + linkTransmissionsMatrixNumberOfStates * lastTransmissionState)++;
}

void GlobalReportClass::reportLinkMatrix(int id, ostream &stream) {
    auto transmissionMatrix = linkTransmissionMatrices.at(id);
    long clockCyclesOfLink = std::accumulate(transmissionMatrix.begin(), transmissionMatrix.end(), 0);
    stream << boost::format("Transmission matrix of link %i:\n") % id;
    int colit = 0, rowit = 0;
    stream << boost::format("from\\to     IDLE     HEAD     HID");
    for (int var = 0; var < global.application_numberOfTrafficTypes; ++var) {
        stream << boost::format("%7iD%6iID") % var % var;
    }
    stream << boost::format("\n");
    for (auto matrixelement : transmissionMatrix) {
        float field = (float) matrixelement / (float) clockCyclesOfLink;
        if (colit == 0) {
            if (rowit == 0) {
                stream << boost::format("IDLE\t");
            } else if (rowit == 1) {
                stream << boost::format("HEAD\t");
            } else if (rowit == 2) {
                stream << boost::format("HID\t\t");
            } else if (rowit % 2 == 0) {
                int trafficType = (rowit - 3) / 2;
                stream << boost::format("%iID\t\t") % trafficType;
            } else if (rowit % 2 == 1) {
                int trafficType = (rowit - 3) / 2;
                stream << boost::format("%iD\t\t") % trafficType;
            }
            rowit++;
            stream << boost::format("[%7.4f, ") % field;
        } else if (colit == linkTransmissionsMatrixNumberOfStates - 1) {
            stream << boost::format("%7.4f ]\n") % field;
        } else {
            stream << boost::format("%7.4f,") % field;
        }
        colit++;
        if (colit == linkTransmissionsMatrixNumberOfStates) {
            colit = 0;
        }
    }
}

void GlobalReportClass::reportLinkMatrices(ostream &stream) {
    auto numberOfLinks = linkTransmissionMatrices.size();
    for (unsigned int var = 0; var < numberOfLinks; ++var) {
        reportLinkMatrix(var, stream);
    }
}

void GlobalReportClass::reportLinkMatricesCSV(ostream &stream) {
    int numberOfElements = pow(linkTransmissionsMatrixNumberOfStates, 2);
    stream << boost::format("link_id");
    for (int var = 0; var < numberOfElements - 1; ++var) {
        stream << boost::format(", %i") % var;
    }
    stream << boost::format(", %i\n") % (numberOfElements - 1);
    for (auto matrix : linkTransmissionMatrices) {
        stream << boost::format("%i, ") % matrix.first;
        for (auto it = matrix.second.begin(); it != matrix.second.end(); it++) {
            int value = matrix.second.at(it - matrix.second.begin());
            if (std::next(it) == matrix.second.end()) {
                stream << boost::format("%i\n") % value;
            } else {
                stream << boost::format("%i, ") % value;
            }
        }
    }
}

void GlobalReportClass::issueAcceleration(int id) {
    std::map<int, int>::iterator it = numberOfAccelerations.find(id);
    if (it != numberOfAccelerations.end())
        it->second++;
    else
        numberOfAccelerations.insert(std::make_pair(id, 1));
}

void GlobalReportClass::reportAccelerations() {
    int totalAccelerations = 0;
    for (auto element : numberOfAccelerations) {
        std::cout << "Router " << element.first << " had " << element.second << " accelerations." << std::endl;
        totalAccelerations += element.second;
    }
    std::cout << "Complete number of Accelerations: " << totalAccelerations << std::endl;
}

void GlobalReportClass::reportAccelerationsTotal() {
    int totalAccelerations = 0;
    for (auto element : numberOfAccelerations) {
        totalAccelerations += element.second;
    }
    std::cout << "Complete number of Accelerations: " << totalAccelerations << std::endl;
}

void GlobalReportClass::updateAverageNetworkLatencySystemLevel(double newLatency) {
    if (averageNetworkLatencySystemLevel == -1) {
        averageNetworkLatencySystemLevel = (double) newLatency;
    } else {
        double scalingLarge = ((double) averageNetworkLatencySystemLevelInstances /
                               ((double) averageNetworkLatencySystemLevelInstances + (double) 1));
        double first = scalingLarge * averageNetworkLatencySystemLevel;
        double scalingSmall = ((double) 1 / ((double) averageNetworkLatencySystemLevelInstances + (double) 1));
        double second = scalingSmall * newLatency;
        averageNetworkLatencySystemLevel = first + second;
    }
    averageNetworkLatencySystemLevelInstances++;
}

void GlobalReportClass::reportAverageNetworkLatencySystemLevel() {
    cout << "The average network latency was: " << averageNetworkLatencySystemLevel << endl;
}

void GlobalReportClass::updateMaxNetworkLatencySystemLevel(double newLatency) {
    if (maxNetworkLatency < newLatency) {
        maxNetworkLatency = newLatency;
    }
}

void GlobalReportClass::reportMaxNetworkLatencySystemLevel() {
    cout << "The maximum network latency was: " << maxNetworkLatency << endl;
}

void GlobalReportClass::updateUsageHist(std::vector<std::vector<std::vector<long>>> &histVec, int routerId, int dir,
                                        int thirdDimensionIndex, int thirdDimensionSize) {
    if (!histVec.at(routerId).empty()) {
        int counter = histVec.at(routerId).at(dir).at(thirdDimensionIndex);
        histVec[routerId][dir][thirdDimensionIndex] = ++counter;
    } else {
        std::vector<std::vector<long>> vec_2d(DIR::size);
        for (auto &vec : vec_2d) {
            vec.assign(thirdDimensionSize, 0);
        }
        histVec[routerId] = vec_2d;
        histVec[routerId][dir][thirdDimensionIndex] = 1;
    }
}

void GlobalReportClass::reportUsageHist(std::vector<std::vector<std::vector<long>>> &histVec, std::string &csvFileName,
                                        int routerId) {
    ofstream csvFile;
    csvFile.open(csvFileName);
    for (unsigned int dir = 0; dir < histVec[routerId].size(); dir++) {
        csvFile << DIR::toString(dir) << ",";
        for (unsigned int value = 0; value < histVec[routerId][dir].size(); value++) {
            if (value < histVec[routerId][dir].size() - 1)
                csvFile << histVec[routerId][dir][value] << ",";
            else
                csvFile << histVec[routerId][dir][value] << "\n";
        }
    }
    csvFile.close();
}

void GlobalReportClass::updateBuffUsagePerVCHist(std::vector<std::vector<std::vector<std::vector<long>>>> &histVec,
                                                 int routerId, int dir, int vc, int bufferOccupation, int numVCs) {
    if (!histVec.at(routerId).empty()) {
        long counter = histVec.at(routerId).at(dir).at(vc).at(bufferOccupation);
        histVec[routerId][dir][vc][bufferOccupation] = ++counter;

    } else {
        std::vector<std::vector<std::vector<long>>> vec_3d(DIR::size);
        for (unsigned int i = 0; i < vec_3d.size(); i++) {
            std::vector<std::vector<long>> vec_2d(numVCs);
            for (auto &vec_1d : vec_2d) {
                vec_1d.assign(MAX_BUFFER_DEPTH, 0);
            }
            vec_3d[i] = vec_2d;
        }
        histVec[routerId] = vec_3d;
        histVec[routerId][dir][vc][bufferOccupation] = 1;
    }
}

void GlobalReportClass::reportBuffUsageHist(std::vector<std::vector<std::vector<std::vector<long>>>> &histVec,
                                            std::string &csvFileName, int routerId, int dir) {
    ofstream csvFile;
    csvFile.open(csvFileName);

    // Write the header
    csvFile << "Buffer\\VC,";
    for (unsigned int vc = 0; vc < histVec[routerId][dir].size(); vc++) { // foreach vc
        csvFile << vc;
        if (vc < histVec[routerId][dir].size() - 1)
            csvFile << ",";
        else
            csvFile << "\n";
    }

    // Write the matrix
    for (unsigned int buffer = 1; buffer <= 50; buffer++) { // foreach buffer
        for (unsigned int column = 0; column <= histVec[routerId][dir].size(); column++) { // foreach column
            if (column == 0)
                csvFile << buffer << ", ";
            else {
                csvFile << histVec[routerId][dir][column - 1][buffer];
                if (column < histVec[routerId][dir].size())
                    csvFile << ",";
                else
                    csvFile << "\n";
            }
        }
    }
    csvFile.close();
}

void GlobalReportClass::reportAllRoutersUsageHist() {
    for (unsigned int i = 0; i < global.nodes.size(); i++) {
        if (global.nodes[i]->type->routerModel == "RouterVC" &&
            std::find(INNER_ROUTERS.begin(), INNER_ROUTERS.end(), i) != INNER_ROUTERS.end()) {
            std::string csvFileName;

            int isFolderCreated = system("mkdir -p ./VCUsage");
            if (isFolderCreated != 0) {
                std::cerr << "VCUsage folder was not created!" << std::endl;
                std::exit(EXIT_FAILURE);
            }
            csvFileName = "VCUsage/" + std::to_string(i) + ".csv";
            reportUsageHist(VCsUsageHist, csvFileName, i);

            isFolderCreated = system("mkdir -p ./BuffUsage");
            if (isFolderCreated != 0) {
                std::cerr << "VCUsage folder was not created!" << std::endl;
                std::exit(EXIT_FAILURE);
            }
            for (unsigned int dir = 0; dir < global.nodes[i]->connections.size(); dir++) {
                csvFileName = "BuffUsage/" + std::to_string(i) + "_" +
                              DIR::toString(global.nodes[i]->conToDir[dir]) + ".csv";
                reportBuffUsageHist(bufferUsagePerVCHist, csvFileName, i, dir);
            }
        }
    }
}




