//
// Created by Nick on 2020/7/16.
//

#ifndef WEBSERVER_NICK_NONCOPYABLE_H
#define WEBSERVER_NICK_NONCOPYABLE_H

class noncopyable {
protected:
    noncopyable() = default;

    ~noncopyable() = default;

public:
    noncopyable(const noncopyable &) = delete;

    const noncopyable &operator=(const noncopyable &) = delete;
};

#endif //WEBSERVER_NICK_NONCOPYABLE_H
