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

#include "RouterVC.h"

RouterVC::RouterVC(sc_module_name nm, Node& node)
        :
        Router(nm, node),
        rrDirOff(0),
        rrSwitch(0)
{
    int conCount = node.connections.size();
    classicPortContainer.init(conCount);
    buffers.resize(conCount);
    flowControlOut.resize(conCount);
    for (int conPos = 0; conPos<node.connections.size(); conPos++) {
        connID_t con_id = node.connections.at(conPos);
        Connection con = globalResources.connections.at(con_id);
        int vcCount = con.getVCCountForNode(node.id);
        flowControlOut.at(conPos) = new std::vector<bool>(vcCount, false);
        buffers.at(conPos) = new std::vector<BufferFIFO<Flit*>*>(vcCount);
        for (int vc = 0; vc<vcCount; vc++) {
            int vc_buf_size = 0;
            if (globalResources.bufferDepthType=="single")
                vc_buf_size = con.getBufferDepthForNode(node.id);
            else if (globalResources.bufferDepthType=="perVC")
                vc_buf_size = con.getBufferDepthForNodeAndVC(node.id, vc);
            else FATAL("The type of buffer depth is not defined!");
            auto* buf = new BufferFIFO<Flit*>(vc_buf_size);
            buffers.at(conPos)->insert(buffers.at(conPos)->begin()+vc, buf);
            rep.reportAttribute(buf->dbid, "node", std::to_string(node.id));
            rep.reportAttribute(buf->dbid, "dir", std::to_string(conPos));
            rep.reportAttribute(buf->dbid, "vc", std::to_string(vc));
        }

    }

    lastReceivedFlits.resize(conCount);

    SC_METHOD(thread);
    sensitive << clk.pos() << clk.neg();
/*
    SC_METHOD(updateUsageStats);
    sensitive << clk.pos();
*/
    SC_METHOD(receive);
    for (int conPos = 0; conPos<node.connections.size(); conPos++) {
        sensitive << classicPortContainer.at(conPos).portValidIn.pos();
    }
}

void RouterVC::bind(Connection* con, SignalContainer* sigContIn, SignalContainer* sigContOut)
{
    classicPortContainer.at(node.getConnPosition(con->id)).bind(sigContIn, sigContOut);
}

void RouterVC::thread()
{
    if (clk.posedge()) {
        //route();
        // key: connection position of output, value: the requesting inputs.
        std::map<int, std::vector<Channel>> requests = generateRequests();
        generateAck(requests);
        switchAllocation();

    }
}

// VC and buffer usage statistics
void RouterVC::updateUsageStats()
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

void RouterVC::route()
{
    /*for (unsigned int conPos = 0; conPos<node.connections.size(); conPos++) {
        for (unsigned int vcIn = 0; vcIn<buffers.at(conPos)->size(); vcIn++) {
            BufferFIFO<Flit*>* buf = buffers.at(conPos)->at(vcIn);
            Flit* flit = buf->front();
            Channel cin = Channel(conPos, vcIn);
            //check, if flit is cin front element of buffer and no route has been calculated
            if (flit && !routingTable.count(cin)) {
                if (flit->type==HEAD) { //check if head and calculate route
                    int src_node_id = flit->packet->src.id;
                    int dst_node_id = flit->packet->dst.id;
                    int chosen_conPos = routingAlg->route(src_node_id, dst_node_id);
                    if (chosen_conPos==-1)
                        std::cerr << "Bad routing" << std::endl;
                    flit->packet->numhops++;
                    flit->packet->traversedRouters.push_back(node.id);
                }
            }
        }
    }*/
}

void RouterVC::receive()
{
    for (unsigned int conPos = 0; conPos<node.connections.size(); conPos++) {
        if (classicPortContainer.at(conPos).portValidIn.read()) {
            Flit* flit = classicPortContainer.at(conPos).portDataIn.read();
            int vc = classicPortContainer.at(conPos).portVcIn.read();
            BufferFIFO<Flit*>* buf = buffers.at(conPos)->at(vc);
            Channel c = Channel(conPos, vc);

            // prevents double writing of data, which occurs for asynchronous buffers.
            // problem: this function is executed for all buffers if new data arrive. If some directions are clocked at different data rate, the slower data rates are duplicated.

            Flit* lrFlit = lastReceivedFlits.at(conPos);
            if (!lrFlit || lrFlit->id!=flit->id) {
                if (buf->enqueue(flit)) {
                    rep.reportEvent(buf->dbid, "buffer_enqueue_flit", std::to_string(flit->id));
                    lastReceivedFlits[conPos] = flit;
                }
                else {
                    LOG(globalReport.verbose_router_buffer_overflow,
                            "Router" << this->id << "[" << DIR::toString(node.getDirOfConPos(conPos)) << vc
                                     << "]\t- Buffer Overflow " << *flit);
                }
                if (flit->type==HEAD) {
                    LOG(globalReport.verbose_router_receive_head_flit,
                            "Router" << this->id << "[" << DIR::toString(node.getDirOfConPos(conPos)) << vc
                                     << "]\t- Receive Flit " << *flit);
                }
                else {
                    LOG(globalReport.verbose_router_receive_flit,
                            "Router" << this->id << "[" << DIR::toString(node.getDirOfConPos(conPos)) << vc
                                     << "]\t- Receive Flit " << *flit);
                }
            }
        }
    }
}

