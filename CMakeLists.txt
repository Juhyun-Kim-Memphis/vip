<<<<<<< HEAD
cmake_minimum_required(VERSION 2.2)
=======
cmake_minimum_required(VERSION 2.8)
>>>>>>> 36cbc22820b101b6f0055af6b8e21f2162256c1d
project(vip)

set(CMAKE_C_STANDARD 99)

set(SOURCE_FILES main.c util.h util.c tbmsg.c tbmsg.h libnet.h aws_vip.c aws_vip.h)
add_executable(vip ${SOURCE_FILES})
target_link_libraries(vip ${CMAKE_SOURCE_DIR}/libnet.a)
