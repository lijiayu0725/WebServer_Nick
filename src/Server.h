//
// Created by Nick on 2020/7/23.
//

#ifndef WEBSERVER_NICK_SERVER_H
#define WEBSERVER_NICK_SERVER_H

#include <memory>
#include "Channel.h"
#include "EventLoop.h"
#include "EventLoopThreadPool.h"

class Server {
private:
    EventLoop *loop_;
    int threadNum_;
    std::unique_ptr<EventLoopThreadPool> eventLoopThreadPool_;
    bool started_;
    std::shared_ptr<Channel> acceptChannel_;
    int port_;
    int listenFd_;
    static const int MAX_FDS = 100000;
public:
    Server(EventLoop *loop, int threadNum, int port);

    ~Server() = default;

    EventLoop *getLoop() const {
        return loop_;
    }

    void start();

    void handNewConn();

    void handThisConn() {
        loop_->updatePoller(acceptChannel_);
    }
};


#endif //WEBSERVER_NICK_SERVER_H
