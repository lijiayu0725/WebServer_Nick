//
// Created by Nick on 2020/7/20.
//

#ifndef WEBSERVER_NICK_TIMER_H
#define WEBSERVER_NICK_TIMER_H

#include <unistd.h>
#include <deque>
#include <memory>
#include <queue>
#include "MutexLock.h"
#include "noncopyable.h"
#include "HttpData.h"

class HttpData;

class TimerNode {
private:
    bool deleted_; // 是否删除，支持延迟删除节点
    ssize_t expiredTime_; // 超时时间，绝对时间 ms
    std::shared_ptr<HttpData> SPHttpData; // 对应的HttpData指针
public:
    TimerNode(std::shared_ptr<HttpData> requestData, int timeout);

    ~TimerNode();

    TimerNode(TimerNode &tn); // 拷贝构造

    void update(int timeout); // 重新设置时间

    bool isValid(); // 是否有效，没被删除

    void clearReq(); // 清除HttpData并删除Timer节点

    void setDeleted() {
        deleted_ = true;
    }

    bool isDeleted() const {
        return deleted_;
    }

    ssize_t getExpTime() const {
        return expiredTime_;
    }
};

// 比较器，用于小根堆
struct TimerCmp {
    bool operator()(std::shared_ptr<TimerNode> &a, std::shared_ptr<TimerNode> &b) const {
        return a->getExpTime() > b->getExpTime();
    }
};

// 定时器管理类
class TimerManager {
private:
    typedef std::shared_ptr<TimerNode> SPTimerNode;
    std::priority_queue<SPTimerNode, std::deque<SPTimerNode>, TimerCmp> timerNodeQueue; // 定时器用小根堆
public:
    TimerManager() = default;

    ~TimerManager() = default;

    void addTimer(std::shared_ptr<HttpData> SPHttpdata, int timeout); // 增加定时器

    void handleExpireEvent(); // 处理超时事件，一次处理多个
};


#endif //WEBSERVER_NICK_TIMER_H
