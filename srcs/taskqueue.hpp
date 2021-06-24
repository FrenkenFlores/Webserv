#pragma once
#include "callback.hpp"
#include "webserv.hpp"

class    TaskQueue {
public:
	void    exec_task() { }
	void    push(std::list<Socket> &clients) {
		Callback cb_temp;
		std::list<Socket>::iterator it_sockets = clients.begin();
		std::list<Socket>::iterator ite_sockets = clients.end();

		while (it_sockets != ite_sockets) {
			if (it_sockets->is_header_read && !it_sockets->is_callback_created &&
				it_sockets->is_cache_resp == false) {
				cb_temp = Callback((*it_sockets),
								   (it_sockets->headers), clients);
				it_sockets->is_callback_created = true;
				_tasks.push(cb_temp);
			}
			++it_sockets;
		}
	}
	size_t  size(void) const { return _tasks.size(); }

private:
	std::queue<Callback>	_tasks;
};
