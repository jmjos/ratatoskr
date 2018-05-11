#!/usr/bin/python
# -*- coding: utf-8 -*-
"""
Created on Thu Jan 18 18:39:03 2018

@author: imad
"""

import ConfigParser

config = ConfigParser.RawConfigParser()

# When adding sections or items, add them in the reverse order of
# how you want them to be displayed in the actual file.
# In addition, please note that using RawConfigParser's and the raw
# mode of ConfigParser's respective set functions, you can assign
# non-string values to keys internally, but will receive an error
# when attempting to write to a file or when you get it in non-raw
# mode. SafeConfigParser does not allow such assignments to take place.
config.add_section('Generate_Senders')
config.set('Generate_Senders', 'num_senders', '3')

config.add_section('Mapping')
config.set('Mapping', 'tasks_indices', '0, 2, 6, 8, 10, 12')

config.add_section('Mesh')
config.set('Mesh', 'verbose', 'False')
config.set('Mesh', 'verticalNode2', '49, 50, 51, 52, 53, 54, 55, 56, 57,58, 59, 60, 61, 62, 63, 64')
config.set('Mesh', 'verticalNode1', '0, 2, 4, 6, 14, 16, 18, 20, 28, 30, 32, 34, 42, 44, 46, 48')
config.set('Mesh', 'vcs', '4,4')
config.set('Mesh', 'bufferDepth', '16, 16')
config.set('Mesh', 'y', '7, 4')
config.set('Mesh', 'x', '7, 4')
config.set('Mesh', 'z', '2')
config.set('Mesh', 'filename', 'mesh.xml')

# Writing our configuration file to 'example.cfg'
with open('general_config.cfg', 'wb') as configfile:
    config.write(configfile)
