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
#include "RouterVC.h"

RouterVC::RouterVC(sc_module_name nm, Node *node) :
        Router(nm, node) {

    int conCount = node->connections.size();
    crossbarcount = 0;
    rInfo = new RoutingInformation(node);
    classicPortContainer.init(conCount);
    buffer.resize(conCount);
    flowControlOut.resize(conCount);
    tagOut.resize(conCount);
    emptyOut.resize(conCount);
    pkgcnt.resize(conCount);
    routedFlits.resize(conCount);
    for (int dir = 0; dir < conCount; dir++) {
        int vcCount = node->connections.at(dir)->getVCCountForNode(node);
        buffer.at(dir) = new std::vector<BufferFIFO<Flit *> *>(vcCount);
        flowControlOut.at(dir) = new std::vector<bool>(vcCount, true);
        tagOut.at(dir) = new std::vector<int>(vcCount, 10); // Why 10?
        emptyOut.at(dir) = new std::vector<bool>(vcCount, true);
        pkgcnt.at(dir) = new std::vector<int>(vcCount, 0);
        routedFlits.at(dir) = new std::vector<Flit *>(vcCount);
        for (int vc = 0; vc < vcCount; vc++) {
            BufferFIFO<Flit *> *buf;
            if (globalResources.bufferDepthType == "single")
                buf = new BufferFIFO<Flit *>(node->connections.at(dir)->getBufferDepthForNode(node));
            else if (globalResources.bufferDepthType == "perVC")
                buf = new BufferFIFO<Flit *>(node->connections.at(dir)->getBufferDepthForNodeAndVC(node, vc));
            else FATAL("The type of buffer depth is not defined!");
            buffer.at(dir)->at(vc) = buf;
            rep.reportAttribute(buf->dbid, "node", std::to_string(node->id));
            rep.reportAttribute(buf->dbid, "dir", std::to_string(dir));
            rep.reportAttribute(buf->dbid, "vc", std::to_string(vc));
            rInfo->tagOut[Channel(dir, vc)] = 0;
        }
    }

    if (node->type->routing == "XYZ") {
        routing = new RoutingXYZ(node);
    } else if (node->type->routing == "HeteroXYZ") {
        routing = new RoutingHeteroXYZ(node);
    } else if (node->type->routing == "TMR") {
        routing = new RoutingTMR(node);
    } else if (node->type->routing == "DPR") {
        routing = new RoutingDPR(node);
    } else if (node->type->routing == "ESPADA") {
        // routing = new RoutingESPADA(node);
    } else {
        FATAL("Router" << this->id << "\t- Unknown Routing: " << node->type->routing);
    }

    if (node->type->selection == "RoundRobin") {
        selection = new SelectionRoundRobin(node);
    } else if (node->type->selection == "OutputRoundRobin") {
        selection = new SelectionOutputRoundRobin(node);
    } else if (node->type->selection == "1stFreeVC") {
        selection = new Selection1stFreeVC(node);
    } else if (node->type->selection == "DyXYZ") {
        selection = new SelectionDyXYZ(node);
    } else if (node->type->selection == "EDXYZ") {
        selection = new SelectionEDXYZ(node);
    } else if (node->type->selection == "AgRA") {
        //	selection = new SelectionAgRA(node);
    } else if (node->type->selection == "MAFA") {
        // selection = new SelectionMAFA(node);
    } else {
        FATAL("Router" << this->id << "\t- Unknown selection: " << node->type->selection);
    }

/*	tf = sc_create_vcd_trace_file(nm);
    tf->set_time_unit(1, SC_NS);
    sc_trace(tf, clk, "clk");
    //trace input
    for (int dir = 0; dir < conCount; dir++) {
        sc_trace(tf, classicPortContainer.at(dir).portValidIn,
                 "validIn." + DIR::toString(node->conToDir[dir]));
    }*/

    lastReceivedFlits.resize(conCount);
}

RouterVC::~RouterVC() {
//	sc_close_vcd_trace_file(tf);
}

void RouterVC::initialize() {
    checkValid();

    routing->checkValid();
    // selection->checkValid();

    SC_METHOD(thread);
    sensitive << clk.pos() << clk.neg();

    SC_METHOD(updateUsageStats);
    sensitive << clk.pos();

    SC_METHOD(receive);

    for (unsigned int con = 0; con < node->connections.size(); con++) {
        classicPortContainer.at(con).portValidOut.write(false);
        classicPortContainer.at(con).portFlowControlOut.write(flowControlOut.at(con));
        classicPortContainer.at(con).portTagOut.write(tagOut.at(con));
        classicPortContainer.at(con).portEmptyOut.write(emptyOut.at(con));
        sensitive << classicPortContainer.at(con).portValidIn.pos();
    }
}

