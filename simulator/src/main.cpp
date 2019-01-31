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
#include "systemc.h"
#include <iostream>
#include <fstream>
#include <string>
#include <chrono>
#include "utils/GlobalResources.h"
#include "utils/GlobalReport.h"
#include "utils/Report.h"
#include "model/NoC.h"

int sc_main(int arg_num, char* arg_vec[])
{
    GlobalResources& globalResources = GlobalResources::getInstance();
    GlobalReport& globalReport = GlobalReport::getInstance();
    //Report& rep = Report::getInstance();  // database report
    //srand(0);
    sleep(1);  // wait for the systemC branding
    cout << endl << "A-3D-NoC Simulator Copyright(C) 2014-2019" << endl;
    cout << "   Jan Moritz Joseph (jan.joseph@ovgu.de) " << endl;
    cout << "This program comes with ABSOLUTELY NO WARRANTY;" << endl;
    cout << "This is free software, and you are welcome to redistribute it" << endl;
    cout << "under certain conditions. For details see README file." << endl << endl;
    cout << "Welcome to the A-3D-NoC Simulator!" << endl;
    sc_report_handler::set_verbosity_level(SC_DEBUG);
    sc_report_handler::set_actions(SC_ID_INSTANCE_EXISTS_, SC_DO_NOTHING); //disable renaming warnings

    if (arg_num==2) {
        globalResources.readConfigFile(arg_vec[1]);
    }
    else {
        std::string config_path = "config/config.xml";
        globalResources.readConfigFile(config_path);
        globalReport.readConfigFile(config_path);
    }
    globalResources.readNoCLayout(globalResources.noc_file);
    if (!globalResources.data_file.empty() && !globalResources.map_file.empty())
        globalResources.readTaskAndMapFiles(globalResources.data_file, globalResources.map_file);

    globalReport.resizeMatrices();

    //rep.connect("127.0.0.1", "10000");
    //rep.startRun("name");

    std::unique_ptr<NoC> noc = std::make_unique<NoC>("Layer");
    std::chrono::high_resolution_clock::time_point t1 = std::chrono::high_resolution_clock::now();
    cout << "Random seed " << globalResources.rd_seed << endl;
    cout << endl << "Starting Simulation!" << endl;
    sc_start(globalResources.simulation_time, SC_NS);

    if (globalResources.outputToFile) {
        cout << "Generating report of the simulation run into file " << globalResources.outputFileName << " ... ";
        globalReport.reportComplete(globalResources.outputFileName);
        cout << " done." << endl;
    }
    globalReport.reportPerformance(cout);
    cout << "Random seed " << globalResources.rd_seed << endl;
    auto duration = std::chrono::duration_cast<std::chrono::seconds>(
            std::chrono::high_resolution_clock::now()-t1).count();
    auto durationmin = std::chrono::duration_cast<std::chrono::minutes>(
            std::chrono::high_resolution_clock::now()-t1).count();
    cout << "Execution time: " << durationmin << " minutes and " << duration << " seconds" << std::endl;
    //rep.close();

    return 0;
}
