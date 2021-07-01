#ifndef SOCKET_HPP
# define SOCKET_HPP

#include <iostream> // operator<<

#include <sys/types.h> // sockaddr
#include <sys/socket.h>

#include "Address.hpp"
#include "Server.hpp"
#include "RequestHeader.hpp"

struct s_similar_get_req {
    int             client_priority;
    std::string     host;
    std::string     respons;
    std::string     path_respons;
    std::string     original_path;
    Address const  *ipport;
    time_t          last_state_change;
};

struct Socket {
    int                client_fd;
    int                entry_socket;
    bool               is_cache_resp;
    bool               is_read_ready;
    bool               is_write_ready;
    bool               is_header_read;
    bool               is_status_line_read;
    bool               is_callback_created;
    sockaddr           client_addr;
    Address const     *ipport;
    Server const     *server;
    RequestHeader   headers;
    std::list<char*>   buffer;
    s_similar_get_req  *similar_req;
    std::list<ssize_t> len_buf_parts; // Save each len of each buff string
};

void    reset_socket(Socket *s);
#endif
