//
// Created by Nick on 2020/7/16.
//

#ifndef WEBSERVER_NICK_THREAD_H
#define WEBSERVER_NICK_THREAD_H

#include <pthread.h>
#include <sys/syscall.h>
#include <unistd.h>
#include <functional>
#include <memory>
#include <string>
#include "CountDownLatch.h"
#include "noncopyable.h"

class Thread {
public:
    typedef std::function<void()> ThreadFunc;
private:
    pthread_t pthreadId_;
    pid_t tid_;
    ThreadFunc func_;
    std::string name_;
    CountDownLatch latch_;
    bool started_;
    bool joined_;

    void setDefaultName();

public:
    explicit Thread(const ThreadFunc &, std::string name = "");

    ~Thread();

    void start();

    int join();

    bool started() const {
        return started_;
    }

    pid_t tid() const {
        return tid_;
    }

    const std::string &name() const {
        return name_;
    }
};

#endif //WEBSERVER_NICK_THREAD_H
