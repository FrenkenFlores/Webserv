#include "Callback.hpp"

Callback::Callback(void) {
    return ;
}

Callback::Callback(Socket *client, RequestHeader *request,
                       std::list<Socket> *clients) {
    this->_fd_body = 0;
    this->_host = false;
    this->_tmpfile = NULL;
    this->_bytes_read = 0;
    this->_chunk_size = -1;
    this->clients = clients;
    this->_is_aborted = false;
    this->_out_tmpfile = NULL;
    this->_is_outfile_read = false;
    this->original_path = request->path;
    _init_Socket(client);                 // Init client socket variables
    _init_request_header(request);          // Init request headers
    if (this->server) {                     // Init server variables
        _init_server_hpp(this->server);
        _server_init_route(this->server->location);
    }
    if (this->client_max_body_size != -1 &&
            this->content_length > (size_t)this->client_max_body_size) {
        this->status_code = 413;
    }
    if (this->fastcgi_pass != "" && _method_allow() == true) {  // CGI case
        _recipes = _init_recipe_cgi();
    } else {                                // Init recipes
        _init_meth_functions();
        _recipes = _meth_funs[this->method];
    }
    if (_recipes.empty() == true) {         // Case when methods is not known
        _recipes = _init_error_request();
    }
    _it_recipes = _recipes.begin();
    return ;
}

Callback::~Callback(void) {
    if (_tmpfile != NULL) {
        delete _tmpfile;
        _tmpfile = NULL;
    }
    if (_fd_body != 0)
        close(_fd_body);
    if (_out_tmpfile != NULL) {
        delete _out_tmpfile;
        _out_tmpfile = NULL;
    }
    if (this->status_code / 100 != 2) {
        remove_client(this->clients, this->client_fd, this->status_code);
    } else if (_is_aborted == false) {
        reset_socket(this->client);
    }
    for (std::list<char*>::iterator it = _sending_buffer.begin();
            it != _sending_buffer.end(); ++it) {
        free(*it);
    }
    return ;
}

void    Callback::exec(void) {
    std::list<Socket>::iterator it = this->clients->begin();
    std::list<Socket>::iterator ite = this->clients->end();

    while (it != ite) {
        if (&(*it) == this->client)
            break;
        ++it;
    }
    if (it == ite)
        _exit();
    if (this->is_over() == false) {
        if (this->status_code / 100 != 2 &&
                _recipes != _init_error_request()) {
            _recipes = _init_error_request();
            _it_recipes = _recipes.begin();
        } else {
            (this->*(*_it_recipes))();
            if (_is_aborted == false && (this->status_code / 100 == 2 ||
                    _recipes == _init_error_request()))
                ++_it_recipes;
        }
    }
}

bool    Callback::is_over(void) {
    return (_it_recipes == _recipes.end());
}

void    Callback::_exit(void) {
    _it_recipes = _recipes.end();
    _is_aborted = true;
    return ;
}

bool    Callback::_method_allow(void) {
    std::list<std::string>::iterator it, ite;
    bool               allow;

    it = this->methods.begin();
    ite = this->methods.end();
    allow = false;
    for (; it != ite; ++it)
        if (method == *it)
            allow = true;
    if (allow == false)
        this->status_code = 405;
    return (allow);
}

void    Callback::_init_meth_functions(void) {
    if (this->status_code / 100 != 2)
        return ;
    if (this->host.empty() == true) {
        this->status_code = 400;
        return ;
    }
    if (_method_allow() == false)
        return ;
    _meth_funs["PUT"] = _init_recipe_put();
    _meth_funs["GET"] = _init_recipe_get();
    _meth_funs["HEAD"] = _init_recipe_head();
    _meth_funs["POST"] = _init_recipe_post();
    _meth_funs["TRACE"] = _init_recipe_trace();
    _meth_funs["DELETE"] = _init_recipe_delete();
    _meth_funs["OPTIONS"] = _init_recipe_options();
}

