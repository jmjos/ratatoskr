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

#include "BaseRouting.h"

Node BaseRouting::getNextNode(const Node& node, DIR::TYPE dir)
{
    auto conn_nodes_ids = node.connectedNodes;
    Node next_node = node;
    for (const auto& conn_node_id:conn_nodes_ids) {
        Node conn_node = globalResources.nodes.at(conn_node_id);
        if (dir==DIR::Up && conn_node.pos.z>node.pos.z) {
            next_node = conn_node;
            break;
        }
        if (dir==DIR::Down && conn_node.pos.z<node.pos.z) {
            next_node = conn_node;
            break;
        }
        else if (dir==DIR::East && conn_node.pos.x>node.pos.x) {
            next_node = conn_node;
            break;
        }
        else if (dir==DIR::West && conn_node.pos.x<node.pos.x) {
            next_node = conn_node;
            break;
        }
        else if (dir==DIR::North && conn_node.pos.y>node.pos.y) {
            next_node = conn_node;
            break;
        }
        else if (dir==DIR::South && conn_node.pos.y<node.pos.y) {
            next_node = conn_node;
            break;
        }
    }
    return next_node;
}