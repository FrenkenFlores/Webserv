#include <iostream>

#include "RequestHeader.hpp"
void    reset_header(RequestHeader *h) {
    h->path = "";
    h->date = "";
    h->host = "";
    h->error = 200;
    h->method = "";
    h->referer = "";
    h->protocol = "";
    h->content_length = 0;
    h->user_agent.clear();
    h->content_type.clear();
    h->authorization.clear();
    h->saved_headers.clear();
    h->transfer_encoding = "";
    h->accept_charset.clear();
    h->accept_language.clear();
    h->is_transfer_mode_saw = false;
};
