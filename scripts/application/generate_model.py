from xml_writers import DataWriter, MapWriter
###############################################################################


data_writer = DataWriter(data')
data_writer.add_dataTypes_node(['dt_0', 'dt_1', 'dt_2', 'dt_3'])
data_writer.write_file('data.xml')

map_writer = MapWriter('map')
map_writer.add_bindings([2, 3], [5, 8])
map_writer.write_file('map.xml')
