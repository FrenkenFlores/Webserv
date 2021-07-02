# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: fflores <fflores@student.21-school.ru>     +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2021/07/02 14:21:02 by fflores           #+#    #+#              #
#    Updated: 2021/07/02 19:57:42 by fflores          ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

HEADER =	./srcs/webserv.hpp \
			./srcs/classes/Callback.hpp \
			./srcs/classes/Location.hpp \
			./srcs/classes/Server.hpp \
			./srcs/classes/TaskQueue.hpp \
			./srcs/classes/Tmpfile.hpp \
			./srcs/data_structures/Address.hpp \
			./srcs/data_structures/RequestHeader.hpp \
			./srcs/data_structures/Socket.hpp
			./srcs/read_headers/read_headers.hpp
NAME = webserv
FLAGS = -Wall -Werror -Wextra -std=c++98
CC = clang++
SRCS = 	./srcs/assign_server_to_clients.cpp \
		./srcs/ft_select.cpp ./srcs/init_clients.cpp \
		./srcs/main.cpp ./srcs/similar_get_req.cpp \
		./srcs/webserv.cpp \
		./srcs/classes/Callback.cpp \
		./srcs/classes/Callback_cgi.cpp \
		./srcs/classes/Callback_chunk.cpp \
		./srcs/classes/Callback_delete.cpp \
		./srcs/classes/Callback_error_request.cpp \
		./srcs/classes/Callback_get.cpp \
		./srcs/classes/Callback_head.cpp \
		./srcs/classes/Callback_options.cpp \
		./srcs/classes/Callback_post.cpp \
		./srcs/classes/Callback_put.cpp \
		./srcs/classes/Callback_trace.cpp \
		./srcs/classes/Callback_utils.cpp \
		./srcs/classes/Location.cpp \
		./srcs/classes/Server.cpp \
		./srcs/classes/TaskQueue.cpp \
		./srcs/classes/Tmpfile.cpp \
		./srcs/data_structures/Address.cpp \
		./srcs/data_structures/RequestHeader.cpp \
		./srcs/data_structures/Socket.cpp \



		