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
#include "Link.h"

Link::Link(sc_module_name nm, const Connection& c, int globalID)
        :
        id(c.id),
        globalID(globalID),
        previousFlit(nullptr),
        currentFlit(nullptr)
{
    classicPortContainer = new FlitPortContainer(
            ("link_portCont_"+std::to_string(this->id)).c_str());
    if (c.nodes.size()==1) {
        this->linkType = HORIZONTAL;
    }
    else {
        Node node0 = globalResources.nodes.at(c.nodes.at(0));
        Node node1 = globalResources.nodes.at(c.nodes.at(1));
        if (node0.pos.z==node1.pos.z) {
            this->linkType = HORIZONTAL;
        }
        else {
            this->linkType = VERTICAL;
        }
    }
    // this->rawDataOutput = new ofstream((std::string) nm + ".txt");
    SC_THREAD(passthrough_thread);
    sensitive << clk.pos();
}

Link::~Link()
{
    delete classicPortContainer;
    // rawDataOutput->close();
    // delete rawDataOutput;
}

void Link::passthrough_thread()
{
    while (true) {
        wait();
        wait(0, SC_NS);
        std::string outputToFile;

        int IDLESTATE = 0;
        int HEADSTATE = 1;
        int HEADIDLESTATE = 2;
        int offset = 3; // three fields: idle, head, headidle

        if (!classicPortContainer->portValidIn.read()) {
            // this cycle idle
            if (previousTransmissionState==IDLESTATE) {
                // initially, no flits traverse links
                outputToFile = "__;";
                currentTransmissionState = IDLESTATE;
            }
            else if (currentFlit->type==HEAD) {
                // a head flit traversed previously
                outputToFile = std::to_string(currentFlit->dataType) + "_;";
                currentTransmissionState = HEADIDLESTATE;
            }
            else {
                // a flit already traversed the links
                outputToFile = std::to_string(currentFlit->dataType) + "_;";
                if (currentFlit->type!=HEAD && currentFlit->type!=BODY && currentFlit->type!=TAIL)
                    continue;
                currentTransmissionState = (2*currentFlit->dataType) + offset + 1;
            }
        }
        else {
            // this cycle active
            currentFlit = classicPortContainer->portDataIn.read();
            if (currentFlit->type==HEAD) {
                //received head flit
                outputToFile = "HD;";
                currentTransmissionState = HEADSTATE;
            }
            else {
                // received data flit
                outputToFile = std::to_string(currentFlit->dataType) + "D;";
                if (currentFlit->type!=HEAD && currentFlit->type!=BODY && currentFlit->type!=TAIL)
                    continue;
                currentTransmissionState = (2*currentFlit->dataType) + offset;
            }
        }

        //rawDataOutput->write(outputToFile.c_str(), 3);
        //rawDataOutput->flush();
        report.issueLinkMatrixUpdate(globalID, currentTransmissionState, previousTransmissionState);

        previousTransmissionState = currentTransmissionState;
        previousFlit = currentFlit;
    }
}

void Link::bind(SignalContainer* sigContIn, SignalContainer* sigContOut)
{
    classicPortContainer->bind(sigContIn, sigContOut);
}

void Link::bindOpen(SignalContainer* sigContIn)
{
    classicPortContainer->bindOpen(sigContIn);
}
