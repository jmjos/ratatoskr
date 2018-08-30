/*
 * Helper.cpp
 *
 *  Created on: May 23, 2017
 *      Author: wrieden
 */
#include "Helper.h"

std::set<Channel> Helper::getAllChannel(Node* curr) {
	std::set<Channel> channel;
	for (Connection* c : curr->connections) {
		for (int vc = 0; vc < c->vcCount.at(c->nodePos.at(curr)); vc++) {
			channel.insert(Channel(curr->conToPos.at(c), vc));
		}
	}
	return channel;
}

std::set<Channel> Helper::getAllChannelWithoutLocal(Node* curr) {
	std::set<Channel> channel;
	for (Connection* c : curr->connections) {
		if (curr->conToDir.at(curr->conToPos.at(c)) != DIR::Local) {
			for (int vc = 0; vc < c->vcCount.at(c->nodePos.at(curr)); vc++) {
				channel.insert(Channel(curr->conToPos.at(c), vc));
			}
		}

	}
	return channel;
}

std::set<Channel> Helper::getChannelWithVC(std::set<int> vc, std::set<Channel> availableChannel) {
	std::set<Channel> channel;
	for (Channel c : availableChannel) {
		if (vc.count(c.vc)) {
			channel.insert(c);
		}
	}
	return channel;
}

std::set<Channel> Helper::getChannelWithDir(std::set<int> dir, std::set<Channel> availableChannel) {
	std::set<Channel> channel;
	for (Channel c : availableChannel) {
		if (dir.count(c.dir)) {
			channel.insert(c);
		}
	}
	return channel;
}

std::set<Channel> Helper::filterTurns(std::set<std::pair<Channel, Channel>> prohibitedTurns,
		Channel inputChannel, std::set<Channel> availableChannel) {
	std::set<Channel> channel;
	for (Channel c : availableChannel) {
		if (prohibitedTurns.count(std::make_pair(inputChannel, c)) == 0) {
			channel.insert(c);
		}
	}
	return channel;
}

std::set<Channel> Helper::filterTurnsDir(std::set<std::pair<int, int>> prohibitedTurns,
		Channel inputChannel, std::set<Channel> availableChannel) {
	std::set<Channel> channel;
	for (Channel c : availableChannel) {
		if (prohibitedTurns.count(std::make_pair(inputChannel.dir, c.dir)) == 0) {
			channel.insert(c);
		}
	}
	return channel;
}

std::set<Channel> Helper::getChannelWithLeastCongestion(Node* curr, int levelCount,
		std::set<Channel> availableChannel) {
	std::set<Channel> channel;
	float leastCongestion = 20;
	std::map<Channel, float> candidates;

	for (Channel c : availableChannel) {
		Connection* con = curr->connections.at(c.dir);
		int nodePos = 0;

		for (std::pair<Node*, int> pos : con->nodePos) {
			if (pos.first != curr) {
				nodePos = pos.second;
			}
		}

//		int congestion = con->vcBufferUtilization.at(nodePos).at(c.vc);
//		int bufferSize = con->bufferDepth.at(nodePos);
//		congestion = (float) congestion / (float) bufferSize * levelCount;
//		if (congestion == levelCount) {
//			congestion--;
//		}

		float congestion = con->nodes.at(nodePos)->congestion;

		if (congestion <= leastCongestion) {
			leastCongestion = congestion;
			candidates[c] = congestion;
		}
	}
	//cout<<leastCongestion<<endl;
	for (std::pair<Channel, float> candidate : candidates) {
		if (candidate.second == leastCongestion) {
			channel.insert(candidate.first);
		}
	}

	return channel;
}

std::set<Channel> Helper::getChannelWithRating(float minRating, float maxRating,
		std::set<Channel> availableChannel, std::map<Channel, float> channelRating) {
	std::set<Channel> channel;

	for (Channel c : availableChannel) {
		float rating = channelRating.at(c);
		if (rating >= minRating && rating <= maxRating) {
			channel.insert(c);
		}
	}

	return channel;
}

std::set<Channel> Helper::getChannelWithHighestRating(std::set<Channel> availableChannel,
		std::map<Channel, float> channelRating) {
	std::set<Channel> channel;
	float maxRating = -1;

	for (Channel c : availableChannel) {
		float rating = channelRating.at(c);
		if (rating > maxRating) {
			maxRating = rating;
		}
	}

	for (Channel c : availableChannel) {
		float rating = channelRating.at(c);
		if (rating == maxRating) {
			channel.insert(c);
		}
	}
	return channel;
}
