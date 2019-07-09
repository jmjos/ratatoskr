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
import xml.etree.ElementTree as ET
from xml.dom import minidom
import numpy as np
###############################################################################


class Writer:
    """ A base class for DataWriter, MapWriter and NetwrokWriter """
    def __init__(self, root_node_name):
        root_node = ET.Element(root_node_name)
        root_node.set('xmlns:xsi', 'http://www.w3.org/2001/XMLSchema-instance')
        self.root_node = root_node

    def write_file(self, output_file):
        """ Write the xml file on disk """
        rough_string = ET.tostring(self.root_node, 'utf-8')
        reparsed = minidom.parseString(rough_string)
        data = reparsed.toprettyxml(indent="  ")
        of = open(output_file, 'w')
        of.write(data)
        of.close()
###############################################################################


class DataWriter(Writer):
    """ The class which is responsible of writing the tasks/data file """

    def add_dataTypes_node(self, data_types):
        """
        Add all DataTypes

        Parameters:
            - data_types: a list of data types
        """
        dataTypes_node = ET.SubElement(self.root_node, 'dataTypes')
        for i in range(0, len(data_types)):
            self.add_dataType_node(dataTypes_node, i, data_types[i])

    def add_dataType_node(self, parent_node, id, value):
        """
        Individual DataType

        Parameters:
            - parent_node: the parent node
            - id: the id of the added data type
            - value: the value of the data type
        """
        dataType_node = ET.SubElement(parent_node, 'dataType')
        dataType_node.set('id', str(id))

        name_node = ET.SubElement(dataType_node, 'name')
        name_node.set('value', str(value))

    def add_tasks_node(self):
        """ Adding the tasks-node of all tasks and returns it """
        return ET.SubElement(self.root_node, 'tasks')

    def add_task_node(self, parent_node, t_id, start=(0, 0), duration=(-1, -1),
                      repeat=(1, 1)):
        """
        Adding a template task node without generates and requires tags

        Parameters:
            - parent_node: the parent node
            - t_id: the id of the task
            - start: the minimum and maximum start time
            - duration: the minimum and maximum duration
            - repeat: the minimum and maximum repeat
        Return:
            - The added task
        """
        task_node = ET.SubElement(parent_node, 'task')
        task_node.set('id', str(t_id))

        start_node = ET.SubElement(task_node, 'start')
        start_node.set('min', str(start[0]))
        start_node.set('max', str(start[1]))

        duration_node = ET.SubElement(task_node, 'duration')
        duration_node.set('min', str(duration[0]))
        duration_node.set('max', str(duration[1]))

        repeat_node = ET.SubElement(task_node, 'repeat')
        repeat_node.set('min', str(repeat[0]))
        repeat_node.set('max', str(repeat[1]))

        return task_node

    def add_generates_node(self, parent_node):
        """
        Adding a generates node

        Parameters:
            - parent_node: parent node

        Return:
            - the 'generates' node
        """
        generates_node = ET.SubElement(parent_node, 'generates')
        return generates_node

    def add_possibility(self, parent_node, id, prob, delay, interval, count,
                        dt_ix, dist_tasks):
        """
        Adding a possibility

        Parameters:
            - parent_node: the parent node
            - id: the id of the possibility
            - prob: the probability of the possibility
            - delay: the delay time before a task starts sending the data
            - interval: the interval (clock cycle)
            - count: the number of packets to send
            - dt_ix: the index of the sent data type
            - dist_tasks: a list of destination tasks
        """
        possibility_node = ET.SubElement(parent_node, 'possibility')
        possibility_node.set('id', str(id))

        probability_node = ET.SubElement(possibility_node, 'probability')
        probability_node.set('value', str(prob))

        destinations_node = ET.SubElement(possibility_node, 'destinations')

        for i in range(0, len(dist_tasks)):
            self.add_destination(destinations_node, i, delay, interval, count,
                                 dt_ix, dist_tasks[i])

    def add_requires_node(self, parent_node):
        """
        Adding a requires node

        Parameters:
            - parent_node: the parent node

        Return:
            - the 'requires' node
        """
        requires_node = ET.SubElement(parent_node, 'requires')
        return requires_node

    def add_requirement(self, parent_node, id, type, source, count):
        """
        Adding a requirement node

        Parameters:
            - parent_node: the parent node
            - id: the id of the requirment
            - type: the id of the data type
            - source: the id of the source task
            - count: the number of the required packets from the source task
        """
        requirement_node = ET.SubElement(parent_node, 'requirement')
        requirement_node.set('id', str(id))

        d_type_node = ET.SubElement(requirement_node, 'type')
        d_type_node.set('value', str(type))

        source_node = ET.SubElement(requirement_node, 'source')
        source_node.set('value', str(source))

        count_node = ET.SubElement(requirement_node, 'count')
        count_node.set('min', str(count))
        count_node.set('max', str(count))

    def add_destination(self, parent_node, id, delay, interval, count, dt_ix,
                        dist_task):
        """
        Adding a destination to a possibility

        Parameters:
            - parent_node: the parent node
            - id: the id of the destination
            - delay: the delay time before a task starts sending the data
            - interval: the interval (clock cycle)
            - count: the number of packets to send
            - dt_ix: the index of the sent data type
            - dist_tasks: a list of destination tasks
        """
        destination_node = ET.SubElement(parent_node, 'destination')
        destination_node.set('id', str(id))

        delay_node = ET.SubElement(destination_node, 'delay')
        delay_node.set('min', str(delay[0]))
        delay_node.set('max', str(delay[1]))

        interval_node = ET.SubElement(destination_node, 'interval')
        interval_node.set('min', str(interval))
        interval_node.set('max', str(interval))

        count_node = ET.SubElement(destination_node, 'count')
        count_node.set('min', str(count))
        count_node.set('max', str(count))

        d_type_node = ET.SubElement(destination_node, 'type')
        d_type_node.set('value', str(dt_ix))

        d_task_node = ET.SubElement(destination_node, 'task')
        d_task_node.set('value', str(dist_task))
