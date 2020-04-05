/*
 * Copyright (c) 2010-2011 The University of Texas at Austin
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met: redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer;
 * redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in the
 * documentation and/or other materials provided with the distribution;
 * neither the name of the copyright holders nor the names of its
 * contributors may be used to endorse or promote products derived from
 * this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/*
 * Modified by Jan Moritz Joseph (c) 2020
 */


#include <iostream>
#include "ntNetrace.h"

ntNetrace::ntNetrace(){

}


void ntNetrace::nt_open_trfile( const char* trfilename ) {
    nt_close_trfile();
    int i;
    int length = 20;
    for( i = 0; trfilename[i] != 0; i++, length++ );
    nt_input_popencmd = (char*) malloc( length * sizeof(char) );
    sprintf( nt_input_popencmd, "bzip2 -dc %s", trfilename );
    nt_input_tracefile = popen( nt_input_popencmd, "r" );
    if( nt_input_tracefile == NULL ) {
        std::cout << "failed to open pipe to trace file" << std::endl;
    }
    nt_input_trheader = nt_read_trheader();
    if( nt_dependency_array == NULL ) {
        nt_dependency_array = (nt_dep_ref_node_t**) malloc(sizeof(nt_dep_ref_node_t*) * NT_DEPENDENCY_ARRAY_SIZE);
        memset( nt_dependency_array, 0, sizeof(nt_dep_ref_node_t*) * NT_DEPENDENCY_ARRAY_SIZE );
        nt_num_active_packets = 0;
    } else {
        std::cout << "dependency array not NULL on file open"<<std::endl;
    }
}

nt_header_t* ntNetrace::nt_read_trheader() {

#pragma pack(push,1)
    struct nt_header_pack {
        unsigned int nt_magic;
        float version;
        char benchmark_name[NT_BMARK_NAME_LENGTH];
        unsigned char num_nodes;
        unsigned char pad;
        unsigned long long int num_cycles;
        unsigned long long int num_packets;
        unsigned int notes_length;  // Includes null-terminating char
        unsigned int num_regions;
        char padding[8];
    };
#pragma pack(pop)

    int err = 0;
    char strerr[180];

    // Read Header
    struct nt_header_pack* in_header = (nt_header_pack*) malloc( sizeof(struct nt_header_pack) );
    fseek( nt_input_tracefile, 0, SEEK_SET );
    if( (err = fread( in_header, sizeof(struct nt_header_pack), 1, nt_input_tracefile )) < 0 ) {
        sprintf( strerr, "failed to read trace file header: err = %d", err );
        std::cout <<  strerr << std::endl;
    }

    // Copy data from struct to header
    nt_header_t* to_return = (nt_header_t*) malloc( sizeof(nt_header_t) );
    memset( to_return, 0, sizeof(nt_header_t) );
    to_return->nt_magic = in_header->nt_magic;
    to_return->version = in_header->version;
    strcpy( to_return->benchmark_name, in_header->benchmark_name );
    to_return->num_nodes = in_header->num_nodes;
    to_return->num_cycles = in_header->num_cycles;
    to_return->num_packets = in_header->num_packets;
    to_return->notes_length = in_header->notes_length;
    to_return->num_regions = in_header->num_regions;
    free( in_header );

    // Error Checking
    if( to_return->nt_magic != NT_MAGIC ) {
        if( nt_little_endian() != 1 ) {
            std::cout <<  "only little endian architectures are currently supported" << std::endl;
        } else {
            std::cout <<  "invalid trace file: bad magic" << std::endl;
        }
    }
    if( to_return->version != 1.0f ) {
        sprintf( strerr, "trace file is unsupported version: %f", to_return->version );
        std::cout <<  strerr <<std::endl;
    }
    // Read Rest of Header
    if( to_return->notes_length > 0 && to_return->notes_length < 8192 ) {
        to_return->notes = (char*) malloc( to_return->notes_length * sizeof(char) );
        if( (err = fread( to_return->notes, sizeof(char), to_return->notes_length, nt_input_tracefile )) < 0 ) {
            sprintf( strerr, "failed to read trace file header notes: err = %d\n", err );
            std::cout <<strerr << std::endl;
        }
    } else {
        to_return->notes = NULL;
    }
    if( to_return->num_regions > 0 ) {
        if( to_return->num_regions <= 100 ) {
            to_return->regions = (nt_regionhead_t*) malloc( to_return->num_regions * sizeof(nt_regionhead_t) );
            if( (err = fread( to_return->regions, sizeof(nt_regionhead_t), to_return->num_regions, nt_input_tracefile )) < 0 ) {
                sprintf( strerr, "failed to read trace file header regions: error = %d\n", err );
                std::cout <<  strerr << std::endl;
            }
        } else {
            std::cout <<  "lots of regions... is this correct?" << std::endl;
        }
    } else {
        to_return->regions = NULL;
    }
    return to_return;
}

