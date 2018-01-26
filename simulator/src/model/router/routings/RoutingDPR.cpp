#include "RoutingDPR.h"

RoutingDPR::RoutingDPR(Node* node):Routing(node){
	forbiddenTurns.push_back(MeshHelper::getProhibitedTurns(node, MeshHelper::ENULAST));
	forbiddenTurns.push_back(MeshHelper::getProhibitedTurns(node, MeshHelper::ESDLAST));
	forbiddenTurns.push_back(MeshHelper::getProhibitedTurns(node, MeshHelper::WSULAST));
	forbiddenTurns.push_back(MeshHelper::getProhibitedTurns(node, MeshHelper::WNDLAST));
}

RoutingDPR::~RoutingDPR() {

}

void RoutingDPR::checkValid() {
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


void RoutingDPR::route(RoutingInformation* ri, RoutingPacketInformation* rpi) {
	//rep.reportEvent(dbid, "routing_route_packet", std::to_string(rpi->packet->id));

	std::set<int> classes;
	std::set<Channel> channel;
	std::map<Channel, float> channelRating;

	Vec3D<float> pos = ri->node->pos;
	Vec3D<float> dstPos = rpi->packet->dst->pos;
	if (pos == dstPos) {
		channel={Channel(node->dirToCon.at(DIR::Local), 0)};
		channelRating[Channel(node->dirToCon.at(DIR::Local), 0)] = 1;
	} else {

		if (ri->node->conToDir.at(rpi->inputChannel.dir) == DIR::Local) {
			if ((pos.x <= dstPos.x && pos.y <= dstPos.y && pos.z <= dstPos.z)
					|| (pos.x >= dstPos.x && pos.y >= dstPos.y
							&& pos.z >= dstPos.z)) {
				classes.insert(0);
			}
			if ((pos.x <= dstPos.x && pos.y >= dstPos.y && pos.z >= dstPos.z)
					|| (pos.x >= dstPos.x && pos.y <= dstPos.y
							&& pos.z <= dstPos.z)) {
				classes.insert(1);
			}
			if ((pos.x >= dstPos.x && pos.y >= dstPos.y && pos.z <= dstPos.z)
					|| (pos.x <= dstPos.x && pos.y <= dstPos.y
							&& pos.z >= dstPos.z)) {
				classes.insert(2);
			}
			if ((pos.x >= dstPos.x && pos.y <= dstPos.y && pos.z >= dstPos.z)
					|| (pos.x <= dstPos.x && pos.y >= dstPos.y
							&& pos.z <= dstPos.z)) {
				classes.insert(3);
			}

		} else {
			classes.insert(rpi->inputChannel.vc);
		}

		for (int pkgclass : classes) {
			std::set<Channel> vcchannel = Helper::getChannelWithVC( { pkgclass },
					ri->allChannelWithoutLocal);
			vcchannel = Helper::filterTurnsDir(forbiddenTurns.at(pkgclass),
					rpi->inputChannel, vcchannel);
			channel.insert(vcchannel.begin(), vcchannel.end());
		}

	}

	channel = MeshHelper::getMinimalChannel(node, rpi->packet->dst, channel);

	for(Channel c: channel){
		channelRating[c] = 1;
	}

	if(channel.empty()){
		FATAL("Unable to route! R:"<<ri->node->id<<*rpi->packet);
	}

	rpi->routedChannel= channel;
	rpi->routedChannelRating = channelRating;
}

void RoutingDPR::makeDecision(RoutingInformation* ri, RoutingPacketInformation* rpi) {
	if(rpi->recentSelectedChannel.size()){
		rpi->outputChannel = *rpi->recentSelectedChannel.begin();
	//cout<<node->id<<" "<<rpi->channelRating.at(*rpi->availableChannel.begin())<<" "<<rpi->packet->dst<<endl;
		}
	else{

		FATAL("Router"<<ri->node->id<<"["<<DIR::toString(ri->node->conToDir.at(rpi->inputChannel.dir))<<rpi->inputChannel.vc<<"] - Unable to make decision! "<<*rpi->packet);
	}

}

void RoutingDPR::beginCycle(RoutingInformation* ri){

}

void RoutingDPR::endCycle(RoutingInformation* ri){

}

