//
// Created by Nick on 2020/7/17.
//
//
#include <string>
#include "EventLoop.h"
#include "Server.h"
#include "base/Logging.h"

int main() {
    int threadNum = 4;
    int port = 80;
    std::string logPath = "./WebServer.log";
    Logger::setLogFileName(logPath);
    EventLoop mainLoop;
    Server myHTTPServer(&mainLoop, threadNum, port);
    myHTTPServer.start();
    return 0;
}