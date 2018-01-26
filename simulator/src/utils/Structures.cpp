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

const std::vector<DIR::TYPE> DIR::XYZ = { DIR::Local, DIR::East, DIR::West,
		DIR::North, DIR::South, DIR::Up, DIR::Down };

SyntheticPhase::SyntheticPhase(std::string nm, std::string sd, int max, int min,
		int wc, int ppw, int wd, int hs) :
		name(nm), spatialDistribution(sd), maxOffset(max), minOffset(min), waveCount(
				wc), pkgPerWave(ppw), waveDelay(wd), hotspot(hs) {

}

NodeType::NodeType(int id, std::string model, int clk) :
		id(id), routerModel(model), clockSpeed(clk) {

}

LayerType::LayerType(int id, int technology) :
		id(id), technology(technology) {

}

Node::Node(int id, Vec3D<float> pos, NodeType* type, LayerType* layer) :
		id(id), pos(pos), type(type), layer(layer) {
}

Connection::Connection(int id, std::vector<Node*> nodes,
		std::vector<int> vcCount, std::vector<int> bufferDepth, float length,
		int linkWidth, int linkDepth, float effectiveCapacityCl,
		float wireCouplingCapacitanceCc, float wireSelfCapacitanceCg,
		float wireSelfCapacitancePerUnitLengthCg,
		float tsvarraySelfCapacitanceC0, float tsvarrayNeighbourCapacitanceCd,
		float tsvarrayDiagonalCapacitanceCn, float tsvarrayEdgeCapacitanceCe) :
		id(id), nodes(nodes), vcCount(vcCount), bufferDepth(bufferDepth), length(
				length), linkWidth(linkWidth), linkDepth(linkDepth), effectiveCapacityCl(
				effectiveCapacityCl), wireCouplingCapacitanceCc(
				wireCouplingCapacitanceCc), wireSelfCapacitanceCg(
				wireSelfCapacitanceCg), wireSelfCapacitancePerUnitLengthCg(
				wireSelfCapacitancePerUnitLengthCg), tsvarraySelfCapacitanceC0(
				tsvarraySelfCapacitanceC0), tsvarrayNeighbourCapacitanceCd(
				tsvarrayNeighbourCapacitanceCd), tsvarrayDiagonalCapacitanceCn(
				tsvarrayDiagonalCapacitanceCn), tsvarrayEdgeCapacitanceCe(
				tsvarrayEdgeCapacitanceCe) {
}

int Connection::getBufferDepthForNode(Node* n) {
	assert(nodePos.count(n) && "Node not found inside connection! (buffer)");
	return bufferDepth.at(nodePos.at(n));
}

int Connection::getVCCountForNode(Node* n) {
	assert(nodePos.count(n) && "Node not found inside connection! (vc)");
	return vcCount.at(nodePos.at(n));
}
