#ifndef SERVER_HPP
# define SERVER_HPP

#include <string>
#include <list>
#include <iostream>
#include <map>
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
    std::list<std::string>                index;
    Address               	listen;
    std::list<std::string>                server_name;
    std::string             root;
    std::string             autoindex;
    std::map<std::string, std::string>             fastcgi_param;
    std::map<int, std::string>             error_page;
    std::list<Location>   location;

};

#endif
