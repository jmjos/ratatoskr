/*
 * SelectionDyXYZ.cpp
 *
 *  Created on: Jan 22, 2018
 *      Author: wrieden
 */


#include "SelectionDyXYZ.h"

void SelectionDyXYZ::select(RoutingInformation* ri, RoutingPacketInformation* rpi){
		std::set<Channel> channel = rpi->routedChannel;

		if(!channel.empty()){
			channel = MeshHelper::getMinimalChannel(node, rpi->packet->dst, channel);
//			if(channel.empty()){
//					FATAL("Unable to select! R:"<<ri->node->id<<*rpi->packet);
//			}

			//rpi->availableChannel = MESH::getChannelWithVC({0}, rpi->availableChannel);

			channel = Helper::getChannelWithRating(0.5, 1, channel, rpi->routedChannelRating);
//			if(channel.empty()){
//					FATAL("Unable to select! R:"<<ri->node->id<<*rpi->packet);
//			}

			channel = Helper::getChannelWithHighestRating(channel, rpi->routedChannelRating);
//			if(channel.empty()){
//					FATAL("Unable to select! R:"<<ri->node->id<<*rpi->packet);
//			}

			channel = Helper::getChannelWithLeastCongestion(node, 10, channel);
//			if(channel.empty()){
//					FATAL("Unable to select! R:"<<ri->node->id<<*rpi->packet);
//			}
			//rpi->availableChannel = {*std::next(rpi->availableChannel.begin(), 0)};
		}

		if(!channel.size()){
			FATAL("Unable to select! R: "<<ri->node->id<<*rpi->packet);


		}

		rpi->selectedChannel = channel;
		rpi->recentSelectedChannel = channel;


	};

