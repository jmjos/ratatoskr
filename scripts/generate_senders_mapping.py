#!/usr/bin/python
# -*- coding: utf-8 -*-
"""
Created on Thu Jan 18 15:52:21 2018

@author: imad
"""
import xml.etree.ElementTree as ET
import ConfigParser

###############################################################################

def init():

    config = ConfigParser.RawConfigParser()
    config.read('general_config.cfg')

    tasks_indices = [int(x) for x in config.get('Mapping', 'tasks_indices').split(',')]
    x = [int(x) for x in config.get('Mesh', 'x').split(',')]
    y = [int(y) for y in config.get('Mesh', 'y').split(',')]
    
    num_resources = 0
    for i in range(0, len(x)):
        num_resources += x[i] * y[i]
    
    return num_resources, tasks_indices

###############################################################################
    
num_resources, tasks_indices = init()                                               # Initialize the script

data = ET.Element('mapping')                                                        # Create the file structure
data.set('xmlns:xsi', 'http://www.w3.org/2001/XMLSchema-instance')
data.set('xsi:noNamespaceSchemaLocation', 'ApplicationMapping.xsd')

for i in range(0, num_resources):

    resource = ET.SubElement(data, 'resource')
    resource.set('id', str(i))
   
    if(i in tasks_indices):                                                         # Adding the task as a child to the resources
        task = ET.SubElement(resource, 'Task')
        task.set('id', str(tasks_indices.index(i)))
                                                                               
mydata = ET.tostring(data)                                                          # Create a new XML file with the results
mapping_file = open(str(num_resources) + "_Mapping.xml", "w")  
mapping_file.write(mydata)
mapping_file.close()
