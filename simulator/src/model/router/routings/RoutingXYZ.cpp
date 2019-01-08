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
#include "RoutingXYZ.h"

RoutingXYZ::RoutingXYZ(Node& node)
        :
        Routing(node)
{
}

void RoutingXYZ::checkValid()
{
    node.checkValid();
}

void RoutingXYZ::route(RoutingInformation* ri, RoutingPacketInformation* rpi)
{
    std::set<Channel> channel{};
    std::map<Channel, float> channelRating;
    Vec3D<float> dstPos = rpi->packet->dst.pos;
    Helper helper{};

    if (dstPos==node.pos) {
        channel = {Channel(node.getConPosOfDir(DIR::Local), 0)};
    }
    else if (dstPos.x<node.pos.x) {
        channel = helper.getChannelsWithConPos({node.getConPosOfDir(DIR::West)},
                ri->allChannelWithoutLocal);
    }
    else if (dstPos.x>node.pos.x) {
        channel = helper.getChannelsWithConPos({node.getConPosOfDir(DIR::East)},
                ri->allChannelWithoutLocal);
    }
    else if (dstPos.y<node.pos.y) {
        channel = helper.getChannelsWithConPos({node.getConPosOfDir(DIR::South)},
                ri->allChannelWithoutLocal);
    }
    else if (dstPos.y>node.pos.y) {
        channel = helper.getChannelsWithConPos({node.getConPosOfDir(DIR::North)},
                ri->allChannelWithoutLocal);
    }
    else if (dstPos.z<node.pos.z) {
        channel = helper.getChannelsWithConPos({node.getConPosOfDir(DIR::Down)},
                ri->allChannelWithoutLocal);
    }
    else if (dstPos.z>node.pos.z) {
        channel = helper.getChannelsWithConPos({node.getConPosOfDir(DIR::Up)},
                ri->allChannelWithoutLocal);
    }

    for (Channel c : channel) {
        channelRating[c] = 1;
    }

    rpi->routedChannel = channel;
    rpi->routedChannelRating = channelRating;
}

void RoutingXYZ::makeDecision(RoutingInformation* ri, RoutingPacketInformation* rpi)
{
    if (!rpi->recentSelectedChannel.empty()) {
        rpi->outputChannel = *rpi->recentSelectedChannel.begin();
    }
    else {
        FATAL("Router" << ri->node.id << "[" << DIR::toString(ri->node.getDirOfConPos(rpi->inputChannel.conPos))
                       << rpi->inputChannel.vc << "] - Unable to make decision! " << *rpi->packet);
    }
}

void RoutingXYZ::beginCycle(RoutingInformation* ri)
{
}

void RoutingXYZ::endCycle(RoutingInformation* ri)
{
}

