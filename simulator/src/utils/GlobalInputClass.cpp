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
#include "GlobalInputClass.h"

bool GlobalInputClass::readInputFile(std::string filePath) {
	pugi::xml_document doc;

	std::cout << "Reading simulator config: " << filePath << endl;
	pugi::xml_parse_result result = doc.load_file(filePath.c_str());
	assert(result && "Failed to read simulator file!");

	//GENERAL
	pugi::xml_node gen_node = doc.child("configuration").child("general");
	simulation_time =
			gen_node.child("simulationTime").attribute("value").as_int();
	outputToFile = gen_node.child("outputToFile").attribute("value").as_bool();
	outputFileName = gen_node.child("outputToFile").child_value();

	//NOC
	pugi::xml_node noc_node = doc.child("configuration").child("noc");
	noc_file = noc_node.child_value("nocFile");
	flitsPerPacket =
			noc_node.child("flitsPerPacket").attribute("value").as_int();
	routingVerticalThreshold =
			noc_node.child("routingVerticalThreshold").attribute("value").as_float();
	Vdd = noc_node.child("Vdd").attribute("value").as_float();

	//APPLICATION
	pugi::xml_node app_node = doc.child("configuration").child("application");
	benchmark = app_node.child_value("benchmark");
	data_file = app_node.child_value("dataFile");
	map_file = app_node.child_value("mapFile");
	application_file = app_node.child_value("simulationFile");
	application_mapping_file = app_node.child_value("mappingFile");
	netraceFile = app_node.child_value("netraceFile");
	netraceStartRegion =
			app_node.child("netraceStartRegion").attribute("value").as_int();
	application_is_uniform =
			app_node.child("isUniform").attribute("value").as_bool();
	application_numberOfTrafficTypes =
			app_node.child("numberOfTrafficTypes").attribute("value").as_int();

	for (pugi::xml_node phase_node : app_node.child("synthetic").children(
			"phase")) {
		std::string name = readRequiredStringAttribute(phase_node, "name");
		std::string distribution = readRequiredStringAttribute(phase_node,
				"distribution", "value");
		int minInterval = readRequiredIntAttribute(phase_node, "interval",
				"min");
		int maxInterval = readRequiredIntAttribute(phase_node, "interval",
				"max");
		SyntheticPhase* sp = new SyntheticPhase(name, distribution, minInterval,
				maxInterval);

		readAttributeIfExists(phase_node, "start", "min", sp->minStart);
		readAttributeIfExists(phase_node, "start", "max", sp->maxStart);
		readAttributeIfExists(phase_node, "duration", "min", sp->minDuration);
		readAttributeIfExists(phase_node, "duration", "max", sp->maxDuration);
		readAttributeIfExists(phase_node, "repeat", "min", sp->minRepeat);
		readAttributeIfExists(phase_node, "repeat", "max", sp->maxRepeat);
		readAttributeIfExists(phase_node, "count", "min", sp->minCount);
		readAttributeIfExists(phase_node, "count", "max", sp->maxCount);
		readAttributeIfExists(phase_node, "delay", "min", sp->minDelay);
		readAttributeIfExists(phase_node, "delay", "max", sp->maxDelay);
		readAttributeIfExists(phase_node, "hotspot", "value", sp->hotspot);

		//set first start time after warmup to start of measurement
		if (benchmark.compare("synthetic") == 0 && name.compare("warmup") != 0
				&& synthetic_start_measurement_time == -1) {
			synthetic_start_measurement_time = sp->minStart;
		}

		syntheticPhase.push_back(sp);
	}

	//---------------------------------------------------------------------------

	//VERBOSE
	pugi::xml_node verbose_node;

	//	PE Verbosity
	verbose_node = doc.child("configuration").child("verbose").child(
			"processingElements");
	verbose_pe_function_calls = verbose_node.child("function_calls").attribute(
			"value").as_bool();
	verbose_pe_send_flit =
			verbose_node.child("send_flit").attribute("value").as_bool();
	verbose_pe_send_head_flit = verbose_node.child("send_head_flit").attribute(
			"value").as_bool();
	verbose_pe_receive_flit = verbose_node.child("receive_flit").attribute(
			"value").as_bool();
	verbose_pe_receive_tail_flit =
			verbose_node.child("receive_tail_flit").attribute("value").as_bool();
	verbose_pe_throttle =
			verbose_node.child("throttle").attribute("value").as_bool();
	verbose_pe_reset = verbose_node.child("reset").attribute("value").as_bool();

	//	Router Verbosity
	verbose_node = doc.child("configuration").child("verbose").child("router");
	verbose_router_function_calls =
			verbose_node.child("function_calls").attribute("value").as_bool();
	verbose_router_send_flit = verbose_node.child("send_flit").attribute(
			"value").as_bool();
	verbose_router_send_head_flit =
			verbose_node.child("send_head_flit").attribute("value").as_bool();
	verbose_router_receive_flit = verbose_node.child("receive_flit").attribute(
			"value").as_bool();
	verbose_router_receive_head_flit =
			verbose_node.child("receive_head_flit").attribute("value").as_bool();
	verbose_router_assign_channel =
			verbose_node.child("assign_channel").attribute("value").as_bool();
	verbose_router_throttle =
			verbose_node.child("throttle").attribute("value").as_bool();
	verbose_router_buffer_overflow =
			verbose_node.child("buffer_overflow").attribute("value").as_bool();
	verbose_router_reset =
			verbose_node.child("reset").attribute("value").as_bool();

	//	Netrace Verbosity
	verbose_node = doc.child("configuration").child("verbose").child("netrace");
	verbose_netrace_inject =
			verbose_node.child("inject").attribute("value").as_bool();
	verbose_netrace_eject =
			verbose_node.child("eject").attribute("value").as_bool();
	verbose_netrace_router_receive =
			verbose_node.child("router_receive").attribute("value").as_bool();

	//	Task Verbosity
	verbose_node = doc.child("configuration").child("verbose").child("tasks");
	verbose_task_xml_parse =
			verbose_node.child("xml_parse").attribute("value").as_bool();
	verbose_task_data_receive = verbose_node.child("data_receive").attribute(
			"value").as_bool();
	verbose_task_data_send =
			verbose_node.child("data_send").attribute("value").as_bool();
	verbose_task_source_execute =
			verbose_node.child("source_execute").attribute("value").as_bool();
	verbose_task_function_calls =
			verbose_node.child("function_calls").attribute("value").as_bool();

	return true;
}

