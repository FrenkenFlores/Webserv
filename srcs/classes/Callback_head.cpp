#include "Callback.hpp"

std::list<Callback::t_task_f>     Callback::_init_recipe_head(void) {
    std::list<t_task_f> tasks;

    tasks.push_back(&Callback::_meth_get_request_is_valid);
    tasks.push_back(&Callback::_gen_resp_headers);
    tasks.push_back(&Callback::_send_respons);
    return (tasks);
}
