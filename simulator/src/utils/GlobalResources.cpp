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

#include "GlobalResources.h"
#include <iostream>
#include <iomanip>

GlobalResources::GlobalResources()
    : simulation_time(0),
      outputToFile(false),
      activateFlitTracing(false),
      routingVerticalThreshold(1.0),
      Vdd(1.0),
      isUniform(false),
      numberOfTrafficTypes(0),
      synthetic_start_measurement_time(-1)
{
    rand = new std::mt19937_64();
    auto seed = std::random_device{}();
    rand->seed(seed);
    rd_seed = seed;
}

GlobalResources &GlobalResources::getInstance()
{
    static GlobalResources instance;
    return instance;
}

int GlobalResources::getRandomIntBetween(int min, int max)
{
    std::uniform_int_distribution<int> dis(min, max);
    return dis(*rand);
}

float GlobalResources::getRandomFloatBetween(float min, float max)
{
    std::uniform_real_distribution<float> dis(min, max);
    return dis(*rand);
}

void GlobalResources::readAttributeIfExists(pugi::xml_node node, const char *child, const char *attribute, int &var)
{
    readAttributeIfExists(node.child(child), attribute, var);
}

void GlobalResources::readAttributeIfExists(pugi::xml_node node, const char *attribute, int &var)
{
    if (!node.attribute(attribute).empty())
    {
        var = node.attribute(attribute).as_int();
    }
}

int GlobalResources::readRequiredIntAttribute(pugi::xml_node node, const char *child, const char *attribute)
{
    return readRequiredIntAttribute(node.child(child), attribute);
}

int GlobalResources::readRequiredIntAttribute(pugi::xml_node node, const char *attribute)
{
    if (node.attribute(attribute).empty())
    {
        FATAL("Can not read node:" << node.path() << " " << attribute);
    }
    return node.attribute(attribute).as_int();
}

float GlobalResources::readRequiredFloatAttribute(pugi::xml_node node, const char *child, const char *attribute)
{
    return readRequiredFloatAttribute(node.child(child), attribute);
}

float GlobalResources::readRequiredFloatAttribute(pugi::xml_node node, const char *attribute)
{
    if (node.attribute(attribute).empty())
    {
        FATAL("Can not read node:" << node.path() << " " << attribute);
    }
    return node.attribute(attribute).as_float();
}

std::string
GlobalResources::readRequiredStringAttribute(pugi::xml_node node, const char *child, const char *attribute)
{
    return readRequiredStringAttribute(node.child(child), attribute);
}

std::string GlobalResources::readRequiredStringAttribute(pugi::xml_node node, const char *attribute)
{
    if (node.attribute(attribute).empty())
    {
        FATAL("Can not read node:" << node.path() << " " << attribute);
    }
    return node.attribute(attribute).as_string();
}

std::vector<std::string> GlobalResources::string_split(const std::string &str, const std::string &delim)
{
    std::vector<std::string> strings;
    auto start = 0U;
    auto end = str.find(delim);
    while (end != std::string::npos)
    {
        strings.push_back(str.substr(start, end - start));
        start = static_cast<unsigned int>(end + 1);
        end = str.find(delim, start);
    }
    strings.push_back(str.substr(start, end));
    return strings;
}

std::vector<int> GlobalResources::strs_to_ints(const std::vector<std::string> &strings)
{
    std::vector<int> ints{};
    for (auto &str : strings)
    {
        ints.push_back(std::stoi(str));
    }
    return ints;
}

void GlobalResources::createRoutingTable()
{
    std::string filename;
    filename = RoutingTable_file;

    std::ifstream infile(filename);
    std::string line;

    int cpt_line = 0;

    while (std::getline(infile, line))
    {
        std::istringstream iss(line);
        std::string word;
        int DIR;
        std::vector<int> v;
        while (iss >> word)
        {
            DIR = std::stoi(word);
            v.push_back(DIR);
        }

        RoutingTable.push_back(v);
        cpt_line += 1;
    }
    std::cout << "Routing Table :" << std::endl;

    for (auto vec : RoutingTable)
    {
        for (auto x : vec)
            std::cout << x << " , ";
        std::cout << std::endl;
    }
}