void ntNetrace::nt_disable_dependencies() {
    if( nt_track_cleared_packets_list ) {
        std::cout <<  "Cannot turn off dependencies when tracking cleared packets list" << std::endl;
    }
    nt_dependencies_off = 1;
}

void ntNetrace::nt_seek_region( nt_regionhead_t* region ) {
    int err = 0;
    char strerr[180];
    if( nt_input_tracefile != NULL ) {
        if( region != NULL ) {
            // Clear all existing dependencies
            nt_delete_all_dependencies();
            // Reopen file to fast-forward to region
            // fseek doesn't work on compressed file
            pclose( nt_input_tracefile );
            nt_input_tracefile = popen( nt_input_popencmd, "r" );
            unsigned long long int seek_offset = nt_get_headersize() + region->seek_offset;
            unsigned long long int read_length = 4096;
            char* buffer = (char*) malloc( read_length );
            while( seek_offset > read_length ) {
                if( (err = fread( buffer, 1, read_length, nt_input_tracefile )) < 0 ) {
                    sprintf( strerr, "failed to seek region: error = %d\n", err );
                    std::cout <<  strerr << std::endl;
                }
                seek_offset -= read_length;
            }
            if( (err = fread( buffer, 1, seek_offset, nt_input_tracefile )) < 0 ) {
                sprintf( strerr, "failed to seek region: error = %d\n", err );
                std::cout <<  strerr << std::endl;
            }
            free( buffer );
            if( nt_self_throttling ) {
                // Prime the pump to read in self throttled packets
                nt_prime_self_throttle();
            }
        } else {
            std::cout <<  "invalid region passed: NULL" <<std::endl;
        }
    } else {
        std::cout <<  "must open trace file with nt_open_trfile before seeking" << std::endl;
    }
}

nt_packet_t* ntNetrace::nt_read_packet( void ) {

#pragma pack(push,1)
    struct nt_packet_pack {
        unsigned long long int cycle;
        unsigned int id;
        unsigned int addr;
        unsigned char type;
        unsigned char src;
        unsigned char dst;
        unsigned char node_types;
        unsigned char num_deps;
    };
#pragma pack(pop)

    int err = 0;
    unsigned int i;
    char strerr[180];
    nt_packet_t* to_return = NULL;
    if( nt_input_tracefile != NULL ) {
        to_return = nt_packet_malloc();
        if( (err = fread( to_return, 1, sizeof(struct nt_packet_pack), nt_input_tracefile )) < 0 ) {
            sprintf( strerr, "failed to read packet: err = %d", err );
            std::cout <<  strerr << std::endl;
        }
        if( err > 0 && err < sizeof(struct nt_packet_pack) ) {
            // Bad packet - end of file
            std::cout <<  "unexpectedly reached end of trace file - perhaps corrupt" << std::endl;
        } else if( err == 0 ) {
            // End of file
            free( to_return );
            to_return = NULL;
            return to_return;
        }
        if( !nt_dependencies_off ) {
            // Track dependencies: add to_return to dependencies array
            nt_dep_ref_node_t* node_ptr = nt_get_dependency_node( to_return->id );
            if( node_ptr == NULL ) {
                node_ptr = nt_add_dependency_node( to_return->id );
            }
            node_ptr->node_packet = to_return;
        }
        nt_num_active_packets++;
        nt_latest_active_packet_cycle = to_return->cycle;
        if( to_return->num_deps == 0 ) {
            to_return->deps = NULL;
        } else {
            to_return->deps = nt_dependency_malloc( to_return->num_deps );
            if( (err = fread( to_return->deps, sizeof(nt_dependency_t), to_return->num_deps, nt_input_tracefile )) < 0 ) {
                sprintf( strerr, "failed to read dependencies: err = %d", err );
                std::cout <<  strerr << std::endl;
            }
            if( !nt_dependencies_off ) {
                // Track dependencies: add to_return downward dependencies to array
                for( i = 0; i < to_return->num_deps; i++ ) {
                    unsigned int dep_id = to_return->deps[i];
                    nt_dep_ref_node_t* node_ptr = nt_get_dependency_node( dep_id );
                    if( node_ptr == NULL ) {
                        node_ptr = nt_add_dependency_node( dep_id );
                    }
                    node_ptr->ref_count++;
                }
            }
        }
    } else {
        std::cout <<  "must open trace file with nt_open_trfile before reading" << std::endl;
    }
    return to_return;
}

