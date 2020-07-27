//
// Created by Nick on 2020/7/17.
//

#ifndef WEBSERVER_NICK_ASYNCLOGGING_H
#define WEBSERVER_NICK_ASYNCLOGGING_H

#include <functional>
#include <string>
#include <vector>
#include "CountDownLatch.h"
#include "LogStream.h"
#include "MutexLock.h"
#include "Thread.h"
#include "noncopyable.h"

class AsyncLogging : noncopyable {
private:
    void threadFunc();

    typedef FixedBuffer<kLargeBuffer> Buffer;
    typedef std::vector<std::shared_ptr<Buffer>> BufferVector;
    typedef std::shared_ptr<Buffer> BufferPtr;
    const int flushInterval_;
    bool running_;
    std::string baseName_;
    Thread thread_;
    MutexLock mutex_;
    Condition cond_;
    BufferPtr currentBuffer_;
    BufferPtr nextBuffer_;
    BufferVector buffers_;
    CountDownLatch latch_;
public:
    explicit AsyncLogging(const std::string baseName, int flushInterval = 2);

    ~AsyncLogging() {
        if (running_) {
            stop();
        }
    }

    void append(const char *logLine, int len);

    void start() {
        running_ = true;
        thread_.start();
        latch_.wait();
    }

    void stop() {
        running_ = false;
        cond_.notify();
        thread_.join();
    }
};


#endif //WEBSERVER_NICK_ASYNCLOGGING_H
