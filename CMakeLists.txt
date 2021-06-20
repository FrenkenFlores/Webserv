cmake_minimum_required(VERSION 3.17)
project(Webserv)

set(CMAKE_CXX_STANDARD 98)

include_directories(sockets_structures)
include_directories(srcs)
include_directories(utils)

add_executable(${PROJECT_NAME}
        sockets_structures/s_socket.cpp
        sockets_structures/s_socket.hpp

        utils/colors.hpp
        utils/utils.hpp
        utils/ft_error.cpp

        srcs/ft_select.cpp
        srcs/web.cpp
        srcs/main.cpp
        srcs/parser.cpp
        srcs/Webserv.hpp
        )