void GlobalResources::readConfigFile(const std::string &configPath)
{
    pugi::xml_document doc;

    std::cout << "Reading simulator config: " << configPath << endl;
    struct stat buffer;
    if (!stat(configPath.c_str(), &buffer) == 0)
        cout << endl
             << "ERROR: Config file not found, program will exit with exception" << endl
             << endl;
    pugi::xml_parse_result result = doc.load_file(configPath.c_str());
    assert(result && "Failed to read simulator config file!");

    //GENERAL
    pugi::xml_node gen_node = doc.child("configuration").child("general");
    simulation_time = gen_node.child("simulationTime").attribute("value").as_int();
    outputToFile = gen_node.child("outputToFile").attribute("value").as_bool();
    outputFileName = gen_node.child("outputToFile").child_value();
    activateFlitTracing = gen_node.child("flitTracing").attribute("value").as_bool();

    //ROUTING TABLE
    pugi::xml_node Routing_node = doc.child("configuration").child("noc").child("routingTable");
    if (Routing_node.child("routingTable_mode") != NULL)
    {
        RoutingTable_mode = Routing_node.child("routingTable_mode").attribute("value").as_bool();
    }
    else
    {
        RoutingTable_mode = 0;
    }
    std::cout << "Routing table mode: " << RoutingTable_mode << std::endl;

    if (RoutingTable_mode == 1)
    {
        pugi::xml_node RTFile_node = Routing_node.child("routingTable_path");
        RoutingTable_file = readRequiredStringAttribute(RTFile_node, "value");
        createRoutingTable();

        // DIRECTION MATRIX
        pugi::xml_node DMFile_node = Routing_node.child("directionMatrix_path");
        DirectionMat_file = readRequiredStringAttribute(DMFile_node, "value");
    }

    //NOC
    pugi::xml_node noc_node = doc.child("configuration").child("noc");
    noc_file = noc_node.child_value("nocFile");
    flitsPerPacket = noc_node.child("flitsPerPacket").attribute("value").as_int();
    bitWidth = noc_node.child("bitWidth").attribute("value").as_int();
    routingVerticalThreshold = noc_node.child("routingVerticalThreshold").attribute("value").as_float();
    Vdd = noc_node.child("Vdd").attribute("value").as_float();

    //APPLICATION
    pugi::xml_node app_node = doc.child("configuration").child("application");
    benchmark = app_node.child_value("benchmark");
    data_file = app_node.child_value("dataFile");
    map_file = app_node.child_value("mapFile");
    simulation_file = app_node.child_value("simulationFile");
    mapping_file = app_node.child_value("mappingFile");
    netraceFile = app_node.child_value("netraceFile");
    netraceStartRegion = app_node.child("netraceStartRegion").attribute("value").as_int();
    isUniform = app_node.child("isUniform").attribute("value").as_bool();
    numberOfTrafficTypes = app_node.child("numberOfTrafficTypes").attribute("value").as_int();

    synthID_t syntheticPhaseID = 0;
    for (pugi::xml_node phase_node : app_node.child("synthetic").children("phase"))
    {
        std::string name = readRequiredStringAttribute(phase_node, "name");
        std::string distribution = readRequiredStringAttribute(phase_node, "distribution", "value");
        float injectionRate = readRequiredFloatAttribute(phase_node, "injectionRate", "value");
        SyntheticPhase sp = SyntheticPhase(syntheticPhaseID, name, distribution, injectionRate);
        ++syntheticPhaseID;

        readAttributeIfExists(phase_node, "start", "min", sp.minStart);
        readAttributeIfExists(phase_node, "start", "max", sp.maxStart);
        readAttributeIfExists(phase_node, "duration", "min", sp.minDuration);
        readAttributeIfExists(phase_node, "duration", "max", sp.maxDuration);
        readAttributeIfExists(phase_node, "repeat", "min", sp.minRepeat);
        readAttributeIfExists(phase_node, "repeat", "max", sp.maxRepeat);
        readAttributeIfExists(phase_node, "count", "min", sp.minCount);
        readAttributeIfExists(phase_node, "count", "max", sp.maxCount);
        readAttributeIfExists(phase_node, "delay", "min", sp.minDelay);
        readAttributeIfExists(phase_node, "delay", "max", sp.maxDelay);
        readAttributeIfExists(phase_node, "hotspot", "value", sp.hotspot);

        //set first start time after warmup to start of measurement
        if (benchmark == "synthetic" && name != "warmup" &&
            synthetic_start_measurement_time == -1)
        {
            synthetic_start_measurement_time = sp.minStart;
        }
        syntheticPhases.push_back(sp);
    }
}

