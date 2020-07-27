//
// Created by Nick on 2020/7/21.
//

#ifndef WEBSERVER_NICK_EPOLL_H
#define WEBSERVER_NICK_EPOLL_H

#include <sys/epoll.h>
#include <memory>
#include <unordered_map>
#include <vector>
#include "Channel.h"
#include "HttpData.h"
#include "Timer.h"

class Epoll {
private:
    static const int MAX_FDS = 100000; // 最大fd数量，用于根据fd取得相应Channel和HttpData
    int epollFd_;
    std::vector<epoll_event> events_; // 用于epoll设置和返回事件
    std::shared_ptr<Channel> fd2channel_[MAX_FDS]; // fd -> Channel
    std::shared_ptr<HttpData> fd2httpData_[MAX_FDS]; // fd -> HttpData
    TimerManager timerManager_; // 定时器管理
public:
    Epoll();

    ~Epoll() = default;

    void epollAdd(SPChannel request, int timeout);

    void epollMod(SPChannel request, int timeout);

    void epollDel(SPChannel request);

    std::vector<std::shared_ptr<Channel>> poll(); // 返回poll返回的事件封装为Channel

    std::vector<std::shared_ptr<Channel>> getEventsRequest(int events_num); // 返回发生事件的Channel，设置其中的revents，并取消其中的events

    void addTimer(std::shared_ptr<Channel> requestData, int timeout); // 向定时器管理器中添加Channel的TimerNode

    int getEpollFd() {
        return epollFd_;
    }

    void handleExpired(); // 调用管理器的处理函数
};


#endif //WEBSERVER_NICK_EPOLL_H
