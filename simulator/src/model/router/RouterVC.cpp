/*******************************************************************************
 * Copyright (C) 2019 jmjos
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
        Router(nm, node)
{
    int conCount = node.connections.size();
    classicPortContainer.init(conCount);
    buffers.resize(conCount);
    int nodeConnsSize = node.connections.size();
    for (int conPos = 0; conPos<nodeConnsSize; conPos++) {
        switchAllocation_outputVCPtr.insert({conPos, 0});
        connID_t con_id = node.connections.at(conPos);
        Connection* con = &globalResources.connections.at(con_id);
        int vcCount = con->getVCCountForNode(node.id);
        std::vector<int> vcs2(vcCount); //TODO make this nice please
        std::iota(vcs2.begin(), vcs2.end(), 0);
        VCAllocation_inputVC_rrList.insert({conPos, vcs2});
        std::vector<int> vcs(vcCount);
        std::iota(vcs.begin(), vcs.end(), 0);
        switchAllocation_inputVC_rrList.insert({conPos, vcs});
        buffers.at(conPos) = new std::vector<BufferFIFO<Flit*>*>(vcCount);
        for (int vc = 0; vc<vcCount; vc++) {
            int vc_buf_size = 0;
            if (globalResources.bufferDepthType=="single")
                vc_buf_size = con->getBufferDepthForNode(node.id);
            else if (globalResources.bufferDepthType=="perVC")
                vc_buf_size = con->getBufferDepthForNodeAndVC(node.id, vc);
            else FATAL("The type of buffer depth is not defined!");
            auto* buf = new BufferFIFO<Flit*>(vc_buf_size);
            buffers.at(conPos)->insert(buffers.at(conPos)->begin()+vc, buf);
            rep.reportAttribute(buf->dbid, "node", std::to_string(node.id));
            rep.reportAttribute(buf->dbid, "dir", std::to_string(conPos));
            rep.reportAttribute(buf->dbid, "vc", std::to_string(vc));
        }
    }

    SC_METHOD(thread);
    sensitive << clk.pos() << clk.neg();

    SC_METHOD(updateUsageStats);
    sensitive << clk.pos();

    SC_METHOD(receive);
    for (int conPos = 0; conPos<nodeConnsSize; conPos++) {
        sensitive << classicPortContainer.at(conPos).portValidIn.pos();
    }

    SC_METHOD(receiveFlowControlCredit);
    for (int conPos = 0; conPos<nodeConnsSize; conPos++) {
        sensitive << classicPortContainer.at(conPos).portFlowControlValidIn.pos();
    }
}

void RouterVC::bind(Connection* con, SignalContainer* sigContIn, SignalContainer* sigContOut)
{
    classicPortContainer.at(node.getConnPosition(con->id)).bind(sigContIn, sigContOut);
}

void RouterVC::initialize()
{
    LOG(globalReport.verbose_router_function_calls,
            "Router" << this->id << "in initialize()");
    int nodeConnsSize = node.connections.size();
    for (int conPos = 0; conPos<nodeConnsSize; conPos++) {
        classicPortContainer.at(conPos).portValidOut.write(false);
        classicPortContainer.at(conPos).portFlowControlValidOut.write(false);
    }

    // Setting credits according to the buffer depth of the downstream routers.
    for (auto& n_id:node.connectedNodes) {
        Node connectedRouter = globalResources.nodes.at(n_id);
        Connection* conn = &globalResources.connections.at(node.getConnWithNode(connectedRouter));
        for (int vc = 0; vc<conn->getVCCountForNode(connectedRouter.id); vc++) {
            int conPos = node.getConnPosition(conn->id);
            Channel ch{conPos, vc};
            int buf_size = 0;
            if (globalResources.bufferDepthType=="single")
                buf_size = conn->getBufferDepthForNode(connectedRouter.id);
            else if (globalResources.bufferDepthType=="perVC")
                buf_size = conn->getBufferDepthForNodeAndVC(connectedRouter.id, vc);
            creditCounter.insert({ch, buf_size});
            lastReceivedCreditID.insert({ch, -1});
            lastReceivedFlitsID.insert({ch, -1});
        }
    }
}

void RouterVC::thread()
{
    LOG(globalReport.verbose_router_function_calls,
            "Router" << this->id << "in tread() @ " << sc_time_stamp());
    if (clk.posedge()) {

        globalReport.increaseClockCount(node.layer);

        send();

        std::map<int, std::vector<Channel>> switch_requests = switchAllocation_generateRequests();
        switchAllocation_generateAck(switch_requests);

        std::map<int, std::vector<Channel>> vc_requests = VCAllocation_generateRequests();
        VCAllocation_generateAck(vc_requests);
    }
    else if (clk.negedge()) {
        //The negative edge of the clock is used to model the asynchronous communication. Otherwise, the clock of the
        // would need to be fed to the downstream router, which is rather complicated. Therefore, we chose this approach
        // although it is not physically accurate.
        int nodeConnsSize = node.connections.size();
        for (unsigned int con = 0; con<nodeConnsSize; con++) {
            classicPortContainer.at(con).portValidOut.write(false);
            classicPortContainer.at(con).portFlowControlValidOut.write(false);
        }
    }
}

void RouterVC::receive()
{
    LOG(globalReport.verbose_router_function_calls,
            "Router" << this->id << "in receive() @ " << sc_time_stamp());
    int nodeConnsSize = node.connections.size();
    for (unsigned int conPos = 0; conPos<nodeConnsSize; conPos++) {
        if (classicPortContainer.at(conPos).portValidIn.read()) {
            Flit* flit = classicPortContainer.at(conPos).portDataIn.read();
            int vc = classicPortContainer.at(conPos).portVcIn.read();
            BufferFIFO<Flit*>* buf = buffers.at(conPos)->at(vc);
            Channel in = Channel(conPos, vc);

            /* Prevents double writing of data, which occurs for asynchronous buffers.
             * Problem: This function is executed for all buffers if new data arrive. If some directions are clocked
             * at different data rate, the slower data rates are duplicated.
             */
            if (lastReceivedFlitsID.at(in)!=flit->id) {
                if (buf->enqueue(flit)) {
                    //rep.reportEvent(buf->dbid, "buffer_enqueue_flit", std::to_string(flit->id));
                    globalReport.increaseBufferPush(this->id);
                    lastReceivedFlitsID.at(in) = flit->id;
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
                else {
                    LOG(globalReport.verbose_router_buffer_overflow,
                            "Router" << this->id << "[" << DIR::toString(node.getDirOfConPos(conPos)) << vc
                                     << "]\t- Buffer Overflow " << *flit);
                }
            }
        }
    }
}

