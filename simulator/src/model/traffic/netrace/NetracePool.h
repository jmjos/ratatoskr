/*******************************************************************************
 * Copyright (C) 2020 Jan Moritz Joseph
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

#include "systemc.h"
#include "utils/GlobalResources.h"
#include "utils/Report.h"
#include "utils/Structures.h"
#include "model/traffic/TrafficPool.h"
#include "model/traffic/netrace/ntNetrace.h"
#include "model/traffic/netrace/ntQueue.h"
#include "model/processingElement/ProcessingElementVC.h"

#define L2_LATENCY 8

typedef struct queue_node queue_node_t;
struct queue_node {
    nt_packet_t* packet;
    unsigned long long int cycle;
};

class NetracePool : public TrafficPool, sc_module {
public:
    unsigned long long int calc_packet_timing( nt_packet_t* );

    nt_header_t* header;
    int x_nodes, y_nodes;
    unsigned long long int cycle;


    SC_HAS_PROCESS(NetracePool);

    sc_event event;

    NetracePool(sc_module_name);//sc_module_name nm);

    ~NetracePool();

    void clear(Task*) override;

    void start() override;

    void thread();
};