###############################################################################


class MapWriter(Writer):
    """ The class which is responsible of writing the map file """

    def add_bindings(self, tasks, nodes):
        """
        Binding the tasks with their nodes

        Parameters:
            - tasks: a list of tasks
            - nodes: a list of nodes
        """
        for t_id, n_id in zip(tasks, nodes):
            bind_node = ET.SubElement(self.root_node, 'bind')
            task_node = ET.SubElement(bind_node, 'task')
            task_node.set('value', str(t_id))

            node_node = ET.SubElement(bind_node, 'node')
            node_node.set('value', str(n_id))
###############################################################################


class NetworkWriter(Writer):
    """ The Network writer class """

    def __init__(self, config):
        Writer.__init__(self, 'network-on-chip')
        self.config = config
        if self.config.z == 1:
            self.z_step = 1
            self.z_range = np.arange(0, 1, self.config.z)
        else:
            self.z_step = 1/(self.config.z - 1)
            self.z_range = np.arange(0, 1+self.z_step, self.z_step)
        self.x_step = []
        self.x_range = []
        for x in self.config.x:
            self.x_step.append(1/(x - 1))
            self.x_range.append(np.arange(0, 1+self.x_step[-1], self.x_step[-1]))
        self.y_step = []
        self.y_range = []
        for y in self.config.y:
            self.y_step.append(1/(y - 1))
            self.y_range.append(np.arange(0, 1+self.y_step[-1], self.y_step[-1]))

    def write_header(self):
        bufferDepthType_node = ET.SubElement(self.root_node, 'bufferDepthType')
        bufferDepthType_node.set('value', self.config.bufferDepthType)

    def write_layers(self):
        layers_node = ET.SubElement(self.root_node, 'layers')
        for i in range(0, self.config.z):
            layer_node = ET.SubElement(layers_node, 'layer')
            layer_node.set('value', str(i))

    def write_nodeTypes(self):
        nodeTypes_node = ET.SubElement(self.root_node, 'nodeTypes')
        for i in range(0, self.config.z):
            nodeType_node = ET.SubElement(nodeTypes_node, 'nodeType')
            nodeType_node.set('id', str(i))
            model_node = ET.SubElement(nodeType_node, 'model')
            model_node.set('value', 'RouterVC')
            routing_node = ET.SubElement(nodeType_node, 'routing')
            routing_node.set('value', self.config.routing)
            selection_node = ET.SubElement(nodeType_node, 'selection')
            selection_node.set('value', '1stFreeVC')
            clockDelay_node = ET.SubElement(nodeType_node, 'clockDelay')
            clockDelay_node.set('value', str(self.config.clockDelay[i]))
            arbiterType_node = ET.SubElement(nodeType_node, 'arbiterType')
            arbiterType_node.set('value', 'fair')
        for i in range(self.config.z, self.config.z*2):
            nodeType_node = ET.SubElement(nodeTypes_node, 'nodeType')
            nodeType_node.set('id', str(i))
            model_node = ET.SubElement(nodeType_node, 'model')
            model_node.set('value', 'ProcessingElement')
            clockDelay_node = ET.SubElement(nodeType_node, 'clockDelay')
            clockDelay_node.set('value', '1')

    def write_nodes_node(self):
        nodes_node = ET.SubElement(self.root_node, 'nodes')
        return nodes_node

    def write_nodes(self, nodes_node, node_type):
        if node_type == 'Router':
            node_id = 0
        else:
            nodecounts = []
            for (x, y) in zip(self.config.x, self.config.y):
                nodecounts.append(x*y)
            node_id = sum(nodecounts)
        nodeType_id = 0

        z = 0
        for zi in self.z_range:
            idType = 0
            for yi in self.y_range[z]:
                for xi in self.x_range[z]:
                    node_node = ET.SubElement(nodes_node, 'node')
                    node_node.set('id', str(node_id))
                    xPos_node = ET.SubElement(node_node, 'xPos')
                    xPos_node.set('value', str(xi))
                    yPos_node = ET.SubElement(node_node, 'yPos')
                    yPos_node.set('value', str(yi))
                    zPos_node = ET.SubElement(node_node, 'zPos')
                    zPos_node.set('value', str(zi))
                    nodeType_node = ET.SubElement(node_node, 'nodeType')
                    if node_type == 'Router':
                        nodeType_node.set('value', str(nodeType_id))
                    else:
                        nodeType_node.set('value', str(self.config.z+nodeType_id))
                    idType_node = ET.SubElement(node_node, 'idType')
                    idType_node.set('value', str(idType))
                    layer_node = ET.SubElement(node_node, 'layer')
                    layer_node.set('value', str(int(zi*(self.config.z-1))))
                    node_id += 1
                    idType += 1
            z += 1
            nodeType_id += 1

    def make_port(self, ports_node, port_id, node_id):
        port_node = ET.SubElement(ports_node, 'port')
        port_node.set('id', str(port_id))
        node_node = ET.SubElement(port_node, 'node')
        node_node.set('value', str(node_id))
        bufferDepth_node = ET.SubElement(port_node, 'bufferDepth')
        bufferDepth_node.set('value', str(self.config.bufferDepth))
        buffersDepths_node = ET.SubElement(port_node, 'buffersDepths')
        buffersDepths_node.set('value', str(self.config.buffersDepths))
        vcCount_node = ET.SubElement(port_node, 'vcCount')
        vcCount_node.set('value', str(self.config.vcCount))

    def make_con(self, connections_node, con_id, src_node, dst_node):
        #print("binding " + str(src_node) + " to " + str(dst_node))
        dupCon = self.is_duplicate_con(connections_node, src_node, dst_node)
        if not dupCon:
            self.construct_con(connections_node, con_id, src_node, dst_node)
            return con_id + 1
        return con_id

    def is_duplicate_con(self, connections_node, src_node, dst_node):
        for con in connections_node:
            check1 = False
            check2 = False
            for port in con.iter('port'):
                node = port.find('node')
                node_id = int(node.get('value'))
                if node_id == dst_node:
                    check1 = True
                elif node_id == src_node:
                    check2 = True
            if check1 and check2:
                return True
        return False

    def construct_con(self, connections_node, con_id, src_node, dst_node):
        con_node = ET.SubElement(connections_node, 'con')
        con_node.set('id', str(con_id))
        #interface_node = ET.SubElement(con_node, 'interface')
        #interface_node.set('value', str(0))
        ports_node = ET.SubElement(con_node, 'ports')
        self.make_port(ports_node, 0, src_node)
        self.make_port(ports_node, 1, dst_node)

    def write_connections(self):
        connections_node = ET.SubElement(self.root_node, 'connections')
        con_id = 0
        node_id = 0
        z = 0
        nodecounts = []
        for (x, y) in zip(self.config.x, self.config.y):
            nodecounts.append(x*y)
        nodecount = sum(nodecounts)
        already_connected = set()

        for zi in self.z_range:
            for yi in self.y_range[z]:
                for xi in self.x_range[z]:
                    # create Local
                    #print("connecting local from " + str(node_id) + " to " + str(node_id + nodecount))
                    connection_tuple = (min(node_id, node_id + nodecount), max(node_id , node_id +nodecount))
                    if not connection_tuple in already_connected:
                        con_id = self.make_con(connections_node, con_id, connection_tuple[1], connection_tuple[0])
                        already_connected.add(connection_tuple)
                    if xi > 0:  # create West
                        connection_tuple = (node_id - 1, node_id)
                        if not connection_tuple in already_connected:
                            con_id = self.make_con(connections_node, con_id, connection_tuple[1], connection_tuple[0])
                            already_connected.add(connection_tuple)
                        #con_id = self.make_con(connections_node, con_id, node_id, node_id-1)
                    if xi < 0.95:  # create East
                        connection_tuple = (node_id, node_id + 1)
                        if not connection_tuple in already_connected:
                            con_id = self.make_con(connections_node, con_id, connection_tuple[1], connection_tuple[0])
                            already_connected.add(connection_tuple)
                        #con_id = self.make_con(connections_node, con_id, node_id, node_id+1)
                    if yi > 0:  # create South
                        connection_tuple = (min(node_id, node_id-self.config.x[z]), max(node_id , node_id-self.config.x[z]))
                        if not connection_tuple in already_connected:
                            con_id = self.make_con(connections_node, con_id, connection_tuple[1], connection_tuple[0])
                            already_connected.add(connection_tuple)
                        #con_id = self.make_con(connections_node, con_id, node_id, node_id-self.config.x[z])
                    if yi < 0.95:  # create North
                        connection_tuple = (min(node_id, node_id+self.config.x[z]), max(node_id , node_id+self.config.x[z]))
                        if not connection_tuple in already_connected:
                            con_id = self.make_con(connections_node, con_id, connection_tuple[1], connection_tuple[0])
                            already_connected.add(connection_tuple)
                        #con_id = self.make_con(connections_node, con_id, node_id, node_id+self.config.x[z])
                    if zi > 0:  # create Down
                        x_finder = np.where(self.x_range[z - 1] == xi)
                        x_index = -1
                        y_finder = np.where(self.y_range[z - 1] == yi)
                        y_index = -1
                        if (len(x_finder[0]) != 0):
                            x_index = x_finder[0][0]
                        if (len(y_finder[0]) != 0):
                            y_index = y_finder[0][0]
                        if (x_index != -1 and y_index != -1):
                            previous_node_count = 0
                            if (z > 1):
                                previous_node_count = sum(nodecounts[ 0 : int(z) - 1 ])
                            dst_id = previous_node_count + y_index * self.config.y[z - 1] + x_index
                            #print("connecting " + str(node_id) + " with " + str(dst_id))
                            connection_tuple = (min(node_id, dst_id), max(node_id , dst_id))
                            if not connection_tuple in already_connected:
                                con_id = self.make_con(connections_node, con_id, connection_tuple[1], connection_tuple[0])
                                already_connected.add(connection_tuple)
                            #con_id = self.make_con(connections_node, con_id, node_id, dst_id)
                    if zi < 0.95:  # create Up
                        x_finder = np.where(self.x_range[z + 1] == xi)
                        x_index = -1
                        y_finder = np.where(self.y_range[z + 1] == yi)
                        y_index = -1
                        if (len(x_finder[0]) != 0):
                            x_index = x_finder[0][0]
                        if (len(y_finder[0]) != 0):
                            y_index = y_finder[0][0]
                        if (x_index != -1 and y_index != -1):
                            previous_node_count = sum(nodecounts[ 0 : int(z) + 1 ])
                            dst_id = previous_node_count + y_index * self.config.y[z + 1] + x_index
                            connection_tuple = (min(node_id, dst_id), max(node_id , dst_id))
                            if not connection_tuple in already_connected:
                                con_id = self.make_con(connections_node, con_id, connection_tuple[1], connection_tuple[0])
                                already_connected.add(connection_tuple)
                            #con_id = self.make_con(connections_node, con_id, node_id, dst_id)
                    node_id += 1
            z += 1

    def write_network(self, file_name):
        self.write_header()
        self.write_layers()
        self.write_nodeTypes()
        nodes_node = self.write_nodes_node()
        self.write_nodes(nodes_node, 'Router')
        self.write_nodes(nodes_node, 'ProcessingElement')
        self.write_connections()
        self.write_file(file_name)