nt_dep_ref_node_t* ntNetrace::nt_add_dependency_node( unsigned int packet_id ) {
    if( nt_dependency_array != NULL ) {
        unsigned int index = packet_id % NT_DEPENDENCY_ARRAY_SIZE;
        nt_dep_ref_node_t* dep_ptr = nt_dependency_array[index];
        if( dep_ptr == NULL ) {
            nt_dependency_array[index] = (nt_dep_ref_node_t*) malloc( sizeof(nt_dep_ref_node_t) );
            dep_ptr = nt_dependency_array[index];
        } else {
            for( ; dep_ptr->next_node != NULL; dep_ptr = dep_ptr->next_node );
            dep_ptr->next_node = (nt_dep_ref_node_t*) malloc( sizeof(nt_dep_ref_node_t) );
            dep_ptr = dep_ptr->next_node;
        }
        dep_ptr->node_packet = NULL;
        dep_ptr->packet_id = packet_id;
        dep_ptr->ref_count = 0;
        dep_ptr->next_node = NULL;
        return dep_ptr;
    } else {
        std::cout <<   "dependency array NULL on ntNode addition" <<std::endl;
    }
    return NULL;
}

void ntNetrace::nt_read_ahead( unsigned long long int current_cycle ) {
    unsigned long long int read_to_cycle = current_cycle + NT_READ_AHEAD;
    if( read_to_cycle < current_cycle ) {
        std::cout <<   "trying to read too far ahead... overflowed :(" <<std::endl;
    }
    if( read_to_cycle > nt_latest_active_packet_cycle ) {
        nt_packet_t* packet;
        while( nt_latest_active_packet_cycle <= read_to_cycle && !nt_done_reading ) {
            packet = nt_read_packet();
            if( packet == NULL ) {
                // This is the exit condition... how do we signal it to the
                // network simulator? We shouldn't need to... It is tracking
                // whether there are packets in flight.
                // Just in case, we'll provide this global indicator
                nt_done_reading = 1;
            } else if( nt_dependencies_cleared( packet ) ) {
                nt_add_cleared_packet_to_list( packet );
                //} else {
                // Ignore this packet, since the reader is already tracking it
            }
        }
    }
}

nt_packet_t* ntNetrace::nt_remove_dependency_node( unsigned int packet_id ) {
    if( nt_dependency_array != NULL ) {
        unsigned int index = packet_id % NT_DEPENDENCY_ARRAY_SIZE;
        nt_dep_ref_node_t* dep_ptr = nt_dependency_array[index];
        if( dep_ptr == NULL ) {
            return NULL;
        } else {
            nt_dep_ref_node_t* prev_ptr = NULL;
            for( ; dep_ptr != NULL; prev_ptr = dep_ptr, dep_ptr = dep_ptr->next_node ) {
                if( dep_ptr->packet_id == packet_id ) break;
            }
            if( dep_ptr == NULL ) {
                return NULL;
            }
            if( prev_ptr == NULL ) {
                nt_dependency_array[index] = dep_ptr->next_node;
            } else {
                prev_ptr->next_node = dep_ptr->next_node;
            }
            nt_packet_t* packet = dep_ptr->node_packet;
            free( dep_ptr );
            return packet;
        }
    } else {
        std::cout <<   "dependency array NULL on ntNode remove" <<std::endl;
    }
    return NULL;
}

