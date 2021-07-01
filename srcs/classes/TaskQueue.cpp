# include "TaskQueue.hpp"

TaskQueue::TaskQueue(void) { }

TaskQueue::~TaskQueue() {
	while (_tasks.empty() == false) {
		delete _tasks.front();
		_tasks.pop();
	}
}

TaskQueue::TaskQueue(TaskQueue const &src) {
    *this = src;
}



size_t   TaskQueue::size(void) const {
    return (_tasks.size());
}

void    TaskQueue::exec_task(void) {
    for (unsigned int i = 0; i < _tasks.size(); ++i) {
        _tasks.front()->exec();
        if (_tasks.front()->is_over() == true)
            delete _tasks.front();
        else
            _tasks.push(_tasks.front());
        _tasks.pop();
    }
}

void    TaskQueue::push(std::list<Socket> *clients) {
    Callback *cb_temp = NULL;
    std::list<Socket>::iterator it_clients = clients->begin();
    std::list<Socket>::iterator ite_clients = clients->end();

    while (it_clients != ite_clients) {
        if (it_clients->is_header_read && !it_clients->is_callback_created &&
                it_clients->is_cache_resp == false) {
            cb_temp = new Callback(&(*it_clients),
                    &(it_clients->headers), clients);
            it_clients->is_callback_created = true;
            _tasks.push(cb_temp);
        }
        ++it_clients;
    }
}

TaskQueue    &TaskQueue::operator=(TaskQueue const &src) {
    (void)src;
    return (*this);
}
