//
// Created by Nick on 2020/7/21.
//

#include "Epoll.h"
#include <cassert>
#include <sys/epoll.h>
#include <queue>
#include "Util.h"
#include "base/Logging.h"

using namespace std;

const int EVENTS_NUM = 4096;
const int EPOLLWAIT_TIME = 10000;

typedef shared_ptr<Channel> SPChannel;


Epoll::Epoll() : epollFd_(epoll_create1(EPOLL_CLOEXEC)), events_(EVENTS_NUM) {
    assert(epollFd_ > 0);
}

void Epoll::epollAdd(SPChannel request, int timeout) {
    int fd = request->getFd();
    if (timeout > 0) {
        addTimer(request, timeout);
        fd2httpData_[fd] = request->getHolder();
    }
    struct epoll_event event;
    event.data.fd = fd;
    event.events = request->getEvents();
    request->EqualAndUpdateLastEvents();
    fd2channel_[fd] = request;
    if (epoll_ctl(epollFd_, EPOLL_CTL_ADD, fd, &event) < 0) {
        perror("epoll_add Error");
        fd2channel_[fd].reset();
    }
}

void Epoll::epollMod(SPChannel request, int timeout) {
    if (timeout > 0)
        addTimer(request, timeout);
    int fd = request->getFd();
    if (!request->EqualAndUpdateLastEvents()) {
        struct epoll_event event;
        event.data.fd = fd;
        event.events = request->getEvents();
        if (epoll_ctl(epollFd_, EPOLL_CTL_MOD, fd, &event) < 0) {
            perror("epoll_mod Error");
            fd2channel_[fd].reset();
        }
    }
}

void Epoll::epollDel(SPChannel request) {
    int fd = request->getFd();
    struct epoll_event event;
    event.data.fd = fd;
    event.events = request->getLastEvents();
    if (epoll_ctl(epollFd_, EPOLL_CTL_DEL, fd, &event) < 0) {
        perror("epoll_del Error");
    }
    fd2channel_[fd].reset();
    fd2httpData_[fd].reset();
}

void Epoll::addTimer(std::shared_ptr<Channel> requestData, int timeout) {
    shared_ptr<HttpData> t = requestData->getHolder();
    if (t)
        timerManager_.addTimer(t, timeout);
    else
        LOG << "Timer Add Failed\n";
}

std::vector<std::shared_ptr<Channel>> Epoll::poll() {
    while (true) {
        int event_count = epoll_wait(epollFd_, events_.data(), events_.size(), EPOLLWAIT_TIME);
        if (event_count < 0)
            perror("epoll_wait Error");
        std::vector<SPChannel> reqData = getEventsRequest(event_count);
        if (!reqData.empty()) {
            return reqData;
        }
    }
}

std::vector<std::shared_ptr<Channel>> Epoll::getEventsRequest(int events_num) {
    std::vector<SPChannel> req_data;
    for (int i = 0; i < events_num; ++i) {
        int fd = events_[i].data.fd;
        SPChannel cur_req = fd2channel_[fd];
        if (cur_req) {
            cur_req->setRevents(events_[i].events);
            cur_req->setEvents(0);
            req_data.push_back(cur_req);
        } else {
            LOG << "SP cur_req is invalid";
        }
    }
    return req_data;
}

void Epoll::handleExpired() {
    timerManager_.handleExpireEvent();
}


