#include "Callback.hpp"

void                    Callback::_meth_delete_request_is_valid(void) {
    struct stat         stat;
    this->path.insert(0, this->root);

    bzero(&stat, sizeof(struct stat));
    if (!lstat(this->path.c_str(), &stat)) {
        if (S_ISREG(stat.st_mode))
            status_code = 204;
        else if (S_ISDIR(stat.st_mode))
            status_code = 204;
    }
    else
        this->status_code = 404;
    if (S_ISDIR(stat.st_mode) == true &&
       *(--this->path.end()) != '/')
        this->status_code = 409;
}

int                     Callback::_remove_directory(const char *path)
{
    int                 ret;
    DIR                 *curr_directory;
    struct dirent       *it_directory;
    struct stat         stat;

    if ((curr_directory = opendir(path)) == NULL) {
        this->status_code = 404;
        return (-1);
    }
    while ((it_directory = readdir(curr_directory)))
    {
        if (!strcmp(it_directory->d_name, ".") ||
        !strcmp(it_directory->d_name, ".."))
             continue;
        std::string new_path(path);
        new_path.push_back('/');
        new_path.insert(new_path.size(), it_directory->d_name);
        if (!lstat(new_path.c_str(), &stat)) {
            if (S_ISREG(stat.st_mode))
                unlink(new_path.c_str());
            else if (S_ISDIR(stat.st_mode))
                _remove_directory(new_path.c_str());
        }
    }
    closedir(curr_directory);
    ret = rmdir(path);
    return (ret);
}

void                                Callback::_meth_delete_remove(void) {
    struct stat         stat;

    if (!lstat(this->path.c_str(), &stat)) {
        if (S_ISREG(stat.st_mode) || S_ISLNK(stat.st_mode))
            unlink(this->path.c_str());
        else if (S_ISDIR(stat.st_mode))
            _remove_directory(this->path.c_str());
    }
}

std::list<Callback::t_task_f>     Callback::_init_recipe_delete(void) {
    std::list<t_task_f> tasks;

    tasks.push_back(&Callback::_meth_delete_request_is_valid);
    tasks.push_back(&Callback::_meth_delete_remove);
    tasks.push_back(&Callback::_gen_resp_headers);
    tasks.push_back(&Callback::_send_respons);
    tasks.push_back(&Callback::_send_respons_body);
    return (tasks);
}
