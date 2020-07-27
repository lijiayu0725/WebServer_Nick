//
// Created by Nick on 2020/7/16.
//

#ifndef WEBSERVER_NICK_COUNTDOWNLATCH_H
#define WEBSERVER_NICK_COUNTDOWNLATCH_H

#include "Condition.h"
#include "MutexLock.h"
#include "noncopyable.h"

class CountDownLatch {
private:
    mutable MutexLock mutex_;
    Condition condition_;
    int count_;
public:
    explicit CountDownLatch(int count);

    void wait();

    void countDown();
};


#endif //WEBSERVER_NICK_COUNTDOWNLATCH_H
