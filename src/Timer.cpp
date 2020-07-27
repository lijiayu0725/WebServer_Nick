//
// Created by Nick on 2020/7/20.
//

#include "Timer.h"
#include <sys/time.h>
#include <unistd.h>
#include <queue>

TimerNode::TimerNode(std::shared_ptr<HttpData> requestData, int timeout) : deleted_(false), SPHttpData(requestData) {
    update(timeout);
}

// TimerNode析构，处理自身的HttpData指针
TimerNode::~TimerNode() {
    if (SPHttpData) {
        SPHttpData->handleClose();
    }
}

// 默认构造不超时事件
TimerNode::TimerNode(TimerNode &tn) : SPHttpData(tn.SPHttpData), expiredTime_(0) {}

// 更新定时
void TimerNode::update(int timeout) {
    struct timeval now;
    gettimeofday(&now, nullptr);
    expiredTime_ = (now.tv_sec % 10000) * 1000 + now.tv_usec / 1000 + timeout; // 毫秒计算绝对时间
}

// 判断当前时间，是否Timer过期，没过期返回true，过期则设置删除，返回false
bool TimerNode::isValid() {
    struct timeval now;
    gettimeofday(&now, nullptr);
    size_t temp = (now.tv_sec % 10000) * 1000 + now.tv_usec / 1000;
    if (temp < expiredTime_) {
        return true;
    } else {
        this->setDeleted();
        return false;
    }
}

// 清除HttpData并删除节点
void TimerNode::clearReq() {
    SPHttpData.reset();
    this->setDeleted();
}

// 增加定时器节点，并将相应的HttpData关联此定时器
void TimerManager::addTimer(std::shared_ptr<HttpData> SPHttpData, int timeout) {
    SPTimerNode newNode(new TimerNode(SPHttpData, timeout));
    timerNodeQueue.push(newNode);
    SPHttpData->linkTimer(newNode);
}

// 处理超时事件，延迟删除在此实现
void TimerManager::handleExpireEvent() {
    while (!timerNodeQueue.empty()) {
        SPTimerNode pTimerNow = timerNodeQueue.top();
        if (pTimerNow->isDeleted())
            timerNodeQueue.pop();
        else if (!pTimerNow->isValid())
            timerNodeQueue.pop();
        else
            break;
    }
}
