#include "RoutingTMR.h"

RoutingTMR::RoutingTMR(Node* node):Routing(node){
	forbiddenTurns = MeshHelper::getProhibitedTurns(node, MeshHelper::NEGATIVEFIRST);
}

RoutingTMR::~RoutingTMR() {

}

void RoutingTMR::checkValid() {
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


void RoutingTMR::route(RoutingInformation* ri, RoutingPacketInformation* rpi) {
	//rep.reportEvent(dbid, "routing_route_packet", std::to_string(rpi->packet->id));

	std::set<Channel> channel;
	std::map<Channel, float> channelRating;

	Vec3D<float> dstPos = rpi->packet->dst->pos;
	if (dstPos == node->pos) {
		channel={Channel(node->dirToCon.at(DIR::Local), 0)};
		channelRating[Channel(node->dirToCon.at(DIR::Local),0)] = 1;
	}else{
		channel=Helper::filterTurnsDir(forbiddenTurns, rpi->inputChannel ,ri->allChannelWithoutLocal);
		for(Channel c: channel){
			channelRating[c] = 1;

			DIR::TYPE dir = ri->node->conToDir.at(c.dir);
			if((ri->node->pos.x>dstPos.x || ri->node->pos.y>dstPos.y || ri->node->pos.z>dstPos.z) &&
					(dir == DIR::East ||dir == DIR::North ||dir == DIR::Up)){
				channelRating[c] = 0;
			}

			if(ri->node->pos.x>=dstPos.x && ri->node->conToDir.at(c.dir) == DIR::East){
				channelRating[c] = 0;
			}
			if(ri->node->pos.y>=dstPos.y && ri->node->conToDir.at(c.dir) == DIR::North){
				channelRating[c] = 0;
			}
			if(ri->node->pos.z>=dstPos.z && ri->node->conToDir.at(c.dir) == DIR::Up){
				channelRating[c] = 0;
			}
		}
	}
	if(channel.empty()){
		FATAL("Unable to route! R:"<<ri->node->id<<*rpi->packet);
	}

	rpi->routedChannel = channel;
	rpi->routedChannelRating = channelRating;
}

void RoutingTMR::makeDecision(RoutingInformation* ri, RoutingPacketInformation* rpi) {
	if(rpi->recentSelectedChannel.size()){
		rpi->outputChannel = *rpi->recentSelectedChannel.begin();
	//cout<<node->id<<" "<<rpi->channelRating.at(*rpi->availableChannel.begin())<<" "<<rpi->packet->dst<<endl;
		}
	else
		FATAL("Router"<<ri->node->id<<"["<<DIR::toString(ri->node->conToDir.at(rpi->inputChannel.dir))<<rpi->inputChannel.vc<<"] - Unable to make decision! "<<*rpi->packet);
}

void RoutingTMR::beginCycle(RoutingInformation* ri){

}

void RoutingTMR::endCycle(RoutingInformation* ri){

}
