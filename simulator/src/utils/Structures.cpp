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
#include "Structures.h"

const std::vector<DIR::TYPE> DIR::XYZ = {DIR::Local, DIR::East, DIR::West, DIR::North, DIR::South, DIR::Up, DIR::Down};

Channel::Channel() :
        dir(0),
        vc(0) {
}

Channel::Channel(int dir, int vc) :
        dir(dir),
        vc(vc) {
}

bool Channel::operator<(const Channel &c) const {
    if (dir != c.dir) {
        return dir < c.dir;
    } else if (vc != c.vc) {
        return vc < c.vc;
    }
    return false;
}

NodeType::NodeType(int id, std::string model, std::string routing,
                   std::string selection, int clk, std::string arbiterType) :
        id(id),
        routerModel(model),
        routing(routing),
        selection(selection),
        clockDelay(clk),
        arbiterType(arbiterType) {
}

/*LayerType::LayerType(int id, int technology) : TODO restructure
        id(id),
        technology(technology) {

}*/

Node::Node(int id, Vec3D<float> pos, const NodeType &type) :
        id(id),
        pos(pos),
        type(type),
        congestion(0.0) {
}

Connection::Connection(int id, std::vector<int> nodes, std::vector<int> vcsCount, std::vector<int> buffersDepth,
                       std::vector<std::vector<int>> buffersDepths, float length, int width, int depth) :
        id(id),
        nodes(nodes),
        vcsCount(vcsCount),
        buffersDepth(buffersDepth),
        buffersDepths(buffersDepths),
        length(length),
        width(width),
        depth(depth) {
}

int Connection::getBufferDepthForNode(int node) {
    // assert(nodePos.count(n) && "Node not found inside connection! (buffer)");  TODO restructure
    return buffersDepth.at(node);
}

int Connection::getBufferDepthForNodeAndVC(int node, int vc) {
    // assert(nodePos.count(n) && "Node not found inside connection! (buffer)");  TODO restructure
    return buffersDepths.at(node).at(vc);
}

int Connection::getVCCountForNode(int node) {
    // assert(nodePos.count(n) && "Node not found inside connection! (vc)");  TODO restructure
    return vcsCount.at(node);
}

DataType::DataType(int id, std::string name) :
        id(id),
        name(name) {
}

DataRequirement::DataRequirement(int id, int dataType) :
        id(id),
        dataType(dataType),
        minCount(-1),
        maxCount(-1) {
}

DataDestination::DataDestination(int id, int dataType, int destinationTask, int minInterval, int maxInterval) :
        id(id),
        dataType(dataType),
        destinationTask(destinationTask),
        minInterval(minInterval),
        maxInterval(maxInterval),
        minCount(-1),
        maxCount(-1),
        minDelay(0),
        maxDelay(0) {
}

DataSendPossibility::DataSendPossibility(int id, float probability, std::vector<int> dataDestinations) :
        id(id),
        probability(probability),
        dataDestinations(dataDestinations) {
}

Task::Task(int id, int node, std::vector<int> requirements, std::vector<int> possibilities) :
        id(id),
        node(node),
        requirements(requirements),
        possibilities(possibilities),
        syntheticPhase(0),
        minStart(0),
        maxStart(0),
        minDuration(-1),
        maxDuration(-1),
        minRepeat(-1),
        maxRepeat(-1) {
}

SyntheticPhase::SyntheticPhase(int id, std::string name, std::string distribution, float injectionRate) :
        id(id),
        name(name),
        distribution(distribution),
        injectionRate(injectionRate),
        minStart(0),
        maxStart(0),
        minDuration(-1),
        maxDuration(-1),
        minRepeat(-1),
        maxRepeat(-1),
        minCount(-1),
        maxCount(-1),
        minDelay(0),
        maxDelay(0),
        hotspot(-1) {

}