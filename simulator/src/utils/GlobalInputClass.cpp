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
	simulation_time = gen_node.child("simulationTime").attribute("value").as_int();
	outputToFile = gen_node.child("outputToFile").attribute("value").as_bool();
	outputFileName = gen_node.child("outputToFile").child_value();

	//NOC
	pugi::xml_node noc_node = doc.child("configuration").child("noc");
	noc_file = noc_node.child_value("nocFile");
	flitsPerPacket = noc_node.child("flitsPerPacket").attribute("value").as_int();
	Vdd = noc_node.child("Vdd").attribute("value").as_float();

	//APPLICATION
	pugi::xml_node app_node = doc.child("configuration").child("application");
	benchmark = app_node.child_value("benchmark");
	data_file = app_node.child_value("dataFile");
	map_file = app_node.child_value("mapFile");
	application_file = app_node.child_value("simulationFile");
	application_mapping_file = app_node.child_value("mappingFile");
	netraceFile = app_node.child_value("netraceFile");
	netraceStartRegion = app_node.child("netraceStartRegion").attribute("value").as_int();
	application_is_uniform = app_node.child("isUniform").attribute("value").as_bool();
	application_numberOfTrafficTypes = app_node.child("numberOfTrafficTypes").attribute("value").as_int();

	for (pugi::xml_node phase_node : app_node.child("synthetic").children("phase")) {
		syntheticPhase.push_back(
				{ phase_node.attribute("name").as_string(), phase_node.child("spatialDistribution").attribute("value").as_string(), phase_node.child(
						"initalOffset").attribute("max").as_int(), phase_node.child("initalOffset").attribute("min").as_int(), phase_node.child(
						"waveCount").attribute("value").as_int(), phase_node.child("packagesPerWave").attribute("value").as_int(), phase_node.child(
						"waveDelay").attribute("value").as_int(),
						phase_node.child("spatialDistribution").attribute("hotspot") ?
								phase_node.child("spatialDistribution").attribute("hotspot").as_int() : -1 });
	}

	//---------------------------------------------------------------------------

	//VERBOSE
	pugi::xml_node verbose_node;

	//	PE Verbosity
	verbose_node = doc.child("configuration").child("verbose").child("processingElements");
	verbose_pe_function_calls = verbose_node.child("function_calls").attribute("value").as_bool();
	verbose_pe_send_flit = verbose_node.child("send_flit").attribute("value").as_bool();
	verbose_pe_send_head_flit = verbose_node.child("send_head_flit").attribute("value").as_bool();
	verbose_pe_receive_flit = verbose_node.child("receive_flit").attribute("value").as_bool();
	verbose_pe_receive_tail_flit = verbose_node.child("receive_tail_flit").attribute("value").as_bool();
	verbose_pe_throttle = verbose_node.child("throttle").attribute("value").as_bool();
	verbose_pe_reset = verbose_node.child("reset").attribute("value").as_bool();

	//	Router Verbosity
	verbose_node = doc.child("configuration").child("verbose").child("router");
	verbose_router_function_calls = verbose_node.child("function_calls").attribute("value").as_bool();
	verbose_router_send_flit = verbose_node.child("send_flit").attribute("value").as_bool();
	verbose_router_send_head_flit = verbose_node.child("send_head_flit").attribute("value").as_bool();
	verbose_router_receive_flit = verbose_node.child("receive_flit").attribute("value").as_bool();
	verbose_router_receive_head_flit = verbose_node.child("receive_head_flit").attribute("value").as_bool();
	verbose_router_assign_channel = verbose_node.child("assign_channel").attribute("value").as_bool();
	verbose_router_throttle = verbose_node.child("throttle").attribute("value").as_bool();
	verbose_router_buffer_overflow = verbose_node.child("buffer_overflow").attribute("value").as_bool();
	verbose_router_reset = verbose_node.child("reset").attribute("value").as_bool();

	//	Netrace Verbosity
	verbose_node = doc.child("configuration").child("verbose").child("netrace");
	verbose_netrace_inject = verbose_node.child("inject").attribute("value").as_bool();
	verbose_netrace_eject = verbose_node.child("eject").attribute("value").as_bool();
	verbose_netrace_router_receive = verbose_node.child("router_receive").attribute("value").as_bool();

	//	Task Verbosity
	verbose_node = doc.child("configuration").child("verbose").child("tasks");
	verbose_task_xml_parse = verbose_node.child("xml_parse").attribute("value").as_bool();
	verbose_task_data_receive = verbose_node.child("data_receive").attribute("value").as_bool();
	verbose_task_data_send = verbose_node.child("data_send").attribute("value").as_bool();
	verbose_task_source_execute = verbose_node.child("source_execute").attribute("value").as_bool();
	verbose_task_function_calls = verbose_node.child("function_calls").attribute("value").as_bool();

	return true;
}

