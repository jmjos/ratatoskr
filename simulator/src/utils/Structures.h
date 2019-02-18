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
#pragma once

#include "systemc.h"
#include <vector>
#include <string>
#include <sstream>
#include <map>

#include "Report.h"

using nodeID_t = int;
using connID_t = int;
using nodeTypeID_t = int;
using dataTypeID_t = int;
using dataReqID_t = int;
using dataDestID_t = int;
using possID_t = int;
using taskID_t = int;
using synthID_t = int;

struct DIR {
    const static int size = 7;

    enum TYPE {
        Local = 0, East = 1, West = 2, North = 3, South = 4, Up = 5, Down = 6
    };

    static const std::vector<TYPE> XYZ;

    static TYPE toDir(int a)
    {
        return static_cast<TYPE>(a);
    }

    static std::string toString(TYPE a)
    {
        switch (a) {
        case Local:
            return "Local";
        case East:
            return " East";
        case West:
            return " West";
        case North:
            return "North";
        case South:
            return "South";
        case Up:
            return "   Up";
        case Down:
            return " Down";
        default:
            return "Unknown Direction!!!";
        }
    }

    static std::string toString(int a)
    {
        return toString(toDir(a));
    }

    static int getOppositeDir(int d)
    {
        switch (d) {
        case 0:
            return 0;
        case 1:
            return 2;
        case 2:
            return 1;
        case 3:
            return 4;
        case 4:
            return 3;
        case 5:
            return 6;
        case 6:
            return 5;
        default:
            return 0;
        }
    }
};

struct Channel {
    int conPos;
    int vc;

    Channel();

    Channel(int dir, int vc);

    bool operator<(const Channel& a) const;

    bool operator==(const Channel& a) const;

    bool operator!=(const Channel& a) const;

    friend ostream& operator<<(ostream& os, const Channel& ch);
};

template<typename T>
struct Vec3D {
    T x = 0, y = 0, z = 0;

    Vec3D() = default;

    Vec3D(T x, T y, T z)
            :
            x(x), y(y), z(z)
    {
    }

    bool operator<(Vec3D<T> v) const
    {
        return fabs(x)<fabs(v.x) ||
                (fabs(x)==fabs(v.x) && (fabs(y)<fabs(v.y) || (fabs(y)==fabs(v.y) && fabs(z)<fabs(v.z))));
    }

    Vec3D<T> operator+(const Vec3D<T> v) const
    {
        return Vec3D<T>(x+v.x, y+v.y, z+v.z);
    }

    Vec3D<T> operator-(const Vec3D<T> v) const
    {
        return Vec3D<T>(x-v.x, y-v.y, z-v.z);
    }

    double norm() const
    {
        return ((x*x)+(y*y)+(z*z));
    }

    double distance(const Vec3D<T> v) const
    {
        double disx = fabs(x-v.x);
        double disy = fabs(y-v.y);
        double disz = fabs(z-v.z);
        return sqrt((disx*disx)+(disy*disy)+(disz*disz));
    }

    double sameDimDistance(const Vec3D<T> v) const
    {
        double disx = fabs(x-v.x);
        double disy = fabs(y-v.y);
        return sqrt((disx*disx)+(disy*disy));
    }

    bool operator==(const Vec3D<T> v) const
    {
        return ((x==v.x) && (y==v.y) && (z==v.z));
    }

    int sameDimCount(const Vec3D<T> v) const
    {
        int count = 0;
        if (x==v.x) {
            count++;
        }
        if (y==v.y) {
            count++;
        }
        if (z==v.z) {
            count++;
        }
        return count;
    }

    int diffDimCount(const Vec3D<T> v) const
    {
        int count = 0;
        if (x!=v.x) {
            count++;
        }
        if (y!=v.y) {
            count++;
        }
        if (z!=v.z) {
            count++;
        }
        return count;
    }

    friend ostream& operator<<(ostream& output, const Vec3D<T>& v)
    {
        output << "(" << v.x << ", " << v.y << ", " << v.z << ")";
        return output;
    }

    bool isZero() const{
        return x==0 && y==0 && z==0;
    }
};

struct NodeType {
    nodeTypeID_t id;
    std::string model;
    std::string routing;
    std::string selection;
    int clockDelay;
    std::string arbiterType;
    // std::vector<Node*> nodes; TODO restructure

    NodeType(nodeTypeID_t id, const std::string& model, const std::string& routing, const std::string& selection,
            int clkDelay, const std::string& arbiterType);
};

/*struct LayerType { TODO restructure
	int id;
	LayerType(int id, int technology);
};*/

class Node {
public:
    nodeID_t id;
    Vec3D<float> pos;
    std::shared_ptr<NodeType> type;
    float congestion; // crossbar utilization 0-1
    std::vector<nodeID_t> connectedNodes;
    std::vector<connID_t> connections;