void RouterVC::initialize()
{
    for (int conPos = 0; conPos<node.connections.size(); conPos++) {
        classicPortContainer.at(conPos).portValidOut.write(false);
        classicPortContainer.at(conPos).portFlowControlOut.write(flowControlOut.at(conPos));
    }

    // Setting credits according to the buffer depth of the downstream routers.
    for (auto& n_id:node.connectedNodes) {
        Node connectedRouter = globalResources.nodes.at(n_id);
        Connection conn = globalResources.connections.at(node.getConnWithNode(connectedRouter));
        for (int vc = 0; vc<conn.getVCCountForNode(connectedRouter.id); vc++) {
            int conPos = node.getConnPosition(conn.id);
            Channel ch{conPos, vc};
            int buf_size = 0;
            if (globalResources.bufferDepthType=="single")
                buf_size = conn.getBufferDepthForNode(connectedRouter.id);
            else if (globalResources.bufferDepthType=="perVC")
                buf_size = conn.getBufferDepthForNodeAndVC(connectedRouter.id, vc);
            creditCounter.insert({ch, buf_size});
        }
    }
}

RouterVC::~RouterVC()
{
    for (auto& fc:flowControlOut)
        delete fc;
    flowControlOut.clear();
}

std::map<int, std::vector<Channel>> RouterVC::generateRequests()
{
    std::map<int, std::vector<Channel>> requests{};
    auto insert_request = [&requests](int out, Channel in) {
        if (!requests.count(out)) {
            auto entry = std::make_pair(out, std::vector<Channel>{in});
            requests.insert(entry);
        }
        else
            requests.at(out).push_back(in);
    };
    for (int in_conPos = 0; in_conPos<node.connections.size(); in_conPos++) {
        int in_vc = 0; //TODO round_robin
        BufferFIFO<Flit*>* buf = buffers.at(in_conPos)->at(in_vc);
        Flit* flit = buf->front();
        if (flit && flit->type==HEAD) {
            int src_node_id = this->id;
            int dst_node_id = flit->packet->dst.id;
            int chosen_conPos = routingAlg->route(src_node_id, dst_node_id);
            if (chosen_conPos==-1)
                std::cerr << "Bad routing" << std::endl;
            insert_request(chosen_conPos, {in_conPos, in_vc});
        }
    }
    return requests;
}

void RouterVC::generateAck(const std::map<int, std::vector<Channel>>& requests)
{
    for (auto& request:requests) {
        int requested_out = request.first;
        std::vector<Channel> requesting_ins = request.second;
        for (auto& requesting_in:requesting_ins) {
            int out_vc = getNextFreeVC(requested_out);
            routingTable.insert(requesting_in, {requested_out, out_vc});
            BufferFIFO<Flit*>* buf = buffers.at(requesting_in.conPos)->at(requesting_in.vc);
            Flit* flit = buf->front();
            assert(flit);
            flit->packet->numhops++;
            flit->packet->traversedRouters.push_back(this->id);
        }
    }
}

int RouterVC::getNextFreeVC(int out)
{
    std::vector<int> used_vcs{};
    for (auto& entry:routingTable) {
        if (out==entry.second.conPos)
            used_vcs.push_back(entry.second.vc);
    }

    std::sort(used_vcs.begin(), used_vcs.end());

    if (used_vcs.empty())   // if there are no used vcs.
        return 0;
    else {
        connID_t con_id = node.connections.at(out);
        Connection con = globalResources.connections.at(con_id);
        int vcCount = con.getVCCountForNode(node.id);
        if (used_vcs.size()==vcCount)   // if all vcs are used.
            return -1;
        else {
            std::vector<int> search_values(vcCount);
            std::iota(search_values.begin(), search_values.end(), 0);
            auto find_if_not_used = [&used_vcs](const int& search_value) {
                auto result = std::find(used_vcs.begin(), used_vcs.end(), search_value);
                if (result==used_vcs.end())
                    return search_value;
            };
            std::for_each(search_values.begin(), search_values.end(), find_if_not_used);
        }
    }
}

void RouterVC::switchAllocation()
{
    std::map<Channel, Channel> routingTableCopy = this->routingTable;
    int num_of_matches{0};
    for (auto& entry:this->routingTable) {
        Channel in = entry.first;
        Channel out = entry.second;
        for(auto& entry_copy:routingTableCopy){
            Channel in_copy = entry_copy.first;
            Channel out_copy = entry_copy.second;
            if(out == out_copy) {
                num_of_matches++;
            }
        }
    }
}

void RouterVC::round_robin(int pos)
{

}
