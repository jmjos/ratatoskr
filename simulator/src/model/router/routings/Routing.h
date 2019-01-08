/*******************************************************************************
 * Copyright (C) 2018 joseph
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

#include "utils/GlobalResources.h"
#include "utils/Structures.h"
#include "model/traffic/Flit.h"
#include "model/traffic/Packet.h"
#include "utils/GlobalReport.h"
#include "utils/Report.h"
#include "model/container/Container.h"
#include "model/router/helper/Helper.h"
#include "model/router/helper/MeshHelper.h"

struct RoutingInformation {
    GlobalResources& globalResources = GlobalResources::getInstance();
    Node node;
    std::set<Channel> allChannel;
    std::set<Channel> allChannelWithoutLocal;
    std::vector<int> vcCount;
    std::map<Channel, Packet*> occupyTable;     // this structure of the occupy table is used with rrVC arbiter.
    std::map<Channel, Channel> fairOccupyTable; // this structure is used with fairArbiter.
    std::map<Channel, bool> freeVCs;
    std::map<Channel, Channel> routingStateTable;

    std::map<Channel, int> congestion;
    std::map<Channel, int> tagOut;
    std::map<Channel, int> tagIn;
    std::map<Channel, bool> flowIn;
    std::map<Channel, bool> emptyIn;

    explicit RoutingInformation(Node& node)
    {
        this->node = node;
        vcCount.resize(node.connections.size());
        for (connID_t con_id : node.connections) {
            Connection c = globalResources.connections.at(con_id);
            int conPos = node.getConnPosition(con_id);
            if (c.nodes.size()==1) {
                vcCount.at(conPos) = c.vcsCount.at(0);
            }
            else {
                for (int i = 0; i<c.vcsCount.size(); i++) {
                    if (i!=c.getNodePos(node.id)) {
                        vcCount.at(conPos) = c.vcsCount.at(i);
                        break;
                    }
                }
            }
            for (int vc = 0; vc<vcCount.at(conPos); vc++) {
                Channel ch = {conPos, vc};
                allChannel.insert(ch);
                if (c.nodes.size()!=1) {
                    allChannelWithoutLocal.insert(ch);
                    freeVCs[ch] = true;
                }
            }
        }
    };
};

struct RoutingPacketInformation {
    Packet* packet;
    Channel inputChannel;
    std::set<Channel> routedChannel;
    std::set<Channel> selectedChannel;
    std::set<Channel> recentSelectedChannel;
    std::map<Channel, float> routedChannelRating;
    std::map<Channel, float> selectedChannelRating;
    std::map<Channel, int> nextClass;
    Channel outputChannel;

    bool rerouteFlag = false;
    bool delayFlag = false;
    bool unableFlag = false;
    bool dropFlag = false;

    explicit RoutingPacketInformation(Packet* packet)
    {
        this->packet = packet;
        outputChannel = Channel(-1, -1);
    };
};

struct Routing {
    GlobalResources& globalResources = GlobalResources::getInstance();
    Report& rep = Report::getInstance();

    int dbid;
    Node node;

    explicit Routing(Node& node)
    {
        this->node = node;
        this->dbid = rep.registerElement("Routing", node.id);
    }

    virtual ~Routing() = default;

    virtual void checkValid() = 0;

    virtual void route(RoutingInformation*, RoutingPacketInformation*) = 0; //returns all allowed channel

    virtual void makeDecision(RoutingInformation*, RoutingPacketInformation*) = 0; //returns all allowed channel

    virtual void beginCycle(RoutingInformation*) = 0; //called at the beginning of each router cycle

    virtual void endCycle(RoutingInformation*) = 0; //called at the beginning of each router cycle
};
