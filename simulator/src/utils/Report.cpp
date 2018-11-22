////////////////////////////////////////////////////////////////////////////////
// Copyright (C) 2018 Jan Moritz Joseph
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
////////////////////////////////////////////////////////////////////////////////
#include "Report.h"

Report::Report() {
	logfile.open("log.txt");
}

void Report::connect(std::string addr, std::string port) {
	networkDisabled = true;
	struct addrinfo addrInfoReq;
	struct addrinfo *addrInfo;
	memset(&addrInfoReq, 0, sizeof addrInfoReq);

	addrInfoReq.ai_family = AF_UNSPEC;
	addrInfoReq.ai_socktype = SOCK_STREAM;

	int error = getaddrinfo(addr.c_str(), port.c_str(), &addrInfoReq, &addrInfo);
	if (error != 0) {
		std::cout << "Report: getaddrinfo() failed -> " << gai_strerror(error) << std::endl;
	} else {
		socketfd = socket(addrInfo->ai_family, addrInfo->ai_socktype, addrInfo->ai_protocol);
		if (socketfd == -1) {
			std::cout << "Report: socket() failed -> " << std::strerror(errno) << std::endl;
		} else if (::connect(socketfd, addrInfo->ai_addr, addrInfo->ai_addrlen) == -1) {
			std::cout << "Report: connect() failed -> " << std::strerror(errno) << std::endl;
		} else {
			networkDisabled = false;
		}
	}

	if (networkDisabled) {
		std::cout << "Disabling database reports" << std::endl;
	}

	freeaddrinfo(addrInfo);

	dbid = registerElement("Report", 0);

}

void Report::close() {
	logfile.close();
	send();
	::close(socketfd);
	networkDisabled = true;
}

void Report::startRun(std::string name) {
	addToSendBuffer("run;" + name);
}

int Report::registerElement(std::string type, int id) {
	addToSendBuffer("reg;" + std::to_string(element_count) + ";" + type + ";" + std::to_string(id));
	return element_count++;
}

void Report::reportEvent(int element_id, std::string event, std::string data) {
	addToSendBuffer("rep;" + std::to_string(element_id) + ";" + std::to_string(sc_time_stamp().value()) + ";" + event + ";" + data);
}

void Report::reportAttribute(int element_id, std::string name, std::string value) {
	addToSendBuffer("att;" + std::to_string(element_id) + ";" + name + ";" + value);
}

void Report::log(bool qualifier, std::string message, int type) {
	if (qualifier) {
		if (type & Logtype::COUT) {
			std::cout  << std::setfill(' ') << std::setw(5)<< sc_time_stamp().value()/1000 << "ns: " << message << std::endl;
		}
		if (type & Logtype::CERR) {
			std::cerr << std::setfill(' ') << std::setw(5)<< sc_time_stamp().value()/1000 << "ns: " << message << std::endl;
		}
		if (type & Logtype::LOGFILE) {
			logfile << std::setfill(' ') << std::setw(5)<< sc_time_stamp().value()/1000 << "ns: " << message << std::endl;
		}
		if (type & Logtype::DB) {
			reportEvent(dbid, "log", message);
		}
	}
}

void Report::addToSendBuffer(std::string str) {
	if (!networkDisabled && sendBuffer.length() + str.length() >= MAX_BUFFER_SIZE) {
		send();
		sendBuffer = "";
	}
	sendBuffer += str + "|";
}

void Report::send() {
	try {
		//::send(socketfd, sendBuffer.c_str(), sendBuffer.length(), 0);
	}
	catch (std::exception) {
		std::cout << "Can't send the report!" << std::endl;
	}
}
