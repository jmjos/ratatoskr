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
#include "NetworkInterfaceVC.h"

NetworkInterfaceVC::NetworkInterfaceVC(sc_module_name nm, Node& node)
        :
        NetworkInterface(nm, node),
        lastReceivedCreditID(-1)
{
    try {
        this->id = node.id%(globalResources.nodes.size()/2);
        this->dbid = rep.registerElement("ProcessingElement", this->id);
        this->node = node;
        Node connectedRouter = globalResources.nodes.at(node.connectedNodes.at(0));
        Connection* conn = &globalResources.connections.at(node.getConnWithNode(connectedRouter));
        this->creditCounter = conn->getBufferDepthForNode(connectedRouter.id);
        this->flitPortContainer = new FlitPortContainer(
                ("NI_FLIT_CONTAINER"+std::to_string(this->id)).c_str());
        this->packetPortContainer = new PacketPortContainer(
                ("NI_PACKET_CONTAINER"+std::to_string(this->id)).c_str());
    }
    catch (std::exception& e) {
        std::cout << e.what() << std::endl;
    }

    SC_METHOD(thread);
    sensitive << clk.pos() << clk.neg();

    SC_METHOD(receiveFlitFromRouter);
    sensitive << flitPortContainer->portValidIn.pos();

    SC_METHOD(receivePacketFromPE);
    sensitive << packetPortContainer->portValidIn.pos();

    SC_METHOD(receiveFlowControlCreditFromRouter);
    sensitive << flitPortContainer->portFlowControlValidIn.pos();
}

void NetworkInterfaceVC::initialize()
{
    flitPortContainer->portValidOut.write(false);
    flitPortContainer->portFlowControlValidOut.write(false);
}

void NetworkInterfaceVC::bind(Connection* con, SignalContainer* sigContIn, SignalContainer* sigContOut)
{
    if (con==nullptr)
        packetPortContainer->bind(sigContIn, sigContOut);
    else
        flitPortContainer->bind(sigContIn, sigContOut);
}

void NetworkInterfaceVC::receivePacketFromPE()
{
    if (packetPortContainer->portValidIn.posedge()) {
        LOG(globalReport.verbose_pe_function_calls, "NI" << this->id << "(Node" << node.id << ")\t\t- receive()");
        Packet* p = packetPortContainer->portDataIn.read();
        //generateFlitsForPacket(p);
        packet_send_queue.push(p);
    }
}

void NetworkInterfaceVC::generateFlitsForPacket(Packet* p)
{
    int flitsPerPacket = p->size;
    for (int i = 0; i<flitsPerPacket; ++i) {
        FlitType flitType;
        if (flitsPerPacket==1)
            flitType = SINGLE;
        else if (i%flitsPerPacket==0)
            flitType = HEAD;
        else if (i%flitsPerPacket==flitsPerPacket-1)
            flitType = TAIL;
        else
            flitType = BODY;
        int seqNum = i%flitsPerPacket;
        Flit* current_flit = new Flit(flitType, seqNum, p, p->dataType, sc_time_stamp().to_double());
        p->toTransmit.push_back(current_flit->id);
        p->flits.push_back(current_flit);
    }
}

