#include <random>

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
#include "SyntheticPool.h"

SyntheticPool::SyntheticPool()
{
    cout << endl;
    cout << "Synthetic Testrun" << endl;
}

void SyntheticPool::start()
{
    taskID_t taskId = 0;
    int phaseId = 0;
    dataTypeID_t dataTypeId = 0;
    dataDestID_t dataDestId = 0;
    int maxClockDelay = 1;
    for (auto const& nodeType: globalResources.nodeTypes) {
        if (nodeType->clockDelay>maxClockDelay)
            maxClockDelay = nodeType->clockDelay;
    }

    for (const SyntheticPhase& sp : globalResources.syntheticPhases) {
        LOG(true, "SyntheticPool\t Initialize phase \"" << sp.name << "\" with \"" << sp.distribution
                                                        << "\" distribution");

        std::map<int, int> srcToDst;
        if (sp.distribution=="uniform") {
            srcToDst = uniform(taskId, phaseId, dataTypeId, dataDestId, maxClockDelay, sp);
        }
        else if (sp.distribution=="bitComplement") {
            srcToDst = bitComplement();
        }
        else if (sp.distribution=="tornado") {
            srcToDst = tornado();
        }
        else if (sp.distribution=="transpose") {
            srcToDst = transpose();
        }
        else if (sp.distribution=="hotspot") {
            srcToDst = hotSpot(sp.hotspot);
        }
        else {
            FATAL("Distribution is not implemented");
        }
    }

/*		for (std::pair<const int, int> con : srcToDst) {
			DataType* dataType = new DataType(dataTypeId, std::to_string(dataTypeId));
			dataTypeId++;

			std::vector<DataDestination*> dests;
			DataDestination* dest = new DataDestination(dataDestId, dataType, processingElements.at(con.second)->node, sp->minInterval, sp->maxInterval);
			dest->minCount = sp->minCount;
			dest->maxCount = sp->maxCount;
			dest->minDelay = sp->minDelay;
			dest->maxDelay = sp->maxDelay;
			dests.push_back(dest);
			dataDestId++;

			std::vector<std::pair<float, std::vector<DataDestination*>>> possibilities;
			possibilities.push_back(std::make_pair(1, dests));

			Task* task = new Task(taskId, processingElements.at(con.first)->node);
			task->minStart = sp->minStart;
			task->maxStart = sp->maxStart;
			task->minDuration = sp->minDuration;
			task->maxDuration = sp->maxDuration;
			task->minRepeat = sp->minRepeat;
			task->maxRepeat = sp->maxRepeat;
			task->possibilities = possibilities;

			processingElements.at(con.first)->execute(task);
			taskId++;
		}*/
}

std::map<int, int>
SyntheticPool::uniform(taskID_t& taskId, int& phaseId, dataTypeID_t& dataTypeId,
        dataDestID_t& dataDestId, int maxClockDelay, const SyntheticPhase& sp)
{
    DataType dataType = DataType(dataTypeId, std::to_string(dataTypeId));
    dataTypeId++;

    for (unsigned int i = 0; i<processingElements.size(); i++) {
        Task* task = new Task(taskId, processingElements.at(i)->node.id);
        task->minStart = sp.minStart;
        task->maxStart = sp.maxStart;
        task->minDuration = sp.minDuration;
        task->maxDuration = sp.maxDuration;
        task->syntheticPhase = sp.id;

        int minInterval = static_cast<int>(std::floor((float) maxClockDelay/sp.injectionRate));
        int maxInterval = static_cast<int>(std::floor((float) maxClockDelay/sp.injectionRate));
        if (sp.minRepeat==-1 && sp.maxRepeat==-1) {
            task->minRepeat = static_cast<int>(std::floor(
                    (float) (task->minDuration-task->minStart)/(float) minInterval));
            task->maxRepeat = static_cast<int>(std::floor(
                    (float) (task->maxDuration-task->minStart)/(float) maxInterval));
        }
        else {
            task->minRepeat = sp.minRepeat;
            task->maxRepeat = sp.maxRepeat;
        }

        std::vector<DataSendPossibility> possibilities{};
        possID_t poss_id = 0;
        for (unsigned int j = 0; j<processingElements.size(); j++) {
            if (i!=j) { // a PE should not send data to itself.
                Node n = processingElements.at(j)->node;
                int minInterval = std::floor((float) maxClockDelay/sp.injectionRate);
                int maxInterval = std::floor((float) maxClockDelay/sp.injectionRate);

                std::vector<DataDestination> dests{};
                DataDestination dest = DataDestination(dataDestId, dataType.id, n.id, minInterval, maxInterval);
                dest.minCount = sp.minCount;
                dest.maxCount = sp.maxCount;
                dest.minDelay = sp.minDelay;
                dest.maxDelay = sp.maxDelay;
                dests.push_back(dest);
                possibilities.emplace_back(poss_id, 1.f/(processingElements.size()-1), dests);
                poss_id++;
                dataDestId++;
            }
        }
        task->possibilities = possibilities;
        tasks.push_back(task);
        taskId++;
    }
    shuffle_execute_tasks(tasks, phaseId);
    phaseId++;

    return std::map<int, int>();
}

