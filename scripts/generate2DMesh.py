#!/bin/python

#Copyright 2018 Jan Moritz Joseph
#
#Permission is hereby granted, free of charge, to any person obtaining a
#copy of this software and associated documentation files (the "Software"),
#to deal in the Software without restriction, including without limitation
#the rights to use, copy, modify, merge, publish, distribute, sublicense,
#and/or sell copies of the Software, and to permit persons to whom the
#Software is furnished to do so, subject to the following conditions:
#
#The above copyright notice and this permission notice shall be included
#in all copies or substantial portions of the Software.
#
#THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
#EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
#MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
#IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
#CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
#TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
#SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

# This script generates simple topology files for 2D meshes

x = 4
y = 4
bufferDepth = 4
vcs = 4

filename = "XYZ_"+str(x)+"_"+str(y)+"_1_"+str(bufferDepth)+"B_"+str(vcs)+"VC.xml"

verbose = True

def format(value):
    return "%.3f" % value

def makeConnection(file, id, srcNode, dstNode, bufferDepth, vcs):
    file.write("\t\t<con id=\""+str(id)+"\">\n")
    file.write("\t\t\t<interface value=\"0\"/>\n")
    file.write("\t\t\t<ports>\n")
    file.write("\t\t\t\t<port id =\"0\">\n")
    file.write("\t\t\t\t\t<node value=\""+str(srcNode)+"\"/>\n")
    file.write("\t\t\t\t\t<bufferDepth value=\""+str(bufferDepth)+"\"/>\n")
    file.write("\t\t\t\t\t<vcCount value=\""+str(vcs)+"\"/>\n")
    file.write("\t\t\t\t</port>\n")
    file.write("\t\t\t\t<port id =\"1\">\n")
    file.write("\t\t\t\t\t<node value=\""+str(dstNode)+"\"/>\n")
    file.write("\t\t\t\t\t<bufferDepth value=\""+str(bufferDepth)+"\"/>\n")
    file.write("\t\t\t\t\t<vcCount value=\""+str(vcs)+"\"/>\n")
    file.write("\t\t\t\t</port>\n")
    file.write("\t\t\t</ports>\n")
    file.write("\t\t</con>\n")


file = open(filename, "w")

# write header
file.write("<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n")
file.write("<network-on-chip xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xsi:noNamespaceSchemaLocation=\"layer.xsd\">\n")
file.write("<bufferDepthType value=\"single\" /> <!-- single, perVC-->\n")
file.write("\t<nodeTypes>\n")
file.write("\t\t<nodeType id=\"0\">\n")
file.write("\t\t\t<model value=\"RouterVC\"/>\n")
file.write("\t\t\t<routing value=\"XYZ\"/>\n")
file.write("\t\t\t<clockDelay value=\"1\"/>\n")
file.write("\t\t</nodeType>\n")
file.write("\t<nodeTypes>\n")
file.write("\t\t<nodeType id=\"1\">\n")
file.write("\t\t\t<model value=\"ProcessingElement\"/>\n")
file.write("\t\t\t<clockDelay value=\"1\"/>\n")
file.write("\t\t</nodeType>\n")
file.write("\t</nodeTypes>\n\n\n")

# generate node list
print("Generating nodes...")
file.write("\t<nodes>\n")
num = 0
#routers
for xval in range(x): #sven: x/y vertauscht
    for yval in range(y):
        if verbose:
            print("Generating router in at x: "+str(xval)+" y:"+str(yval))
        file.write("\t\t<node id=\""+str(num)+"\">\n")
        file.write("\t\t\t<xPos value=\""+str(format(float(xval)/(float(x-1))))+"\"/>\n")
        file.write("\t\t\t<yPos value=\""+str(format(float(yval)/(float(y-1))))+"\"/>\n")
        file.write("\t\t\t<zPos value=\""+str(format(float(0)/(float(1))))+"\"/>\n")
        file.write("\t\t\t<nodeType value=\"0\"/>\n")
        file.write("\t\t\t<idType value=\""+str(num)+"\"/>\n")
        file.write("\t\t\t<layerType value=\""+str(0)+"\"/>\n")
        file.write("\t\t</node>\n")
        num = num + 1
#PEs
peid = 0
for xval in range(x): #sven: x/y vertauscht
    for yval in range(y):
        if verbose:
            print("Generating PE in at x: "+str(xval)+" y:"+str(yval))
        file.write("\t\t<node id=\""+str(num)+"\">\n")
        file.write("\t\t\t<xPos value=\""+str(format(float(xval)/(float(x-1))))+"\"/>\n")
        file.write("\t\t\t<yPos value=\""+str(format(float(yval)/(float(y-1))))+"\"/>\n")
        file.write("\t\t\t<zPos value=\""+str(format(float(0)/(float(1))))+"\"/>\n")
        file.write("\t\t\t<nodeType value=\"1\"/>\n")
        file.write("\t\t\t<idType value=\""+str(peid)+"\"/>\n")
        file.write("\t\t\t<layerType value=\""+str(0)+"\"/>\n")
        file.write("\t\t</node>\n")
        peid = peid + 1
        num = num + 1
file.write("\t</nodes>\n\n\n")

# generate connections
print("Generating connections...")
file.write("\t<connections>\n")
num = 0
for xval in range(x):
    for yval in range(y):
        # connections at local ports:
        if verbose:
            print("Generating connections at node in layer at x: "+str(xval)+" y:"+str(yval))
        nodeRouter = xval*x+yval #index of routers at position [xval, yval, layer]
        nodePE = xval*x+yval+(x*y)
        if verbose:
            print(str(num)+": Connecting router  "+str(nodeRouter)+" with pe "+str(nodePE))
        file.write("\t\t<con id=\""+str(num)+"\">\n")
        file.write("\t\t\t<interface value=\"0\"/>\n")
        file.write("\t\t\t<ports>\n")
        file.write("\t\t\t\t<port id =\"0\">\n")
        file.write("\t\t\t\t\t<node value=\""+str(nodeRouter)+"\"/>\n")
        file.write("\t\t\t\t\t<bufferDepth value=\""+str(bufferDepth)+"\"/>\n")
        file.write("\t\t\t\t\t<vcCount value=\""+str(vcs)+"\"/>\n")
        file.write("\t\t\t\t</port>\n")
        file.write("\t\t\t\t<port id =\"1\">\n")
        file.write("\t\t\t\t\t<node value=\""+str(nodePE)+"\"/>\n")
        file.write("\t\t\t\t\t<bufferDepth value=\""+str(bufferDepth)+"\"/>\n")
        file.write("\t\t\t\t\t<vcCount value=\""+str(vcs)+"\"/>\n")
        file.write("\t\t\t\t</port>\n")
        file.write("\t\t\t</ports>\n")
        file.write("\t\t</con>\n")
        num = num + 1

        # connections within layer following mesh tolopogy
        # only east and north required since links are bidirectional
        for neigbour in range(2):
            writeCon = False
            if neigbour == 0 and yval != (y-1):
                # east: node2 = node1+1
                node2 = nodeRouter + 1
                writeCon = True
            elif neigbour == 1 and xval != (x-1):
                # north: node 2 = node1+x
                node2 = nodeRouter + (y)
                writeCon = True
            if writeCon:
                if verbose:
                    print(str(num)+": Connecting "+str(nodeRouter)+" with "+str(node2))
                makeConnection(file, num, nodeRouter, node2, bufferDepth, vcs)
                num = num + 1



file.write("\t</connections>\n")

file.write("</network-on-chip>\n")

file.close
