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
#ifndef _TASK_H_
#define _TASK_H_

#include "systemc.h"
#include <set>
#include <vector>
#include <map>
#include <math.h>
#include <random>
#include "../../utils/GlobalInputClass.h"
#include "../../utils/GlobalReportClass.h"
#include "../../utils/Report.h"
#include "TaskPacket.h"

#define TASK_IDLE 0
#define TASK_READY 2
#define TASK_RUN 3
#define TASK_FREE 4

class TaskPool;

#define DEBUG 1

typedef int Taskid;
enum Distribution {
	POLYNOMIAL, UNIFORM, NORMAL
};

/*
 *
 *
 *				Task
 *		/------------------\
*	 -> | inports          |->
 *      |                  |
 *   -> |         outports |->
 *      \------------------/
 *
 *    Task -> Task ->Task --> Task
 *                `->Task
 *
 */

class Task: public sc_module {

private:
	GlobalInputClass& global = GlobalInputClass::getInstance();
	GlobalReportClass& globalReportClass = GlobalReportClass::getInstance();

	Report& rep = Report::getInstance();
public:
	//--------- MEMBERS ---------//
	int dbid;
	int id = -1;
	std::string name;

	TaskPool* taskPool;

	//---------- PORTS -----------//

	//ids of Tasks at inports of Taks
	std::set<Taskid> inports;
	//TODO inports_rec und inports_rec_count zusammenfassen
	//ids of Tasks, from which data where recieved.
	std::map<Taskid, int> inports_recieved;

	//number of packeges recieved per inport
	std::map<Taskid, int> inports_recieved_count;
	//amount of data for each input port that are needed to execute once
	std::map<Taskid, int> inports_data_volume;

	//outports of Task
	std::set<Taskid> outports;
	// Probability, that data are send.
	std::map<Taskid, float> outports_probabilities;

	std::map<Taskid, Distribution> outports_distribution;
	//the first value contains min, mean or coefficient depending on the selected distribution
	//the second value contains max, std_deviation or exponent depending on the selected distribution
	//the alternating storage of the values ensures fast access times.
	std::map<Taskid, std::vector<float> > outports_distribution_parameters;
	std::map<Taskid, int> outputPortsTrafficTypeId;
	std::map<Taskid, float> outputPortsAs;
	std::map<Taskid, float> outputPortsAc;

	//internal state
	int state = -1;

	//source parameters:
	bool is_source = false;
	Distribution source_distribution = POLYNOMIAL;
	std::vector<float> source_distribution_parameters;

	//mapping
	int associated_pe = -1;

	//ExecCount variables
	Distribution exec_distribution;
	//here the same applies as above.

	//global execution parameters. 
	std::vector<float> exec_distribution_parameters;
	int exec_count = -1;
	int exec_count_max = -1;

	bool and_trigger = true;

	int internal_task_delay;

	//events
	sc_event task_ev;

	//threads
	void task_thread(void);
	void recieve(TaskPacket* p);
	void send_thread(int delay, Taskid addr);
	void send(TaskPacket* p);
	void dummy(bool value);
	void source_process(void);
	float calculate_distribution(float x, Distribution dis,
			std::vector<float> params);
	void register_outport(Taskid outport, float probability,
			Distribution distribution, int trafficTypeId, float as, float ac);
	void register_inport(Taskid inport, int data_volume);
	void set_execution(Distribution distribution, std::vector<float> params);
	bool register_source(Distribution distribution, std::vector<float> params);SC_HAS_PROCESS(Task);

	//Constuctors
	Task(sc_module_name nm);
	~Task();

};

#endif
