//
// Created by Nick on 2020/7/23.
//

#include "EventLoop.h"
#include <sys/epoll.h>
#include <sys/eventfd.h>
#include "Util.h"
#include "base/Logging.h"

using namespace std;

__thread EventLoop *t_loopInThisThread = nullptr;

int createEventFd() {
    int evtfd = eventfd(0, EFD_NONBLOCK | EFD_CLOEXEC);
    if (evtfd < 0) {
        LOG << "Failed in eventfd";
        abort();
    }
    return evtfd;
}

EventLoop::EventLoop() : looping_(false), poller_(new Epoll()), wakeupFd_(createEventFd()), quit_(false),
                         eventHandling_(false), callingPendingFunctors_(false), threadId_(CurrentThread::tid()),
                         pWakeUpChannel_(new Channel(this, wakeupFd_)) {
    if (t_loopInThisThread) {
        LOG << "Another EventLoop " << t_loopInThisThread << " exists in this thread " << threadId_;
    } else {
        t_loopInThisThread = this;
    }
    pWakeUpChannel_->setEvents(EPOLLIN | EPOLLET);
    pWakeUpChannel_->setReadHandler(bind(&EventLoop::handleRead, this));
    pWakeUpChannel_->setConnHandler(bind(&EventLoop::handleConn, this));
    poller_->epollAdd(pWakeUpChannel_, 0);
}

void EventLoop::handleConn() {
    updatePoller(pWakeUpChannel_, 0);
}

EventLoop::~EventLoop() {
    close(wakeupFd_);
    t_loopInThisThread = nullptr;
}

void EventLoop::wakeup() {
    uint64_t one = 1;
    ssize_t n = writen(wakeupFd_, (void *) &one, sizeof(one));
    if (n != sizeof(one)) {
        LOG << "EventLoop::wakeup() writes " << n << " bytes instead of 8";
    }
}

void EventLoop::handleRead() {
    uint64_t one = 1;
    ssize_t n = readn(wakeupFd_, &one, sizeof(one));
    if (n != sizeof(one)) {
        LOG << "EventLoop::handleRead() reads " << n << " bytes instead of 8";
    }
    pWakeUpChannel_->setEvents(EPOLLIN | EPOLLET);
}

void EventLoop::runInLoop(EventLoop::Functor &&cb) {
    if (isInLoopThread()) {
        cb();
    } else {
        queueInLoop(std::move(cb));
    }
}

void EventLoop::queueInLoop(EventLoop::Functor &&cb) {
    {
        MutexLockGuard lock(mutex_);
        pendingFunctors_.emplace_back(std::move(cb));
    }
    if (!isInLoopThread() || callingPendingFunctors_) {
        wakeup();
    }
}

void EventLoop::loop() {
    assert(!looping_);
    assert(isInLoopThread());
    looping_ = true;
    quit_ = false;
    std::vector<SPChannel> ret;
    while (!quit_) {
        ret.clear();
        ret = poller_->poll();
        eventHandling_ = true;
        for (auto &it : ret) {
            it->handleEvents();
        }
        eventHandling_ = false;
        doPendingFunctors();
        poller_->handleExpired();
    }
    looping_ = false;
}

void EventLoop::doPendingFunctors() {
    std::vector<Functor> functors;
    callingPendingFunctors_ = true;
    {
        MutexLockGuard lock(mutex_);
        functors.swap(pendingFunctors_);
    }
    for (size_t i = 0; i < functors.size(); ++i) {
        functors[i]();
    }
    callingPendingFunctors_ = false;
}

void EventLoop::quit() {
    quit_ = true;
    if (!isInLoopThread())
        wakeup();
}










