cmake_minimum_required(VERSION 3.17)
project(Webserv)

set(CMAKE_CXX_STANDARD 98)
include_directories(srcs)
add_executable(${PROJECT_NAME}
        srcs/main.cpp
        srcs/parser.cpp
        srcs/init_socket_list.cpp
        srcs/launch_server.cpp
        srcs/parser.cpp
        srcs/parse_buffer.cpp
        srcs/Webserv.hpp
        )
