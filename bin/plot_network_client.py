#!/bin/python

# Copyright 2018 Jan Moritz Joseph

# Permission is hereby granted, free of charge, to any person obtaining a
# copy of this software and associated documentation files (the "Software"),
# to deal in the Software without restriction, including without limitation
# the rights to use, copy, modify, merge, publish, distribute, sublicense,
# and/or sell copies of the Software, and to permit persons to whom the
# Software is furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included
# in all copies or substantial portions of the Software.

# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
# EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
# MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
# IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
# CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
# TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
# SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

# This script generates simple topology files for 2D or 3D meshes
###############################################################################
import sys
import numpy as np
from mpl_toolkits.mplot3d import Axes3D
from mpl_toolkits.mplot3d.art3d import Poly3DCollection
import matplotlib.pyplot as plt
import xml.etree.ElementTree as ET
import configparser
import zmq
###############################################################################
# Global variables
fig = None  # Figure Object
ax = None  # Axes Object
points = []  # List of the nodes/points
excluded_points = []  # Those are processing elemnts
connections = []  # List of the connection between the points
num_of_layers = 0  # The number of layers in the mesh
# Note: a face is not a layer, but the number of faces equals the number of layers.
# A face consists of the corner points of the layer only.
# That means each face consists of only four points.
layers = []  # list of the layers
faces = []  # List of the faces, for drawing reasons
###############################################################################


def init_script(mesh_file):
    """
    Initialize the script by reading the mesh information from the mesh xml file
    """
    try:
        tree = ET.parse(mesh_file)
    except FileNotFoundError:
        raise FileNotFoundError
    else:
        root = tree.getroot()

        config = configparser.ConfigParser()
        config.read('config.ini')

        # Number of layers
        global num_of_layers
        # num_of_layers = len(root.find('layerTypes'))
        num_of_layers = int(config['Hardware']['z'])
        # Find the id of the ProcessingElements
        proc_elemnt_ids = []
        for nodeType in root.find('nodeTypes').iter('nodeType'):
            if nodeType.find('model').attrib['value'] == 'ProcessingElement':
                proc_elemnt_ids.append(int(nodeType.attrib['id']))

        # Points is a list of tuples
        global points
        i = 0
        for node in root.find('nodes').iter('node'):
            # don't include processing element nodes
            if int(node.find('nodeType').attrib['value']) not in proc_elemnt_ids:
                x = float(node.find('xPos').attrib['value'])
                y = float(node.find('yPos').attrib['value'])
                z = float(node.find('zPos').attrib['value'])
                layer = int(node.find('layer').attrib['value'])
                points.append(([x, y, z], layer))
                i = i + 1
            else:
                global excluded_points
                excluded_points.append(int(node.attrib['id']))

        global connections
        for con in root.find('connections').iter('con'):
            valid_con = True
            for port in con.find('ports').iter('port'):
                if (int(port.find('node').attrib['value']) in excluded_points):
                    valid_con = False
                    break
            if valid_con:
                connection = []
                connection.append(int(con.find('ports')[0].find('node').attrib['value']))
                connection.append(int(con.find('ports')[1].find('node').attrib['value']))
                connections.append(connection)
###############################################################################


def create_fig():
    """
    Create the figure object
    """
    global fig
    fig = plt.figure()
    global ax
    ax = fig.gca(projection='3d')
    ax = Axes3D(fig)
    ax.set_xlabel('X')
    ax.set_ylabel('Y')
    ax.set_zlabel('Z')
###############################################################################


def vertical_horizontal_connection(p1_ix, p2_ix):
    """
    Draws the vertical and horizontal connection of the points
    """
    x = [];
    y = [];
    z = []

    x.append(points[p1_ix][0][0])
    x.append(points[p2_ix][0][0])

    y.append(points[p1_ix][0][1])
    y.append(points[p2_ix][0][1])

    z.append(points[p1_ix][0][2])
    z.append(points[p2_ix][0][2])

    ax.plot(x, y, z, marker='o', color='black')
###############################################################################


