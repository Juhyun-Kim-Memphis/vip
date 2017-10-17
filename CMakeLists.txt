cmake_minimum_required(VERSION 2.8)
project(vip)

set(CMAKE_C_STANDARD 99)

set(SOURCE_FILES main.c util.h util.c tbmsg.c tbmsg.h libnet.h)
add_executable(vip ${SOURCE_FILES})
target_link_libraries(vip ${CMAKE_SOURCE_DIR}/libnet.a)