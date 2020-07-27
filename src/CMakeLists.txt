include_directories(${PROJECT_SOURCE_DIR}/src/base)

add_subdirectory(base)

aux_source_directory(./ DIR_SRCS)

add_executable(main ${DIR_SRCS} HttpData.cpp Util.cpp Timer.cpp ThreadPool.cpp EventLoopThread.cpp EventLoopThreadPool.cpp Channel.cpp Epoll.cpp EventLoop.cpp Server.cpp)

target_link_libraries(main base)

find_package(Threads)

target_link_libraries(main ${CMAKE_THREAD_LIBS_INIT})

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)