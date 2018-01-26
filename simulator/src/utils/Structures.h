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
#ifndef _STRUCTURES_H_
#define _STRUCTURES_H_

//includes
#include "systemc.h"
#include <vector>
#include <string>
#include <sstream>
#include <map>
#include "Report.h"

#define LOG(x,y) { std::ostringstream oss; oss<<y; Report::getInstance().log(x,oss.str());}
#define FATAL(x) { LOG(true,x); std::cout<<"Terminating"<<std::endl; exit(EXIT_FAILURE);}

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
	;
	Vec3D(T x, T y, T z) :
			x(x), y(y), z(z) {
	}
	;

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

	bool operator ==(const Vec3D<T> v) const {
		return ((x == v.x) && (y == v.y) && (z == v.z));
	}

	//bool operator<(const Vec3D<T> v) const {
	//	return (pow(x,2)+pow(y,2)+pow(z,2))<(pow(v.x,2)+pow(v.y,2)+pow(v.z,2));
	//}

	friend ostream &operator<<(ostream &output, const Vec3D<T> &v) {
		output << "(" << v.x << ", " << v.y << ", " << v.z << ")";
		return output;
	}
};

struct SyntheticPhase {
	std::string name, spatialDistribution;
	int maxOffset, minOffset;
	int waveCount, pkgPerWave, waveDelay;
	int hotspot;

	SyntheticPhase(std::string nm, std::string sd, int max, int min, int wc,
			int ppw, int wd, int hs);

};

struct NodeType {
	int id;
	std::string routerModel;
	//buffer model
	//Allocator model ...
	int clockSpeed;

	NodeType(int id, std::string model, int clk);
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
	NodeType* type;
	LayerType* layer;

	std::vector<Node*> connectedNodes;
	std::vector<Connection*> connections;
	std::map<Node*, std::vector<int>> connectionsToNode; //get connection by connected node
	std::map<Connection*, int> conToPos; // get position of connection inside array
	std::map<DIR::TYPE, int> dirToCon; //maps direction names to connection number
	std::map<int, DIR::TYPE> conToDir; //maps connection number to direction name

	Node(int id, Vec3D<float> pos, NodeType* type, LayerType* layer);

};

struct Connection {
	int id;
	std::vector<Node*> nodes;
	std::vector<int> vcCount;
	std::vector<int> bufferDepth;
	std::vector<std::vector<int>> congestion; // for the lazy folks ...
	std::map<Node*, int> nodePos; // get position of node inside the above vectors
	//connection dependent attributes, e.g.
	//thickness, calculated capacitance,
	//buffer size per connection
	float length;
	int linkWidth;
	int linkDepth;
	float effectiveCapacityCl;
	float wireCouplingCapacitanceCc;
	float wireSelfCapacitanceCg;
	float wireSelfCapacitancePerUnitLengthCg;
	float tsvarraySelfCapacitanceC0;
	float tsvarrayNeighbourCapacitanceCd;
	float tsvarrayDiagonalCapacitanceCn;
	float tsvarrayEdgeCapacitanceCe;

	Connection(int id, std::vector<Node*> nodes, std::vector<int> vcCount,
			std::vector<int> bufferDepth, float length, int linkWidth,
			int linkDepth, float effectiveCapacityCl,
			float wireCouplingCapacitanceCc, float wireSelfCapacitanceCg,
			float wireSelfCapacitancePerUnitLengthCg,
			float tsvarraySelfCapacitanceC0,
			float tsvarrayNeighbourCapacitanceCd,
			float tsvarrayDiagonalCapacitanceCn,
			float tsvarrayEdgeCapacitanceCe);
	int getBufferDepthForNode(Node* n);
	int getVCCountForNode(Node* n);
};

#endif
