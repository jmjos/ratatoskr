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

# This script generates simple topology files for 2D meshes in a
# 3D network. The vertial links must be set manually.
# The nodes at position [xval, yval, layer] are indexed follwing
#   sum([a*b for a,b in zip(x[:layer],y[:layer])]) + (xval*(x[layer])+yval)

filename = "mesh.xml"
z = 2
x = [5, 3]
y = [5, 3]
bufferDepth = [16,16]
vcs = [4,4]

verticalNode1 = [0, 2, 4, 10, 12, 14, 20, 22, 24]
verticalNode2 = range(25,34)

verbose = False

def format(value):
    return "%.3f" % value

def makeConnection(file, id, srcNode, dstNode, bufferDepth, vcs):
    file.write("\t\t<con id=\""+str(id)+"\">\n")
    #file.write("\t\t\t<length value = \"900\"/>\n")
    #file.write("\t\t\t<width value = \"3\"/>\n")
    #file.write("\t\t\t<depth value = \"1\"/>\n")
    #file.write("\t\t\t<effectiveCapacityCl value = \"1.0\"/>\n")
    #file.write("\t\t\t<wireCouplingCapacitanceCc value = \"1.0\"/>\n")
    #file.write("\t\t\t<wireSelfCapacitanceCg value = \"1.0\"/>\n")
    #file.write("\t\t\t<wireSelfCapacitancePerUnitLengthCg value = \"1.0\"/>\n")
    #file.write("\t\t\t<tsvarraySelfCapacitanceC0 value = \"1.0\"/>\n")
    #file.write("\t\t\t<tsvarrayNeighbourCapacitanceCd value = \"1.0\"/>\n")
    #file.write("\t\t\t<tsvarrayDiagonalCapacitanceCn value = \"1.0\"/>\n")
    #file.write("\t\t\t<tsvarrayEdgeCapacitanceCe value = \"1.0\"/>\n")
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


assert(len(x) == z and len(y) == z and len(bufferDepth) == z and len(vcs) == z), "Dimensions do not agree."
assert(len(verticalNode1) == len(verticalNode2)), "Dimensions of vertical connections do not agree."

file = open(filename, "w")

# write header
file.write("<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n")
file.write("<network-on-chip xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xsi:noNamespaceSchemaLocation=\"layer.xsd\">\n")
file.write("\t<nodeTypes>\n")
file.write("\t\t<nodeType id=\"0\">\n")
file.write("\t\t\t<routerModel value=\"XYZ\"/>\n")
file.write("\t\t\t<clockSpeed value=\"2\"/>\n")
file.write("\t\t</nodeType>\n")
file.write("\t</nodeTypes>\n\n\n")

# generate layer list
print("Generating layers...")
file.write("\t<layerTypes>\n")
for layer in range(z):
    if verbose:
        print ("Generating layer "+str(layer))
    file.write("\t\t<layerType id=\""+str(layer)+"\">\n")
    file.write("\t\t\t<technology value=\"130\"/>\n")
    file.write("\t\t</layerType>\n")
file.write("\t</layerTypes>\n\n\n")

# generate node list
print("Generating nodes...")
file.write("\t<nodes>\n")
num = 0
for layer in range(z):
    for xval in range(x[layer]): #sven: x/y vertauscht
        for yval in range(y[layer]):
            if verbose:
                print("Generating node in layer "+str(layer)+" at x: "+str(xval)+" y:"+str(yval))
            node = sum([a*b for a,b in zip(x[:layer],y[:layer])]) + (xval*(x[layer])+yval)
            file.write("\t\t<node id=\""+str(node)+"\">\n")
            file.write("\t\t\t<xPos value=\""+str(format(float(xval)/(float(x[layer]-1))))+"\"/>\n")
            file.write("\t\t\t<yPos value=\""+str(format(float(yval)/(float(y[layer]-1))))+"\"/>\n")
            file.write("\t\t\t<zPos value=\""+str(format(float(layer)/(float(z-1))))+"\"/>\n")
            file.write("\t\t\t<nodeType value=\"0\"/>\n")
            file.write("\t\t\t<layerType value=\""+str(layer)+"\"/>\n")
            file.write("\t\t</node>\n")
            num = num + 1
file.write("\t</nodes>\n\n\n")

# generate connections
print("Generating connections...")
file.write("\t<connections>\n")
num = 0
for layer in range(z):
    for xval in range(x[layer]):
        for yval in range(y[layer]):
            # connections at local ports:
            if verbose:
                print("Generating connections at node in layer "+str(layer)+" at x: "+str(xval)+" y:"+str(yval))
            node1 = sum([a*b for a,b in zip(x[:layer],y[:layer])]) + (xval*(x[layer])+yval) #index of routers at position [xval, yval, layer]
            if verbose:
                print(str(num)+": Connecting "+str(node1)+" locally")
            file.write("\t\t<con id=\""+str(num)+"\">\n")
            #file.write("\t\t\t<length value = \"0\"/>\n")
            #file.write("\t\t\t<width value = \"3\"/>\n")
            #file.write("\t\t\t<depth value = \"1\"/>\n")
            #file.write("\t\t\t<effectiveCapacityCl value = \"1.0\"/>\n")
            #file.write("\t\t\t<wireCouplingCapacitanceCc value = \"1.0\"/>\n")
            #file.write("\t\t\t<wireSelfCapacitanceCg value = \"1.0\"/>\n")
            #file.write("\t\t\t<wireSelfCapacitancePerUnitLengthCg value = \"1.0\"/>\n")
            #file.write("\t\t\t<tsvarraySelfCapacitanceC0 value = \"1.0\"/>\n")
            #file.write("\t\t\t<tsvarrayNeighbourCapacitanceCd value = \"1.0\"/>\n")
            #file.write("\t\t\t<tsvarrayDiagonalCapacitanceCn value = \"1.0\"/>\n")
            #file.write("\t\t\t<tsvarrayEdgeCapacitanceCe value = \"1.0\"/>\n")
            file.write("\t\t\t<ports>\n")
            file.write("\t\t\t\t<port id =\"0\">\n")
            file.write("\t\t\t\t\t<node value=\""+str(node1)+"\"/>\n")
            file.write("\t\t\t\t\t<bufferDepth value=\""+str(bufferDepth[layer])+"\"/>\n")
            file.write("\t\t\t\t\t<vcCount value=\""+str(vcs[layer])+"\"/>\n")
            file.write("\t\t\t\t</port>\n")
            file.write("\t\t\t</ports>\n")
            file.write("\t\t</con>\n")
            num = num + 1

            # connections within layer following mesh tolopogy
            # only east and north required since links are bidirectional
            for neigbour in range(2):
                writeCon = False
                if neigbour == 0 and yval != (y[layer]-1):
                    # east: node2 = node1+1
                    node2 = node1 + 1
                    writeCon = True
                elif neigbour == 1 and xval != (x[layer]-1):
                    # north: node 2 = node1+x
                    node2 = node1 + (y[layer])
                    writeCon = True
                if writeCon:
                    if verbose:
                        print(str(num)+": Connecting "+str(node1)+" with "+str(node2))
                    makeConnection(file, num, node1, node2, bufferDepth[layer], vcs[layer])
                    num = num + 1

# vertical links
for node1, node2 in zip(verticalNode1, verticalNode2):
    if verbose:
        print(str(num)+": Connecting "+str(node1)+" with "+str(node2))
    makeConnection(file, num, node1, node2, bufferDepth[layer], vcs[layer])
    num = num + 1


file.write("\t</connections>\n")

file.write("</network-on-chip>\n")

file.close
