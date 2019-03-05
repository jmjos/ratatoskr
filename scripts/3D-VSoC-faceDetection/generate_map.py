import modules.map_XML_writer as writer
import modules.init as init
##########################################################################
def bind(p_parent, p_t_start, p_t_end, p_n_start, p_n_end):
    """
    Binding a set of tasks with a set of nodes

    Parameters:
        - p_parent: the id of the parent node
        - p_t_start: the start of the tasks
        - p_t_end: the end of the tasks
        - p_n_start: the start of the nodes
        - p_n_end: the end of the nodes
    Return:
        - the last id of the added tasks and nodes
    """

    tasks = list(range(p_t_start, p_t_end))
    nodes = list(range(p_n_start, p_n_end))
    for i in range(len(tasks)):
        if len(nodes) != 1:
            writer.bind_task_node(p_parent, tasks[i], nodes[i])
        else:
            writer.bind_task_node(p_parent, tasks[i], nodes[0])
    return tasks[-1], nodes[-1]
##########################################################################
def main():
    """ Main function of this file """
    init.init_mapping()
    root = writer.add_root_node()

    # Mapping the camera
    cam_t_start = init.camera_pos # 0
    cam_t_end = cam_t_start + 1 # 1
    cam_n_start = init.camera_pos # 0
    cam_n_end = cam_n_start + 1 # 1
    cam_t, cam_n = bind(root, cam_t_start, cam_t_end, cam_n_start, cam_t_end) # 0, 0

    # Mapping Layer1
    l1_t_start = cam_t + 1 # 1
    l1_t_end =  l1_t_start + init.num_tiles # 7
    l1_n_start = cam_n + 1 # 1
    l1_n_end = l1_n_start + init.num_tiles # 7
    l1_t, l1_n = bind(root, l1_t_start, l1_t_end, l1_n_start, l1_n_end) # 6, 6

    # Mapping Shadow1
    s1_t_start = l1_t + 1 # 7
    s1_t_end = s1_t_start + init.num_tiles # 13
    s1_n_start = cam_n + 1 # 1
    s1_n_end = s1_n_start + init.num_tiles # 7
    s1_t, s1_n = bind(root, s1_t_start, s1_t_end, s1_n_start, s1_n_end) # 12, 6

    # Mapping Master1
    m1_t_start = s1_t + 1 # 13
    m1_t_end = m1_t_start + 1 # 14
    m1_n_start = s1_n + 1 # 7
    m1_n_end = m1_n_start + 1 # 8
    m1_t, m1_n = bind(root, m1_t_start, m1_t_end, m1_n_start, m1_n_end) # 13, 7

    # Mapping Layer2
    l2_t_start = m1_t + 1 # 14
    l2_t_end = l2_t_start + init.num_found_faces # 16
    l2_n_start = m1_n + 1 # 8
    l2_n_end = l2_n_start + init.num_found_faces # 10
    l2_t, l2_n = bind(root, l2_t_start, l2_t_end, l2_n_start, l2_n_end) # 15, 9

    # Mapping Shadow Cameras
    sc_t_start = l2_t + 1 # 16
    sc_t_end = sc_t_start + init.num_found_faces # 18
    sc_n_start = cam_n # 0
    sc_n_end = sc_n_start + 1 # 1
    sc_t, sc_n = bind(root, sc_t_start, sc_t_end, sc_n_start, sc_n_end) # 17, 0

    # Mapping Shadow2
    s2_t_start = sc_t + 1 # 18
    s2_t_end = s2_t_start + init.num_found_faces # 20
    s2_n_start = m1_n + 1 # 8
    s2_n_end = s2_n_start + init.num_found_faces # 10
    s2_t, s2_n = bind(root, s2_t_start, s2_t_end, s2_n_start, s2_n_end) # 19, 9

    # Mapping Master2
    m2_t_start = s2_t + 1 # 20
    m2_t_end = m2_t_start + 1 # 21
    m2_n_start = s2_n + 1 # 10
    m2_n_end = m2_n_start + 1 # 11
    m2_t, m2_n = bind(root, m2_t_start, m2_t_end, m2_n_start, m2_n_end) # 20, 10

    # Mapping Layer3 activator
    actv_l3_t_start = m2_t + 1 # 21
    actv_l3_t_end = actv_l3_t_start + 1 # 22
    actv_l3_n_start = cam_n # 0
    actv_l3_n_end = actv_l3_n_start + 1 # 1
    actv_l3_t, actv_l3_n = bind(root, actv_l3_t_start, actv_l3_t_end, actv_l3_n_start, actv_l3_n_end) # 21, 0

    # Mapping Layer3
    l3_t_start = actv_l3_t + 1 # 22
    l3_t_end = l3_t_start + init.num_found_faces # 24
    l3_n_start = m2_n + 1 # 11
    l3_n_end = l3_n_start + init.num_found_faces # 13
    l3_t, l3_n = bind(root, l3_t_start, l3_t_end, l3_n_start, l3_n_end) # 23, 12

    # Mapping Shadow Cameras 2
    sc2_t_start = l3_t + 1 # 24
    sc2_t_end = sc2_t_start + init.num_found_faces # 26
    sc2_n_start = cam_n # 0
    sc2_n_end = sc2_n_start + 1 # 1
    sc2_t, sc2_n = bind(root, sc2_t_start, sc2_t_end, sc2_n_start, sc2_n_end) # 25, 0

    # Mapping Shadow3
    s3_t_start = sc2_t + 1 # 26
    s3_t_end = s3_t_start + init.num_found_faces # 28
    s3_n_start = m2_n + 1 # 11
    s3_n_end = s3_n_start + init.num_found_faces # 13
    s3_t, s3_n = bind(root, s3_t_start, s3_t_end, s3_n_start, s3_n_end) # 27, 12

    # Mapping Shadow33
    s33_t_start = s3_t + 1 # 28
    s33_t_end = s33_t_start + init.num_found_faces # 30
    s33_n_start = m2_n + 1 # 11
    s33_n_end = s33_n_start + init.num_found_faces # 13
    s33_t, s33_n = bind(root, s33_t_start, s33_t_end, s33_n_start, s33_n_end) # 29, 12

    init.write_xml('output/map.xml', root)
##########################################################################
