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
import pandas as pd
###############################################################################
num_of_routers = 0
num_of_layers = 0
router_d = []
###############################################################################


def make_list(string):
    l1 = string.strip('[] \n')
    l2 = l1.split(',')
    return [float(c.strip()) for c in l2]
###############################################################################


def get_layer(router_id):
    global num_of_routers
    global num_of_layers
    return router_id // (num_of_routers / num_of_layers)
###############################################################################


def compute_router_power(row):
    global router_d
    router_power = 0.0
    layer = int(get_layer(row['router_id']))
    for index, value in enumerate(row):
        if index != 0:
            router_power += value * router_d[layer]
    return router_power
###############################################################################


class PowerCalculator:
    """ The Power Calculator Class """
    def __init__(self, config_path, clocks_path, power_path):
        self.config_path = config_path
        self.clocks_path = clocks_path
        self.power_path = power_path
        config = configparser.ConfigParser()
        try:
            config.read(self.config_path)
            f = open(self.clocks_path, 'r')
            self.df_power = pd.read_csv(self.power_path)
        except Exception:
            raise
        self.ni_s = make_list(config['Static']['ni'])
        self.router_s = make_list(config['Static']['router'])
        global router_d
        router_d = make_list(config['Dynamic']['router'])
        for line in f:
            if 'Clock Counts' in line:
                l1 = line.split(':')[1]
                self.clocks = make_list(l1)
                break
        global num_of_layers
        global num_of_routers
        num_of_layers = len(self.clocks)
        num_of_routers = len(self.df_power)

    def compute_static_power(self):
        total_sum = 0.0
        global num_of_layers
        for i in range(0, num_of_layers):
            total_sum += self.clocks[i] * (self.ni_s[i] + self.router_s[i])
        return total_sum

    def compute_dynamic_power(self):
        self.df_power['power'] = self.df_power.apply(lambda row: compute_router_power(row), axis=1)
        self.df_power.to_csv('power_per_router.csv')
        return self.df_power['power'].sum()


config_path = 'config.ini'
clocks_path = '../../simulator/report.txt'
power_path = '../../simulator/report_Routers_Power.csv'
calc = PowerCalculator(config_path, clocks_path, power_path)
print('Static Power:', calc.compute_static_power())
print('Dynamic Power:', calc.compute_dynamic_power())
