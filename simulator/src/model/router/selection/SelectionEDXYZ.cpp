/*
 * SelectionEDXYZ.cpp
 *
 *  Created on: Jan 22, 2018
 *      Author: wrieden
 */

#include "SelectionEDXYZ.h"


void SelectionEDXYZ::select(RoutingInformation* ri, RoutingPacketInformation* rpi){
		std::set<Channel> channel = rpi->routedChannel;

		if(!channel.empty()){
			if (rpi->packet->dst->pos == ri->node->pos) {
				rpi->selectedChannel = {Channel(ri->node->dirToCon.at(DIR::Local), 0)};
				rpi->recentSelectedChannel = rpi->selectedChannel;
				return;
			}

			channel = MeshHelper::getMinimalChannel(node, rpi->packet->dst, channel);
			//rpi->availableChannel = MESH::getChannelWithVC({0}, rpi->availableChannel);
			channel = Helper::getChannelWithRating(0.5, 1, channel, rpi->routedChannelRating);

			std::map<Channel, int> lcf;

			Vec3D<float> dstPos = rpi->packet->dst->pos;
			for(Channel c: channel){
				DIR::TYPE dir=ri->node->conToDir.at(c.dir);
				if(dir!=DIR::Local){
					Connection* con = ri->node->connections.at(ri->node->dirToCon.at(dir));
					int nodePos=0;

					for(std::pair<Node*, int> pos: con->nodePos){
						if(pos.first!=ri->node){
							nodePos = pos.second;
						}
					}

					Node* neighbourNode = con->nodes.at(nodePos);

					if(neighbourNode->pos==dstPos){
						lcf[c] = 0;

					}else{
											Vec3D<float> nextdiff = neighbourNode->pos-dstPos;
					Vec3D<float> currdiff = ri->node->pos-dstPos;


					if((!nextdiff.x && !nextdiff.y) || (!nextdiff.x && !nextdiff.z) || (!nextdiff.y && !nextdiff.z)){
						Channel ch = *(MeshHelper::getMinimalChannel(neighbourNode, rpi->packet->dst, ri->allChannel).begin());
						DIR::TYPE nextdir=neighbourNode->conToDir.at(ch.dir);
						if(nextdir != dir){
							lcf[c] = MeshHelper::getCongestionAlong(neighbourNode, 5, 1, ch);
						}else{
							lcf[c] = 0;
						}

					}else{
						lcf[c] = 0;
					}
					}
				}
				else{
					return;
				}
			}

			std::set<Channel> selchannel;

			for(Channel c: channel){
				if(lcf.at(c)!=4){
					selchannel.insert(c);
				}
			}

			if(selchannel.empty()){
				channel = Helper::getChannelWithLeastCongestion(node, 10, channel);
			}else{
				channel = Helper::getChannelWithLeastCongestion(node, 10, selchannel);
			}


			//rpi->availableChannel = {*std::next(rpi->availableChannel.begin(), 0)};
		}

		if(!channel.size()){
			FATAL("Unable to select! R:" << ri->node->id << *rpi->packet);
		}

		rpi->selectedChannel = channel;
		rpi->recentSelectedChannel = channel;

	};
