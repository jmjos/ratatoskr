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
#ifndef SRC_TRAFFIC_PACKET_H_
#define SRC_TRAFFIC_PACKET_H_

#include <string>
#include <iosfwd>
#include <ostream>
#include <queue>
#include <set>
#include <vector>
#include "Flit.h"
#include "utils/Report.h"


struct Packet {
	static int idcnt;
	int id;
	int dbid;

	Node* src;
	Node* dst;
	int size;
	int generationTime;
	int pkgclass;
	int trafficTypeId;
	float as; // statistical properties of traffic
	float ac; // statistical properties of traffic

	DataType* dataType = 0;

	int numhops;
		std::vector<int> traversedRouter;
		std::set<int> routerIDs;

	std::vector<Flit*> toTransmit;
	std::vector<Flit*> inTransmit;
	std::vector<Flit*> transmitted;

	Report& rep = Report::getInstance();

	Packet(Node* src, Node* dst, int size, int generationTime, int trafficTypeId, int as, int ac);
	virtual ~Packet();

	friend ostream & operator <<(ostream & os, const Packet& p);

};


#endif /* SRC_TRAFFIC_PACKET_H_ */
