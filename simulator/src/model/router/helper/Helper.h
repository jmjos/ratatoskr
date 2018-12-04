/*
 * Helper.h
 *
 *  Created on: May 23, 2017
 *      Author: wrieden
 */

#ifndef __HELPER_GENERIC_H_
#define __HELPER_GENERIC_H_

#include "utils/GlobalResources.h"
#include "utils/Structures.h"
#include "model/traffic/Flit.h"
#include "model/traffic/Packet.h"
#include "utils/GlobalReport.h"
#include "utils/Report.h"
#include "model/container/Container.h"

class Helper{
public:
	static std::set<Channel> getAllChannel(Node* curr);
	static std::set<Channel> getAllChannelWithoutLocal(Node* curr);
	static std::set<Channel> getChannelWithVC(std::set<int> vc, std::set<Channel> availableChannel);
	static std::set<Channel> getChannelWithDir(std::set<int> dir, std::set<Channel> availableChannel);
	static std::set<Channel> filterTurns(std::set<std::pair<Channel,Channel>> prohibitedTurns, Channel inputChannel, std::set<Channel> availableChannel);
	static std::set<Channel> filterTurnsDir(std::set<std::pair<int,int>> prohibitedTurns, Channel inputChannel, std::set<Channel> availableChannel);
	static std::set<Channel> getChannelWithLeastCongestion(Node* curr, int levelCount, std::set<Channel> availableChannel);
	static std::set<Channel> getChannelWithRating(float minRating, float maxRating, std::set<Channel> availableChannel, std::map<Channel, float> channelRating);
	static std::set<Channel> getChannelWithHighestRating(std::set<Channel> availableChannel, std::map<Channel, float> channelRating);

private:
	Helper();
};

#endif /* SRC_MODEL_ROUTINGS_MESH_HELPER_H_ */
