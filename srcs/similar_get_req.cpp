//
// Created by Jamis Heat on 6/22/21.
//

#include "Webserv.hpp"

static bool    check_old_respons_is_valid(Socket &socket,
										  IdenticalGetRequest &similar_req) {
	struct stat stat;

	if (socket.ip_port.ip != similar_req.ip_port.ip &&
			socket.ip_port.port != similar_req.ip_port.port) //TODO operator != deleted
		return (false);
	if (socket.headers.host != similar_req.host)
		return (false);
	if (similar_req.last_state_change != stat.st_ctime)
		return (false);
	return (true);
}

void    similar_get_req_sender(std::list<Socket> &socket_list,
							   IdenticalGetRequest &similar_req) {
	int         resp_len = similar_req.respons.length();
	char const  *resp = similar_req.respons.c_str();
	std::list<Socket>::iterator it = socket_list.begin();
	std::list<Socket>::iterator ite = socket_list.end();

	for (; it != ite; ++it) {
		if (it->is_cache_resp == false || it->is_write_ready == false)
			continue ;
		if (send(it->response_fd, resp, resp_len, 0) < 1) {
			std::cerr << "ERR: cache_sender: Respons to client" << std::endl;
			close(it->response_fd);
			socket_list.erase(it++);
			continue ;
		}
		reset_socket(*it);
	}
}

void    similar_get_req_checker(std::list<Socket> &socket_list,
								IdenticalGetRequest &similar_req) {
	struct stat                   stat;
	std::list<Socket>::iterator it = socket_list.begin();
	std::list<Socket>::iterator ite = socket_list.end();

	for (; it != ite; ++it) {
		if (lstat(similar_req.path_respons.c_str(), &stat) == -1 ||
			it->response_fd == 0 || it->is_header_read == false)
			continue ;
		if (it->is_callback_created == false && it->is_write_ready == true &&
			it->headers.method == "GET"                                &&
			it->headers.path == similar_req.original_path             &&
			it->headers.status_code == 200                                   &&
			check_old_respons_is_valid((*it), similar_req) == true     &&
			(it->ip_port.ip == similar_req.ip_port.ip //TODO operator == deleted
			&& it->ip_port.port == similar_req.ip_port.port) &&
			it->headers.host == similar_req.host                      &&
			similar_req.last_state_change == stat.st_ctime) {
			it->is_cache_resp = true;
		}
	}
}