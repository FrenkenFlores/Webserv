#include "Location.hpp"

Location::Location(void) : client_max_body_size(-1) {
    return ;
}

Location::Location(Location const &src) {
    *this = src;
}

Location::~Location() { }
Location   &Location::operator=(Location const &src) {
    this->root = src.root;
    this->index = src.index;
    this->route = src.route;
    this->methods = src.methods;
    this->autoindex = src.autoindex;
    this->error_page = src.error_page;
    this->fastcgi_pass = src.fastcgi_pass;
    this->fastcgi_param = src.fastcgi_param;
    this->client_max_body_size = src.client_max_body_size;
    return (*this);
}