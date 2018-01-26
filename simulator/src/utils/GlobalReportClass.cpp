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

GlobalReportClass::GlobalReportClass() {
}

void GlobalReportClass::makeReport(std::string filename){
	ofstream myfile;
	  myfile.open (filename + ".txt");
	  // Writing general simulation data:
	  myfile << boost::format("Report file of this simulation run. \n\n");
	  myfile << "----------------------------------------------------" << endl;
	  myfile << "General Information:" << endl;
	  myfile << "----------------------------------------------------" << endl;


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

	  //generate csv
	  std::string csvFilename = filename+ "Links.csv";
	  ofstream csvfile;
	  csvfile.open(csvFilename);
	  reportLinkMatricesCVS(csvfile);
	  csvfile.close();
}

void GlobalReportClass::issueRoutingCalculation(int id){
	std::map<int, int>::iterator it = routingCalulcations.find(id);
	if (it != routingCalulcations.end())
		it->second++;
	else
		routingCalulcations.insert(std::make_pair(id, 1));
}
void GlobalReportClass::reportRoutingCalculations(ostream& stream){
	for (auto it: routingCalulcations){
		stream << boost::format("\tRouter id: %i had %i calculations \n")% it.first % it.second;
	}

}

void GlobalReportClass::issueReset(int id) {
	std::map<int, int>::iterator it = numberOfResets.find(id);
	if (it != numberOfResets.end())
		it->second++;
	else
		numberOfResets.insert(std::make_pair(id, 1));
}

void GlobalReportClass::reportReset() {
	int totalResets = 0;
	for (auto element : numberOfResets) {
		std::cout << "Router " << element.first << " had " << element.second
				<< " resets." << std::endl;
		totalResets += element.second;
	}
	std::cout << "Complete number of Resets: " << totalResets << endl;
}

void GlobalReportClass::reportResetTotal() {
	int totalResets = 0;
	for (auto element : numberOfResets) {
		totalResets += element.second;
	}
	std::cout << "Complete number of Resets: " << totalResets << endl;
}

void GlobalReportClass::issueLinkMatrixUpdate(int id,
		int currentTransmissionState, int lastTransmissionState) {
	std::map<int, std::vector<long> >::iterator it =
			linkTransmissionMatrices.find(id);
	if (it == linkTransmissionMatrices.end()) {
		// not found, initialization
		int numberElements = pow(global.application_numberOfTrafficTypes + 2, 2);
		std::vector<long> matrix(numberElements, 0);
		linkTransmissionMatrices.insert(std::make_pair(id, matrix));
	}
	linkTransmissionMatrices.at(id).at(
			currentTransmissionState
					+ (global.application_numberOfTrafficTypes + 2)
							* lastTransmissionState)++;
}

void GlobalReportClass::reportLinkMatrix(int id, ostream& stream) {
	auto transmissionMatrix = linkTransmissionMatrices.at(id);
	long clockCyclesOfLink = std::accumulate(transmissionMatrix.begin(), transmissionMatrix.end(), 0);
	stream << boost::format("Transmission matrix of link %i:\n")% id;
	int colit = 0;
	for (auto matrixelement : transmissionMatrix) {
		float field = (float)matrixelement/(float)clockCyclesOfLink;
		if (colit == 0){
			stream << boost::format("\t[%7.4f, ") % field;
		} else if (colit == global.application_numberOfTrafficTypes + 1) {
			stream << boost::format("%7.4f]\n")% field;
		} else {
			stream << boost::format("%7.4f,")% field;
		}
		colit++;
		if (colit == global.application_numberOfTrafficTypes + 2){
			colit = 0;
		}
	}
}

void GlobalReportClass::reportLinkMatrices(ostream& stream) {
	auto numberOfLinks = linkTransmissionMatrices.size();
	for (unsigned int var = 0; var < numberOfLinks; ++var) {
		reportLinkMatrix(var, stream);
	}
}

void GlobalReportClass::reportLinkMatricesCVS(ostream& stream){
	int numberOfElements = pow(2 + global.application_numberOfTrafficTypes, 2);
	stream << boost::format("link_id");
	for (int var = 0; var < numberOfElements-1; ++var) {
		stream << boost::format(", %i") % var;
	}
	stream << boost::format(", %i\n") % (numberOfElements - 1);
	for (auto matrix : linkTransmissionMatrices){
		stream << boost::format("%i, ") % matrix.first;
		for (auto it = matrix.second.begin(); it != matrix.second.end(); it++){
			int value = matrix.second.at(it - matrix.second.begin());
			if (std::next(it) == matrix.second.end()){
				stream << boost::format("%i\n") % value;
			} else{
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
		std::cout << "Router " << element.first << " had " << element.second
				<< " accelerations." << std::endl;
		totalAccelerations += element.second;
	}
	std::cout << "Complete number of Accelerations: " << totalAccelerations
			<< std::endl;
}

void GlobalReportClass::reportAccelerationsTotal() {
	int totalAccelerations = 0;
	for (auto element : numberOfAccelerations) {
		totalAccelerations += element.second;
	}
	std::cout << "Complete number of Accelerations: " << totalAccelerations
			<< std::endl;
}

void GlobalReportClass::updateAverageNetworkLatencySystemLevel(
		double newLatency) {
	if (averageNetworkLatencySystemLevel == -1) {
		averageNetworkLatencySystemLevel = (double) newLatency;
	} else {
		double scalingLarge =
				((double) averageNetworkLatencySystemLevelInstances
						/ ((double) averageNetworkLatencySystemLevelInstances
								+ (double) 1));
		double first = scalingLarge * averageNetworkLatencySystemLevel;
		double scalingSmall = ((double) 1
				/ ((double) averageNetworkLatencySystemLevelInstances
						+ (double) 1));
		double second = scalingSmall * newLatency;
		averageNetworkLatencySystemLevel = first + second;
	}
	averageNetworkLatencySystemLevelInstances++;
}

void GlobalReportClass::reportAverageNetworkLatencySystemLevel() {
	cout << "The average network latency was: "
			<< averageNetworkLatencySystemLevel << endl;
}

void GlobalReportClass::updateMaxNetworkLatencySystemLevel(double newLatency) {
	if (maxNetworkLatency < newLatency) {
		maxNetworkLatency = newLatency;
	}
}

void GlobalReportClass::reportMaxNetworkLatencySystemLevel() {
	cout << "The maximum network latency was: " << maxNetworkLatency << endl;
}

