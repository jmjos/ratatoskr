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
// VC Router
// - wormhole
// - virtual channel
// - round robin arbiter
// - flow control
#ifndef SRC_MODEL_ROUTER_ROUTERVC_H_
#define SRC_MODEL_ROUTER_ROUTERVC_H_

#include <model/container/FlitContainer.h>
#include "Router.h"
#include "Buffer.h"

#include "routings/Routing.h"
#include "routings/RoutingXYZ.h"
#include "routings/RoutingHeteroXYZ.h"
#include "routings/RoutingTMR.h"
#include "routings/RoutingDPR.h"
#include "routings/RoutingESPADA.h"

#include "selection/Selection.h"
#include "selection/SelectionRoundRobin.h"
#include "selection/SelectionOutputRoundRobin.h"
#include "selection/Selection1stFreeVC.h"
#include "selection/SelectionDyXYZ.h"
#include "selection/SelectionEDXYZ.h"
#include "selection/SelectionAgRA.h"
#include "selection/SelectionMAFA.h"

class RouterVC: public Router {

private:
	std::map<int, int> currentVCs; // for each direction, what is the current vc that sent a flit

	void arbitrateFlit(Channel in, Channel out, std::set<int>& arbitratedDirs);
	bool isDownStreamRouterReady(const Channel& in, const Channel& out);
	int getNextAvailableVC(int dir);
	std::vector<int> getVCsFromOccupytable(int dir);
	bool handleHeadFlitSpecialCases(RoutingPacketInformation* rpi, const Flit* flit, const Channel& in);
	bool handleBodyFlitWithDropFlag(RoutingPacketInformation* rpi, const Flit* flit, const Channel& in);
	void checkFailedDecision(RoutingPacketInformation* rpi, const Flit* flit, const Channel& in);

//	sc_trace_file *tf;
public:
	sc_vector<FlitPortContainer> classicPortContainer;
	std::vector<std::vector<BufferFIFO<Flit*>*>*> buffer;
	std::vector<std::vector<int>*> pkgcnt;
	std::vector<std::vector<Flit*>*> routedFlits;

	std::vector<std::vector<bool>*> flowControlOut;
	std::vector<std::vector<int>*> tagOut;
	std::vector<std::vector<bool>*> emptyOut;

	Routing* routing;
	RoutingInformation* rInfo;
	std::map<std::pair<Packet*, Channel>, RoutingPacketInformation*> rpInfo;

	Selection* selection;

	int rrDirOff = 0;

	std::set<std::tuple<Flit*, Channel, Channel>> arbitratedFlits; //flit, in, out
	std::set<std::pair<Packet*, Channel>> routedPackets;

	int crossbarcount;

	sc_in<bool> clk;

	std::vector<Flit*> lastReceivedFlits;

	RouterVC(sc_module_name nm, Node* node);
	~RouterVC();

	void initialize();
	void checkValid();
	void bind(Connection*, SignalContainer*, SignalContainer*);
	void send();
	void thread();
	void receive();
	void updateUsageStats();
	void route();
	void allocateVC();
	void arbitrate();
	void arbitrateFair();


	void negThred();
	void readControl();
	void writeControl();

};

#endif /* SRC_MODEL_ROUTER_RouterVC_H_ */
