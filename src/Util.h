//
// Created by Nick on 2020/7/20.
//

#ifndef WEBSERVER_NICK_UTIL_H
#define WEBSERVER_NICK_UTIL_H

#include <cstdlib>
#include <string>

// 各种读取接口
ssize_t readn(int fd, void *buf, size_t n);

ssize_t readn(int fd, std::string &inBuffer, bool &zero);

ssize_t readn(int fd, std::string &inBuffer);

// 各种写入接口
ssize_t writen(int fd, void *buff, size_t n);

ssize_t writen(int fd, std::string &sbuff);

// 设置收到SIGPIPE信号默认忽略，而非终止。
void handleForSigpipe();

// 设置文件描述符非阻塞
int setSocketNonBlocking(int fd);

// 关闭Nagle算法
void setSocketNoDelay(int fd);

// 设置socket关闭行为，收到FIN后30秒内可接收数据
void setSocketNoLinger(int fd);

// 优雅关闭不丢弃接收缓存的数据
void shutDownWR(int fd);

// socket服务器端口绑定并监听
int socketBindListen(int port);

#endif //WEBSERVER_NICK_UTIL_H
