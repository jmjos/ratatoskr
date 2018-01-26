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
#include "systemc.h"
#include <iostream>
#include <fstream>
#include <string>
#include <chrono>

#include "utils/GlobalInputClass.h"
#include "utils/GlobalReportClass.h"
#include "utils/Report.h"
#include "model/layer/LayerTop.h"
#include "traffic/netrace/NetracePool.h"
#include "traffic/synthetic/SyntheticPool.h"
#include "traffic/tasks/TaskPool.h"

#include "model/link/Link.h"
#include "model/processingElements/ProcessingElementVC3D.h"
#include "utils/portsOpenConst.h"

int sc_main(int arg_num, char *arg_vet[]) {
	GlobalInputClass& global = GlobalInputClass::getInstance();
	Report& rep = Report::getInstance();
	srand(time(NULL));
	sleep(1); //wait for the systemC branding :/

	cout << "\nA-3D-NoC Simulator Copyright(C) 2014-2018" << endl;
	cout << "   Jan Moritz Joseph (jan.joseph@ovgu.de) " << endl;
	cout << "This program comes with ABSOLUTELY NO WARRANTY;" << endl;
	cout << "This is free software, and you are welcome to redistribute it"
			<< endl;
	cout << "under certain conditions. For details see README file." << endl
			<< endl;
	cout << "Welcome to the A-3D-NoC Simulator!" << endl;

	sc_report_handler::set_verbosity_level(SC_DEBUG);
	sc_report_handler::set_actions(SC_ID_INSTANCE_EXISTS_, SC_DO_NOTHING); //disable renaming warnings

	global.readInputFile("config/config.xml");
	global.readNoCLayout(global.noc_file);

	rep.connect("127.0.0.1", "10000");
	rep.startRun("name");

	LayerTop *layer = new LayerTop("Layer");
	TrafficPool *tp;

	if (strcmp(global.benchmark.c_str(), "application") == 0) {
		tp = new TaskPool("taskhandler", &layer->processingElements,
				global.application_file, global.application_mapping_file);
	} else if (strcmp(global.benchmark.c_str(), "netrace") == 0) {
		tp = new NetracePool("netracePool", &layer->processingElements);
	} else if (strcmp(global.benchmark.c_str(), "synthetic") == 0) {
		tp = new SyntheticPool("syntheticPool", &layer->processingElements);
	} else {
		std::cout << "Please specify correct benchmark type" << std::endl;
		return 0;
	}

	std::chrono::high_resolution_clock::time_point t1 =
			std::chrono::high_resolution_clock::now();

	cout << endl << "Starting Simulation!" << endl;
	sc_start(global.simulation_time, SC_NS);

	GlobalReportClass report = GlobalReportClass::getInstance();
//
//	std::ostream & objOstream = std::cout;
//	report.reportLinkMatrix(8, objOstream);

	if (global.outputToFile){
		cout << "Generating report of the simulation run into file " << global.outputFileName << " ... ";
		report.makeReport(global.outputFileName);
		cout << " done." << endl;
	}


	auto duration = std::chrono::duration_cast<std::chrono::microseconds>(
			chrono::high_resolution_clock::now() - t1).count();
	cout << "Execution time: " << duration << " microseconds" << std::endl;

	rep.close();
	return 0;
}
