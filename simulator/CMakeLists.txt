CMAKE_MINIMUM_REQUIRED(VERSION 2.8)
PROJECT(sim)

SET(CMAKE_PREFIX_PATH /usr/local/systemc-2.3.3)

set(CMAKE_CXX_STANDARD 14)

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -O3")

INCLUDE_DIRECTORIES(${CMAKE_PREFIX_PATH}/include)
INCLUDE_DIRECTORIES(./src)

FIND_LIBRARY(systemc systemc ${CMAKE_PREFIX_PATH}/lib-linux64)
LINK_DIRECTORIES(${CMAKE_PREFIX_PATH}/lib-linux64)

ADD_DEFINITIONS(-D SC_INCLUDE_DYNAMIC_PROCESSES)

ADD_EXECUTABLE(${PROJECT_NAME} src/utils/Report.cpp src/utils/Structures.cpp src/utils/PacketFactory.cpp  src/utils/GlobalResources.cpp src/utils/Statistics.cpp  src/utils/GlobalReport.cpp src/main.cpp  src/model/processingElement/ProcessingElement.cpp src/model/processingElement/ProcessingElementVC.cpp src/model/networkInterface/NetworkInterfaceVC.cpp  src/model/networkInterface/NetworkInterface.cpp src/model/traffic/Packet.cpp src/model/traffic/Flit.cpp src/model/traffic/synthetic/SyntheticPool.cpp src/model/traffic/task/TaskPool.cpp src/model/traffic/TrafficPool.cpp src/model/router/RouterVC.cpp src/model/router/Buffer.cpp src/model/router/Router.cpp src/model/router/NewRouterVC.cpp src/model/NoC.cpp src/model/link/Link.cpp )

TARGET_LINK_LIBRARIES(${PROJECT_NAME} systemc pugixml)

