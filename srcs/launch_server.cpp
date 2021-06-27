#include "webserv.hpp"
#include "taskqueue.hpp"

bool g_run = true;

void test(std::list<Socket> &socket_list) {
	std::list<Socket>::iterator it_b = socket_list.begin();
	std::list<Socket>::iterator it_e = socket_list.end();
	while (it_b != it_e) {
		send(it_b->response_fd, "Hello world", strlen("Hello world"), 0);
		it_b++;
	}
}

void test_2(Socket &i) {
	std::cout << "{" <<                                                       \
    COLOR_WHITE_("entry_socket") << " = " << i.listen_fd <<        \
    COLOR_WHITE_(", client_fd") << " = " << i.response_fd << std::endl;
	std::cout <<                                                   \
    COLOR_WHITE_(" is_read_ready") << " = " <<             \
        std::boolalpha << COLOR_BOOL_(i.is_read_ready) <<  \
    COLOR_WHITE_(", is_write_ready") << " = " <<           \
        std::boolalpha << COLOR_BOOL_(i.is_write_ready) << \
    COLOR_WHITE_(", is_header_read") << " = " <<           \
        std::boolalpha << COLOR_BOOL_(i.is_header_read) << \
    std::endl;
	std::cout <<                                                         \
    COLOR_WHITE_(" is_status_line_read") << " = " <<             \
        std::boolalpha << COLOR_BOOL_(i.is_status_line_read) <<  \
    COLOR_WHITE_(", is_callback_created") << " = " <<            \
        std::boolalpha << COLOR_BOOL_(i.is_callback_created) <<  \
    COLOR_WHITE_(", is_cache_resp") << " = " <<            \
        std::boolalpha << COLOR_BOOL_(i.is_cache_resp) <<  \
    std::endl;
	//  std::cout << COLOR_WHITE_(" buffer") << " = " << i.buffer;
//    std::cout << COLOR_WHITE_(" len_buf_parts") << " = " << i.len_buf_parts;

	std::cout << "}" << std::endl;
}




static void init_sets(const std::list<Socket> &socket_list, fd_set &read_set, fd_set &write_set) {
	FD_ZERO(&read_set);
	FD_ZERO(&write_set);
	for (std::list<Socket>::const_iterator it = socket_list.begin(); it != socket_list.end(); ++it) {
		if (it->response_fd == 0) {
			FD_SET(it->listen_fd, &read_set);
		} else {
			FD_SET(it->response_fd, &read_set);
			FD_SET(it->response_fd, &write_set);
		}
	}
}

static int max_fd(std::list<Socket> &socket_list) {
	int max = -1;

	for (std::list<Socket>::const_iterator it = socket_list.begin(); it != socket_list.end(); ++it) {
		if (it->response_fd == 0 && it->listen_fd > max)
			max = it->listen_fd;
		else if (it->response_fd != 0 && it->response_fd > max)
			max = it->response_fd;
	}
	return (max);
}

int check_sets(std::list<Socket> &socket_list, fd_set &read_set, fd_set &write_set) {
	std::list<Socket>::iterator it = socket_list.begin(), ite = socket_list.end();
	bool read_set_ready, write_set_ready;
	int flag = 0;

	while (it != ite) {
		read_set_ready = FD_ISSET(it->response_fd, &read_set);
		write_set_ready = FD_ISSET(it->response_fd, &write_set);

		it->is_read_ready = read_set_ready;
		it->is_write_ready = write_set_ready;
		if ((flag & 1) == false && FD_ISSET(it->listen_fd, &read_set))
			flag |= 1;
		if ((flag & 2) == false && read_set_ready && !it->is_header_read)
			flag |= 2;
		it++;
	}
	return (flag);
}


