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

class ClassicSignalContainer : public SignalContainer {
public:
    sc_signal<bool> sigValid;
    sc_signal<std::vector<bool>*> sigFlowControl;
    sc_signal<Flit*> sigData;
    sc_signal<int> sigVc;

    ClassicSignalContainer(sc_module_name nm)
            :
            SignalContainer(nm)
    {
    };

    ~ClassicSignalContainer()
    {
    };

};

class ClassicPortContainer : public PortContainer {
public:
    sc_in<bool> portValidIn;
    sc_in<std::vector<bool>*> portFlowControlIn;
    sc_in<Flit*> portDataIn;
    sc_in<int> portVcIn;

    sc_out<bool> portValidOut;
    sc_out<std::vector<bool>*> portFlowControlOut;
    sc_out<Flit*> portDataOut;
    sc_out<int> portVcOut;

    ClassicPortContainer(sc_module_name nm)
            :
            PortContainer(nm)
    {
    };

    ~ClassicPortContainer()
    {
    };

    void bind(SignalContainer* sIn, SignalContainer* sOut)
    {
        auto cscin = dynamic_cast<ClassicSignalContainer*>(sIn);
        auto cscout = dynamic_cast<ClassicSignalContainer*>(sOut);

        assert(cscin);
        assert(cscout);

        portValidIn(cscin->sigValid);
        portFlowControlIn(cscin->sigFlowControl);
        portDataIn(cscin->sigData);
        portVcIn(cscin->sigVc);

        portValidOut(cscout->sigValid);
        portFlowControlOut(cscout->sigFlowControl);
        portDataOut(cscout->sigData);
        portVcOut(cscout->sigVc);

    };

    void bindOpen(SignalContainer* sIn)
    {
        auto cscin = dynamic_cast<ClassicSignalContainer*>(sIn);

        assert(cscin);

        portValidIn(cscin->sigValid);
        portFlowControlIn(cscin->sigFlowControl);
        portDataIn(cscin->sigData);
        portVcIn(cscin->sigVc);

        portValidOut(portOpen);
        portFlowControlOut(portOpen);
        portDataOut(portOpen);
        portVcOut(portOpen);
    }
};