bool GlobalInputClass::readNoCLayout(std::string filePath) {
	std::cout << "Reading NoC config: " << filePath << endl;
	pugi::xml_document doc;
	pugi::xml_parse_result result = doc.load_file(filePath.c_str());
	assert(result && "Failed to read NoC file!");

	pugi::xml_node noc_node = doc.child("network-on-chip");

	/// Read Types ///
	nodeTypes.resize(
			noc_node.child("nodeTypes").select_nodes("nodeType").size());
	for (pugi::xml_node node : noc_node.child("nodeTypes").children("nodeType")) {
		int id = node.attribute("id").as_int();
		std::string model =
				node.child("routerModel").attribute("value").as_string();
		std::string routing =
				node.child("routing").attribute("value").as_string();
		std::string selection =
				node.child("selection").attribute("value").as_string();
		int clk = node.child("clockSpeed").attribute("value").as_int();
		std::string arbiterType = node.child("arbiterType").attribute("value").as_string();

		nodeTypes.at(id) = new NodeType(id, model, routing, selection, clk, arbiterType);
		nodeTypes.at(id)->nodes.resize(
				noc_node.select_nodes(
						("nodes/node/nodeType[@value='" + std::to_string(id)
								+ "']").c_str()).size());
		typeByName[model] = nodeTypes.at(id);
	}

	/// Read Layer ///
	layerTypes.resize(
			noc_node.child("layerTypes").select_nodes("layerType").size());
	for (pugi::xml_node node : noc_node.child("layerTypes").children(
			"layerType")) {
		int id = node.attribute("id").as_int();
		int technology = node.child("technology").attribute("value").as_int();

		layerTypes.at(id) = new LayerType(id, technology);
	}

	/// Read Nodes ///
	nodes.resize(noc_node.child("nodes").select_nodes("node").size());
	for (pugi::xml_node xmlnode : noc_node.child("nodes").children("node")) {
		int id = xmlnode.attribute("id").as_int();
		std::string name = "node_" + std::to_string(id);
		float x = xmlnode.child("xPos").attribute("value").as_float();
		float y = xmlnode.child("yPos").attribute("value").as_float();
		float z = xmlnode.child("zPos").attribute("value").as_float();
		int idType = xmlnode.child("idType").attribute("value").as_int();
		NodeType* nodeType = nodeTypes.at(
				xmlnode.child("nodeType").attribute("value").as_int());
		LayerType* layerType = layerTypes.at(
				xmlnode.child("layerType").attribute("value").as_int());

		Node* node = new Node(id, Vec3D<float>(x, y, z), idType, nodeType,
				layerType);
		nodes.at(id) = node;
		nodeType->nodes.at(idType) = node;
		posToId[node->pos].insert(node);
		xPositions.push_back(node->pos.x);
		yPositions.push_back(node->pos.y);
		zPositions.push_back(node->pos.z);
	}

	sort(xPositions.begin(), xPositions.end());
	xPositions.erase(unique(xPositions.begin(), xPositions.end()),
			xPositions.end());

	sort(yPositions.begin(), yPositions.end());
	yPositions.erase(unique(yPositions.begin(), yPositions.end()),
			yPositions.end());

	sort(zPositions.begin(), zPositions.end());
	zPositions.erase(unique(zPositions.begin(), zPositions.end()),
			zPositions.end());

	/// Read Connections ///
	connections.resize(
			noc_node.child("connections").select_nodes("con").size());
	for (pugi::xml_node connection : noc_node.child("connections").children(
			"con")) {
		int id = connection.attribute("id").as_int();
		std::vector<Node*> nodesOfConnection;
		std::vector<int> vcCount;
		std::vector<int> bufferDepth;

		for (pugi::xml_node nodeNum : connection.child("ports").children("port")) {
			Node* currentNode = nodes.at(
					nodeNum.child("node").attribute("value").as_int());
			nodesOfConnection.push_back(currentNode);
			vcCount.push_back(
					nodeNum.child("vcCount").attribute("value").as_int());
			bufferDepth.push_back(
					nodeNum.child("bufferDepth").attribute("value").as_int());
		}

		float length = connection.child("length").attribute("value").as_float();
		int depth = connection.child("depth").attribute("value").as_int();
		int width = connection.child("width").attribute("value").as_int();

		connections.at(id) = new Connection(id, nodesOfConnection, vcCount,
				bufferDepth, length, width, depth);

		// add Connected nodes and connections to nodes list
		int i = 0;
		for (pugi::xml_node nodeNum : connection.child("ports").children("port")) {
			Node* currentNode = nodes.at(
					nodeNum.child("node").attribute("value").as_int());
			connections.at(id)->nodePos.insert( { currentNode, i });
			i++;

			if (connection.child("ports").select_nodes("port").size() == 2) {
				for (pugi::xml_node nodeNum : connection.child("ports").children(
						"port")) {
					Node* conNode = nodes.at(
							nodeNum.child("node").attribute("value").as_int());

					if (conNode != currentNode) {
						if (std::find(currentNode->connectedNodes.begin(),
								currentNode->connectedNodes.end(), conNode)
								== currentNode->connectedNodes.end()) {

							currentNode->connectedNodes.push_back(conNode);
							std::vector<int> v;
							v.push_back(currentNode->connections.size());
							currentNode->connectionsToNode.insert(
									{ conNode, v });
						}
						currentNode->connectionsToNode.at(conNode).push_back(
								currentNode->connections.size());
						currentNode->conToNode[currentNode->connections.size()] =
								conNode;
					}
				}

				currentNode->conToPos[connections.at(id)] =
						currentNode->connections.size();
				currentNode->connections.push_back(connections.at(id));
			} else {
				std::cerr << "ERROR: Other than 2 Nodes inside Connection ID:"
						<< id << endl;
			}
		}
	}

	for (Node* node : nodes) {

		//check for common directions
		std::vector<Vec3D<float>> distance(DIR::size, Vec3D<float>(2, 2, 2));
		for (Node* connectedNode : node->connectedNodes) {
			Vec3D<float> offset = node->pos - connectedNode->pos;
			int nullValues = offset.x ? 0 : 1;
			nullValues += offset.y ? 0 : 1;
			nullValues += offset.z ? 0 : 1;

			if (nullValues == 2) { //one axis differs
				if (offset.x > 0
						&& (!node->dirToCon[DIR::West]
								|| offset.x < distance.at(DIR::West).x)) {
					node->dirToCon[DIR::West] = node->connectionsToNode.at(
							connectedNode).at(0);
					node->conToDir[node->connectionsToNode.at(connectedNode).at(
							0)] = DIR::West;
					distance.at(DIR::West) = offset;
				} else if (offset.x < 0
						&& (!node->dirToCon[DIR::East]
								|| offset.x > distance.at(DIR::East).x)) {
					node->dirToCon[DIR::East] = node->connectionsToNode.at(
							connectedNode).at(0);
					node->conToDir[node->connectionsToNode.at(connectedNode).at(
							0)] = DIR::East;
					distance.at(DIR::East) = offset;
				} else if (offset.y < 0
						&& (!node->dirToCon[DIR::North]
								|| offset.y > distance.at(DIR::North).y)) {
					node->dirToCon[DIR::North] = node->connectionsToNode.at(
							connectedNode).at(0);
					node->conToDir[node->connectionsToNode.at(connectedNode).at(
							0)] = DIR::North;
					distance.at(DIR::North) = offset;
				} else if (offset.y > 0
						&& (!node->dirToCon[DIR::South]
								|| offset.y < distance.at(DIR::South).y)) {
					node->dirToCon[DIR::South] = node->connectionsToNode.at(
							connectedNode).at(0);
					node->conToDir[node->connectionsToNode.at(connectedNode).at(
							0)] = DIR::South;
					distance.at(DIR::South) = offset;
				} else if (offset.z < 0
						&& (!node->dirToCon[DIR::Up]
								|| offset.z > distance.at(DIR::Up).z)) {
					node->dirToCon[DIR::Up] = node->connectionsToNode.at(
							connectedNode).at(0);
					node->conToDir[node->connectionsToNode.at(connectedNode).at(
							0)] = DIR::Up;
					distance.at(DIR::Up) = offset;
				} else if (offset.z > 0
						&& (!node->dirToCon[DIR::Down]
								|| offset.z < distance.at(DIR::Down).z)) {
					node->dirToCon[DIR::Down] = node->connectionsToNode.at(
							connectedNode).at(0);
					node->conToDir[node->connectionsToNode.at(connectedNode).at(
							0)] = DIR::Down;
					distance.at(DIR::Down) = offset;
				}
			} else if (nullValues == 3) { //no axis differs
				if (!node->dirToCon[DIR::Local]) {
					node->dirToCon[DIR::Local] = node->connectionsToNode.at(
							connectedNode).at(0);
					node->conToDir[node->connectionsToNode.at(connectedNode).at(
							0)] = DIR::Local;
				}
			}
		}
	}

	for (Connection* c : connections) {
		c->vcBufferUtilization.resize(c->nodes.size());
		c->bufferUtilization.resize(c->nodes.size());
		c->vcBufferCongestion.resize(c->nodes.size());
		c->bufferCongestion.resize(c->nodes.size());
		for (Node* n : c->nodes) {
			c->vcBufferUtilization.at(c->nodePos.at(n)).resize(
					c->vcCount.at(c->nodePos.at(n)), 0);
			c->vcBufferCongestion.at(c->nodePos.at(n)).resize(
					c->vcCount.at(c->nodePos.at(n)), 0);

		}
	}

	return true;
}

