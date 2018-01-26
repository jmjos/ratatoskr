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
#ifndef SRC_MODEL_SELECTION_SELECTIONROUNDROBIN_H_
#define SRC_MODEL_SELECTION_SELECTIONROUNDROBIN_H_


#include "Selection.h"

struct SelectionRoundRobin : public Selection {
	int rrVC=0;

	SelectionRoundRobin(Node* node):Selection(node){

	};
	~SelectionRoundRobin(){

	};
	//void checkValid()=0;
	void select(RoutingInformation* ri, RoutingPacketInformation* rpi){
		if(!rpi->availableChannel.empty()){
			rpi->availableChannel = MESH::getChannelWithDir({rpi->availableChannel.begin()->dir}, rpi->availableChannel);
			int vcs = ri->vcCount.at(rpi->availableChannel.begin()->dir);

			for(int i= 0; i<vcs; i++){
				std::set<Channel> channel = MESH::getChannelWithVC({(rrVC+i)%vcs}, rpi->availableChannel);
				if(!channel.empty()){
					rpi->availableChannel = channel;
					return;
				}
			}
		}
		rrVC++;
	};
};


#endif /* SRC_MODEL_SELECTION_SELECTIONROUNDROBIN_H_ */
