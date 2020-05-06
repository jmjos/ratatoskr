import xml.etree.ElementTree

# Open original file
et = xml.etree.ElementTree.parse('heteroSynch_base_allVCS4.xml')

upperNodeIds = []

for nodes in et.findall('nodes'):
    for node in nodes.findall('node'):
        print(node)
        nodeid = node.attrib.get('id')
        nodeZ = node.find('zPos').attrib.get('value')
        nodetype = node.find('nodeType').attrib.get('value')
        if float(nodeZ) == 1.0 and int(nodetype) == 1:
            upperNodeIds.append(int(nodeid))

print(upperNodeIds)

for connections in et.findall('connections'):
    for con in connections.findall('con'):
        for ports in con.findall('ports'):
            for port in ports.findall('port'):
                nodeid = port.find('node').attrib.get('value')
                if (int(nodeid)) in upperNodeIds:
                    vc = port.find('vcCount')
                    vc.set('value', '1')
                    vcCount = port.find('vcCount').attrib.get('value')
                    #port.find('node').find('vcCount').set('value', '1')
# Write back to file
et.write('heteroSynch.xml')