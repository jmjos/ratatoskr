////////////////////////////////////////////////////////////////////////////////
// Copyright (C) 2018 joseph
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
/*
 * Helper.cpp
 *
 *  Created on: May 23, 2017
 *      Author: wrieden
 */
#include "MeshHelper.h"

std::set<Channel> MeshHelper::getMinimalChannel(Node* curr, Node* dst,
		std::set<Channel> availableChannel) {
	std::set<Channel> channel;
	std::set<int> dirs;
	Vec3D<float> dstPos= dst->pos;
	Vec3D<float> dist = dstPos - curr->pos;
	for (Node* n : curr->connectedNodes) {
		Vec3D<float> ndist = dstPos - n->pos;
		if (ndist < dist) {
			for (int dir : curr->connectionsToNode.at(n)) {
				dirs.insert(dir);
			}
		}
	}

	if (curr->pos == dst->pos) {
		dirs.insert(curr->dirToCon.at(DIR::Local));
	}

	for (Channel c : availableChannel) {
		if (dirs.count(c.dir)) {
			channel.insert(c);
		}
	}
	return channel;
}


int MeshHelper::getCongestionAlong(Node* curr,
		int levelCount,bool perVc, Channel c) {


		DIR::TYPE dir=curr->conToDir.at(c.dir);
		uint32_t mostCongestion = 0;

		for(Node* n = curr; n->dirToCon.count(dir) && dir!=DIR::Local;){
			Connection* con = n->connections.at(n->dirToCon.at(dir));
			int nodePos=0;

			for(std::pair<Node*, int> pos: con->nodePos){
				if(pos.first!=n){
					nodePos = pos.second;
				}
			}
			int congestion = 0;
			int bufferSize = con->bufferDepth.at(nodePos);

			if(perVc){
				congestion = con->vcBufferUtilization.at(nodePos).at(c.vc);
				congestion = (float) congestion / (float) bufferSize * levelCount;
			}else{
				for(int cong: con->vcBufferUtilization.at(nodePos)){
					congestion+=cong;
				}
				bufferSize *= con->vcCount.at(nodePos);
				congestion = (float) congestion / (float) bufferSize * levelCount;
			}

			if (congestion == levelCount) {
				congestion--;
			}
			if (congestion >= mostCongestion) {
					mostCongestion = congestion;
			}
			//cout<<n->id<<DIR::toString(dir)<<c.vc<<" "<<congestion<<endl;
			n = con->nodes.at(nodePos);
		}


		//cout<<"all"<<DIR::toString(dir)<<c.vc<<" "<<mostCongestion<<endl;

	//cout<<leastCongestion<<endl;


	return mostCongestion;
}


std::set<Channel> MeshHelper::getChannelWithLeastCongestionAlong(Node* curr,
		int levelCount, bool perVc,std::set<Channel> availableChannel) {
	std::set<Channel> channel;
	uint32_t leastCongestion = -1;
	std::map<Channel, uint32_t> candidates;

	for (Channel c : availableChannel) {
		DIR::TYPE dir=curr->conToDir.at(c.dir);
		uint32_t mostCongestion = 0;

		for(Node* n = curr; n->dirToCon.count(dir) && dir!=DIR::Local;){
			Connection* con = n->connections.at(n->dirToCon.at(dir));
			int nodePos=0;

			for(std::pair<Node*, int> pos: con->nodePos){
				if(pos.first!=n){
					nodePos = pos.second;
				}
			}
			int congestion = 0;
			int bufferSize = con->bufferDepth.at(nodePos);

			if(perVc){
				congestion = con->vcBufferUtilization.at(nodePos).at(c.vc);
				congestion = (float) congestion / (float) bufferSize * levelCount;
			}else{
				for(int cong: con->vcBufferUtilization.at(nodePos)){
					congestion+=cong;
				}
				bufferSize *= con->vcCount.at(nodePos);
				congestion = (float) congestion / (float) bufferSize * levelCount;
			}

			if (congestion == levelCount) {
				congestion--;
			}
			if (congestion >= mostCongestion) {
					mostCongestion = congestion;
			}
			//cout<<n->id<<DIR::toString(dir)<<c.vc<<" "<<congestion<<endl;
			n = con->nodes.at(nodePos);
		}
		if (mostCongestion <= leastCongestion) {
			leastCongestion = mostCongestion;
			candidates[c]=mostCongestion;
		}
		//cout<<"all"<<DIR::toString(dir)<<c.vc<<" "<<mostCongestion<<endl;
	}
	//cout<<leastCongestion<<endl;
	for (std::pair<Channel, int> candidate : candidates) {
		if (candidate.second == leastCongestion) {
			channel.insert(candidate.first);
		}
	}

	return channel;
}


