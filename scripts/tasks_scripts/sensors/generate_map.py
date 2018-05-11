import xml.etree.ElementTree as ET
from xml.dom import minidom

def prettyPrint(elem):
    """Return a pretty-printed XML string for the Element.
    """
    rough_string = ET.tostring(elem, 'utf-8')
    reparsed = minidom.parseString(rough_string)
    return reparsed.toprettyxml(indent="  ")

#########################################################################
""" Model Parameters """
sender_nodes = list(range(32, 48))      # where the senders should map on the mesh
receiver_nodes = list(range(16,18))     # where the receivers should map on the mesh
#########################################################################
""" Root Node """
map_data = ET.Element('map')
#########################################################################
""" Sender Nodes Binding """
for i in range(0, len(sender_nodes)):
    bind = ET.SubElement(map_data, 'bind')

    task = ET.SubElement(bind, 'task')
    task.set('value', str(i))

    node = ET.SubElement(bind, 'node')
    node.set('value', str(sender_nodes[i]))
#########################################################################
""" Receiver Nodes Binding """
for i in range(0, len(receiver_nodes)):
    bind = ET.SubElement(map_data, 'bind')

    task = ET.SubElement(bind, 'task')
    task.set('value', str(len(sender_nodes) + i))

    node = ET.SubElement(bind, 'node')
    node.set('value', str(receiver_nodes[i]))
##########################################################################
""" Write data to xml file """
senders_file = open('map.xml', 'w')
senders_file.write(prettyPrint(map_data))
senders_file.close()
