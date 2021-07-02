#include "Callback.hpp"

bool	Callback::_if_error_page_exist(void) {
    std::string                 path_error_page;
    std::map<int, std::string> ::iterator      it;
    struct stat                 stat;

    it = this->error_page.find(this->status_code);
    if (it != this->error_page.end()) {
        path_error_page.insert(0, (*it).second);
        if (lstat(path_error_page.c_str(), &stat) == 0)
            if (S_ISREG(stat.st_mode)) {
                this->path = path_error_page;
                this->content_length_h = stat.st_size;
                return (true);
            }
    }
    return (false);
}

void	Callback::_gen_error_header_and_body(void) {

    if (_if_error_page_exist() == false) {
        std::string     tmp = get_err_page(this->status_code, this->error_page.find(this->status_code)->second);
        this->content_length_h = tmp.length();
        _gen_resp_headers();
        _resp_headers.append(tmp);
        this->_resp_body = false;
    } else {
        _gen_resp_headers();
        this->_resp_body = true;
    }
}

void	Callback::_send_error_page(void) {
    if (_resp_body)
        _send_respons_body();
}

std::list<Callback::t_task_f>        Callback::_init_error_request(void) {
    std::list<t_task_f> tasks;

    tasks.push_back(&Callback::_gen_error_header_and_body);
    tasks.push_back(&Callback::_send_respons);
    tasks.push_back(&Callback::_send_error_page);
    return (tasks);
}
