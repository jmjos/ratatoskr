/*
 * SelectionRoundRobin.cpp
 *
 *  Created on: Jan 22, 2018
 *      Author: wrieden
 */


#include "SelectionRoundRobin.h"

	void SelectionRoundRobin::select(RoutingInformation* ri, RoutingPacketInformation* rpi){
		std::set<Channel> channel = rpi->routedChannel;
		rrVC++;
		if(!channel.empty()){
			int vcs = ri->vcCount.at(channel.begin()->dir);

			for(int i= 0; i<vcs; i++){
				std::set<Channel> selchannel = Helper::getChannelWithVC({(rrVC+i)%vcs}, channel);
				if(!selchannel.empty()){
					rpi->selectedChannel = selchannel;
					rpi->recentSelectedChannel = selchannel;
					return;
				}
			}
		}

	};
