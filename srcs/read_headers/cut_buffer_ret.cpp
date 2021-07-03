#include <list>
#include "../utils/utils.hpp"

static std::string del_nodes(std::list<char*> *buffer, unsigned int &len,
        std::list<ssize_t> *len_buf_parts) {
    std::string save_del = "";

    save_del.reserve(len);
    while (buffer->size() > 0 && len > len_buf_parts->front()) {
        len -= len_buf_parts->front();
        save_del += buffer->front();
        free(buffer->front());
        buffer->pop_front();
        len_buf_parts->pop_front();
    }
    return (save_del);
}

char   *cut_buffer_ret(std::list<char*> *buffer, unsigned int len,
        std::list<ssize_t> *len_buf_parts) {
    char *sub_string = NULL;
    unsigned int sub_str_len;
    std::string save_del;

    save_del += del_nodes(buffer, len, len_buf_parts);
    if (buffer->size() > 0) {
        sub_str_len = len_buf_parts->front() - len;
        sub_string = substr(buffer->front(), 0, len);
        save_del += sub_string;
        free(sub_string);
        sub_string = substr(buffer->front(), len, sub_str_len);
        free(buffer->front());
        buffer->pop_front();
        len_buf_parts->pop_front();
        if (strlen(sub_string) > 0) {
            buffer->push_front(sub_string);
            len_buf_parts->push_front(sub_str_len);
        }
        else
            free(sub_string);
    }
    return (strdup(save_del.c_str()));
}