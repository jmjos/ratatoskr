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
#include "model/traffic/Packet.h"

class PacketSignalContainer : public SignalContainer {
public:
    sc_signal<bool> sigValid;
    sc_signal<bool> sigFlowControl;
    sc_signal<Packet*> sigData;

    explicit PacketSignalContainer(const sc_module_name& nm)
            :
            SignalContainer(nm)
    {
    };

    ~PacketSignalContainer() override = default;

};

class PacketPortContainer : public PortContainer {
public:
    sc_in<bool> portValidIn;
    sc_in<bool> portFlowControlIn;
    sc_in<Packet*> portDataIn;

    sc_out<bool> portValidOut;
    sc_out<bool> portFlowControlOut;
    sc_out<Packet*> portDataOut;

    explicit PacketPortContainer(const sc_module_name& nm)
            :
            PortContainer(nm)
    {
    }

    ~PacketPortContainer() override = default;

    void bind(SignalContainer* sIn, SignalContainer* sOut) override
    {
        auto cscin = dynamic_cast<PacketSignalContainer*>(sIn);
        auto cscout = dynamic_cast<PacketSignalContainer*>(sOut);

        assert(cscin);
        assert(cscout);

        portValidIn(cscin->sigValid);
        portFlowControlIn(cscin->sigFlowControl);
        portDataIn(cscin->sigData);
        portValidOut(cscout->sigValid);
        portFlowControlOut(cscout->sigFlowControl);
        portDataOut(cscout->sigData);
    };

    void bindOpen(SignalContainer* sIn)
    {
        auto cscin = dynamic_cast<PacketSignalContainer*>(sIn);

        assert(cscin);

        portValidIn(cscin->sigValid);
        portFlowControlIn(cscin->sigFlowControl);
        portDataIn(cscin->sigData);
        portValidOut(portOpen);
        portFlowControlOut(portOpen);
        portDataOut(portOpen);
    }
};