std::map<int, int> SyntheticPool::bitComplement()
{
    std::map<int, int> srcToDst{};

    int nodes = processingElements.size();

    for (int i = 0; i<nodes; i++) {
        srcToDst[i] = nodes-i-1;
    }

    return srcToDst;
}

std::map<int, int> SyntheticPool::transpose()
{
    std::map<int, int> srcToDst{};
    int nodes = processingElements.size();

    for (int i = 0; i<nodes; i++) {
        srcToDst[i] = (i << int(ceil(log2(nodes)/2)))%(nodes-1);
    }

    return srcToDst;
}

std::map<int, int> SyntheticPool::tornado()
{
    std::map<int, int> srcToDst{};

    std::vector<float>* xPos = &globalResources.xPositions;
    std::vector<float>* yPos = &globalResources.yPositions;
    std::vector<float>* zPos = &globalResources.zPositions;

    int nodes = processingElements.size();
    for (int i = 0; i<nodes; i++) {
        Vec3D<float> srcPos = processingElements.at(i)->node.pos;
        Vec3D<float> dstPos;
        dstPos.x = xPos->at(((std::find(xPos->begin(), xPos->end(), srcPos.x)-xPos->begin())+(xPos->size() >> 1))%
                xPos->size());
        dstPos.y = yPos->at(((std::find(yPos->begin(), yPos->end(), srcPos.y)-yPos->begin())+(yPos->size() >> 1))%
                yPos->size());
        dstPos.z = zPos->at(((std::find(zPos->begin(), zPos->end(), srcPos.z)-zPos->begin())+(zPos->size() >> 1))%
                zPos->size());

        Node* dstNode = nullptr;
        std::vector<Node*> matching_nodes = globalResources.getNodesByPos(dstPos);
        for (auto& node : matching_nodes) {
            if (node->type->model=="ProcessingElement") {
                dstNode = node;
                break;
            }
        }
        if (dstNode) {
            srcToDst[i] = dstNode->id; // TODO: node's id or processing element id?
        }

    }

    return srcToDst;
}

std::map<int, int> SyntheticPool::hotSpot(int hotSpot)
{
    int nodes = processingElements.size();

    if (hotSpot==-1) {
        hotSpot = globalResources.getRandomIntBetween(0, nodes-1);
    }

    LOG(true, "\t\t Hotspot: " << hotSpot);

    std::map<int, int> srcToDst{};
    for (int i = 0; i<nodes; i++) {
        srcToDst[i] = hotSpot;
    }
    return srcToDst;
}

void SyntheticPool::clear(Task*)
{
}

void SyntheticPool::shuffle_execute_tasks(std::vector<Task*>& tasks, int phaseId)
{
    // Execute the tasks in random order
    int offset = phaseId*processingElements.size();
    auto start = tasks.begin()+offset;
    auto end = tasks.end();
    std::vector<Task*> phaseTasks(start, end);

    std::shuffle(phaseTasks.begin(), phaseTasks.end(), std::mt19937(std::random_device()()));
    for (Task* task : phaseTasks) {
        processingElements.at(task->nodeID%processingElements.size())->execute(*task);
    }
}

SyntheticPool::~SyntheticPool()
{
    for (auto& task:tasks)
        delete task;
    tasks.clear();
}