void NetworkInterfaceVC::thread()
{
    LOG(globalReport.verbose_pe_function_calls, "NI" << this->id << "(Node" << node.id << ")\t\t- send_data_process()");
    if (clk.posedge()) {
        if (!packet_send_queue.empty()) {
            if (creditCounter!=0) {
                Packet* p = packet_send_queue.front();
                if (p->flits.empty())
                    generateFlitsForPacket(p);
                flitID_t f_id = p->toTransmit.front();
                auto iter = std::find_if(p->flits.begin(), p->flits.end(),
                        [&f_id](Flit* f) { return f->id==f_id; });
                Flit* current_flit = *iter;
                current_flit->injectionTime = sc_time_stamp().to_double();
                auto toDelete_pos = std::find(p->toTransmit.begin(), p->toTransmit.end(), current_flit->id);
                p->toTransmit.erase(toDelete_pos);
                p->inTransmit.push_back(current_flit->id);
                //rep.reportEvent(dbid, "pe_send_flit", std::to_string(current_flit->id));
                if (p->toTransmit.empty()) {
                    packet_send_queue.pop();
                }
                flitPortContainer->portValidOut.write(true);
                flitPortContainer->portDataOut.write(current_flit);
                flitPortContainer->portVcOut.write(0);
                creditCounter--;
                LOG((globalReport.verbose_pe_send_head_flit && current_flit->type==HEAD)
                        || globalReport.verbose_pe_send_flit,
                        "NI" << this->id << "(Node" << node.id << ")\t\t- Send Flit " << *current_flit);
            }
            else {
                LOG(globalReport.verbose_pe_throttle,
                        "NI" << this->id << "(Node" << node.id << ")\t\t- Waiting for Router!");
            }
        }
        if (!packet_recv_queue.empty()) {
            if (packetPortContainer->portFlowControlIn.read()) {
                Packet* p = packet_recv_queue.front();
                packet_recv_queue.pop();
                packetPortContainer->portValidOut.write(true);
                packetPortContainer->portDataOut.write(p);
            }
        }
    }
    else if (clk.negedge()) {
        //The negative edge of the clock is used to model the asynchronous communication. Otherwise, the clock of the
        // would need to be fed to the downstream router, which is rather complicated. Therefore, we chose this approach
        // although it is not physically accurate.
        flitPortContainer->portValidOut.write(false);
        packetPortContainer->portValidOut.write(false);
    }
}

void NetworkInterfaceVC::receiveFlitFromRouter()
{
    LOG(globalReport.verbose_pe_function_calls,
            "NI" << this->id << "(Node" << node.id << ")\t- receive_data_process()");
    if (flitPortContainer->portValidIn.posedge()) {
        Flit* received_flit = flitPortContainer->portDataIn.read();
        Packet* p = received_flit->packet;
        double time = sc_time_stamp().to_double();
        // generate packet statistics. in case of synthetic traffic only for run phase
#ifndef ENABLE_NETRACE
        if ((float) globalResources.synthetic_start_measurement_time<=(time/1000)) {
            globalReport.latencyFlit.sample((float)(time-received_flit->injectionTime)); // evil line of code
            if (received_flit->type==TAIL || received_flit->type==SINGLE) {
                globalReport.latencyPacket.sample((float) (time-received_flit->headFlit->generationTime));
                globalReport.latencyNetwork.sample((float) (time-received_flit->headFlit->injectionTime));
                globalReport.undeliveredPackages--;
            }
        }
#endif
#ifdef ENABLE_NETRACE
        globalReport.latencyFlit.sample((float)(time-received_flit->injectionTime)); // evil line of code
        if (received_flit->type==TAIL || received_flit->type==SINGLE) {
            globalReport.latencyPacket.sample((float) (time-received_flit->headFlit->generationTime));
            globalReport.latencyNetwork.sample((float) (time-received_flit->headFlit->injectionTime));
        }
#endif



        auto position = std::find(p->inTransmit.begin(), p->inTransmit.end(), received_flit->id);
        if (position!=p->inTransmit.end())
            p->inTransmit.erase(position);
        p->transmitted.push_back(received_flit->id);

        //rep.reportEvent(dbid, "pe_receive_flit", std::to_string(received_flit->id));
        LOG((globalReport.verbose_pe_receive_tail_flit && (received_flit->type==TAIL || received_flit->type==SINGLE))
                || globalReport.verbose_pe_receive_flit,
                "NI" << this->id << "(Node" << node.id << ")\t\t- Receive Flit " << *received_flit);
        LOG((received_flit->type==TAIL || received_flit->type==SINGLE) && (!p->toTransmit.empty() || !p->inTransmit.empty()),
                "NI" << this->id << "(Node" << node.id << ")\t\t- Received Tail Flit, but still missing flits! "
                     << *received_flit);
        if (p->toTransmit.empty() && p->inTransmit.empty())
            packet_recv_queue.push(p);
    }
}

NetworkInterfaceVC::~NetworkInterfaceVC()
{
    delete flitPortContainer;
    delete packetPortContainer;
}

void NetworkInterfaceVC::receiveFlowControlCreditFromRouter()
{
    if (flitPortContainer->portFlowControlValidIn.posedge()) {
        auto credit = flitPortContainer->portFlowControlIn.read();
        assert(credit.vc==0);
        if (lastReceivedCreditID!=credit.id) {
            ++creditCounter;
        }
    }
}
