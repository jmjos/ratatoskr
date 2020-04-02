/*******************************************************************************
 * Copyright (C) 2018 joseph
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
#include <model/NoC.h>

ProcessingElementVC::ProcessingElementVC(sc_module_name mn, Node& node, TrafficPool* tp)
        :
        ProcessingElement(mn, node, tp)
{
    this->packetPortContainer = new PacketPortContainer(("NI_PACKET_CONTAINER"+std::to_string(this->id)).c_str());

    SC_THREAD(thread);
    SC_METHOD(receive);
    sensitive << packetPortContainer->portValidIn.pos();
}

void ProcessingElementVC::initialize()
{
    packetPortContainer->portValidOut.write(false);
    packetPortContainer->portFlowControlOut.write(true);
    // sc_spawn(sc_bind(&SyntheticPool::sendThread, this, con.first,
    // con.second,initDelay,sp.waveCount,sp.waveDelay,sp.pkgPerWave, sp.name));
}

void ProcessingElementVC::thread()
{
    for (;;) {
#ifndef ENABLE_NETRACE
        int timeStamp = static_cast<int>(sc_time_stamp().value()/1000);

        std::vector<DataDestination> removeList;

        for (auto const& tw : destWait) {
            DataDestination dest = tw.first;
            Task task = destToTask.at(dest);
            if (taskTerminationTime.count(task) && taskTerminationTime.at(task)<timeStamp) {
                removeList.push_back(dest);
            }
        }

        for (auto& dest : removeList) {
            Task task = destToTask.at(dest);
            destToTask.erase(dest);
            taskToDest.erase(task);
            taskRepeatLeft.erase(task);
            taskStartTime.erase(task);
            taskTerminationTime.erase(task);
            countLeft.erase(dest);
            destWait.erase(dest);
        }

        for (auto const& pair : destWait) {
            DataDestination dest = pair.first;

            if (pair.second<=timeStamp) {
                Task t = globalResources.tasks.at(dest.destinationTask);
                Node dstNode = globalResources.nodes.at(t.nodeID);
                Packet* p = packetFactory.createPacket(this->node, dstNode, globalResources.flitsPerPacket, sc_time_stamp().to_double(),
                        dest.dataType);

                packetPortContainer->portValidOut = true;
                packetPortContainer->portDataOut = p;

                countLeft.at(dest)--;

                if (!countLeft.at(dest)) {
                    countLeft.erase(dest);
                    destWait.erase(dest);

                    Task task = destToTask.at(dest);
                    destToTask.erase(dest);
                    taskToDest.at(task).erase(dest);

                    if (taskToDest.at(task).empty()) {
                        taskToDest.erase(task);
                        execute(task);
                    }
                }
                else {
                    destWait.at(dest) =
                            globalResources.getRandomIntBetween(dest.minInterval, dest.maxInterval)+timeStamp;
                }
                break;
            }
        }

        wait(SC_ZERO_TIME);
        packetPortContainer->portValidOut = false;

        int nextCall = -1;
        for (auto const& dw : destWait) {
            if (nextCall>dw.second || nextCall==-1) {
                /* In synthetic mode, we want to apply uniform_batch_mode experiment,
                 * that means all tasks need to send data once in one interval
                 * with some random offset in each interval.
                 */

                /* TODO:
                 * Attention: we are always taking the minStart and minInterval to calculate the nextCall.
                 * In the future, we may add randomness to the process,
                 * by selecting a number between minStart and maxStart,
                 * and the same thing for minInterval and maxInterval.
                 */
                if (globalResources.benchmark=="synthetic") {
                    Task task = this->destToTask.at(dw.first);
                    int minInterval = dw.first.minInterval;

                    if (timeStamp<task.minStart) {
                        nextCall = task.minStart+globalResources.getRandomIntBetween(0, minInterval-1);
                    }
                    else {
                        int numIntervalsPassed = (timeStamp-task.minStart)/minInterval;
                        int intervalBeginning = task.minStart+(numIntervalsPassed*minInterval);
                        nextCall = intervalBeginning+minInterval+globalResources.getRandomIntBetween(0, minInterval-1);
                    }
                }
                else { // if not synthetic, then execute the original behavior
                    nextCall = dw.second;
                }
            }
        }

        if (nextCall==0) { // limit packet rate
            nextCall = 1;
        }

        if (nextCall!=-1) {
            event.notify(nextCall-timeStamp, SC_NS);
        }

        wait(event);