// VC and buffer usage statistics
void RouterVC::updateUsageStats()
{
    LOG(globalReport.verbose_router_function_calls,
            "Router" << this->id << "in updateUsageStats() @ " << sc_time_stamp());

    auto findInListit = std::find(globalReport.bufferReportRouters.begin(), globalReport.bufferReportRouters.end(),
            this->id);

    if (findInListit!=globalReport.bufferReportRouters.end()) {
        int nodeConnsSize = node.connections.size();
        for (int conPos = 0; conPos<nodeConnsSize; conPos++) {
            Connection* con = &globalResources.connections.at(node.connections.at(conPos));
            int vcCount = con->getVCCountForNode(node.id);
            int numberActiveVCs = 0;

            for (int vc = 0; vc<vcCount; vc++) {
                BufferFIFO<Flit*>* buf = buffers.at(conPos)->at(vc);
                if (!buf->empty()) {
                    numberActiveVCs++;
                    globalReport.updateBuffUsagePerVCHist(this->id, node.getDirOfConPos(conPos), vc,
                            static_cast<int>(buf->occupied()));
                }
            }
            globalReport.updateVCUsageHist(this->id, node.getDirOfConPos(conPos), numberActiveVCs);
        }
    }
}

std::map<int, std::vector<Channel>> RouterVC::VCAllocation_generateRequests()
{
    LOG(globalReport.verbose_router_function_calls,
            "Router" << this->id << "in VCAllocation_generateRequests() @ " << sc_time_stamp());
    std::map<int, std::vector<Channel>> requests{};
    int nodeConnsSize = node.connections.size();
    for (int in_conPos = 0; in_conPos<nodeConnsSize; in_conPos++) {
        int in_vc = VCAllocation_getNextVCToBeAllocated(in_conPos);
        if (in_vc!=-1) {
            BufferFIFO<Flit*>* buf = buffers.at(in_conPos)->at(in_vc);
            Flit* flit = buf->front();
            globalReport.increaseBufferFront(id);

            if (flit && flit->type==HEAD && !routingTable.count({in_conPos, in_vc})) {
                int src_node_id = this->id;
                int dst_node_id = flit->packet->dst.id;
                int chosen_conPos = routingAlg->route(src_node_id, dst_node_id);
                globalReport.increaseRouting(this->id);
                if (chosen_conPos==-1)
                    std::cerr << "Bad routing" << std::endl;
                Channel in{in_conPos, in_vc};
                insert_request(chosen_conPos, in, requests);
            }
        }
    }
    return requests;
}

