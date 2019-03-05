import modules.init as init
##########################################################################
def add_root_node():
    """ Add root Node """
    return init.ET.Element('map')
##########################################################################
def bind_task_node(p_parent, p_task_id, p_node_id):
    """
    Binding a task with its node

    Parameters:
        - p_parent: the id of the node parent
        - p_task_id: the id of the task
        - p_node_id: the id of the node
    """
    bind = init.ET.SubElement(p_parent, 'bind')

    task = init.ET.SubElement(bind, 'task')
    task.set('value', str(p_task_id))

    node = init.ET.SubElement(bind, 'node')
    node.set('value', str(p_node_id))
##########################################################################
