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
///*
// * SelectionRoundRobin.h
// *
// *  Created on: Jun 5, 2017
// *      Author: wrieden
// */
//
//#ifndef SRC_MODEL_SELECTION_SELECTIONAGRA_H_
//#define SRC_MODEL_SELECTION_SELECTIONAGRA_H_
//
//#include "Selection.h"
//
//struct SelectionAgRA: public Selection {
//	int rrVC = 0;
//
//	SelectionAgRA(Node* node) :
//			Selection(node) {
//
//	}
//	;
//	~SelectionAgRA() {
//
//	}
//	;
//	//void checkValid()=0;
//	void select(RoutingInformation* ri, RoutingPacketInformation* rpi) {
//		std::set<Channel> channel = rpi->routedChannel;
//
//		if (!channel.empty()) {
//			channel = MESH::getMinimalChannel(node, global.nodes.at(rpi->packet->dst), channel);
//			//FATALCASE(channel.empty(), "Unable to select1! R:"<<ri->node->id<<*rpi->packet);
//
//			channel = MESH::getChannelWithRating(0.5, 1, channel, rpi->routedChannelRating);
//			//FATALCASE(channel.empty(), "Unable to select2! R:"<<ri->node->id<<*rpi->packet);
//
//			channel = MESH::getChannelWithHighestRating(channel, rpi->routedChannelRating);
//			//FATALCASE(channel.empty(), "Unable to select3! R:"<<ri->node->id<<*rpi->packet);
//
//			Vec3D<int> currPos = global.idToScPos.at(ri->node->id);
//			Vec3D<int> dstPos = global.idToScPos.at(rpi->packet->dst);
//
//			Vec3D<int> currClusterPos(currPos.x % 2, currPos.x % 2, currPos.x % 2);
//			Vec3D<int> dstClusterPos(dstPos.x % 2, dstPos.x % 2, dstPos.x % 2);
//
////			float minCongestion = 1000;
////			std::map<Channel, float> selectedChannel;
////
//
////			channel = selectedChannel;
////			float
////
//			float minCongestion = 1000;
//			std::map<Channel, float> selectedChannel;
//
//			int diffClusterDim = currClusterPos.diffDimCount(dstClusterPos);
////
//			if (diffClusterDim == 0) {
//				channel = MESH::getChannelWithLeastCongestion(node, 10, channel);
//			} else if (diffClusterDim == 1) {
//				for (Channel c : channel) {
//					float congestion = MESH::getLeastMinimalCongestionToNode(ri->node, global.nodes.at(rpi->packet->dst), c, 0, 2, 0, 1, 0.5);
//
//					if (congestion <= minCongestion) {
//						//LOG(true, congestion);
//						minCongestion = congestion;
//						selectedChannel[c] = congestion;
//					}
//				}
//
//				channel.clear();
//				for (std::pair<Channel, float> c : selectedChannel) {
//					if (c.second == minCongestion) {
//						channel.insert(c.first);
//					}
//				}
//
//			} else if (diffClusterDim >= 2) {
//				for (Channel c : channel) {
//					Connection* con = node->connections.at(c.dir);
//					Node* nextNode = node->conToNode.at(c.dir);
//					float congestion = nextNode->congestion*2;//con->bufferCongestion.at(con->nodePos.at(nextNode));
//
//					DIR::TYPE dir = node->conToDir.at(c.dir);
//					if(nextNode->dirToCon.count(dir)){
//					Node* nextNextNode = nextNode->conToNode.at(nextNode->dirToCon.at(dir));
//					Vec3D<int> nextNextPos = global.idToScPos.at(nextNextNode->id);
//					Vec3D<int> nextClusterPos(nextNextPos.x % 2, nextNextPos.y % 2, nextNextPos.z % 2);
//
//					float clusterCongestion = 0;
//					int nodesInCluster = 0;
//					std::set<int> nodes;
//					for (int x = 0; x < 2; x++) {
//						for (int y = 0; y < 2; y++) {
//							for (int z = 0; z < 2; z++) {
//								Vec3D<int> pos(nextClusterPos.x + x, nextClusterPos.y + y, nextClusterPos.z + z);
//								if (global.scPosToId.count(pos)) {
//									clusterCongestion += global.nodes.at(global.scPosToId.at(pos))->congestion;
//									nodesInCluster++;
//								}
//
//							}
//						}
//					}
//
//					congestion += clusterCongestion / nodesInCluster;
//					}else{
//						congestion++;
//					}
//					if (congestion <= minCongestion) {
//						//LOG(true, congestion);
//						minCongestion = congestion;
//						selectedChannel[c] = congestion;
//					}
//				}
//
//				channel.clear();
//				for (std::pair<Channel, float> c : selectedChannel) {
//					if (c.second == minCongestion) {
//						channel.insert(c.first);
//					}
//				}
//			}
//
////			for(Channel c: channel){
////				congestion = MESH::getCongestionRelative(ri->node, {{c.dir, 1}});
////
////			}
//
////channel = MESH::getChannelWithLeastCongestion(node, 10, channel);
////FATALCASE(channel.empty(),"Unable to select! R:"<<ri->node->id<<*rpi->packet);
//
////rpi->availableChannel = {*std::next(rpi->availableChannel.begin(), 0)};
//		}
//
////		if (!channel.size()) {
////			FATAL("Unable to select!4 R:"<<ri->node->id<<*rpi->packet);
////
////		}
//
//		rpi->selectedChannel = channel;
//		rpi->recentSelectedChannel = channel;
//
//	}
//	;
//};
//
//#endif /* SRC_MODEL_SELECTION_SELECTIONROUNDROBIN_H_ */