def solve_diagonal_connection(p1, p2):
    """
    Draws the diagonal connection as vertical and horizontal one
    """
    x = [];
    y = [];
    z = []

    # p1 is higher than p2
    # The drawing starts from the high point to the low point, no reason, just a choice
    if (p1[2] > p2[2]):
        x.append(p1[0])
        x.append(p1[0])
        x.append(p2[0])

        y.append(p1[1])
        y.append(p1[1])
        y.append(p2[1])

        z.append(p1[2])
        z.append(p2[2])
        z.append(p2[2])
    else:
        x.append(p2[0])
        x.append(p2[0])
        x.append(p1[0])

        y.append(p2[1])
        y.append(p2[1])
        y.append(p1[1])

        z.append(p2[2])
        z.append(p1[2])
        z.append(p1[2])

    ax.plot(x, y, z, marker='o', color='black')
###############################################################################


def plot_connections():
    """
    Plot the connections between the nodes/points
    """
    for c in connections:
        p1_ix = c[0]
        p2_ix = c[1]

        p1 = points[p1_ix][0]
        p2 = points[p2_ix][0]
        if (p1[0] != p2[0]) and (p1[1] != p2[1]):
            solve_diagonal_connection(p1, p2)
        else:
            vertical_horizontal_connection(p1_ix, p2_ix)
###############################################################################


def annotate_points():
    """
    Annotating the points using their index
    """
    points_coordinates = []
    for p in points:
        points_coordinates.append(p[0])
    points_coordinates = np.array(points_coordinates)
    i = 0
    for x, y, z in zip(points_coordinates[:, 0], points_coordinates[:, 1], points_coordinates[:, 2]):
        ax.text(x, y, z, i, size=12, color='red')
        i = i + 1
###############################################################################


def create_faces():
    """
    Create the faces of the mesh, each layer will become a face
    """
    # Make layers

    # Seperate lists of x, y and z coordinates
    x_s = [];
    y_s = [];
    z_s = []

    for i in range(0, num_of_layers):
        layer = []
        for p in points:
            if p[1] == i:
                layer.append(p[0])
                x_s.append(p[0][0])
                y_s.append(p[0][1])
                if (p[0][2] not in z_s):
                    z_s.append(p[0][2])
        layers.append(layer)

    # Making faces, only out of the corner points of the layer
    global faces
    for i in range(0, num_of_layers):
        x_min = min(x_s)
        x_max = max(x_s)
        y_min = min(y_s)
        y_max = max(y_s)
        face = list([(x_min, y_min, z_s[i]), (x_max, y_min, z_s[i]), (x_max, y_max, z_s[i]), (x_min, y_max, z_s[i])])
        faces.append(face)
###############################################################################


def plot_faces():
    """
    Plot the faces
    """
    # Create multiple 3D polygons out of the faces
    poly = Poly3DCollection(faces, linewidths=1, alpha=0.1)
    faces_colors = []
    # Generating random color for each face
    for i in range(0, num_of_layers):
        red = np.random.randint(0, 256)
        green = np.random.randint(0, 256)
        blue = np.random.randint(0, 256)
        color = (red, green, blue)
        if color not in faces_colors:
            faces_colors.append('#%02x%02x%02x' % color)
    poly.set_facecolors(faces_colors)
    ax.add_collection3d(poly)
###############################################################################


def main():
    """
    Main Execution Point
    """
    network_file = 'network.xml'
    try:
        network_file = sys.argv[1]
    except IndexError:
        pass
    init_script(network_file)
    
    context = zmq.Context()

    #  Socket to talk to server
    print("Connecting to hello world server")
    socket = context.socket(zmq.REQ)
    socket.connect("tcp://localhost:5555")

    for request in range(100):
        print("Sending request %s " % request)
        socket.send_string("Hello")
        message = socket.recv()
        print("Received reply %s [ %s ]" % (request, message))
        create_fig()
        plot_connections()
        annotate_points()
        create_faces()
        plot_faces()
        plt.show()
###############################################################################


if __name__ == "__main__":
    main()
