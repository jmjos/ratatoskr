/*******************************************************************************
 * Copyright (C) 2020 Jan Moritz Joseph
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

#include <model/processingElement/ProcessingElementVC.h>
#include "NetracePool.h"

NetracePool::NetracePool(sc_module_name nm)
{
    cout << endl;
    cout << "Netrace Benchmark" << endl;
    SC_THREAD(thread);
}

NetracePool::~NetracePool()
{
}

void NetracePool::thread() {
    ntNetrace ntnetrace;
    //TODO read parameters from xml?
    int i;
    int ignore_dependencies = 0;
    int start_region = 0;
    int reader_throttling = 0;
    const char* tracefile = "src/model/traffic/netrace/testraces/example.tra.bz2";

    int packets_left = 0;
    cycle = 0;
    nt_packet_t* trace_packet = NULL;
    nt_packet_t* packet = NULL;
    ntnetrace.nt_open_trfile( tracefile );
    if( ignore_dependencies ) {
        ntnetrace.nt_disable_dependencies();
    }
    ntnetrace.nt_print_trheader();
    header = ntnetrace.nt_get_trheader();
    ntnetrace.nt_seek_region( &header->regions[start_region] );
    for( i = 0; i < start_region; i++ ) {
        cycle += header->regions[i].num_cycles;
    }

    x_nodes = sqrt( header->num_nodes );
    y_nodes = header->num_nodes / x_nodes;

    ntQueue* waiting[header->num_nodes];
    ntQueue* inject[header->num_nodes];
    ntQueue* traverse[header->num_nodes];

    if( !reader_throttling ) {
        trace_packet = ntnetrace.nt_read_packet();
        ntnetrace.nt_print_packet(trace_packet);
    } else if( !ignore_dependencies ) {
        ntnetrace.nt_init_self_throttling();
    }


    for(;;){

        // Reset packets remaining check
        packets_left = 0;

        // Get packets for this cycle
        if( reader_throttling ) {
            nt_packet_list_t* list;
            for( list = ntnetrace.nt_get_cleared_packets_list(); list != NULL; list = list->next ) {
                if( list->node_packet != NULL ) {
                    trace_packet = list->node_packet;
                    queue_node_t* new_node = (queue_node_t*) malloc( sizeof(queue_node_t) );
                    new_node->packet = trace_packet;
                    new_node->cycle = (trace_packet->cycle > cycle) ? trace_packet->cycle : cycle;
                    inject[trace_packet->src]->queue_push( inject[trace_packet->src], new_node, new_node->cycle );
                } else {
                    printf( "Malformed packet list" );
                    exit(-1);
                }
            }
            ntnetrace.nt_empty_cleared_packets_list();
        } else {
            while( (trace_packet != NULL) && (trace_packet->cycle == cycle) ) {
                // Place in appropriate queue
                queue_node_t* new_node = (queue_node_t*) malloc( sizeof(queue_node_t) );
                new_node->packet = trace_packet;
                new_node->cycle = (trace_packet->cycle > cycle) ? trace_packet->cycle : cycle;
                if( ignore_dependencies || ntnetrace.nt_dependencies_cleared( trace_packet ) ) {
                    // Add to inject queue
                    std::cout << "HERE5a" << std::endl;
                    ntnetrace.nt_print_packet(new_node->packet);
                    ProcessingElementVC* pe = (ProcessingElementVC*) processingElements.at(trace_packet->src);
                    pe->ntInject.push(std::make_pair(*new_node, new_node->cycle));
                } else {
                    // Add to waiting queue
                    ProcessingElementVC* pe = (ProcessingElementVC*) processingElements.at(trace_packet->src);
                    pe->ntWaiting.push(std::make_pair(*new_node, new_node->cycle));
                }
                // Get another packet from trace
                std::cout << "HERE 6" << std::endl;
                trace_packet = ntnetrace.nt_read_packet();
            }
            if( (trace_packet != NULL) && (trace_packet->cycle < cycle) ) {
                // Error check: Crash and burn
                printf( "Invalid trace_packet cycle time: %llu, current cycle: %llu\n", trace_packet->cycle, cycle );
                exit(-1);
            }
        }


        //cout << "Netrace Pool is running!" << endl;
        //TODO read files from netrace, use update from anna

        //TODO set correct delay
        auto clockDelay = 1;
        event.notify(clockDelay, SC_NS);
        wait(event);
    }
}

void NetracePool::clear(Task*)
{
}

void NetracePool::start()
{
    int numOfPEs = processingElements.size();
    for (auto& task: globalResources.tasks) {
        processingElements.at(task.nodeID%numOfPEs)->execute(task);
    }
}