//////////////////////////////////////////////////////////////////////////////////
//#include "Packet.h"
//
//int Packet::idcnt=0;
//
//Packet::Packet(int src, int dst, int size, int generationTime, int trafficTypeId, int as, int ac) {
//	this->id = idcnt;
//	idcnt++;
//
//	this->src = src;
//	this->dst = dst;
//	this->size = size;
//	this->generationTime = generationTime;
//	this->dbid = rep.registerElement("Packet", id);
//	this->pkgclass = 0;
//	this->as = as;
//	this->ac= ac;
//	this->trafficTypeId = trafficTypeId;
//
//	rep.reportAttribute(dbid, "packet_src", std::to_string(src));
//	rep.reportAttribute(dbid, "packet_dst", std::to_string(dst));
//}
//
//Packet::~Packet() {
//	for (Flit* f : toTransmit) {
//		delete f;
//	}
//	for (Flit* f : inTransmit) {
//		delete f;
//	}
//	for (Flit* f : transmitted) {
//		delete f;
//	}
//}
//