void RouterVC::checkValid() {
}

void RouterVC::bind(Connection *con, SignalContainer *sigContIn, SignalContainer *sigContOut) {
    classicPortContainer.at(node->conToPos.at(con)).bind(sigContIn, sigContOut);
}

void RouterVC::thread() {
    if (clk.posedge()) {
        readControl();
        routing->beginCycle(rInfo);

        // #4 send
        send();
        // #3 arbitrate
        if (globalResources.arbiterType == "rrVC")
            arbitrate();
        else if (globalResources.arbiterType == "fair")
            arbitrateFair();
        else FATAL("Unknown arbiter type!");
        // #2 receive & route
        route();
        // #1 allocate VC
        allocateVC();

    } else if (clk.negedge()) {
        // unset valids
        for (unsigned int con = 0; con < node->connections.size(); con++) {
            classicPortContainer.at(con).portValidOut.write(false);
        }

        // used to set signals that could propagate instantaneously else
        for (unsigned int dir = 0; dir < node->connections.size(); dir++) {

            Connection *c = node->connections.at(dir);
            int bufferDepth = c->bufferDepth.at(c->nodePos.at(node));
            c->bufferUtilization.at(c->nodePos.at(node)) = 0; // TODO
            c->bufferCongestion.at(c->nodePos.at(node)) = 0;

            for (unsigned int vc = 0; vc < buffer.at(dir)->size(); vc++) {
                int occupiedSlots = buffer.at(dir)->at(vc)->occupied();
                c->vcBufferUtilization.at(c->nodePos.at(node)).at(vc) = occupiedSlots;
                c->vcBufferCongestion.at(c->nodePos.at(node)).at(vc) = (float) occupiedSlots / (float) bufferDepth;
                c->bufferUtilization.at(c->nodePos.at(node)) += occupiedSlots;
                c->bufferCongestion.at(c->nodePos.at(node)) += (float) occupiedSlots / (float) bufferDepth;
            }
            c->bufferCongestion.at(c->nodePos.at(node)) /= buffer.at(dir)->size();
        }

        node->congestion = (float) crossbarcount / (float) node->connections.size();
        routing->endCycle(rInfo);
        writeControl();
    }
}

void RouterVC::readControl() {
    for (unsigned int i = 0; i < node->connections.size(); i++) {
        std::vector<bool> *fc = classicPortContainer.at(i).portFlowControlIn.read();
        std::vector<int> *tag = classicPortContainer.at(i).portTagIn.read();
        std::vector<bool> *empty = classicPortContainer.at(i).portEmptyIn.read();

        for (unsigned int vc = 0; vc < fc->size(); vc++) {
            rInfo->flowIn[Channel(i, vc)] = fc->at(vc);
            rInfo->tagIn[Channel(i, vc)] = tag->at(vc);
            rInfo->emptyIn[Channel(i, vc)] = empty->at(vc);
        }
    }
}

void RouterVC::writeControl() {
    for (unsigned int dir = 0; dir < node->connections.size(); dir++) {
        for (unsigned int vc = 0; vc < buffer.at(dir)->size(); vc++) {
            // TODO HACK this needs to be 2 due to signal propagation speed to
            // upstream router. Please note, this should be fixed as it may be not
            // safe for all cases. 4 is too large, 3 might be okay the value should be
            // dynamic, i.e. counter in upstream router

            flowControlOut.at(dir)->at(vc) = buffer.at(dir)->at(vc)->free() >= 3;
            tagOut.at(dir)->at(vc) = rInfo->tagOut.at(Channel(dir, vc));
            emptyOut.at(dir)->at(vc) = buffer.at(dir)->at(vc)->empty();

            classicPortContainer.at(dir).portFlowControlOut.write(flowControlOut.at(dir));
            classicPortContainer.at(dir).portTagOut.write(tagOut.at(dir));
            classicPortContainer.at(dir).portEmptyOut.write(emptyOut.at(dir));
        }
    }
}