nt_dep_ref_node_t* ntNetrace::nt_get_dependency_node( unsigned int packet_id ) {
    if( nt_dependency_array != NULL ) {
        unsigned int index = packet_id % NT_DEPENDENCY_ARRAY_SIZE;
        nt_dep_ref_node_t* dep_ptr;
        for( dep_ptr = nt_dependency_array[index]; dep_ptr != NULL; dep_ptr = dep_ptr->next_node ) {
            if( dep_ptr->packet_id == packet_id ) break;
        }
        return dep_ptr;
    } else {
        std::cout <<   "dependency array not NULL on ntNode search" << std::endl;
    }
    return NULL;
}

int ntNetrace::nt_dependencies_cleared( nt_packet_t* packet ) {
    if( nt_input_tracefile != NULL ) {
        nt_dep_ref_node_t* node_ptr = nt_get_dependency_node( packet->id );
        if( node_ptr == NULL || nt_dependencies_off ) {
            return 1;
        } else {
            return (node_ptr->ref_count == 0);
        }
    } else {
        std::cout <<   "must open trace file with nt_open_trfile before injecting" << std::endl;
    }
    return 1;
}

void ntNetrace::nt_clear_dependencies_free_packet( nt_packet_t* packet ) {
    unsigned int i;
    if( nt_input_tracefile != NULL ) {
        if( packet != NULL ) {
            // If self-throttling, read ahead in the trace file
            // to ensure that there are new packets ready to go
            if( nt_self_throttling ) {
                nt_read_ahead( packet->cycle );
            }
            for( i = 0; i < packet->num_deps; i++ ) {
                unsigned int dep_id = packet->deps[i];
                nt_dep_ref_node_t* node_ptr = nt_get_dependency_node( dep_id );
                if( node_ptr == NULL ) {
                    if( !nt_dependencies_off ) {
                        // TODO: check if this is a problem with short seeks
                        nt_print_packet( packet );
                        std::cout <<   "failed to find dependency ntNode" << std::endl;
                    }
                } else {
                    if( node_ptr->ref_count == 0 ) {
                        std::cout <<   "invalid reference count on ntNode while decrementing" << std::endl;
                    }
                    node_ptr->ref_count--;
                    if( nt_track_cleared_packets_list ) {
                        if( node_ptr->ref_count == 0 ) {
                            // This test alleviates the possibility of a packet
                            // having ref_count zero before it has been read
                            // from the trace (node_packet = NULL)
                            if( node_ptr->node_packet ) {
                                nt_add_cleared_packet_to_list( node_ptr->node_packet );
                            }
                        }
                    }
                }
            }
            nt_remove_dependency_node( packet->id );
            nt_packet_free( packet );
            nt_num_active_packets--;
        }
    } else {
        std::cout <<   "must open trace file with nt_open_trfile before ejecting" << std::endl;
    }
}

void ntNetrace::nt_init_cleared_packets_list() {
    if( nt_dependencies_off ) {
        std::cout <<   "Cannot return cleared packets list when dependencies are turned off" << std::endl;
    }
    nt_track_cleared_packets_list = 1;
    nt_cleared_packets_list = NULL;
    nt_cleared_packets_list_tail = NULL;
}

void ntNetrace::nt_init_self_throttling() {
    if( nt_dependencies_off ) {
        std::cout <<   "Cannot self throttle packets when dependencies are turned off" << std::endl;
    }
    nt_self_throttling = 1;
    nt_primed_self_throttle = 0;
    nt_init_cleared_packets_list();
}

