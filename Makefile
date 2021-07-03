# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: jheat <jheat@student.42.fr>                +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2021/07/02 14:21:02 by fflores           #+#    #+#              #
#    Updated: 2021/07/03 14:57:34 by jheat            ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME = ./srcs/build/webserv

all: $(NAME)
	
$(NAME):
	./setup.sh
	cmake -S srcs/ -B srcs/build
	cmake --build srcs/build/ --target all
	
clean:
	cmake --build srcs/build/ --target clean

fclean: clean
	rm -rf srcs/build

re: fclean all

.PHONY: all clean fclean re


		