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
//
//#include "RoutingDyXYZ.h"
//
//RoutingDyXYZ::RoutingDyXYZ(Node* node) :
//		Routing(node) {
//
//}
//
//RoutingDyXYZ::~RoutingDyXYZ() {
//
//}
//
//void RoutingDyXYZ::checkValid() {
//	assert(node->connectedNodes.size() <= 6);
//	assert(node->connections.size() <= node->connectedNodes.size() + 1);
//	assert(node->dirToCon.size() == node->connections.size());
//
//	int i=0;
//	for(std::pair<DIR::TYPE, int> pair : node->dirToCon){
//		assert(std::find(DIR::XYZ.begin(), DIR::XYZ.end(), pair.first) != DIR::XYZ.end());
//		i++;
//	}
//	assert(node->connections.size() == i);
//}
//
//std::set<Channel> RoutingDyXYZ::route(RoutingInformation* ri) {
//	rep.reportEvent(dbid, "routing_route_packet", std::to_string(ri->packet->id));
////	cout<<"here"<<endl;
//
//	std::set<Channel> channel = MESH::getAllChannel(node);
///*	for(Channel c: channel){
//		Connection* con = node->connections.at(c.dir);
//		int nodePos=0;
//
//		for(std::pair<Node*, int> pos: con->nodePos){
//			if(pos.first!=node){
//				nodePos = pos.second;
//			}
//		}
//
//		int congestion = con->congestion.at(nodePos).at(c.vc);
//		int bufferSize = con->bufferDepth.at(nodePos);
//		int congestion4 = (float) congestion / (float) bufferSize * 4;
//
//		cout<<DIR::toString(node->conToDir.at(c.dir))<<c.vc<<" "<<congestion<<" "<<congestion4<<endl;
//	}
//	cout<<"here02"<<endl;*/
//	if(node->id==ri->packet->dst){
//		channel = {Channel(node->dirToCon.at(DIR::Local),0)};
//	}else{
//		std::set<int> vcs;
//		Vec3D<float> diff = global.nodes.at(ri->packet->dst)->pos-node->pos;
//		if((diff.x>=0&&diff.y>=0&&diff.z>=0)||(diff.x<=0&&diff.y<=0&&diff.z<=0)){
//			vcs.insert(0);
//		}if((diff.x>=0&&diff.y>=0&&diff.z<=0)||(diff.x<=0&&diff.y<=0&&diff.z>=0)){
//			vcs.insert(1);
//		}if((diff.x>=0&&diff.y<=0&&diff.z>=0)||(diff.x<=0&&diff.y>=0&&diff.z<=0)){
//			vcs.insert(2);
//		}if((diff.x>=0&&diff.y<=0&&diff.z<=0)||(diff.x<=0&&diff.y>=0&&diff.z>=0)){
//			vcs.insert(3);
//		}
//		channel = MESH::getChannelWithVC(vcs,channel);
//	}
//	channel = MESH::getMinimalChannel(node, ri->packet, channel);
//	channel = MESH::getChannelWithLeastCongestion(node, 4, channel);
//////
////	std::set<std::pair<Channel, Channel>> turns;
////	if(node->dirToCon.count(DIR::North)){
////		if(node->dirToCon.count(DIR::East)){
////			turns.insert(std::make_pair(Channel(node->dirToCon.at(DIR::North),0),Channel(node->dirToCon.at(DIR::East),0)));
////		}
////		if(node->dirToCon.count(DIR::West)){
////			turns.insert(std::make_pair(Channel(node->dirToCon.at(DIR::North),0),Channel(node->dirToCon.at(DIR::West),0)));
////		}
////	}
////	if(node->dirToCon.count(DIR::South)){
////		if(node->dirToCon.count(DIR::East)){
////			turns.insert(std::make_pair(Channel(node->dirToCon.at(DIR::South),1),Channel(node->dirToCon.at(DIR::East),1)));
////		}
////		if(node->dirToCon.count(DIR::West)){
////			turns.insert(std::make_pair(Channel(node->dirToCon.at(DIR::South),1),Channel(node->dirToCon.at(DIR::West),1)));
////		}
////	}
//
//
////	for(Channel c: channel){
////		cout<<DIR::toString(node->conToDir.at(c.dir))<<c.vc<<endl;
////	}
//
//
//	return channel;
//
//}
//
//std::set<Channel>  RoutingDyXYZ::select(RoutingInformation* ri) {
//	return ri->availableChannel;
//}
//
//Channel RoutingDyXYZ::selectFinal(RoutingInformation* ri) {
//	assert(!ri->availableChannel.empty());
//	Channel c= *ri->availableChannel.begin();
//
//	if(ri->flowIn.at(c)){
//		return c;
//	}else{
//		return Channel(-1,-1);
//	}
//
//}