nt_packet_list_t* ntNetrace::nt_get_cleared_packets_list() {
    if( !nt_primed_self_throttle ) {
        nt_prime_self_throttle();
    }
    return nt_cleared_packets_list;
}

void ntNetrace::nt_prime_self_throttle() {
    nt_packet_t* packet = nt_read_packet();
    if( nt_dependencies_cleared( packet ) ) {
        nt_add_cleared_packet_to_list( packet );
    }
    nt_primed_self_throttle = 1;
    nt_read_ahead( packet->cycle );
}

void ntNetrace::nt_add_cleared_packet_to_list( nt_packet_t* packet ) {
    nt_packet_list_t* new_node = (nt_packet_list_t*) malloc( sizeof(nt_packet_list_t) );
    new_node->node_packet = packet;
    new_node->next = NULL;
    if( nt_cleared_packets_list == NULL ) {
        nt_cleared_packets_list = nt_cleared_packets_list_tail = new_node;
    } else {
        nt_cleared_packets_list_tail->next = new_node;
        nt_cleared_packets_list_tail = new_node;
    }
}

void ntNetrace::nt_empty_cleared_packets_list() {
    while( nt_cleared_packets_list != NULL ) {
        nt_packet_list_t* temp = nt_cleared_packets_list;
        nt_cleared_packets_list = nt_cleared_packets_list->next;
        free( temp );
    }
    nt_cleared_packets_list = nt_cleared_packets_list_tail = NULL;
}

void ntNetrace::nt_close_trfile() {
    if( nt_input_tracefile != NULL ) {
        pclose( nt_input_tracefile );
        nt_input_tracefile = NULL;
        nt_free_trheader( nt_input_trheader );
        if( nt_input_popencmd != NULL ) {
            free( nt_input_popencmd );
        }
        nt_input_popencmd = NULL;
        nt_delete_all_dependencies();
        free(nt_dependency_array);
        nt_dependency_array = NULL;
    }
}

void ntNetrace::nt_delete_all_dependencies() {
    int i;
    for( i = 0; i < NT_DEPENDENCY_ARRAY_SIZE; i++ ) {
        while( nt_dependency_array[i] != NULL ) {
            nt_remove_dependency_node( nt_dependency_array[i]->packet_id );
        }
    }
}

void ntNetrace::nt_print_header( nt_header_t* header ) {
    unsigned int i;
    if( header != NULL ) {
        printf( "NT_TRACEFILE---------------------\n" );

        printf( "  Benchmark: %s\n", header->benchmark_name );
        printf( "  Magic Correct? %s\n", (header->nt_magic == NT_MAGIC) ? "TRUE" : "FALSE" );
        printf( "  Tracefile Version: v%1.1f\n", header->version );
        printf( "  Number of Program Regions: %d\n", header->num_regions );
        printf( "  Number of Simulated Nodes: %d\n", header->num_nodes );
        printf( "  Simulated Cycles: %lld\n", header->num_cycles );
        printf( "  Simulated Packets: %lld\n", header->num_packets );
        printf( "  Average injection rate: %f\n", (double)header->num_packets / (double)header->num_cycles );
        if( header->notes_length > 0 ) {
            printf( "  Notes: %s\n", header->notes );
        }

        for( i = 0; i < header->num_regions; i++ ) {
            printf( "    Region %d:\n", i );
            printf( "      Seek Offset: %lld\n", header->regions[i].seek_offset );
            printf( "      Simulated Cycles: %lld\n", header->regions[i].num_cycles );
            printf( "      Simulated Packets: %lld\n", header->regions[i].num_packets );
            printf( "      Average injection rate: %f\n", (double)header->regions[i].num_packets / (double)header->regions[i].num_cycles );
            printf( "      Average injection rate per ntNode: %f\n", (double)header->regions[i].num_packets / (double)header->regions[i].num_cycles / (double)header->num_nodes );
        }
        printf( "  Size of header (B): %u\n", nt_get_headersize() );
        printf( "NT_TRACEFILE---------------------\n" );
    } else {
        printf( "NULL header passed to nt_print_header\n" );
    }
}

