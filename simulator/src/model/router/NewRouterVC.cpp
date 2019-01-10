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

#include "NewRouterVC.h"

NewRouterVC::NewRouterVC(sc_module_name nm, Node& node)
        :
        Router(nm, node)
{
    int conCount = node.connections.size();
    classicPortContainer.init(conCount);
    buffers.resize(conCount);
    flowControlOut.resize(conCount);
    for (int conPos=0; conPos < node.connections.size(); conPos++) {
        connID_t con_id = node.connections.at(conPos);
        Connection con = globalResources.connections.at(con_id);
        int vcCount = con.getVCCountForNode(node.id);
        flowControlOut.at(conPos) = new std::vector<bool>(vcCount, false);
        buffers.at(conPos) = new std::vector<BufferFIFO<Flit *> *>(vcCount);
        for (int vc = 0; vc<vcCount; vc++) {

            int vc_buf_size = 0;
            if (globalResources.bufferDepthType=="single")
                vc_buf_size = con.getBufferDepthForNode(node.id);
            else if (globalResources.bufferDepthType=="perVC")
                vc_buf_size = con.getBufferDepthForNodeAndVC(node.id, vc);
            else FATAL("The type of buffer depth is not defined!");
            auto*buf = new BufferFIFO<Flit *>(vc_buf_size);
            buffers.at(conPos)->insert(buffers.at(conPos)->begin() + vc, buf);
            rep.reportAttribute(buf->dbid, "node", std::to_string(node.id));
            rep.reportAttribute(buf->dbid, "dir", std::to_string(conPos));
            rep.reportAttribute(buf->dbid, "vc", std::to_string(vc));
        }
        classicPortContainer.at(conPos).portValidOut.write(false);
        classicPortContainer.at(conPos).portFlowControlOut.write(flowControlOut.at(conPos));
        sensitive << classicPortContainer.at(conPos).portValidIn.pos();
    }
    lastReceivedFlits.resize(conCount);


    SC_METHOD(thread);
    sensitive << clk.pos() << clk.neg();

    SC_METHOD(updateUsageStats);
    sensitive << clk.pos();

    SC_METHOD(receive);
}

NewRouterVC::~NewRouterVC()
{
    for(auto& dir_buf:buffers) {
        for (auto& vc_buf:*dir_buf)
            vc_buf->flush();
        dir_buf->clear();
    }
    buffers.clear();
}

void NewRouterVC::bind(Connection* con, SignalContainer* sigContIn, SignalContainer* sigContOut)
{
    classicPortContainer.at(node.getConnPosition(con->id)).bind(sigContIn, sigContOut);
}

void NewRouterVC::thread()
{
    if (clk.posedge()) {
        route();
    }
}

// VC and buffer usage statistics
void NewRouterVC::updateUsageStats()
{
    for (unsigned int dirIn = 0; dirIn<node.connections.size(); dirIn++) {
        int numVCs = buffers.at(dirIn)->size();
        int numberActiveVCs = 0;

        for (unsigned int vcIn = 0; vcIn<numVCs; vcIn++) {
            BufferFIFO<Flit*>* buf = buffers.at(dirIn)->at(vcIn);
            if (!buf->empty()) {
                numberActiveVCs++;
                globalReport.updateBuffUsagePerVCHist(globalReport.bufferUsagePerVCHist, this->id, dirIn,
                        vcIn, static_cast<int>(buf->occupied()), numVCs);
            }
        }

        /* this 1 is added to create a column for numberOfActiveVCs=0.
           yes it's an extra column but it allow us to use the same function to update both buffer stats and VC stats.
        */
        globalReport.updateUsageHist(globalReport.VCsUsageHist, this->id, node.getDirOfConPos(dirIn),
                numberActiveVCs, numVCs+1);
    }
}

void NewRouterVC::route()
{
   /* for (unsigned int dirIn = 0; dirIn<node->connections.size(); dirIn++) {
        for (unsigned int vcIn = 0; vcIn<buffers.at(dirIn)->size(); vcIn++) {
            BufferFIFO<Flit*>* buf = buffers.at(dirIn)->at(vcIn);
            Flit* flit = buf->front();
            Channel in = Channel(dirIn, vcIn);
            //check, if flit is in front element of buffer and no route has been calculated
            if (flit && !rInfo->routingStateTable.count(in)) {
                if (flit->type==HEAD) { //check if head and calculate route
                    RoutingPacketInformation* rpi = rpInfo.at({flit->packet, in});
                    routing->route(rInfo, rpi);
                    if (!rpi->routedChannel.empty()) { //a route has been calculated
                        //insert route into state field
                        Channel out = Channel(rpi->routedChannel.begin()->dir, -1);
                        rInfo->routingStateTable.insert({in, out}); //only works for deterministic with a single path

                        routedPackets.insert({flit->packet, in});
                        flit->packet->numhops++;
                        flit->packet->traversedRouters.push_back(node.id);
                    }
                    else {
                        rpi->dropFlag = true;
                        rep.reportEvent(dbid, "router_routefail", std::to_string(flit->id));
                    }
                }
            }
        }
    }*/
}

void NewRouterVC::receive()
{

}

void NewRouterVC::initialize()
{
}