void RouterVC::receive() {
    // read input ports to buffers
    for (unsigned int dirIn = 0; dirIn < node->connections.size(); dirIn++) {
        if (classicPortContainer.at(dirIn).portValidIn.read()) {
            Flit *flit = classicPortContainer.at(dirIn).portDataIn.read();
            int vcIn = classicPortContainer.at(dirIn).portVcIn.read();
            BufferFIFO<Flit *> *buf = buffer.at(dirIn)->at(vcIn);
            Channel c = Channel(dirIn, vcIn);

            // prevents double writing of data, which occurs for asynchronous buffers.
            // problem: this function is executed for all buffers if new data arrive. If some directions are clocked at different data rate, the slower data rates are duplicated.

            Flit *lrFlit = lastReceivedFlits.at(dirIn);
            if (!lrFlit || lrFlit->id != flit->id) {
                if (flit->type == HEAD) {

                    if (rpInfo.count({flit->packet, c})) FATAL("packet duplication!");
                    LOG(
                            (globalResources.verbose_router_receive_head_flit) || globalResources.verbose_router_receive_flit,
                            "Router" << this->id << "[" << DIR::toString(node->conToDir[dirIn]) << vcIn
                                     << "]\t- Receive Flit " << *flit);
                    pkgcnt.at(dirIn)->at(vcIn)++;
                    rInfo->tagOut.at(Channel(dirIn, vcIn)) = flit->packet->pkgclass;

                }
                // rep.reportEvent(dbid, "router_receive_flit", std::to_string(flit->id));
                if (!buf->enqueue(flit)) {
                    LOG(globalResources.verbose_router_buffer_overflow,
                        "Router" << this->id << "[" << DIR::toString(node->conToDir[dirIn]) << vcIn
                                 << "]\t- Buffer Overflow " << *flit);
                } else {
                    rep.reportEvent(buf->dbid, "buffer_enqueue_flit", std::to_string(flit->id));

                    lastReceivedFlits[dirIn] = flit;

                    if (flit->type == HEAD) {
                        rpInfo[{flit->packet, c}] = new RoutingPacketInformation(flit->packet);
                        rpInfo[{flit->packet, c}]->inputChannel = c;
                    }
                }
            }
        }
    }
}

// VC and buffer usage statistics
void RouterVC::updateUsageStats() {
    for (unsigned int dirIn = 0; dirIn < node->connections.size(); dirIn++) {
        int numVCs = buffer.at(dirIn)->size();
        std::vector<int> buffSizes;
        int numberActiveVCs = 0;
        BufferFIFO<Flit *> *buf;
        for (unsigned int vcIn = 0; vcIn < numVCs; vcIn++) {
            buf = buffer.at(dirIn)->at(vcIn);
            if (!buf->empty()) {
                numberActiveVCs++;
                globalResourcesReportClass.updateBuffUsagePerVCHist(globalResourcesReportClass.bufferUsagePerVCHist, this->id, dirIn,
                                                           vcIn, buf->occupied(), numVCs);
            }
        }
        /* this 1 is added to create a column for numberOfActiveVCs=0.
           yes it's an extra column but it allow us to use the same function to update both buffer stats and VC stats.
        */
        globalResourcesReportClass.updateUsageHist(globalResourcesReportClass.VCsUsageHist, this->id, node->conToDir[dirIn],
                                          numberActiveVCs, numVCs + 1);
    }
}

void RouterVC::route() {
    for (unsigned int dirIn = 0; dirIn < node->connections.size(); dirIn++) {
        for (unsigned int vcIn = 0; vcIn < buffer.at(dirIn)->size(); vcIn++) {
            BufferFIFO<Flit *> *buf = buffer.at(dirIn)->at(vcIn);
            Flit *flit = buf->front();
            Channel in = Channel(dirIn, vcIn);
            //check, if flit is in front element of buffer and no route has been calculated
            if (flit && !rInfo->routingStateTable.count(in)) {
                if (flit->type == HEAD) { //check if head and calculate route
                    RoutingPacketInformation *rpi = rpInfo.at({flit->packet, in});
                    routing->route(rInfo, rpi);
                    if (!rpi->routedChannel.empty()) { //a route has been calculated
                        //insert route into state field
                        Channel out = Channel(rpi->routedChannel.begin()->dir, -1);
                        rInfo->routingStateTable.insert({in, out}); //only works for deterministic with a single path

                        routedPackets.insert({flit->packet, in});
                        flit->packet->numhops++;
                        flit->packet->traversedRouter.push_back(node->id);
                        flit->packet->routerIDs.insert(node->id);
                    } else {
                        rpi->dropFlag = true;
                        rep.reportEvent(dbid, "router_routefail", std::to_string(flit->id));
                    }
                }
            }
        }
    }
}

