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

class FlitSignalContainer: public SignalContainer {
public:
	sc_signal<bool> sigValid;
	sc_signal<std::vector<bool>*> sigFlowControl;
	sc_signal<std::vector<int>*> sigTag;
	sc_signal<std::vector<bool>*> sigEmpty;
	sc_signal<Flit*> sigData;
	sc_signal<int> sigVc;
	sc_signal<bool> sigReset;

	FlitSignalContainer(sc_module_name nm) :
			SignalContainer(nm) {
	}

	~FlitSignalContainer() {
	}

};

class FlitPortContainer: public PortContainer {
public:
	sc_in<bool> portValidIn;
	sc_in<std::vector<bool>*> portFlowControlIn;
	sc_in<std::vector<int>*> portTagIn;
	sc_in<std::vector<bool>*> portEmptyIn;
	sc_in<Flit*> portDataIn;
	sc_in<int> portVcIn;
	sc_in<bool> portResetIn;

	sc_out<bool> portValidOut;
	sc_out<std::vector<bool>*> portFlowControlOut;
	sc_out<std::vector<int>*> portTagOut;
	sc_out<std::vector<bool>*> portEmptyOut;
	sc_out<Flit*> portDataOut;
	sc_out<int> portVcOut;
	sc_out<bool> portResetOut;

	FlitPortContainer(sc_module_name nm) :
			PortContainer(nm) {
	}

	~FlitPortContainer() {
	}

	void bind(SignalContainer* sIn, SignalContainer* sOut) {
		FlitSignalContainer* cscin = dynamic_cast<FlitSignalContainer*>(sIn);
		FlitSignalContainer* cscout = dynamic_cast<FlitSignalContainer*>(sOut);

		assert(cscin);
		assert(cscout);

		portValidIn(cscin->sigValid);
		portFlowControlIn(cscin->sigFlowControl);
		portTagIn(cscin->sigTag);
		portEmptyIn(cscin->sigEmpty);
		portDataIn(cscin->sigData);
		portVcIn(cscin->sigVc);
		portResetIn(cscin->sigReset);

		portValidOut(cscout->sigValid);
		portFlowControlOut(cscout->sigFlowControl);
		portTagOut(cscout->sigTag);
		portEmptyOut(cscout->sigEmpty);
		portDataOut(cscout->sigData);
		portVcOut(cscout->sigVc);
		portResetOut(cscout->sigReset);

	}

	void bindOpen(SignalContainer* sIn) {
		FlitSignalContainer* cscin = dynamic_cast<FlitSignalContainer*>(sIn);

		assert(cscin);

		portValidIn(cscin->sigValid);
		portFlowControlIn(cscin->sigFlowControl);
		portTagIn(cscin->sigTag);
		portEmptyIn(cscin->sigEmpty);
		portDataIn(cscin->sigData);
		portVcIn(cscin->sigVc);
		portResetIn(cscin->sigReset);

		portValidOut(portOpen);
		portFlowControlOut(portOpen);
		portTagOut(portOpen);
		portEmptyOut(portOpen);
		portDataOut(portOpen);
		portVcOut(portOpen);
		portResetOut(portOpen);

	}

};
