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
/*
 * Routing.h
 *
 *  Created on: Jan 13, 2017
 *      Author: wrieden
 */

#ifndef SRC_MODEL_ROUTINGS_ROUTING_H_
#define SRC_MODEL_ROUTINGS_ROUTING_H_

#include "utils/GlobalInputClass.h"
#include "utils/Structures.h"
#include "model/traffic/Flit.h"
#include "model/traffic/Packet.h"
#include "utils/GlobalReportClass.h"
#include "utils/Report.h"
#include "model/container/Container.h"
#include "model/router/helper/Helper.h"
#include "model/router/helper/MeshHelper.h"

struct RoutingInformation {
	Node* node;
	std::set<Channel> allChannel;
	std::set<Channel> allChannelWithoutLocal;
	std::vector<int> vcCount;
	std::map<Channel, Packet*> occupyTable; // this structure of the occupy table is used with rrVC arbiter.
	std::map<Channel, Channel> fairOccupyTable; // this structure is used with fairArbiter.

	std::map<Channel, int> congestion;
	std::map<Channel, int> tagOut;
	std::map<Channel, int> tagIn;
	std::map<Channel, bool> flowIn;
	std::map<Channel, bool> emptyIn;

	RoutingInformation(Node* node) {
		this->node = node;
		vcCount.resize(node->connections.size());
		for (Connection* c : node->connections) {

			if (c->nodes.size() == 1) {
				vcCount.at(node->conToPos.at(c)) = c->vcCount.at(0);
			} else {
				for (int i = 0; i < c->vcCount.size(); i++) {
					if (i != c->nodePos.at(node)) {
						vcCount.at(node->conToPos.at(c)) = c->vcCount.at(i);
						break;
					}
				}
			}

			for (int vc = 0; vc < vcCount.at(node->conToPos.at(c)); vc++) {
				allChannel.insert(Channel(node->conToPos.at(c), vc));
				if (c->nodes.size() != 1) {
					allChannelWithoutLocal.insert(Channel(node->conToPos.at(c), vc));
				}
			}
		}
	}
	;
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

	bool rerouteFlag = 0;
	bool delayFlag = 0;
	bool unableFlag = 0;
	bool dropFlag = 0;

	RoutingPacketInformation(Packet* packet) {
		this->packet = packet;
		outputChannel = Channel(-1, -1);
	}
	;
};

struct Routing {
	GlobalInputClass& global = GlobalInputClass::getInstance();
	Report& rep = Report::getInstance();

	int dbid;
	Node* node;

	Routing(Node* node) {
		this->node = node;
		this->dbid = rep.registerElement("Routing", node->id);
	}

	virtual ~Routing() {
	}

	virtual void checkValid()=0;
	virtual void route(RoutingInformation*, RoutingPacketInformation*)=0; //returns all allowed channel
	virtual void makeDecision(RoutingInformation*, RoutingPacketInformation*)=0; //returns all allowed channel

	virtual void beginCycle(RoutingInformation*)=0; //called at the beginning of each router cycle
	virtual void endCycle(RoutingInformation*)=0; //called at the beginning of each router cycle

	//virtual std::set<Channel> select(RoutingInformation*)=0; //selects channel based on congestion etc
	//virtual Channel selectFinal(RoutingInformation*)=0; //selects output channel, directly called before arbiter, -1 halt, -2 reroute

};

#endif
