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
#include "Task.h"
#include "TaskPool.h"

#define BYTES_PER_PACKET 1

void Task::recieve(TaskPacket* p) {
	if (global.verbose_task_function_calls) {
		cout << "Task " << id << ":" << name << " recieved Data @ " << sc_time_stamp() << endl; // << p;
	}
	if (inports.count(p->srcTask) != 0) {
		inports_recieved_count[p->srcTask] = inports_recieved_count[p->srcTask] + p->size;

		if (inports_recieved_count[p->srcTask] >= inports_data_volume[p->srcTask]) {
			if (global.verbose_task_data_receive) {
				cout << "Task " << id << ":" << name << " recieved " << inports_recieved_count[p->srcTask] << " data from " << p->srcTask << " @ "
						<< sc_time_stamp() << endl;
				double latency = fabs(p->generationTime - sc_time_stamp().to_double());
				globalReportClass.updateAverageNetworkLatencySystemLevel(latency);
			}
			inports_recieved[p->srcTask] = inports_recieved[p->srcTask] + 1;
		}

		task_ev.notify(internal_task_delay, SC_NS);
	} else {
		cout << "Task: " << id << " received wrong packet from " << p->srcTask << " @ " << sc_time_stamp() << endl;
		if (global.application_is_uniform) {
			double latency = fabs(p->generationTime - sc_time_stamp().to_double());
			globalReportClass.updateAverageNetworkLatencySystemLevel(latency);
		}
	}

	delete p;
	return;
}

void Task::send_thread(int delay, Taskid addr) {

	wait(delay, SC_NS);

	//recieved some number of packets
	//TODO fix this that it can recieve more than one packet per inport
	int recieved_number_of_packets = 1;
	//calculate the bytes the packet repersent
	float recieved_bytes = (float) recieved_number_of_packets * BYTES_PER_PACKET;
	//calculate the outgowing nuber of bytes
	float bytes_to_send = calculate_distribution(recieved_bytes, this->outports_distribution[addr], outports_distribution_parameters[addr]);
	//convert in number of packets
	int packetsToSend = (int) (bytes_to_send / BYTES_PER_PACKET);

	if (global.verbose_task_data_send) {
		cout << "Task " << id << ": " << name << " send " << packetsToSend << " packets to " << addr << " @ " << sc_time_stamp() << endl;
	}

	TaskPacket* p = new TaskPacket(taskPool->Taskmapping[this->id], taskPool->Taskmapping[addr], packetsToSend, sc_time_stamp().to_double(), this->id, addr, outputPortsTrafficTypeId[addr], outputPortsAs[addr], outputPortsAc[addr]);
	send(p);
	return;
}

//Constuctors
Task::Task(sc_module_name nm) {
	this->state = TASK_IDLE;
	this->exec_count = 0;
	this->internal_task_delay = 1;
	SC_THREAD(task_thread);
	//this->task_ev.notify(SC_ZERO_TIME);
}

Task::~Task() {

}

void Task::send(TaskPacket* p) {
	taskPool->send(p);
	return;

}

void Task::register_outport(Taskid outport, float probability,
		Distribution distribution, int trafficTypeId, float as, float ac) {
	outports.insert(outport);
	outports_probabilities[outport] = probability;
	outports_distribution[outport] = distribution;
	outputPortsTrafficTypeId[outport] = trafficTypeId;
	outputPortsAs[outport] = as;
	outputPortsAc[outport] = ac;
}

void Task::register_inport(Taskid inport, int data_volume) {
	inports.insert(inport);
	inports_data_volume[inport] = data_volume;
}

void Task::set_execution(Distribution distribution, std::vector<float> params) {
	this->exec_distribution = distribution;
	this->exec_distribution_parameters = params;
}

bool Task::register_source(Distribution distribution, std::vector<float> params) {
	if (inports.size() != 0) {
		return false;
	}
	else {
		if (global.verbose_task_xml_parse) {
			cout << "registers as source" << endl;
		}
		is_source = true;
		source_distribution = distribution;
		source_distribution_parameters = params;
		task_ev.notify(1, SC_NS);
		return true;
	}
}

