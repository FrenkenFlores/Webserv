# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: fflores <fflores@student.21-school.ru>     +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2021/07/02 14:21:02 by fflores           #+#    #+#              #
#    Updated: 2021/07/02 19:44:04 by fflores          ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

HEADER = ./srcs/webserv.hpp
NAME = webserv
FLAGS = -Wall -Werror -Wextra -std=c++98
CC = clang++
SRCS = 	assign_server_to_clients.cpp \
		ft_select.cpp \
		