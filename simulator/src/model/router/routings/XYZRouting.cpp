/*******************************************************************************
 * Copyright (C) 2018 Jan Moritz Joseph
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 ******************************************************************************/
#include "XYZRouting.h"

int XYZRouting::route(int src_node_id, int dst_node_id)
{
    Node src_node = globalResources.nodes.at(src_node_id);
    Vec3D<float> src_pos = src_node.pos;
    Vec3D<float> dst_pos = globalResources.nodes.at(dst_node_id).pos;
    int con_pos = -1;

    if (globalResources.routingDebugMode)
    {
        cout << "ROUTING DEBUG src pos " << src_pos << " dst pos " << dst_pos << endl;
        cout << "ROUTING DEBUG src position in array " << src_node_id << endl;
        cout << "ROUTING DEBUG src id " << src_node.id << endl;
    }

    if (globalResources.routingCircular == false)
    {
        if (dst_pos == src_pos)
            con_pos = src_node.getConPosOfDir(DIR::Local);
        else if (dst_pos.x < src_pos.x)
            con_pos = src_node.getConPosOfDir(DIR::West);
        else if (dst_pos.x > src_pos.x)
            con_pos = src_node.getConPosOfDir(DIR::East);
        else if (dst_pos.y < src_pos.y)
            con_pos = src_node.getConPosOfDir(DIR::South);
        else if (dst_pos.y > src_pos.y)
            con_pos = src_node.getConPosOfDir(DIR::North);
        else if (dst_pos.z < src_pos.z)
            con_pos = src_node.getConPosOfDir(DIR::Down);
        else if (dst_pos.z > src_pos.z)
            con_pos = src_node.getConPosOfDir(DIR::Up);
    }
    else if (globalResources.routingCircular == true)
    {
        Vec3D<int> src_coord = globalResources.norm_to_coord[src_pos];
        Vec3D<float> inner_len = (src_pos - dst_pos).abs();
        Vec3D<float> outer_len;

        outer_len.x = 1. - inner_len.x + globalResources.x_step[src_coord.z];
        outer_len.y = 1. - inner_len.y + globalResources.y_step[src_coord.z];
        outer_len.z = 1. - inner_len.z + globalResources.z_step;

        if (dst_pos == src_pos)
            con_pos = src_node.getConPosOfDir(DIR::Local);
        // x-axis
        else if ((dst_pos.x < src_pos.x) && (inner_len.x <= outer_len.x))
            con_pos = src_node.getConPosOfDir(DIR::West);
        else if ((dst_pos.x < src_pos.x) && (inner_len.x >= outer_len.x))
            con_pos = src_node.getConPosOfDir(DIR::East);
        else if ((dst_pos.x > src_pos.x) && (inner_len.x <= outer_len.x))
            con_pos = src_node.getConPosOfDir(DIR::East);
        else if ((dst_pos.x > src_pos.x) && (inner_len.x >= outer_len.x))
            con_pos = src_node.getConPosOfDir(DIR::West);
        // y-axis
        else if ((dst_pos.y < src_pos.y) && (inner_len.y <= outer_len.y))
            con_pos = src_node.getConPosOfDir(DIR::South);
        else if ((dst_pos.y < src_pos.y) && (inner_len.y >= outer_len.y))
            con_pos = src_node.getConPosOfDir(DIR::North);
        else if ((dst_pos.y > src_pos.y) && (inner_len.y <= outer_len.y))
            con_pos = src_node.getConPosOfDir(DIR::North);
        else if ((dst_pos.y > src_pos.y) && (inner_len.y >= outer_len.y))
            con_pos = src_node.getConPosOfDir(DIR::South);
        // z-axis
        else if ((dst_pos.z < src_pos.z) && (inner_len.z <= outer_len.z))
            con_pos = src_node.getConPosOfDir(DIR::Down);
        else if ((dst_pos.z < src_pos.z) && (inner_len.z >= outer_len.z))
            con_pos = src_node.getConPosOfDir(DIR::Up);
        else if ((dst_pos.z > src_pos.z) && (inner_len.z <= outer_len.z))
            con_pos = src_node.getConPosOfDir(DIR::Up);
        else if ((dst_pos.z > src_pos.z) && (inner_len.z >= outer_len.z))
            con_pos = src_node.getConPosOfDir(DIR::Down);
    }

    return con_pos;
}