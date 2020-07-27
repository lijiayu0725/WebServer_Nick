//
// Created by Nick on 2020/7/21.
//

#include "ThreadPool.h"

pthread_mutex_t ThreadPool::lock = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t ThreadPool::notify = PTHREAD_COND_INITIALIZER;
std::vector<pthread_t> ThreadPool::threads;
std::vector<ThreadPoolTask> ThreadPool::queue;
int ThreadPool::threadCount = 0;
int ThreadPool::queueSize = 0;
int ThreadPool::head = 0;
int ThreadPool::tail = 0;
int ThreadPool::count = 0;
int ThreadPool::shutdown = 0;
int ThreadPool::started = 0;

int ThreadPool::threadPoolCreate(int _threadCount, int _queueSize) {

    if (_threadCount <= 0 || _threadCount > MAX_THREADS || _queueSize <= 0 || _queueSize > MAX_QUEUE) {
        _threadCount = 8;
        _queueSize = 1024;
    }

    threadCount = 0;
    queueSize = _queueSize;
    head = tail = count = 0;
    shutdown = started = 0;

    threads.resize(_threadCount);
    queue.resize(_queueSize);

    for (int i = 0; i < _threadCount; ++i) {
        if (pthread_create(&threads[i], nullptr, threadPoolThread, (void *) (0)) != 0) {
            return -1;
        }
        ++threadCount;
        ++started;
    }

    return 0;
}

int ThreadPool::threadPoolAdd(std::shared_ptr<void> args, std::function<void(std::shared_ptr<void>)> func) {
    int next, err = 0;
    if (pthread_mutex_lock(&lock)) {
        return THREADPOOL_LOCK_FAILURE;
    }

    do {
        next = (tail + 1) % queueSize;
        if (count == queueSize) {
            err = THREADPOOL_QUEUE_FULL;
            break;
        }
        if (shutdown) {
            err = THREADPOOL_SHUTDOWN;
            break;
        }
        queue[tail].func = func;
        queue[tail].args = args;
        tail = next;
        ++count;
        if (pthread_cond_signal(&notify) != 0) {
            err = THREADPOOL_LOCK_FAILURE;
            break;
        }
    } while (false);

    if (pthread_mutex_unlock(&lock) != 0)
        err = THREADPOOL_LOCK_FAILURE;
    return err;
}

int ThreadPool::threadPoolFree() {
    if (started) {
        return -1;
    }
    pthread_mutex_lock(&lock);
    pthread_mutex_destroy(&lock);
    pthread_cond_destroy(&notify);
    return 0;
}

int ThreadPool::threadPoolDestroy(ShutDownOption shutdownOption) {
    printf("ThreadPool Destroy !\n");
    int err = 0;

    if (pthread_mutex_lock(&lock)) {
        return THREADPOOL_LOCK_FAILURE;
    }
    do {
        if (shutdown) {
            err = THREADPOOL_SHUTDOWN;
            break;
        }
        shutdown = shutdownOption;
        if (pthread_cond_broadcast(&notify) != 0 || pthread_mutex_unlock(&lock) != 0) {
            err = THREADPOOL_LOCK_FAILURE;
            break;
        }
        for (int i = 0; i < threadCount; ++i) {
            if (pthread_join(threads[i], nullptr) != 0) {
                err = THREADPOOL_THREAD_FAILURE;
            }
        }
    } while (false);
    if (!err) {
        threadPoolFree();
    }
    return err;
}

void *ThreadPool::threadPoolThread(void *args) {
    while (true) {
        ThreadPoolTask task;
        pthread_mutex_lock(&lock);
        while (count == 0 && !shutdown) {
            pthread_cond_wait(&notify, &lock);
        }
        if (shutdown == immediate_shutdown || (shutdown == graceful_shutdown && count == 0))
            break;
        task.func = queue[head].func;
        task.args = queue[head].args;
        queue[head].func = nullptr;
        queue[head].args.reset();
        head = (head + 1) % queueSize;
        --count;
        pthread_mutex_unlock(&lock);
        task.func(task.args);
    }
    --started;
    pthread_mutex_unlock(&lock);
    printf("Thread finished !\n");
    pthread_exit(nullptr);
}

