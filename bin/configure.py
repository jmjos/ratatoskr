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
import configparser
import xml_writers as writers
import plot_network
###############################################################################


class Configuration:
    """ The main configuration """
    def __init__(self, path):
        self.path = path
        self.config = configparser.ConfigParser()

        try:
            self.config.read(self.path)
        except Exception:
            raise

        self.x = int(self.config['Network']['x'])
        self.y = int(self.config['Network']['y'])
        self.z = int(self.config['Network']['z'])
        self.router = self.config['Network']['routing']
        self.clockDelay = int(self.config['Network']['clockDelay'])
        self.bufferDepthType = self.config['Network']['bufferDepthType']
        self.bufferDepth = int(self.config['Network']['bufferDepth'])
        self.buffersDepths = self.config['Network']['buffersDepths']
        self.buffersDepths = self.buffersDepths[1:len(self.buffersDepths)-1]
        self.vcCount = int(self.config['Network']['vcCount'])
###############################################################################


config = Configuration('config.ini')
writer = writers.ConfigkWriter(config)
writer.write_config('config.xml')
writer = writers.NetworkWriter(config)
writer.write_network('network.xml')
plot_network.main()
