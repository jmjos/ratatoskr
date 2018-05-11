import xml.etree.ElementTree as ET
from xml.dom import minidom

def prettyPrint(elem):
    """Return a pretty-printed XML string for the Element.
    """
    rough_string = ET.tostring(elem, 'utf-8')
    reparsed = minidom.parseString(rough_string)
    return reparsed.toprettyxml(indent="  ")

##################################################################################
""" Model Parameters """
data_types_names = ['image', 'image part', 'feature']
data_type_ix = data_types_names.index('image') # The index of the data type to be send and received
num_senders = 16
num_receivers = 2
probabilities_values = [0.5, 0.5]
packets_rate = ['1', '400'] # [number of packets, interval]
##################################################################################
""" Root Node """
data = ET.Element('data')
##################################################################################
""" Data Types """
dataTypes = ET.SubElement(data, 'dataTypes')

for i in range(0, len(data_types_names)):
    dataType = ET.SubElement(dataTypes, 'dataType')
    dataType.set('id', str(i))

    name = ET.SubElement(dataType, 'name')
    name.set('value', str(data_types_names[i]))
################################################################################
""" Senders Tasks """
tasks = ET.SubElement(data, 'tasks')

for i in range(0, num_senders):
    sender_task = ET.SubElement(tasks, 'task')
    sender_task.set('id', str(i))

    start = ET.SubElement(sender_task, 'start')
    start.set('min', '0')
    start.set('max', '0')

    duration = ET.SubElement(sender_task, 'duration')
    duration.set('min', '-1')
    duration.set('max', '-1')

    repeat = ET.SubElement(sender_task, 'repeat')
    repeat.set('min', '2')
    repeat.set('max', '2')

    generates = ET.SubElement(sender_task, 'generates')

    for j in range(0, len(probabilities_values)):
        possibility = ET.SubElement(generates, 'possibility')
        possibility.set('id', str(j))

        probability = ET.SubElement(possibility, 'probability')
        probability.set('value', str(probabilities_values[j]))

        destinations = ET.SubElement(possibility, 'destinations')

        destination = ET.SubElement(destinations, 'destination')
        destination.set('id', str(0))

        delay = ET.SubElement(destination, 'delay')
        delay.set('min', '0')
        delay.set('max', '3')

        interval = ET.SubElement(destination, 'interval')
        interval.set('min', str(packets_rate[1]))
        interval.set('max', str(packets_rate[1]))

        count = ET.SubElement(destination, 'count')
        count.set('min', str(packets_rate[0]))
        count.set('max', str(packets_rate[0]))

        d_type = ET.SubElement(destination, 'type')
        d_type.set('value', str(data_type_ix))

        d_task = ET.SubElement(destination, 'task')
        d_task.set('value', str(num_senders + j))
################################################################################
""" Receiver Tasks """
for i in range(0, num_receivers):
    receiver_task = ET.SubElement(tasks, 'task')
    receiver_task.set('id', str(num_senders + i))

    start = ET.SubElement(receiver_task, 'start')
    start.set('min', '0')
    start.set('max', '0')

    duration = ET.SubElement(receiver_task, 'duration')
    duration.set('min', '-1')
    duration.set('max', '-1')

    repeat = ET.SubElement(receiver_task, 'repeat')
    repeat.set('min', '2')
    repeat.set('max', '2')

    requires = ET.SubElement(receiver_task, 'requires')

    for i in range(0, num_senders):
        requirement = ET.SubElement(requires, 'requirement')
        requirement.set('id', str(i))

        r_type = ET.SubElement(requirement, 'type')
        r_type.set('value', str(data_type_ix))

        source = ET.SubElement(requirement, 'source')
        source.set('value', str(i))

        count = ET.SubElement(requirement, 'count')
        count.set('min', '1')
        count.set('max', '1')
################################################################################
""" Write data to xml file """
senders_file = open('data.xml', 'w')
senders_file.write(prettyPrint(data))
senders_file.close()
