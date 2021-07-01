#include "Socket.hpp"
#include <stdlib.h>

void    reset_socket(Socket *s) {
    std::list<char*>::iterator it_buf = s->buffer.begin();
    std::list<char*>::iterator ite_buf = s->buffer.end();

    s->server = NULL;
    s->is_read_ready = false;
    s->is_cache_resp = false;
    s->is_write_ready = false;
    s->is_header_read = false;
    reset_header(&(s->headers));
    s->is_status_line_read = false;
    s->is_callback_created = false;
    s->len_buf_parts.clear();
    while (it_buf != ite_buf) {
        free(*it_buf);
        s->buffer.erase(it_buf++);
    }
}