void RouterVC::allocateVC() {
    for (auto &routingPair : rInfo->routingStateTable) {
        Channel in = routingPair.first;
        Channel &out = routingPair.second;
        BufferFIFO<Flit *> *buf = buffer.at(in.dir)->at(in.vc);
        Flit *flit = buf->front();
        if (out.vc == -1) { // allocate output VCs, if necessary
            RoutingPacketInformation *rpi = rpInfo.at({flit->packet, in});
            selection->select(rInfo, rpi);
            if (rpi->selectedChannel.empty()) {
                rpi->dropFlag = true;
                rep.reportEvent(dbid, "router_selectfail", std::to_string(flit->id));
            } else {
                out.vc = rpi->selectedChannel.begin()->vc;
            }
        }
    }
}

/* Handles the following cases when we have a head flit:
 * 1- dropFlag is set.
 * 2- rerouteFlag is set.
 * 3- No selected channel.
 * 4- delayFlag is set.
 *
 * The function returns true in the following cases, and false otherwise:
 * 1- dropFlag is set and no selected channel.
 * 2- delayFlag is set.
 * */
bool RouterVC::handleHeadFlitSpecialCases(RoutingPacketInformation *rpi, const Flit *flit, const Channel &in) {
    bool result = false;

    if (rpInfo.at({flit->packet, in})->dropFlag) {
        rep.reportEvent(dbid, "router_decisfail", std::to_string(flit->id));
    }

    if (rpi->rerouteFlag) {
        rpi->rerouteFlag = false;
        LOG(true,
            "Router" << this->id << "[" << DIR::toString(node->conToDir[in.dir]) << in.vc << "]\t- Reroute! "
                     << *flit);
        routing->route(rInfo, rpi);
        selection->select(rInfo, rpi);

        if (rpi->selectedChannel.empty()) {
            rpi->dropFlag = true;
            rep.reportEvent(dbid, "router_select2fail", std::to_string(flit->id));
            LOG(true,
                "Router" << this->id << "[" << DIR::toString(node->conToDir[in.dir]) << in.vc
                         << "]\t- Reroute Fail! "
                         << *flit);

        } else {
            LOG(true, "Router" << this->id << "[" << DIR::toString(node->conToDir[in.dir]) << in.vc
                               << "]\t- Reroute Success! " << *flit);
            result = true;
        }

    } else if (rpi->delayFlag) {
        rpi->delayFlag = false;
        FATAL("Router" << this->id << "[" << DIR::toString(node->conToDir[in.dir]) << in.dir << "]\t- Delay! "
                       << *flit);
    }
    return result;
}

// Handles the situation when we have a body/tail flit and dropFlag is set and returns true to the caller.
bool RouterVC::handleBodyFlitWithDropFlag(RoutingPacketInformation *rpi, const Flit *flit, const Channel &in) {
    if (rpi->dropFlag) {
        if (flit->type == TAIL) {
            rep.reportEvent(dbid, "pkg_dropped", std::to_string(flit->packet->dbid));
            routedPackets.erase({flit->packet, in});
            rpInfo.erase({flit->packet, in});
            pkgcnt.at(in.dir)->at(in.vc)--;
            delete flit->packet;
            delete rpi;
            globalResources.droppedCounter++;
        }

        routedFlits.at(in.dir)->at(in.vc) = 0;
        return true;
    }
    return false;
}

// If not able to make a decision, then report it.
void RouterVC::checkFailedDecision(RoutingPacketInformation *rpi, const Flit *flit, const Channel &in) {
    if (rpi->outputChannel.dir == -1) {
        rep.reportEvent(dbid, "router_routefail", std::to_string(flit->id));
        FATAL("Router" << this->id << "[" << DIR::toString(node->conToDir[in.dir]) << in.dir
                       << "]\t- Failed Decision! "
                       << *flit);
    }
}

