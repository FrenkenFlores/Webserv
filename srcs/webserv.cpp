#include "webserv.hpp"

void    launch_server(std::list<Server> const &conf, std::list<Socket> &clients) {
    bool                        is_new_request = false;
    bool                        has_new_header_ready = false;
	TaskQueue                task_queue;
	s_similar_get_req           similar_req;

	while (true) {
        has_new_header_ready = ft_select(&clients, &similar_req);
		if (has_new_header_ready) {
            is_new_request = read_headers(&clients);
			has_new_header_ready = false;
        }
        if (!similar_req.host.empty()) {        // cache manager
            similar_get_req_sender(&clients, &similar_req);
			similar_get_req_checker(&clients, &similar_req);
		}
        if (is_new_request) {
            assign_server_to_clients(conf, &clients);
			task_queue.push(&clients);
			is_new_request = false;
        } else if (task_queue.size() > 0)
            task_queue.exec_task();
    }
}
