# include "Server.hpp"
Server::Server(void) : client_max_body_size(0), srv_id(0) { }

Server::Server(Server const &src) {
    *this = src;
}

Server::~Server() { }
Server   &Server::operator=(Server const &src) {
    this->client_max_body_size = src.client_max_body_size;
    this->srv_id = src.srv_id;
    this->index = src.index;
    this->listen = src.listen;
    this->server_name = src.server_name;
    this->root = src.root;
    this->autoindex = src.autoindex;
    this->fastcgi_param = src.fastcgi_param;
    this->error_page = src.error_page;
    this->location = src.location;
    return (*this);
}