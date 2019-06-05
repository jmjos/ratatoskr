/*******************************************************************************
 * Copyright (C) 2018 Jan Moritz Joseph
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
#pragma once

#include <set>
#include <model/container/FlitContainer.h>
#include <model/router/routings/BaseRouting.h>
#include "systemc.h"
#include "model/NetworkParticipant.h"
#include "utils/Structures.h"
#include "Buffer.h"

class Router : public NetworkParticipant, public sc_module {
public:
    int id;
    int dbid;
    Node& node;
    sc_vector<FlitPortContainer> classicPortContainer;
    std::vector<std::vector<BufferFIFO<Flit*>*>*> buffers;
    sc_in<bool> clk;
    std::map<Channel, std::array<flitID_t, 4>> lastReceivedFlitsID;
    std::map<Channel, Channel> routingTable; // key:in, value:out
    std::map<Channel, Channel> switchTable;  // key:in, value:out
    std::unique_ptr<BaseRouting> routingAlg;

    SC_HAS_PROCESS(Router);

    Router(sc_module_name nm, Node& node);

    ~Router();

    void initialize() override = 0;

    void bind(Connection*, SignalContainer*, SignalContainer*) override = 0;

    virtual void thread() = 0;

    virtual void receive() = 0;

    virtual void receiveFlowControlCredit() = 0;

    virtual void updateUsageStats() = 0;
};
