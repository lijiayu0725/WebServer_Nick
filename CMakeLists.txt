cmake_minimum_required(VERSION 3.16)
project(WebServer_Nick)

set(CMAKE_CXX_STANDARD 11)

add_compile_options(-std=c++11 -Wall -pthread)

add_executable(WebServer_Nick main.cpp base/LogFile.cpp base/LogFile.h base/noncopyable.h base/MutexLock.h base/FileUtil.cpp base/FileUtil.h base/CurrentThread.h base/CountDownLatch.cpp base/CountDownLatch.h base/Condition.h base/Thread.cpp base/Thread.h)