bool	ft_select(std::list<Socket> &socket_list, IdenticalGetRequest &similar_req) {
    fd_set  r_fdset, w_fdset;
    struct timeval time = {2, 0};
    int     updated_flag;

    errno = 0;
    init_sets(socket_list, r_fdset, w_fdset);
    select(max_fd(socket_list) + 1, &r_fdset, &w_fdset, NULL, &time);
    updated_flag = check_sets(socket_list, r_fdset, w_fdset);
    if (errno == EAGAIN || errno == EINTR || (updated_flag & 1) == false) {
        // std::cout << "*silence*" << std::endl;
        errno = 0;
        return (updated_flag & 2);
    }
    else if (errno != 0)
        std::perror("select");
    if (socket_list.size() > 250) // Too much open fd protection
        return (updated_flag & 2);
    // ACCEPT NEW CLIENTS
    std::list<Socket> new_clients;
    std::list<Socket>::iterator it = socket_list.begin(), ite = socket_list.end();
    socklen_t   socklen = sizeof(sockaddr);
    for (; it != ite; ++it) {
        if (it->response_fd != 0 || FD_ISSET(it->listen_fd, &r_fdset) == false)
            continue ;
        Socket nclient = *it;

        nclient.response_fd = accept(nclient.listen_fd, \
                            (sockaddr *)&nclient.address, &socklen);
        reset_socket(nclient);
        nclient.similar_req = &similar_req;
        if (errno != 0)
            std::perror("accept");
        fcntl(nclient.response_fd, F_SETFL, O_NONBLOCK);
        new_clients.push_back(nclient);
    }
    socket_list.splice(socket_list.end(), new_clients);
    // std::cout << "*Toc toc toc*" << std::endl;
    return (updated_flag & 2);
}



//bool	ft_select(std::list<Socket> &socket_list, IdenticalGetRequest &similar_req) {
//	fd_set read_set;
//	fd_set write_set;
//	timeval timeout = {1, 0};
//
//	errno = 0;
//	init_sets(socket_list, read_set, write_set);
//	if (select(max_fd(socket_list) + 1, &read_set, &write_set, nullptr, &timeout) == -1) {
//		throw std::logic_error("select error");
//	}
//	int updated_flag = check_sets(socket_list, read_set, write_set);
//	if (errno == EAGAIN || errno == EINTR || (updated_flag & 1) == false) {
//		errno = 0;
//		return (updated_flag & 2);
//	}
//	std::list<Socket>::iterator ite = socket_list.end();
//	socklen_t   addr_size;
//	for (std::list<Socket>::iterator it = socket_list.begin(); it != ite; ++it) {
//		addr_size = sizeof(sockaddr_in);
//		if (it->response_fd != 0 || FD_ISSET(it->listen_fd, &read_set) == false)
//			continue ;
//		if ((it->response_fd = accept(it->listen_fd, (sockaddr *)&it->address, &addr_size)) == -1)
//			it->similar_req = similar_req;
//		fcntl(it->listen_fd, F_SETFL, O_NONBLOCK);
//		socket_list.push_back(*it);
//	}
//	return (updated_flag & 2);
//}

void launch_server(std::list<Server> &server_list, std::list<Socket> &socket_list) {
	TaskQueue *task_queue = new TaskQueue();
	IdenticalGetRequest	similar_req;

	bool	is_new_request;
	bool	has_new_header_ready;
	task_queue->set_clients(&socket_list);
	while (g_run) {
//	    std::cout << "x" << std::endl;
		has_new_header_ready = ft_select(socket_list, similar_req);
		if (has_new_header_ready) {
            std::cout << "y" << std::endl;
            is_new_request = read_headers(socket_list);
			has_new_header_ready = false;
			std::cout << similar_req.ip_port.ip << std::endl;
            std::cout << "yy" << std::endl;
        }
		if (similar_req.host.empty() == false) {        // If a cache is ready
            std::cout << "z" << std::endl;
            similar_get_req_sender(socket_list, similar_req);
			similar_get_req_checker(socket_list, similar_req);
            std::cout << "zz" << std::endl;
        }
		if (is_new_request) {
            std::cout << "a" << std::endl;
            assign_server_to_socket(server_list, socket_list);
			task_queue->push(&socket_list);
			is_new_request = false;
            std::cout << "aa" << std::endl;
        } else if (task_queue->size() > 0) {
            std::cout << "b" << std::endl;
            task_queue->exec_task();
            std::cout << "bb" << std::endl;
        }
	}
}