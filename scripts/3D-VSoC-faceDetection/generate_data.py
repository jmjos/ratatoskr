import modules.data_XML_writer as writer
import modules.init as init
##################################################################################
def add_camera_node(p_parent):
    """
    Adding the camera task

    Parameters:
        - p_parent: the id of the parent node
    """
    task_id = 0
    repeat_min = repeat_max = int(init.camera_proc_rate[1] / init.R_i)
    camera_task = writer.add_task_node(p_parent, task_id, p_repeat=(repeat_min,repeat_max))

    # Send the data to the processors of layer1
    generates = writer.add_generates_node(camera_task)
    g_start = task_id + 1
    g_end = g_start + init.num_tiles
    dist_list = list(range(g_start, g_end))
    writer.add_possibility(generates, 0, 1, (1,init.num_tiles), init.camera_proc_rate[1], init.camera_proc_rate[0], init.data_types.index('c_l1'), dist_list)

    return task_id
##################################################################################
def add_layer1_nodes(p_parent, p_last_task):
    """
    Adding Layer1 Processors Nodes

    Parameters:
        - p_parent: the id of the parent node
        - p_last_task: the id of the last added task
    """
    start = p_last_task + 1
    end = start + init.num_tiles
    repeat_min = repeat_max = int(init.proc_proc_rate[1] / init.R_i)
    for i in range(start, end):
        task = writer.add_task_node(p_parent, i, p_repeat=(repeat_min,repeat_max))

        # Adding the camera as a requirement
        requires = writer.add_requires_node(task)
        writer.add_requirement(requires, 0, init.data_types.index('c_l1'), p_last_task, init.camera_proc_rate[0])

        # Send the data to shadow1 layer
        generates = writer.add_generates_node(task)
        g_start = end
        g_end = g_start + init.num_tiles
        shadow_list = list(range(g_start, g_end))
        shadow_list.remove(init.num_tiles + i)
        writer.add_possibility(generates, 0, 1, (1,10), init.proc_proc_rate[1], init.proc_proc_rate[0], init.data_types.index('l1_s1'), shadow_list)

    return end - 1
##################################################################################
def add_shadow1_nodes(p_parent, p_last_task):
    """
    Adding a shadow layer in layer 1 to prevent deadlock situation

    Parameters:
        - p_parent: the id of the parent node
        - p_last_task: the id of the last added task
    """
    start = p_last_task + 1
    end = start + init.num_tiles
    repeat_min = repeat_max = int(init.winner_master_rate[1] / init.R_i)
    for i in range(start, end):
        task = writer.add_task_node(p_parent, i, p_repeat=(repeat_min,repeat_max))

        # Adding layer1 processors as a requirment
        requires = writer.add_requires_node(task)
        r_start = start - p_last_task
        r_end = r_start + init.num_tiles
        require_list = list(range(r_start, r_end))
        require_list.remove(i - init.num_tiles)
        for j in range(0, len(require_list)):
            writer.add_requirement(requires, j, init.data_types.index('l1_s1'), require_list[j], init.proc_proc_rate[0])

        # Send the data to the master node
        generates = writer.add_generates_node(task)
        dist_list = [end]
        writer.add_possibility(generates, 0, 1/init.num_tiles, (0,9), init.winner_master_rate[1], init.winner_master_rate[0], init.data_types.index('s1_m1'), dist_list)

    return end - 1
##################################################################################
def add_master_node(p_parent, p_last_task):
    """
    Adding the master node that will distribute the work to the second level

    Parameters:
        - p_parent: the id of the parent node
        - p_last_task: the id of the last added task
    """
    task_id = p_last_task + 1
    repeat_min = repeat_max = int(init.winner_master_rate[1] / init.R_i)
    master_node = writer.add_task_node(p_parent, task_id, p_repeat=(repeat_min,repeat_max))

    # Adding the shadow1 layer as a requirement
    requires = writer.add_requires_node(master_node)
    r_start = task_id - init.num_tiles
    r_end = r_start + init.num_tiles
    require_list = list(range(r_start, r_end))
    for i in range(len(require_list)):
        writer.add_requirement(requires, i, init.data_types.index('s1_m1'), require_list[i], init.proc_proc_rate[0])

    # Generate data to the second layer
    generates = writer.add_generates_node(master_node)
    g_start = task_id + 1
    g_end = g_start + init.num_found_faces
    dist_list = list(range(g_start, g_end))
    writer.add_possibility(generates, 0, 1, (0,9), init.winner_master_rate[1], init.winner_master_rate[0], init.data_types.index('m1_l2'), dist_list)

    return task_id
