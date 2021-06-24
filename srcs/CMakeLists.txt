cmake_minimum_required(VERSION 3.19)
project(srcs)

set(CMAKE_CXX_STANDARD 14)

include_directories(.)

add_executable(srcs
        assign_server_to_socket.cpp
        callback.cpp
        callback.hpp
        callback_utils.cpp
        init_socket_list.cpp
        is_fd_ready.cpp
        launch_server.cpp
        main.cpp
        parse_buffer.cpp
        parse_header_functions.cpp
        parser.cpp
        read_headers.cpp
        remove_client.cpp
        reset_socket.cpp
        similar_get_req.cpp
        taskqueue.hpp
        webserv.hpp webserv.cpp)
