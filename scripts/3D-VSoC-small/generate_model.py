from xml_writers import DataWriter, MapWriter
import math
###############################################################################

""" Data Size Per Step """
stp1_data_sz = 13161
stp2_data_sz = 3716
stp3_data_sz = 929
stp4_data_sz = 1

interval = 100
###############################################################################

""" Create data.xml """
data_writer = DataWriter('data')
data_writer.add_dataTypes_node(['dt_0', 'dt_1', 'dt_2', 'dt_3'])
tasks_node = data_writer.add_tasks_node()

""" Bottom Layer """
for t_id in range(12):
    task_node = data_writer.add_task_node(tasks_node, t_id)
    requires_node = data_writer.add_requires_node(task_node)
    if t_id in range(8, 12):
        data_writer.add_requirement(requires_node, 0, 3, 32, stp4_data_sz)
    elif t_id in range(4, 8):
        data_writer.add_requirement(requires_node, 0, 3, 28, stp4_data_sz)
    elif t_id in range(0, 4):
        data_writer.add_requirement(requires_node, 0, 3, 24, stp4_data_sz)

""" Middle Layer """
for t_id in [12, 13, 16, 17, 20, 21]:
    task_node = data_writer.add_task_node(tasks_node, t_id)
    requires_node = data_writer.add_requires_node(task_node)
    if t_id in [12, 16, 20]:
        data_writer.add_requirement(requires_node, 0, 0, t_id + 13,
                                    stp1_data_sz)
        #data_writer.add_requirement(requires_node, 1, 0, t_id + 14,
        #                            math.ceil(stp1_data_sz/2))
    if t_id in [13, 17, 21]:
        data_writer.add_requirement(requires_node, 0, 0, t_id + 14,
                                    stp1_data_sz)
        #data_writer.add_requirement(requires_node, 1, 0, t_id + 13,
        #                            math.ceil(stp1_data_sz/2))
    generates_node = data_writer.add_generates_node(task_node)
    data_writer.add_possibility(generates_node, 0, 1, [0, 100], interval,
                                stp2_data_sz, 1, [t_id + 2])
for t_id in [14, 15, 18, 19, 22, 23]:
    task_node = data_writer.add_task_node(tasks_node, t_id)
    requires_node = data_writer.add_requires_node(task_node)
    data_writer.add_requirement(requires_node, 0, 1, t_id - 2, stp2_data_sz)
    generates_node = data_writer.add_generates_node(task_node)
    if t_id in [14, 15]:
        data_writer.add_possibility(generates_node, 0, 1, [0, 100],
                                    interval, stp3_data_sz, 2, [24])
    elif t_id in [18, 19]:
        data_writer.add_possibility(generates_node, 0, 1, [0, 100],
                                    interval, stp3_data_sz, 2, [28])
    elif t_id in [22, 23]:
        data_writer.add_possibility(generates_node, 0, 1, [0, 100],
                                    interval, stp3_data_sz, 2, [32])

""" Top Layer """
for t_id in [24, 28, 32]:
    task_node = data_writer.add_task_node(tasks_node, t_id)
    requires_node = data_writer.add_requires_node(task_node)
    data_writer.add_requirement(requires_node, 0, 2, t_id - 10, stp3_data_sz)
    #data_writer.add_requirement(requires_node, 1, 2, t_id - 9, stp3_data_sz)
    generates_node = data_writer.add_generates_node(task_node)
    data_writer.add_possibility(generates_node, 0, 1, [0, 100], interval,
                                math.ceil(stp4_data_sz/4), 3, [t_id - 24,
                                         t_id - 23, t_id - 22, t_id - 21])
for t_id in [25, 26, 27, 29, 30, 31, 33, 34, 35]:
    task_node = data_writer.add_task_node(tasks_node, t_id)
    generates_node = data_writer.add_generates_node(task_node)
    if t_id in [25, 29, 33]:
        data_writer.add_possibility(generates_node, 0, 1, [0, 100],
                                    interval, stp1_data_sz, 0, [t_id - 13])
    elif t_id in [26, 30, 34]:
        data_writer.add_possibility(generates_node, 0, 1, [0, 100],
                                    interval, math.ceil(stp1_data_sz/2),
                                    0, [t_id - 13, t_id - 14])
    elif t_id in [27, 31, 35]:
        data_writer.add_possibility(generates_node, 0, 1, [0, 100],
                                    interval, stp1_data_sz, 0, [t_id - 14])

data_writer.write_file('data.xml')
###############################################################################

""" Create map.xml """
map_writer = MapWriter('map')
map_writer.add_bindings(list(range(0, 36)),
                        list(range(0, 12)) +
                        list(range(20, 32)) +
                        list(range(36, 48)))
map_writer.write_file('map.xml')
