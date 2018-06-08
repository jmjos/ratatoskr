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

#include "ProcessingElementVC.h"

ProcessingElementVC::ProcessingElementVC(sc_module_name nm, Node *node,
		TrafficPool *tp) :
		ProcessingElement(nm, node, tp) {
	this->packetPortContainer = new PacketPortContainer(
			("NI_PACKET_CONTAINER" + std::to_string(id)).c_str());
}

ProcessingElementVC::~ProcessingElementVC() {
}

void ProcessingElementVC::initialize() {

	packetPortContainer->portValidOut.write(false);
	packetPortContainer->portFlowControlOut.write(true);

	// sc_spawn(sc_bind(&SyntheticPool::sendThread, this, con.first,
	// con.second,initDelay,sp.waveCount,sp.waveDelay,sp.pkgPerWave, sp.name));

	SC_THREAD(thread);

	SC_METHOD(receive);
	sensitive << packetPortContainer->portValidIn.pos();
}

void ProcessingElementVC::thread() {
	// int lastTimeStamp = 0;

	for (;;) {
		int timeStamp = sc_time_stamp().value() / 1000;

		std::vector<DataDestination *> removeList;

		for (auto const &tw : destWait) {
			DataDestination *dest = tw.first;
			Task *task = destToTask.at(dest);
			if (taskTerminationTime.count(task)
					&& taskTerminationTime.at(task) < timeStamp) {
				removeList.push_back(dest);
			}
		}

		for (DataDestination *dest : removeList) {
			Task *task = destToTask.at(dest);
			destToTask.erase(dest);
			taskToDest.erase(task);
			taskRepeatLeft.erase(task);
			taskStartTime.erase(task);
			taskTerminationTime.erase(task);
			countLeft.erase(dest);
			destWait.erase(dest);
		}

		for (auto const &tw : destWait) {
			DataDestination *dest = tw.first;

			if (destWait.at(dest) <= timeStamp) {
				Packet *p = new Packet(this->node, dest->destination, 1,
						sc_time_stamp().to_double(), dest->type->id, 0, 0);
				p->dataType = dest->type;

				packetPortContainer->portValidOut = true;
				packetPortContainer->portDataOut = p;

				countLeft.at(dest)--;

				if
(				!countLeft.at(dest)) {
					countLeft.erase(dest);
					destWait.erase(dest);

					Task *task = destToTask.at(dest);
					destToTask.erase(dest);
					taskToDest.at(task).erase(dest);

					if (taskToDest.at(task).empty()) {
						taskToDest.erase(task);
						execute(task);
					}

				} else {
					destWait.at(dest) =
					global.getRandomIntBetween(dest->minInterval, dest->maxInterval) +
					timeStamp;
				}
				break;
			}
		}

		wait(SC_ZERO_TIME);
		packetPortContainer->portValidOut = false;

		int nextCall = -1;
		for (auto const &tw : destWait) {
			if (nextCall > tw.second || nextCall == -1) {
				/* we want to apply uniform_batch_mode experiment, that means all tasks need to send data once in one interval
				 with some random offset in each interval.
				 */

				/* TODO:
				 * Attention: we are alwas taking the the minStart and minInterval to calculate the nextCall.
				 * In the future, we may add randomness to the process, by selecting a number between minStart and maxStart,
				 * and the same thing for minInterval and maxInterval.
				 */
				Task* task = destToTask.at(tw.first);
				SyntheticPhase* sp = task->currentSP;
				if (timeStamp < sp->minStart) {
					nextCall =
							sp->minStart
									+ global.getRandomIntBetween(0,
											sp->minInterval
													- (2
															* this->node->type->clockSpeed));

				} else {
					if (timeStamp < sp->minStart + sp->minInterval)
						nextCall =
								sp->minStart + sp->minInterval
										+ global.getRandomIntBetween(0,
												sp->minInterval
														- (2
																* this->node->type->clockSpeed));
					else {
						int numIntervalsPassed = (timeStamp - sp->minStart)
								/ sp->minInterval;
						int intervalBeginning = sp->minStart
								+ (numIntervalsPassed * sp->minInterval);
						nextCall =
								intervalBeginning + sp->minInterval
										+ global.getRandomIntBetween(0,
												sp->minInterval
														- (2
																* this->node->type->clockSpeed));

					}
				}
			}
		}
		if (nextCall == 0) { // limit packet rate
			nextCall = 1;
		}

		if (nextCall != -1) {
			event.notify(nextCall - timeStamp, SC_NS);
		}

		// lastTimeStamp = timeStamp;
		wait(event);
	}
}

