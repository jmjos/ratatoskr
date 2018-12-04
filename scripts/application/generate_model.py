import xml_writers

map_writer = xml_writers.MapWriter('map.xml')
map_writer.add_bindings([2, 3], [5, 8])
map_writer.write_file()
