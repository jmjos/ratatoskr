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
///*
// * RoutingXYZ.cpp
// *
// *  Created on: Jan 15, 2017
// *      Author: wrieden
// */
//
//#include "RoutingESPADA.h"
//
//RoutingESPADA::RoutingESPADA(Node* node):Routing(node){
//	forbiddenTurns.push_back(MESH::getProhibitedTurns(node, MESH::ENULAST));
//	forbiddenTurns.push_back(MESH::getProhibitedTurns(node, MESH::ESDLAST));
//	forbiddenTurns.push_back(MESH::getProhibitedTurns(node, MESH::WSULAST));
//	forbiddenTurns.push_back(MESH::getProhibitedTurns(node, MESH::WNDLAST));
//	this->classcount=4;
//
//	for(int i= 0; i<node->connections.size(); i++){
//		for (int j = 0; j<node->connections.at(i)->getVCCountForNode(node); j++){
//			tagLocal[Channel(i,j)]={100,0};
//		}
//	}
//
//}
//
//RoutingESPADA::~RoutingESPADA() {
//
//}
//
//void RoutingESPADA::checkValid() {
//	assert(node->connectedNodes.size() <= 6);
//	assert(node->connections.size() <= node->connectedNodes.size() + 1);
//	assert(node->dirToCon.size() == node->connections.size());
//
//	int i=0;
//	for(std::pair<DIR::TYPE, int> pair : node->dirToCon){
//		assert(std::find(DIR::XYZ.begin(), DIR::XYZ.end(), pair.first) != DIR::XYZ.end());
//		i++;
//	}
//	assert(node->connections.size() == i);
//
//
//	std::set<Node*> connected;
//	std::set<Node*> newconnected;
//
//	newconnected.insert(node);
//
//	while(!newconnected.empty()){
//		std::set<Node*> nextnewconnected;
//
//		for(Node* n: newconnected){
//			connected.insert(n);
//
//			for(int i=0; i<n->connectedNodes.size();i++){
//				if(!connected.count(n->connectedNodes.at(i))){
//					nextnewconnected.insert(n->connectedNodes.at(i));
//				}
//			}
//		}
//
//		newconnected = nextnewconnected;
//	}
//
//	if(connected.size()!= globalResources.nodes.size()){
//		rep.reportEvent(dbid, "invalid_topology", std::to_string(node->id));
//		//remove(globalResources.inputNoc.c_str());
//		FATAL("Not Fully Connected!");
//	}else{
//		//FATAL("Fully Connected!")
//	}
//
//}
//
//void RoutingESPADA::route(RoutingInformation* ri, RoutingPacketInformation* rpi) {
//	//rep.reportEvent(dbid, "routing_route_packet", std::to_string(rpi->packet->id));
//
//	std::set<Channel> channel;
//	std::map<Channel, float> channelRating;
//	std::map<Channel, int> nextClass;
//
//
//	Vec3D<float> pos = ri->node->pos;
//	Vec3D<float> dstPos = globalResources.nodes.at(globalResources.getIdByPe(rpi->packet->dst))->pos;
//
//	if (pos == dstPos) {
//		channel= {Channel(node->dirToCon.at(DIR::Local), 0)};
//		channelRating[Channel(node->dirToCon.at(DIR::Local), 0)] = 1;
//		nextClass[Channel(node->dirToCon.at(DIR::Local), 0)] = classcount-1;
//	} else {
//
//		if (ri->node->conToDir.at(rpi->inputChannel.dir) == DIR::Local) {
//			if ((pos.x <= dstPos.x && pos.y <= dstPos.y && pos.z <= dstPos.z)
//					|| (pos.x >= dstPos.x && pos.y >= dstPos.y
//							&& pos.z >= dstPos.z)) {
//				rpi->packet->pkgclass = 0;
//			}
//			else if ((pos.x <= dstPos.x && pos.y >= dstPos.y && pos.z >= dstPos.z)
//					|| (pos.x >= dstPos.x && pos.y <= dstPos.y
//							&& pos.z <= dstPos.z)) {
//				rpi->packet->pkgclass = 1;
//			}
//			else if ((pos.x >= dstPos.x && pos.y >= dstPos.y && pos.z <= dstPos.z)
//					|| (pos.x <= dstPos.x && pos.y <= dstPos.y
//							&& pos.z >= dstPos.z)) {
//				rpi->packet->pkgclass = 2;
//			}
//			else if ((pos.x >= dstPos.x && pos.y <= dstPos.y && pos.z >= dstPos.z)
//					|| (pos.x <= dstPos.x && pos.y >= dstPos.y
//							&& pos.z <= dstPos.z)) {
//				rpi->packet->pkgclass = 3;
//			} else {
//				FATAL("meh");
//			}
//			rpi->packet->pkgclass = 0;
//		}
//
//
//		int pkgclass = rpi->packet->pkgclass;
//		Channel inputChannel = rpi->inputChannel;
//
//
//
//		for (int i = 0; i < this->classcount; i++) {
//		//for (int i = pkgclass; i < this->classcount; i++) {
//		//for (int i = 0; i < 1; i++) {
//			std::set<Channel> classchannel = MESH::getChannelWithVC( {0}, ri->allChannelWithoutLocal);
//			classchannel = MESH::filterTurnsDir(forbiddenTurns.at(pkgclass), inputChannel, classchannel);
//
//			std::set<int> dirs;
//			for (Channel c: classchannel) {
//				std::set<Channel> channelForDir = checkValidChannelForDir(ri, c.dir, pkgclass);
//				if(!channelForDir.empty()) {
//					dirs.insert(c.dir);
//				}
//			}
//
//			classchannel = MESH::getChannelWithDir(dirs, classchannel);
//
//			//channel = MESH::getChannelWithVC( { pkgclass }, channel);
//			//channel = MESH::getMinimalChannel(node, rpi->packet, channel);
//
//
//
//
//			//channel = ri->allChannelWithoutLocal;
//
//			for (Channel c : classchannel) {
//				if(!channelRating.count(c)) {
//					channelRating[c] = 1-(0.1*i);
//				}
//				if(!nextClass.count(c)) {
//					nextClass[c] = pkgclass;
//				}
//			}
//
//			channel.insert(classchannel.begin(),classchannel.end());
//
//			pkgclass= (pkgclass+1)%classcount;
//			inputChannel = Channel(ri->node->dirToCon.at(DIR::Local),0);
//		}
//	}
//
//	if (channel.empty()) {
//		FATAL("Unable to route! R:"<<ri->node->id<<*rpi->packet);
//	}
//
//	rpi->routedChannel = channel;
//	rpi->routedChannelRating = channelRating;
//	rpi->nextClass = nextClass;
//
//}
//
//void RoutingESPADA::makeDecision(RoutingInformation* ri, RoutingPacketInformation* rpi) {
//	if(rpi->recentSelectedChannel.size()){
//		Channel selectedChannel = *(rpi->recentSelectedChannel.begin());
//		//rpi->outputChannel = selectedChannel;
//
//
//		int selectedDir = selectedChannel.dir;
//		int selectedClass = rpi->nextClass.at(selectedChannel);//rpi->packet->pkgclass;
//
//
//		if(ri->node->conToDir.at(selectedDir)== DIR::Local){
//			rpi->outputChannel = {selectedDir,0};
//		}else{
//			std::set<Channel> classFilteredChannel = checkValidChannelForDir(ri, selectedDir, selectedClass);
//			//classFilteredChannel = MESH::filterTurnsDir(forbiddenTurns.at(selectedClass), rpi->inputChannel, classFilteredChannel);
//
//
//			if(classFilteredChannel.empty()){
//				//selectedClass = rpi->nextClass.at(selectedChannel);
//				//classFilteredChannel = checkValidChannelForDir(ri, selectedDir, selectedClass);
//
//				//if(classFilteredChannel.empty()){
//					rpi->rerouteFlag = true;
//					return;
//				//}
//			}
//
//			std::set<Channel> candidates = classFilteredChannel;
////			for(Channel c: classFilteredChannel){
////				if(!ri->occupyTable.count(c)){
////					candidates.insert(c);
////				}
////			}
////
////			if(candidates.empty()){
////				rpi->delayFlag = true;
////				return;
////			}
//
//			candidates = MESH::getChannelWithLeastCongestion(ri->node, 10, candidates);
//			rpi->outputChannel =*(candidates.begin());
//
//
//			if(selectedClass<rpi->packet->pkgclass){
//				ri->tagOut.at(rpi->inputChannel) = -1;
//			}
//
//			rpi->packet->pkgclass = selectedClass;
//			tagLocal.at(rpi->outputChannel) = {rpi->packet->pkgclass, 5};
//
//
//		}
//
//	}else{
//		//FATAL("Router"<<ri->node->id<<"["<<DIR::toString(ri->node->conToDir.at(rpi->inputChannel.dir))<<rpi->inputChannel.vc<<"] - Unable to make decision! "<<*rpi->packet);
//	}
//
//
//}
//
//
//void RoutingESPADA::beginCycle(RoutingInformation* ri){
//	for(Channel channel: ri->allChannelWithoutLocal){
//		if(tagLocal.at(channel).second && ri->tagIn.at(channel)!=-1){
//			ri->tagIn.at(channel) = tagLocal.at(channel).first;
//		}
//
//		if(tagLocal.at(channel).second){
//			tagLocal.at(channel).second--;
//		}
//	}
//}
//
//void RoutingESPADA::endCycle(RoutingInformation* ri){
//	for(Channel channel: ri->allChannelWithoutLocal){
//		Connection* con = node->connections.at(channel.dir);
//		if(con->vcBufferUtilization.at(con->nodePos.at(ri->node)).at(channel.vc) == 0){
//			ri->tagOut.at(channel)=this->classcount-1;
//		}
//	}
//
//	//set flow control flag
//
////					//set tag
////				if(buffer.at(dir)->at(vc)->empty()){
////					tagOut.at(dir)->at(vc) = 1;
////					rInfo->tagOut[Channel(dir,vc)] = 1;
////				}else{
////					tagOut.at(dir)->at(vc)= rInfo->tagOut.at(Channel(dir,vc));
////				}
////					if (c->congestion.at(c->nodePos.at(node)).at(vc) >= 15){
////					LOG(false, "Router" << id << "[" << DIR::toString(node->conToDir[dir]) << vc << "]\t- BUFFER " << c->congestion.at(c->nodePos.at(node)).at(vc));
////				}
//}
//
//
//std::set<Channel> RoutingESPADA::checkValidChannelForDir(RoutingInformation* ri, int dir, int pkgclass) {
//	std::set<Channel> channel = MESH::getChannelWithDir({dir}, ri->allChannel);
//	bool containNegative = false;
//
//	for(Channel c: channel){
//		if (ri->tagIn.at(c) == -1) {
//			containNegative = true;
//		}
//	}
//
//	bool failed = false;
//	std::set<Channel> filteredChannel = channel;
//
//	if (!containNegative) {
//		for (int i = classcount - 1; i != pkgclass; i--) {
//			bool succ = false;
//			for (Channel c : channel) {
//				if (ri->tagIn.at(c) >= i && filteredChannel.count(c)) {
//					filteredChannel.erase(c);
//					succ = true;
//					break;
//				}
//			}
//			if (!succ) {
//				failed = true;
//				break;
//			}
//		}
//
//		if (failed) {
//			channel.clear();
//			return channel;
//		}
//
//	} else {
//		//FATAL("Negative!");
//	}
//
//	channel = filteredChannel;
//
//	for(Channel c: filteredChannel){
//		if (ri->tagIn.at(c) < pkgclass) {
//			channel.erase(c);
//		}
//	}
//
//	return channel;
//}
