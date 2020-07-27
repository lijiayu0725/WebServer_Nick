//
// Created by Nick on 2020/7/21.
//

#ifndef WEBSERVER_NICK_EVENTLOOPTHREAD_H
#define WEBSERVER_NICK_EVENTLOOPTHREAD_H

#include "EventLoop.h"
#include "base/Condition.h"
#include "base/MutexLock.h"
#include "base/Thread.h"
#include "base/noncopyable.h"

class EventLoopThread : noncopyable {
private:
    void threadFunc();

    EventLoop *loop_;
    bool exiting_;
    Thread thread_;
    MutexLock mutex_;
    Condition cond_;
public:
    EventLoopThread();

    ~EventLoopThread();

    EventLoop *startLoop();
};


#endif //WEBSERVER_NICK_EVENTLOOPTHREAD_H
