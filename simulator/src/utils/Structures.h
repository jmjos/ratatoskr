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


struct DIR {
    const static int size = 7;

    enum TYPE {
        Local = 0, East = 1, West = 2, North = 3, South = 4, Up = 5, Down = 6
    };

    static const std::vector<TYPE> XYZ;

    static TYPE toDir(int a) {
        return static_cast<TYPE>(a);
    }

    static std::string toString(TYPE a) {
        switch (a) {
            case Local:
                return "Local";
            case East:
                return "East";
            case West:
                return "West";
            case North:
                return "North";
            case South:
                return "South";
            case Up:
                return "Up";
            case Down:
                return "Down";
            default:
                return "Unknown Direction!!!";
        }
    }

    static std::string toString(int a) {
        return toString(toDir(a));
    }

    static int getOppositeDir(int d) {
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
    int dir;
    int vc;

    Channel();

    Channel(int dir, int vc);

    bool operator<(const Channel &a) const;
};

template<typename T>
struct Vec3D {
    T x = 0, y = 0, z = 0;

    Vec3D() {
    }

    Vec3D(T x, T y, T z) :
            x(x), y(y), z(z) {
    }

    bool operator<(Vec3D<T> v) const {
        return fabs(x) < fabs(v.x) ||
               (fabs(x) == fabs(v.x) && (fabs(y) < fabs(v.y) || (fabs(y) == fabs(v.y) && fabs(z) < fabs(v.z))));
    }

    Vec3D<T> operator+(const Vec3D<T> v) const {
        return Vec3D<T>(x + v.x, y + v.y, z + v.z);
    }

    Vec3D<T> operator-(const Vec3D<T> v) const {
        return Vec3D<T>(x - v.x, y - v.y, z - v.z);
    }

    float norm() const {
        return ((x * x) + (y * y) + (z * z));
    }

    float distance(const Vec3D<T> v) const {
        float disx = fabs(x - v.x);
        float disy = fabs(y - v.y);
        float disz = fabs(z - v.z);
        return sqrt((disx * disx) + (disy * disy) + (disz * disz));
    }

    float sameDimDistance(const Vec3D<T> v) const {
        float disx = fabs(x - v.x);
        float disy = fabs(y - v.y);
        return sqrt((disx * disx) + (disy * disy));
    }

    bool operator==(const Vec3D<T> v) const {
        return ((x == v.x) && (y == v.y) && (z == v.z));
    }

    int sameDimCount(const Vec3D<T> v) const {
        int count = 0;
        if (x == v.x) {
            count++;
        }
        if (y == v.y) {
            count++;
        }
        if (z == v.z) {
            count++;
        }
        return count;
    }

    int diffDimCount(const Vec3D<T> v) const {
        int count = 0;
        if (x != v.x) {
            count++;
        }
        if (y != v.y) {
            count++;
        }
        if (z != v.z) {
            count++;
        }
        return count;
    }

    friend ostream &operator<<(ostream &output, const Vec3D<T> &v) {
        output << "(" << v.x << ", " << v.y << ", " << v.z << ")";
        return output;
    }
};

struct NodeType {
    int id;
    std::string routerModel;
    std::string routing;
    std::string selection;
    int clockDelay;
    std::string arbiterType;
    // std::vector<Node*> nodes; TODO restructure

    NodeType(int id, std::string routerModel, std::string routing, std::string selection, int clkDelay,
             std::string arbiterType);
};

/*struct LayerType { TODO restructure
	int id;
	LayerType(int id, int technology);
};*/

struct Node {
    int id;
    Vec3D<float> pos;
    const NodeType &type;
    float congestion; // crossbar utilization 0-1
    std::vector<int> connectedNodes;
    std::vector<int> connections;
    std::map<DIR::TYPE, int> dirToCon; //maps direction names to connection number
    std::map<int, DIR::TYPE> conToDir; //maps connection number to direction name

    /* int idType; TODO restructure
     LayerType* layer;
     std::vector<Node *> connectedNodes;
     std::vector<Connection *> connections;
     std::map<Node *, std::vector<int>> connectionsToNode; //get connection by connected node
     std::map<Connection *, int> conToPos; // get position of connection inside array
     */
    Node(int id, Vec3D<float> pos, const NodeType &type); //, LayerType* layer); TODO restructure
};

struct Connection {
    int id;
    std::vector<int> nodes; //TODO restructure Node* to int
    std::vector<int> vcsCount;  // vc count for each end.
    std::vector<int> buffersDepth;  // one buffer depth for all ends of a connection.
    std::vector<std::vector<int>> buffersDepths;  // one buffer depth per end.
    float length;
    int width;
    int depth;

    std::vector<std::vector<int>> vcBufferUtilization; // for the lazy folks ...
    std::vector<int> bufferUtilization;
    std::vector<std::vector<float>> vcBufferCongestion; // buffer utilization 0-1
    std::vector<float> bufferCongestion;
    //std::map<Node*, int> nodePos; // get position of node inside the above vectors TODO restructure

    Connection(int id, std::vector<int> nodes, std::vector<int> vcsCount, std::vector<int> buffersDepth,
               std::vector<std::vector<int>> buffersDepths, float length, int width, int depth);

    int getBufferDepthForNode(int node);

    int getBufferDepthForNodeAndVC(int node, int vc);

    int getVCCountForNode(int node);
};

struct DataType {
    int id;
    std::string name;

    DataType(int id, std::string name);
};

struct DataRequirement {
    int id;
    int dataType;
    int minCount;    //required amount of DataType to fulfill requirement (to fire)
    int maxCount;

    DataRequirement(int id, int dataType);
};

struct DataDestination {
    int id;
    int dataType;
    int destinationNode;    //fyi: "task" in XML ??
    int destinationTask;    //fyi: "task" in XML ??
    int minInterval;        //delay between each sent packet
    int maxInterval;
    int minCount;           //generated amount of packets per fire
    int maxCount;
    int minDelay;           //delay between fire and first generated packet
    int maxDelay;

    DataDestination(int id, int dataType, int destinationTask, int minInterval, int maxInterval);
};

struct DataSendPossibility {
    int id;
    float probability;
    std::vector<int> dataDestinations;

    DataSendPossibility(int id, float probability, std::vector<int> dataDestinations);
};

struct Task {
    int id;
    int node;
    std::vector<int> requirements;
    // std::vector<std::pair<float, std::vector<DataDestination*>>> possibilities;
    std::vector<int> possibilities;
    int syntheticPhase;
    int minStart;       // simulation time in ns at which the task starts
    int maxStart;
    int minDuration;    // maximal task duration in ns
    int maxDuration;
    int minRepeat;      // maximal task execution count
    int maxRepeat;      // task terminates at whatever comes first, maxRepeates or duration

    Task(int id, int nodeID, std::vector<int> requirements, std::vector<int> possibilities);
};

struct SyntheticPhase {
    int id;
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

    SyntheticPhase(int id, std::string name, std::string distribution, float injectionRate);
};

