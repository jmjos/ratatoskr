/*******************************************************************************
 * Copyright (C) 2018 joseph
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
/*
 * RoutingXYZ.h
 *
 *  Created on: Jan 15, 2017
 *      Author: wrieden
 */

#ifndef SRC_MODEL_ROUTINGS_ROUTINGESPADA_H_
#define SRC_MODEL_ROUTINGS_ROUTINGESPADA_H_

#include "Routing.h"

struct RoutingESPADA : public Routing {
    int classcount;
	std::map<Channel, std::pair<int,int>> tagLocal;
    std::vector<std::set<std::pair<int,int>>> forbiddenTurns;

    RoutingESPADA(Node* node);
	~RoutingESPADA();
	void checkValid();
	void route(RoutingInformation*, RoutingPacketInformation*);
	void makeDecision(RoutingInformation*, RoutingPacketInformation*);

	void beginCycle(RoutingInformation*);
	void endCycle(RoutingInformation*);

	std::set<Channel> checkValidChannelForDir(RoutingInformation*, int, int);
};

#endif /* SRC_MODEL_ROUTINGS_ROUTINGXYZ_H_ */