void GlobalResources::readNoCLayout(const std::string &nocPath)
{
    std::cout << "Reading NoC layout: " << nocPath << endl;
    assert(access(nocPath.c_str(), F_OK) != -1);
    pugi::xml_document doc;
    pugi::xml_parse_result result = doc.load_file(nocPath.c_str());
    assert(result && "Failed to read NoC file!");

    pugi::xml_node noc_node = doc.child("network-on-chip");
    bufferDepthType = noc_node.child("bufferDepthType").attribute("value").as_string();
    if (!bufferDepthType.empty() && (bufferDepthType != "single" && bufferDepthType != "perVC"))
    {
        FATAL("The value of bufferDepthType in your network file should be either 'single' or 'perVC'!");
    }

    // topology
    topology = noc_node.child_value("topology");
    routingCircular = std::set<std::string>({std::string("torus"), std::string("ring")}).count(topology);

    pugi::xml_node abstract = noc_node.child("abstract");

    // init
    for (int i = 0; i < abstract.child("z").attribute("value").as_int(); i++)
    {
        y_step.push_back((float)-1);
        x_step.push_back((float)-1);
        x_range.push_back(*new std::vector<float>);
        y_range.push_back(*new std::vector<float>);
    }

    // xyz step value
    z_step = abstract.child("z").child("zStep").attribute("value").as_float();
    for (pugi::xml_node node : abstract.child("y").children("layer"))
        y_step[node.attribute("value").as_int()] = node.child("yStep").attribute("value").as_float();
    for (pugi::xml_node node : abstract.child("x").children("layer"))
        x_step[node.attribute("value").as_int()] = node.child("xStep").attribute("value").as_float();

    // xyz range value
    float temp_float;
    std::string str;
    std::stringstream sstr;

    str = abstract.child("z").child_value("zRange");
    sstr = std::stringstream(str);
    while (sstr >> temp_float)
        z_range.push_back(temp_float);
    for (pugi::xml_node node : abstract.child("y").children("layer"))
    {
        str = node.child_value("yRange");
        sstr = std::stringstream(str);
        while (sstr >> temp_float)
            y_range[node.attribute("value").as_int()].push_back(temp_float);
    }
    for (pugi::xml_node node : abstract.child("x").children("layer"))
    {
        str = node.child_value("xRange");
        sstr = std::stringstream(str);
        while (sstr >> temp_float)
            x_range[node.attribute("value").as_int()].push_back(temp_float);
    }

    Vec3D<float> temp_norm;
    Vec3D<int> temp_coord;
    for (int zi = 0; zi < z_range.size(); zi++)
        for (int yi = 0; yi < y_range[zi].size(); yi++)
            for (int xi = 0; xi < x_range[zi].size(); xi++)
            {
                temp_coord.x = xi;
                temp_coord.y = yi;
                temp_coord.z = zi;
                temp_norm.x = x_range[zi][xi];
                temp_norm.y = y_range[zi][yi];
                temp_norm.z = z_range[zi];
                norm_to_coord[temp_norm] = temp_coord;
            }

#ifdef ENABLE_NETRACE
    int temp_z = noc_node.child("abstract").child("z").attribute("value").as_int();
    std::vector<int> temp_xs;
    std::vector<int> temp_ys;
    int temp;
    std::stringstream iss;
    iss = std::stringstream(noc_node.child("abstract").child("y").attribute("value").as_string());
    while (iss >> temp)
        temp_xs.push_back(temp);
    iss = std::stringstream(noc_node.child("abstract").child("x").attribute("value").as_string());
    while (iss >> temp)
        temp_ys.push_back(temp);
    int node_count = 0;
    for (int i = 0; i < temp_z; i++)
        node_count += (temp_xs[i] * temp_ys[i]);

    // make sure node_count always larger equal than 64, because netrace simulates 64 nodes
    // this line is to make it compatible for old simulation (old network.xml do not have xs, ys & z values.)
    node_count = (node_count > 64) ? node_count : 64;

    for (int i = 0; i < node_count; ++i)
    {
        netraceNodeToTask.insert(std::pair<nodeID_t, int>(i + node_count, i));
        netraceTaskToNode.insert(std::pair<nodeID_t, int>(i, i + node_count));
    }
#endif

    readNodeTypes(noc_node);
    readNodes(noc_node);
    readConnections(noc_node);
    if (!RoutingTable_mode)
    {
        fillDirInfoOfNodeConn();
    }
    else
    {
        fillDirInfoOfNodeConn_DM();
    }
}

