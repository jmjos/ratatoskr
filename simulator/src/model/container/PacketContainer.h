/*******************************************************************************
 * -----------------------------------------------------------------------------
 *     A-3D-NoC Simulator
 *     Copyright (C) 2014-2017 Jan Moritz Joseph (joseph(at)jmjoseph.de),
 *     Otto-von-Guericke Univeristaet Magdeburg
 * 
 *     This program is free software: you can redistribute it and/or modify
 *     it under the terms of the GNU General Public License as published by
 *     the Free Software Foundation, either version 3 of the License, or
 *     (at your option) any later version.
 * 
 *     This program is distributed in the hope that it will be useful,
 *     but WITHOUT ANY WARRANTY; without even the implied warranty of
 *     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *     GNU General Public License for more details.
 * 
 *     You should have received a copy of the GNU General Public License
 *     along with this program.  If not, see <http://www.gnu.org/licenses/>.
 * -----------------------------------------------------------------------------
 ******************************************************************************/
#ifndef SRC_MODEL_CONTAINER_PACKETCONTAINER_H_
#define SRC_MODEL_CONTAINER_PACKETCONTAINER_H_

#include "Container.h"
#include "model/traffic/Packet.h"

class PacketSignalContainer: public SignalContainer {
public:
	sc_signal<bool> sigValid;
	sc_signal<bool> sigFlowControl;
	sc_signal<Packet*> sigData;

	PacketSignalContainer(sc_module_name nm) :
			SignalContainer(nm) {
	}
	;
	~PacketSignalContainer() {
	}
	;

};

class PacketPortContainer: public PortContainer {
public:
	sc_in<bool> portValidIn;
	sc_in<bool> portFlowControlIn;
	sc_in<Packet*> portDataIn;

	sc_out<bool> portValidOut;
	sc_out<bool> portFlowControlOut;
	sc_out<Packet*> portDataOut;

	PacketPortContainer(sc_module_name nm) :
			PortContainer(nm) {
	}

	~PacketPortContainer() {
	}


	void bind(SignalContainer* sIn, SignalContainer* sOut) {
		PacketSignalContainer* cscin = dynamic_cast<PacketSignalContainer*>(sIn);
		PacketSignalContainer* cscout = dynamic_cast<PacketSignalContainer*>(sOut);

		assert(cscin);
		assert(cscout);

		portValidIn(cscin->sigValid);
		portFlowControlIn(cscin->sigFlowControl);
		portDataIn(cscin->sigData);

		portValidOut(cscout->sigValid);
		portFlowControlOut(cscout->sigFlowControl);
		portDataOut(cscout->sigData);

	}
	;

	void bindOpen(SignalContainer* sIn) {
		PacketSignalContainer* cscin = dynamic_cast<PacketSignalContainer*>(sIn);
		assert(cscin);

		portValidIn(cscin->sigValid);
		portFlowControlIn(cscin->sigFlowControl);
		portDataIn(cscin->sigData);

		portValidOut(portOpen);
		portFlowControlOut(portOpen);
		portDataOut(portOpen);
	}

};

#endif /* SRC_MODEL_CONTAINER_FLITCONTAINER_H_ */