bool GlobalInputClass::readDataStream(std::string taskFilePath,
		std::string mappingFilePath) {
	std::cout << "Reading Mapping config: " << mappingFilePath << endl;
	pugi::xml_document doc;
	pugi::xml_parse_result result = doc.load_file(mappingFilePath.c_str());
	assert(result && "Failed to read Mapping file!");

	pugi::xml_node map_node = doc.child("map");

	/// Read Bindings ///
	std::map<int, Node*> bindings;

	for (pugi::xml_node bind_node : map_node.children("bind")) {
		int task = readRequiredIntAttribute(bind_node, "task", "value");
		Node* node = nodes.at(
				readRequiredIntAttribute(bind_node, "node", "value"));
		bindings[task] = node;
	}

	std::cout << "Reading Data config: " << taskFilePath << endl;
	result = doc.load_file(taskFilePath.c_str());
	assert(result && "Failed to read Data file!");
	pugi::xml_node data_node = doc.child("data");

	/// Read Types ///
	application_numberOfTrafficTypes =
			data_node.child("dataTypes").select_nodes("dataType").size();
	dataTypes.resize(
			data_node.child("dataTypes").select_nodes("dataType").size());
	for (pugi::xml_node type_node : data_node.child("dataTypes").children(
			"dataType")) {
		int id = readRequiredIntAttribute(type_node, "id");
		std::string name = readRequiredStringAttribute(type_node, "name",
				"value");
		dataTypes.at(id) = new DataType(id, name);
	}

	/// Read Data ///
	tasks.resize(data_node.child("tasks").select_nodes("task").size());
	for (pugi::xml_node task_node : data_node.child("tasks").children("task")) {
		int id = task_node.attribute("id").as_int();
		Task* task = new Task(id, bindings.at(id));
		readAttributeIfExists(task_node, "start", "min", task->minStart);
		readAttributeIfExists(task_node, "start", "max", task->maxStart);
		readAttributeIfExists(task_node, "duration", "min", task->minDuration);
		readAttributeIfExists(task_node, "duration", "max", task->maxDuration);
		readAttributeIfExists(task_node, "repeat", "min", task->minRepeat);
		readAttributeIfExists(task_node, "repeat", "max", task->maxRepeat);

		// Read Requirements
		std::vector<DataRequirement*> requirements;
		requirements.resize(
				task_node.child("requires").select_nodes("requirement").size());
		for (pugi::xml_node requirement_node : task_node.child("requires").children(
				"requirement")) {
			int id = readRequiredIntAttribute(requirement_node, "id");
			DataType* type = dataTypes.at(
					readRequiredIntAttribute(requirement_node, "type",
							"value"));
			DataRequirement* req = new DataRequirement(id, type);
			readAttributeIfExists(requirement_node, "count", "min",
					req->minCount);
			readAttributeIfExists(requirement_node, "count", "max",
					req->maxCount);
			requirements.at(id) = req;
		}
		task->requirements = requirements;

		// Read Destinations
		std::vector<std::pair<float, std::vector<DataDestination*>>> possibilities;
		possibilities.resize(
				task_node.child("generates").select_nodes("possibility").size());
		for (pugi::xml_node generate_node : task_node.child("generates").children(
				"possibility")) {
			int id = readRequiredIntAttribute(generate_node, "id");
			float probability = readRequiredFloatAttribute(generate_node,
					"probability", "value");

			std::vector<DataDestination*> destinations;
			destinations.resize(
					generate_node.child("destinations").select_nodes(
							"destination").size());
			for (pugi::xml_node destination_node : generate_node.child(
					"destinations").children("destination")) {
				int id = readRequiredIntAttribute(destination_node, "id");
				DataType* type = dataTypes.at(
						readRequiredIntAttribute(destination_node, "type",
								"value"));
				Node* node = bindings.at(
						readRequiredIntAttribute(destination_node, "task",
								"value"));
				int minInterval = readRequiredIntAttribute(destination_node,
						"interval", "min");
				int maxInterval = readRequiredIntAttribute(destination_node,
						"interval", "max");
				DataDestination* dataDestination = new DataDestination(id, type,
						node, minInterval, maxInterval);

				readAttributeIfExists(destination_node, "count", "min",
						dataDestination->minCount);
				readAttributeIfExists(destination_node, "count", "max",
						dataDestination->maxCount);
				readAttributeIfExists(destination_node, "delay", "min",
						dataDestination->minDelay);
				readAttributeIfExists(destination_node, "delay", "max",
						dataDestination->maxDelay);

				destinations.at(id) = dataDestination;
			}

			possibilities.at(id) = {probability, destinations};
		}
		task->possibilities = possibilities;

		tasks.at(id) = task;

	}

	return true;
}

