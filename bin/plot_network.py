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

# This script generates simple topology files for mesh, torus and ring
###############################################################################
import sys
import numpy as np
from mpl_toolkits.mplot3d import Axes3D
from mpl_toolkits.mplot3d.art3d import Poly3DCollection
import matplotlib.pyplot as plt
import xml.etree.ElementTree as ET
import configparser
###############################################################################
# Global variables
fig = None  # Figure Object
ax = None  # Axes Object
points = []  # List of the nodes/points
excluded_points = []  # Those are processing elemnts
connections = []  # List of the connection between the points
num_of_layers = 0  # The number of layers in the mesh
topology = None
# Note: a face is not a layer, but the number of faces equals the number of layers.
# A face consists of the corner points of the layer only.
# That means each face consists of only four points.
layers = []  # list of the layers
faces = []  # List of the faces, for drawing reasons
###############################################################################


###############################################################################
# INIT
###############################################################################
def init_script(network_file, config_file):
    """
    Initialize the script by reading the mesh information from the mesh xml file
    """
    try:
        tree = ET.parse(network_file)
    except FileNotFoundError:
        raise FileNotFoundError
    else:
        root = tree.getroot()

        config = configparser.ConfigParser()
        config.read(config_file)

        # network topology for different plot
        global topology
        topology = config['Hardware']['topology']

        # Number of layers
        global num_of_layers
        num_of_layers = int(config['Hardware']['z'])

        # Find the id of the ProcessingElements
        proc_elemnt_ids = []
        for nodeType in root.find('nodeTypes').iter('nodeType'):
            if nodeType.find('model').attrib['value'] == 'ProcessingElement':
                proc_elemnt_ids.append(int(nodeType.attrib['id']))

        # Points is a list of tuples
        global points
        for node in root.find('nodes').iter('node'):
            # don't include processing element nodes
            if int(node.find('nodeType').attrib['value']) not in proc_elemnt_ids:
                x = float(node.find('xPos').attrib['value'])
                y = float(node.find('yPos').attrib['value'])
                z = float(node.find('zPos').attrib['value'])
                layer = int(node.find('layer').attrib['value'])
                points.append(([x, y, z], layer))
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
                connection.append(
                    int(con.find('ports')[0].find('node').attrib['value']))
                connection.append(
                    int(con.find('ports')[1].find('node').attrib['value']))
                connections.append(connection)


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
# PLOT CONNECTIONS
###############################################################################
def generate_3D_half_ellipse(pt1, pt2, theta=np.pi/2, ratio=0.1, num=20):
    """
    Calculate the curvature line of the given 2 points
    """
    assert len(pt1) == 3 and len(pt2) == 3, "The given points are not in 3D dimension."

    pt1 = np.array(pt1)
    pt2 = np.array(pt2)

    pt_mean = (pt1 + pt2) / 2
    pt_diff = np.abs(pt1 - pt2)

    assert sum(pt_diff != 0) == 1, "Diagonal ellipse is not supported"

    tdata = np.linspace(-theta, theta, num=num)

    x, y, z = pt_mean
    xlen, ylen, zlen = pt_diff / 2

    if (pt_diff[0] != 0):
        xdata = x + xlen * np.sin(tdata)
        ydata = y + xlen * np.cos(tdata) * ratio
        zdata = z + np.zeros(tdata.shape)
    elif (pt_diff[1] != 0):
        ydata = y + ylen * np.sin(tdata)
        xdata = x + ylen * np.cos(tdata) * ratio
        zdata = z + np.zeros(tdata.shape)
    elif (pt_diff[2] != 0):
        zdata = z + zlen * np.sin(tdata)
        xdata = x + zlen * np.cos(tdata) * ratio
        ydata = y + np.zeros(tdata.shape)

    return xdata, ydata, zdata


def vertical_horizontal_connection(p1, p2):
    """
    Draws the vertical and horizontal connection of the points
    """
    assert len(p1) == 3 and len(p2) == 3
    line = np.array([p1, p2]).T.tolist()
    ax.plot(*line, color='black')


def is_opposite_border(p1, p2):
    """
    Check the given 2 points (3D) are at the border of the cube and
    one of their dimension exist at the same axis
    """
    p1 = np.array(p1)
    p2 = np.array(p2)
    pt_diff = np.abs(p1 - p2)
    return (np.sum(pt_diff != 0) == 1) and (1 in pt_diff)


def plot_connections():
    """
    Plot the connections between the nodes/points
    """
    for p1_idx, p2_idx in connections:
        p1 = points[p1_idx][0]
        p2 = points[p2_idx][0]

        if is_opposite_border(p1, p2) and topology in set(["torus", "ring"]):
            pts_ellipse = generate_3D_half_ellipse(p1, p2)
            ax.plot(*pts_ellipse, color='grey')
        else:
            vertical_horizontal_connection(p1, p2)

###############################################################################
# PLOT NODES
###############################################################################


def plot_nodes():
    """
    Plot the nodes in the figure
    """
    points_coordinates = np.array([*np.array(points)[:, 0]]).T.tolist()
    ax.scatter(*points_coordinates, color='black', alpha=0.4, s=50)


def annotate_points():
    """
    Annotating the points using their index
    """
    points_coordinates = np.array([*np.array(points)[:, 0]])
    for idx, (x, y, z) in enumerate(points_coordinates):
        ax.text(x, y, z, idx, size=10, color='red')


###############################################################################
# PLOT SURFACES
###############################################################################
def create_faces():
    """
    Create the faces of the mesh, each layer will become a face
    """
    # Make layers

    # Seperate lists of x, y and z coordinates
    x_s = []
    y_s = []
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
    for i in range(0, min(len(z_s), num_of_layers)):
        x_min = min(x_s)
        x_max = max(x_s)
        y_min = min(y_s)
        y_max = max(y_s)
        face = list([(x_min, y_min, z_s[i]), (x_max, y_min, z_s[i]),
                     (x_max, y_max, z_s[i]), (x_min, y_max, z_s[i])])
        faces.append(face)


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
    poly.set_facecolor(faces_colors)
    ax.add_collection3d(poly)

###############################################################################
# MAIN
###############################################################################
def main():
    """
    Main Execution Point
    """
    network_file = 'network.xml'
    config_file = 'config.ini'
    try:
        network_file = sys.argv[1]
        config_file = sys.argv[2]
    except IndexError:
        pass
    init_script(network_file, config_file)
    create_fig()
    plot_nodes()
    plot_connections()
    annotate_points()
    create_faces()
    plot_faces()
    plt.show()


if __name__ == "__main__":
    main()