// This is the behavior of rrVC arbiter.
void RouterVC::arbitrate() {
    std::set<int> arbiterDirs;

    for (unsigned int dirIn = rrDirOff, i = 0; i < buffer.size(); dirIn = (dirIn + 1) % buffer.size(), i++) {
        for (unsigned int vcIn = 0; vcIn < buffer.at(dirIn)->size(); vcIn++) {
            Flit *flit = routedFlits.at(dirIn)->at(vcIn);
            Channel c = Channel(dirIn, vcIn);

            if (flit && rpInfo.count({flit->packet, c})) {

                RoutingPacketInformation *rpi = rpInfo.at({flit->packet, c});

                if (flit->type == HEAD) {
                    routing->makeDecision(rInfo, rpi);

                    bool badFlit = handleHeadFlitSpecialCases(rpi, flit, c);
                    if (badFlit)
                        continue;
                }

                bool badFlit = handleBodyFlitWithDropFlag(rpi, flit, c);
                if (badFlit)
                    continue;

                checkFailedDecision(rpi, flit, c);
                Channel out = rpi->outputChannel;

                bool isAvailable = isDownStreamRouterReady(c, out);
                if (!isAvailable)
                    continue;

                // Make sure router table has only one entry per flit
                if (!arbiterDirs.count(out.dir)) {
                    if (flit->type == HEAD && !rInfo->occupyTable.count(out)) {
                        rInfo->occupyTable[out] = flit->packet;
                        LOG(globalResources.verbose_router_assign_channel,
                            "Router" << this->id << "[" << DIR::toString(node->conToDir[out.dir]) << out.vc
                                     << "]\t- Assign to " << *flit);
                    }

                    if (rInfo->occupyTable.at(out) == flit->packet) {
                        arbitratedFlits.insert({flit, c, out});
                        arbiterDirs.insert(out.dir);
                        routedFlits.at(dirIn)->at(vcIn) = 0;
                    }
                }
            }
        }
    }

    rrDirOff = (rrDirOff + 1) % buffer.size();
    crossbarcount = arbiterDirs.size();
}

// Get all VCs of a direction from the occupy table.
std::vector<int> RouterVC::getVCsFromOccupytable(int dir) {
    std::vector<int> vcs;

    for (auto entry : rInfo->fairOccupyTable) {
        if (entry.first.dir == dir)
            vcs.push_back(entry.first.vc);
    }
    return vcs;
}

// Round Robin over the 'available' (i.e. used) VCs of a direction.
int RouterVC::getNextAvailableVC(int dir) {
    std::vector<int> vcs = getVCsFromOccupytable(dir);

    // If there is no VCs in the occupy table for this direction, then simply pick vc=0.
    if (vcs.size() == 0)
        currentVCs[dir] = 0;

        // If there is only one VC for this direction in the occupy table, then use the VC of that entry.
    else if (vcs.size() == 1)
        currentVCs[dir] = *vcs.begin();

        // Else, there are multiple VCs, then we need to round robin over them.
    else {
        int currVC = currentVCs.at(dir);

        /* Current VC contains the VC used in the previous cycle.
         * there are two cases:
         * 1) if this VC still has flits left, the next VC will be used (if-case)
         * 2) if there are no flit left, i.e. a tail flit passed the router, the first VC available will be used (else).
         * */
        if (std::count(vcs.begin(), vcs.end(), currVC)) {
            auto p = std::find(vcs.begin(), vcs.end(), currVC);
            if (p == vcs.end() - 1)
                p = vcs.begin();
            else
                p++;
            currentVCs[dir] = *p;
        } else
            currentVCs[dir] = *vcs.begin();
    }
    return currentVCs[dir];
}

// Check if the down stream router is ready (flow control).
bool RouterVC::isDownStreamRouterReady(const Channel &in, const Channel &out) {
    bool isReady = classicPortContainer.at(out.dir).portFlowControlIn.read()->at(out.vc);
    if (!isReady) {
        BufferFIFO<Flit *> *buf = buffer.at(in.dir)->at(in.vc);
        Flit *flit = buf->front();
        //Flit *flit = routedFlits.at(in.dir)->at(in.vc);
        LOG(globalResources.verbose_router_throttle,
            "Router" << this->id << "[" << DIR::toString(node->conToDir[in.dir]) << in.dir << "]\t- Flow Control! "
                     << *flit);
    }
    return isReady;
}

// The actual arbitration step; flits are inserted into arbitration data structure, which is read by send function.
void RouterVC::arbitrateFlit(Channel in, Channel out, std::set<int> &arbitratedDirs) {
    BufferFIFO<Flit *> *buf = buffer.at(in.dir)->at(in.vc);
    Flit *flit = buf->front();//routedFlits.at(dirIn)->at(vcIn);
    //Flit *flit = routedFlits.at(in.dir)->at(in.vc);
    arbitratedFlits.insert({flit, in, out});
    arbitratedDirs.insert(out.dir);
    //routedFlits.at(in.dir)->at(in.vc) = 0;
}