float MeshHelper::getCongestionRelative(Node* curr, std::vector<std::pair<DIR::TYPE, int>> path) {
	float congestion = 0;
	bool fault = false;
	bool end = false;
	Node* n = curr;

	for (std::pair<DIR::TYPE, int> next : path) {
		DIR::TYPE dir = next.first;
		int weight = next.second;

		if(n->dirToCon.count(dir)){
			int dirNum = n->dirToCon.at(dir);
			Connection* con = n->connections.at(dir);
			Node* nextNode = n->conToNode.at(dir);
			int nodeNum = con->nodePos.at(nextNode);
			//int vcCount = con->vcCount.at(nodeNum);

			congestion = con->bufferCongestion.at(nodeNum);
			n= nextNode;
		}else{
			if((n->pos.x=0 && dir==DIR::West)||(n->pos.x=1 && dir==DIR::East)||
				(n->pos.y=0 && dir==DIR::South)||(n->pos.y=1 && dir==DIR::North)||
				(n->pos.z=0 && dir==DIR::Down)||(n->pos.z=1 && dir==DIR::Up) ||
				dir == DIR::Local){
				return congestion;
			}else{
				return -1;
			}
		}
	}
	return congestion;
}

float MeshHelper::getLeastMinimalCongestionToNode(Node* curr, Node* dst, Channel channel, int hop, int maxHops, float congestion, float factor, float decrease) {
	if(curr->conToDir.at(channel.dir) == DIR::Local){
		return 0;
	}

	Connection* con = curr->connections.at(channel.dir);
	Node* nextNode = curr->conToNode.at(channel.dir);
	int nodeNum = con->nodePos.at(nextNode);
	std::set<Channel> nextNodeMinimalChannel = getMinimalChannel(nextNode, dst, getAllChannel(curr));

	if(nextNodeMinimalChannel.empty() && nextNode != dst && hop<maxHops){
		return 1000;
	}


	if(!hop){
		congestion = con->bufferCongestion.at(nodeNum);
	}else{
		congestion += con->bufferCongestion.at(nodeNum) * factor;
	}

	if(hop<maxHops && nextNode != dst){
		float minNextCongestion = 100;
		for(Channel c: nextNodeMinimalChannel){
			float nextCongestion = getLeastMinimalCongestionToNode(nextNode, dst, c, hop+1, maxHops, congestion, factor*decrease, decrease);
			if(minNextCongestion>nextCongestion){
				minNextCongestion = nextCongestion;
			}
		}
		return minNextCongestion;
	}else{
		return congestion;
	}
}

std::map<Channel, float> getChannelWithTraversedCongestion(Node* curr, Node* dst, std::set<Channel> channel, int maxHops, float decrease) {
	std::map<Channel, float> map;
	for (Channel c : channel) {
		map[c] = MeshHelper::getLeastMinimalCongestionToNode(curr, dst, c, 0, maxHops, 0, 1, decrease);

	}
	return map;
}

int MeshHelper::getHopDistance(Node* n1, Node* n2){
	Vec3D<int> n1Pos = GlobalInputClass::getInstance().idToScPos.at(n1->id);
	Vec3D<int> n2Pos = GlobalInputClass::getInstance().idToScPos.at(n2->id);

	return (abs(n1Pos.x-n2Pos.x)+
			abs(n1Pos.y-n2Pos.y)+
			abs(n1Pos.z-n2Pos.z));
}


