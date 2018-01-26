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
#include "TaskPool.h"
#include "../../model/processingElements/ProcessingElementVC3D.h"

TaskPool::TaskPool(sc_module_name mn, std::vector<ProcessingElementVC3D*>* pe, std::string taskFile, std::string mappingFile) : TrafficPool(mn, pe) {
	cout << endl;
	cout << "The following application is running" << endl;
	cout << "            " << global.application_file << endl;
	cout << "with mapping:" << endl << "            ";
	cout << global.application_mapping_file << endl << endl;

	task.resize(100);

	pugi::xml_document doc;
	pugi::xml_parse_result result = doc.load_file(mappingFile.c_str());
	if (global.verbose_task_xml_parse) {
		cout << result.description() << endl;
	}

	for (pugi::xml_node resource_node = doc.child("mapping").first_child(); resource_node; resource_node = resource_node.next_sibling()) {
		int peID = resource_node.attribute("id").as_int();
		std::vector<int> TaskVec;
		for (pugi::xml_node task_node = resource_node.first_child(); task_node; task_node = task_node.next_sibling()) {
			Taskmapping.insert({task_node.attribute("id").as_int(), peID});
			TaskVec.push_back(task_node.attribute("id").as_int());
		}
		PEmapping.insert({peID, TaskVec});
	}

	result = doc.load_file(taskFile.c_str());
	if (global.verbose_task_xml_parse) {
		cout << result.description() << endl;
	}

	int current_task_index = 0;
	for (pugi::xml_node task_node = doc.child("application").first_child(); task_node; task_node = task_node.next_sibling()) {
		std::string name = task_node.attribute("name").as_string();
		const char* cstr_2 = name.c_str();

		//Construct Task
		task[current_task_index] = new Task(cstr_2);

		//set basic parameters
		task[current_task_index]->name = name;
		task[current_task_index]->id = task_node.attribute("id").as_int();
		task[current_task_index]->exec_count_max = task_node.attribute("execution_count").as_int();
		task[current_task_index]->and_trigger = task_node.attribute("trigger_and").as_bool();

		task[current_task_index]->taskPool= this;

		task[current_task_index]->dbid = rep.registerElement("Task", task[current_task_index]->id);

		//configure inports of task
		for (pugi::xml_node inport_node = task_node.child("inport"); inport_node; inport_node = inport_node.next_sibling("inport")) {
			int inport = inport_node.attribute("id").as_int();
			int data_volume = inport_node.attribute("data_volume").as_int();
			task[current_task_index]->register_inport(inport, data_volume);
		}
		if (!task_node.child("inport")) {
			//this task is a source
			Distribution dis;
			std::vector<float> parameters_vector;
			std::string distribution_str = task_node.child("parameters").child("source").child_value("distribution");
			if (distribution_str == "POLYNOMIAL") {
				dis = POLYNOMIAL;
				std::string coeffs = task_node.child("parameters").child("source").child("parameters").child_value("coefficient");
				std::string exponents = task_node.child("parameters").child("source").child("parameters").child_value("exponent");

				//split strings from XML:
				std::istringstream coeffs_stream(coeffs);
				std::vector<std::string> coeffs_vec { std::istream_iterator<std::string> { coeffs_stream }, std::istream_iterator<std::string> { } };
				std::istringstream exp_stream(exponents);
				std::vector<std::string> exp_vec { std::istream_iterator<std::string> { exp_stream }, std::istream_iterator<std::string> { } };

				for (unsigned int vector_iterator = 0; vector_iterator < coeffs_vec.size(); vector_iterator++) {
					parameters_vector.push_back((float) std::stoi(coeffs_vec[vector_iterator]));
					parameters_vector.push_back((float) std::stoi(exp_vec[vector_iterator]));
				}

			} else if (distribution_str == "NORMAL") {
				dis = NORMAL;
				float mean = task_node.child("parameters").child("source").child("parameters").attribute("mean").as_float();
				float std_deviation = task_node.child("parameters").child("source").child("parameters").attribute("std_deviation").as_float();
				parameters_vector.push_back(mean);
				parameters_vector.push_back(std_deviation);
			} else if (distribution_str == "UNIFORM") {
				dis = UNIFORM;
				int min = task_node.child("parameters").child("source").child("parameters").attribute("minimum").as_int();
				int max = task_node.child("parameters").child("source").child("parameters").attribute("maximum").as_int();
				parameters_vector.push_back((float) min);
				parameters_vector.push_back((float) max);
			}

			if (!task[current_task_index]->register_source(dis, parameters_vector)) {
				cout << "Task is already normal, cannot register als source" << endl;
			}

		}

		//configure outports
		for (pugi::xml_node outport_node = task_node.child("outport"); outport_node; outport_node = outport_node.next_sibling("outport")) {
			int outport = outport_node.attribute("id").as_int();
			Distribution dis;
			std::string distribution_str = outport_node.child_value("distribution");
			if (distribution_str == "POLYNOMIAL") {
				dis = POLYNOMIAL;
				std::string coeffs = outport_node.child("parameters").child_value("coefficient");
				std::string exponents = outport_node.child("parameters").child_value("exponent");

				//split strings from XML:
				std::istringstream coeffs_stream(coeffs);
				std::vector<std::string> coeffs_vec { std::istream_iterator<std::string> { coeffs_stream }, std::istream_iterator<std::string> { } };
				std::istringstream exp_stream(exponents);
				std::vector<std::string> exp_vec { std::istream_iterator<std::string> { exp_stream }, std::istream_iterator<std::string> { } };
				for (unsigned int vector_iterator = 0; vector_iterator < coeffs_vec.size(); vector_iterator++) {
					task[current_task_index]->outports_distribution_parameters[outport].push_back((float) std::stoi(coeffs_vec[vector_iterator]));
					task[current_task_index]->outports_distribution_parameters[outport].push_back((float) std::stoi(exp_vec[vector_iterator]));
				}

			} else if (distribution_str == "NORMAL") {
				dis = NORMAL;
				float mean = outport_node.child("parameters").attribute("mean").as_float();
				float std_deviation = outport_node.child("parameters").attribute("std_deviation").as_float();
				task[current_task_index]->outports_distribution_parameters[outport].push_back(mean);
				task[current_task_index]->outports_distribution_parameters[outport].push_back(std_deviation);
			} else if (distribution_str == "UNIFORM") {
				dis = UNIFORM;
				int min = outport_node.child("parameters").attribute("minimum").as_int();
				int max = outport_node.child("parameters").attribute("maximum").as_int();
				task[current_task_index]->outports_distribution_parameters[outport].push_back((float) min);
				task[current_task_index]->outports_distribution_parameters[outport].push_back((float) max);
			}
			float probability = outport_node.attribute("probability").as_float();
			int trafficTypeId = outport_node.attribute("trafficType").as_int();
			float as = outport_node.attribute("as").as_float();
			float ac = outport_node.attribute("ac").as_float();
			task[current_task_index]->register_outport(outport, probability, dis, trafficTypeId, as, ac);
		}

		//configure further parameters
		for (pugi::xml_node parameters_node = task_node.child("parameters"); parameters_node; parameters_node = parameters_node.next_sibling("parameters")) {
			int delay = parameters_node.attribute("delay").as_int();
			task[current_task_index]->internal_task_delay = delay;
		}

		current_task_index++;
	}

	number_of_tasks = current_task_index;


}

TaskPool::~TaskPool() {
}

void TaskPool::receive(Packet *p) {
	TaskPacket *tp = (TaskPacket*)p;
	task[tp->dstTask]->recieve(tp);
}

void TaskPool::send(Packet *p) {
	pe->at(p->src)->receive(p);
}