int GlobalInputClass::getRandomIntBetween(int min, int max) {
	std::uniform_int_distribution<int> dis(min, max);
	return dis(*rand);
}

float GlobalInputClass::getRandomFloatBetween(float min, float max) {
	std::uniform_real_distribution<float> dis(min, max);
	return dis(*rand);
}

void GlobalInputClass::readAttributeIfExists(pugi::xml_node node,
		const char* child, const char* attribute, int& var) {
	readAttributeIfExists(node.child(child), attribute, var);
}

void GlobalInputClass::readAttributeIfExists(pugi::xml_node node,
		const char* attribute, int& var) {
	if (!node.attribute(attribute).empty()) {
		var = node.attribute(attribute).as_int();
	}
}

int GlobalInputClass::readRequiredIntAttribute(pugi::xml_node node,
		const char* child, const char* attribute) {
	return readRequiredIntAttribute(node.child(child), attribute);
}

int GlobalInputClass::readRequiredIntAttribute(pugi::xml_node node,
		const char* attribute) {
	if (node.attribute(attribute).empty()) {
		FATAL("Can not read node:" << node.path() << " " << attribute);
	}
	return node.attribute(attribute).as_int();
}

float GlobalInputClass::readRequiredFloatAttribute(pugi::xml_node node,
		const char* child, const char* attribute) {
	return readRequiredFloatAttribute(node.child(child), attribute);
}

float GlobalInputClass::readRequiredFloatAttribute(pugi::xml_node node,
		const char* attribute) {
	if (node.attribute(attribute).empty()) {
		FATAL("Can not read node:" << node.path() << " " << attribute);
	}
	return node.attribute(attribute).as_float();
}

std::string GlobalInputClass::readRequiredStringAttribute(pugi::xml_node node,
		const char* child, const char* attribute) {
	return readRequiredStringAttribute(node.child(child), attribute);
}

std::string GlobalInputClass::readRequiredStringAttribute(pugi::xml_node node,
		const char* attribute) {
	if (node.attribute(attribute).empty()) {
		FATAL("Can not read node:" << node.path() << " " << attribute);
	}
	return node.attribute(attribute).as_string();
}