    /* int idType; TODO restructure
     LayerType* layer;
     std::map<Node *, std::vector<int>> connectionsToNode; //get connection by connected node
     std::map<connID_t, int> conToPos; // get position of connection inside array
     */
    Node(nodeID_t id, Vec3D<float> pos, const std::shared_ptr<NodeType>& type);

    Node();

    int getConnPosition(connID_t connID) const;

    int getConPosOfDir(DIR::TYPE dir) const;

    void setConPosOfDir(DIR::TYPE dir, connID_t connID);

    DIR::TYPE getDirOfCon(connID_t connID) const;

    DIR::TYPE getDirOfConPos(int conPos) const;

    int getConPosOfId(connID_t connID) const;

    void setDirOfConn(connID_t connID, DIR::TYPE dir);

    connID_t getConnWithNode(const Node& connectedNode);

    Node* getNodeByPos(const Vec3D<float>& pos);

    void checkValid();

private:
    std::map<DIR::TYPE, int> conPosOfDir; //maps direction names to connection position inside this node's connections
    std::map<int, DIR::TYPE> dirOfConPos; //maps connection position (inside this node's connections) to direction name
};

struct Connection {
    connID_t id;
    std::vector<nodeID_t> nodes;
    std::vector<int> vcsCount;  // vc count for each end.
    std::vector<int> buffersDepth;  // one buffer depth for all ends of a connection.
    std::vector<std::vector<int>> buffersDepths;  // one buffer depth per end.
    float length;
    int width;
    int depth;


    std::vector<int> bufferUtilization;

    Connection(connID_t id, const std::vector<nodeID_t>& nodes, const std::vector<int>& vcsCount,
            const std::vector<int>& buffersDepth,
            const std::vector<std::vector<int>>& buffersDepths, float length, int width, int depth);

    int getBufferDepthForNode(nodeID_t nodeID);

    int getBufferDepthForNodeAndVC(nodeID_t nodeID, int vcID);

    int getVCCountForNode(nodeID_t nodeID);

    int getNodePos(nodeID_t n_id);
};

struct DataType {
    dataTypeID_t id;
    std::string name;

    DataType(dataTypeID_t id, const std::string& name);
};

struct DataRequirement {
    dataReqID_t id;
    dataTypeID_t dataType;
    int minCount;    //required amount of DataType to fulfill requirement (to fire)
    int maxCount;

    DataRequirement(dataReqID_t id, dataTypeID_t dataType);
};

struct DataDestination {
    dataDestID_t id;
    dataTypeID_t dataType;
    //nodeID_t destinationNode;       //fyi: "task" in XML ??
    taskID_t destinationTask;       //fyi: "task" in XML ??
    int minInterval;                //delay between each sent packet
    int maxInterval;
    int minCount;                   //generated amount of packets per fire
    int maxCount;
    int minDelay;                   //delay between fire and first generated packet
    int maxDelay;

    DataDestination() = default;

    DataDestination(dataDestID_t id, dataTypeID_t dataType, nodeID_t destinationNode, int minInterval, int maxInterval);

    bool operator==(const DataDestination& dt) const;

    bool operator<(const DataDestination& dt) const;
};

struct DataSendPossibility {
    possID_t id;
    float probability;
    std::vector<DataDestination> dataDestinations;

    DataSendPossibility(possID_t id, float probability, const std::vector<DataDestination>& dataDestinations);
};

struct Task {
    taskID_t id;
    nodeID_t nodeID;
    std::vector<DataRequirement> requirements;
    std::vector<DataSendPossibility> possibilities;
    int syntheticPhase;
    int minStart;       // simulation time in ns at which the task starts
    int maxStart;
    int minDuration;    // maximal task duration in ns
    int maxDuration;
    int minRepeat;      // maximal task execution count
    int maxRepeat;      // task terminates at whatever comes first, maxRepeates or duration

    Task() = default;

    Task(taskID_t id, nodeID_t nodeID);

    Task(taskID_t id, nodeID_t nodeID, const std::vector<DataRequirement>& requirements,
            const std::vector<DataSendPossibility>& possibilities);

    bool operator==(const Task& t) const;

    bool operator<(const Task& t) const;
};

struct SyntheticPhase {
    synthID_t id;
    std::string name;
    std::string distribution;
    float injectionRate;
    int minStart;
    int maxStart;
    int minDuration;
    int maxDuration;
    int minRepeat;
    int maxRepeat;
    int minCount;
    int maxCount;
    int minDelay;
    int maxDelay;
    int hotspot;

    SyntheticPhase(synthID_t id, const std::string& name, const std::string& distribution, float injectionRate);
};

class Credit{
public:
    int id;
    static int idcnt;
    int vc;

    Credit() = default;

    explicit Credit(int vc);

    ~Credit() = default;

    bool operator==(const Credit& credit) const;

    Credit& operator=(const Credit& credit);

    friend ostream& operator<<(ostream& os, const Credit& credit) {
        return os;
    }

    friend void sc_trace(sc_trace_file*& tf, const Credit& credit, std::string nm){};
};
