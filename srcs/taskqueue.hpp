#pragma once
#include "callback.hpp"
#include "webserv.hpp"

class    TaskQueue {
public:
	TaskQueue() { }
	TaskQueue(TaskQueue const &src) { *this = src; }
	virtual ~TaskQueue() {
		while (_tasks.empty() == false) {
			delete _tasks.front();
			_tasks.pop();
		}
	}
	void    set_clients(std::list<Socket> *clients) {_clients = clients; }

	void    exec_task() {
		for (unsigned int i = 0; i < _tasks.size(); ++i) {
			_tasks.front()->exec();
			if (_tasks.front()->is_over() == true)
				delete _tasks.front();             // Ended task deletion
			else
				_tasks.push(_tasks.front());       // Push executed task to call back
			_tasks.pop();                          // Delete task executed
		}
	}
	void    push(std::list<Socket> *clients) {
		Callback *cb_temp = nullptr;
		std::list<Socket>::iterator it_sockets = clients->begin();
		std::list<Socket>::iterator ite_sockets = clients->end();

		while (it_sockets != ite_sockets) {
			if (it_sockets->is_header_read && !it_sockets->is_callback_created &&
				it_sockets->is_cache_resp == false) {
				cb_temp = new Callback((*it_sockets),
										 (it_sockets->headers), *clients);
				it_sockets->is_callback_created = true;
				_tasks.push(cb_temp);
			}
			++it_sockets;
		}
	}
	size_t  size(void) const { return _tasks.size(); }

private:
	std::queue<Callback *>	_tasks;
	std::list<Socket>     *_clients;
};
