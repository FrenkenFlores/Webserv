//
// Created by Jamis Heat on 6/19/21.
//

#include "Webserv.hpp"

bool g_run = 1;



void    webserv(std::list<Server> const &conf){
	bool						has_new_header_ready;
	s_similar_get_req           similar_req;
	t_socket_list				*clients;


while (g_run)
{
	has_new_header_ready = ft_select(clients, &similar_req);
}//while

} //webserv
