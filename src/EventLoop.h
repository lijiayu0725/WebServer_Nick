//
// Created by Nick on 2020/7/23.
//

#ifndef WEBSERVER_NICK_EVENTLOOP_H
#define WEBSERVER_NICK_EVENTLOOP_H

#include <functional>
#include <memory>
#include <vector>
#include "Channel.h"
#include "Epoll.h"
#include "Util.h"
#include "base/CurrentThread.h"
#include "base/Logging.h"
#include "base/Thread.h"
#include <iostream>

class EventLoop {
public:
    typedef std::function<void()> Functor;
private:
    bool looping_; // 是否开始循环
    std::shared_ptr<Epoll> poller_; // epoll对象
    int wakeupFd_; // 唤醒用的fd
    bool quit_; // 是否退出
    bool eventHandling_; // 是否正在处理事件
    mutable MutexLock mutex_;
    std::vector<Functor> pendingFunctors_; // 临时插入的方法
    bool callingPendingFunctors_; // 是否正在处理临时插入方法
    const pid_t threadId_; // EventLoop所属ThreadId
    std::shared_ptr<Channel> pWakeUpChannel_; // 唤醒用的Channel

    void wakeup(); // 发送唤醒描述符事件，优雅传递唤醒事件

    void handleRead();

    void doPendingFunctors();

    void handleConn();

public:
    EventLoop();

    ~EventLoop();

    void loop();

    void quit();

    void runInLoop(Functor &&cb);

    void queueInLoop(Functor &&cb);

    bool isInLoopThread() const {
        return threadId_ == CurrentThread::tid();
    }

    void assertInLoopThread() const {
        assert(isInLoopThread());
    }

    void shutdown(std::shared_ptr<Channel> channel) {
        shutDownWR(channel->getFd());
    }

    void removeFromPoller(std::shared_ptr<Channel> channel) {
        poller_->epollDel(channel);
    }

    void updatePoller(std::shared_ptr<Channel> channel, int timeout = 0) {
        poller_->epollMod(channel, timeout);
    }

    void addToPoller(std::shared_ptr<Channel> channel, int timeout = 0) {
        poller_->epollAdd(channel, timeout);
    }
};


#endif //WEBSERVER_NICK_EVENTLOOP_H
