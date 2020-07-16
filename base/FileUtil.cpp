//
// Created by Nick on 2020/7/16.
//

#include <fcntl.h>
#include <cstdio>
#include "FileUtil.h"

AppendFile::AppendFile(const std::string &fileName) : fp_(fopen(fileName.c_str(), "ae")) {
    setbuffer(fp_, buffer_, sizeof(buffer_));
}

AppendFile::~AppendFile() {
    fclose(fp_);
}

void AppendFile::append(const char *logLine, size_t len) {
    size_t n = this->write(logLine, len);
    size_t remain = len - n;
    while (remain > 0) {
        size_t x = this->write(logLine + n, remain);
        if (x == 0) {
            int err = ferror(fp_);
            if (err) {
                fprintf(fp_, "AppendFile::append() failed !\n");
            }
            break;
        }
        n += x;
        remain = len - n;
    }
}

size_t AppendFile::write(const char *logLine, size_t len) {
    return fwrite_unlocked(logLine, 1, len, fp_);
}

void AppendFile::flush() {
    fflush(fp_);
}


