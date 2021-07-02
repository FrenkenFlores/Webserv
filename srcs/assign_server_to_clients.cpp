#include <list>
#include "Socket.hpp"
#include "Server.hpp"
#include "RequestHeader.hpp"

static bool is_right_conf(std::list<std::string> const &serv_name,
            RequestHeader const &request) {
    std::list<std::string>::const_iterator s_it = serv_name.begin(), s_ite = serv_name.end();

    for (; s_it != s_ite; ++s_it) {
        if (*s_it == request.host)
            return (true);
    }
    return (false);
}

static Server const *get_right_conf(std::list<Server> const &conf,
        Address const *const ipport, RequestHeader const &request) {
    std::list<Server>::const_reverse_iterator it_conf = conf.rbegin();
    std::list<Server>::const_reverse_iterator ite_conf = conf.rend();
    std::list<Server>::const_reverse_iterator right_conf;

    for (; it_conf != ite_conf; ++it_conf) {
        if (*ipport != it_conf->listen)
            continue ;
        right_conf = it_conf;
        if (is_right_conf(it_conf->server_name, request))
            break ;
    }
    return (&(*right_conf));
}

void    assign_server_to_clients(std::list<Server> const &conf,
                                 std::list<Socket> *const clients) {
    std::list<Socket>::iterator it_clients = clients->begin();
    std::list<Socket>::iterator ite_clients = clients->end();

    while (it_clients != ite_clients) {
        if (it_clients->is_header_read && !it_clients->is_callback_created) {
            it_clients->server = get_right_conf(conf, it_clients->ipport,
                                                it_clients->headers);
        }
        ++it_clients;
    }
}
