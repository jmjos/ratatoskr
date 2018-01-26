/*******************************************************************************
 * Copyright (C) 2018 Jan Moritz Joseph
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
#ifndef SRC_MODEL_ROUTINGS_MESH_HELPER_H_
#define SRC_MODEL_ROUTINGS_MESH_HELPER_H_

#include "../../../utils/GlobalInputClass.h"
#include "../../../utils/Structures.h"
#include "../../../traffic/Flit.h"
#include "../../../traffic/Packet.h"
#include "../../../utils/GlobalReportClass.h"
#include "../../../utils/Report.h"
#include "../../container/Container.h"

namespace MESH {

	std::set<Channel> getAllChannel(Node* curr);
	std::set<Channel> getChannelWithVC(std::set<int> vc, std::set<Channel> availableChannel);
	std::set<Channel> getChannelWithDir(std::set<int> dir, std::set<Channel> availableChannel);
	std::set<Channel> getMinimalChannel(Node* curr, Packet* pkg, std::set<Channel> availableChannel);
	std::set<Channel> filterTurns(std::set<std::pair<Channel,Channel>> prohibitedTurns, Channel inputChannel, std::set<Channel> availableChannel);
	std::set<Channel> filterTurnsDir(std::set<std::pair<int,int>> prohibitedTurns, Channel inputChannel, std::set<Channel> availableChannel);
	std::set<Channel> getChannelWithLeastCongestion(Node* curr, int levelCount, std::set<Channel> availableChannel);
	std::set<Channel> getChannelWithLeastCongestionAlong(Node* curr, int levelCount, bool perVc,std::set<Channel> availableChannel);
};

#endif /* SRC_MODEL_ROUTINGS_MESH_HELPER_H_ */