##################################################################################
def add_layer2_nodes(p_parent, p_last_task):
    """
    Adding the second layer of processors

    Parameters:
        - p_parent: the id of the parent node
        - p_last_task: the id of the last added task
    """
    start = p_last_task + 1
    end = start + init.num_found_faces
    repeat_min = repeat_max = int(init.lay2_cam_rate[1] / init.R_i)
    for i in range(start, end):
        task = writer.add_task_node(p_parent, i, p_repeat=(repeat_min,repeat_max))

        # Add the master node as a requirement
        requires = writer.add_requires_node(task)
        writer.add_requirement(requires, 0, init.data_types.index('m1_l2'), p_last_task, init.winner_master_rate[0])

        # Send the data to shadow cameras, because the camera can't have a requirement
        generates = writer.add_generates_node(task)
        dist_list = [i + init.num_found_faces]
        writer.add_possibility(generates, 0, 1, (0,9),  init.lay2_cam_rate[1], init.lay2_cam_rate[0], init.data_types.index('l2_sc'), dist_list)

    return end - 1
##################################################################################
def add_shadow_cameras(p_parent, p_last_task):
    """
    Adding shadow cameras because the real camera can't have a requirement

    Parameters:
        - p_parent: the id of the parent node
        - p_last_task: the id of the last added task
    """
    start = p_last_task + 1
    end = start + init.num_found_faces
    repeat_min = repeat_max = int(init.cam_lay2_rate[1] / init.R_i)
    for i in range(start, end):
        task = writer.add_task_node(p_parent, i, p_repeat=(repeat_min,repeat_max))

        # Add matching layer2 task as a requirment
        requires = writer.add_requires_node(task)
        writer.add_requirement(requires, 0, init.data_types.index('l2_sc'), i - init.num_found_faces, init.lay2_cam_rate[0])

        # Send the data to matching shadow2 task
        generates = writer.add_generates_node(task)
        dist_list = [i + init.num_found_faces]
        writer.add_possibility(generates, 0, 1, (0,9), init.cam_lay2_rate[1], init.cam_lay2_rate[0], init.data_types.index('sc_s2'), dist_list)

    return end - 1
##################################################################################
def add_shadow2_nodes(p_parent, p_last_task):
    """
    Adding a shadow layer in layer2

    Parameters:
        - p_parent: the id of the parent node
        - p_last_task: the id of the last added task
    """
    start = p_last_task + 1
    end = start + init.num_found_faces
    repeat_min = repeat_max = int(init.shadow2_master2_rate[1] / init.R_i)
    for i in range(start, end):
        task = writer.add_task_node(p_parent, i, p_repeat=(repeat_min,repeat_max))

        # Add matching shadow camera as a requirement
        requires = writer.add_requires_node(task)
        writer.add_requirement(requires, 0, init.data_types.index('sc_s2'), i - init.num_found_faces, init.cam_lay2_rate[0])

        # Send the data to master2
        generates = writer.add_generates_node(task)
        dist_list = [end]
        writer.add_possibility(generates, 0, 1, (0,9), init.shadow2_master2_rate[1], init.shadow2_master2_rate[0], init.data_types.index('s2_m2'), dist_list)

    return end - 1
