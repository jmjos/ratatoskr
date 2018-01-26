////////////////////////////////////////////////////////////////////////////////
// Copyright (C) 2018 joseph
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
#include "SyntheticPool.h"

SyntheticPool::SyntheticPool() {
	cout << endl;
	cout << "Synthetic Testrun" << endl;
}

SyntheticPool::~SyntheticPool() {

}

void SyntheticPool::start() {
	int datatypeid = 0;
	int datadestid = 0;
	int taskid = 0;
	for (SyntheticPhase sp : global.syntheticPhase) {
		LOG(true,
				"SyntheticPool\t Initialize phase \"" << sp.name <<"\" with \"" << sp.spatialDistribution << "\" distribution");
		LOG(true,
				"\t\t Starting Time: " << sp.minOffset<<" - "<<sp.maxOffset << + "ns"<<", Waves: "<< sp.waveCount<< ", Pkg per Wave: "<<sp.pkgPerWave<<", Wave Delay: "<<sp.waveDelay);

		std::map<int, int> srcToDst;

		if (sp.spatialDistribution == "uniform") {
			DataType* datatype = new DataType(datatypeid,
					std::to_string(datatypeid));
			datatypeid++;

			std::vector<DataRequirement*> requirements;

			std::vector<std::pair<float, std::vector<DataDestination*>>> possibilities;
			for (int i = 0; i < processingElements.size(); i++) {

				for (int j = 0; j < processingElements.size(); j++) {
					std::vector<DataDestination*> dests;
					dests.push_back(
							new DataDestination(datadestid, datatype, 1, 1, 0,
									0, sp.waveDelay, sp.waveDelay,
									processingElements.at(j)->node));
					datadestid++;
					possibilities.push_back(
							std::make_pair(1.f / processingElements.size(),
									dests));
				}
				Task* task = new Task(taskid, processingElements.at(i)->node, 1,
						1, requirements, possibilities);
				taskid++;

				processingElements.at(i)->execute(task);

			}

		} else {
			if (sp.spatialDistribution == "bitreverse") {
				//srcToDst = bitreverse();
			} else if (sp.spatialDistribution == "bitcomplement") {
				srcToDst = bitcomplement();
			} else if (sp.spatialDistribution == "tornado") {
				//	srcToDst = tornado();
			} else if (sp.spatialDistribution == "transpose") {
				//srcToDst = transpose();
			} else if (sp.spatialDistribution == "hotspot") {
				//	srcToDst = hotspot(sp.hotspot);
			} else {
				FATAL("Distribution is not implemented");
			}
		}

		for (std::pair<const int, int> con : srcToDst) {

			DataType* datatype = new DataType(datatypeid,
					std::to_string(datatypeid));
			datatypeid++;

			std::vector<DataRequirement*> requirements;

			std::vector<DataDestination*> destinations;
			destinations.push_back(
					new DataDestination(datadestid, datatype, 10, 10, 0, 0,
							sp.waveDelay, sp.waveDelay,
							processingElements.at(con.second)->node));
			datadestid++;

			std::vector<std::pair<float, std::vector<DataDestination*>>> possibilities;
			possibilities.push_back(std::make_pair(1.0, destinations));

			Task* task = new Task(taskid,
					processingElements.at(con.first)->node, 1, 1, requirements,
					possibilities);
			taskid++;

			processingElements.at(con.first)->execute(task);

		}

	}
}

void SyntheticPool::clear(Task*) {

}

//std::map<int, int> SyntheticPool::bitreverse() {
//	std::map<int, int> srcToDst;
//	std::vector<int> alreadysend;
//	std::vector<int> alreadyrecv;
//
//	std::multimap<float, std::pair<int, int>> distance;
//
//	for (int i = 0; i < global.getPeCount(); i++) {
//		Vec3D<float> node1Pos = global.nodes.at(i)->pos;
//
//		for (int j = 0; j < global.getPeCount(); j++) {
//			Vec3D<float> node2Pos = global.nodes.at(j)->pos;
//			float dist = fabs(node1Pos.x - node2Pos.x) + fabs(node1Pos.y - node2Pos.y) + fabs(node1Pos.z - node2Pos.z);
//
//			distance.insert(std::pair<float, std::pair<int, int>>(dist, { i, j }));
//		}
//	}
//
//	for (std::multimap<float, std::pair<int, int>>::reverse_iterator it = distance.rbegin(); it != distance.rend(); ++it) {
//		auto itsend = std::find(alreadysend.begin(), alreadysend.end(), it->second.first);
//		auto itrecv = std::find(alreadyrecv.begin(), alreadyrecv.end(), it->second.second);
//
//		if (itsend == alreadysend.end() && itrecv == alreadyrecv.end()) {
//			alreadysend.push_back(it->second.first);
//			alreadyrecv.push_back(it->second.second);
//
//			srcToDst.insert( { it->second.first, it->second.second });
//
//			//cout << it->second.first << " -> " << it->first << " -> " << it->second.second << endl;
//		};
//
//	};
//	return srcToDst;
//}

std::map<int, int> SyntheticPool::bitcomplement() {
	std::map<int, int> srcToDst;

	int nodes = processingElements.size();

	for (int i = 0; i < nodes; i++) {
		srcToDst[i] = nodes - i - 1;
	}

	return srcToDst;
}