//exclude dir??
ChannelInfo MeshHelper::getRecursiveChannelInfo(ChannelInfo info) {
	if(info.currNode == info.destinationNode || info.hop >= info.maxHops){
		info.minimalCongestion = info.congestion;
		info.averageCongestion = info.congestion;
		info.minPathToDestDistance = getHopDistance(info.currNode, info.destinationNode);
		info.maxDistanceToOriginWithMinPathToDest = getHopDistance(info.currNode, info.sourceNode);
		info.availablePaths = 1;

		if(info.onMinimalPath){
			info.minimalPaths=1;
		}else{
			info.nonminimalPaths=1;
		}

		return info;
	}


	Connection* con = info.currNode->connections.at(info.channel.dir);
	Node* connectedNode = info.currNode->conToNode.at(info.channel.dir);
	int nodeNum = con->nodePos.at(connectedNode);
	int oldDistanceToDest = getHopDistance(info.currNode, info.destinationNode);
	int newDistanceToDest = getHopDistance(connectedNode, info.destinationNode);

	if(newDistanceToDest>=oldDistanceToDest){
		info.onMinimalPath = false;
		info.nonMinimalCount++;
		//return info;
	}



	std::set<Channel> channel = getAllChannelWithoutLocal(connectedNode);
	channel = MeshHelper::getChannelWithVC({0}, channel);

//	if(!info.forbiddenTurns.empty()){
//		channel = MeshHelper::filterTurns(info.forbiddenTurns,info.channel, channel);
//	}


	//std::set<Channel> minimalChannel = getMinimalChannel(connectedNode, info.destinationNode, channel);

	if(channel.empty() && connectedNode != info.destinationNode && info.hop+1 < info.maxHops){
		info.availablePaths = 0;
		info.minimalPaths=0;
		info.nonminimalPaths=0;
		info.minimalCongestion = -1;
		info.averageCongestion = -1;
		info.minPathToDestDistance = -1;
		info.maxDistanceToOriginWithMinPathToDest = -1;
		return info;
	}

	info.congestion += connectedNode->congestion*info.congestionFactor;//con->bufferCongestion.at(nodeNum) * info.congestionFactor;
	info.currNode = connectedNode;
	info.congestionFactor *= info.decreaseFactor;
	info.hop++;

	ChannelInfo result(info.sourceNode, info.destinationNode, info.channel, info.maxHops);
	if (connectedNode == info.destinationNode || info.hop >= info.maxHops) {
		result.combine(getRecursiveChannelInfo(info));
	} else {
		for (Channel c : channel) {
			info.channel = c;
			result.combine(getRecursiveChannelInfo(info));
		}
	}

	return result;
}

std::map<Channel, ChannelInfo> MeshHelper::getChannelInfo(Node* curr, Node* dst, std::set<Channel> channel, int maxHops, float decrease) {
	std::map<Channel, ChannelInfo> map;
	for (Channel c : channel) {
		ChannelInfo info(curr, dst, c, maxHops);
		info.decreaseFactor = decrease;
		map[c] = getRecursiveChannelInfo(info);

	}
	return map;
}

