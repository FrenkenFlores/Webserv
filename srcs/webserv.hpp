#ifndef WEBSERV_HPP
# define WEBSERV_HPP

//#define DEFAULT_CONFIG_PATH "./configs/default.conf"
#define DEFAULT_CONFIG_PATH "./../../configs/clion_default.conf"

# include <list>
# include <iostream>
# include <fstream>
# include <signal.h>
# include <sys/socket.h>
# include <fstream>
# include "data_structures/Socket.hpp"
# include "classes/Server.hpp"
# include "classes/TaskQueue.hpp"
# include "data_structures/RequestHeader.hpp"

typedef void (*parser_function)(std::string::const_iterator cit, void*);

extern int g_worker_connections;

bool    read_headers(std::list<Socket> *clients);
void	init_socket_list(std::list<Server> &server_list, std::list<Socket>	&socket_list);
void    launch_server(std::list<Server> const &conf, std::list<Socket> &clients);
bool    ft_select(std::list<Socket> *const clients,
                  s_similar_get_req *similar_req);

void    similar_get_req_sender(std::list<Socket> *clients,
                                s_similar_get_req *similar_req);
void    similar_get_req_checker(std::list<Socket> *clients,
                                s_similar_get_req *similar_req);

void    assign_server_to_clients(std::list<Server> const &conf,
                                 std::list<Socket> *const clients);

#endif
