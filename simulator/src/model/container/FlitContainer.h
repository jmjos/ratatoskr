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

#include "Container.h"

#include "model/traffic/Flit.h"
#include "utils/Structures.h"
#include <array>

class FlitSignalContainer : public SignalContainer {
public:
    sc_signal<bool> sigValid;
    sc_signal<bool> sigFlowControlValid;
    std::array<sc_signal<Credit>, 4> sigsFlowControl;
    std::array<sc_signal<Flit*>, 4> sigsData;
    std::array<sc_signal<int>, 4> sigsVc;

    explicit FlitSignalContainer(const sc_module_name& nm)
            :
            SignalContainer(nm)
    {
    }

    ~FlitSignalContainer() override = default;

};

class FlitPortContainer : public PortContainer {
public:
    sc_in<bool> portValidIn;
    sc_in<bool> portFlowControlValidIn;
    std::array<sc_in<Credit>, 4> portsFlowControlIn;
    std::array<sc_in<Flit*>, 4> portsDataIn;
    std::array<sc_in<int>, 4> portsVcIn;

    sc_out<bool> portValidOut;
    sc_out<bool> portFlowControlValidOut;
    std::array<sc_out<Credit>, 4> portsFlowControlOut;
    std::array<sc_out<Flit*>, 4> portsDataOut;
    std::array<sc_out<int>, 4> portsVcOut;

    explicit FlitPortContainer(const sc_module_name& nm)
            :
            PortContainer(nm)
    {
    }

    ~FlitPortContainer() override = default;

    void bind(SignalContainer* sIn, SignalContainer* sOut) override
    {
        auto cscin = dynamic_cast<FlitSignalContainer*>(sIn);
        auto cscout = dynamic_cast<FlitSignalContainer*>(sOut);

        assert(cscin);
        assert(cscout);

        portValidIn(cscin->sigValid);
        portFlowControlValidIn(cscin->sigFlowControlValid);
        size_t size = portsFlowControlIn.size();
        for (unsigned int i = 0; i<size; ++i) {
            portsFlowControlIn[i](cscin->sigsFlowControl[i]);
            portsDataIn[i](cscin->sigsData[i]);
            portsVcIn[i](cscin->sigsVc[i]);
        }

        portValidOut(cscout->sigValid);
        portFlowControlValidOut(cscout->sigFlowControlValid);
        for (unsigned int i = 0; i<size; ++i) {
            portsFlowControlOut[i](cscout->sigsFlowControl[i]);
            portsDataOut[i](cscout->sigsData[i]);
            portsVcOut[i](cscout->sigsVc[i]);
        }
    }

    void bindOpen(SignalContainer* sIn)
    {
        auto cscin = dynamic_cast<FlitSignalContainer*>(sIn);

        assert(cscin);

        portValidIn(cscin->sigValid);
        portFlowControlValidIn(cscin->sigFlowControlValid);
        size_t size = portsFlowControlIn.size();
        for (unsigned int i = 0; i<size; ++i) {
            portsFlowControlIn[i](cscin->sigsFlowControl[i]);
            portsDataIn[i](cscin->sigsData[i]);
            portsVcIn[i](cscin->sigsVc[i]);
        }

        portValidOut(portOpen);
        portFlowControlValidOut(portOpen);
        for (unsigned int i = 0; i<size; ++i) {
            portsFlowControlOut[i](portOpen);
            portsDataOut[i](portOpen);
            portsVcOut[i](portOpen);
        }
    }
};
