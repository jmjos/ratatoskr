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
//#ifndef SRC_MODEL_SELECTION_SELECTIONMAFA_H_
//#define SRC_MODEL_SELECTION_SELECTIONMAFA_H_
//
//#include "Selection.h"
//
//struct SelectionMAFA: public Selection {
//	int rrVC = 0;
//
//	SelectionMAFA(Node* node) :
//			Selection(node) {
//
//	}
//
//	~SelectionMAFA() {
//
//	}
//
//	//void checkValid()=0;
//	void select(RoutingInformation* ri, RoutingPacketInformation* rpi) {
//		std::set<Channel> channel = rpi->routedChannel;
//
////		if(rpi->packet->routerIDs.count(ri->node->id)){
////			LOG(true, "CYCLE");
////		}
//
//		if (!channel.empty()) {
//			if (rpi->packet->dst == ri->node->id) {
//				rpi->selectedChannel = {Channel(ri->node->dirToCon.at(DIR::Local), 0)};
//				rpi->recentSelectedChannel = rpi->selectedChannel;
//				return;
//			}
//
//			//FATALCASE(channel.empty(), "Unable to select2! R:"<<ri->node->id<<*rpi->packet);
//
//			for(float i=0.95; i>0; i-=0.1) {
//
//				channel = MESH::getChannelWithRating(i, 1, rpi->routedChannel, rpi->routedChannelRating);
//				if(MESH::getMinimalChannel(ri->node, globalResources.nodes.at(rpi->packet->dst), channel).empty()){
//					continue;
//				}
//
//				std::map<Channel, ChannelInfo> info = MESH::getChannelInfo(node, globalResources.nodes.at(rpi->packet->dst), channel, 2, 0.5);
//
//				float minCong = 100;
//				int maxDiffDim = 0;
//				bool minimalExists = false;
//				bool dimOffExists = false;
//				bool otherExists = false;
//
//				channel.clear();
//				Node* dst = globalResources.nodes.at(rpi->packet->dst);
//				for(std::pair<Channel, ChannelInfo> p: info) {
//
//					Node* next = ri->node->conToNode.at(p.first.dir);
//					int distance = MESH::getHopDistance(ri->node, dst);
//					int nextdistance = MESH::getHopDistance(next, dst);
//					int diffdim = ri->node->pos.diffDimCount(dst->pos);
//					int nextdiffdim = next->pos.diffDimCount(dst->pos);
//
//					if(nextdistance == 0) {
//						rpi->selectedChannel = {p.first};
//						rpi->recentSelectedChannel = {p.first};
//						return;
//					}
//
//					if(p.second.minimalPaths && nextdistance < distance) {
//						if(nextdiffdim >= diffdim) {
//							if(p.second.minimalCongestion <= minCong || !dimOffExists) {
//								minCong = p.second.minimalCongestion;
//								channel= {p.first};
//							}
//
//							dimOffExists = true;
//						} else if(!dimOffExists) {
//							if(p.second.minimalCongestion <= minCong || !minimalExists) {
//								minCong = p.second.minimalCongestion;
//								channel= {p.first};
//							}
//						}
//						minimalExists = true;
//					}
//					else if(p.second.availablePaths && !minimalExists) {
//						if(nextdiffdim > diffdim && (rpi->packet->traversedRouter.empty() || !rpi->packet->routerIDs.count(next->id))) {
//							if(p.second.minimalCongestion <= minCong || !dimOffExists) {
//								minCong = p.second.minimalCongestion;
//								channel= {p.first};
//							}
//							otherExists = true;
//						}
//					}
//
//				}
//
//				if(!channel.empty()) {
//					break;
//				}
//			}
//
////			for(std::pair<Channel, ChannelInfo> p: info){
////				if(p.second.minimalCongestion <= minCong){
////					channel.insert(p.first);
////				}
////			}
//
////			if (!channel.size()) {
////				FATAL("Unable to select!4 R:"<<ri->node->id<<*rpi->packet);
////
////			}
//		}
//
//		rpi->recentSelectedChannel = channel;
//		rpi->selectedChannel = channel;
//	}
//};
//
//#endif /* SRC_MODEL_SELECTION_SELECTIONROUNDROBIN_H_ */
