////////////////////////////////////////////////////////////////////////////////
// Copyright (C) 2018 Jan Moritz Joseph
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
////////////////////////////////////////////////////////////////////////////////
#include "Flit.h"
#include "Packet.h"

int Flit::idcnt = 0;

Flit::Flit(FlitType type, int seq_nb, Packet* p){
	this->id = idcnt++;
	this->dbid = rep.registerElement("Flit", id);
	this->type = type;
	this->seq_nb = seq_nb;
	this->packet = p;
	this->ac = 0;
	this->as = 0;
	this->trafficTypeId = 0;

	rep.reportAttribute(dbid, "flit_packet", std::to_string(p->id));
	rep.reportAttribute(dbid, "flit_type", std::to_string(type));
	rep.reportAttribute(dbid, "flit_seq", std::to_string(seq_nb));
}

Flit::Flit(FlitType type, int seq_nb, Packet* p, float as, float ac, int trafficTypeId){
	this->id = idcnt++;
	this->dbid = rep.registerElement("Flit", id);
	this->type = type;
	this->seq_nb = seq_nb;
	this->packet = p;
	this->ac = ac;
	this->as = as;
	this->trafficTypeId = trafficTypeId;
	rep.reportAttribute(dbid, "flit_packet", std::to_string(p->id));
	rep.reportAttribute(dbid, "flit_type", std::to_string(type));
	rep.reportAttribute(dbid, "flit_seq", std::to_string(seq_nb));
}


Flit::~Flit(){
}

ostream & operator <<(ostream & os, const Flit & flit){
	os << "[";
	switch (flit.type) {
	case HEAD:
		os << "H";
		break;
	case BODY:
		os << "B";
		break;
	case TAIL:
		os << "T";
		break;
	}
	os << "_"<< flit.id << ": " << flit.packet->src << "-->" << flit.packet->dst << "]";

	return os;
}

void sc_trace(sc_trace_file*& tf, const Flit& flit, std::string nm){
	sc_trace(tf, flit.type, nm + ".type");
	sc_trace(tf, flit.seq_nb, nm + ".seq_nb");
	sc_trace(tf, flit.id, nm + ".id");
}
