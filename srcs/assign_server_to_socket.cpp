#include "Webserv.hpp"

/*
 * match sockets with servers
*/

static bool is_right_conf(std::list<std::string> const &serv_name,
						  Header const &request) {
	std::list<std::string>::const_iterator s_it = serv_name.begin(), s_ite = serv_name.end();
	while (s_it != s_ite) {
		if (*s_it == request.host)
			return (true);
		++s_it;
	}
	return (false);
}

void assign_server_to_socket(std::list<Server> &server_list, std::list<Socket> &socket_list) {
	std::list<Server>::reverse_iterator right_server;
	char socket_addr[INET_ADDRSTRLEN];

	for (std::list<Socket>::iterator it_socket = socket_list.begin(); it_socket != socket_list.end(); it_socket++) {
		if (it_socket->is_header_read && !it_socket->is_callback_created) {
			inet_ntop(AF_INET, &it_socket->address.sin_addr, socket_addr, INET_ADDRSTRLEN)
			for (std::list<Server>::reverse_iterator it_server = server_list.rbegin(); it_server != server_list.rend(); it_server++) {
				if (!strcmp(socket_addr, &it_server->ip_port.ip[0]))
					continue ;
				right_server = it_server;
				if (is_right_conf(it_server->server_name, it_socket->headers))
					break ;
			}
			it_socket->server = &(*right_server);
		}
	}
}