void    Callback::_init_request_header(RequestHeader *request) {
    this->_resp_body = false;
    this->date = request->date;
    this->host = request->host;
    this->path = request->path;
    this->method = request->method;
    this->referer = request->referer;
    this->status_code = request->error;
    this->protocol = request->protocol;
    this->user_agent = request->user_agent;
    this->content_type = request->content_type;
    this->authorization = request->authorization;
    this->saved_headers = request->saved_headers;
    this->accept_charset = request->accept_charset;
    this->content_length = request->content_length;
    this->accept_language = request->accept_language;
    if (strcmp(request->transfer_encoding.c_str(), "chunked") != 0 &&
        request->transfer_encoding.empty() == false)
        this->status_code = 400;
    else
        this->transfer_encoding = request->transfer_encoding;
    return ;
}

void        Callback::_init_Socket(Socket *client) {
    this->client = client;
    this->content_length_h = 0;
    this->client_fd = client->client_fd;
    this->client_addr = client->client_addr;
    this->server = (Server*)client->server;
    this->entry_socket = client->entry_socket;
    this->client_buffer = &(client->buffer);
    this->is_read_ready = &(client->is_read_ready);
    this->is_write_ready = &(client->is_write_ready);
}

void        Callback::_init_server_hpp(Server const *server) {
    this->root = server->root;
    this->index = server->index;
    this->listen = server->listen;
    this->autoindex = server->autoindex;
    this->error_page = server->error_page;
    this->server_name = server->server_name;
    this->fastcgi_param = server->fastcgi_param;
    this->client_max_body_size = server->client_max_body_size;
}

std::list<Location>::iterator        Callback::_server_find_route(
    std::list<Location>::iterator &it, std::list<Location>::iterator &ite) {
    std::list<Location>::iterator     it_find;
    std::string                         tmp_path;
    size_t                              i = 0;
    int                                 status = 0;

    it_find = ite;
    i = this->path.find_first_of("/", 1);
    if (i != std::string::npos)
        tmp_path.insert(0, this->path, 0, i);
    else
        tmp_path = this->path;
    for (; it != ite; ++it)
    {
        if (strncmp((*it).route.c_str(), ".", 1) == 0) { // location management by file
            std::string tmp_string;
            size_t found = this->path.find_last_of(".");
             if (found != std::string::npos) {
                tmp_string.insert(0, this->path, found, this->path.length());

                if ((strncmp(tmp_string.c_str(), (*it).route.c_str(),
                        tmp_string.length())) == 0 &&
                        (tmp_string.length() == (*it).route.length())) {
                    it_find = it;
                    // tmp_string = this->path;
                    break ;
                }
            }
        }
        if (strcmp((*it).route.c_str(), "/") == 0)
            it_find = it;
        if ((strncmp(tmp_path.c_str(), (*it).route.c_str(),
                        tmp_path.length())) == 0 &&
                        (tmp_path.length() == (*it).route.length())) {
            it_find = it;
            status = 1;
        }
    }
    if (status == 1 && i != std::string::npos)
        this->path.erase(0, i);
    if (status == 1 && i == std::string::npos)
        this->path.clear();
    return (it_find);
}

void        Callback::_server_init_route(std::list<Location> location) {
    std::list<Location>::iterator     it, ite;

    it = location.begin();
    ite = location.end();
    it = _server_find_route(it, ite);
    if (it != ite) {
        client_max_body_size = (*it).client_max_body_size;
        if ((*it).index.begin() != (*it).index.end())
            index = (*it).index;
        if ((*it).methods.empty() == false)
            methods = (*it).methods;
        if ((*it).root.empty() == false)
            root = (*it).root;
        if ((*it).autoindex.empty() == false)
            autoindex = (*it).autoindex;
        if ((*it).fastcgi_param.empty() == false)
            fastcgi_param = (*it).fastcgi_param;
        if ((*it).error_page.empty() == false)
            error_page = (*it).error_page;
        if ((*it).fastcgi_pass.empty() == false)
            fastcgi_pass = (*it).fastcgi_pass;
    }
}