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
#include "Structures.h"

const std::vector<DIR::TYPE> DIR::XYZ = {DIR::Local, DIR::East, DIR::West, DIR::North, DIR::South, DIR::Up, DIR::Down};

Channel::Channel()
        :
        conPos(0),
        vc(0) {
}

Channel::Channel(int dir, int vc)
        :
        conPos(dir),
        vc(vc) {
}

bool Channel::operator<(const Channel &c) const {
    if (conPos != c.conPos) {
        return conPos < c.conPos;
    } else if (vc != c.vc) {
        return vc < c.vc;
    }
    return false;
}

bool Channel::operator==(const Channel &a) const {
    return this->conPos == a.conPos && this->vc == a.vc;
}

bool Channel::operator!=(const Channel &a) const {
    return !operator==(a);
}

ostream &operator<<(ostream &os, const Channel &ch) {
    os << "dir: " << ch.conPos << " vc: " << ch.vc;
    return os;
}

NodeType::NodeType(nodeTypeID_t id, const std::string &model, const std::string &routing,
                   const std::string &selection, int clk, const std::string &arbiterType)
        :
        id(id),
        model(model),
        routing(routing),
        selection(selection),
        clockDelay(clk),
        arbiterType(arbiterType) {
}

/*LayerType::LayerType(int id, int technology) : TODO restructure
        id(id),
        technology(technology) {
}*/

Node::Node(nodeID_t id, Vec3D<float> pos, const std::shared_ptr<NodeType> &type)
        :
        id(id),
        pos(pos),
        type(type),
        congestion(0.0) {
}

Node::Node()
        :
        id(0),
        congestion(0.0) {
}

int Node::getConnPosition(connID_t connID) const {
    return std::find(connections.begin(), connections.end(), connID) - connections.begin();
}

DIR::TYPE Node::getDirOfCon(connID_t connID) const {
    int pos = getConnPosition(connID);
    return dirOfConPos.at(pos);
}

void Node::setDirOfConn(connID_t connID, DIR::TYPE dir) {
    int pos = getConnPosition(connID);
    dirOfConPos.insert({pos, dir});
}

int Node::getConPosOfDir(DIR::TYPE dir) const {
    if (conPosOfDir.count(dir) == 1)
        return conPosOfDir.at(dir);
    else
        return -1; // so the caller can use this method in a condition
}

void Node::setConPosOfDir(DIR::TYPE dir, connID_t connID) {
    int pos = getConnPosition(connID);
    conPosOfDir.insert({dir, pos});
}

DIR::TYPE Node::getDirOfConPos(int conPos) const {
    return dirOfConPos.at(conPos);
}

int Node::getConPosOfId(connID_t connID) const{
    return std::find(connections.begin(), connections.end(), connID) - connections.begin();
}

Node *Node::getNodeByPos(const Vec3D<float> &pos) {
    if (this->pos == pos)
        return this;
    else return nullptr;
}

void Node::checkValid() {
    assert(connectedNodes.size() <= 7);
    assert(connections.size() <= connectedNodes.size() + 1);
    assert(conPosOfDir.size() == connections.size());

    int i = 0;
    for (std::pair<DIR::TYPE, int> pair : conPosOfDir) {
        assert(std::find(DIR::XYZ.begin(), DIR::XYZ.end(), pair.first) != DIR::XYZ.end());
        i++;
    }
    assert(connections.size() == i);
}

connID_t Node::getConnWithNode(const Node &connectedNode) {
    for (auto &conn1: this->connections) {
        for (auto &conn2: connectedNode.connections) {
            if (conn1 == conn2)
                return conn1;
        }
    }
}

Connection::Connection(connID_t id, const std::vector<nodeID_t> &nodes, const std::vector<int> &vcsCount,
                       const std::vector<int> &buffersDepth,
                       const std::vector<std::vector<int>> &buffersDepths, float length, int width, int depth)
        :
        id(id),
        nodes(nodes),
        vcsCount(vcsCount),
        buffersDepth(buffersDepth),
        buffersDepths(buffersDepths),
        length(length),
        width(width),
        depth(depth) {
}

int Connection::getBufferDepthForNode(nodeID_t nodeID) {
    int pos = getNodePos(nodeID);
    return buffersDepth.at(pos);
}

int Connection::getBufferDepthForNodeAndVC(nodeID_t nodeID, int vcID) {
    int pos = getNodePos(nodeID);
    return buffersDepths.at(pos).at(vcID);
}

int Connection::getVCCountForNode(nodeID_t nodeID) {
    int pos = getNodePos(nodeID);
    return vcsCount.at(pos);
}

int Connection::getNodePos(nodeID_t n_id) {
    return std::find(nodes.begin(), nodes.end(), n_id) - nodes.begin();
}

DataType::DataType(dataTypeID_t id, const std::string &name)
        :
        id(id),
        name(name) {
}

DataRequirement::DataRequirement(dataReqID_t id, dataTypeID_t dataType)
        :
        id(id),
        dataType(dataType),
        minCount(-1),
        maxCount(-1) {
}

DataDestination::DataDestination(dataDestID_t id, dataTypeID_t dataType, taskID_t destinationTask, int minInterval,
                                 int maxInterval)
        :
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

bool DataDestination::operator==(const DataDestination &dt) const {
    return this->id == dt.id;
}

bool DataDestination::operator<(const DataDestination &dt) const {
    return this->id < dt.id;
}

DataSendPossibility::DataSendPossibility(possID_t id, float probability,
                                         const std::vector<DataDestination> &dataDestinations)
        :
        id(id),
        probability(probability),
        dataDestinations(dataDestinations) {
}

Task::Task(taskID_t id, nodeID_t nodeID)
        :
        id(id),
        nodeID(nodeID) {
}

Task::Task(taskID_t id, nodeID_t node, const std::vector<DataRequirement> &requirements,
           const std::vector<DataSendPossibility> &possibilities)
        :
        id(id),
        nodeID(node),
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

bool Task::operator==(const Task &t) const {
    return this->id == t.id;
}

bool Task::operator<(const Task &t) const {
    return this->id < t.id;
}

SyntheticPhase::SyntheticPhase(synthID_t id, const std::string &name, const std::string &distribution,
                               float injectionRate)
        :
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

int Credit::idcnt = 0;

Credit::Credit(int vc)
        : vc(vc){
    id = (idcnt++) % INT_MAX;
}

bool Credit::operator==(const Credit &credit) const {
    return this->id == credit.id;
}

Credit &Credit::operator=(const Credit &credit) {
    if (this == &credit)
        return *this;
    this->id = credit.id;
    this->vc = credit.vc;
    return *this;
}