void ntNetrace::nt_print_trheader() {
    nt_print_header( nt_input_trheader );
}

nt_header_t* ntNetrace::nt_get_trheader() {
    if( nt_input_tracefile != NULL ) {
        return nt_input_trheader;
    } else {
        std::cout <<  "must open trace file with nt_open_trfile before header is available" <<std::endl;
    }
    return NULL;
}

float ntNetrace::nt_get_trversion() {
    if( nt_input_tracefile != NULL ) {
        return nt_input_trheader->version;
    } else {
        std::cout <<  "must open trace file with nt_open_trfile before version is available" <<std::endl;
    }
    return 0.0f;
}

nt_packet_t* ntNetrace::nt_packet_malloc() {
    // Allocate large array (1+ pages) to reduce # system calls
    return (nt_packet_t*) malloc( sizeof(nt_packet_t) );
}

nt_dependency_t* ntNetrace::nt_dependency_malloc( unsigned char num_deps ) {
    // Allocate large array (1+ pages) to reduce # system calls
    return (nt_dependency_t*) malloc( num_deps * sizeof(nt_dependency_t) );
}

int ntNetrace::nt_get_src_type( nt_packet_t* packet ) {
    return (int) ( packet->node_types >> 4 );
}

int ntNetrace::nt_get_dst_type( nt_packet_t* packet ) {
    return (int) ( 0xF & packet->node_types );
}

const std::string ntNetrace::nt_node_type_to_string( int type ) {
    if( type < NT_NUM_NODE_TYPES ) {
        return nt_node_types[type];
    } else {
        return nt_node_types[NT_NUM_NODE_TYPES];
    }
}

int ntNetrace::nt_get_packet_size( nt_packet_t* packet ) {
    if( packet->type < NT_NUM_PACKET_TYPES ) {
        return nt_packet_sizes[packet->type];
    } else {
        return nt_packet_sizes[0];
    }
}

const std::string ntNetrace::nt_packet_type_to_string( nt_packet_t* packet ) {
    if( packet->type < NT_NUM_PACKET_TYPES ) {
        return nt_packet_types[packet->type];
    } else {
        return nt_packet_types[0];
    }
}

nt_packet_t* ntNetrace::nt_packet_copy( nt_packet_t* packet ) {
    if( packet != NULL ) {
        nt_packet_t* to_return = nt_packet_malloc();
        memcpy( to_return, packet, sizeof(nt_packet_t) );
        if( packet->num_deps > 0 ) {
            to_return->deps = nt_dependency_malloc( to_return->num_deps );
            memcpy( to_return->deps, packet->deps, sizeof(nt_dependency_t) * to_return->num_deps );
        }
        return to_return;
    }
    return NULL;
}

void ntNetrace::nt_packet_free( nt_packet_t* packet ) {
    if( packet != NULL ) {
        if( packet->num_deps > 0 ) {
            free( packet->deps );
        }
        free( packet );
    }
}

void ntNetrace::nt_print_packet( nt_packet_t* packet ) {
    int i;
    if( packet != NULL ) {
        std::cout << "  ID:" << packet->id << "CYC: " << packet->cycle << " SRC: " << packet->src << "DST: " << packet->dst << "ADR: " << packet->addr << "TYP: " << nt_packet_type_to_string( packet ) << " NDEP: "<< packet->num_deps << std::endl;
        for( i = 0; i < packet->num_deps; i++ ) {
            printf( " %d", packet->deps[i] );
        }
        printf( "\n" );
    } else {
        printf( "WARNING: %s:%d: NULL packet printed!\n", __FILE__, __LINE__ );
    }
}

void ntNetrace::nt_free_trheader( nt_header_t* header ) {
    if( header != NULL ) {
        if( header->regions != NULL ) {
            free( header->regions );
        }
        if( header->notes != NULL ) {
            free( header->notes );
        }
        free( header );
    }
}

