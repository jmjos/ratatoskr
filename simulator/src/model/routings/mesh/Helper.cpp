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
#include "Helper.h"

std::set<Channel> MESH::getAllChannel(Node* curr){
	std::set<Channel> channel;
	for (Connection* c : curr->connections) {
		for (int vc = 0; vc < c->vcCount.at(c->nodePos.at(curr)); vc++) {
			channel.insert(Channel(curr->conToPos.at(c), vc));
		}
	}
	return channel;
}

std::set<Channel> MESH::getChannelWithVC(std::set<int> vc,
		std::set<Channel> availableChannel) {
	std::set<Channel> channel;
	for (Channel c : availableChannel) {
		if (vc.count(c.vc)) {
			channel.insert(c);
		}
	}
	return channel;
}

std::set<Channel> MESH::getChannelWithDir(std::set<int> dir,
		std::set<Channel> availableChannel) {
	std::set<Channel> channel;
	for (Channel c : availableChannel) {
		if (dir.count(c.dir)) {
			channel.insert(c);
		}
	}
	return channel;
}


std::set<Channel> MESH::getMinimalChannel(Node* curr, Packet* pkg,
		std::set<Channel> availableChannel) {
	std::set<Channel> channel;
	std::set<int> dirs;
	Vec3D<float> dst = GlobalInputClass::getInstance().nodes.at(pkg->dst)->pos;
	Vec3D<float> dist = dst - curr->pos;
	for (Node* n : curr->connectedNodes) {
		Vec3D<float> ndist = dst - n->pos;
		if (ndist < dist) {
			for (int dir : curr->connectionsToNode.at(n)) {
				dirs.insert(dir);
			}
		}
	}

	if (curr->id == pkg->dst) {
		dirs.insert(curr->dirToCon.at(DIR::Local));
	}

	for (Channel c : availableChannel) {
		if (dirs.count(c.dir)) {
			channel.insert(c);
		}
	}
	return channel;
}


std::set<Channel> MESH::filterTurns(
		std::set<std::pair<Channel, Channel>> prohibitedTurns,
		Channel inputChannel, std::set<Channel> availableChannel) {
	std::set<Channel> channel;
	for (Channel c : availableChannel) {
		if (prohibitedTurns.count(std::make_pair(inputChannel, c)) == 0) {
			channel.insert(c);
		}
	}
	return channel;
}

std::set<Channel> MESH::filterTurnsDir(
		std::set<std::pair<int, int>> prohibitedTurns,
		Channel inputChannel, std::set<Channel> availableChannel) {
	std::set<Channel> channel;
	for (Channel c : availableChannel) {
		if (prohibitedTurns.count(std::make_pair(inputChannel.dir, c.dir)) == 0) {
			channel.insert(c);
		}
	}
	return channel;
}


std::set<Channel> MESH::getChannelWithLeastCongestion(Node* curr,
		int levelCount, std::set<Channel> availableChannel) {
	std::set<Channel> channel;
	uint32_t leastCongestion = -1;
	std::map<Channel, uint32_t> candidates;

	for (Channel c : availableChannel) {
		Connection* con = curr->connections.at(c.dir);
		int nodePos=0;

		for(std::pair<Node*, int> pos: con->nodePos){
			if(pos.first!=curr){
				nodePos = pos.second;
			}
		}

		int congestion = con->congestion.at(nodePos).at(c.vc);
		int bufferSize = con->bufferDepth.at(nodePos);
		congestion = (float) congestion / (float) bufferSize * levelCount;
		if (congestion == levelCount) {
			congestion--;
		}

		if (congestion <= leastCongestion) {
			leastCongestion = congestion;
			candidates[c]=congestion;
		}
	}
	//cout<<leastCongestion<<endl;
	for (std::pair<Channel, int> candidate : candidates) {
		if (candidate.second == leastCongestion) {
			channel.insert(candidate.first);
		}
	}

	return channel;
}

std::set<Channel> MESH::getChannelWithLeastCongestionAlong(Node* curr,
		int levelCount, bool perVc,std::set<Channel> availableChannel) {
	std::set<Channel> channel;
	uint32_t leastCongestion = -1;
	std::map<Channel, uint32_t> candidates;

	for (Channel c : availableChannel) {
		DIR::TYPE dir=curr->conToDir.at(c.dir);
		uint32_t mostCongestion = 0;

		for(Node* n = curr; n->dirToCon.count(dir) && dir!=DIR::Local;){
			Connection* con = n->connections.at(n->dirToCon.at(dir));
			int nodePos=0;

			for(std::pair<Node*, int> pos: con->nodePos){
				if(pos.first!=n){
					nodePos = pos.second;
				}
			}
			int congestion = 0;
			int bufferSize = con->bufferDepth.at(nodePos);

			if(perVc){
				congestion = con->congestion.at(nodePos).at(c.vc);
				congestion = (float) congestion / (float) bufferSize * levelCount;
			}else{
				for(int cong: con->congestion.at(nodePos)){
					congestion+=cong;
				}
				bufferSize *= con->vcCount.at(nodePos);
				congestion = (float) congestion / (float) bufferSize * levelCount;
			}

			if (congestion == levelCount) {
				congestion--;
			}
			if (congestion >= mostCongestion) {
					mostCongestion = congestion;
			}
			//cout<<n->id<<DIR::toString(dir)<<c.vc<<" "<<congestion<<endl;
			n = con->nodes.at(nodePos);
		}
		if (mostCongestion <= leastCongestion) {
			leastCongestion = mostCongestion;
			candidates[c]=mostCongestion;
		}
		//cout<<"all"<<DIR::toString(dir)<<c.vc<<" "<<mostCongestion<<endl;
	}
	//cout<<leastCongestion<<endl;
	for (std::pair<Channel, int> candidate : candidates) {
		if (candidate.second == leastCongestion) {
			channel.insert(candidate.first);
		}
	}

	return channel;
}