std::set<std::pair<int,int>> MeshHelper::getProhibitedTurns(Node* node, TURNMODEL tm){
	std::set<std::pair<int,int>> forbiddenTurns;
	//ENULAST, ESDLAST, WSULAST, WNDLAST
	switch(tm){
	case NEGATIVEFIRST:
	case ENULAST:
		forbiddenTurns.insert({node->dirToCon.count(DIR::West)?node->dirToCon.at(DIR::West):-1,node->dirToCon.count(DIR::South)?node->dirToCon.at(DIR::South):-1});
		forbiddenTurns.insert({node->dirToCon.count(DIR::West)?node->dirToCon.at(DIR::West):-1,node->dirToCon.count(DIR::Down)?node->dirToCon.at(DIR::Down):-1});
		forbiddenTurns.insert({node->dirToCon.count(DIR::South)?node->dirToCon.at(DIR::South):-1,node->dirToCon.count(DIR::West)?node->dirToCon.at(DIR::West):-1});
		forbiddenTurns.insert({node->dirToCon.count(DIR::South)?node->dirToCon.at(DIR::South):-1,node->dirToCon.count(DIR::Down)?node->dirToCon.at(DIR::Down):-1});
		forbiddenTurns.insert({node->dirToCon.count(DIR::Down)?node->dirToCon.at(DIR::Down):-1,node->dirToCon.count(DIR::West)?node->dirToCon.at(DIR::West):-1});
		forbiddenTurns.insert({node->dirToCon.count(DIR::Down)?node->dirToCon.at(DIR::Down):-1,node->dirToCon.count(DIR::South)?node->dirToCon.at(DIR::South):-1});

		forbiddenTurns.insert({node->dirToCon.count(DIR::West)?node->dirToCon.at(DIR::West):-1,node->dirToCon.count(DIR::West)?node->dirToCon.at(DIR::West):-1});
		forbiddenTurns.insert({node->dirToCon.count(DIR::South)?node->dirToCon.at(DIR::South):-1,node->dirToCon.count(DIR::South)?node->dirToCon.at(DIR::South):-1});
		forbiddenTurns.insert({node->dirToCon.count(DIR::Down)?node->dirToCon.at(DIR::Down):-1,node->dirToCon.count(DIR::Down)?node->dirToCon.at(DIR::Down):-1});

		forbiddenTurns.erase({-1,-1});
		break;
	case ESDLAST:
		forbiddenTurns.insert({node->dirToCon.count(DIR::West)?node->dirToCon.at(DIR::West):-1,node->dirToCon.count(DIR::North)?node->dirToCon.at(DIR::North):-1});
		forbiddenTurns.insert({node->dirToCon.count(DIR::West)?node->dirToCon.at(DIR::West):-1,node->dirToCon.count(DIR::Up)?node->dirToCon.at(DIR::Up):-1});
		forbiddenTurns.insert({node->dirToCon.count(DIR::North)?node->dirToCon.at(DIR::North):-1,node->dirToCon.count(DIR::West)?node->dirToCon.at(DIR::West):-1});
		forbiddenTurns.insert({node->dirToCon.count(DIR::North)?node->dirToCon.at(DIR::North):-1,node->dirToCon.count(DIR::Up)?node->dirToCon.at(DIR::Up):-1});
		forbiddenTurns.insert({node->dirToCon.count(DIR::Up)?node->dirToCon.at(DIR::Up):-1,node->dirToCon.count(DIR::West)?node->dirToCon.at(DIR::West):-1});
		forbiddenTurns.insert({node->dirToCon.count(DIR::Up)?node->dirToCon.at(DIR::Up):-1,node->dirToCon.count(DIR::North)?node->dirToCon.at(DIR::North):-1});

		forbiddenTurns.insert({node->dirToCon.count(DIR::West)?node->dirToCon.at(DIR::West):-1,node->dirToCon.count(DIR::West)?node->dirToCon.at(DIR::West):-1});
		forbiddenTurns.insert({node->dirToCon.count(DIR::North)?node->dirToCon.at(DIR::North):-1,node->dirToCon.count(DIR::North)?node->dirToCon.at(DIR::North):-1});
		forbiddenTurns.insert({node->dirToCon.count(DIR::Up)?node->dirToCon.at(DIR::Up):-1,node->dirToCon.count(DIR::Up)?node->dirToCon.at(DIR::Up):-1});

		forbiddenTurns.erase({-1,-1});
		break;
	case WSULAST:
		forbiddenTurns.insert({node->dirToCon.count(DIR::East)?node->dirToCon.at(DIR::East):-1,node->dirToCon.count(DIR::North)?node->dirToCon.at(DIR::North):-1});
		forbiddenTurns.insert({node->dirToCon.count(DIR::East)?node->dirToCon.at(DIR::East):-1,node->dirToCon.count(DIR::Down)?node->dirToCon.at(DIR::Down):-1});
		forbiddenTurns.insert({node->dirToCon.count(DIR::North)?node->dirToCon.at(DIR::North):-1,node->dirToCon.count(DIR::East)?node->dirToCon.at(DIR::East):-1});
		forbiddenTurns.insert({node->dirToCon.count(DIR::North)?node->dirToCon.at(DIR::North):-1,node->dirToCon.count(DIR::Down)?node->dirToCon.at(DIR::Down):-1});
		forbiddenTurns.insert({node->dirToCon.count(DIR::Down)?node->dirToCon.at(DIR::Down):-1,node->dirToCon.count(DIR::East)?node->dirToCon.at(DIR::East):-1});
		forbiddenTurns.insert({node->dirToCon.count(DIR::Down)?node->dirToCon.at(DIR::Down):-1,node->dirToCon.count(DIR::North)?node->dirToCon.at(DIR::North):-1});

		forbiddenTurns.insert({node->dirToCon.count(DIR::East)?node->dirToCon.at(DIR::East):-1,node->dirToCon.count(DIR::East)?node->dirToCon.at(DIR::East):-1});
		forbiddenTurns.insert({node->dirToCon.count(DIR::North)?node->dirToCon.at(DIR::North):-1,node->dirToCon.count(DIR::North)?node->dirToCon.at(DIR::North):-1});
		forbiddenTurns.insert({node->dirToCon.count(DIR::Down)?node->dirToCon.at(DIR::Down):-1,node->dirToCon.count(DIR::Down)?node->dirToCon.at(DIR::Down):-1});

		forbiddenTurns.erase({-1,-1});
		break;
	case WNDLAST:
		forbiddenTurns.insert({node->dirToCon.count(DIR::East)?node->dirToCon.at(DIR::East):-1,node->dirToCon.count(DIR::South)?node->dirToCon.at(DIR::South):-1});
		forbiddenTurns.insert({node->dirToCon.count(DIR::East)?node->dirToCon.at(DIR::East):-1,node->dirToCon.count(DIR::Up)?node->dirToCon.at(DIR::Up):-1});
		forbiddenTurns.insert({node->dirToCon.count(DIR::South)?node->dirToCon.at(DIR::South):-1,node->dirToCon.count(DIR::East)?node->dirToCon.at(DIR::East):-1});
		forbiddenTurns.insert({node->dirToCon.count(DIR::South)?node->dirToCon.at(DIR::South):-1,node->dirToCon.count(DIR::Up)?node->dirToCon.at(DIR::Up):-1});
		forbiddenTurns.insert({node->dirToCon.count(DIR::Up)?node->dirToCon.at(DIR::Up):-1,node->dirToCon.count(DIR::East)?node->dirToCon.at(DIR::East):-1});
		forbiddenTurns.insert({node->dirToCon.count(DIR::Up)?node->dirToCon.at(DIR::Up):-1,node->dirToCon.count(DIR::South)?node->dirToCon.at(DIR::South):-1});

		forbiddenTurns.insert({node->dirToCon.count(DIR::East)?node->dirToCon.at(DIR::East):-1,node->dirToCon.count(DIR::East)?node->dirToCon.at(DIR::East):-1});
		forbiddenTurns.insert({node->dirToCon.count(DIR::South)?node->dirToCon.at(DIR::South):-1,node->dirToCon.count(DIR::South)?node->dirToCon.at(DIR::South):-1});
		forbiddenTurns.insert({node->dirToCon.count(DIR::Up)?node->dirToCon.at(DIR::Up):-1,node->dirToCon.count(DIR::Up)?node->dirToCon.at(DIR::Up):-1});

		forbiddenTurns.erase({-1,-1});
		break;
	default:
		FATAL("Turn model not implemented!");
	}

	return forbiddenTurns;
}
