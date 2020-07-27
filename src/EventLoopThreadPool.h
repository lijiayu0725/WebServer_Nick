//
// Created by Nick on 2020/7/21.
//

#ifndef WEBSERVER_NICK_EVENTLOOPTHREADPOOL_H
#define WEBSERVER_NICK_EVENTLOOPTHREADPOOL_H

#include <memory>
#include <vector>
#include "EventLoopThread.h"
#include "base/Logging.h"
#include "base/noncopyable.h"


class EventLoopThreadPool : noncopyable {
private:
    EventLoop *baseLoop_;
    bool started_;
    int numThreads_;
    int next_;
    std::vector<std::shared_ptr<EventLoopThread>> threads_;
    std::vector<EventLoop *> loops_;
public:
    EventLoopThreadPool(EventLoop *baseLoop, int numThreads);

    ~EventLoopThreadPool() {
        LOG << "~EventLoopThreadPool()";
    }

    void start();

    EventLoop *getNextLoop();
};


#endif //WEBSERVER_NICK_EVENTLOOPTHREADPOOL_H
