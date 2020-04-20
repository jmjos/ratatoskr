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
###############################################################################
import configparser
import xml_writers as writers
import plot_network
import os
import multiprocessing
###############################################################################


class Configuration:
    """ The main configuration """
    def __init__(self, path):
        self.path = path
        config = configparser.ConfigParser()
        try:
            config.read(self.path)
        except Exception:
            raise
        self.simulationTime = int(config['Config']['simulationTime'])
        self.flitsPerPacket = int(config['Config']['flitsPerPacket'])
        self.bitWidth = int(config['Config']['bitWidth'])
        self.benchmark = config['Config']['benchmark']
        self.libDir = config['Config']['libDir']

        self.simDir = config['Synthetic']['simDir']
        self.basedir = os.getcwd()
        self.restarts = int(config['Synthetic']['restarts'])
        self.warmupStart = int(config['Synthetic']['warmupStart'])
        self.warmupDuration = int(config['Synthetic']['warmupDuration'])
        self.warmupRate = float(config['Synthetic']['warmupRate'])
        self.runRateMin = float(config['Synthetic']['runRateMin'])
        self.runRateMax = float(config['Synthetic']['runRateMax'])
        self.runRateStep = float(config['Synthetic']['runRateStep'])
        self.runStartAfterWarmup = int(config['Synthetic']['runStartAfterWarmup'])
        self.runStart = self.warmupStart + self.warmupDuration + self.runStartAfterWarmup
        self.runDuration = int(config['Synthetic']['runDuration'])
        self.numCores = int(config['Synthetic']['numCores'])
        if (self.numCores == -1):
            self.numCores = multiprocessing.cpu_count()

        self.bufferReportRouters = config['Report']['bufferReportRouters']
        try:
                self.bufferReportRouters = self.bufferReportRouters[1:len(self.bufferReportRouters)-1]
        except Exception:
                raise

        self.x = config['Hardware']['x']
        self.y = config['Hardware']['y']
        try:
                self.x = self.x[1:len(self.x)-1]
                self.x = self.x.split(',')
                self.x = [ int(x) for x in self.x ]
                self.y = self.y[1:len(self.y)-1]
                self.y = self.y.split(',')
                self.y = [ int(y) for y in self.y ]
        except Exception:
                raise
        self.z = int(config['Hardware']['z'])
        self.routing = config['Hardware']['routing']
        self.clockDelay = config['Hardware']['clockDelay']
        try:
                self.clockDelay = self.clockDelay[1:len(self.clockDelay)-1]
                self.clockDelay = self.clockDelay.split(',')
                self.clockDelay = [ int(cd) for cd in self.clockDelay ]
        except Exception:
                raise
        self.bufferDepthType = config['Hardware']['bufferDepthType']
        #self.bufferDepth = int(config['Hardware']['bufferDepth'])
        self.bufferDepth = config['Hardware']['bufferDepth']
        self.bufferDepth = self.bufferDepth[1:len(self.bufferDepth)-1]
        self.bufferDepth = self.bufferDepth.split(',')

        self.buffersDepths = config['Hardware']['buffersDepths']
        self.buffersDepths = self.buffersDepths[1:len(self.buffersDepths)-1]
        self.vcCount = int(config['Hardware']['vcCount'])
        self.topologyFile = config['Hardware']['topologyFile']
        self.flitSize = int(config['Hardware']['flitSize'])
        self.portNum = int(config['Hardware']['portNum'])
###############################################################################


def main():
    config = Configuration('config.ini')

    writer = writers.ConfigWriter(config)
    writer.write_config('config.xml')

    writer = writers.NetworkWriter(config)
    writer.write_network('network.xml')
    #plot_network.main()
###############################################################################


if __name__ == '__main__':
    main()
