#include "Callback.hpp"
#include <sys/time.h>
# define HTTP_DATE_FORMAT "%a, %d %b %Y %H:%M:%S %Z"
#define MSG_NOSIGNAL SO_NOSIGPIPE

static std::string get_content_length(int content_length) {
	std::string str_content_length;
    std::string str_cl;

    str_content_length = std::to_string(content_length);
    str_cl = str_content_length;
    return (str_cl);
}


static std::string get_date(void) {
    struct timeval  curr_time;
    char            result[100];

    errno = 0;
    bzero(result, 100);
    if (gettimeofday(&curr_time, NULL) == -1) {
        throw std::logic_error("get_time_of_day() failed");
    }
    strftime(result, 100, HTTP_DATE_FORMAT, localtime(&(curr_time.tv_sec)));
    if (errno != 0) {
        throw std::logic_error("strftime() failed");
    }
    return (result);
}

static void grh_add_headers(std::list<std::string> &headers, Callback &cb) {
    headers.push_back("Server: webserv v6.66");
    try {
        headers.push_back("Date: " + get_date());
    } catch (std::exception &e) {
        cb.status_code = 500;
        std::cerr << e.what() << std::endl;
    }
    try {
        headers.push_back("Content-Length: " + \
                          get_content_length(cb.content_length_h));
    } catch (std::exception &e) {
        cb.status_code = 500;
        std::cerr << e.what() << std::endl;
    }
    if (cb.status_code / 100 == 2)
        headers.push_back("Connection: keep-alive");
    else
        headers.push_back("Connection: close");
    if (cb.location_h != "")
        headers.push_back("Location: " + cb.location_h);
    if (cb.last_modified_h != "")
        headers.push_back("Last-Modified: " + cb.last_modified_h);
}

void    Callback::_gen_resp_headers(void) {
    std::list<std::string> headers;

    headers.push_back(get_status_line(status_code));
    grh_add_headers(headers, *this);
    headers.push_back("\r\n");
    if (_dir_listening_page.size() > 0) {
        headers.insert(headers.end(), _dir_listening_page.begin(),
                                      _dir_listening_page.end());
    }
    _resp_headers = lststr_to_strcont(headers, "\r\n");
}

#define BUFFER_READ 4096
void                    Callback::_send_respons_body(void) {
    char            buf[BUFFER_READ + 1];
    int             bytes_read;
    int             ret;

    if (_resp_body != true) {
        return ;
    }
    if (_fd_body == 0) {
        errno = 0;
        _fd_body = open(this->path.c_str(), O_RDONLY);
        fcntl(_fd_body, F_SETFL, O_NONBLOCK);
        if (_fd_body == -1) {
            std::cerr <<                                                  \
                "ERR : _SEND_RESP_BODY : open() : " << strerror(errno) << \
            std::endl;
            this->status_code = 500;
            --_it_recipes;
            return ;
        }
    }
    if (is_fd_read_ready(_fd_body) == false ||
            *this->is_write_ready == false ) {
        --_it_recipes;
        return ;
    }
    bzero(buf, BUFFER_READ + 1);
    bytes_read = read(_fd_body, buf, BUFFER_READ);
    if (bytes_read > 0) {
        if ((ret = send(client_fd, buf, bytes_read, MSG_NOSIGNAL)) < 1) {
            std::cerr << "_send_respons_body : send() failed" << std::endl;
            remove_client(this->clients, this->client_fd, ret);
            _exit();
            return ;
        }
        if (this->method == "GET" &&
            this->client->similar_req->client_priority == this->client_fd) {
            this->client->similar_req->respons.append(buf);
            this->client->similar_req->client_priority = 0;
        }
        if (bytes_read > 0 && bytes_read == BUFFER_READ)
            --_it_recipes;
    } else if (bytes_read == -1) {
        std::cerr << "_send_respons_body : read() failed" << std::endl;
        this->status_code = 500;
        --_it_recipes;
        return ;
    } else if (bytes_read == 0) {
        close(_fd_body);
    }
    return ;
}

void                    Callback::_send_respons(void) {
    int         ret;
    struct stat stat;

    if (*(this->is_write_ready) == false) {
        _it_recipes--;
        return ;
    }
    if ((ret = send(client_fd, _resp_headers.c_str(),
                    _resp_headers.length(), MSG_NOSIGNAL)) < 1) {
        std::cerr << "Error: Respons to client" << std::endl;
        remove_client(this->clients, this->client_fd, ret);
        _exit();
        return ;
    }
    if (this->method == "GET" && this->status_code == 200) {
            if (this->client->similar_req->client_priority == 0)
                this->client->similar_req->client_priority = this->client_fd;
            this->client->similar_req->host = this->host;
            this->client->similar_req->path_respons = this->path;
            this->client->similar_req->original_path = this->original_path;

            if (lstat(this->path.c_str(), &stat) == -1)
                std::cerr << "Error: lstat _send_respons()" << std::endl;
            this->client->similar_req->last_state_change = stat.st_ctime;
            this->client->similar_req->ipport = this->client->ipport;
            this->client->similar_req->respons = _resp_headers;
    }
}
