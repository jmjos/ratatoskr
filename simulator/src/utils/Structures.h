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

#define LOG(x,y) { std::ostringstream oss; oss<<y; Report::getInstance().log(x,oss.str());}
#define FATAL(x) { LOG(true,x); std::cout<<"Terminating"<<std::endl; Report::getInstance().close(); exit(EXIT_FAILURE);}
#define FATALCASE(x,y) { if(x){LOG(true,y); std::cout<<"Terminating"<<std::endl; Report::getInstance().close(); exit(EXIT_FAILURE);}}

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

	static std::string toString(int a) {
		return toString(toDir(a));
	}
};

struct Channel {
	int dir;
	int vc;

	Channel() {
		this->dir = 0;
		this->vc = 0;
	}
	;

	Channel(int dir, int vc) {
		this->dir = dir;
		this->vc = vc;
	}
	;

	bool operator<(const Channel &a) const {
		if (dir != a.dir) {
			return dir < a.dir;
		}

		else if (vc != a.vc) {
			return vc < a.vc;
		}
		return false;
	}

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
		return fabs(x) < fabs(v.x)
				|| (fabs(x) == fabs(v.x)
						&& (fabs(y) < fabs(v.y)
								|| (fabs(y) == fabs(v.y) && fabs(z) < fabs(v.z))));
	}

	Vec3D<T> operator +(const Vec3D<T> v) const {
		return Vec3D<T>(x + v.x, y + v.y, z + v.z);
	}

	Vec3D<T> operator -(const Vec3D<T> v) const {
		return Vec3D<T>(x - v.x, y - v.y, z - v.z);
	}

	float norm() const{
		return ((x*x)+(y*y)+(z*z));
	}

	float distance (const Vec3D<T> v) const {
		float disx = fabs(x-v.x);
		float disy = fabs(y-v.y);
		float disz = fabs(z-v.z);
		return sqrt((disx*disx)+(disy*disy)+(disz*disz));
	}

	float sameDimDistance (const Vec3D<T> v) const {
			float disx = fabs(x-v.x);
			float disy = fabs(y-v.y);
			return sqrt((disx*disx)+(disy*disy));
	}

	bool operator ==(const Vec3D<T> v) const {
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

	//bool operator<(const Vec3D<T> v) const {
	//	return (pow(x,2)+pow(y,2)+pow(z,2))<(pow(v.x,2)+pow(v.y,2)+pow(v.z,2));
	//}

	friend ostream &operator<<(ostream &output, const Vec3D<T> &v) {
		output << "(" << v.x << ", " << v.y << ", " << v.z << ")";
		return output;
	}
};

struct Node;

struct NodeType {
	int id;
	std::string routerModel;
	std::string routing;
	std::string selection;
	std::vector<Node*> nodes;
	//buffer model
	//Allocator model ...
	int clockSpeed;
	std::string arbiterType;


	NodeType(int id, std::string model, std::string routing,
			std::string selection, int clk, std::string arbiterType);
};

struct LayerType {
	int id;
	int technology;
	//layer dependent attributes, e.g.
	//capacity formula, heat distribution

	LayerType(int id, int technology);
};

struct Connection;

struct Node {
	int id;
	Vec3D<float> pos;
	int idType;
	NodeType* type;
	LayerType* layer;

	std::vector<Node*> connectedNodes;
	std::vector<Connection*> connections;
	std::map<Node*, std::vector<int>> connectionsToNode; //get connection by connected node
	std::map<Connection*, int> conToPos; // get position of connection inside array
	std::map<DIR::TYPE, int> dirToCon; //maps direction names to connection number
	std::map<int, DIR::TYPE> conToDir; //maps connection number to direction name
	std::map<int, Node*> conToNode; //maps connection number to node

	float congestion; // crossbar utilization 0-1

	Node(int id, Vec3D<float> pos, int idType, NodeType* type,
			LayerType* layer);

};

struct Connection {
	int id;
	std::vector<Node*> nodes;
	std::vector<int> vcCount;
	std::vector<int> bufferDepth;

	std::vector<std::vector<int>> vcBufferUtilization; // for the lazy folks ...
	std::vector<int> bufferUtilization;

	std::vector<std::vector<float>> vcBufferCongestion; // buffer utilization 0-1
	std::vector<float> bufferCongestion;

	std::map<Node*, int> nodePos; // get position of node inside the above vectors
	//connection dependent attributes, e.g.
	//thickness, calculated capacitance,
	//buffer size per connection
	float length;
	int linkWidth;
	int linkDepth;

	Connection(int id, std::vector<Node*> nodes, std::vector<int> vcCount,
			std::vector<int> bufferDepth, float length, int linkWidth,
			int linkDepth);
	int getBufferDepthForNode(Node* n);
	int getVCCountForNode(Node* n);
};

struct SyntheticPhase {
	std::string name;
	std::string distribution;
	int minStart = 0;
	int maxStart = 0;
	int minDuration = -1;
	int maxDuration = -1;
	int minRepeat = -1;
	int maxRepeat = -1;
	int minCount = -1;
	int maxCount = -1;
	int minDelay = 0;
	int maxDelay = 0;
	int minInterval;
	int maxInterval;
	int hotspot = -1;

	SyntheticPhase(std::string name, std::string distribution, int minInterval, int maxInterval): name(name), distribution(distribution), minInterval(minInterval), maxInterval(maxInterval){

	}
};


struct DataType {
	int id;
	std::string name;

	DataType(int id, std::string name) :
			id(id), name(name) {
	}

};

struct DataRequirement {
	int id;
	DataType* type;
	//Node* src = 0;			//requires the data to be from this node (optional)

	int minCount = -1;		//required amount of DataType to fulfill requirement (to fire)
	int maxCount = -1;

	DataRequirement(int id, DataType* type) : id(id), type(type) {
	}
};

struct DataDestination {
	int id;
	DataType* type;
	Node* destination;

	int minCount = -1;		//generated amount of packets per fire
	int maxCount = -1;

	int minDelay = 0;		//delay between fire and first generated packet
	int maxDelay = 0;

	int minInterval;		//delay between each sent packet
	int maxInterval;

	DataDestination(int id, DataType* type, Node* destination, int minInterval, int maxInterval) :
			id(id), type(type), destination(destination), minInterval(minInterval), maxInterval(maxInterval) {

	}
};

struct Task {
	int id;
	Node* node;

	int minStart = 0;		// simulation time in ns at which the task starts
	int maxStart = 0;
	int minDuration = -1;	// maximal task duration in ns
	int maxDuration = -1;
	int minRepeat = -1;		// maximal task execution count
	int maxRepeat = -1;		// task terminates at whatever comes first, maxRepeates or duration

	std::vector<DataRequirement*> requirements;
	std::vector<std::pair<float, std::vector<DataDestination*>>> possibilities;

	Task(int id, Node* node) :
			id(id), node(node) {
	}
};

