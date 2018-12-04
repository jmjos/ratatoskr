import xml.etree.ElementTree as ET
from xml.dom import minidom
###############################################################################


class Writer:
    """ A base class for DataWriter and MapWriter """
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
