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
#pragma once

#include "utils/GlobalResources.h"
#include "utils/Structures.h"
#include "model/traffic/Flit.h"
#include "model/traffic/Packet.h"
#include "utils/GlobalReport.h"
#include "utils/Report.h"
#include "model/container/Container.h"
#include "Helper.h"

struct ChannelInfo {
    Node sourceNode;
    Node destinationNode;
    Node currNode;
    Channel channel;
    int hop = 0;
    int maxHops = 0;
    float congestion = 0;
    float decreaseFactor = 1;
    float congestionFactor = 1;
    bool onMinimalPath = true;
    int nonMinimalCount = 0;
    int availablePaths = 0;
    int minimalPaths = 0;
    int nonminimalPaths = 0;
    float minimalCongestion = -1;
    float averageCongestion = -1;
    long minPathToDestDistance = -1;
    long maxDistanceToOriginWithMinPathToDest;

    ChannelInfo(Node& src, Node& dst, Channel channel, int maxHops)
            :
            sourceNode(src),
            destinationNode(dst),
            channel(channel),
            maxHops(maxHops),
            maxDistanceToOriginWithMinPathToDest(-1)
    {
        this->currNode = src;
    }

    void combine(ChannelInfo info)
    {
        availablePaths += info.availablePaths;
        minimalPaths += info.minimalPaths;
        nonminimalPaths += info.nonminimalPaths;

        averageCongestion = (averageCongestion+info.averageCongestion)/2;

        if (info.onMinimalPath) {
            if (minimalCongestion==-1 || (minimalCongestion>info.minimalCongestion && info.minimalCongestion!=-1)) {
                minimalCongestion = info.minimalCongestion;
            }
        }
        if (minPathToDestDistance==-1
                || (minPathToDestDistance>info.minPathToDestDistance && info.minPathToDestDistance!=-1)) {
            minPathToDestDistance = info.minPathToDestDistance;
            maxDistanceToOriginWithMinPathToDest = info.maxDistanceToOriginWithMinPathToDest;
        }
    }
};

class MeshHelper : public Helper {
public:
    ChannelInfo getRecursiveChannelInfo(ChannelInfo info);

    long getHopDistance(const Node& n1, const Node& n2);

private:
    MeshHelper();
};

