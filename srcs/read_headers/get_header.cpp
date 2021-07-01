#include <list>
#include <stdlib.h>
#include "read_headers.hpp"

#define CRLF "\r\n"

std::string get_header(std::list<char*> *buffer, bool is_status_line_read,
        std::list<ssize_t> *len_buf_parts) {
    char         *str_tmp = NULL;
    std::string  final_header = "";
    unsigned int header_len = 0;

    if (is_status_line_read == false)
        flush_crlf(buffer, len_buf_parts);
    if (find_str_buffer(buffer, CRLF) == 0)
        return ("");
    header_len = find_str_buffer(buffer, CRLF);
    str_tmp = cut_buffer_ret(buffer, header_len, len_buf_parts);
    cut_buffer(buffer, 2, len_buf_parts);
    final_header = str_tmp;
    free(str_tmp);
    return (final_header);
}
