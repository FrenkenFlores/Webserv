#include "webserv.hpp"

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
	if ((return_value = getaddrinfo(server.ip_port.ip.c_str(), std::to_string(server.ip_port.port).c_str(), &hints, &hints_list)) != 0) {
		std::cerr << gai_strerror(return_value);
		exit(100);
	}

	Socket new_socket;
	addrinfo *ptr = hints_list;
	while (ptr != nullptr) {
		if ((new_socket.listen_fd = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol)) == -1) {
			perror("socket status_code");
			ptr = ptr->ai_next;
			continue;
		}
		if (setsockopt(new_socket.listen_fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof (int)) == -1) {
			throw std::logic_error("Setsockopt status_code");
		}

		if (bind(new_socket.listen_fd, ptr->ai_addr, ptr->ai_addrlen) == -1) {
			perror("bind status_code");
			ptr = ptr->ai_next;
			close (new_socket.listen_fd);
			continue;
		}
//		char ipv4[INET_ADDRSTRLEN];
//		inet_ntop(AF_INET, ptr->ai_addr, ipv4, INET_ADDRSTRLEN);
//		std::cout << ipv4 << std::endl;
		break;
	}
	if (ptr == nullptr) {
		close(new_socket.listen_fd);
		throw std::logic_error("could not bind socket");
	}
	if (listen(new_socket.listen_fd, g_worker_connections) == -1) {
		close (new_socket.listen_fd);
		throw std::logic_error("listen status_code");
	}
	freeaddrinfo(hints_list);
	return (new_socket);
}


void init_socket_list(std::list<Server> &server_list, std::list<Socket>	&socket_list) {
	std::list<Server>::const_iterator it = server_list.begin();
	std::list<Server>::const_iterator ite = server_list.end();

	while (it != ite) {
		if (!(it->ip_port.ip.empty()) && it->ip_port.port != -1) {
			socket_list.push_back(get_socket(*it));
		}
		++it;
	}
}
