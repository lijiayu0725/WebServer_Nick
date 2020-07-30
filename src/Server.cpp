//
// Created by Nick on 2020/7/23.
//

#include "Server.h"
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <functional>
#include "Util.h"
#include "base/Logging.h"

using namespace std;

Server::Server(EventLoop *loop, int threadNum, int port) : loop_(loop), threadNum_(threadNum), eventLoopThreadPool_(
        new EventLoopThreadPool(loop_, threadNum)), started_(false), acceptChannel_(new Channel(loop_)), port_(port),
                                                           listenFd_(socketBindListen(port_)) {
    acceptChannel_->setFd(listenFd_);
    handleForSigpipe();
    if (setSocketNonBlocking(listenFd_) < 0) {
        perror("set socket nonblocking failed");
        abort();
    }
}

void Server::start() {
    eventLoopThreadPool_->start();
    acceptChannel_->setEvents(EPOLLIN | EPOLLET);
    acceptChannel_->setReadHandler(std::bind(&Server::handNewConn, this));
    acceptChannel_->setConnHandler(std::bind(&Server::handThisConn, this));
    loop_->addToPoller(acceptChannel_, 0);
    started_ = true;
    loop_->loop();
}

void Server::handNewConn() {
    struct sockaddr_in client_addr;
    bzero(&client_addr, sizeof(client_addr));
    socklen_t client_addr_len = sizeof(client_addr);
    int accept_fd;
    while ((accept_fd = accept(listenFd_, (struct sockaddr *) &client_addr, &client_addr_len)) > 0) {
        EventLoop *loop = eventLoopThreadPool_->getNextLoop();
        LOG << "New connection from " << inet_ntoa(client_addr.sin_addr) << ":" << ntohs(client_addr.sin_port);
        if (accept_fd >= MAX_FDS) {
            close(accept_fd);
            continue;
        }
        if (setSocketNonBlocking(accept_fd) < 0) {
            LOG << "Set nonblocking failed";
            return;
        }
        setSocketNoDelay(accept_fd);
        shared_ptr<HttpData> req_info(new HttpData(loop, accept_fd));
        req_info->getChannel()->setHolder(req_info);
        loop->runInLoop(std::bind(&HttpData::newEvent, req_info));
    }
    acceptChannel_->setEvents(EPOLLIN | EPOLLET);
}


