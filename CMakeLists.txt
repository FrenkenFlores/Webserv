cmake_minimum_required(VERSION 3.17)
project(Webserv)

set(CMAKE_CXX_STANDARD 98)

include_directories(srcs)

add_executable(${PROJECT_NAME}
        srcs/read_headers.cpp
        srcs/remove_client.cpp
        srcs/similar_get_req.cpp
        srcs/parse_header_functions.cpp
        srcs/reset_socket.cpp
        srcs/is_fd_ready.cpp

        srcs/main.cpp
        srcs/init_socket_list.cpp
        srcs/launch_server.cpp
        srcs/parser.cpp
        srcs/parse_buffer.cpp
        srcs/Webserv.hpp
        srcs/assign_server_to_socket.cpp
        srcs/callback.cpp
        srcs/callback_utils.cpp
        )
