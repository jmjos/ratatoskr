Before running the test run "run.sh", make sure to change the following path to match your own system:
- in config.ini: change the "RoutingTable" parameter.
- in Globalressource.cpp, in function GlobalResources::fillDirInfoOfNodeConn(), change the "filename" variable.

Test the set-up by running "run.sh"


The routing table option requires 2 files:
- RT.txt : text file containing the routing table
- Direction_Mat.txt: text file containing the connections directions. Used in  /simulator/src/GlobalResources.cpp, in funtion "GlobalResources::fillDirInfoOfNodeConn()"

To use the routing table option, set the parameter routingTable_mode = 1 in config.ini. Then specify the path to RT.txt and Direction_Mat.txt.

// CONFIG

Propose a 4 router NoC. Links crossed in the middle (see plot when run.sh is ran).
--> old direction allocation (old commented GlobalResources::fillDirInfoOfNodeConn()) led to an error
--> From Direction_Mat.txt, there is no ambiguity between links directions.


