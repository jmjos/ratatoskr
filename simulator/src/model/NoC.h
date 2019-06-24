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

#include <model/container/FlitContainer.h>
#include <model/processingElement/ProcessingElementVC.h>
#include <iostream>
#include <map>
#include <string>
#include <vector>
#include <utils/PacketFactory.h>

#include "systemc.h"
#include <boost/asio.hpp>
#include <boost/property_tree/ptree.hpp>
//#include <boost/property_tree/ptree_serialization.hpp>
#include <boost/property_tree/json_parser.hpp>
//#include <boost/archive/binary_oarchive.hpp>
//#include <boost/serialization/serialization.hpp>
#include <zmq.hpp>

#include "traffic/Flit.h"
#include "utils/Structures.h"
#include "utils/GlobalResources.h"
#include "utils/Report.h"

#include "container/Container.h"
#include "router/Router.h"
#include "model/router/RouterVC.h"

#include "link/Link.h"
#include "networkInterface/NetworkInterfaceVC.h"

#include "model/traffic/synthetic/SyntheticPool.h"
#include "model/traffic/task/TaskPool.h"

class NoC : public sc_module {
private:
    int rc;
    void *context = zmq_ctx_new ();
    void *socket = zmq_socket (context, ZMQ_REP);
    int pipefds[2];
    zmq_pollitem_t items [2] = {NULL, NULL};
    boost::property_tree::ptree pt;

public:
    PacketFactory& packetFactory = PacketFactory::getInstance();

    SC_HAS_PROCESS(NoC);

    explicit NoC(sc_module_name);

    ~NoC() override;

    //This function was used to verify the credit counter. It works!
    //void verifyFlowControl();
    void guiServer();

private:
    GlobalResources& globalResources = GlobalResources::getInstance();
    Report& rep = Report::getInstance();
    int dbid;
    std::vector<std::unique_ptr<sc_clock>> clocks;
    std::unique_ptr<TrafficPool> tp;
    std::vector<NetworkParticipant*> networkParticipants;
    std::vector<std::unique_ptr<FlitSignalContainer>> flitSignalContainers;
    std::vector<std::unique_ptr<PacketSignalContainer>> packetSignalContainers;
    //std::vector<SignalContainer*> signalContainers;
    std::vector<std::unique_ptr<Link>> links;

    void createClocks();
    void createTrafficPool();
    void createNetworkParticipants();
    void createSigContainers();
    void createLinks(const std::vector<std::unique_ptr<sc_clock>>& clocks);
    void runNoC();
};

#define S_NOTIFY_MSG " "
#define S_ERROR_MSG "Error while writing to self-pipe.\n"
static int s_fd;
static void s_signal_handler (int signal_value)
{
    int rc = write (s_fd, S_NOTIFY_MSG, sizeof(S_NOTIFY_MSG));
    if (rc != sizeof(S_NOTIFY_MSG)) {
        write (STDOUT_FILENO, S_ERROR_MSG, sizeof(S_ERROR_MSG)-1);
        exit(1);
    }
}

static void s_catch_signals (int fd)
{
    s_fd = fd;

    struct sigaction action;
    action.sa_handler = s_signal_handler;
    //  Doesn't matter if SA_RESTART set because self-pipe will wake up zmq_poll
    //  But setting to 0 will allow zmq_read to be interrupted.
    action.sa_flags = 0;
    sigemptyset (&action.sa_mask);
    sigaction (SIGINT, &action, NULL);
    sigaction (SIGTERM, &action, NULL);
}
