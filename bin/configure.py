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

        self.topologyFile = config['URAND']['topologyFile']
        self.libdir = config['URAND']['libdir']
        self.simdir = config['URAND']['simdir']
        self.basedir = os.getcwd()
        self.simulation_time = int(config['URAND']['simulation_time'])
        self.restarts = int(config['URAND']['restarts'])
        self.warmup_start = int(config['URAND']['warmup_start'])
        self.warmup_duration = int(config['URAND']['warmup_duration'])
        self.warmup_rate = float(config['URAND']['warmup_rate'])
        self.run_rate_min = float(config['URAND']['run_rate_min'])
        self.run_rate_max = float(config['URAND']['run_rate_max'])
        self.run_rate_step = float(config['URAND']['run_rate_step'])
        self.run_start_after_warmup = int(config['URAND']['run_start_after_warmup'])
        self.run_start = self.warmup_start + self.warmup_duration + self.run_start_after_warmup
        self.run_duration = int(config['URAND']['run_duration'])
        self.num_cores = int(config['URAND']['num_cores'])
        if (self.num_cores == -1):
            self.num_cores = multiprocessing.cpu_count()

        self.flit_size = int(config['NOC_3D_PACKAGE']['flit_size'])

        self.port_num = int(config['router']['port_num'])
        self.Xis = int(config['router']['Xis'])
        self.Yis = int(config['router']['Yis'])
        self.Zis = int(config['router']['Zis'])
        self.rout_algo = config['router']['rout_algo']

        self.pl_rout_algo = config['router_pl']['rout_algo']
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