void RouterVC::VCAllocation_generateAck(const std::map<int, std::vector<Channel>>& requests)
{
    LOG(globalReport.verbose_router_function_calls,
            "Router" << this->id << "in VCAllocation_generateAck() @ " << sc_time_stamp());
    for (auto& request:requests) {
        int requested_out = request.first;
        std::vector<Channel> requesting_ins = request.second;
        for (auto& requesting_in:requesting_ins) {
            int out_vc = VCAllocation_getNextFreeVC(requested_out);
            if (out_vc!=-1) {// no VC is available at the moment
                routingTable[requesting_in] = {requested_out, out_vc};
                //VCAllocation_inputVCPtr.at(requesting_in.conPos)++;
                auto vcList = &VCAllocation_inputVC_rrList.at(requesting_in.conPos);
                auto vcCount = vcList->size();
                std::for_each(vcList->begin(), vcList->end(), [&vcCount](int& vc) { vc = (vc+1)%vcCount; });

                BufferFIFO<Flit*>* buf = buffers.at(requesting_in.conPos)->at(requesting_in.vc);
                Flit* flit = buf->front();
                assert(flit);
                flit->packet->numhops++;
                flit->packet->traversedRouters.push_back(this->id);
            }
        }
    }
}

int RouterVC::VCAllocation_getNextFreeVC(int out)
{
    LOG(globalReport.verbose_router_function_calls,
            "Router" << this->id << "in VCAllocation_getNextFreeVC() @ " << sc_time_stamp());
    std::vector<int> used_vcs = getAllocatedVCsOfOutDir(out);
    std::sort(used_vcs.begin(), used_vcs.end());

    if (used_vcs.empty())   // if there are no used vcs.
        return 0;
    else {
        connID_t con_id = node.connections.at(out);
        Connection* con = &globalResources.connections.at(con_id);
        int vcCount = con->getVCCountForNode(node.id);
        if (used_vcs.size()==vcCount)   // if all vcs are used.
            return -1;
        else {
            std::vector<int> search_values(vcCount);
            std::iota(search_values.begin(), search_values.end(), 0);
            for (auto& sv:search_values) {
                auto result = std::find(used_vcs.begin(), used_vcs.end(), sv);
                if (result==used_vcs.end())
                    return sv;
            }
        }
    }
}