##################################################################################
def add_master2_node(p_parent, p_last_task):
    """
    Adding the master node in layer2

    Parameters:
        - p_parent: the id of the parent node
        - p_last_task: the id of the last added task
    """
    task_id = p_last_task + 1
    repeat_min = repeat_max = int(init.shadow2_master2_rate[1] / init.R_i)
    task = writer.add_task_node(p_parent, task_id, p_repeat=(repeat_min,repeat_max))

    # Add shadow2 as requirements
    requires = writer.add_requires_node(task)
    r_start = task_id - init.num_found_faces
    r_end = r_start + init.num_found_faces
    require_list = list(range(r_start, r_end))
    for i in range(len(require_list)):
        writer.add_requirement(requires, i, init.data_types.index('s2_m2'), require_list[i], init.shadow2_master2_rate[0])

    # Send the data to layer3
    generates = writer.add_generates_node(task)
    g_start = task_id + init.num_found_faces
    g_end = g_start + init.num_found_faces
    dist_list = list(range(g_start, g_end))
    writer.add_possibility(generates, 0, 1, (0,9), init.shadow2_master2_rate[1], init.shadow2_master2_rate[0], init.data_types.index('m2_l3'), dist_list)

    return task_id
##################################################################################
def add_layer3_activator(p_parent, p_last_task):
    """
    Adding the layer3 activator task, which is a taske mapped to the camera node

    Parameters:
        - p_parent: the id of the parent node
        - p_last_task: the id of the last added task
    """
    task_id = p_last_task + 1
    repeat_min = repeat_max = int(init.shadow2_master2_rate[1] / init.R_min)
    task = writer.add_task_node(p_parent, task_id, p_repeat=(repeat_min,repeat_max))

    # Send the data to layer3
    generates = writer.add_generates_node(task)
    g_start = task_id + 1
    g_end = g_start + init.num_found_faces
    dist_list = list(range(g_start, g_end))
    # Amount of data is not important, it's only for activation purposes, so we send the least amount possible which is 1 boolean and we leave the interval as it is for now
    writer.add_possibility(generates, 0, 1, (0,9), init.shadow2_master2_rate[1], init.getsizeof_bool(), init.data_types.index('actv_l3'), dist_list)

    return task_id
##################################################################################
def add_layer3_nodes(p_parent, p_last_task):
    """
    Adding the third layer of processors

    Parameters:
        - p_parent: the id of the parent node
        - p_last_task: the id of the last added task
    """
    start = p_last_task + 1
    end = start + init.num_found_faces
    repeat_min = repeat_max = int(init.lay3_cam_rate[1] / init.R_min)
    for i in range(start, end):
        task = writer.add_task_node(p_parent, i, p_repeat=(repeat_min,repeat_max))

        # Layer3 is not activated by the layer2, rather by the camera
        requires = writer.add_requires_node(task)
        writer.add_requirement(requires, 0, init.data_types.index('actv_l3'), p_last_task, init.getsizeof_bool())

        # Send the data to shadow cameras 2, because the camera can't have a requirement
        generates = writer.add_generates_node(task)
        dist_list = [i + init.num_found_faces]
        writer.add_possibility(generates, 0, 1, (0,9),  init.lay3_cam_rate[1], init.lay3_cam_rate[0], init.data_types.index('l3_sc2'), dist_list)

    return end - 1
##################################################################################
def add_shadow_cameras2(p_parent, p_last_task):
    """
    Adding shadow cameras 2 because the real camera can't have a requirement

    Parameters:
        - p_parent: the id of the parent node
        - p_last_task: the id of the last added task
    """
    start = p_last_task + 1
    end = start + init.num_found_faces
    repeat_min = repeat_max = int(init.cam_lay3_rate[1] / init.R_min)
    for i in range(start, end):
        task = writer.add_task_node(p_parent, i, p_repeat=(repeat_min,repeat_max))

        # Add matching layer3 shadow task as a requirment
        requires = writer.add_requires_node(task)
        writer.add_requirement(requires, 0, init.data_types.index('l3_sc2'), i - init.num_found_faces, init.lay3_cam_rate[0])

        # Send the data to matching shadow3 task
        generates = writer.add_generates_node(task)
        dist_list = [i + init.num_found_faces]
        writer.add_possibility(generates, 0, 1, (0,9), init.cam_lay3_rate[1], init.cam_lay3_rate[0], init.data_types.index('sc2_s3'), dist_list)

    return end - 1
