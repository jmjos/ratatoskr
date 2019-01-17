/*******************************************************************************
 * Copyright (C) 2018 joseph
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
#include <vector>
#include "NoC.h"

NoC::NoC(sc_module_name nm)
{
    dbid = rep.registerElement("NoC", 0);
    networkParticipants.resize(globalResources.nodes.size());
    //packetSignalContainers.resize(globalResources.nodes.size());
    flitSignalContainers.resize(globalResources.connections.size() * 2);
    links.resize(globalResources.connections.size()*2);

    createTrafficPool();

    //create a clock for every node type
    clocks.resize(globalResources.nodeTypes.size());
    for (const auto& nodeType : globalResources.nodeTypes) {
        clocks.at(nodeType->id) = std::make_unique<sc_clock>(("NodeType"+std::to_string(nodeType->id)+"Clock").c_str(),
                nodeType->clockDelay, SC_NS);
    }

    createNetworkParticipants();
    createSigContainers();
    createLinks(clocks);
    runNoC();

}

void NoC::createTrafficPool()
{
    unsigned long numOfPEs = globalResources.nodes.size()/2;
    if (globalResources.benchmark=="task") {
        tp = std::make_unique<TaskPool>();
    }
    else if (globalResources.benchmark=="synthetic") {
        tp = std::make_unique<SyntheticPool>();
    }
    else {
        FATAL("Please specify correct benchmark type");
    }
    tp->processingElements.resize(numOfPEs);
}

void NoC::createNetworkParticipants()
{

    for (Node& n : globalResources.nodes) {
        if (n.type->model=="RouterVC") {
           std::string name = "router_"+std::to_string(n.id);
           RouterVC* r = new RouterVC(name.c_str(), n);
           r->clk(*clocks.at(n.type->id));
           networkParticipants.at(n.id) = dynamic_cast<NetworkParticipant*>(r);
       }
       else if (n.type->model=="ProcessingElement") {
           // Creating an network interface.
           std::string ni_name = "ni_"+std::to_string(n.id%tp->processingElements.size());
           NetworkInterfaceVC* ni = new NetworkInterfaceVC(ni_name.c_str(), n);
           ni->clk(*clocks.at(n.type->id));
           networkParticipants.at(n.id) = dynamic_cast<NetworkParticipant*>(ni);

           // Creating a processing element.
           std::string pe_name = "pe_"+std::to_string(n.id%tp->processingElements.size());
           ProcessingElementVC* pe = new ProcessingElementVC(pe_name.c_str(), n, tp.get());
           std::unique_ptr<PacketSignalContainer> sig1 =  std::make_unique<PacketSignalContainer>(
                  ("packetSigCon1_"+std::__cxx11::to_string(n.id)).c_str());
            std::unique_ptr<PacketSignalContainer> sig2 = std::make_unique<PacketSignalContainer>(
                   ("packetSigCon2_"+std::__cxx11::to_string(n.id)).c_str());
           ni->bind(nullptr, sig1.get(), sig2.get());
           pe->bind(nullptr, sig2.get(), sig1.get());
           networkParticipants.push_back(dynamic_cast<NetworkParticipant*>(pe));
           packetSignalContainers.push_back(move(sig1));
           packetSignalContainers.push_back(move(sig2));
           tp->processingElements.at(n.id%tp->processingElements.size()) =  pe;
       }
   }

}

void NoC::createSigContainers()
{

    for (int i = 0; i<flitSignalContainers.size(); i++) {
        flitSignalContainers.at(i) = std::make_unique<FlitSignalContainer>(
                ("flitSigCont_"+std::__cxx11::to_string(i)).c_str());
    }

}

void NoC::createLinks(const std::vector<std::unique_ptr<sc_clock>>& clocks)
{
    int link_id = 0;
    for (auto& c : globalResources.connections) {
        if (c.nodes.size()==2) { //might extend to bus architecture
            links.at(link_id) = std::make_unique<Link>(
                    ("link_"+std::to_string(link_id)+"_Conn_"+std::to_string(c.id)).c_str(), c, link_id);
            links.at(link_id+1) = std::make_unique<Link>(
                    ("link_"+std::to_string(link_id+1)+"_Conn_"+std::to_string(c.id)).c_str(), c,
                    link_id+1);

            Node& node1 = globalResources.nodes.at(c.nodes.at(0));
            Node& node2 = globalResources.nodes.at(c.nodes.at(1));

            std::unique_ptr<Connection> c_ptr = std::make_unique<Connection>(c);
            networkParticipants.at(node1.id)->bind(c_ptr.get(), flitSignalContainers.at(link_id).get(),
                    flitSignalContainers.at(link_id+1).get());
            networkParticipants.at(node2.id)->bind(c_ptr.get(), flitSignalContainers.at(link_id+1).get(),
                    flitSignalContainers.at(link_id).get());

            links.at(link_id)->classicPortContainer->bindOpen(flitSignalContainers.at(link_id).get());
            links.at(link_id+1)->classicPortContainer->bindOpen(flitSignalContainers.at(link_id+1).get());
            links.at(link_id)->clk(*clocks.at(node1.type->id));
            links.at(link_id+1)->clk(*clocks.at(node2.type->id));
            link_id +=2;
        }
        else {
            FATAL("Unsupported number of endpoints in connection " << c.id);
        }
    }
}

void NoC::runNoC()
{
    for (auto& r : networkParticipants) {
        r->initialize();
    }
    tp->start();

 }

NoC::~NoC()
{
    for (auto& r : networkParticipants)
        delete r;
    networkParticipants.clear();
}
