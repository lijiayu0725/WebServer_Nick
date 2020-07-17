cmake_minimum_required(VERSION 3.16)
project(WebServer_Nick)

set(CMAKE_CXX_STANDARD 11)

add_compile_options(-std=c++11 -Wall)

add_executable(WebServer_Nick base/Test.cpp base/LogFile.cpp base/FileUtil.cpp base/CountDownLatch.cpp base/Thread.cpp base/LogStream.cpp base/AsyncLogging.cpp base/Logging.cpp)

find_package(Threads)

target_link_libraries(WebServer_Nick ${CMAKE_THREAD_LIBS_INIT})