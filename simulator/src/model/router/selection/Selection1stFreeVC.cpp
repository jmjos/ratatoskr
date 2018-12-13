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
#include "Selection1stFreeVC.h"

void Selection1stFreeVC::select(RoutingInformation *ri, RoutingPacketInformation *rpi) {
    std::set<Channel> channel = rpi->routedChannel;
    if (!channel.empty()) {
        int selectedDirection = channel.begin()->dir; //not needed, general case of multiple directions
        if (node->dirOfConPos[selectedDirection] != DIR::Local) {
            channel = Helper::getChannelsWithDir({selectedDirection}, channel);
            int vcs = ri->vcCount.at(selectedDirection);
            for (int i = 0; i < vcs; i++) {
                Channel selChannel{selectedDirection, i};
                if (ri->freeVCs.at(selChannel)) {
                    rpi->selectedChannel = std::set<Channel>{selChannel};
                    rpi->recentSelectedChannel = std::set<Channel>{selChannel};
                    ri->freeVCs[selChannel] = false;
                    return;
                }
            }
        } else {
            Channel selChannel{selectedDirection, 0};
            rpi->selectedChannel = std::set<Channel>{selChannel};
            rpi->recentSelectedChannel = std::set<Channel>{selChannel};
        }
    }
};
