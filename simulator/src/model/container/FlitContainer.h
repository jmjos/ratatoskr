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

class FlitSignalContainer : public SignalContainer {
public:
    sc_signal<bool> sigValid;
    sc_signal<bool> sigFlowControlValid;
    sc_signal<Credit> sigFlowControl;
    sc_signal<Flit*> sigData;
    sc_signal<int> sigVc;

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
    sc_in<Credit> portFlowControlIn;
    sc_in<Flit*> portDataIn;
    sc_in<int> portVcIn;

    sc_out<bool> portValidOut;
    sc_out<bool> portFlowControlValidOut;
    sc_out<Credit> portFlowControlOut;
    sc_out<Flit*> portDataOut;
    sc_out<int> portVcOut;

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
        portFlowControlIn(cscin->sigFlowControl);
        portDataIn(cscin->sigData);
        portVcIn(cscin->sigVc);

        portValidOut(cscout->sigValid);
        portFlowControlValidOut(cscout->sigFlowControlValid);
        portFlowControlOut(cscout->sigFlowControl);
        portDataOut(cscout->sigData);
        portVcOut(cscout->sigVc);

    }

    void bindOpen(SignalContainer* sIn)
    {
        auto cscin = dynamic_cast<FlitSignalContainer*>(sIn);

        assert(cscin);

        portValidIn(cscin->sigValid);
        portFlowControlValidIn(cscin->sigFlowControlValid);
        portFlowControlIn(cscin->sigFlowControl);
        portDataIn(cscin->sigData);
        portVcIn(cscin->sigVc);

        portValidOut(portOpen);
        portFlowControlValidOut(portOpen);
        portFlowControlOut(portOpen);
        portDataOut(portOpen);
        portVcOut(portOpen);
    }
};
