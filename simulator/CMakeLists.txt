CMAKE_MINIMUM_REQUIRED(VERSION 2.8)
PROJECT(sim)

SET(CMAKE_PREFIX_PATH /usr/local/systemc-2.3.3)

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -std=c++14 -O3")

INCLUDE_DIRECTORIES(${CMAKE_PREFIX_PATH}/include)
INCLUDE_DIRECTORIES(./src)

FIND_LIBRARY(systemc systemc ${CMAKE_PREFIX_PATH}/lib-linux64)
LINK_DIRECTORIES(${CMAKE_PREFIX_PATH}/lib-linux64)

ADD_DEFINITIONS(-D SC_INCLUDE_DYNAMIC_PROCESSES)

ADD_EXECUTABLE(${PROJECT_NAME} src/model/networkInterface/NetworkInterface.cpp src/model/networkInterface/NetworkInterfaceVC.cpp src/model/NoC.cpp src/model/link/Link.cpp src/model/router/selection/Selection1stFreeVC.cpp src/model/router/selection/SelectionRoundRobin.cpp src/model/router/selection/SelectionOutputRoundRobin.cpp src/model/router/selection/SelectionAgRA.cpp src/model/router/selection/SelectionDyXYZ.cpp src/model/router/selection/SelectionEDXYZ.cpp src/model/router/selection/SelectionMAFA.cpp src/model/router/helper/Helper.cpp src/model/router/helper/MeshHelper.cpp src/model/router/Buffer.cpp src/model/router/routings/RoutingXYZ.cpp src/model/router/routings/RoutingHeteroXYZ.cpp src/model/router/routings/RoutingDPR.cpp src/model/router/routings/RoutingTMR.cpp src/model/router/routings/RoutingESPADA.cpp src/model/router/RouterVC.cpp src/model/router/Router.cpp src/model/traffic/TrafficPool.cpp src/model/traffic/Data.cpp src/model/traffic/Packet.cpp src/model/traffic/Flit.cpp src/model/traffic/synthetic/SyntheticPool.cpp src/model/traffic/task/TaskPool.cpp src/model/processingElement/ProcessingElementVC.cpp src/model/processingElement/ProcessingElement.cpp src/utils/GlobalReport.cpp src/utils/Report.cpp src/utils/Structures.cpp src/utils/Statistics.cpp src/main.cpp src/utils/GlobalResources.cpp src/utils/GlobalResources.h src/utils/PacketFactory.h src/utils/PacketFactory.cpp)

TARGET_LINK_LIBRARIES(${PROJECT_NAME} systemc pugixml)

