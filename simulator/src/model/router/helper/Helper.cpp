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
#include "Helper.h"

std::set<Channel> Helper::getAllChannels(const Node& node)
{
    std::set<Channel> channel{};
    for (auto& cID : node.connections) {
        Connection c = globalResources.connections.at(cID);
        for (int vc = 0; vc<c.getVCCountForNode(node.id); vc++) {
            channel.emplace(node.getConnPosition(cID), vc);
        }
    }
    return channel;
}

std::set<Channel> Helper::getAllChannelsWithoutLocal(const Node& node)
{
    std::set<Channel> channel{};
    for (auto& cID : node.connections) {
        Connection con = globalResources.connections.at(cID);
        if (node.getDirOfCon(cID)!=DIR::Local) {
            for (int vc = 0; vc<con.getVCCountForNode(node.id); vc++) {
                channel.emplace(node.getConnPosition(cID), vc);
            }
        }
    }
    return channel;
}

std::set<Channel> Helper::getChannelsWithVC(const std::set<int>& vc, const std::set<Channel>& availableChannel)
{
    std::set<Channel> channel{};
    for (Channel ch : availableChannel) {
        if (vc.count(ch.vc)) {
            channel.insert(ch);
        }
    }
    return channel;
}

std::set<Channel> Helper::getChannelsWithConPos(const std::set<int>& conPos, const std::set<Channel>& availableChannel)
{
    std::set<Channel> channel{};
    for (Channel ch : availableChannel) {
        if (conPos.count(ch.conPos)) {
            channel.insert(ch);
        }
    }
    return channel;
}
