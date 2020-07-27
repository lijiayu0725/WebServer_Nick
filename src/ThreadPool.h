//
// Created by Nick on 2020/7/21.
//

#ifndef WEBSERVER_NICK_THREADPOOL_H
#define WEBSERVER_NICK_THREADPOOL_H

#include <pthread.h>
#include <functional>
#include <memory>
#include <vector>

const int THREADPOOL_INVALID = -1;
const int THREADPOOL_LOCK_FAILURE = -2;
const int THREADPOOL_QUEUE_FULL = -3;
const int THREADPOOL_SHUTDOWN = -4;
const int THREADPOOL_THREAD_FAILURE = -5;
const int THREADPOOL_GRACEFUL = 1;

const int MAX_THREADS = 1024;
const int MAX_QUEUE = 65535;

typedef enum {
    immediate_shutdown = 1,
    graceful_shutdown = 2
} ShutDownOption;

struct ThreadPoolTask {
    std::function<void(std::shared_ptr<void>)> func;
    std::shared_ptr<void> args;
};

class ThreadPool {
private:
    static pthread_mutex_t lock;
    static pthread_cond_t notify;

    static std::vector<pthread_t> threads;
    static std::vector<ThreadPoolTask> queue;
    static int threadCount;
    static int queueSize;
    static int head;
    static int tail;
    static int count;
    static int shutdown;
    static int started;
public:
    static int threadPoolCreate(int _threadCount, int _queueSize);

    static int threadPoolAdd(std::shared_ptr<void> args, std::function<void(std::shared_ptr<void>)> func);

    static int threadPoolDestroy(ShutDownOption shutdownOption = graceful_shutdown);

    static int threadPoolFree();

    static void *threadPoolThread(void *args);
};


#endif //WEBSERVER_NICK_THREADPOOL_H
