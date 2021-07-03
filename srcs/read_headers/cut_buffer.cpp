#include <list>
#include "../utils/utils.hpp"

static void del_nodes(std::list<char*> *buffer, unsigned int &len,
        std::list<ssize_t> *len_buf_parts) {
    while (buffer->size() > 0 && len > strlen(buffer->front())) {
        len -= strlen(buffer->front());
        free(buffer->front());
        buffer->pop_front();
        len_buf_parts->pop_front();
    }
}

void    cut_buffer(std::list<char*> *buffer, unsigned int len,
        std::list<ssize_t> *len_buf_parts) {
    char *sub_string = NULL;
    unsigned int sub_str_len;

    del_nodes(buffer, len, len_buf_parts);
    if (buffer->size() > 0) {
        sub_str_len = len_buf_parts->front() - len;
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
}
