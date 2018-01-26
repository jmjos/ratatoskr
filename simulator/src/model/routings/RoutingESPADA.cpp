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
//#include "RoutingESPADA.h"
//
//RoutingESPADA::RoutingESPADA(Node* node):Routing(node){
//	forbiddenTurns.at(0).insert({node->dirToCon.count(DIR::South)?node->dirToCon.at(DIR::South):-1,node->dirToCon.count(DIR::West)?node->dirToCon.at(DIR::West):-1});
//	forbiddenTurns.at(0).insert({node->dirToCon.count(DIR::South)?node->dirToCon.at(DIR::South):-1,node->dirToCon.count(DIR::Down)?node->dirToCon.at(DIR::Down):-1});
//	forbiddenTurns.at(0).insert({node->dirToCon.count(DIR::West)?node->dirToCon.at(DIR::West):-1,node->dirToCon.count(DIR::South)?node->dirToCon.at(DIR::South):-1});
//	forbiddenTurns.at(0).insert({node->dirToCon.count(DIR::West)?node->dirToCon.at(DIR::West):-1,node->dirToCon.count(DIR::Down)?node->dirToCon.at(DIR::Down):-1});
//	forbiddenTurns.at(0).insert({node->dirToCon.count(DIR::Down)?node->dirToCon.at(DIR::Down):-1,node->dirToCon.count(DIR::West)?node->dirToCon.at(DIR::West):-1});
//	forbiddenTurns.at(0).insert({node->dirToCon.count(DIR::Down)?node->dirToCon.at(DIR::Down):-1,node->dirToCon.count(DIR::South)?node->dirToCon.at(DIR::South):-1});
//	forbiddenTurns.at(0).erase({-1,-1});
//
//	forbiddenTurns.at(1).insert({node->dirToCon.count(DIR::West)?node->dirToCon.at(DIR::West):-1,node->dirToCon.count(DIR::North)?node->dirToCon.at(DIR::North):-1});
//	forbiddenTurns.at(1).insert({node->dirToCon.count(DIR::West)?node->dirToCon.at(DIR::West):-1,node->dirToCon.count(DIR::Up)?node->dirToCon.at(DIR::Up):-1});
//	forbiddenTurns.at(1).insert({node->dirToCon.count(DIR::North)?node->dirToCon.at(DIR::North):-1,node->dirToCon.count(DIR::West)?node->dirToCon.at(DIR::West):-1});
//	forbiddenTurns.at(1).insert({node->dirToCon.count(DIR::North)?node->dirToCon.at(DIR::North):-1,node->dirToCon.count(DIR::Up)?node->dirToCon.at(DIR::Up):-1});
//	forbiddenTurns.at(1).insert({node->dirToCon.count(DIR::Up)?node->dirToCon.at(DIR::Up):-1,node->dirToCon.count(DIR::West)?node->dirToCon.at(DIR::West):-1});
//	forbiddenTurns.at(1).insert({node->dirToCon.count(DIR::Up)?node->dirToCon.at(DIR::Up):-1,node->dirToCon.count(DIR::North)?node->dirToCon.at(DIR::North):-1});
//	forbiddenTurns.at(1).erase({-1,-1});
//
//	forbiddenTurns.at(2).insert({node->dirToCon.count(DIR::East)?node->dirToCon.at(DIR::East):-1,node->dirToCon.count(DIR::North)?node->dirToCon.at(DIR::North):-1});
//	forbiddenTurns.at(2).insert({node->dirToCon.count(DIR::East)?node->dirToCon.at(DIR::East):-1,node->dirToCon.count(DIR::Down)?node->dirToCon.at(DIR::Down):-1});
//	forbiddenTurns.at(2).insert({node->dirToCon.count(DIR::North)?node->dirToCon.at(DIR::North):-1,node->dirToCon.count(DIR::East)?node->dirToCon.at(DIR::East):-1});
//	forbiddenTurns.at(2).insert({node->dirToCon.count(DIR::North)?node->dirToCon.at(DIR::North):-1,node->dirToCon.count(DIR::Down)?node->dirToCon.at(DIR::Down):-1});
//	forbiddenTurns.at(2).insert({node->dirToCon.count(DIR::Down)?node->dirToCon.at(DIR::Down):-1,node->dirToCon.count(DIR::East)?node->dirToCon.at(DIR::East):-1});
//	forbiddenTurns.at(2).insert({node->dirToCon.count(DIR::Down)?node->dirToCon.at(DIR::Down):-1,node->dirToCon.count(DIR::North)?node->dirToCon.at(DIR::North):-1});
//	forbiddenTurns.at(2).erase({-1,-1});
//
//	forbiddenTurns.at(3).insert({node->dirToCon.count(DIR::East)?node->dirToCon.at(DIR::East):-1,node->dirToCon.count(DIR::South)?node->dirToCon.at(DIR::South):-1});
//	forbiddenTurns.at(3).insert({node->dirToCon.count(DIR::East)?node->dirToCon.at(DIR::East):-1,node->dirToCon.count(DIR::Up)?node->dirToCon.at(DIR::Up):-1});
//	forbiddenTurns.at(3).insert({node->dirToCon.count(DIR::South)?node->dirToCon.at(DIR::South):-1,node->dirToCon.count(DIR::East)?node->dirToCon.at(DIR::East):-1});
//	forbiddenTurns.at(3).insert({node->dirToCon.count(DIR::South)?node->dirToCon.at(DIR::South):-1,node->dirToCon.count(DIR::Up)?node->dirToCon.at(DIR::Up):-1});
//	forbiddenTurns.at(3).insert({node->dirToCon.count(DIR::Up)?node->dirToCon.at(DIR::Up):-1,node->dirToCon.count(DIR::East)?node->dirToCon.at(DIR::East):-1});
//	forbiddenTurns.at(3).insert({node->dirToCon.count(DIR::Up)?node->dirToCon.at(DIR::Up):-1,node->dirToCon.count(DIR::South)?node->dirToCon.at(DIR::South):-1});
//	forbiddenTurns.at(3).erase({-1,-1});
//}
//
//RoutingESPADA::~RoutingESPADA() {
//
//}
//
//void RoutingESPADA::checkValid() {
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
//std::set<Channel>  RoutingESPADA::route(RoutingInformation* ri) {
//	rep.reportEvent(dbid, "routing_route_packet", std::to_string(ri->packet->id));
//
//	std::set<Channel> channeltmp, channel;
//
//	if(node->conToDir.at(ri->inputChannel.dir) == DIR::Local){
//		return ri->allChannelWithoutLocal;
//	}else if(ri->packet->dst==node->id){
//		return {Channel(node->dirToCon.at(DIR::Local),0)};
//	}else{
//		for(Channel c: ri->allChannelWithoutLocal){
//			if(ri->packet->pkgclass <= ri->tag.at(c)){
//				channeltmp.insert(c);
//			}
//		}
//	}
//	for(Channel c: channeltmp){
//		for(Channel c2: ri->allChannelWithoutLocal){
//			if(c.dir == c2.dir){
//				channel.insert(c2);
//			}
//		}
//	}
//	return channel;
//}
//
//std::set<Channel>  RoutingESPADA::select(RoutingInformation* ri) {
//
//	std::set<Channel> channel;
//	for(int i = 0; i<ri->packet->pkgclass; i++){
//		std::set<Channel> channel = MESH::filterTurnsDir(forbiddenTurns.at((ri->packet->pkgclass+i)%classcount),ri->inputChannel,ri->availableChannel);
//		if (channel.empty()){
//			return channel;
//		}
//	}
//
//	 return ri->availableChannel;
//}
//
//Channel RoutingESPADA::selectFinal(RoutingInformation* ri) {
//	assert(!ri->availableChannel.empty());
//	Channel c= *ri->availableChannel.begin();
//
//	if(ri->flowIn.at(c)){
//		return c;
//	}else{
//		return Channel(-1,-1);
//	}
//}
