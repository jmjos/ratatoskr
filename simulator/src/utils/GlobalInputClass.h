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
#pragma once

#include <string>
#include <set>
#include <boost/tokenizer.hpp>
#include <pugixml.hpp>
#include <map>
#include <unistd.h>
#include <random>

#include "Structures.h"


class GlobalInputClass {
	GlobalInputClass() {
		rd = new std::random_device();
		rand = new std::mt19937_64((*rd)());
	};

public:

	std::map<Vec3D<float>, std::set<Node*>> posToId;
	std::vector<float> xPositions;
	std::vector<float> yPositions;
	std::vector<float> zPositions;

	std::map<std::string, NodeType*> typeByName;
	std::vector<NodeType*> nodeTypes;
	std::vector<LayerType*> layerTypes;
	std::vector<Node*> nodes;
	std::vector<Connection*> connections;
	std::vector<Task*> tasks;
	std::vector<DataType*> dataTypes;

	int droppedCounter = 0;

	std::random_device* rd;
	std::mt19937_64* rand;


	//General
	int simulation_time = 0;
	bool outputToFile = false;
	std::string outputFileName = "";

	//NOC
	std::string noc_file;
	int flitsPerPacket = 0;
	float Vdd= 1.0f;

	//Application
	std::string benchmark;
	std::string data_file;
	std::string map_file;
	std::string application_file;
	std::string application_mapping_file;
	std::string netraceFile;
	int netraceStartRegion = 0;
	bool application_is_uniform = false;
	int application_numberOfTrafficTypes;

	std::vector<SyntheticPhase*> syntheticPhase;

	/// VERBOSE ///
	//processing elements
	bool verbose_pe_function_calls = false;
	bool verbose_pe_send_flit = false;
	bool verbose_pe_send_head_flit = false;
	bool verbose_pe_receive_flit = false;
	bool verbose_pe_receive_tail_flit = false;
	bool verbose_pe_throttle = false;
	bool verbose_pe_reset = false;

	//router
	bool verbose_router_function_calls = false;
	bool verbose_router_send_flit = false;
	bool verbose_router_send_head_flit = false;
	bool verbose_router_receive_flit = false;
	bool verbose_router_receive_head_flit = false;
	bool verbose_router_assign_channel = false;
	bool verbose_router_throttle = false;
	bool verbose_router_buffer_overflow = false;
	bool verbose_router_reset = false;

	//netrace
	bool verbose_netrace_inject = false;
	bool verbose_netrace_eject = false;
	bool verbose_netrace_router_receive = false;

	//tasks
	bool verbose_task_function_calls = false;
	bool verbose_task_xml_parse = false;
	bool verbose_task_data_receive = true;
	bool verbose_task_data_send = true;
	bool verbose_task_source_execute = true;


	bool readInputFile(std::string filePath);
	bool readNoCLayout(std::string filePath);
	bool readDataStream(std::string taskFilePath, std::string mappingFilePath);

	static GlobalInputClass& getInstance() {
		static GlobalInputClass instance;
		return instance;
	}

	int getRandomIntBetween(int, int);
	float getRandomFloatBetween(float, float);

	void readAttributeIfExists(pugi::xml_node, const char*, const char*, int&);
	void readAttributeIfExists(pugi::xml_node, const char*, int&);

	std::string readRequiredStringAttribute(pugi::xml_node, const char*, const char*);
	std::string readRequiredStringAttribute(pugi::xml_node, const char*);

	int readRequiredIntAttribute(pugi::xml_node, const char*, const char*);
	int readRequiredIntAttribute(pugi::xml_node, const char*);

	float readRequiredFloatAttribute(pugi::xml_node, const char*, const char*);
	float readRequiredFloatAttribute(pugi::xml_node, const char*);
};
