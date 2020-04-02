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

#include "NetracePool.h"

NetracePool::NetracePool() : sc_module("NetracePool")
{
    cout << endl;
    cout << "Netrace Benchmark" << endl;
    SC_THREAD(thread);
}

NetracePool::~NetracePool()
{
}

void NetracePool::thread() {
    for(;;){
        cout << "Netrace Pool is running!" << endl;
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