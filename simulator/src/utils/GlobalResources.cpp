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
        :
        simulation_time(0),
        outputToFile(false),
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

GlobalResources& GlobalResources::getInstance()
{
    static GlobalResources instance;
    //std::cout<<"\n\n\nInstance created\n\n\n"<<std::endl;
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

void GlobalResources::readAttributeIfExists(pugi::xml_node node, const char* child, const char* attribute, int& var)
{
    readAttributeIfExists(node.child(child), attribute, var);
}

void GlobalResources::readAttributeIfExists(pugi::xml_node node, const char* attribute, int& var)
{
    if (!node.attribute(attribute).empty()) {
        var = node.attribute(attribute).as_int();
    }
}

int GlobalResources::readRequiredIntAttribute(pugi::xml_node node, const char* child, const char* attribute)
{
    return readRequiredIntAttribute(node.child(child), attribute);
}

int GlobalResources::readRequiredIntAttribute(pugi::xml_node node, const char* attribute)
{
    if (node.attribute(attribute).empty()) {
        FATAL("Can not read node:" << node.path() << " " << attribute);
    }
    return node.attribute(attribute).as_int();
}

float GlobalResources::readRequiredFloatAttribute(pugi::xml_node node, const char* child, const char* attribute)
{
    return readRequiredFloatAttribute(node.child(child), attribute);
}

float GlobalResources::readRequiredFloatAttribute(pugi::xml_node node, const char* attribute)
{
    if (node.attribute(attribute).empty()) {
        FATAL("Can not read node:" << node.path() << " " << attribute);
    }
    return node.attribute(attribute).as_float();
}

std::string
GlobalResources::readRequiredStringAttribute(pugi::xml_node node, const char* child, const char* attribute)
{
    return readRequiredStringAttribute(node.child(child), attribute);
}

std::string GlobalResources::readRequiredStringAttribute(pugi::xml_node node, const char* attribute)
{
    if (node.attribute(attribute).empty()) {
        FATAL("Can not read node:" << node.path() << " " << attribute);
    }
    return node.attribute(attribute).as_string();
}

std::vector<std::string> GlobalResources::string_split(const std::string& str, const std::string& delim)
{
    std::vector<std::string> strings;
    auto start = 0U;
    auto end = str.find(delim);
    while (end!=std::string::npos) {
        strings.push_back(str.substr(start, end-start));
        start = static_cast<unsigned int>(end+1);
        end = str.find(delim, start);
    }
    strings.push_back(str.substr(start, end));
    return strings;
}

std::vector<int> GlobalResources::strs_to_ints(const std::vector<std::string>& strings)
{
    std::vector<int> ints{};
    for (auto& str: strings) {
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
        while(iss >> word) {
            DIR = std::stoi(word);
            v.push_back(DIR);
        }
        
        RoutingTable.push_back(v);
        cpt_line += 1;
    }
    std::cout<<"Routing Table :"<< std::endl;

    for(auto vec : RoutingTable)
    {
        for(auto x : vec)
            std::cout<<x<<" , ";
        std::cout << std::endl;
    }
}



void GlobalResources::readConfigFile(const std::string& configPath)
{
    pugi::xml_document doc;
    //std::cout<<"\n\n\nInstance created\n\n\n"<<std::endl;
    std::cout << "Reading simulator config: " << configPath << endl;
    pugi::xml_parse_result result = doc.load_file(configPath.c_str());
    assert(result && "Failed to read simulator config file!");

    //GENERAL
    pugi::xml_node gen_node = doc.child("configuration").child("general");
    simulation_time = gen_node.child("simulationTime").attribute("value").as_int();
    outputToFile = gen_node.child("outputToFile").attribute("value").as_bool();
    outputFileName = gen_node.child("outputToFile").child_value();

    //ROUTING TABLE
    pugi::xml_node Routing_node = doc.child("configuration").child("verbose").child("router");
    RoutingTable_mode = Routing_node.child("routingTable_mode").attribute("value").as_bool();
    
    std::cout<<"Routing table mode: "<<RoutingTable_mode<<std::endl;
    
    if (RoutingTable_mode){
        pugi::xml_node RTFile_node = Routing_node.child("routingTable_path");
        RoutingTable_file = readRequiredStringAttribute(RTFile_node, "value");
        createRoutingTable();
    }

    //NOC
    pugi::xml_node noc_node = doc.child("configuration").child("noc");
    noc_file = noc_node.child_value("nocFile");
    flitsPerPacket = noc_node.child("flitsPerPacket").attribute("value").as_int();
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
    for (pugi::xml_node phase_node : app_node.child("synthetic").children("phase")) {
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
        if (benchmark=="synthetic" && name!="warmup" &&
                synthetic_start_measurement_time==-1) {
            synthetic_start_measurement_time = sp.minStart;
        }
        syntheticPhases.push_back(sp);
    }
}

void GlobalResources::readNoCLayout(const std::string& nocPath)
{
    std::cout << "Reading NoC layout: " << nocPath << endl;
    pugi::xml_document doc;
    pugi::xml_parse_result result = doc.load_file(nocPath.c_str());
    assert(result && "Failed to read NoC file!");

    pugi::xml_node noc_node = doc.child("network-on-chip");
    bufferDepthType = noc_node.child("bufferDepthType").attribute("value").as_string();
    if (!bufferDepthType.empty() && (bufferDepthType!="single" && bufferDepthType!="perVC")) {
        FATAL("The value of bufferDepthType in your network file should be either 'single' or 'perVC'!");
    }

    readNodeTypes(noc_node);
    readNodes(noc_node);
    readConnections(noc_node);
    fillDirInfoOfNodeConn();
}

void GlobalResources::readNodeTypes(const pugi::xml_node& noc_node)
{
    for (pugi::xml_node node : noc_node.child("nodeTypes").children("nodeType")) {
        dataTypeID_t typeID = node.attribute("id").as_int();
        std::string model = node.child("model").attribute("value").as_string();
        if (model.empty()) cout << "Model not set properly for nodeType with id " << typeID << endl;
        std::string routing = node.child("routing").attribute("value").as_string();
        std::string selection = node.child("selection").attribute("value").as_string();
        int clkDelay = node.child("clockDelay").attribute("value").as_int();
        std::string arbiterType = node.child("arbiterType").attribute("value").as_string();
        auto nodeType = std::make_shared<NodeType>(typeID, model, routing, selection, clkDelay, arbiterType);
        nodeTypes.emplace_back(nodeType);
    }
}

void GlobalResources::readNodes(const pugi::xml_node& noc_node)
{
    for (pugi::xml_node xmlnode : noc_node.child("nodes").children("node")) {
        nodeID_t nodeID = xmlnode.attribute("id").as_int();
        std::string name = "node_"+std::to_string(nodeID);
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
/*
void GlobalResources::fillDirInfoOfNodeConn()
{
    for (Node& node : nodes) {
        //check for common directions
        Vec3D<float> distance{};
        for (int connectedNodeID : node.connectedNodes) {
            Node connectedNode = nodes.at(connectedNodeID);
            distance = node.pos-connectedNode.pos;
            connID_t matching_conn = node.getConnWithNode(connectedNode);
            DIR::TYPE dir{};
            if (distance.isZero()) { //no axis differs
                dir = DIR::Local;
            }
            else { //one axis differs
                if (distance.x>0) {
                    dir = DIR::West;
                }
                else if (distance.x<0) {
                    dir = DIR::East;
                }
                else if (distance.y<0) {
                    dir = DIR::North;
                }
                else if (distance.y>0) {
                    dir = DIR::South;
                }
                else if (distance.z<0) {
                    dir = DIR::Up;
                }
                else if (distance.z>0) {
                    dir = DIR::Down;
                }
            }
            node.setConPosOfDir(dir, matching_conn);
            node.setDirOfConn(matching_conn, dir);
            std::cout << "Router " << node.id << " to " << matching_conn << ": Dir = " << dir << "\n";
        }
    }
}
*/
float computeAngle(Vec3D<float> distance)
{
	float hypo;
	float dx, dy, angle, adx;
	dx = distance.x;
	dy = distance.y;
	adx = abs(dx);
	hypo = sqrt(pow(dx, 2)+pow(dy, 2));
	float pi = (float)M_PI;
   
	if (dx>=0){
		if (dy>=0){
			angle = asin(adx/hypo);
		}
		else {
			angle = pi - asin(adx/hypo);
		}
	}
	else {
		if (dy>=0){
			angle = 2*pi - asin(adx/hypo);
		}
		else {
			angle = pi + asin(adx/hypo);
		}
	}

	return angle;
}

bool comp(float a, float b) 
{ 
    return (a < b); 
} 

float my_min(std::list<float> l)
{
    int size;
    size = l.size();
    int i;
    int id_min;
    std::list<float>::iterator it_a;
    float mini;

    it_a = l.begin();
    mini = *it_a;

    for (it_a = l.begin(); it_a != l.end(); ++it_a){
        mini = std::min(mini, *it_a, comp);
    }

    return mini;
}

// My function 1.0
/*
void GlobalResources::fillDirInfoOfNodeConn()
{
    std::cout << "Start filling direction of nodes \n";
    
    float pi = (float)M_PI;
    for (Node& node : nodes) {
        //std::cout << "check 1 \n";
        //check for common directions
        Vec3D<float> distance{};
        std::list<DIR> usedDir = {};
        std::list<float> angles = {};
        std::list<int> connectedNodesIDs = {};
        int nbConn = node.connectedNodes.size();
        nbConn = nbConn - 1; // delete the local connection
	float angle;

        for (int connectedNodeID : node.connectedNodes) {
            //std::cout << "check 2 \n";
            Node connectedNode = nodes.at(connectedNodeID);
            distance = connectedNode.pos - node.pos; // old: node.pos-connectedNode.pos
            connID_t matching_conn = node.getConnWithNode(connectedNode);
            DIR::TYPE dir{};
            if (distance.isZero()) { //no axis differs
                dir = DIR::Local;
                node.setConPosOfDir(dir, matching_conn);
                node.setDirOfConn(matching_conn, dir);
                std::cout << "Router " << node.id << " to " << connectedNodeID << ": Dir = " << dir << "\n";
            }
            else { //axis differs --> get connections angles
                std::list<float>::iterator it_a;
                std::list<connID_t>::iterator it_c;

                it_a = angles.begin();
                it_c = connectedNodesIDs.begin();
                angle = computeAngle(distance);
                angles.insert(it_a,angle);
                connectedNodesIDs.insert(it_c, connectedNodeID);
            }
        }
        //std::cout << "Router " << node.id << "\n";
        //std::cout << "Angles: " << "\n";
        //for (float a : angles){
        //    std::cout << a << ", ";
        //}
        //std::cout << "\n";
        //std::cout << "Connected to: \n";
        //for (int c : connectedNodesIDs){
        //    std::cout << c << ", ";
        //}
        //std::cout << "\n";
        //std::cout << "check 3 \n";
        // Order the angles
        std::list<float> angles_tmp = angles;
        std::list<float> angles_ordered = {};
        std::list<int> connectedNodesIDs_tmp = connectedNodesIDs;
        std::list<int> connectedNodesIDs_ordered = {};
        //std::cout << "angles tmp: " << "\n";
        //for (float a : angles){
        //    std::cout << a << ", ";
        //}
        //std::cout << "\n";
        //std::cout << "check 4 \n";
        for (int i=0;i<nbConn;i++){
            // 1. find the min value in the tmp list
            float angle_min = my_min(angles_tmp);
            //std::cout << "angle min = " << angle_min << "\n";
            // 2. find the corresponding idx in both angles and connIDs
            int idx;
            std::list<float>::iterator it_a = angles_tmp.begin();
            std::list<int>::iterator it_c = connectedNodesIDs_tmp.begin();
	    int j;
            for (j=0; j<nbConn; j++){
                if(*it_a == angle_min){
                    idx = *it_c;
                    break;
                }
                else{
                    std::advance(it_a, 1);
                    std::advance(it_c, 1);
                }
            }
            //std::list<float>::iterator my_it = angles_tmp.begin();
            //advance(my_it, j);
            // 3. place the corresponding angle and ID in last position of the ordered lists
            angles_ordered.push_back(angle_min);
            connectedNodesIDs_ordered.push_back(idx);
            //std::cout << "check 6.. \n";
            // 4. delete element from the tmp lists
            //auto tmpIter = it_a;
            //std::cout << "check 8 \n";
            *it_a = 3*pi;
            //std::cout << "check 7 \n";
            //connIDs_tmp.erase(it_c);
        }
        //std::cout << "connectedNodesIDs_ordered : \n";
        //for (int c : connectedNodesIDs_ordered){
        //    std::cout << c << ", ";
        //}
        //std::cout << "\n";
        //std::cout << "angles_ordered : \n";
        //for (float a : angles_ordered){
        //    std::cout << a << ", ";
        //}
        std::cout << "\n";
        //std::cout << "check 5 \n";
        DIR::TYPE dir{};
        DIR::TYPE dir_prev{};
        // if number of connection > 0, continue
        if (nbConn > 0) {
            if (nbConn == 4) { // N,E,S,W
                std::cout << "nbconn = 4 --> " << nbConn << "\n";
                std::list<int>::iterator it = connectedNodesIDs_ordered.begin();
                for (int i=0;i<nbConn;i++){
                    int connectedNodeID = *it;
                    if (i==0){
                        dir = DIR::North;
                    }
                    else if (i==1){
                        dir = DIR::East;
                    }
                    else if (i==2){
                        dir = DIR::South;
                    }
                    else {
                        dir = DIR::West;
                    }
                    Node connectedNode = nodes.at(connectedNodeID);
                    connID_t matching_conn = node.getConnWithNode(connectedNode);
                    node.setConPosOfDir(dir, matching_conn);
                    node.setDirOfConn(matching_conn, dir);
                    std::cout << "Router " << node.id << " to " << connectedNode.id << ": Dir = " << dir << "\n";
                    std::advance(it, 1);
                }
            }
            else if (nbConn == 3) {
                std::cout << "nbconn = 3 --> " << nbConn << "\n";
                // min angle get its closer direction, then rotate N,E,S,W,N,E,...
                std::list<int>::iterator it = connectedNodesIDs_ordered.begin();
                std::list<float>::iterator it_a = angles_ordered.begin();

                int connectedNodeID = *it;
                float min_a = *it_a;
                float dN, dE, dS, dW;
                dN = min_a;
                dE = abs(min_a - pi/2);
                dS = abs(min_a - pi);
                dW = abs(min_a - 3*pi/2);
                
                float dmin = std::min({dN,dE,dS,dW}, comp);
                if (dN == dmin){
                    dir = DIR::North;

                }else if (dE == dmin){
                    dir = DIR::East;

                }else if (dS == dmin){
                    dir = DIR::South;

                }else{
                    dir = DIR::West;
                }
                dir_prev = dir;

                Node connectedNode = nodes.at(connectedNodeID);
                connID_t matching_conn = node.getConnWithNode(connectedNode);
                node.setConPosOfDir(dir, matching_conn);
                node.setDirOfConn(matching_conn, dir);
                std::cout << "Router " << node.id << " to " << connectedNode.id << ": Dir = " << dir << "\n";
                std::advance(it, 1); 

                for (int i=0; i<2; i++){
                    int connectedNodeID = *it;
                    if (dir_prev == DIR::North){
                        dir = DIR::East;
                    }
                    else if (dir_prev == DIR::East){
                        dir = DIR::South;
                    }
                    else if (dir_prev == DIR::South){
                        dir = DIR::West;
                    }
                    else {
                        dir = DIR::North;
                    }
                    dir_prev = dir;

                    Node connectedNode = nodes.at(connectedNodeID);
                    connID_t matching_conn = node.getConnWithNode(connectedNode);
                    node.setConPosOfDir(dir, matching_conn);
                    node.setDirOfConn(matching_conn, dir);
                    std::cout << "Router " << node.id << " to " << connectedNode.id << ": Dir = " << dir << "\n";
                    std::advance(it, 1);
                }
            }
            else {
                std::cout << "nbconn <= 2 --> " << nbConn << "\n";
                // each connection gets its closer direction among the available one 
                int n,e,s,w;
                n=0; e=0; s=0; w=0;
                std::list<int>::iterator it = connectedNodesIDs_ordered.begin();
                std::list<float>::iterator it_a = angles_ordered.begin();
                for (int i=0;i<nbConn;i++){
                    int connectedNodeID = *it;
                    //std::cout << "connectedNodeID: " << connectedNodeID << "\n";
                    float min_a = *it_a;
                    int dN, dE, dS, dW;
                    dN = min_a*100000;
                    dE = abs(min_a - pi/2)*100000;
                    dS = abs(min_a - pi)*100000;
                    dW = abs(min_a - 3*pi/2)*100000;
                    std::cout << "dN = " << dN << ", dE = " << dE << ", dS = " << std::setprecision(8) << dS << ", dW = " << std::setprecision(8) << dW << "\n";
                    bool test = (dS == dW);
                    std::cout << test;
                    float dmin = std::min({dN,dE,dS,dW}, comp);
                    if (dN == dmin){
                        if (n == 1){
                            dmin = std::min({dE,dS,dW}, comp);
                            if (dE == dmin){
                                dir = DIR::East;
                            }else if (dS == dmin){
                                dir = DIR::South;
                            }else{
                                dir = DIR::West;
                            }
                        }
                        else{
                            dir = DIR::North;
                            n = 1;
                        }
                    }else if (dE == dmin){
                        if (e == 1){
                            dmin = std::min({dN,dS,dW}, comp);
                            if (dN == dmin){
                                dir = DIR::North;
                            }else if (dS == dmin){
                                dir = DIR::South;
                            }else{
                                dir = DIR::West;
                            }
                        }else{
                            dir = DIR::East;
                            e = 1;
                        }
                    }else if (dS == dmin){
                        if (s == 1){
                            dmin = std::min({dE,dN,dW}, comp);
                            if (dE == dmin){
                                dir = DIR::East;
                            }else if (dN == dmin){
                                dir = DIR::North;
                            }else{
                                dir = DIR::West;
                            }
                        }else{
                            dir = DIR::South;
                            s = 1;
                        }
                    }else{
                        if (w == 1){
                            dmin = std::min({dE,dS,dN}, comp);
                            if (dE == dmin){
                                dir = DIR::East;
                            }else if (dS == dmin){
                                dir = DIR::South;
                            }else{
                                dir = DIR::North;
                            }
                        }else{
                            dir = DIR::West;
                            w = 1;
                        }
                    }
                    dir_prev = dir;
                    //std::cout << "check 6";
                    Node connectedNode = nodes.at(connectedNodeID);
                    connID_t matching_conn = node.getConnWithNode(connectedNode);
                    node.setConPosOfDir(dir, matching_conn);
                    node.setDirOfConn(matching_conn, dir);
                    std::cout << "Router " << node.id << " to " << connectedNode.id << ": Dir = " << dir << "\n";
                    std::advance(it, 1);
                    std::advance(it_a, 1);
                    
                }
            }
            // print connection by connection
        }
    }
}
*/

// My function 2.0

void GlobalResources::fillDirInfoOfNodeConn()
{
    std::cout << "Start filling direction of nodes \n";

    std::string filename;
    filename = "/home/demo/ratagan/Ratatoskr/bin/Direction_Mat.txt";
    std::vector<std::vector<int>> directions_mat ;
    std::ifstream infile(filename);
    std::string line;

    int cpt_line = 0;

    while (std::getline(infile, line))
    {
        std::istringstream iss(line);
        std::string word;
        int DIR;
        std::vector<int> v;
        while(iss >> word) {
            DIR = std::stoi(word);
            v.push_back(DIR);
        }
        
        directions_mat.push_back(v);
        cpt_line += 1;
    }
    std::cout<<"Direction Matrix :"<< std::endl;

    for(auto vec : directions_mat)
    {
        for(auto x : vec)
            std::cout<<x<<" , ";
        std::cout << std::endl;
    }

    for (Node& node : nodes) {
        for (int connectedNodeID : node.connectedNodes) {
            Node connectedNode = nodes.at(connectedNodeID);
            Vec3D<float> distance{};
            distance = node.pos-connectedNode.pos;
            connID_t matching_conn = node.getConnWithNode(connectedNode);
            DIR::TYPE dir{};
            int d;
            if (distance.isZero()) { //no axis differs
                dir = DIR::Local;
            }
            else { 
                d = directions_mat[node.id][connectedNode.id];
                if (d == -1){
                    std::cout << "error, no direction found \n";
                }
                else{
                    if (d==1){
                        dir = DIR::East;
                    }
                    else{
                        if (d==2){
                            dir = DIR::West;
                        }
                        else{
                            if (d==3){
                                dir = DIR::North;
                            }
                            else{
                                if (d==4){
                                    dir = DIR::South;
                                }
                                else{
                                    if (d==5){ 
                                        dir = DIR::Up;
                                    }
                                    else{
                                        if (d==6){
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
            std::cout << "Router " << node.id << " to " << connectedNode.id << ": Dir = " << dir << "\n";
        }
    }

}


void GlobalResources::readConnections(const pugi::xml_node& noc_node)
{
    for (pugi::xml_node xml_con : noc_node.child("connections").children("con")) {
        connID_t connID = xml_con.attribute("id").as_int();
        std::vector<nodeID_t> nodesOfConnection{};
        std::vector<int> vcsCount{};
        std::vector<int> buffersDepth{};
        std::vector<std::vector<int>> buffersDepths{};

        for (pugi::xml_node xml_port : xml_con.child("ports").children("port")) {
            nodeID_t connectedNodeID = xml_port.child("node").attribute("value").as_int();
            nodesOfConnection.push_back(connectedNodeID);
            int vcCount = xml_port.child("vcCount").attribute("value").as_int();
            vcsCount.push_back(vcCount);
            buffersDepth.push_back(xml_port.child("bufferDepth").attribute("value").as_int());

            if (bufferDepthType=="perVC") {
                std::string str_vec = xml_port.child("buffersDepths").attribute("value").as_string();
                std::vector<std::string> strings = string_split(str_vec, ",");
                if (strings.size()!=vcCount) {
                    FATAL("The buffersDepths size is not equal to vcCount!");
                }
                std::vector<int> bd = strs_to_ints(strings);
                buffersDepths.push_back(bd);
            }
        }

        float length = xml_con.child("length").attribute("value").as_float();
        int depth = xml_con.child("depth").attribute("value").as_int();
        int width = xml_con.child("width").attribute("value").as_int();

        Connection con = Connection(connID, nodesOfConnection, vcsCount, buffersDepth, buffersDepths, length, width,
                depth);
        //cout << " con = " << con.id << con.nodes[0] << con.nodes[1] << endl;
        int nodesSize = nodesOfConnection.size();
        con.bufferUtilization.resize(nodesSize);
        for (nodeID_t nID : con.nodes) {
            // Now that we know the nodes of each connection, we should reflect this info to the nodes themselves.
            for (nodeID_t dstNodeID: con.nodes) {
                if (nID!=dstNodeID)
                    nodes.at(nID).connectedNodes.push_back(dstNodeID);
            }
            nodes.at(nID).connections.push_back(con.id);
        }

        connections.push_back(con);
    }
}

void GlobalResources::readTaskAndMapFiles(const std::string& taskFilePath, const std::string& mappingFilePath)
{
    std::map<taskID_t, nodeID_t> bindings = readMappingFile(mappingFilePath);
    readTaskFile(taskFilePath, bindings);
}

std::map<taskID_t, nodeID_t> GlobalResources::readMappingFile(const std::string& mappingFilePath)
{
    std::cout << "Reading Mapping config: " << mappingFilePath << endl;
    pugi::xml_document doc;
    pugi::xml_parse_result result = doc.load_file(mappingFilePath.c_str());
    assert(result && "Failed to read Mapping file!");

    pugi::xml_node map_node = doc.child("map");
    std::map<taskID_t, nodeID_t> bindings;
    for (pugi::xml_node bind_node : map_node.children("bind")) {
        int taskID = readRequiredIntAttribute(bind_node, "task", "value");
        int nodeID = readRequiredIntAttribute(bind_node, "node", "value");
        bindings.emplace(taskID, nodeID);
    }
    return bindings;
}

void GlobalResources::readTaskFile(const std::string& taskFilePath, const std::map<int, int>& bindings)
{
    //TODO the bindings vector was used to get the destination node and not task, of a current task.. keep it?
    std::cout << "Reading Data config: " << taskFilePath << endl;
    pugi::xml_document doc;
    pugi::xml_parse_result result = doc.load_file(taskFilePath.c_str());
    assert(result && "Failed to read Data file!");
    pugi::xml_node data_node = doc.child("data");

    // Read Data Types
    numberOfTrafficTypes = static_cast<int>(data_node.child("dataTypes").select_nodes("dataType").size());
    for (pugi::xml_node type_node : data_node.child("dataTypes").children("dataType")) {
        dataTypeID_t dataTypeID = readRequiredIntAttribute(type_node, "id");
        std::string name = readRequiredStringAttribute(type_node, "name", "value");
        dataTypes.emplace_back(dataTypeID, name);
    }
    // Read Tasks
    for (pugi::xml_node task_node : data_node.child("tasks").children("task")) {
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
        for (pugi::xml_node requirement_node : task_node.child("requires").children("requirement")) {
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
        for (pugi::xml_node generate_node : task_node.child("generates").children("possibility")) {
            possID_t possID = readRequiredIntAttribute(generate_node, "id");
            float probability = readRequiredFloatAttribute(generate_node, "probability", "value");
            std::vector<DataDestination> destinations{};
            for (pugi::xml_node destination_node : generate_node.child("destinations").children("destination")) {
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

std::vector<Node*> GlobalResources::getNodesByPos(const Vec3D<float>& pos)
{
    std::vector<Node*> matching_nodes{};
    for (auto& node:nodes)
        if (node.getNodeByPos(pos)) {
            matching_nodes.push_back(&node);
        }
    return matching_nodes;
}

GlobalResources::~GlobalResources()
{
    delete rand;
}