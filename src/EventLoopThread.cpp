//
// Created by Nick on 2020/7/21.
//

#include "EventLoopThread.h"
#include <functional>
#include <cassert>


EventLoopThread::EventLoopThread() : loop_(nullptr), exiting_(false),
                                     thread_(std::bind(&EventLoopThread::threadFunc, this), "EventLoopThread"),
                                     mutex_(),
                                     cond_(mutex_) {
}

EventLoopThread::~EventLoopThread() {
    exiting_ = true;
    if (loop_ != nullptr) {
        loop_->quit();
        thread_.join();
    }
}

EventLoop *EventLoopThread::startLoop() {
    assert(!thread_.started());
    thread_.start();
    {
        MutexLockGuard lock(mutex_);
        while (loop_ == nullptr) {
            cond_.wait();
        }
    }
    return loop_;
}

void EventLoopThread::threadFunc() {
    EventLoop loop;
    {
        MutexLockGuard lock(mutex_);
        loop_ = &loop;
        cond_.notify();
    }
    loop.loop();
    loop_ = nullptr;
}




