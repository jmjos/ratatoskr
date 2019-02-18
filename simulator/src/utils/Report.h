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

#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <cerrno>
#include <fstream>
#include <iostream>
#include <iomanip>
#include "systemc.h"

#define MAX_BUFFER_SIZE 1000    //Max Buffer Size in Bytes
#define LOG(x, y) { if(x) {std::ostringstream oss; oss<<y; Report::getInstance().log(x,oss.str());}}
#define FATAL(x) { LOG(true,x); std::cout<<"Terminating"<<std::endl; Report::getInstance().close(); exit(EXIT_FAILURE);}

enum Logtype {
    COUT = 1 << 0,
    CERR = 1 << 1,
    LOGFILE = 1 << 2,
    DB = 1 << 3
};

class Report {

public:
    static Report& getInstance()
    {
        static Report instance;
        return instance;
    }

    void connect(const std::string& server, const std::string& port);

    void startRun(const std::string& name);

    int registerElement(const std::string& type, int id);

    void reportEvent(int element_id, const std::string& event, const std::string& data);

    void reportAttribute(int element_id, const std::string& name, const std::string& value);

    void log(bool qualifier, const std::string& message, int type = COUT | DB);

    void close();

private:
    bool networkDisabled = true;
    int socketfd = 0;
    int element_count = 0;
    std::ofstream logfile;
    std::string sendBuffer;
    int dbid = 0;

    Report();

    void addToSendBuffer(const std::string& str);

    void send();

};