void GlobalResources::readNodeTypes(const pugi::xml_node &noc_node)
{
    for (pugi::xml_node node : noc_node.child("nodeTypes").children("nodeType"))
    {
        dataTypeID_t typeID = node.attribute("id").as_int();
        std::string model = node.child("model").attribute("value").as_string();
        if (model.empty())
            cout << "Model not set properly for nodeType with id " << typeID << endl;
        std::string routing = node.child("routing").attribute("value").as_string();
        std::string selection = node.child("selection").attribute("value").as_string();
        int clkDelay = node.child("clockDelay").attribute("value").as_int();
        std::string arbiterType = node.child("arbiterType").attribute("value").as_string();
        auto nodeType = std::make_shared<NodeType>(typeID, model, routing, selection, clkDelay, arbiterType);
        nodeTypes.emplace_back(nodeType);
    }
}

void GlobalResources::readNodes(const pugi::xml_node &noc_node)
{
    for (pugi::xml_node xmlnode : noc_node.child("nodes").children("node"))
    {
        nodeID_t nodeID = xmlnode.attribute("id").as_int();
        std::string name = "node_" + std::to_string(nodeID);
        float x = xmlnode.child("xPos").attribute("value").as_float();
        float y = xmlnode.child("yPos").attribute("value").as_float();
        float z = xmlnode.child("zPos").attribute("value").as_float();
        xPositions.push_back(x);
        yPositions.push_back(y);
        zPositions.push_back(z);
        nodeTypeID_t nodeTypeID = xmlnode.child("nodeType").attribute("value").as_int();
        std::shared_ptr<NodeType> nodeType = nodeTypes.at(nodeTypeID);
        int layer = xmlnode.child("layer").attribute("value").as_int();
        nodes.emplace_back(nodeID, Vec3D<float>(x, y, z), nodeType, layer);
    }
    sortNodesPositions();
}

void GlobalResources::sortNodesPositions()
{
    sort(xPositions.begin(), xPositions.end());
    xPositions.erase(unique(xPositions.begin(), xPositions.end()), xPositions.end());

    sort(yPositions.begin(), yPositions.end());
    yPositions.erase(unique(yPositions.begin(), yPositions.end()), yPositions.end());

    sort(zPositions.begin(), zPositions.end());
    zPositions.erase(unique(zPositions.begin(), zPositions.end()), zPositions.end());
}

