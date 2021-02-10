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

#include "TrafficTracer.h"

TrafficTracer::TrafficTracer()
{
    if (globalResources.activateFlitTracing){
        tracefile.open("flitTrace_" + std::to_string(globalResources.rd_seed) + ".csv");
        tracefile << boost::format ("time, src, dst, packetid, flitid, type\n");
    }
}

void TrafficTracer::traceFlit(Flit *flit) {
    auto flit_type_to_str = [](enum FlitType ft) { switch(ft){case HEAD: return "H"; case BODY: return "B"; case TAIL: return "T"; case SINGLE: return "S"; default: return "-";} };

    float injTime = flit->injectionTime/(float) 1000;
    tracefile << boost::format("%10.10f, %i, %i, %i, %i, %s\n")
        % injTime
        % flit->packet->src.id
        % flit->packet->dst.id
        % flit->packet->idcnt
        % flit->id
        % flit_type_to_str(flit->type);
}