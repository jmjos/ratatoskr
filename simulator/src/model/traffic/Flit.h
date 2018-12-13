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
#include <string>
#include "utils/Report.h"

#include "utils/Structures.h"

using flitID_t = int;

class Packet;

enum FlitType {
    HEAD = 10, BODY = 11, TAIL = 12
};

class Flit {
public:
    Report& rep = Report::getInstance();
    static flitID_t idcnt;
    flitID_t id;
    flitID_t dbid;
    int seq_nb;
    FlitType type;
    Packet* packet;
    Flit* headFlit;
    dataTypeID_t dataType;
    double injectionTime;
    double generationTime;

    Flit(FlitType type, int seq_nb, Packet* p);

    Flit(FlitType type, int seq_nb, Packet* p, dataTypeID_t dataType, double generationTime);

    friend ostream& operator<<(ostream& os, const Flit& flit);

    friend void sc_trace(sc_trace_file*& tf, const Flit& flit, std::string nm);

    ~Flit() = default;
};

