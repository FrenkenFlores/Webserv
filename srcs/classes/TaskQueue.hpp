#ifndef TASKQUEUE_HPP
# define TASKQUEUE_HPP
# include <list>
# include <queue>

# include "Socket.hpp"
# include "Callback.hpp"
# include "RequestHeader.hpp"

class    TaskQueue {
 public:

    TaskQueue(void);
    TaskQueue(TaskQueue const &src);
    TaskQueue &operator=(TaskQueue const &src);
    virtual ~TaskQueue();

    void    exec_task(void);
    void    push(std::list<Socket> *clients);
    size_t  size(void) const;

 private:
    std::queue<Callback*> _tasks;
};

#endif
