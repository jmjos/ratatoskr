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
#include <map>
#include <unistd.h>
#include <random>
#include <sys/stat.h>

#include "Structures.h"
#include "pugixml.hpp"

// my includes
#include <cmath>
#include <list>
#include <fstream>
#include <iostream>

class GlobalResources
{

public:
    std::vector<float> xPositions;
    std::vector<float> yPositions;
    std::vector<float> zPositions;
    //General
    int simulation_time;
    bool outputToFile;
    bool activateFlitTracing;
    std::string outputFileName;
    std::string outputDirectory;
    std::string noc_file;
    std::string topology;         // store the topology of the network (mesh, torus, ring)
    bool routingCircular = false; // decide whether the routing algorithm is routed in a circular way (for torus and ring)
    int flitsPerPacket = 0;
    int bitWidth = 32;
    float routingVerticalThreshold = 1.0f;
    float Vdd = 1.0f;
    std::string bufferDepthType;
    std::string RoutingTable_file;
    bool RoutingTable_mode;
    std::vector<std::vector<int>> RoutingTable;
    std::string DirectionMat_file;
    //Application
    std::string benchmark;
    std::string data_file;
    std::string map_file;
    std::string simulation_file;
    std::string mapping_file;
    std::string netraceFile;
    int netraceStartRegion;
#ifdef ENABLE_NETRACE
    std::map<nodeID_t, int> netraceNodeToTask;
    std::map<int, nodeID_t> netraceTaskToNode;
    bool netrace2Dor3Dmode = true; // true == 2D
    int netraceVerbosity = 2;      // 2==all, 1 == base, 0 == none
#endif
    bool isUniform;
    int numberOfTrafficTypes;
    int synthetic_start_measurement_time;
    // General NoC data
    std::vector<std::shared_ptr<NodeType>> nodeTypes;
    std::vector<Node> nodes;
    std::vector<Connection> connections;
    std::vector<Task> tasks;
    std::vector<DataType> dataTypes;
    std::vector<SyntheticPhase> syntheticPhases;

    long long rd_seed;
    std::mt19937_64 *rand;

    //debug
    bool routingDebugMode = false;

    static GlobalResources &getInstance();

    int getRandomIntBetween(int, int);

    float getRandomFloatBetween(float, float);

    void readConfigFile(const std::string &configPath);

    void readNoCLayout(const std::string &nocPath);

    void readTaskAndMapFiles(const std::string &taskFilePath, const std::string &mappingFilePath);

    std::vector<Node *> getNodesByPos(const Vec3D<float> &pos);

private:
    GlobalResources();

    ~GlobalResources();

    std::vector<std::string> string_split(const std::string &str, const std::string &delim);

    std::vector<int> strs_to_ints(const std::vector<std::string> &strings);

    void readNodeTypes(const pugi::xml_node &noc_node);

    void readNodes(const pugi::xml_node &noc_node);

    void sortNodesPositions();

    void fillDirInfoOfNodeConn();

    void fillDirInfoOfNodeConn_DM();

    void readConnections(const pugi::xml_node &noc_node);

    void readAttributeIfExists(pugi::xml_node, const char *, const char *, int &);

    void readAttributeIfExists(pugi::xml_node, const char *, int &);

    void readTaskFile(const std::string &taskFilePath, const std::map<int, int> &bindings);

    std::map<int, int> readMappingFile(const std::string &mappingFilePath);

    std::string readRequiredStringAttribute(pugi::xml_node, const char *, const char *);

    std::string readRequiredStringAttribute(pugi::xml_node, const char *);

    int readRequiredIntAttribute(pugi::xml_node, const char *, const char *);

    int readRequiredIntAttribute(pugi::xml_node, const char *);

    float readRequiredFloatAttribute(pugi::xml_node, const char *, const char *);

    float readRequiredFloatAttribute(pugi::xml_node, const char *);

    void createRoutingTable();
};
