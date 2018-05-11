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
    num_senders = config.getint('Generate_Senders', 'num_senders')              # The number of senders
    
    return num_senders    

###############################################################################
    
data = ET.Element('application')                                                # Create the file structure
data.set('xmlns:xsi', 'http://www.w3.org/2001/XMLSchema-instance')
data.set('xsi:noNamespaceSchemaLocation', 'ApplicationGraph.xsd')

node_id = 0
num_senders = init()                                                            # Initialize the script

for i in range(0, num_senders):
    
    start_task = ET.SubElement(data, 'task')                                    # Building the start task
    start_task.set('name', 'start_'+str(i))
    start_task.set('id', str(node_id))
    start_task.set('execution_count', '1')
    start_task.set('trigger_and', 'false')
    
    outport = ET.SubElement(start_task, 'outport')
    outport.set('name', 'end_'+str(i))
    outport.set('id', str(node_id+1))
    outport.set('probability', '1.0')
    outport.set('trafficType', '0')
    
    distribution = ET.SubElement(outport, 'distribution')
    distribution.text = 'POLYNOMIAL'
    
    outport_parameters = ET.SubElement(outport, 'parameters')
    
    coefficient = ET.SubElement(outport_parameters, 'coefficient')
    coefficient.text = '1'
    
    exponent = ET.SubElement(outport_parameters, 'exponent')
    exponent.text = '0'
    
    parameters = ET.SubElement(start_task, 'parameters')
    parameters.set('delay', '1')
    
    source = ET.SubElement(parameters, 'source')
    
    source_distribution = ET.SubElement(source, 'distribution')
    source_distribution.text = 'POLYNOMIAL'
    
    source_parameters = ET.SubElement(source, 'parameters')
    
    source_parameters_coefficient = ET.SubElement(source_parameters, 'coefficient')
    source_parameters_coefficient.text = '100'
    
    source_parameters_exponent = ET.SubElement(source_parameters, 'exponent')
    source_parameters_exponent.text = '0'
    
    end_task = ET.SubElement(data, 'task')                                      # Building the end task
    end_task.set('name', 'end_'+str(i))
    end_task.set('id', str(node_id+1))
    end_task.set('execution_count', '1')
    end_task.set('trigger_and', 'false')
    
    inport = ET.SubElement(end_task, 'inport')
    inport.set('id', str(node_id))
    inport.set('data_volume', '3')
    
    end_task_parameters = ET.SubElement(end_task, 'parameters')
    end_task_parameters.set('delay', '100')

    node_id = node_id + 2
    
mydata = ET.tostring(data)                                                      # Create a new XML file with the results
senders_file = open(str(num_senders) + "_Senders.xml", "w")  
senders_file.write(mydata)
senders_file.close()
