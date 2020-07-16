//
// Created by Nick on 2020/7/16.
//

#ifndef WEBSERVER_NICK_FILEUTIL_H
#define WEBSERVER_NICK_FILEUTIL_H

#include <string>
#include "noncopyable.h"

class AppendFile : noncopyable {
private:
    FILE *fp_; // 文件描述符
    char buffer_[64 * 1024]{}; // 自定义缓冲区
    size_t write(const char *logLine, size_t len); // 单次写文件
public:
    explicit AppendFile(const std::string &fileName);

    ~AppendFile();

    void append(const char *logLine, size_t len); // 写文件接口
    void flush();
};


#endif //WEBSERVER_NICK_FILEUTIL_H
