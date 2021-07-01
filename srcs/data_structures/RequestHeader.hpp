#ifndef REQUEST_HEADER_HPP
# define REQUEST_HEADER_HPP

# include <list>
# include <string>

struct RequestHeader {
    bool                   is_transfer_mode_saw;
    size_t                 error;
    size_t                 content_length;
    std::string            method;
    std::string            path;
    std::string            protocol;
    std::string            date;
    std::string            host;
    std::string            referer;
    std::string            transfer_encoding;
    std::list<std::string> accept_charset;
    std::list<std::string> accept_language;
    std::list<std::string> authorization;
    std::list<std::string> content_type;
    std::list<std::string> user_agent;
    std::list<std::string> saved_headers;
};

void    reset_header(RequestHeader *h);

#endif