std::map<int, std::vector<Channel>> RouterVC::switchAllocation_generateRequests()
{
    LOG(globalReport.verbose_router_function_calls,
            "Router" << this->id << "in switchAllocation_generateRequests() @ " << sc_time_stamp());
    std::map<int, std::vector<Channel>> requests{};
    int nodeConnsSize = node.connections.size();
    for (int in_conPos = 0; in_conPos<nodeConnsSize; in_conPos++) {
        std::vector<int> allocated_vcs = getAllocatedVCsOfInDir(in_conPos);

        // subtract empty_vcs from allocated_vcs
        std::vector<int> empty_vcs{};
        for (int in_vc:allocated_vcs) {
            BufferFIFO<Flit*>* buf = buffers.at(in_conPos)->at(in_vc);
            if (buf->empty())
                empty_vcs.push_back(in_vc);
        }
        for (auto delete_vc : empty_vcs) {
            auto it = std::find(allocated_vcs.begin(), allocated_vcs.end(), delete_vc);
            if (it!=allocated_vcs.end())
                allocated_vcs.erase(it);
        }

        if (!allocated_vcs.empty()) {
            auto vcs = &switchAllocation_inputVC_rrList.at(in_conPos);
            int in_vc = -1;
            for (auto& vc:*vcs) {
                auto result = std::find(allocated_vcs.begin(), allocated_vcs.end(), vc);
                if (result!=allocated_vcs.end()) {
                    in_vc = vc;
                    break;
                }
            }
            assert(in_vc!=-1);
            Channel in{in_conPos, in_vc};
            Channel out = routingTable.at(in);
            insert_request(out.conPos, in, requests);
        }
    }
    return requests;
}

void RouterVC::switchAllocation_generateAck(const std::map<int, std::vector<Channel>>& requests)
{
    LOG(globalReport.verbose_router_function_calls,
            "Router" << this->id << "in switchAllocation_generateAck() @ " << sc_time_stamp());
    for (auto& request:requests) {
        int requested_out_dir = request.first;
        std::vector<Channel> requesting_ins = request.second;
        auto vcs = generateVCsFromPtr(requested_out_dir, switchAllocation_outputVCPtr);

        bool found = false;
        Channel selected_in{};
        for (auto vc : vcs) {
            if (!found) {
                for (auto requesting_in : requesting_ins) {
                    assert(routingTable.count(requesting_in));
                    int requested_out_vc = routingTable.at(requesting_in).vc;
                    if (requested_out_vc==vc) {
                        found = true;
                        selected_in = requesting_in;
                        break;
                    }
                }
            }
        }

        int requested_out_vc = routingTable.at(selected_in).vc;
        switchTable[selected_in] = {requested_out_dir, requested_out_vc};
        auto vcList = &switchAllocation_inputVC_rrList.at(selected_in.conPos);
        auto vcCount = vcList->size();
        std::for_each(vcList->begin(), vcList->end(), [&vcCount](int& vc) { vc = (vc+1)%vcCount; });
        switchAllocation_outputVCPtr.at(requested_out_dir) = requested_out_vc+1;
    }
}

std::vector<int> RouterVC::getAllocatedVCsOfInDir(int conPos)
{
    std::vector<int> vcs{};
    for (auto& entry:routingTable) {
        if (conPos==entry.first.conPos)
            vcs.push_back(entry.first.vc);
    }
    return vcs;
}

std::vector<int> RouterVC::getAllocatedVCsOfOutDir(int conPos)
{
    std::vector<int> vcs{};
    for (auto& entry:routingTable) {
        if (conPos==entry.second.conPos)
            vcs.push_back(entry.second.vc);
    }
    return vcs;
}

