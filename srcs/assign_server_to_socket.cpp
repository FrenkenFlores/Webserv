#include "webserv.hpp"
//
///*
// * match sockets with servers
//*/
//
//static bool is_right_conf(std::list<std::string> const &serv_name,
//						  Header const &request) {
//	std::list<std::string>::const_iterator s_it = serv_name.begin(), s_ite = serv_name.end();
//	while (s_it != s_ite) {
//		if (*s_it == request.host)
//			return (true);
//		++s_it;
//	}
//	return (false);
//}
//
//void assign_server_to_socket(std::list<Server> &server_list, std::list<Socket> &socket_list) {
//	std::list<Server>::reverse_iterator right_server;
//	char socket_addr[INET_ADDRSTRLEN];
//
//	for (std::list<Socket>::iterator it_socket = socket_list.begin(); it_socket != socket_list.end();) {
//		if (it_socket->is_header_read && !it_socket->is_callback_created) {
//			inet_ntop(AF_INET, &it_socket->address.sin_addr, socket_addr, INET_ADDRSTRLEN);
//			for (std::list<Server>::reverse_iterator it_server = server_list.rbegin(); it_server != server_list.rend(); it_server++) {
//				if (!strcmp(socket_addr, &it_server->ip_port.ip[0]))
//					continue ;
//				right_server = it_server;
//				if (is_right_conf(it_server->server_name, it_socket->headers))
//					break ;
//			}
//			it_socket->server = &(*right_server);
//		}
//		it_socket++;
//	}
//}

static bool is_right_conf(std::list<std::string> &serv_name,
						  Header &request) {
	std::list<std::string>::iterator s_it = serv_name.begin(), s_ite = serv_name.end();

	for (; s_it != s_ite; ++s_it) {
		if (*s_it == request.host)
			return (true);
	}
	return (false);
}

static Server *get_right_conf(std::list<Server> &conf,
									  Address &ipport, Header &request) {
	std::list<Server>::reverse_iterator it_conf = conf.rbegin();
	std::list<Server>::reverse_iterator ite_conf = conf.rend();
	std::list<Server>::reverse_iterator right_conf;

	for (; it_conf != ite_conf; ++it_conf) {
		if (ipport.ip != it_conf->ip_port.ip)
			continue ;
		right_conf = it_conf;
		if (is_right_conf(it_conf->server_name, request))
			break ;
	}
	return (&(*right_conf));
}

void assign_server_to_socket(std::list<Server> &server_list, std::list<Socket> &socket_list) {
	std::list<Socket>::iterator it_socket = socket_list.begin();
	std::list<Socket>::iterator ite_socket = socket_list.end();

	while (it_socket != ite_socket) {
		if (it_socket->is_header_read && !it_socket->is_callback_created) {
			it_socket->server = get_right_conf(server_list, it_socket->ip_port,
												it_socket->headers);
		}
		++it_socket;
	}
}
