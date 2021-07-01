#include <list>
#include <stdlib.h>

bool    is_buffer_crlf(std::list<char*> *buffer) {
    char  str_crlf[] = "\r\n";
    char  *i_buf_parts = NULL;
    short i_crlf = 0;
    std::list<char*>::iterator it_buf = buffer->begin();
    std::list<char*>::iterator ite_buf = buffer->end();

    while (it_buf != ite_buf && str_crlf[i_crlf] != '\0') {
        i_buf_parts = *it_buf;
        while (*i_buf_parts != '\0' && str_crlf[i_crlf] != '\0') {
            if (*i_buf_parts == str_crlf[i_crlf]) {
                ++i_crlf;
            } else if (i_crlf != 0) {
                i_crlf = 0;
            }
            ++i_buf_parts;
        }
        ++it_buf;
    }
    if (str_crlf[i_crlf] == '\0')
        return (true);
    return (false);
}
