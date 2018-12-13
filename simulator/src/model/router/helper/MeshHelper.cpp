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

#include "MeshHelper.h"

long MeshHelper::getHopDistance(const Node& n1, const Node& n2)
{
    std::vector<float>* xPos = &GlobalResources::getInstance().xPositions;
    std::vector<float>* yPos = &GlobalResources::getInstance().yPositions;
    std::vector<float>* zPos = &GlobalResources::getInstance().zPositions;

    long xDist = std::distance(std::find(xPos->begin(), xPos->end(), n1.pos.x),
            std::find(xPos->begin(), xPos->end(), n2.pos.x));
    long yDist = std::distance(std::find(yPos->begin(), yPos->end(), n1.pos.y),
            std::find(yPos->begin(), yPos->end(), n2.pos.y));
    long zDist = std::distance(std::find(zPos->begin(), zPos->end(), n1.pos.z),
            std::find(zPos->begin(), zPos->end(), n2.pos.z));

    return (abs(xDist)+
            abs(yDist)+
            abs(zDist));
}

//exclude dir??
ChannelInfo MeshHelper::getRecursiveChannelInfo(ChannelInfo info)
{
    if (info.currNode.id == info.destinationNode.id || info.hop>=info.maxHops) {
        info.minimalCongestion = info.congestion;
        info.averageCongestion = info.congestion;
        info.minPathToDestDistance = getHopDistance(info.currNode, info.destinationNode);
        info.maxDistanceToOriginWithMinPathToDest = getHopDistance(info.currNode, info.sourceNode);
        info.availablePaths = 1;

        if (info.onMinimalPath) {
            info.minimalPaths = 1;
        }
        else {
            info.nonminimalPaths = 1;
        }

        return info;
    }

    Connection* con = info.currNode->connections.at(info.channel.conPos);
    Node* connectedNode = info.currNode->conToNode.at(info.channel.conPos);
    int nodeNum = con->nodePos.at(connectedNode);
    long oldDistanceToDest = getHopDistance(info.currNode, info.destinationNode);
    long newDistanceToDest = getHopDistance(connectedNode, info.destinationNode);

    if (newDistanceToDest>=oldDistanceToDest) {
        info.onMinimalPath = false;
        info.nonMinimalCount++;
        //return info;
    }

    std::set<Channel> channel = getAllChannelsWithoutLocal(connectedNode);
    channel = MeshHelper::getChannelsWithVC({0}, channel);

    //	if(!info.forbiddenTurns.empty()){
    //		channel = MeshHelper::filterTurns(info.forbiddenTurns,info.channel, channel);
    //	}
    //std::set<Channel> minimalChannel = getMinimalChannels(connectedNode, info.destinationNode, channel);

    if (channel.empty() && connectedNode!=info.destinationNode && info.hop+1<info.maxHops) {
        info.availablePaths = 0;
        info.minimalPaths = 0;
        info.nonminimalPaths = 0;
        info.minimalCongestion = -1;
        info.averageCongestion = -1;
        info.minPathToDestDistance = -1;
        info.maxDistanceToOriginWithMinPathToDest = -1;
        return info;
    }

    info.congestion +=
            connectedNode->congestion*info.congestionFactor;//con->bufferCongestion.at(nodeNum) * info.congestionFactor;
    info.currNode = connectedNode;
    info.congestionFactor *= info.decreaseFactor;
    info.hop++;

    ChannelInfo result(info.sourceNode, info.destinationNode, info.channel, info.maxHops);
    if (connectedNode==info.destinationNode || info.hop>=info.maxHops) {
        result.combine(getRecursiveChannelInfo(info));
    }
    else {
        for (Channel c : channel) {
            info.channel = c;
            result.combine(getRecursiveChannelInfo(info));
        }
    }

    return result;
}

MeshHelper::MeshHelper() = default;
