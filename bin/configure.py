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
        self.simulationTime = int(config['CONFIG']['simulationTime'])
        self.flitsPerPacket = int(config['CONFIG']['flitsPerPacket'])
        self.benchmark = config['CONFIG']['benchmark']
        self.libDir = config['CONFIG']['libDir']

        self.x = int(config['Network']['x'])
        self.y = int(config['Network']['y'])
        self.z = int(config['Network']['z'])
        self.router = config['Network']['routing']
        self.clockDelay = int(config['Network']['clockDelay'])
        self.bufferDepthType = config['Network']['bufferDepthType']
        self.bufferDepth = int(config['Network']['bufferDepth'])
        self.buffersDepths = config['Network']['buffersDepths']
        self.buffersDepths = self.buffersDepths[1:len(self.buffersDepths)-1]
        self.vcCount = int(config['Network']['vcCount'])
        self.topologyFile = config['Network']['topologyFile']

        self.simdir = config['URAND']['simdir']
        self.basedir = os.getcwd()
        self.restarts = int(config['URAND']['restarts'])
        self.warmupStart = int(config['URAND']['warmupStart'])
        self.warmupDuration = int(config['URAND']['warmupDuration'])
        self.warmupRate = float(config['URAND']['warmupRate'])
        self.runRateMin = float(config['URAND']['runRateMin'])
        self.runRateMax = float(config['URAND']['runRateMax'])
        self.runRateStep = float(config['URAND']['runRateStep'])
        self.runStartAfterWarmup = int(config['URAND']['runStartAfterWarmup'])
        self.runStart = self.warmupStart + self.warmupDuration + self.runStartAfterWarmup
        self.runDuration = int(config['URAND']['runDuration'])
        self.numCores = int(config['URAND']['numCores'])
        if (self.numCores == -1):
            self.numCores = multiprocessing.cpu_count()

        self.flitSize = int(config['NOC']['flitSize'])

        self.portNum = int(config['router']['portNum'])
        self.Xis = int(config['router']['Xis'])
        self.Yis = int(config['router']['Yis'])
        self.Zis = int(config['router']['Zis'])
        self.routAlgo = config['router']['routAlgo']

        self.pl_routAlgo = config['router_pl']['routAlgo']

        self.bufferReportRouters = config['Report']['bufferReportRouters']
        try:
                self.bufferReportRouters = self.bufferReportRouters[1:len(self.bufferReportRouters)-1]
        except Exception:
                raise
###############################################################################


def main():
    config = Configuration('config.ini')

    writer = writers.ConfigkWriter(config)
    writer.write_config('config.xml')

    writer = writers.NetworkWriter(config)
    writer.write_network('network.xml')
    plot_network.main()
###############################################################################


if __name__ == '__main__':
    main()
