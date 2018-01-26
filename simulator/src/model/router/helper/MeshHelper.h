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
 * Helper.h
 *
 *  Created on: May 23, 2017
 *      Author: wrieden
 */

#ifndef SRC_MODEL_ROUTINGS_MESH_HELPER_H_
#define SRC_MODEL_ROUTINGS_MESH_HELPER_H_

#include "utils/GlobalInputClass.h"
#include "utils/Structures.h"
#include "model/traffic/Flit.h"
#include "model/traffic/Packet.h"
#include "utils/GlobalReportClass.h"
#include "utils/Report.h"
#include "model/container/Container.h"
#include "Helper.h"


struct ChannelInfo{
	Node* sourceNode = 0;
	Node* destinationNode = 0;
	Node* currNode = 0;
	Channel channel;
	std::set<DIR::TYPE> forbiddenDir;
	std::set<std::pair<Channel,Channel>> forbiddenTurns;
	int hop = 0;
	int maxHops = 0;
	float congestion = 0;
	float decreaseFactor = 1;
	float congestionFactor = 1;
	bool onMinimalPath = 1;
	int nonMinimalCount = 0;

	int availablePaths = 0;
	int minimalPaths = 0;
	int nonminimalPaths = 0;

	float minimalCongestion = -1;
	float averageCongestion = -1;
	int minPathToDestDistance = -1;
	int maxDistanceToOriginWithMinPathToDest = -1;

	ChannelInfo(){

	}

	ChannelInfo(Node* src, Node* dst, Channel channel, int maxHops){
		this->sourceNode = src;
		this->destinationNode = dst;
		this->channel = channel;
		this->currNode = src;
		this->maxHops = maxHops;
	}

	void combine(ChannelInfo info){
		availablePaths += info.availablePaths;
		minimalPaths += info.minimalPaths;
		nonminimalPaths += info.nonminimalPaths;

		averageCongestion = (averageCongestion+info.averageCongestion) / 2;

		if(info.onMinimalPath){
			if(minimalCongestion == -1 || (minimalCongestion > info.minimalCongestion && info.minimalCongestion != -1)){
				minimalCongestion = info.minimalCongestion;
			}


		}
		if(minPathToDestDistance == -1 || (minPathToDestDistance > info.minPathToDestDistance && info.minPathToDestDistance != -1)){
				//nonMinimalCount = info.nonMinimalCount;
				minPathToDestDistance = info.minPathToDestDistance;

				//if(info.maxDistanceToOriginWithMinPathToDest != -1){
					maxDistanceToOriginWithMinPathToDest = info.maxDistanceToOriginWithMinPathToDest;
				//}
		}

	}
};


class MeshHelper: public Helper {
public:
	enum TURNMODEL {NEGATIVEFIRST, NORTHLAST, SOUTHLAST, ENULAST, ESDLAST, WSULAST, WNDLAST};

	static std::set<Channel> getMinimalChannel(Node* curr, Node* dst, std::set<Channel> availableChannel);
	static std::set<Channel> getChannelWithLeastCongestionAlong(Node* curr, int levelCount, bool perVc,std::set<Channel> availableChannel);
	static int getCongestionAlong(Node* curr, int levelCount,bool perVc, Channel c);
	static float getCongestionRelative(Node* curr, std::vector<std::pair<DIR::TYPE, int>> path);
	static float getLeastMinimalCongestionToNode(Node* curr, Node* dst, Channel channel, int hop, int maxHops, float congestion, float factor, float decrease);
	static ChannelInfo getRecursiveChannelInfo(ChannelInfo info);
	static int getHopDistance(Node* n1, Node* n2);
	static std::map<Channel, ChannelInfo> getChannelInfo(Node* curr, Node* dst, std::set<Channel> channel, int maxHops, float decrease);
	static std::set<std::pair<int,int>> getProhibitedTurns(Node* ,TURNMODEL);

private:
	MeshHelper();
};

#endif /* SRC_MODEL_ROUTINGS_MESH_HELPER_H_ */
