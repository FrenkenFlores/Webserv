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

NAME = webserv

HEADER =	./srcs/webserv.hpp \
			./srcs/classes/Callback.hpp \
			./srcs/classes/Location.hpp \
			./srcs/classes/Server.hpp \
			./srcs/classes/TaskQueue.hpp \
			./srcs/classes/Tmpfile.hpp \
			./srcs/data_structures/Address.hpp \
			./srcs/data_structures/RequestHeader.hpp \
			./srcs/data_structures/Socket.hpp \
			./srcs/read_headers/read_headers.hpp

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
		./srcs/read_headers/cut_buffer.cpp \
		./srcs/read_headers/cut_buffer_ret.cpp \
		./srcs/read_headers/find_str_buffer.cpp \
		./srcs/read_headers/flush_crlf.cpp \
		./srcs/read_headers/get_header.cpp \
		./srcs/read_headers/init_maps.cpp \
		./srcs/read_headers/parse_header.cpp \
		./srcs/read_headers/parse_header_functions.cpp \
		./srcs/read_headers/parse_status_line.cpp \
		./srcs/read_headers/read_headers.cpp \
		./srcs/utils/cgitohttp.cpp \
		./srcs/utils/concate_list_str.cpp \
		./srcs/utils/ft_basename.cpp \
		./srcs/utils/ft_dirname.cpp \
		./srcs/utils/ft_error.cpp \
		./srcs/utils/ft_freestrs.cpp \
		./srcs/utils/gen_listening.cpp \
		./srcs/utils/get_err_page.cpp \
		./srcs/utils/get_status_line.cpp \
		./srcs/utils/get_status_msg.cpp \
		./srcs/utils/get_word.cpp \
		./srcs/utils/hextodec.cpp \
		./srcs/utils/is_buffer_crlf.cpp \
		./srcs/utils/is_fd_ready.cpp \
		./srcs/utils/lststr_len.cpp \
		./srcs/utils/lststr_to_str.cpp \
		./srcs/utils/lststr_to_strcont.cpp \
		./srcs/utils/lststr_to_strs.cpp \
		./srcs/utils/remove_client.cpp \
		./srcs/utils/strcont_to_str.cpp \
		./srcs/utils/substr.cpp




OBJ = $(patsubst %.cpp,%.o,$(SRCS))

all: $(NAME)
	@bash setup.sh
$(NAME): $(OBJ)
		$(CC) $(FLAGS) -o $(NAME) $(OBJ)

%.o: %.cpp $(HEADERS)
		$(CC) $(FLAGS) -c $< -o $@

clean:
	rm -f $(OBJ)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re


		
