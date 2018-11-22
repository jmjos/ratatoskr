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

NodeType::NodeType(int id, std::string model, std::string routing,
		std::string selection, int clk, std::string arbiterType) :
		id(id),
		routerModel(model),
		routing(routing),
		selection(selection),
		clockDelay(clk),
		arbiterType(arbiterType)
{
}

LayerType::LayerType(int id, int technology) :
		id(id),
		technology(technology)
{

}

Node::Node(int id, Vec3D<float> pos, int idType, NodeType* type, LayerType* layer) :
		id(id),
		pos(pos),
		idType(idType),
		type(type),
		layer(layer),
		congestion(0.0)
{
}

Connection::Connection(int id, std::vector<Node*> nodes,
		std::vector<int> vcCount, std::vector<int> bufferDepth, float length,
		int linkWidth, int linkDepth) :
		id(id),
		nodes(nodes),
		vcCount(vcCount),
		bufferDepth(bufferDepth),
		length(length),
		linkWidth(linkWidth),
		linkDepth(linkDepth)
{
}

int Connection::getBufferDepthForNode(Node* n) {
	assert(nodePos.count(n) && "Node not found inside connection! (buffer)");
	return bufferDepth.at(nodePos.at(n));
}

int Connection::getBufferDepthForNodeAndVC(Node* n, int vc) {
    assert(nodePos.count(n) && "Node not found inside connection! (buffer)");
	return buffersDepths.at(nodePos.at(n)).at(vc);
}

int Connection::getVCCountForNode(Node* n) {
	assert(nodePos.count(n) && "Node not found inside connection! (vc)");
	return vcCount.at(nodePos.at(n));
}
