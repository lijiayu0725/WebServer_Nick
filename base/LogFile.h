//
// Created by Nick on 2020/7/16.
//

#ifndef WEBSERVER_NICK_LOGFILE_H
#define WEBSERVER_NICK_LOGFILE_H

#include <string>
#include <memory>
#include "MutexLock.h"
#include "noncopyable.h"
#include "FileUtil.h"

class LogFile : noncopyable {
private:
    const std::string baseName_;
    const int flushEveryN_; // 每写多少次往文件flush一次
    int count_; // 记录写次数
    std::unique_ptr<MutexLock> mutex_;
    std::unique_ptr<AppendFile> file_;

    void append_unlocked(const char *logLine, int len);

public:
    explicit LogFile(const std::string &baseName, int flushEveryN = 1024);

    ~LogFile() = default;

    void append(const char *logLine, int len);

    void flush();
};


#endif //WEBSERVER_NICK_LOGFILE_H
