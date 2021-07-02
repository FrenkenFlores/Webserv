#ifndef CALLBACK_HPP
# define CALLBACK_HPP

# include <fcntl.h>
# include <unistd.h>
# include <dirent.h>
# include <sys/stat.h>
# include <sys/wait.h>
# include <sys/types.h>
# include <arpa/inet.h>
# include <sys/socket.h>
# include <netinet/ip.h>
# include <map>
# include <list>
# include <string>
# include <sstream>
# include <iostream>
# include <algorithm>

# include "utils.hpp"
# include "Socket.hpp"
# include "Address.hpp"
# include "Server.hpp"
# include "Tmpfile.hpp"
# include "Location.hpp"
# include "RequestHeader.hpp"

class	Callback
{
public:
    typedef void (Callback::*t_task_f)(void);
    typedef std::list<t_task_f>                 t_recipes;
    typedef std::list<t_task_f>::const_iterator t_recipes_it;

    Callback(void);
    virtual ~Callback();

    Callback(Socket *client, RequestHeader *request,
               std::list<Socket> *clients);

    int                         client_fd;
    int                         entry_socket;
    bool                        *is_read_ready;
    bool                        *is_write_ready;
    Server                    *server;
    Socket                    *client;
    sockaddr                    client_addr;
    std::list<char*>            *client_buffer;
    std::list<Socket>         *clients;

    int                   client_max_body_size;
    std::list<std::string>              index;
    std::list<std::string>              methods;
    Address              listen;
    std::list<std::string>              server_name;
    std::string           root;
    std::string           autoindex;
    std::string           fastcgi_pass;
    std::map<std::string, std::string>           fastcgi_param;
    std::map<int, std::string>           error_page;

    std::string                 method;
    std::string                 path;
    std::string                 original_path;
    std::string                 protocol;
    std::string                 date;
    std::string                 host;
    std::string                 referer;
    std::string                 transfer_encoding;
    std::list<std::string>      accept_charset;
    std::list<std::string>      accept_language;
    std::list<std::string>      authorization;
    std::list<std::string>      content_type;
    std::list<std::string>      user_agent;
    std::list<std::string>      saved_headers;
    size_t                      content_length;
    size_t                      status_code;

    size_t      content_length_h;
    std::string location_h;
    std::string last_modified_h;

    bool                        is_over();
    void                        exec();

private:
    void    _init_request_header(RequestHeader *request);
    void    _init_Socket(Socket *client);
    void    _init_server_hpp(Server const *server);
    void    _server_init_route(std::list<Location> location);

    std::list<Location>::iterator    _server_find_route(
                                         std::list<Location>::iterator &it,
                                         std::list<Location>::iterator &ite);

    void                _init_meth_functions(void);
    std::map<std::string, std::list<t_task_f> > _meth_funs;
    bool                _method_allow(void);

    std::list<t_task_f> _init_recipe_get(void);
    std::list<t_task_f> _init_recipe_put(void);
    std::list<t_task_f> _init_recipe_cgi(void);
    std::list<t_task_f> _init_recipe_head(void);
    std::list<t_task_f> _init_recipe_delete(void);
    std::list<t_task_f> _init_recipe_options(void);
    std::list<t_task_f> _init_recipe_post(void);
    std::list<t_task_f> _init_recipe_trace(void);
    std::list<t_task_f> _init_error_request(void);

    bool         _is_aborted;
    t_recipes    _recipes;
    t_recipes_it _it_recipes;
    void _exit(void);
    void  _gen_error_header_and_body(void);
    void  _send_error_page(void);
    void  _gen_resp_headers(void);
    std::string _resp_headers;
    bool    _if_error_page_exist(void);
    bool    _resp_body;
    Tmpfile *_tmpfile;
    int     _chunk_size;
    void    _chunk_reading(void);
    int     _fd_body;
    void    _send_respons_body(void);
    void    _send_respons(void);
    std::list<std::string> _dir_listening_page;
    void            _meth_get_request_is_valid(void);
    std::string     _find_index_if_exist(void);
    int      _remove_directory(const char *path);
    void     _meth_delete_request_is_valid(void);
    void     _meth_delete_remove(void);
    void     _meth_put_open_fd(void);
    void     _meth_put_choose_in(void);
    void     _meth_put_write_body(void);
    int      _put_fd_in;
    int      _fd_to_write;
    void    _read_body_post(void);
    int     _bytes_read;
    void    _gen_resp_header_options(void);
    void    _read_client_to_tmpfile(void);
    void    _write_request_line(void);
    bool    _host;
    bool                   _is_outfile_read;
    pid_t                  _pid;
    Tmpfile              *_out_tmpfile;
    std::list<char*>       _sending_buffer;
    std::list<ssize_t>     _len_send_buffer;
    std::list<std::string> cgi_env_variables;
    void    _meth_cgi_init_meta(void);
    void    _meth_cgi_init_http(void);
    void    _meth_cgi_save_client_in(void);
    void    _meth_cgi_launch(void);
    void    _meth_cgi_wait(void);
    void    _meth_cgi_send_http(void);
    void    _meth_cgi_send_resp(void);
};

#endif
