#include "Callback.hpp"

std::string         Callback::_find_index_if_exist(void) {
    std::list<std::string>::iterator    it, ite;
    std::string                         tmp_path = this->path;
    std::string::iterator               it_path;
    struct stat                         stat;

    it = this->index.begin();
    ite = this->index.end();
    it_path = tmp_path.end();
    if (*(--(tmp_path.end())) != '/')
        tmp_path.insert(tmp_path.end(), '/');
    for (; it != ite ; ++it)
    {
        tmp_path.insert(tmp_path.length(), *it);
        if (lstat(tmp_path.c_str(), &stat) == 0) {
            this->content_length_h = stat.st_size;
            break ;
        }
        tmp_path = this->path;
    }
    return (tmp_path);
}

void                Callback::_meth_get_request_is_valid(void) {
    struct stat     stat;
    std::string     tmp_path;
    this->path.insert(0, this->root);

    if (this->method == "GET")
        _resp_body = true;
    tmp_path = this->path;
    errno = 0;
    if (lstat(this->path.c_str(), &stat) == -1) {
        std::cerr <<                                        \
            "ERR: get first lstat : " << strerror(errno) << \
        std::endl;
        this->status_code = 404;
        return ;
    }
    if (S_ISDIR(stat.st_mode) == true) {
        this->path = _find_index_if_exist();
        if (this->path == tmp_path && this->autoindex == "on") {
            _dir_listening_page = gen_listening(this->path);
            _resp_body = false;
            content_length_h = lststr_len(_dir_listening_page, "\r\n");
            return ;
        } else if (this->path == tmp_path) {
            this->status_code = 404;
            return ;
        }
    }
    if (lstat(this->path.c_str(), &stat) == -1) {
        this->status_code = 404;
        return ;
    } else if ((stat.st_mode & S_IRUSR) == false) {
        this->status_code = 403;
            return ;
    }
    this->content_length_h = stat.st_size;
}

std::list<Callback::t_task_f>     Callback::_init_recipe_get(void) {
    std::list<t_task_f>     tasks;

    tasks.push_back(&Callback::_meth_get_request_is_valid);
    tasks.push_back(&Callback::_gen_resp_headers);
    tasks.push_back(&Callback::_send_respons);
    tasks.push_back(&Callback::_send_respons_body);
    return tasks;
}
