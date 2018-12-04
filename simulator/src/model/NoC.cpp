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
#include "NoC.h"

NoC::NoC(sc_module_name nm) {
    dbid = rep.registerElement("NoC", 0);
    networkParticipant.resize(globalResources.nodes.size());
    signalContainer.resize(globalResources.connections.size() * 2);
    link.resize(globalResources.connections.size() * 2);

    if (strcmp(globalResources.benchmark.c_str(), "task") == 0) {
        tp = std::make_unique<TaskPool>();
    }
//		else if (strcmp(globalResources.benchmark.c_str(), "netrace") == 0) {
//		tp = new NetracePool("netracePool", &layer->nis);
//	} else
    else if (strcmp(globalResources.benchmark.c_str(), "synthetic") == 0) {
        tp = new SyntheticPool();
    } else {
        FATAL("Please specify correct benchmark type");
    }

    int size = 0;

    for (auto it:globalResources.nodes){
    }
    std::map<int, NodeType *>::iterator it;
    for (it = globalResources.typeById.begin(); it != globalResources.typeById.end(); ++it) {
        if (it->second->model == "ProcessingElement")
            size += it->second->nodes.size();
    }
    tp->processingElements.resize(size);

    //tp->processingElements.resize(
    //		globalResources.typeByName.at("ProcessingElement")->nodes.size());

    std::vector<sc_clock *> clocks(globalResources.nodeTypes.size());
    //create a clock for every nodetype
    for (NodeType *t : globalResources.nodeTypes) {
        clocks.at(t->id) = new sc_clock(("NodeType" + std::to_string(t->id) + "Clock").c_str(),
                                        t->clockDelay, SC_NS);
    }

    for (Node *n : globalResources.nodes) {
        if (n->type->routerModel == "RouterVC") {
            std::string name = "router_" + std::to_string(n->id);
            RouterVC *r = new RouterVC(name.c_str(), n);
            r->clk(*clocks.at(n->type->id));
            networkParticipant.at(n->id) = dynamic_cast<NetworkParticipant *>(r);
        } else //if (n->type->model == "ProcessingElement") {
        {
            std::string name = "ni_" + std::to_string(n->id);
            NetworkInterfaceVC *ni = new NetworkInterfaceVC(name.c_str(), n);
            ni->clk(*clocks.at(n->type->id));
            networkParticipant.at(n->id) = dynamic_cast<NetworkParticipant *>(ni);
        }
    }

    for (int i = 0; i < signalContainer.size(); i++) {
        signalContainer.at(i) = new FlitSignalContainer(
                ("flitSigCont_" + std::to_string(i)).c_str());
    }

    for (it = globalResources.typeById.begin(); it != globalResources.typeById.end(); it++) {
        if (it->second->model == "ProcessingElement") {
            for (Node *n : it->second->nodes) {
//	for (Node* n : globalResources.typeByName.at("ProcessingElement")->nodes) {
                std::string name = "pe_" + std::to_string(n->id);
                ProcessingElementVC *pe = new ProcessingElementVC(name.c_str(), n, tp);
                PacketSignalContainer *sig1 = new PacketSignalContainer(
                        ("packetSigCon1_" + std::to_string(n->id)).c_str());
                PacketSignalContainer *sig2 = new PacketSignalContainer(
                        ("packetSigCon2_" + std::to_string(n->id)).c_str());

                networkParticipant.at(n->id)->bind(0, sig1, sig2);
                pe->bind(0, sig2, sig1);

                networkParticipant.push_back(dynamic_cast<NetworkParticipant *>(pe));
                signalContainer.push_back(sig1);
                signalContainer.push_back(sig2);

                //tp->processingElements.at(pe->node->idType) = pe;
                tp->processingElements.at(n->id % tp->processingElements.size()) = pe;
            }
        }
    }

    int i = 0;
    for (Connection *c : globalResources.connections) {
        link.at(i) = new Link(
                ("link_" + std::to_string(i) + "_Conn_" + std::to_string(c->id)).c_str(), c, i);
        link.at(i + 1) = new Link(
                ("link_" + std::to_string(i + 1) + "_Conn_" + std::to_string(c->id)).c_str(), c,
                i + 1);
        i += 2;
    }

    i = 0;
    for (Connection *c : globalResources.connections) {
        if (c->nodes.size() == 2) { //might extend to bus architecture
            Node *node1 = c->nodes.at(0);
            Node *node2 = c->nodes.at(1);

            networkParticipant.at(node1->id)->bind(c, signalContainer.at(i),
                                                   signalContainer.at(i + 1));
            networkParticipant.at(node2->id)->bind(c, signalContainer.at(i + 1),
                                                   signalContainer.at(i));

            link.at(i)->classicPortContainer->bindOpen(signalContainer.at(i));
            link.at(i + 1)->classicPortContainer->bindOpen(signalContainer.at(i + 1));
            link.at(i)->clk(*clocks.at(node1->type->id));
            link.at(i + 1)->clk(*clocks.at(node2->type->id));
        } else {
            FATAL("Unsupported number of endpoints in connection " << c->id);
        }
        i += 2;
    }

    for (NetworkParticipant *r : networkParticipant) {
        r->initialize();
    }

    tp->start();
}