bool GlobalInputClass::readNoCLayout(std::string filePath) {
	std::cout << "Reading NoC config: " << filePath << endl;
	pugi::xml_document doc;
	pugi::xml_parse_result result = doc.load_file(filePath.c_str());
	assert(result && "Failed to read NoC file!");

	pugi::xml_node noc_node = doc.child("network-on-chip");

	/// Read Types ///
	nodeTypes.resize(noc_node.child("nodeTypes").select_nodes("nodeType").size());
	for (pugi::xml_node node : noc_node.child("nodeTypes").children("nodeType")) {
		int id = node.attribute("id").as_int();
		std::string model = node.child("routerModel").attribute("value").as_string();
		std::string routing = node.child("routing").attribute("value").as_string();
		std::string selection = node.child("selection").attribute("value").as_string();
		int clk = node.child("clockSpeed").attribute("value").as_int();
		nodeTypes.at(id) = new NodeType(id, model, routing, selection, clk);
		nodeTypes.at(id)->nodes.resize(noc_node.select_nodes(("nodes/node/nodeType[@value='" + std::to_string(id) + "']").c_str()).size());
		typeByName[model] = nodeTypes.at(id);
	}

	/// Read Layer ///
	layerTypes.resize(noc_node.child("layerTypes").select_nodes("layerType").size());
	for (pugi::xml_node node : noc_node.child("layerTypes").children("layerType")) {
		int id = node.attribute("id").as_int();
		int technology = node.child("technology").attribute("value").as_int();

		layerTypes.at(id) = new LayerType(id, technology);
	}

	/// Read Nodes ///
	nodes.resize(noc_node.child("nodes").select_nodes("node").size());
	for (pugi::xml_node node : noc_node.child("nodes").children("node")) {
		int id = node.attribute("id").as_int();
		std::string name = "node_" + std::to_string(id);
		float x = node.child("xPos").attribute("value").as_float();
		float y = node.child("yPos").attribute("value").as_float();
		float z = node.child("zPos").attribute("value").as_float();
		int idType = node.child("idType").attribute("value").as_int();
		NodeType* nodeType = nodeTypes.at(node.child("nodeType").attribute("value").as_int());
		LayerType* layerType = layerTypes.at(node.child("layerType").attribute("value").as_int());

		nodes.at(id) = new Node(id, Vec3D<float>(x, y, z), idType, nodeType, layerType);
		idToPos.insert(std::pair<int, Vec3D<float>>(id, Vec3D<float>(x, y, z)));
//		posToId.insert(std::pair<Vec3D<float>, int>(Vec3D<float>(x, y, z), id));

		nodeType->nodes.at(idType) = nodes.at(id);
	}

	/// Read Connections ///
	connections.resize(noc_node.child("connections").select_nodes("con").size());
	for (pugi::xml_node connection : noc_node.child("connections").children("con")) {
		int id = connection.attribute("id").as_int();
		std::vector<Node*> nodesOfConnection;
		std::vector<int> vcCount;
		std::vector<int> bufferDepth;

		for (pugi::xml_node nodeNum : connection.child("ports").children("port")) {
			Node* currentNode = nodes.at(nodeNum.child("node").attribute("value").as_int());
			nodesOfConnection.push_back(currentNode);
			vcCount.push_back(nodeNum.child("vcCount").attribute("value").as_int());
			bufferDepth.push_back(nodeNum.child("bufferDepth").attribute("value").as_int());
		}

		float length = connection.child("length").attribute("value").as_float();
		int depth = connection.child("depth").attribute("value").as_int();
		int width = connection.child("width").attribute("value").as_int();

		connections.at(id) = new Connection(id, nodesOfConnection, vcCount, bufferDepth, length, width, depth);

		// add Connected nodes and connections to nodes list
		int i = 0;
		for (pugi::xml_node nodeNum : connection.child("ports").children("port")) {
			Node* currentNode = nodes.at(nodeNum.child("node").attribute("value").as_int());
			connections.at(id)->nodePos.insert( { currentNode, i });
			i++;

			if (connection.child("ports").select_nodes("port").size() == 2) {
				for (pugi::xml_node nodeNum : connection.child("ports").children("port")) {
					Node* conNode = nodes.at(nodeNum.child("node").attribute("value").as_int());

					if (conNode != currentNode) {
						if (std::find(currentNode->connectedNodes.begin(), currentNode->connectedNodes.end(), conNode)
								== currentNode->connectedNodes.end()) {

							currentNode->connectedNodes.push_back(conNode);
							std::vector<int> v;
							v.push_back(currentNode->connections.size());
							currentNode->connectionsToNode.insert( { conNode, v });
						}
						currentNode->connectionsToNode.at(conNode).push_back(currentNode->connections.size());
						currentNode->conToNode[currentNode->connections.size()]=conNode;
					}
				}

				currentNode->conToPos[connections.at(id)] = currentNode->connections.size();
				currentNode->connections.push_back(connections.at(id));
			} else {
				std::cerr << "ERROR: Other than 2 Nodes inside Connection ID:" << id << endl;
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
				if (offset.x > 0 && (!node->dirToCon[DIR::West] || offset.x < distance.at(DIR::West).x)) {
					node->dirToCon[DIR::West] = node->connectionsToNode.at(connectedNode).at(0);
					node->conToDir[node->connectionsToNode.at(connectedNode).at(0)] = DIR::West;
					distance.at(DIR::West) = offset;
				} else if (offset.x < 0 && (!node->dirToCon[DIR::East] || offset.x > distance.at(DIR::East).x)) {
					node->dirToCon[DIR::East] = node->connectionsToNode.at(connectedNode).at(0);
					node->conToDir[node->connectionsToNode.at(connectedNode).at(0)] = DIR::East;
					distance.at(DIR::East) = offset;
				} else if (offset.y < 0 && (!node->dirToCon[DIR::North] || offset.y > distance.at(DIR::North).y)) {
					node->dirToCon[DIR::North] = node->connectionsToNode.at(connectedNode).at(0);
					node->conToDir[node->connectionsToNode.at(connectedNode).at(0)] = DIR::North;
					distance.at(DIR::North) = offset;
				} else if (offset.y > 0 && (!node->dirToCon[DIR::South] || offset.y < distance.at(DIR::South).y)) {
					node->dirToCon[DIR::South] = node->connectionsToNode.at(connectedNode).at(0);
					node->conToDir[node->connectionsToNode.at(connectedNode).at(0)] = DIR::South;
					distance.at(DIR::South) = offset;
				} else if (offset.z < 0 && (!node->dirToCon[DIR::Up] || offset.z > distance.at(DIR::Up).z)) {
					node->dirToCon[DIR::Up] = node->connectionsToNode.at(connectedNode).at(0);
					node->conToDir[node->connectionsToNode.at(connectedNode).at(0)] = DIR::Up;
					distance.at(DIR::Up) = offset;
				} else if (offset.z > 0 && (!node->dirToCon[DIR::Down] || offset.z < distance.at(DIR::Down).z)) {
					node->dirToCon[DIR::Down] = node->connectionsToNode.at(connectedNode).at(0);
					node->conToDir[node->connectionsToNode.at(connectedNode).at(0)] = DIR::Down;
					distance.at(DIR::Down) = offset;
				}
			} else if (nullValues == 3) { //no axis differs
				if (!node->dirToCon[DIR::Local]) {
					node->dirToCon[DIR::Local] = node->connectionsToNode.at(connectedNode).at(0);
					node->conToDir[node->connectionsToNode.at(connectedNode).at(0)] = DIR::Local;
				}
			}
		}
	}

	for (Node* node : nodes) {
		xPositions.insert( { node->pos.x, 0 });
		yPositions.insert( { node->pos.y, 0 });
		zPositions.insert( { node->pos.z, 0 });
	}

	for (Node* node : nodes) {
		int x = std::distance(xPositions.begin(), xPositions.find(node->pos.x));
		int y = std::distance(yPositions.begin(), yPositions.find(node->pos.y));
		int z = std::distance(zPositions.begin(), zPositions.find(node->pos.z));

		xPositions[node->pos.x] = x;
		yPositions[node->pos.y] = y;
		zPositions[node->pos.z] = z;

		idToScPos.insert(std::pair<int, Vec3D<int>>(node->id, Vec3D<int>(x, y, z)));
//		scPosToId.insert(std::pair<Vec3D<int>, int>(Vec3D<int>(x, y, z), node->id));
	}

	for(Connection* c: connections){
			c->vcBufferUtilization.resize(c->nodes.size());
			c->bufferUtilization.resize(c->nodes.size());
			c->vcBufferCongestion.resize(c->nodes.size());
			c->bufferCongestion.resize(c->nodes.size());
			for(Node* n: c->nodes){
				c->vcBufferUtilization.at(c->nodePos.at(n)).resize(c->vcCount.at(c->nodePos.at(n)), 0);
				c->vcBufferCongestion.at(c->nodePos.at(n)).resize(c->vcCount.at(c->nodePos.at(n)), 0);

	 		}
	 	}
////	sort router and pe by ascending position
//	sort( router.begin( ), router.end( ), [ ]( const Node* lnode, const Node* rnode ){
//	   return rnode->pos < lnode->pos;
//	});
//
//	sort( pe.begin( ), pe.end( ), [ ]( const Node* lnode, const Node* rnode ){
//	   return lnode->pos < rnode->pos;
//	});
//
////	for(Node* n: router){
////		std::cout<<n->pos.x<<n->pos.y<<n->pos.z<<endl;
////	}



	return true;
}