void ProcessingElementVC::execute(Task *task) {
	if (!taskRepeatLeft.count(task)) {
		taskRepeatLeft[task] = global.getRandomIntBetween(task->minRepeat,
				task->maxRepeat);
	} else {
		if (taskRepeatLeft.at(task) > 0) {
			taskRepeatLeft.at(task)--;}

		if (!taskRepeatLeft.at(task)) {
			taskRepeatLeft.erase(task);
			return;
		}
	}

	if (!taskStartTime.count(task)) {
		taskStartTime[task] = global.getRandomIntBetween(task->minStart,
				task->maxStart);
	}

	if (!taskTerminationTime.count(task) && task->minDuration != -1) {
		taskTerminationTime[task] = taskStartTime[task]
				+ global.getRandomIntBetween(task->minDuration,
						task->maxDuration);
	}

	if (task->requirements.empty()) {
		startSending(task);
	} else {
		for (DataRequirement *r : task->requirements) {
			neededFor[r->type].insert(task);
			neededAmount[std::make_pair(task, r->type)] =
					global.getRandomIntBetween(r->minCount, r->maxCount);
			needs[task].insert(r->type);
		}
	}
}

void ProcessingElementVC::bind(Connection *con, SignalContainer *sigContIn,
		SignalContainer *sigContOut) {
	packetPortContainer->bind(sigContIn, sigContOut);
}

void ProcessingElementVC::receive() {
	LOG(global.verbose_pe_function_calls,
			"PE" << node->idType << "(Node" << node->id << ")\t- receive_data_process()");

	if (packetPortContainer->portValidIn.posedge()) {
		Packet *received_packet = packetPortContainer->portDataIn.read();
		if (received_packet) {
			DataType *type = received_packet->dataType;

			if (receivedData.count(type)) {
				receivedData.at(type)++;}
			else {
				receivedData[type] = 1;
			}

			checkNeed();
		}
	}
}

void ProcessingElementVC::startSending(Task *task) {
	float rn = global.getRandomFloatBetween(0, 1);

	for (unsigned int i = 0; i < task->possibilities.size(); i++) {
		if (task->possibilities.at(i).first > rn) {
			std::vector<DataDestination *> *destVec = &(task->possibilities.at(
					i).second);

			for (DataDestination *dest : *destVec) {
				destToTask[dest] = task;
				taskToDest[task].insert(dest);

				countLeft[dest] = global.getRandomIntBetween(dest->minCount,
						dest->maxCount);

				int delayTime = (sc_time_stamp().value() / 1000)
						+ global.getRandomIntBetween(dest->minDelay,
								dest->maxDelay);

				if (taskStartTime.count(task)
						&& taskStartTime.at(task) > delayTime) {
					destWait[dest] = taskStartTime.at(task);
				} else {
					destWait[dest] = delayTime;
				}
				event.notify(SC_ZERO_TIME);
			}
			break;
		} else {
			rn -= task->possibilities.at(i).first;
		}
	}
}

void ProcessingElementVC::checkNeed() {
	for (auto const &data : receivedData) {
		DataType *type = data.first;
		std::vector<std::pair<Task *, DataType *>> removeList;

		if (neededFor.count(type)) {
			for (Task *t : neededFor.at(type)) {
				std::pair<Task *, DataType *> pair = std::make_pair(t, type);
				neededAmount.at(pair) -= receivedData.at(type);
				/* This line was commented out because if a task requires several packets from several data types,
				 it says that the task is finished receiving the required packets while in fact, it still needs some packets.
				 receivedData.at(type) = 0;
				 */
				if (neededAmount.at(pair) <= 0) {
					removeList.push_back(pair);
					// This line is also commented out for the same reason mentioned above.
					// receivedData.at(type) = -neededAmount.at(pair);
				}
			}
		}

		for (std::pair<Task *, DataType *> p : removeList) {
			neededFor.erase(p.second);
			neededAmount.erase(p);
			needs.at(p.first).erase(p.second);

			if (needs.at(p.first).empty()) {
				startSending(p.first);
			}
		}
	}
}