void GlobalResources::fillDirInfoOfNodeConn()
{
    for (Node &node : nodes)
    {
        //check for common directions
        Vec3D<float> distance{};
        for (int connectedNodeID : node.connectedNodes)
        {
            Node connectedNode = nodes.at(connectedNodeID);
            distance = node.pos - connectedNode.pos;
            connID_t matching_conn = node.getConnWithNode(connectedNode);
            DIR::TYPE dir{};
            if (distance.isZero())
            { //no axis differs
                dir = DIR::Local;
            }
            else
            { //one axis differs
                int layer = norm_to_coord[node.pos].z;
                if (routingCircular && x_range[layer].size() > 2 && (node.pos.x == 0. && connectedNode.pos.x == 1. && node.pos.y == connectedNode.pos.y && node.pos.z == connectedNode.pos.z))
                {
                    dir = DIR::West;
                }
                else if (routingCircular && x_range[layer].size() > 2 && (node.pos.x == 1. && connectedNode.pos.x == 0. && node.pos.y == connectedNode.pos.y && node.pos.z == connectedNode.pos.z))
                {
                    dir = DIR::East;
                }
                else if (routingCircular && y_range[layer].size() > 2 && (node.pos.y == 0. && connectedNode.pos.y == 1. && node.pos.x == connectedNode.pos.x && node.pos.z == connectedNode.pos.z))
                {
                    dir = DIR::South;
                }
                else if (routingCircular && y_range[layer].size() > 2 && (node.pos.y == 1. && connectedNode.pos.y == 0. && node.pos.x == connectedNode.pos.x && node.pos.z == connectedNode.pos.z))
                {
                    dir = DIR::North;
                }
                else if (routingCircular && z_range.size() > 2 && (node.pos.z == 0. && connectedNode.pos.z == 1. && node.pos.x == connectedNode.pos.x && node.pos.y == connectedNode.pos.y))
                {
                    dir = DIR::Down;
                }
                else if (routingCircular && z_range.size() > 2 && (node.pos.z == 1. && connectedNode.pos.z == 0. && node.pos.x == connectedNode.pos.x && node.pos.y == connectedNode.pos.y))
                {
                    dir = DIR::Up;
                }
                else if (distance.x > 0)
                {
                    dir = DIR::West;
                }
                else if (distance.x < 0)
                {
                    dir = DIR::East;
                }
                else if (distance.y < 0)
                {
                    dir = DIR::North;
                }
                else if (distance.y > 0)
                {
                    dir = DIR::South;
                }
                else if (distance.z < 0)
                {
                    dir = DIR::Up;
                }
                else if (distance.z > 0)
                {
                    dir = DIR::Down;
                }
            }
            node.setConPosOfDir(dir, matching_conn);
            node.setDirOfConn(matching_conn, dir);
        }
    }
}

// My fillDirInfoOfNodeConn() function. Rely on Direction_Mat.txt file

