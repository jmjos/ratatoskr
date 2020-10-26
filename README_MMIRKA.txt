##############################################
#               26/10/2020                   #
#                                            #
#  This file described the different changes #
#  proposed.                                 #
#  Each modified / added files are depicted. #
#                                            #
#  Author: Maxime Mirka                      #
##############################################

Proposed changes:

1. Table routing:

--> simulator/src/model/router/routings/ : 
	. added TableRouting.h and TableRouting.cpp
	
--> simulator/src/model/router/Router.cpp : 
	. modified to accept table routing
	
--> simulator/src/utils/Globalresources.cpp : 
	. modified readConfigFile() function.
	. Added routing table option to the Routing_node. 
	. Relies on two parameters of the config.ini file: "routingTable_mode" = boolean (i.e. on/off), and "routingTable_path" = file path to a text file containing the routing table. 
	. Calls "createRoutingTable()" function: read the routing table file abd initialize the Routingtable attribute.
	
	
--> bin/config.ini : 
	. takes two new arguments: RoutingTable_mode (boolean) and RoutingTable (filepath), see current config.ini file.
	. the argument "routing" can take the value "Table"
	
	
2. "Universal" routing:

Implemented the improved Up*/Down* presented in https://www.researchgate.net/publication/221274418_A_New_Methodology_to_Compute_Deadlock-Free_Routing_Tables_for_Irregular_Networks 
"A New Methodology to Compute Deadlock-Free Routing Tables for Irregular Networks", José Carlos Sanchos et al.  

--> bin/Create_RoutingTable.py :
	. Python script that creates a text file containing a routing table. These routing table is created following the improved Up/Down algorithm presented in the paper above.
	


3. Allows crossing connections:

In order to simulate any topology of NoC, I implemented a way to set directions to link, not based on routers positions.

--> bin/Create_RoutingTable.py:
	. Compute_directions():assign a direction at each connections
	. create texte file "Direction_Mat.txt": contain a matrix describing the direction of each links.
	
--> simulator/src/utils/Globalresources.cpp:
	. GlobalResources::fillDirInfoOfNodeConn() : changed to read the direction from "Direction_Mat.txt"
