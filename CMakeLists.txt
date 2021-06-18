cmake_minimum_required(VERSION 3.17)
project(Webserv)

set(CMAKE_CXX_STANDARD 98)

add_executable(${PROJECT_NAME}
        srcs/main.cpp
        srcs/Webserv.hpp
        )