void GlobalResources::fillDirInfoOfNodeConn_DM()
{
    std::cout << "Start filling direction of nodes \n";

    std::vector<std::vector<int>> directions_mat;
    std::ifstream infile(DirectionMat_file);
    std::string line;

    int cpt_line = 0;

    while (std::getline(infile, line))
    {
        std::istringstream iss(line);
        std::string word;
        int DIR;
        std::vector<int> v;
        while (iss >> word)
        {
            DIR = std::stoi(word);
            v.push_back(DIR);
        }

        directions_mat.push_back(v);
        cpt_line += 1;
    }
    std::cout << "Direction Matrix :" << std::endl;

    for (auto vec : directions_mat)
    {
        for (auto x : vec)
            std::cout << x << " , ";
        std::cout << std::endl;
    }

    for (Node &node : nodes)
    {
        for (int connectedNodeID : node.connectedNodes)
        {
            Node connectedNode = nodes.at(connectedNodeID);
            Vec3D<float> distance{};
            distance = node.pos - connectedNode.pos;
            connID_t matching_conn = node.getConnWithNode(connectedNode);
            DIR::TYPE dir{};
            int d;
            if (distance.isZero())
            { //no axis differs
                dir = DIR::Local;
            }
            else
            {
                d = directions_mat[node.id][connectedNode.id];
                if (d == -1)
                {
                    std::cout << "error, no direction found \n";
                }
                else
                {
                    if (d == 1)
                    {
                        dir = DIR::East;
                    }
                    else
                    {
                        if (d == 2)
                        {
                            dir = DIR::West;
                        }
                        else
                        {
                            if (d == 3)
                            {
                                dir = DIR::North;
                            }
                            else
                            {
                                if (d == 4)
                                {
                                    dir = DIR::South;
                                }
                                else
                                {
                                    if (d == 5)
                                    {
                                        dir = DIR::Up;
                                    }
                                    else
                                    {
                                        if (d == 6)
                                        {
                                            dir = DIR::Down;
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
            node.setConPosOfDir(dir, matching_conn);
            node.setDirOfConn(matching_conn, dir);
        }
    }
}

void GlobalResources::readConnections(const pugi::xml_node &noc_node)
{
    for (pugi::xml_node xml_con : noc_node.child("connections").children("con"))
    {
        connID_t connID = xml_con.attribute("id").as_int();
        std::vector<nodeID_t> nodesOfConnection{};
        std::vector<int> vcsCount{};
        std::vector<int> buffersDepth{};
        std::vector<std::vector<int>> buffersDepths{};

        for (pugi::xml_node xml_port : xml_con.child("ports").children("port"))
        {
            nodeID_t connectedNodeID = xml_port.child("node").attribute("value").as_int();
            nodesOfConnection.push_back(connectedNodeID);
            int vcCount = xml_port.child("vcCount").attribute("value").as_int();
            vcsCount.push_back(vcCount);
            buffersDepth.push_back(xml_port.child("bufferDepth").attribute("value").as_int());

            if (bufferDepthType == "perVC")
            {
                std::string str_vec = xml_port.child("buffersDepths").attribute("value").as_string();
                std::vector<std::string> strings = string_split(str_vec, ",");
                if (strings.size() != vcCount)
                {
                    FATAL("The buffersDepths size is not equal to vcCount!");
                }
                std::vector<int> bd = strs_to_ints(strings);
                buffersDepths.push_back(bd);
            }
        }

        assert(nodesOfConnection.size() == 2); // make sure each connection only consists 2 nodes

        float length = xml_con.child("length").attribute("value").as_float();
        int depth = xml_con.child("depth").attribute("value").as_int();
        int width = xml_con.child("width").attribute("value").as_int();

        Connection con = Connection(connID, nodesOfConnection, vcsCount, buffersDepth, buffersDepths, length, width,
                                    depth);

        int nodesSize = nodesOfConnection.size();
        con.bufferUtilization.resize(nodesSize);
        for (nodeID_t nID : con.nodes)
        {
            // Now that we know the nodes of each connection, we should reflect this info to the nodes themselves.
            for (nodeID_t dstNodeID : con.nodes)
            {
                if (nID != dstNodeID)
                    nodes.at(nID).connectedNodes.push_back(dstNodeID);
            }
            nodes.at(nID).connections.push_back(con.id);
        }

        connections.push_back(con);
    }
}

void GlobalResources::readTaskAndMapFiles(const std::string &taskFilePath, const std::string &mappingFilePath)
{
    std::map<taskID_t, nodeID_t> bindings = readMappingFile(mappingFilePath);
    readTaskFile(taskFilePath, bindings);
}

std::map<taskID_t, nodeID_t> GlobalResources::readMappingFile(const std::string &mappingFilePath)
{
    std::cout << "Reading Mapping config: " << mappingFilePath << endl;
    pugi::xml_document doc;
    pugi::xml_parse_result result = doc.load_file(mappingFilePath.c_str());
    assert(result && "Failed to read Mapping file!");

    pugi::xml_node map_node = doc.child("map");
    std::map<taskID_t, nodeID_t> bindings;
    for (pugi::xml_node bind_node : map_node.children("bind"))
    {
        int taskID = readRequiredIntAttribute(bind_node, "task", "value");
        int nodeID = readRequiredIntAttribute(bind_node, "node", "value");
        bindings.emplace(taskID, nodeID);
    }
    return bindings;
}

void GlobalResources::readTaskFile(const std::string &taskFilePath, const std::map<int, int> &bindings)
{
    //TODO the bindings vector was used to get the destination node and not task, of a current task.. keep it?
    std::cout << "Reading Data config: " << taskFilePath << endl;
    pugi::xml_document doc;
    pugi::xml_parse_result result = doc.load_file(taskFilePath.c_str());
    assert(result && "Failed to read Data file!");
    pugi::xml_node data_node = doc.child("data");

    // Read Data Types
    numberOfTrafficTypes = static_cast<int>(data_node.child("dataTypes").select_nodes("dataType").size());
    for (pugi::xml_node type_node : data_node.child("dataTypes").children("dataType"))
    {
        dataTypeID_t dataTypeID = readRequiredIntAttribute(type_node, "id");
        std::string name = readRequiredStringAttribute(type_node, "name", "value");
        dataTypes.emplace_back(dataTypeID, name);
    }
    // Read Tasks
    for (pugi::xml_node task_node : data_node.child("tasks").children("task"))
    {
        taskID_t taskID = task_node.attribute("id").as_int();
        Task task = Task{taskID, bindings.at(taskID)};
        readAttributeIfExists(task_node, "start", "min", task.minStart);
        readAttributeIfExists(task_node, "start", "max", task.maxStart);
        readAttributeIfExists(task_node, "duration", "min", task.minDuration);
        readAttributeIfExists(task_node, "duration", "max", task.maxDuration);
        readAttributeIfExists(task_node, "repeat", "min", task.minRepeat);
        readAttributeIfExists(task_node, "repeat", "max", task.maxRepeat);

        // Read Requirements
        std::vector<DataRequirement> requirements{};
        for (pugi::xml_node requirement_node : task_node.child("requires").children("requirement"))
        {
            dataReqID_t reqID = readRequiredIntAttribute(requirement_node, "id");
            dataTypeID_t typeID = readRequiredIntAttribute(requirement_node, "type", "value");
            DataRequirement req = DataRequirement{reqID, typeID};
            readAttributeIfExists(requirement_node, "count", "min", req.minCount);
            readAttributeIfExists(requirement_node, "count", "max", req.maxCount);
            requirements.push_back(req);
        }
        task.requirements = requirements;

        // Read Destinations
        std::vector<DataSendPossibility> possibilities{};
        for (pugi::xml_node generate_node : task_node.child("generates").children("possibility"))
        {
            possID_t possID = readRequiredIntAttribute(generate_node, "id");
            float probability = readRequiredFloatAttribute(generate_node, "probability", "value");
            std::vector<DataDestination> destinations{};
            for (pugi::xml_node destination_node : generate_node.child("destinations").children("destination"))
            {
                dataDestID_t dataDestID = readRequiredIntAttribute(destination_node, "id");
                dataTypeID_t typeID = readRequiredIntAttribute(destination_node, "type", "value");
                taskID_t destTaskID = readRequiredIntAttribute(destination_node, "task", "value");
                int minInterval = readRequiredIntAttribute(destination_node, "interval", "min");
                int maxInterval = readRequiredIntAttribute(destination_node, "interval", "max");
                DataDestination dataDestination = DataDestination{dataDestID, typeID, destTaskID, minInterval,
                                                                  maxInterval};

                readAttributeIfExists(destination_node, "count", "min", dataDestination.minCount);
                readAttributeIfExists(destination_node, "count", "max", dataDestination.maxCount);
                readAttributeIfExists(destination_node, "delay", "min", dataDestination.minDelay);
                readAttributeIfExists(destination_node, "delay", "max", dataDestination.maxDelay);

                destinations.push_back(dataDestination);
            }
            possibilities.emplace_back(possID, probability, destinations);
        }
        task.possibilities = possibilities;

        tasks.push_back(task);
    }
}

std::vector<Node *> GlobalResources::getNodesByPos(const Vec3D<float> &pos)
{
    std::vector<Node *> matching_nodes{};
    for (auto &node : nodes)
        if (node.getNodeByPos(pos))
        {
            matching_nodes.push_back(&node);
        }
    return matching_nodes;
}

GlobalResources::~GlobalResources()
{
    delete rand;
}
