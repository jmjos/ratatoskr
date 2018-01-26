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
#include "LayerTop.h"

LayerTop::LayerTop(sc_module_name nm) {
	dbid = rep.registerElement("LayerTop", 0);

	router.resize(global.nodes.size());
	processingElements.resize(global.getPeCount());
	signalContainer.resize(global.connections.size()*2);
	link.resize(global.connections.size()*2);


	std::vector<sc_clock*> clocks(global.nodeTypes.size());
	//create a clock for every nodetype
	for (NodeType* t : global.nodeTypes) {
		clocks.at(t->id) = new sc_clock(("NodeType" + std::to_string(t->id) + "Clock").c_str(), t->clockSpeed, SC_NS);
	}

	for (Node* n : global.nodes) {
		std::string name = "router_" + std::to_string(n->id);
		RouterVC* r = new RouterVC(name.c_str(), n);
		r->clk(*clocks.at(n->type->id));
		//r->clk_local(clocks.at(n->type->id));

		router.at(n->id) = dynamic_cast<Router*>(r);

	}

	for(unsigned int i=0; i<signalContainer.size(); i++){
		signalContainer.at(i) = new ClassicSignalContainer(("sigCont_"+std::to_string(i)).c_str());
	}

	int i = 0;
	for (Connection* c: global.connections){
		link.at(i) = new Link(("link_"+std::to_string(i)).c_str(),c, i);
		link.at(i+1) = new Link(("link_"+std::to_string(i+1)).c_str(), c, i+1);
		i+=2;
	}

	i = 0;
	for (Connection* c : global.connections) {
		if (c->nodes.size() == 1) { //local
			Node* node = c->nodes.at(0);
			int peId = global.getPeById(node->id);
			ProcessingElementVC3D* processingElement = new ProcessingElementVC3D(("PE_" + std::to_string(peId)).c_str(), node);
			processingElement->clk(*clocks.at(node->type->id));
			processingElement->clk_router(*clocks.at(node->type->id));
			processingElement->classicPortContainer->bind(signalContainer.at(i),signalContainer.at(i+1));
			router.at(node->id)->bind(c, signalContainer.at(i+1), signalContainer.at(i));
			processingElements.at(peId) = (processingElement);

			processingElement->classicPortContainer->portFlowControlOut.write(processingElement->flowControlOut);
			processingElement->classicPortContainer->portTagOut.write(processingElement->tagOut);

			link.at(i)->classicPortContainer->bindOpen(signalContainer.at(i));
			link.at(i+1)->classicPortContainer->bindOpen(signalContainer.at(i+1));
			link.at(i)->clk(*clocks.at(node->type->id));
			link.at(i+1)->clk(*clocks.at(node->type->id));

		} else if (c->nodes.size() == 2) { //might extend to bus architecture
			Node* node1 = c->nodes.at(0);
			Node* node2 = c->nodes.at(1);

			router.at(node1->id)->bind(c, signalContainer.at(i), signalContainer.at(i+1));
			router.at(node2->id)->bind(c, signalContainer.at(i+1), signalContainer.at(i));

			link.at(i)->classicPortContainer->bindOpen(signalContainer.at(i));
			link.at(i+1)->classicPortContainer->bindOpen(signalContainer.at(i+1));
			link.at(i)->clk(*clocks.at(node1->type->id));
			link.at(i+1)->clk(*clocks.at(node2->type->id));
		}
		i+=2;
	}

	for(Router* r: router){
		r->initialize();
	}
}
