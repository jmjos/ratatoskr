import modules.init as init
##################################################################################
def add_root_node():
    """ Add root Node """
    return init.ET.Element('data')
##################################################################################
def add_dataType_node(p_parent, p_id, p_value):
    """
    Individual DataType

    Parameters:
        - p_parent: the id of the parent node
        - p_id: the id of the added data type
        - p_value: the value of the data type
    """
    dataType = init.ET.SubElement(p_parent, 'dataType')
    dataType.set('id', p_id)

    name = init.ET.SubElement(dataType, 'name')
    name.set('value', p_value)
##################################################################################
def add_dataTypes_node(p_parent):
    """
    Add all DataTypes

    Parameters:
        - p_parent: the id of the parent node
    """
    dataTypes = init.ET.SubElement(p_parent, 'dataTypes')
    for i in range(0, len(init.data_types)):
        add_dataType_node(dataTypes, str(i), str(init.data_types[i]))
##################################################################################
def add_tasks_node(p_parent):
    """
    Adding the tasks-node of all tasks

    Parameters:
        - p_parent: the id of the parent node
    """
    return init.ET.SubElement(p_parent, 'tasks')
##################################################################################
def add_task_node(p_parent, p_t_id, p_start=(0,0), p_duration=(-1,-1), p_repeat=(1,1)):
    """
    Adding a template task node without generates and requires tags

    Parameters:
        - p_parent: the id of the parent node
        - p_t_id: the id of the task
        - p_start: the minimum and maximum start time
        - p_duration: the minimum and maximum duration
        - p_repeat: the minimum and maximum repeat
    Return:
        - The added task
    """
    task = init.ET.SubElement(p_parent, 'task')
    task.set('id', str(p_t_id))

    start = init.ET.SubElement(task, 'start')
    start.set('min', str(p_start[0]))
    start.set('max', str(p_start[1]))

    duration = init.ET.SubElement(task, 'duration')
    duration.set('min', str(p_duration[0]))
    duration.set('max', str(p_duration[1]))

    repeat = init.ET.SubElement(task, 'repeat')
    repeat.set('min', str(p_repeat[0]))
    repeat.set('max', str(p_repeat[1]))

    return task
##################################################################################
def add_generates_node(p_parent):
    """
    Adding a generates node

    Parameters:
        - p_parent: the id of the parent node
    """
    generates = init.ET.SubElement(p_parent, 'generates')
    return generates
##################################################################################
def add_possibility(p_parent, p_id, p_prob, p_delay, p_interval, p_count, p_dt_ix, p_dist_tasks):
    """
    Adding a possibility

    Parameters:
        - p_parent: the id of the parent node
        - p_id: the id of the possibility
        - p_prob: the probability of the possibility
        - p_delay: the delay time before a task starts sending the data
        - p_interval: the interval (clock cycle)
        - p_count: the number of packets to send
        - p_dt_ix: the index of the sent data type
        - p_dist_tasks: a list of destination tasks
    """
    possibility = init.ET.SubElement(p_parent, 'possibility')
    possibility.set('id', str(p_id))

    probability = init.ET.SubElement(possibility, 'probability')
    probability.set('value', str(p_prob))

    destinations = init.ET.SubElement(possibility, 'destinations')

    for i in range(0, len(p_dist_tasks)):
        add_destination(destinations, i, p_delay, p_interval, p_count, p_dt_ix, p_dist_tasks[i])
##################################################################################
def add_requires_node(p_parent):
    """
    Adding a requires node

    Parameters:
        - p_parent: the id of the parent node
    """
    requires = init.ET.SubElement(p_parent, 'requires')
    return requires
##################################################################################
def add_requirement(p_parent, p_id, p_type, p_source, p_count):
    """
    Adding a requirement node

    Parameters:
        - p_parent: the id of the parent node
        - p_id: the id of the requirment
        - p_type: the id of the data type
        - p_source: the id of the source task
        - p_count: the number of the required packets from the source task
    """
    requirement = init.ET.SubElement(p_parent, 'requirement')
    requirement.set('id', str(p_id))

    d_type = init.ET.SubElement(requirement, 'type')
    d_type.set('value', str(p_type))

    source = init.ET.SubElement(requirement, 'source')
    source.set('value', str(p_source))

    count = init.ET.SubElement(requirement, 'count')
    num_packs = None
    if init.send_one_pack:
        num_packs = init.default_num_packs
    else:
        num_packs = p_count
    count.set('min', str(num_packs))
    count.set('max', str(num_packs))
##################################################################################
def add_destination(p_parent, p_id, p_delay, p_interval, p_count, p_dt_ix, p_dist_task):
    """
    Adding a destination to a possibility

    Parameters:
        - p_parent: the id of the parent node
        - p_id: the id of the destination
        - p_delay: the delay time before a task starts sending the data
        - p_interval: the interval (clock cycle)
        - p_count: the number of packets to send
        - p_dt_ix: the index of the sent data type
        - p_dist_tasks: a list of destination tasks
    """
    destination = init.ET.SubElement(p_parent, 'destination')
    destination.set('id', str(p_id))

    delay = init.ET.SubElement(destination, 'delay')
    delay.set('min', str(p_delay[0]))
    delay.set('max', str(p_delay[1]))

    interval = init.ET.SubElement(destination, 'interval')
    interval.set('min', str(p_interval))
    interval.set('max', str(p_interval))

    count = init.ET.SubElement(destination, 'count')
    num_packs = None
    if init.send_one_pack:
        num_packs = init.default_num_packs
    else:
        num_packs = p_count
    count.set('min', str(num_packs))
    count.set('max', str(num_packs))

    d_type = init.ET.SubElement(destination, 'type')
    d_type.set('value', str(p_dt_ix))

    d_task = init.ET.SubElement(destination, 'task')
    d_task.set('value', str(p_dist_task))
##################################################################################
