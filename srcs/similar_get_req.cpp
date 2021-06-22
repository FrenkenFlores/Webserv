//
// Created by Jamis Heat on 6/22/21.
//

#include "Webserv.hpp"

void    similar_get_req_sender(std::list<Socket> *clients,
							   IdenticalGetRequest *similar_req) {
	int         resp_len = similar_req->respons.length();
	char const  *resp = similar_req->respons.c_str();
	std::list<Socket>::iterator it = clients->begin();
	std::list<Socket>::iterator ite = clients->end();

	for (; it != ite; ++it) {
		if (it->is_cache_resp == false || it->is_write_ready == false)
			continue ;
		if (send(it->response_fd, resp, resp_len, 0) < 1) {
			std::cerr << "ERR: cache_sender: Respons to client" << std::endl;
			close(it->response_fd);
			clients->erase(it++);
			continue ;
		}
//		reset_socket(&(*it));
	}
}