// This is the behavior of fairArbiter.
void RouterVC::arbitrateFair() {
    std::set<int> arbitratedDirs;
    // Step 1: Insert into occupy table the input channels with their associated output channels.
    for (unsigned int dirIn = rrDirOff, i = 0; i < buffer.size(); dirIn = (dirIn + 1) % buffer.size(), i++) {
        for (unsigned int vcIn = 0; vcIn < buffer.at(dirIn)->size(); vcIn++) {

            BufferFIFO<Flit *> *buf = buffer.at(dirIn)->at(vcIn);
            Flit *flit = buf->front();//routedFlits.at(dirIn)->at(vcIn);
            Channel in = Channel(dirIn, vcIn);

            if (flit && rpInfo.count({flit->packet, in})) {
                RoutingPacketInformation *rpi = rpInfo.at({flit->packet, in});
                if (flit->type == HEAD) {
                    if (rInfo->routingStateTable.count(in) && rInfo->routingStateTable.at(in).vc != -1) {
                        //routing->makeDecision(rInfo, rpi);
                        bool badFlit = handleHeadFlitSpecialCases(rpi, flit, in);
                        if (badFlit)
                            continue;
                        //Channel out = rpi->outputChannel;
                        Channel out = rInfo->routingStateTable.at(in);
                        rpi->outputChannel = out;
                        if (!rInfo->fairOccupyTable.count(out)) {
                            rInfo->fairOccupyTable[out] = in;
                        }
                    }
                } else { // body or tail flit
                    bool badFlit = handleBodyFlitWithDropFlag(rpi, flit, in);
                    if (badFlit)
                        continue;
                    checkFailedDecision(rpi, flit, in);
                }
            }
        }
    }

    // Step 2: From the occupy table, select the records that are ready for arbitration.
    for (unsigned int dirOut = rrDirOff, i = 0; i < buffer.size(); dirOut = (dirOut + 1) % buffer.size(), i++) {
        int vcOut = getNextAvailableVC(dirOut);
        Channel out = Channel(dirOut, vcOut);
        if (rInfo->fairOccupyTable.count(out) && !arbitratedDirs.count(out.dir)) {
            Channel in = rInfo->fairOccupyTable[out];
            BufferFIFO<Flit *> *buf = buffer.at(in.dir)->at(in.vc);
            Flit *flit = buf->front();//routedFlits.at(dirIn)->at(vcIn);
            //Flit *flit = routedFlits.at(in.dir)->at(in.vc);

            if (flit && isDownStreamRouterReady(in, out)) {
                arbitrateFlit(in, out, arbitratedDirs);
                buf->dequeue();
            }
        }

    }
    rrDirOff = (rrDirOff + 1) % buffer.size();
    crossbarcount = arbitratedDirs.size();
}

void RouterVC::send() {
    for (std::tuple<Flit *, Channel, Channel> p : arbitratedFlits) {

        Flit *flit = std::get<0>(p);
        Channel in = std::get<1>(p);
        Channel out = std::get<2>(p);
        RoutingPacketInformation *rpi;
        if (rpInfo.count({flit->packet, in}))
            rpi = rpInfo.at({flit->packet, in});

        classicPortContainer.at(out.dir).portValidOut.write(true);
        classicPortContainer.at(out.dir).portDataOut.write(flit);
        classicPortContainer.at(out.dir).portVcOut.write(out.vc);

        if (flit->type == TAIL) {
            routedPackets.erase({flit->packet, in});
            if (globalResources.arbiterType == "rrVC")
                rInfo->occupyTable.erase(out);
            else if (globalResources.arbiterType == "fair")
                rInfo->fairOccupyTable.erase(out);
            else FATAL("Unknown arbiter type!");

            if (node->conToDir[out.dir] != DIR::Local)
                rInfo->freeVCs[out] = true;

            rpInfo.erase({flit->packet, in});
            pkgcnt.at(in.dir)->at(in.vc)--;
            rInfo->routingStateTable.erase(in);
            if (rpInfo.count({flit->packet, in}))
                delete rpi;
        }
        LOG(
                (globalResources.verbose_router_send_head_flit && flit->type == HEAD) || globalResources.verbose_router_send_flit,
                "Router" << this->id << "[" << DIR::toString(node->conToDir[out.dir]) << out.vc << "]\t- Send Flit "
                         << *flit);
    }
    arbitratedFlits.clear();
}
