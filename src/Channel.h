//
// Created by Nick on 2020/7/21.
//

#ifndef WEBSERVER_NICK_CHANNEL_H
#define WEBSERVER_NICK_CHANNEL_H

#include <sys/epoll.h>
#include <functional>
#include <memory>
#include <string>
#include <unordered_map>
#include "Timer.h"
#include "HttpData.h"

class EventLoop;

// Channel 从属于一个Eventloop和fd，做事件分发
class Channel {
private:
    typedef std::function<void()> CallBack;
    EventLoop *loop_; // Channel所属的EventLoop
    int fd_; // Channel管理的fd
    uint32_t events_; // 关心的事件
    uint32_t revents_; // 返回的事件
    uint32_t lastEvents_; // 用来判断上一次事件是否与此次事件相同
    std::weak_ptr<HttpData> holder_; // Channel持有的HttpData对象
    CallBack readHandler_; // 读事件处理函数
    CallBack writeHandler_; // 写事件处理函数
    CallBack errorHandler_; // 错误回调函数
    CallBack connHandler_; // 回调函数
public:
    explicit Channel(EventLoop *loop);

    Channel(EventLoop *loop, int fd);

    ~Channel() = default;

    int getFd();

    void setFd(int fd);

    void setHolder(std::shared_ptr<HttpData> holder) {
        holder_ = holder;
    }

    std::shared_ptr<HttpData> getHolder() { // 防止weak_ptr对象被析构，暂时提升为shared_ptr
        std::shared_ptr<HttpData> ret(holder_.lock());
        return ret;
    }

    void setReadHandler(CallBack &&readHandler) {
        readHandler_ = readHandler;
    }

    void setWriteHandler(CallBack &&writeHandler) {
        writeHandler_ = writeHandler;
    }

    void setErrorHandler(CallBack &&errorHandler) {
        errorHandler_ = errorHandler;
    }

    void setConnHandler(CallBack &&connHandler) {
        connHandler_ = connHandler;
    }

    // EPOLLHUP: RST事件
    void handleEvents() {
        events_ = 0;
        if ((revents_ & EPOLLHUP) && !(revents_ & EPOLLIN)) {
            events_ = 0;
            return;
        }
        if (revents_ & EPOLLERR) {
            if (errorHandler_) errorHandler_();
            events_ = 0;
            return;
        }
        if (revents_ & (EPOLLIN | EPOLLPRI | EPOLLRDHUP)) {
            handleRead();
        }
        if (revents_ & EPOLLOUT) {
            handleWrite();
        }
        handleConn();
    }

    void handleRead();

    void handleWrite();

    void handleError(int fd, int err_num, std::string short_msg);

    void handleConn();

    void setRevents(__uint32_t ev) {
        revents_ = ev;
    }

    void setEvents(__uint32_t ev) {
        events_ = ev;
    }

    __uint32_t &getEvents() {
        return events_;
    }

    // 上一次事件是否与此次事件相同，并更新
    bool EqualAndUpdateLastEvents() {
        bool ret = (lastEvents_ == events_);
        lastEvents_ = events_;
        return ret;
    }

    __uint32_t getLastEvents() {
        return lastEvents_;
    }
};

typedef std::shared_ptr<Channel> SPChannel;

#endif //WEBSERVER_NICK_CHANNEL_H