#endif
//#ifdef ENABLE_NETRACE
        //definition of the netrace mode, in which the PE forwards packets to the NIs. Packets are generated in the central NetracePool.
        if (globalResources.netraceNodeToTask.find(this->node.id) != globalResources.netraceNodeToTask.end()){
            //cout << "PE "<< id <<" running in nettrace mode at timestamp" << sc_time_stamp() << endl;
            //TODO here we can add code to run netrace.
            auto clockDelay = this->node.type->clockDelay;
            event.notify(clockDelay, SC_NS);
            wait(event);
        } else{
            //cout << "Node with id" << node.id << " does not have a netrace task so can sleep quite some time @ " << sc_time_stamp()  << endl;
            event.notify(1000, SC_SEC);
            wait(event);
        }
//#endif
    }
}

void ProcessingElementVC::execute(Task& task)
{
    if (!taskRepeatLeft.count(task)) {
        taskRepeatLeft[task] = globalResources.getRandomIntBetween(task.minRepeat, task.maxRepeat);
    }
    else {
        if (taskRepeatLeft.at(task)>0) {
            taskRepeatLeft.at(task)--;
        }

        if (!taskRepeatLeft.at(task)) {
            taskRepeatLeft.erase(task);
            return;
        }
    }

    if (!taskStartTime.count(task)) {
        taskStartTime[task] = globalResources.getRandomIntBetween(task.minStart, task.maxStart);
    }

    if (!taskTerminationTime.count(task) && task.minDuration!=-1) {
        taskTerminationTime[task] =
                taskStartTime[task]+globalResources.getRandomIntBetween(task.minDuration, task.maxDuration);
    }

    if (task.requirements.empty()) {
        startSending(task);
    }
    else {
        for (DataRequirement& r : task.requirements) {
            neededFor[r.dataType].insert(task);
            neededAmount[std::make_pair(task, r.dataType)] = globalResources.getRandomIntBetween(r.minCount,
                    r.maxCount);
            needs[task].insert(r.dataType);
        }
    }
}

void ProcessingElementVC::bind(Connection* con, SignalContainer* sigContIn, SignalContainer* sigContOut)
{
    packetPortContainer->bind(sigContIn, sigContOut);
}

void ProcessingElementVC::receive()
{
    LOG(globalReport.verbose_pe_function_calls,
            "PE" << this->id << "(Node" << node.id << ")\t- receive_data_process()");

    if (packetPortContainer->portValidIn.posedge()) {
        Packet* received_packet = packetPortContainer->portDataIn.read();
        if (received_packet) {
            dataTypeID_t type = received_packet->dataType;

            if (receivedData.count(type)) {
                ++receivedData.at(type);
            }
            else {
                receivedData[type] = 1;
            }
            checkNeed();
            packetFactory.deletePacket(received_packet);
        }
    }
}

void ProcessingElementVC::startSending(Task& task)
{
    float rn = globalResources.getRandomFloatBetween(0, 1);
    int numOfPoss = task.possibilities.size();
    for (unsigned int i = 0; i<numOfPoss; ++i) {
        if (task.possibilities.at(i).probability>rn) {
            std::vector<DataDestination> destVec = task.possibilities.at(i).dataDestinations;
            for (DataDestination& dest : destVec) {
                destToTask[dest] = task;
                taskToDest[task].insert(dest);

                countLeft[dest] = globalResources.getRandomIntBetween(dest.minCount, dest.maxCount);

                int delayTime =
                        static_cast<int>((sc_time_stamp().value()/1000)
                                +globalResources.getRandomIntBetween(dest.minDelay, dest.maxDelay));

                if (taskStartTime.count(task) && taskStartTime.at(task)>delayTime) {
                    destWait[dest] = taskStartTime.at(task);
                }
                else {
                    destWait[dest] = delayTime;
                }
                event.notify(SC_ZERO_TIME);
            }
            break;
        }
        else {
            rn -= task.possibilities.at(i).probability;
        }
    }
}

void ProcessingElementVC::checkNeed()
{
    for (auto const& data : receivedData) {
        dataTypeID_t type = data.first;
        std::vector<std::pair<Task, dataTypeID_t>> removeList;

        if (neededFor.count(type)) {
            for (const Task& t : neededFor.at(type)) {
                std::pair<Task, dataTypeID_t> pair = std::make_pair(t, type);
                neededAmount.at(pair) -= receivedData.at(type);
                /* This line was commented out because if a task requires several packets from several data types,
                 it says that the task is finished receiving the required packets while in fact, it still needs some packets.
                 receivedData.at(type) = 0;
                 */
                if (neededAmount.at(pair)<=0) {
                    removeList.push_back(pair);
                    // This line is also commented out for the same reason mentioned above.
                    // receivedData.at(type) = -neededAmount.at(pair);
                }
            }
        }

        for (auto& p : removeList) {
            neededFor.erase(p.second);
            neededAmount.erase(p);
            needs.at(p.first).erase(p.second);

            if (needs.at(p.first).empty()) {
                startSending(p.first);
            }
        }
    }
}

ProcessingElementVC::~ProcessingElementVC()
{
    delete packetPortContainer;
}
