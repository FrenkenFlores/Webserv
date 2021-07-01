#include "webserv.hpp"
#include "Server.hpp"
#include "Socket.hpp"

#include <list>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netdb.h>

Socket get_socket(const Server &server) {
	int yes = 1;
	addrinfo hints;
	addrinfo *hints_list;
	int return_value;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;
	hints.ai_protocol = IPPROTO_TCP;
//	getaddrinfo(ip_address, port_number, data_structer_to_fill, list_of_filled_data_structers);
	if ((return_value = getaddrinfo(server.listen.ip.c_str(), std::to_string(server.listen.port).c_str(), &hints, &hints_list)) != 0) {
		std::cerr << gai_strerror(return_value);
		exit(100);
	}

	Socket new_socket;
	new_socket.ipport = &server.listen;
	addrinfo *ptr = hints_list;
	while (ptr != nullptr) {
		if ((new_socket.entry_socket = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol)) == -1) {
			perror("socket status_code");
			ptr = ptr->ai_next;
			continue;
		}
		if (setsockopt(new_socket.entry_socket, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof (int)) == -1) {
			throw std::logic_error("Setsockopt status_code");
		}

		if (bind(new_socket.entry_socket, ptr->ai_addr, ptr->ai_addrlen) == -1) {
			perror("bind status_code");
			ptr = ptr->ai_next;
			close (new_socket.entry_socket);
			continue;
		}
		break;
	}
	if (ptr == nullptr) {
		close(new_socket.entry_socket);
		throw std::logic_error("could not bind socket");
	}
	if (listen(new_socket.entry_socket, g_worker_connections) == -1) {
		close (new_socket.entry_socket);
		throw std::logic_error("listen status_code");
	}
	freeaddrinfo(hints_list);
	return (new_socket);
}


void init_socket_list(std::list<Server> &server_list, std::list<Socket>	&socket_list) {
	std::list<Server>::const_iterator it = server_list.begin();
	std::list<Server>::const_iterator ite = server_list.end();

	while (it != ite) {
		if (!(it->listen.ip.empty()) && it->listen.port != -1) {
			socket_list.push_back(get_socket(*it));
		}
		++it;
	}
}














//static int  makeSocketfd(const Address &ipport) {
//    int newSocket;
//    const int opt = 1;
//    sockaddr_in servaddr;
//
//    if ((newSocket = socket(AF_INET, SOCK_STREAM, 0)) < 0)
//        throw std::logic_error("socket");
//
//    bzero(&servaddr, sizeof(servaddr));
//    servaddr.sin_family = AF_INET;
//    servaddr.sin_addr.s_addr = inet_addr(ipport.ip.c_str());
//    servaddr.sin_port = htons(ipport.port);
//
//    if (servaddr.sin_addr.s_addr == static_cast<in_addr_t>(-1))
//        throw std::logic_error("inet_addr: Invalid IP");
//    if (setsockopt(newSocket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)))
//        throw std::logic_error("setsockopt");
//    #ifdef __APPLE__
//        if (setsockopt(newSocket, SOL_SOCKET, SO_NOSIGPIPE, &opt, sizeof(opt)))
//            throw std::logic_error("setsockopt");
//    #endif
//    if ((bind(newSocket, (sockaddr *)&servaddr, sizeof(servaddr))) < 0)
//        throw std::logic_error(std::string("bind: ") + strerror(errno));
//    if ((listen(newSocket, 1000)) < 0)
//        throw std::logic_error("listen");
//    return (newSocket);
//}
//
//static Socket makeSocket(const Server *server) {
//    Socket newSocket;
//
//    newSocket.ipport = NULL;
//    newSocket.server = NULL;
//    newSocket.client_fd = 0;
//    newSocket.entry_socket = 0;
//    bzero(&newSocket.client_addr, sizeof(sockaddr));
//    newSocket.entry_socket = makeSocketfd(server->listen);
//    newSocket.ipport = &server->listen;
//
//    reset_socket(&newSocket);
//    return (newSocket);
//}
//
//void    init_clients(std::list<Server> const &conf, std::list<Socket> *clients) {
//    std::list<Server>::const_iterator conf_it = conf.begin();
//    std::list<Server>::const_iterator conf_ite = conf.end();
//    std::list<Socket>::const_iterator sock_it;
//    std::list<Socket>::const_iterator sock_ite;
//
//    for (; conf_it != conf_ite; ++conf_it) {
//        if (conf_it->listen.ip.empty())
//            continue ;
//        sock_it = clients->begin();
//        sock_ite = clients->end();
//        while (sock_it != sock_ite && !(*sock_it->ipport == conf_it->listen))
//            ++sock_it;
//        if (sock_it != sock_ite) // Avoid duplicated sockets
//            continue ;
//        clients->push_back(makeSocket(&(*conf_it)));
//    }
//}