bool GlobalInputClass::readDataStream(std::string taskFilePath, std::string mappingFilePath) {
	std::cout << "Reading Mapping config: " << mappingFilePath << endl;
	pugi::xml_document doc;
	pugi::xml_parse_result result = doc.load_file(mappingFilePath.c_str());
	assert(result && "Failed to read Mapping file!");

	pugi::xml_node map_node = doc.child("map");

	/// Read Bindings ///
	std::map<int, Node*> bindings;

	for (pugi::xml_node bind_node : map_node.children("bind")) {
		int task = bind_node.child("task").attribute("value").as_int();
		Node* node= nodes.at(bind_node.child("node").attribute("value").as_int());
		bindings[task] = node;
	}



	std::cout << "Reading Data config: " << taskFilePath << endl;
	result = doc.load_file(taskFilePath.c_str());
	assert(result && "Failed to read Data file!");
	pugi::xml_node data_node = doc.child("data");

	/// Read Types ///
	dataTypes.resize(data_node.child("dataTypes").select_nodes("dataType").size());
	for (pugi::xml_node type_node : data_node.child("dataTypes").children("dataType")) {
		int id = type_node.attribute("id").as_int();
		std::string name = type_node.child("name").attribute("value").as_string();
		dataTypes.at(id) = new DataType(id, name);
	}

	/// Read Data ///
	tasks.resize(data_node.child("tasks").select_nodes("task").size());
	for (pugi::xml_node task_node : data_node.child("tasks").children("task")) {
		int id = task_node.attribute("id").as_int();
		int minRepeat = task_node.child("repeat").attribute("min").as_int();
		int maxRepeat = task_node.child("repeat").attribute("max").as_int();

		// Read Requirements
		std::vector<DataRequirement*> requirements;
		requirements.resize(task_node.child("requires").select_nodes("requirement").size());
		for (pugi::xml_node requirement_node : task_node.child("requires").children("requirement")) {
			int id = requirement_node.attribute("id").as_int();
			DataType* type = dataTypes.at(requirement_node.child("type").attribute("value").as_int());
			int minCount = requirement_node.child("count").attribute("min").as_int();
			int maxCount = requirement_node.child("count").attribute("max").as_int();
			requirements.at(id) = new DataRequirement(id, type, minCount, maxCount);
		}

		// Read Destinations
		std::vector<std::pair<float, std::vector<DataDestination*>>> possibilities;
		possibilities.resize(task_node.child("generates").select_nodes("possibility").size());
		for (pugi::xml_node generate_node : task_node.child("generates").children("possibility")) {
					int id = generate_node.attribute("id").as_int();
					float probability = generate_node.child("probability").attribute("value").as_float();

					std::vector<DataDestination*> destinations;
					destinations.resize(generate_node.child("destinations").select_nodes("destination").size());
					for (pugi::xml_node destination_node : generate_node.child("destinations").children("destination")) {
						int id = destination_node.attribute("id").as_int();
						DataType* type = dataTypes.at(destination_node.child("type").attribute("value").as_int());
						int minCount = destination_node.child("count").attribute("min").as_int();
						int maxCount = destination_node.child("count").attribute("max").as_int();
						int minDelay = destination_node.child("delay").attribute("min").as_int();
						int maxDelay = destination_node.child("delay").attribute("max").as_int();
						int minInterval = destination_node.child("interval").attribute("min").as_int();
						int maxInterval = destination_node.child("interval").attribute("max").as_int();
						Node* node = bindings.at(destination_node.child("task").attribute("value").as_int());

						destinations.at(id) = new DataDestination(id, type, minCount, maxCount, minDelay, maxDelay, minInterval, maxInterval, node);
					}

					possibilities.at(id) = {probability, destinations};
				}

		tasks.at(id) = new Task(id, bindings.at(id), minRepeat, maxRepeat, requirements, possibilities);

	}





	return true;
}

int GlobalInputClass::getRandomIntBetween(int min, int max){
	std::uniform_int_distribution<int> dis(min, max);
	return dis(*rand);
}


float GlobalInputClass::getRandomFloatBetween(float min, float max){
	std::uniform_real_distribution<float> dis(min, max);
	return dis(*rand);
}