##################################################################################
def add_shadow3_nodes(p_parent, p_last_task):
    """
    Adding a shadow layer in layer3

    Parameters:
        - p_parent: the id of the parent node
        - p_last_task: the id of the last added task
    """
    start = p_last_task + 1
    end = start + init.num_found_faces
    repeat_min = repeat_max = int(init.lay3_lay3_rate[1] / init.R_min)
    for i in range(start, end):
        task = writer.add_task_node(p_parent, i, p_repeat=(repeat_min,repeat_max))

        # Add matching shadow camera as a requirement
        requires = writer.add_requires_node(task)
        writer.add_requirement(requires, 0, init.data_types.index('sc2_s3'), i - init.num_found_faces, init.cam_lay3_rate[0])

        # Send the data to matching shadow33 task
        generates = writer.add_generates_node(task)
        dist_list = [i + init.num_found_faces]
        writer.add_possibility(generates, 0, 1, (0,9), init.lay3_lay3_rate[1], init.lay3_lay3_rate[0], init.data_types.index('s3_s33'), dist_list)

    return end - 1
##################################################################################
def add_shadow33_nodes(p_parent, p_last_task):
    """
    Adding a second shadow layer in layer3

    Parameters:
        - p_parent: the id of the parent node
        - p_last_task: the id of the last added task
    """
    start = p_last_task + 1
    end = start + init.num_found_faces
    repeat_min = repeat_max = int(init.lay3_lay3_rate[1] / init.R_min)
    for i in range(start, end):
        task = writer.add_task_node(p_parent, i, p_repeat=(repeat_min,repeat_max))

        # Add matching shadow task as a requirement
        requires = writer.add_requires_node(task)
        writer.add_requirement(requires, 0, init.data_types.index('s3_s33'), i - init.num_found_faces, init.lay3_lay3_rate[0])

    return end - 1
##################################################################################
def initialize():
    """ Initialize the application data file """
    init.init_face_detector()
    root = writer.add_root_node()
    writer.add_dataTypes_node(root)
    tasks = writer.add_tasks_node(root)
    return root, tasks
##################################################################################
def add_step1(p_parent, p_last_task):
    """
    Adding Step 1

    Parameters:
        - p_parent: the id of the parent node
        - p_last_task: the id of the last added task
    Return:
        - The id of the last added node, which is the master node of layer1
    """
    l1_last_id = add_layer1_nodes(p_parent, p_last_task)
    s1_last_id = add_shadow1_nodes(p_parent, l1_last_id)
    m1_id = add_master_node(p_parent, s1_last_id)
    return m1_id
##################################################################################
def add_step2(p_parent, p_last_task):
    """
    Adding Step 2

    Parameters:
        - p_parent: the id of the parent node
        - p_last_task: the id of the last added task
    Return:
        - The id of the last added node, which is the master node of layer2
    """
    l2_last_id = add_layer2_nodes(p_parent, p_last_task)
    scs_last_id = add_shadow_cameras(p_parent, l2_last_id)
    s2_last_id = add_shadow2_nodes(p_parent, scs_last_id)
    m2_id = add_master2_node(p_parent, s2_last_id)
    return m2_id
##################################################################################
def add_step3(p_parent, p_last_task):
    """
    Adding Step 3

    Parameters:
        - p_parent: the id of the parent node
        - p_last_task: the id of the last added task
    Return:
        - The id of the last added node, which is the master node of layer2
    """
    actv_l3_id = add_layer3_activator(p_parent, p_last_task)
    l3_last_id = add_layer3_nodes(p_parent, actv_l3_id)
    sc2_last_id = add_shadow_cameras2(p_parent, l3_last_id)
    s3_last_id = add_shadow3_nodes(p_parent, sc2_last_id)
    s33_last_id = add_shadow33_nodes(p_parent, s3_last_id)
    return s33_last_id
##################################################################################
def main():
    """ Main function of  this file """
    # Initialize the structure
    root, tasks = initialize()

    # Add the camera
    cam_id = add_camera_node(tasks)

    # STEP 1
    m1_id = add_step1(tasks, cam_id)

    # STEP 2
    m2_id = add_step2(tasks, m1_id)

    # STEP 3
    s33_last_id = add_step3(tasks, m2_id)

    # Write the data to xml file
    init.write_xml('output/data.xml', root)
##################################################################################
