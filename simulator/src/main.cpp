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

#include "boost/program_options.hpp"

#include "utils/GlobalResources.h"
#include "utils/GlobalReport.h"
#include "utils/Report.h"
#include "model/NoC.h"

namespace po = boost::program_options;

int sc_main(int arg_num, char* arg_vec[])
{
    GlobalResources& globalResources = GlobalResources::getInstance();
    GlobalReport& globalReport = GlobalReport::getInstance();
    Report& rep = Report::getInstance();  // database report
    sleep(1);  // wait for the systemC branding
    cout << endl << "Ratatoskr 3D-NoC Simulator Copyright(C) 2014-2021" << endl;
    cout << "   Dr. Jan Moritz Joseph (joseph@ice.rwth-aachen.de) " << endl;
    cout << "This program comes with ABSOLUTELY NO WARRANTY;" << endl;
    cout << "This is free software, and you are welcome to redistribute it" << endl;
    cout << "under certain conditions. For details see README file." << endl << endl;
    sc_report_handler::set_verbosity_level(SC_DEBUG);
    sc_report_handler::set_actions(SC_ID_INSTANCE_EXISTS_, SC_DO_NOTHING); //disable renaming warnings

    if (arg_num==2) {
        globalResources.readConfigFile(arg_vec[1]);
    }
    else {
#ifndef ENABLE_NETRACE
        std::string config_path = "config/config.xml";
#endif
#ifdef ENABLE_NETRACE
        std::string config_path = "config/ntConfig.xml";
#endif
        globalResources.readConfigFile(config_path);
        globalReport.readConfigFile(config_path);
    }
    globalResources.readNoCLayout(globalResources.noc_file);
    if (!globalResources.data_file.empty() && !globalResources.map_file.empty())
        globalResources.readTaskAndMapFiles(globalResources.data_file, globalResources.map_file);

    globalReport.resizeMatrices();

    rep.connect("127.0.0.1", "10000");
    rep.startRun("name");

#ifdef ENABLE_NETRACE
    po::variables_map vm;
    po::options_description desc("Allowed Options");
    desc.add_options()
            ("simTime", po::value<std::string>()->default_value(""), "Length of simulation");
    desc.add_options()
            ("netraceRegion", po::value<int>()->default_value(2), "Netrace: Region of the simulation, per default the PARSEC's ROI");
    desc.add_options()
            ("netraceTraceFile", po::value<std::string>()->default_value(""), "Netrace: Trace file");
    desc.add_options()
            ("netraceVerbosity", po::value<std::string>()->default_value("all"), "Netrace: Verbosity Level,"
                                                                                 "all=info about trace file and about packet generation. "
                                                                                 "base = info about trace file. none = no output");
    try{
        po::store(po::parse_command_line(arg_num, arg_vec, desc), vm);
        po::notify(vm);
    } catch (po::error& e) {
        cerr << "ERROR: " << e.what() << endl << endl << desc << endl;
        return 1;
    }
    std::string simTimeString = vm["simTime"].as<std::string>();
    if (!simTimeString.empty()) {
        globalResources.simulation_time = std::stoi(simTimeString);
    }
    std::string netraceTraceFile = vm["netraceTraceFile"].as<std::string>();
    if (!netraceTraceFile.empty()) {
        globalResources.netraceFile = netraceTraceFile;
    }
    if (vm["netraceRegion"].as<int>() <=5 && vm["netraceRegion"].as<int>() >= 0)
        globalResources.netraceStartRegion = vm["netraceRegion"].as<int>();
    else
        globalResources.netraceStartRegion = 2;
    std::string netraceVerbosityInput = vm["netraceVerbosity"].as<std::string>();
    if (netraceVerbosityInput.compare("all") == 0)
        globalResources.netraceVerbosity = 2;
    else if (netraceVerbosityInput.compare("base") == 0)
        globalResources.netraceVerbosity = 1;
    else
        globalResources.netraceVerbosity = 0;

    std::string config_path = "config/ntConfig.xml";
#endif


    std::unique_ptr<NoC> noc = std::make_unique<NoC>("Layer");
    std::chrono::high_resolution_clock::time_point t1 = std::chrono::high_resolution_clock::now();
    cout << "Random seed " << globalResources.rd_seed << endl;
    cout << endl << "Starting Simulation!" << endl;
    sc_start(globalResources.simulation_time, SC_NS);

    if (globalResources.outputToFile) {
        cout << "Generating report of the simulation run into file " << globalResources.outputFileName << " ... " << endl << endl;
        globalReport.reportComplete(globalResources.outputFileName);
        cout << " done." << endl;
    }
    globalReport.reportPerformance(cout);
    cout << "Random seed " << globalResources.rd_seed << endl;

    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now()-t1);
    auto secs = std::chrono::duration_cast<std::chrono::seconds>(ms);
    ms -= std::chrono::duration_cast<std::chrono::milliseconds>(secs);
    auto mins = std::chrono::duration_cast<std::chrono::minutes>(secs);
    secs -= std::chrono::duration_cast<std::chrono::seconds>(mins);
    auto hours = std::chrono::duration_cast<std::chrono::hours>(mins);
    mins -= std::chrono::duration_cast<std::chrono::minutes>(hours);
    cout << "Execution time: " << hours.count() << "h " << mins.count() << " min and " << secs.count() << " seconds" << std::endl;

    rep.close();

    return 0;
}
