#include <map>
#include <list>
#include <string>
#include "utils.hpp"
#include "Socket.hpp"
#include "read_headers.hpp"

static bool        is_sep_header(std::list<char*> *buffer) {
    char *i_buf_parts = NULL;
    std::string crlf_save = "";
    std::list<char*>::iterator it = buffer->begin();
    std::list<char*>::iterator ite = buffer->end();

    while (it != ite && crlf_save != "\r\n\r\n") {
        i_buf_parts = *it;
        while (*i_buf_parts && crlf_save != "\r\n\r\n") {
            if (*i_buf_parts == '\r' || *i_buf_parts == '\n')
                crlf_save += *i_buf_parts;
            else
                crlf_save.clear();
            ++i_buf_parts;
        }
        ++it;
    }
    return (crlf_save == "\r\n\r\n");
}

#define BUFF_SIZE_SOCKET 1024

static ssize_t     read_socket(std::list<char*> *buffer, int client_fd,
        std::list<ssize_t> *len_buf_parts) {
    char    *read_buffer = NULL;
    ssize_t bytes_read = 0;

    if (!(read_buffer = (char*)malloc(sizeof(char) * (BUFF_SIZE_SOCKET + 1))))
        return (NULL);
    bzero(read_buffer, BUFF_SIZE_SOCKET + 1);
    bytes_read = recv(client_fd, read_buffer, BUFF_SIZE_SOCKET, 0);
    if (bytes_read > 0) {
        buffer->push_back(read_buffer);
        len_buf_parts->push_back(bytes_read);
    } else if (bytes_read == -1) {
        free(read_buffer);
        throw std::logic_error("read_socket : recv : error");
    }
    else if (bytes_read == 0){
        free(read_buffer);
    }
    return (bytes_read);
}

static void parse_buffer(std::list<char*> *buffer, RequestHeader *headers,
                std::map<std::string, f_request_header> *headers_parsers,
                bool *is_status_line_read, std::list<ssize_t> *len_buf_parts) {
    std::string                  header_to_parse;
    std::map<std::string, void*> headers_ptrs;

    header_to_parse = get_header(buffer, *is_status_line_read, len_buf_parts);
    if (header_to_parse != "")
        headers_ptrs = init_header_ptrs(headers);
    while (header_to_parse != "" && headers->error == 200) {
        if (*is_status_line_read == false) {
            parse_status_line(header_to_parse, &headers_ptrs);
            *is_status_line_read = true;
            if (headers->method == "TRACE") {
                headers->host = "tmp";
                return ;
            }
        } else {
            if (parse_header(header_to_parse, &headers_ptrs, headers_parsers) == 1) {
                headers->error = 400;
            }
            headers->saved_headers.push_back(header_to_parse);
        }
        header_to_parse = get_header(buffer, is_status_line_read,
                                    len_buf_parts);
    }
    cut_buffer(buffer, 2, len_buf_parts);
    return ;
}

bool    read_headers(std::list<Socket> *clients) {
    bool        is_one_req_ready = false;
    ssize_t     bytes_read = 0;
    std::list<Socket>::iterator           it = clients->begin();
    std::list<Socket>::iterator           ite = clients->end();
    std::map<std::string, f_request_header> headers_parsers;

    headers_parsers = init_header_parsers();
    while (it != ite) {
        if (it->is_read_ready == false || it->client_fd == 0 ||
                it->is_header_read == true) {
            ++it;
            continue;
        }
        usleep(100);
        bytes_read = read_socket(&it->buffer, it->client_fd,
                &(it->len_buf_parts));
        if (bytes_read == 0 || bytes_read == -1) {
            remove_client(clients, (it++)->client_fd, bytes_read);
            continue;
        }
        it->is_header_read = is_sep_header(&it->buffer);
        is_one_req_ready |= it->is_header_read;
        if (it->is_header_read == true) {
            parse_buffer(&(it->buffer), &(it->headers), &headers_parsers,
                     &(it->is_status_line_read), &(it->len_buf_parts));
        }
        if (it->headers.error != 200)
            it->is_header_read = true;

        ++it;
    }
    return (is_one_req_ready);
}
