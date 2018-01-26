////////////////////////////////////////////////////////////////////////////////
// Copyright (C) 2018 Jan Moritz Joseph
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
#include "RoutingXYZ.h"

RoutingXYZ::RoutingXYZ(Node* node) :Routing(node)
	 {

}

RoutingXYZ::~RoutingXYZ() {

}

void RoutingXYZ::checkValid() {
	assert(node->connectedNodes.size() <= 6);
	assert(node->connections.size() <= node->connectedNodes.size() + 1);
	assert(node->dirToCon.size() == node->connections.size());

	int i=0;
	for(std::pair<DIR::TYPE, int> pair : node->dirToCon){
		assert(std::find(DIR::XYZ.begin(), DIR::XYZ.end(), pair.first) != DIR::XYZ.end());
		i++;
	}
	assert(node->connections.size() == i);
}

void RoutingXYZ::route(RoutingInformation* ri, RoutingPacketInformation* rpi) {
	rep.reportEvent(dbid, "routing_route_packet", std::to_string(rpi->packet->id));

	Vec3D<float> dstPos = global.nodes.at(global.getIdByPe(rpi->packet->dst))->pos;
	if (dstPos == node->pos) {
		rpi->availableChannel={Channel(node->dirToCon.at(DIR::Local), 0)};
	} else if (dstPos.x < node->pos.x) {
		rpi->availableChannel=MESH::getChannelWithDir({node->dirToCon.at(DIR::West)}, ri->allChannelWithoutLocal);
	} else if (dstPos.x > node->pos.x) {
		rpi->availableChannel=MESH::getChannelWithDir({node->dirToCon.at(DIR::East)}, ri->allChannelWithoutLocal);
	} else if (dstPos.y < node->pos.y) {
		rpi->availableChannel=MESH::getChannelWithDir({node->dirToCon.at(DIR::South)}, ri->allChannelWithoutLocal);
	} else if (dstPos.y > node->pos.y) {
		rpi->availableChannel=MESH::getChannelWithDir({node->dirToCon.at(DIR::North)}, ri->allChannelWithoutLocal);
	} else if (dstPos.z < node->pos.z) {
		rpi->availableChannel=MESH::getChannelWithDir({node->dirToCon.at(DIR::Down)}, ri->allChannelWithoutLocal);
	} else if (dstPos.z > node->pos.z) {
		rpi->availableChannel=MESH::getChannelWithDir({node->dirToCon.at(DIR::Up)}, ri->allChannelWithoutLocal);
	}
}

void RoutingXYZ::makeDecision(RoutingInformation* ri, RoutingPacketInformation* rpi) {
	rpi->outputChannel = *rpi->availableChannel.begin();

}