float Task::calculate_distribution(float x, Distribution dis, std::vector<float> params) {
	switch (dis) {
	case NORMAL:
		{
		//std::default_random_engine generator;
		//std::normal_distribution<float> distribution(params[0], params[1]);
		//return distribution(generator);
		return -1;
	}
	case POLYNOMIAL:
		{
		float returnvalue = 0;
		for (unsigned int interator = 0; interator < params.size(); interator = interator + 2)
				{
			returnvalue += params[interator] * pow(x, params[interator + 1]);
		}
		return returnvalue;
	}
	case UNIFORM:
		//srand(time(NULL));
		return (params[1] - params[0]) * ((float) (rand() % 100) / 100) + params[0];
	default:
		return -1;
	}
}

void Task::source_process() {
	if (global.verbose_task_source_execute) {
		cout << "source_process() called @ " << sc_time_stamp() << "  ";
	}
	for (auto addr : outports) {

		if (DEBUG || rand() <= outports_probabilities[addr]) {
			int delay = 0;
			delay = internal_task_delay;
			sc_spawn(sc_bind(&Task::send_thread, this, delay, addr));
		}
	}
	this->exec_count++;
	if (this->exec_count == exec_count_max) {
		// goto FREE state (only defined in non-source tasks.)
		is_source = false;
		state = TASK_FREE;
	}

	//scheduling
	//in case of Polynomial: enable only constant delay:
	int x = 0;
	float delay_to_next_output = calculate_distribution((float) x, source_distribution, source_distribution_parameters);
	int delay_floored = (int) (delay_to_next_output);
	if (global.verbose_task_source_execute) {
		cout << "source_process: Delay " << delay_floored << endl;
	}

	this->task_ev.notify(delay_floored, SC_NS);
}

void Task::task_thread(void) {
	while (state != TASK_FREE) {
		wait(this->task_ev);

		//cout << "Task " << id << ": " << name << " state" << state << endl;

		//for source tasks
		if (this->is_source) {
			source_process();
		} else {

			//state-machine in infinite loop, exit via "FREE" into destructor:
			switch (this->state) {
			case TASK_IDLE:
				if (and_trigger && (inports.size() == inports_recieved.size())) {
					//clear inports and switch state
					//inports_recieved.clear();*1
					//decrement all inports_recieved -1
					//decrement inports_recieved_count um die ben�tigeten daten je ausf�hrung (inports_data_volume)
					for (auto iterator : inports)
					//for each (int iterator in inports)
					{
						inports_recieved[iterator]--;
						inports_recieved_count[iterator] = inports_recieved_count[iterator] - inports_data_volume[iterator];
					}

					state = TASK_READY;
					this->task_ev.notify(1, SC_NS);
				} else if ((!and_trigger) && (inports_recieved.size() >= 1)) {
					//clear inports and switch state
					//inports_recieved.clear();*1
					for (auto iterator : inports_recieved)
					//for each (std::pair<TaskID, int> iterator in inports_recieved){
					{
						inports_recieved[iterator.first]--;
						inports_recieved_count[iterator.first] = inports_recieved_count[iterator.first] - inports_data_volume[iterator.first];
					}

					state = TASK_READY;
					this->task_ev.notify(1, SC_NS);
				}
				break;
			case TASK_READY:
				//currently no scheduler is present
				state = TASK_RUN;
				this->task_ev.notify(1, SC_NS);
				break;
			case TASK_RUN:
				for (auto addr : outports)
				//for each (int addr in outports)
				{

					if (DEBUG || rand() <= outports_probabilities[addr]) {
						int delay = 0;
						sc_spawn(sc_bind(&Task::send_thread, this, delay, addr));
					}
				}
				this->exec_count++;
				if (this->exec_count == exec_count_max) {
					state = TASK_FREE;
					this->task_ev.notify(1, SC_NS);
				}
				else {
					state = TASK_IDLE;
				}
				break;
				case TASK_FREE:
				//no further events, final state
								break;
							}
						}
					}
				}