void RouterVC::send()
{
    LOG(globalReport.verbose_router_function_calls,
            "Router" << this->id << "in send() @ " << sc_time_stamp());
    for (auto& entry:switchTable) {
        Channel in = entry.first;
        Channel out = entry.second;
        if (creditCounter.count(out) && creditCounter.at(out)>0) {
            BufferFIFO<Flit*>* buf = buffers.at(in.conPos)->at(in.vc);
            assert(!buf->empty());
            Flit* flit = buf->front();
            assert(flit);
            buf->dequeue();
            globalReport.increaseBufferPop(this->id);
            classicPortContainer.at(out.conPos).portValidOut.write(true);
            classicPortContainer.at(out.conPos).portDataOut.write(flit);
            classicPortContainer.at(out.conPos).portVcOut.write(out.vc);
            Credit credit{in.vc};
            classicPortContainer.at(in.conPos).portFlowControlOut.write(credit);
            classicPortContainer.at(in.conPos).portFlowControlValidOut.write(true);
            globalReport.increaseCrossbar(this->id);
            Node n = globalResources.nodes.at(this->node.id);
            globalReport.increase_power_stats(this->id, n.getDirOfConPos(out.conPos));
            if (DIR::Local!=node.getDirOfConPos(out.conPos)) // NI has infinite buffers
                creditCounter.at(out)--;
            if (flit->type==TAIL) {
                routingTable.erase(in);
            }
            LOG(
                    (globalReport.verbose_router_send_head_flit && flit->type==HEAD) ||
                            globalReport.verbose_router_send_flit,
                    "Router" << this->id << "[" << DIR::toString(node.getDirOfConPos(out.conPos)) << out.vc
                             << "]\t- Send Flit "
                             << *flit);
        }
        else {
            LOG(globalReport.verbose_router_throttle,
                    "Router" << this->id << "(Node" << node.id << ")\t- Waiting for downstream Router!");
        }
    }
    switchTable.clear();
}

void RouterVC::receiveFlowControlCredit()
{
    LOG(globalReport.verbose_router_function_calls,
            "Router" << this->id << "in VCAllocation_generateRequests() @ " << sc_time_stamp());
    int nodeConnsSize = node.connections.size();
    for (int conPos = 0; conPos<nodeConnsSize; conPos++) {
        if (classicPortContainer.at(conPos).portFlowControlValidIn.posedge()) {
            if (classicPortContainer.at(conPos).portFlowControlValidIn.read()) {
                auto credit = classicPortContainer.at(conPos).portFlowControlIn.read();
                Channel ch{conPos, credit.vc};
                if (lastReceivedCreditID.at(ch)!=credit.id)
                    creditCounter.at(ch)++;
                lastReceivedCreditID.at(ch) = credit.id;
            }
        }
    }
}

int RouterVC::VCAllocation_getNextVCToBeAllocated(int in)
{
    LOG(globalReport.verbose_router_function_calls,
            "Router" << this->id << "in VCAllocation_getNextVCToBeAllocated() @ " << sc_time_stamp());

    auto vcSourceList = &VCAllocation_inputVC_rrList.at(in);

    for (auto vc : *vcSourceList) {
        // ignore allocated vcs
        Channel possiblyAllocated = {in, vc};
        auto allocation = routingTable.find(possiblyAllocated);
        if (allocation!=routingTable.end()) {
            continue;
        }
        //ignore empty vcs
        BufferFIFO<Flit*>* buf = buffers.at(in)->at(vc);
        Flit* flit = buf->front();
        if (!flit) {
            continue;
        }
        return vc;
    }

    return -1;
}

std::vector<int> RouterVC::generateVCsFromPtr(int direction, std::map<int, int> vcOffset)
{
    LOG(globalReport.verbose_router_function_calls,
            "Router" << this->id << "in generateVCsFromPtr() @ " << sc_time_stamp());
    // get all vcs starting from VCAllocation_inputVCPtr.
    connID_t con_id = node.connections.at(direction);
    Connection* con = &globalResources.connections.at(con_id);
    int vcCount = con->getVCCountForNode(node.id);
    std::vector<int> vcs(vcCount);
    //this generates a list staring from the ptr (e.g. [2 3 4 5])
    std::iota(vcs.begin(), vcs.end(), vcOffset.at(direction));
    // this generates a list under round robin, i.e. convert [2 3 4 5] to [2 3 0 1]
    std::for_each(vcs.begin(), vcs.end(), [&vcCount](int& vc) { vc = vc%vcCount; });
    return vcs;
}

RouterVC::~RouterVC()
{
}

void RouterVC::insert_request(int out_conPos, Channel in, std::map<int, std::vector<Channel>>& requests)
{
    if (!requests.count(out_conPos)) {
        auto entry = std::make_pair(out_conPos, std::vector<Channel>{in});
        requests.insert(entry);
    }
    else
        requests.at(out_conPos).push_back(in);
}
