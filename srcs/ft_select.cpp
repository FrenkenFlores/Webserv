//
// Created by Jamis Heat on 6/20/21.
//
#include <sys/select.h>
#include <sys/socket.h>
#include <fcntl.h>

#include "Webserv.hpp"
#include "s_socket.hpp"

static void init_fdsets(t_socket_list *const clients, fd_set *read_fdset, fd_set *write_fdset)
{
	t_socket_list::const_iterator cit = clients->begin();
	t_socket_list::const_iterator cite = clients->end();

	FD_ZERO(read_fdset);
	FD_ZERO(write_fdset);

	for (; cit != cite; ++cit) {
		if (cit->client_fd == 0)
			FD_SET(cit->entry_socket, read_fdset);
		else
		{
			FD_SET(cit->client_fd, read_fdset);
			FD_SET(cit->client_fd, write_fdset);
		}
	}
}

static int socket_max(t_socket_list const *const lst) {
	t_socket_list::const_iterator it = lst->begin(), ite = lst->end();
	int max = -1;

	for (; it != ite; ++it) {
		if (it->client_fd == 0 && it->entry_socket > max)
			max = it->entry_socket;
		else if (it->client_fd != 0 && it->client_fd > max)
			max = it->client_fd;
	}
	return (max);
}

static int check_socket_list(t_socket_list *const clients, fd_set *read_fdset, fd_set *write_fdset)
{
	t_socket_list::iterator it = clients->begin();
	t_socket_list::iterator ite = clients->end();
	bool isset_read;
	bool isset_write;
	int flag = 0;

	for (; it != ite; ++it)
	{
		isset_read = FD_ISSET(it->client_fd, read_fdset);
		isset_write = FD_ISSET(it->client_fd, write_fdset);

		it->is_read_ready = isset_read;
		it->is_write_ready = isset_write;

		if ((flag & 1) == false && FD_ISSET(it->entry_socket, read_fdset))
			flag |= 1;
		if ((flag & 2) == false && isset_read && it->is_header_read == false)
			flag |= 2;
	}
	return (flag);
}

bool 	ft_select(t_socket_list *const clients, s_similar_get_req *similar_req)
{
	struct timeval	timeout = {2,0};
	fd_set			read_fdset;
	fd_set			write_fdset;
	int				check_flag;

	init_fdsets(clients, &read_fdset, &write_fdset);
	select(socket_max(clients) + 1, &read_fdset, &write_fdset, NULL, &timeout);
	check_flag = check_socket_list(clients, &read_fdset, &write_fdset);
/*
 * Returns:
 * 0: No new client/header ready
 * 1: New client
 * 2: New header ready to be read
 * 3: 1 & 2
 */
	if (errno == EAGAIN || errno == EINTR || (check_flag & 1) == false) {
		//EAGAIN если сокет помечен как неблокирующий,
		// а в очереди нет запросов на соединение
		errno = 0;
		return (check_flag & 2);
	}
	if (clients->size() > 250)
		return (check_flag & 2);
	//Accept new clients
	t_socket_list new_clients;
	t_socket_list::iterator it = clients->begin();
	t_socket_list::iterator ite = clients->end();
	socklen_t		socklen = sizeof(sockaddr);

	for (; it != ite; ++it) {
		if (it->client_fd != 0 || FD_ISSET(it->entry_socket, &read_fdset) == false)
			continue ;
		s_socket nclient = *it;

		nclient.client_fd = accept(nclient.entry_socket, \
                            &nclient.client_addr, &socklen);
		reset_socket(&nclient);
		nclient.similar_req = similar_req;
		if (errno != 0)
			ft_error("accept",  strerror(errno));
		fcntl(nclient.client_fd, F_SETFL, O_NONBLOCK);
//		new_clients.push_back(nclient);
	}
	clients->splice(clients->end(), new_clients);
	return (check_flag & 2);
}

