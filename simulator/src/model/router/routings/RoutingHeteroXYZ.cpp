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
#include "RoutingHeteroXYZ.h"

RoutingHeteroXYZ::RoutingHeteroXYZ(Node* node) :
		Routing(node) {
}

RoutingHeteroXYZ::~RoutingHeteroXYZ() {
}

void RoutingHeteroXYZ::checkValid() {
	assert(node->connectedNodes.size() <= 7);
	assert(node->connections.size() <= node->connectedNodes.size() + 1);
	assert(node->conPosOfDir.size() == node->connections.size());

	int i = 0;
	for (std::pair<DIR::TYPE, int> pair : node->conPosOfDir) {
		assert(std::find(DIR::XYZ.begin(), DIR::XYZ.end(), pair.first) != DIR::XYZ.end());
		i++;
	}
	assert(node->connections.size() == i);
}

void RoutingHeteroXYZ::route(RoutingInformation* ri, RoutingPacketInformation* rpi) {
	//rep.reportEvent(dbid, "routing_route_packet", std::to_string(rpi->packet->id));

	std::set<Channel> channel;
	std::map<Channel, float> channelRating;

	Vec3D<float> dstPos = rpi->packet->dst->pos;
	if (dstPos == node->pos) {
		channel= {Channel(node->conPosOfDir.at(DIR::Local), 0)};
	} else if (dstPos.z < node->pos.z) {
		channel = Helper::getChannelsWithDir({node->conPosOfDir.at(DIR::Down)},
                ri->allChannelWithoutLocal);
	} else if (dstPos.z >= node->pos.z) {
		if (dstPos.x < node->pos.x) {
			channel = Helper::getChannelsWithDir({node->conPosOfDir.at(DIR::West)},
                    ri->allChannelWithoutLocal);
		} else if (dstPos.x > node->pos.x) {
			channel = Helper::getChannelsWithDir({node->conPosOfDir.at(DIR::East)},
                    ri->allChannelWithoutLocal);
		} else if (dstPos.y < node->pos.y) {
			channel = Helper::getChannelsWithDir({node->conPosOfDir.at(DIR::South)},
                    ri->allChannelWithoutLocal);
		} else if (dstPos.y > node->pos.y) {
			channel = Helper::getChannelsWithDir({node->conPosOfDir.at(DIR::North)},
                    ri->allChannelWithoutLocal);
		}
		else
		channel = Helper::getChannelsWithDir({node->conPosOfDir.at(DIR::Up)},
                ri->allChannelWithoutLocal);
	}

	for (Channel c : channel) {
		channelRating[c] = 1;
	}

	rpi->routedChannel = channel;
	rpi->routedChannelRating = channelRating;
}

void RoutingHeteroXYZ::makeDecision(RoutingInformation* ri, RoutingPacketInformation* rpi) {
	if (rpi->recentSelectedChannel.size()) {
		rpi->outputChannel = *rpi->recentSelectedChannel.begin();
	} else {
		FATAL("Router" << ri->node->id << "[" << DIR::toString(ri->node->dirOfConPos.at(rpi->inputChannel.conPos)) << rpi->inputChannel.vc << "] - Unable to make decision! "<< *rpi->packet);
	}
}

void RoutingHeteroXYZ::beginCycle(RoutingInformation* ri) {
}

void RoutingHeteroXYZ::endCycle(RoutingInformation* ri) {
}

