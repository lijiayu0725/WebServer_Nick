//
// Created by Nick on 2020/7/16.
//

#ifndef WEBSERVER_NICK_MUTEXLOCK_H
#define WEBSERVER_NICK_MUTEXLOCK_H

#include <pthread.h>
#include <cstdio>
#include "noncopyable.h"

class MutexLock : noncopyable {
private:
    pthread_mutex_t mutex;

    friend class Condition;

public:
    MutexLock() {
        pthread_mutex_init(&mutex, nullptr);
    }

    ~MutexLock() {
        pthread_mutex_lock(&mutex); // 防止有线程还在持有锁
        pthread_mutex_destroy(&mutex);
    }

    void lock() {
        pthread_mutex_lock(&mutex);
    }

    void unlock() {
        pthread_mutex_unlock(&mutex);
    }

    pthread_mutex_t *get() {
        return &mutex;
    }
};

class MutexLockGuard : noncopyable {
private:
    MutexLock &mutex;
public:
    explicit MutexLockGuard(MutexLock &_mutex) : mutex(_mutex) {
        mutex.lock();
    }

    ~MutexLockGuard() {
        mutex.unlock();
    }
};

#endif //WEBSERVER_NICK_MUTEXLOCK_H