int ntNetrace::nt_get_headersize() {

#pragma pack(push,1)
    struct nt_header_pack {
        unsigned int nt_magic;
        float version;
        char benchmark_name[NT_BMARK_NAME_LENGTH];
        unsigned char num_nodes;
        unsigned long long int num_cycles;
        unsigned long long int num_packets;
        unsigned int notes_length;  // Includes null-terminating char
        unsigned int num_regions;
        char padding[9];
    };
#pragma pack(pop)

    if( nt_input_tracefile != NULL ) {
        int to_return = 0;
        to_return += sizeof(struct nt_header_pack);
        to_return += nt_input_trheader->notes_length;
        to_return += nt_input_trheader->num_regions * sizeof(nt_regionhead_t);
        return to_return;
    } else {
        std::cout << "must open trace file with nt_open_trfile before header is available" <<std::endl;
    }
    return -1;
}

void* ntNetrace::_nt_checked_malloc( size_t n, char* file, int line ) {
    void* ptr;
    ptr = malloc( n );
    if( ptr == NULL ) {
        fprintf( stderr, "ERROR: bad allocation at %s:%d\n", file, line );
        exit(0);
    }
    return ptr;
}

int ntNetrace::nt_little_endian() {
    int to_return = 1;
    union {
        int number;
        char bytes[sizeof(int)];
    } u;
    unsigned int i;
    u.number = (int)0;
    for( i = 0; i < sizeof(int); i++ ) {
        u.number |= (int)(i + 1) << (8 * i);
    }
    for( i = 0; i < sizeof(int); i++ ) {
        if( (unsigned int)u.bytes[i] != i+1 ) {
            to_return = 0;
            break;
        }
    }
    return to_return;
}

void ntNetrace::_nt_error( const char* str, char* file, int line ) {
#ifdef DEBUG_ON
    fprintf( stderr, "WARNING: In %s:%d: %s\n", file, line, str );
#else
    fprintf( stderr, "ERROR: In %s:%d: %s\n", file, line, str );
    exit(0);
#endif
}

// Backend functions for creating trace files
void ntNetrace::nt_dump_header( nt_header_t* header, FILE* fp ) {

#pragma pack(push,1)
    struct nt_header_pack {
        unsigned int nt_magic;
        float version;
        char benchmark_name[NT_BMARK_NAME_LENGTH];
        unsigned char num_nodes;
        unsigned char pad;
        unsigned long long int num_cycles;
        unsigned long long int num_packets;
        unsigned int notes_length;  // Includes null-terminating char
        unsigned int num_regions;
        char padding[8];
    };
#pragma pack(pop)

    if( header != NULL ) {
        struct nt_header_pack* out_header = (nt_header_pack*) malloc( sizeof(struct nt_header_pack) );
        out_header->nt_magic = header->nt_magic;
        out_header->version = header->version;
        strcpy( out_header->benchmark_name, header->benchmark_name );
        out_header->num_nodes = header->num_nodes;
        out_header->num_cycles = header->num_cycles;
        out_header->num_packets = header->num_packets;
        out_header->notes_length = header->notes_length;
        out_header->num_regions = header->num_regions;
        fwrite( out_header, sizeof(struct nt_header_pack), 1, fp );
        fwrite( header->notes, sizeof(char), header->notes_length, fp );
        fwrite( header->regions, sizeof(nt_regionhead_t), header->num_regions, fp );
        free( out_header );
    } else {
        std::cout<< "dumping NULL header" <<std::endl;
    }
}

void ntNetrace::nt_dump_packet( nt_packet_t* packet, FILE* fp ) {

#pragma pack(push,1)
    struct nt_read_pack {
        unsigned long long int cycle;
        unsigned int id;
        unsigned int addr;
        unsigned char type;
        unsigned char src;
        unsigned char dst;
        unsigned char node_types;
        unsigned char num_deps;
    };
#pragma pack(pop)

    if( packet != NULL ) {
        fwrite( packet, sizeof(struct nt_read_pack), 1, fp );
        fwrite( packet->deps, sizeof(nt_dependency_t), packet->num_deps, fp );
    } else {
        std::cout << "dumping NULL packet" <<std::endl;
    }
}
