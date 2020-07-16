//
// Created by Nick on 2020/7/16.
//

#include "LogFile.h"

LogFile::LogFile(const std::string &baseName, int flushEveryN) : baseName_(baseName), flushEveryN_(flushEveryN),
                                                                 count_(0), mutex_(new MutexLock) {
    file_.reset(new AppendFile(baseName));
}

void LogFile::append_unlocked(const char *logLine, size_t len) {
    file_->append(logLine, len);
    ++count_;
    if (count_ >= flushEveryN_) {
        count_ = 0;
        file_->flush();
    }
}

void LogFile::append(const char *logLine, int len) {
    MutexLockGuard lock(*mutex_);
    append_unlocked(logLine, len);
}

void LogFile::flush() {
    MutexLockGuard lock(*mutex_);
    file_->flush();
}


