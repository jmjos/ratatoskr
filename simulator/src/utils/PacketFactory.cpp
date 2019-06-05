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

#include "PacketFactory.h"

PacketFactory& PacketFactory::getInstance()
{
    static PacketFactory instance;
    return instance;
}

Packet* PacketFactory::createPacket(Node& src, Node& dst, int size, double generationTime, dataTypeID_t dataType)
{
    auto p = new Packet(src, dst, 1, sc_time_stamp().to_double(), dataType);
    packets.push_back(p);
    return p;
}

void PacketFactory::deletePacket(Packet* p)
{
    auto it = std::find(packets.begin(), packets.end(), p);
    delete (*it);
    packets.erase(it);
}
