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

#pragma once

#include "Router.h"

class RouterVC : public Router {
public:
    std::map<Channel, int> creditCounter;
    std::map<Channel, std::array<int, 4>> lastReceivedCreditID;
    std::map<int, std::vector<int>> VCAllocation_inputVC_rrList;
    std::map<int, std::vector<int>> switchAllocation_inputVC_rrList;
    std::map<int, int> switchAllocation_outputVCPtr;

    RouterVC(sc_module_name nm, Node& node);

    ~RouterVC();

    void initialize() override;

    void bind(Connection*, SignalContainer*, SignalContainer*) override;

    void receive() override;

    void thread() override;

    void updateUsageStats() override;

    void send();

    std::map<int, std::vector<Channel>> VCAllocation_generateRequests();

    void VCAllocation_generateAck(const std::map<int, std::vector<Channel>>& requests);

    int VCAllocation_getNextFreeVC(int out);

    int VCAllocation_getNextVCToBeAllocated(int in);

    std::map<int, std::vector<Channel>> switchAllocation_generateRequests();

    void switchAllocation_generateAck(const std::map<int, std::vector<Channel>>& requests);

    void receiveFlowControlCredit() override;

private:

    std::vector<int> getAllocatedVCsOfInDir(int conPos);

    std::vector<int> getAllocatedVCsOfOutDir(int conPos);

    std::vector<int> generateVCsFromPtr(int direction, std::map<int, int> vcOffset);

    void insert_request(int out_conPos, Channel in, std::map<int, std::vector<Channel>>& requests);

    void send_one_flit(BufferFIFO<Flit*>* buf, Channel in, Channel out);

    void send_multi_flits(BufferFIFO<Flit*>* buf, Channel in, Channel out);

    void receive_one_flit();

    void receive_multi_flits();
};