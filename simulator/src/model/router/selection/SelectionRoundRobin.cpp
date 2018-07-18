/*
 * SelectionRoundRobin.cpp
 *
 *  Created on: Jan 22, 2018
 *      Author: wrieden
 */


#include "SelectionRoundRobin.h"

	void SelectionRoundRobin::select(RoutingInformation* ri, RoutingPacketInformation* rpi){
		std::set<Channel> channel = rpi->routedChannel;

		if(!channel.empty()){
			int selectedDirection = channel.begin()->dir;
			channel = Helper::getChannelWithDir({selectedDirection}, channel);
			int vcs = ri->vcCount.at(selectedDirection);
			for(int i= 0; i<vcs; i++){
				std::set<Channel> selchannel = Helper::getChannelWithVC({(rrVC.at(selectedDirection))%vcs}, channel);
				rrVC.at(selectedDirection)++;
				if(!selchannel.empty()){
					rpi->selectedChannel = selchannel;
					rpi->recentSelectedChannel = selchannel;
					return;
				}
			}
		}


	};
