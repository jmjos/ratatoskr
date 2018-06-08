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
	std::vector<Task*> tasks;
	int phaseId = 0;

	for (SyntheticPhase* sp : global.syntheticPhase) {
		LOG(true,
				"SyntheticPool\t Initialize phase \"" << sp->name <<"\" with \"" << sp->distribution << "\" distribution");

		std::map<int, int> srcToDst;

		if (sp->distribution == "uniform") {
			DataType* datatype = new DataType(datatypeid,
					std::to_string(datatypeid));

			datatypeid++;

			std::vector<std::pair<float, std::vector<DataDestination*>>> possibilities;
			for (unsigned int i = 0; i < processingElements.size(); i++) {
				for (unsigned int j = 0; j < processingElements.size(); j++) {
					std::vector<DataDestination*> dests;
					DataDestination* dest = new DataDestination(datadestid,
							datatype, processingElements.at(j)->node,
							sp->minInterval, sp->maxInterval);
					dest->minCount = sp->minCount;
					dest->maxCount = sp->maxCount;
					dest->minDelay = sp->minDelay;
					dest->maxDelay = sp->maxDelay;
					dests.push_back(dest);

					datadestid++;
					possibilities.push_back(
							std::make_pair(1.f / processingElements.size(),
									dests));
				}

				Task* task = new Task(taskid, processingElements.at(i)->node);
				task->minStart = sp->minStart;
				task->maxStart = sp->maxStart;
				task->minDuration = sp->minDuration;
				task->maxDuration = sp->maxDuration;
				if (sp->minRepeat == -1 && sp->maxRepeat == -1) {
					task->minRepeat = std::ceil(
							(float) sp->minDuration / (float) sp->minInterval);
					task->maxRepeat = std::ceil(
							(float) sp->maxDuration / (float) sp->maxInterval);
				} else {
					task->minRepeat = sp->minRepeat;
					task->maxRepeat = sp->maxRepeat;
				}

				task->possibilities = possibilities;
				task->currentSP = sp;
				tasks.push_back(task);
				// processingElements.at(i)->execute(task);
				taskid++;

			}
			shuffle_execute_tasks(tasks, phaseId);
			phaseId++;
		} else {
			if (sp->distribution == "bitcomplement") {
				srcToDst = bitcomplement();
			} else if (sp->distribution == "tornado") {
				srcToDst = tornado();
			} else if (sp->distribution == "transpose") {
				srcToDst = transpose();
			} else if (sp->distribution == "hotspot") {
				srcToDst = hotspot(sp->hotspot);
			} else {
				FATAL("Distribution is not implemented");
			}
		}

		for (std::pair<const int, int> con : srcToDst) {
			DataType* datatype = new DataType(datatypeid,
					std::to_string(datatypeid));
			datatypeid++;

			std::vector<DataDestination*> dests;
			DataDestination* dest = new DataDestination(datadestid, datatype,
					processingElements.at(con.second)->node, sp->minInterval,
					sp->maxInterval);
			dest->minCount = sp->minCount;
			dest->maxCount = sp->maxCount;
			dest->minDelay = sp->minDelay;
			dest->maxDelay = sp->maxDelay;
			dests.push_back(dest);
			datadestid++;

			std::vector<std::pair<float, std::vector<DataDestination*>>> possibilities;
			possibilities.push_back(std::make_pair(1, dests));

			Task* task = new Task(taskid,
					processingElements.at(con.first)->node);
			task->minStart = sp->minStart;
			task->maxStart = sp->maxStart;
			task->minDuration = sp->minDuration;
			task->maxDuration = sp->maxDuration;
			task->minRepeat = sp->minRepeat;
			task->maxRepeat = sp->maxRepeat;
			task->possibilities = possibilities;

			processingElements.at(con.first)->execute(task);
			taskid++;
		}
	}
}

void SyntheticPool::clear(Task*) {

}

std::map<int, int> SyntheticPool::bitcomplement() {
	std::map<int, int> srcToDst;

	int nodes = processingElements.size();

	for (int i = 0; i < nodes; i++) {
		srcToDst[i] = nodes - i - 1;
	}

	return srcToDst;
}

std::map<int, int> SyntheticPool::transpose() {
	std::map<int, int> srcToDst;
	int nodes = processingElements.size();

	for (int i = 0; i < nodes; i++) {
		srcToDst[i] = (i << int(ceil(log2(nodes) / 2))) % (nodes - 1);
	}

	return srcToDst;
}

std::map<int, int> SyntheticPool::tornado() {
	std::map<int, int> srcToDst;

	std::vector<float>* xPos = &global.xPositions;
	std::vector<float>* yPos = &global.yPositions;
	std::vector<float>* zPos = &global.zPositions;

	int nodes = processingElements.size();
	for (int i = 0; i < nodes; i++) {
		Vec3D<float> srcPos = processingElements.at(i)->node->pos;
		Vec3D<float> dstPos;
		dstPos.x = xPos->at(
				((std::find(xPos->begin(), xPos->end(), srcPos.x)
						- xPos->begin()) + (xPos->size() >> 1)) % xPos->size());
		dstPos.y = yPos->at(
				((std::find(yPos->begin(), yPos->end(), srcPos.y)
						- yPos->begin()) + (yPos->size() >> 1)) % yPos->size());
		dstPos.z = zPos->at(
				((std::find(zPos->begin(), zPos->end(), srcPos.z)
						- zPos->begin()) + (zPos->size() >> 1)) % zPos->size());

		Node* dstNode = 0;
		for (Node* node : global.posToId.at(dstPos)) {
			if (node->type->routerModel == "ProcessingElement") {
				dstNode = node;
			}
		}
		if (dstNode) {
			srcToDst[i] = dstNode->idType;
		}

	}

	return srcToDst;
}

std::map<int, int> SyntheticPool::hotspot(int hotspot) {
	int nodes = processingElements.size();

	if (hotspot == -1) {
		hotspot = global.getRandomIntBetween(0, nodes - 1);
	}

	LOG(true, "\t\t Hotspot: " << hotspot);

	std::map<int, int> srcToDst;
	for (int i = 0; i < nodes; i++) {
		srcToDst[i] = hotspot;
	}
	return srcToDst;
}

void SyntheticPool::shuffle_execute_tasks(std::vector<Task*> &tasks,
		int phaseId) {
	// Execute the tasks in random order
	int begin = phaseId * processingElements.size();
	std::random_shuffle(tasks.begin() + begin, tasks.end());
	for (unsigned int k = 0; k < processingElements.size(); k++) {
		processingElements.at(k)->execute(
				tasks.at(k + phaseId * processingElements.size()));
	}
}
