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

#include "systemc.h"
#include <iostream>
#include <fstream>

#include "model/traffic/Flit.h"
#include "model/container/FlitContainer.h"
#include "utils/GlobalResources.h"
#include "utils/GlobalReport.h"

enum LinkType {
    HORIZONTAL = 10, VERTICAL = 11
};

class Link : public sc_module {
public:
    GlobalResources& globalResources = GlobalResources::getInstance();
    GlobalReport& report = GlobalReport::getInstance();
    int id, globalID;
    Flit* previousFlit;
    Flit* currentFlit;
    int previousTransmissionState = 0;
    int currentTransmissionState = 0;
    LinkType linkType;
    int linkWidth;
    int linkDepth;
    // UNCOMMENT FOR RAW DATA ON LINK (@Lennart)
    // ofstream *rawDataOutput;
    sc_in<bool> clk;
    FlitPortContainer* classicPortContainer;

    SC_HAS_PROCESS(Link);

    Link(sc_module_name nm, const Connection& c, int globalID);

    ~Link() override;

    void bind(SignalContainer* sigContIn, SignalContainer* sigContOut);

    void bindOpen(SignalContainer* sigContIn);

    void passthrough_thread();
};

