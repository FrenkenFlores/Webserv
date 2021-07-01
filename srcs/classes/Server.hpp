#ifndef SERVER_HPP
# define SERVER_HPP

#include <string>
#include <list>
#include <iostream>

#include "std_typedefs.hpp"
#include "Address.hpp"
#include "Location.hpp"

class   Server {
public:

    Server(void);
    Server(Server const &src);
    Server &operator=(Server const &src);
    virtual ~Server();

    int                     client_max_body_size;
    int                     srv_id;
    t_strlst                index;
    Address               	listen;
    t_strlst                server_name;
    std::string             root;
    std::string             autoindex;
    t_cgi_param             fastcgi_param;
    t_error_page            error_page;
    std::list<Location>   location;

};

#endif
