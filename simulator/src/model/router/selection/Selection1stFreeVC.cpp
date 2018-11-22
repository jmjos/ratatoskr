#include "Selection1stFreeVC.h"

void Selection1stFreeVC::select(RoutingInformation *ri, RoutingPacketInformation *rpi) {
    std::set<Channel> channel = rpi->routedChannel;
    if (!channel.empty()) {
        int selectedDirection = channel.begin()->dir; //not needed, general case of multiple directions
        if (node->conToDir[selectedDirection] != DIR::Local) {
            channel = Helper::getChannelWithDir({selectedDirection}, channel);
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