###############################################################################


class ConfigkWriter(Writer):
    """ The Config writer class """

    def __init__(self, config):
        Writer.__init__(self, 'configuration')
        self.config = config

    def write_general(self):
        general_node = ET.SubElement(self.root_node, 'general')
        simulationTime_node = ET.SubElement(general_node, 'simulationTime')
        simulationTime_node.set('value', str(self.config.simulationTime))
        outputToFile_node = ET.SubElement(general_node, 'outputToFile')
        outputToFile_node.set('value', 'true')
        outputToFile_node.text = 'report'

    def write_noc(self):
        noc_node = ET.SubElement(self.root_node, 'noc')
        nocFile_node = ET.SubElement(noc_node, 'nocFile')
        nocFile_node.text = 'config/network.xml'
        flitsPerPacket_node = ET.SubElement(noc_node, 'flitsPerPacket')
        flitsPerPacket_node.set('value', str(self.config.flitsPerPacket))
        Vdd_node = ET.SubElement(noc_node, 'Vdd')
        Vdd_node.set('value', '5')

    def write_phase(self, synthetic_node, name, start, duration):
        phase_node = ET.SubElement(synthetic_node, 'phase')
        phase_node.set('name', name)
        distribution_node = ET.SubElement(phase_node, 'distribution')
        distribution_node.set('value', 'uniform')
        start_node = ET.SubElement(phase_node, 'start')
        start_node.set('min', str(start[0]))
        start_node.set('max', str(start[1]))
        duration_node = ET.SubElement(phase_node, 'duration')
        duration_node.set('min', str(duration[0]))
        duration_node.set('max', str(duration[1]))
        repeat_node = ET.SubElement(phase_node, 'repeat')
        repeat_node.set('min', '-1')
        repeat_node.set('max', '-1')
        delay_node = ET.SubElement(phase_node, 'delay')
        delay_node.set('min', '0')
        delay_node.set('max', '0')
        injectionRate_node = ET.SubElement(phase_node, 'injectionRate')
        injectionRate_node.set('value', '0.002')
        count_node = ET.SubElement(phase_node, 'count')
        count_node.set('min', '1')
        count_node.set('max', '1')
        hotspot_node = ET.SubElement(phase_node, 'hotspot')
        hotspot_node.set('value', '0')

    def write_synthetic(self, application_node):
        synthetic_node = ET.SubElement(application_node, 'synthetic')
        # write two phases as a template
        self.write_phase(synthetic_node, 'warmup', [100, 100], [1090, 1090])
        self.write_phase(synthetic_node, 'run', [1100, 1100], [101100, 101100])

    def write_task(self, application_node):
        dataFile_node = ET.SubElement(application_node, 'dataFile')
        dataFile_node.text = 'config/data.xml'
        mapFile_node = ET.SubElement(application_node, 'mapFile')
        mapFile_node.text = 'config/map.xml'

    def write_application(self):
        application_node = ET.SubElement(self.root_node, 'application')
        benchmark_node = ET.SubElement(application_node, 'benchmark')
        benchmark_node.text = self.config.benchmark
        if self.config.benchmark == 'synthetic':
            self.write_synthetic(application_node)
        elif self.config.benchmark == 'task':
            self.write_task(application_node)
            # data_writer = DataWriter('data')
            # data_writer.write_file('data.xml')
            # map_writer = MapWriter('map')
            # map_writer.write_file('map.xml')
        simulationFile_node = ET.SubElement(application_node, 'simulationFile')
        simulationFile_node.text = 'traffic/pipelinePerformance_2D/PipelineResetTB.xml'
        mappingFile_node = ET.SubElement(application_node, 'mappingFile')
        mappingFile_node.text = 'traffic/pipelinePerformance_2D/PipelineResetTBMapping.xml'
        netraceFile_node = ET.SubElement(application_node, 'netraceFile')
        netraceFile_node.text = 'traffic/netrace/example.tra.bz2'
        netraceStartRegion_node = ET.SubElement(application_node, 'netraceStartRegion')
        netraceStartRegion_node.set('value', '0')
        isUniform_node = ET.SubElement(application_node, 'isUniform')
        isUniform_node.set('value', 'false')
        numberOfTrafficTypes_node = ET.SubElement(application_node, 'numberOfTrafficTypes')
        numberOfTrafficTypes_node.set('value', '5')

    def write_node_verbose(self, verbose_node, node_name):
        node = ET.SubElement(verbose_node, node_name)
        function_calls_node = ET.SubElement(node, 'function_calls')
        function_calls_node.set('value', 'false')
        send_flit_node = ET.SubElement(node, 'send_flit')
        send_flit_node.set('value', 'false')
        send_head_flit_node = ET.SubElement(node, 'send_head_flit')
        receive_flit_node = ET.SubElement(node, 'receive_flit')
        receive_flit_node.set('value', 'false')
        receive_tail_flit_node = ET.SubElement(node, 'receive_tail_flit')
        receive_tail_flit_node.set('value', 'true')
        throttle_node = ET.SubElement(node, 'throttle')
        throttle_node.set('value', 'false')
        reset_node = ET.SubElement(node, 'reset')
        reset_node.set('value', 'false')
        if node_name == 'router':
            assign_channel_node = ET.SubElement(node, 'assign_channel')
            assign_channel_node.set('value', 'false')
            buffer_overflow_node = ET.SubElement(node, 'buffer_overflow')
            buffer_overflow_node.set('value', 'true')
        send_head_flit_node.set('value', 'true')

    def write_netrace_verbose(self, verbose_node):
        netrace_node = ET.SubElement(verbose_node, 'netrace')
        inject_node = ET.SubElement(netrace_node, 'inject')
        inject_node.set('value', 'true')
        eject_node = ET.SubElement(netrace_node, 'eject')
        eject_node.set('value', 'true')
        router_receive_node = ET.SubElement(netrace_node, 'router_receive')
        router_receive_node.set('value', 'true')

    def write_tasks_verbose(self, verbose_node):
        tasks_node = ET.SubElement(verbose_node, 'tasks')
        function_calls_node = ET.SubElement(tasks_node, 'function_calls')
        function_calls_node.set('value', 'true')
        xml_parse_node = ET.SubElement(tasks_node, 'xml_parse')
        xml_parse_node.set('value', 'false')
        data_receive_node = ET.SubElement(tasks_node, 'data_receive')
        data_receive_node.set('value', 'true')
        data_send_node = ET.SubElement(tasks_node, 'data_send')
        data_send_node.set('value', 'true')
        source_execute_node = ET.SubElement(tasks_node, 'source_execute')
        source_execute_node.set('value', 'false')

    def write_verbose(self):
        verbose_node = ET.SubElement(self.root_node, 'verbose')
        self.write_node_verbose(verbose_node, 'processingElements')
        self.write_node_verbose(verbose_node, 'router')
        self.write_netrace_verbose(verbose_node)
        self.write_tasks_verbose(verbose_node)

    def write_report(self):
        report_node = ET.SubElement(self.root_node, 'report')
        bufferReportRouters = ET.SubElement(report_node, 'bufferReportRouters')
        bufferReportRouters.text = str(self.config.bufferReportRouters).replace(",", "")

    def write_config(self, file_name):
        self.write_general()
        self.write_noc()
        self.write_application()
        self.write_verbose()
        self.write_report()
        self.write_file(file_name)
