#include "Webserv.hpp"

static int  makeSocketfd(const Address &ip_port) {
	int sockfd;

	return (sockfd);
}


static Socket makeSocket(const Server &server) {
	Socket new_socket;

	new_socket.entry_socket = makeSocketfd(server.ip_port);
	new_socket.ip_port = server.ip_port;
	return (new_socket);
}


std::list<Socket>	*init_clients(std::list<Server> &server_list) {
	std::list<Socket>	*clients = new std::list<Socket>;

	return clients;
}
