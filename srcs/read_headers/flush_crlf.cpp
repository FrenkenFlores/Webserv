#include <list>
#include "read_headers.hpp"

bool    flush_crlf(std::list<char*> *buffer,
        std::list<ssize_t> *len_buf_parts) {
    char         *i_buf_parts = NULL;
    unsigned int len_to_flush = 0;
    std::list<char*>::iterator    it = buffer->begin();
    std::list<char*>::iterator    ite = buffer->end();

    while (it != ite) {
        i_buf_parts = *it;
        while (*i_buf_parts == '\r' || *i_buf_parts == '\n') {
            ++i_buf_parts;
            ++len_to_flush;
        }
        if (*i_buf_parts != '\0')
            break;
        ++it;
    }
    cut_buffer(buffer, len_to_flush, len_buf_parts);
    return (len_to_flush > 0);
}