//std::map<int, int> SyntheticPool::transpose() {
//	std::map<int, int> srcToDst;
//	for (Node* n:global.nodes) {
//		if(n->dirToCon.count(DIR::Local)){
//			Vec3D<float> newpos;
//			newpos.x=n->pos.y-0.03;
//			newpos.y=n->pos.x-0.03;
//
//
//
//			//newpos.z=1-n->pos.z-0.3;
//
//			//float linex = n->pos.y>n->pos.z?n->pos.y:n->pos.z;
//			//float diffx = linex - n->pos.x;
//			//newpos.x=n->pos.x+diffx-0.03;
//
//			//float liney = n->pos.x>n->pos.z?n->pos.x:n->pos.z;
//			//float diffy = liney - n->pos.y;
//			//newpos.y=n->pos.y+diffy-0.03;
//
//			float linez2 = n->pos.x>n->pos.y?n->pos.x:n->pos.y;
//			float linez = (n->pos.x+n->pos.y);
//			float diffz = linez - n->pos.z;
//			float diffz2 = linez/2 - n->pos.z;
//			newpos.z=diffz-0.03;
//			//newpos.z = linez;
//
//			if(newpos.x==newpos.y){
//				newpos.z=n->pos.z+diffz2-0.03;
//				newpos.x-=diffz2;
//				newpos.y-=diffz2;
//			}
//
//			if(newpos.x<0){
//				newpos.x=0;
//			}
//			if(newpos.y<0){
//				newpos.y=0;
//			}
//			if(newpos.z<0){
//				newpos.z=0;
//			}
//
//			Vec3D<float> maxpos = global.posToId.rbegin()->first;
//			if(newpos.x>maxpos.x){
//				newpos.x=maxpos.x;
//			}
//			if(newpos.y>maxpos.y){
//				newpos.y=maxpos.y;
//			}
//			if(newpos.z>maxpos.z){
//				newpos.z=maxpos.z;
//			}
//
//			newpos.x=global.posToId.lower_bound (newpos)->first.x;
//			newpos.y=global.posToId.lower_bound (newpos)->first.y;
//			int id = global.posToId.lower_bound (newpos)->second;
//			srcToDst.insert( { n->id, id });
//		}
//
//	}
//	return srcToDst;
//}

//std::map<int, int> SyntheticPool::tornado() {
//	std::map<int, int> srcToDst;
//	int xdim = global.xPositions.rbegin()->second+1;
//	int ydim = global.yPositions.rbegin()->second+1;
//	int zdim = global.zPositions.rbegin()->second+1;
//
//	for (Node* n:global.nodes) {
//		if(n->dirToCon.count(DIR::Local)){
//			Vec3D<int> pos = global.idToScPos.at(n->id);
//			Vec3D<int> newpos;
//
//			newpos.x=(pos.x+(xdim/2))%xdim;
//			newpos.y=(pos.y+(ydim/2))%ydim;
//			newpos.z=(pos.z+(zdim/2))%zdim;
//
//
//			int id = global.scPosToId.at(newpos);
//
//			Node* dst = global.nodes.at(id);
//			if(dst->dirToCon.count(DIR::Local)){
//				srcToDst[n->id]=dst->id;
//			}
//		}
//	}
//	return srcToDst;
//}

//std::map<int, int> SyntheticPool::uniform() {
//	std::map<int, int> srcToDst;
//	for (int i = 0; i < pe->size(); i++) {
//		srcToDst.insert( { i, rand() % pe->size() });
//	}
//	return srcToDst;
//}

//std::map<int, int> SyntheticPool::hotspot(int hotspot) {
//	if (hotspot == -1) {
//		hotspot = rand() % global.getPeCount();
//	}
//	LOG(true, "\t\t Hotspot: " << hotspot);
//
//	std::map<int, int> srcToDst;
//	for (int i = 0; i < global.getPeCount(); i++) {
//		srcToDst.insert( { i, hotspot });
//	}
//	return srcToDst;
//}

//void SyntheticPool::sendThread(Taskid src, Taskid dst, int initDelay, int waveCount, int waveDelay, int pkgPerWave, std::string phasename) {
//	wait(initDelay, SC_NS);
//
//	for (int i = 0; i < waveCount; i++) {
//		for (int j = 0; j < pkgPerWave; j++) {
//
//			SyntheticPacket* p = new SyntheticPacket(pe->at(src)->node->id, pe->at(dst)->node->id,1,sc_time_stamp().to_double(),phasename);
//			p->trafficTypeId = rand() % 9 + 2;
//			cout << "traffic type number " << p->trafficTypeId << endl;
//			send(p);
//		}
//		wait(waveDelay, SC_NS);
//	}
//}

//Packet* SyntheticPool::send() {
//	int id = global.typeByName.at("ProcessingElement")->nodes.at(0)->id;
//	SyntheticPacket* p = new SyntheticPacket(id, id+1,1,sc_time_stamp().to_double(),"phase");
//	p->trafficTypeId = rand() % 9 + 2;
//	cout << "traffic type number " << p->trafficTypeId << endl;
//	return p;
//}
//
//void SyntheticPool::receive(Packet* p) {
////	rep.reportEvent(p->id, "RECV", "", DB);
////	cout<<"git me"<<endl;
////	GlobalInputClass& global = GlobalInputClass::getInstance();
////	if(p->generationTime>=(double)global.syntheticPhase.at(1).minOffset*1000){ //FIXME DELETEME
////		GlobalReportClass& globalReportClass = GlobalReportClass::getInstance();
////		double latency = fabs(p->generationTime - sc_time_stamp().to_double());
////		globalReportClass.updateAverageNetworkLatencySystemLevel(latency);
////		globalReportClass.updateMaxNetworkLatencySystemLevel(latency);
////	}
//
//	delete p;
//}
