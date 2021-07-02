#include "Callback.hpp"
#include "read_headers.hpp"
#include <sys/types.h>
#include <sys/socket.h>
#include <fcntl.h>
#define MSG_NOSIGNAL SO_NOSIGPIPE


std::list<Callback::t_task_f> Callback::_init_recipe_cgi(void) {
    std::list<t_task_f> recipe;

    recipe.push_back(&Callback::_meth_cgi_init_meta);
    recipe.push_back(&Callback::_meth_cgi_init_http);
    if (this->transfer_encoding == "chunked") {
        recipe.push_back(&Callback::_chunk_reading);
    } else {
        recipe.push_back(&Callback::_meth_cgi_save_client_in);
    }
    recipe.push_back(&Callback::_meth_cgi_launch);
    recipe.push_back(&Callback::_meth_cgi_wait);
    recipe.push_back(&Callback::_meth_cgi_send_http);
    recipe.push_back(&Callback::_meth_cgi_send_resp);
    return (recipe);
}

void    Callback::_meth_cgi_init_meta(void) {
	std::string c_tmp;
    std::string tmp;
    std::string::iterator it_tmp;

    if (this->authorization.size() > 0) {
        tmp = "AUTH_TYPE=" + this->authorization.back();
        this->cgi_env_variables.push_back(tmp);
    }
    tmp = "CONTENT_LENGTH=";
    c_tmp = std::to_string(this->content_length);
    tmp += c_tmp;
    this->cgi_env_variables.push_back(tmp);
    if (this->content_type.size() > 0) {
        tmp = "CONTENT_TYPE=";
        for (std::list<std::string>::iterator it = this->content_type.begin();
                it != this->content_type.end(); ++it) {
            tmp += *it;
            if (it != this->content_type.end())
                tmp += ";";
        }
        this->cgi_env_variables.push_back(tmp);
    }
    tmp = "GATEWAY_INTERFACE=CGI/1.1";
    this->cgi_env_variables.push_back(tmp);
    if (this->path.size() != 0)
        tmp = "PATH_INFO=" + this->path;
    else
        tmp = "PATH_INFO=/";
    this->cgi_env_variables.push_back(tmp);
    tmp = "PATH_TRANSLATED=" + this->root + this->path;
    if (find(this->index.begin(), this->index.end(), std::string("index.php"))
            != this->index.end() && *(--this->path.end()) == '/') {
        tmp += "index.php";
    }
    this->cgi_env_variables.push_back(tmp);
    it_tmp = this->path.end();
    while (it_tmp != this->path.begin() && *it_tmp != '?')
        it_tmp--;
    if (*it_tmp == '?') {
        it_tmp++;
        tmp = "QUERY_STRING=";
        tmp += std::string(it_tmp, this->path.end());
        this->cgi_env_variables.push_back(tmp);
    }
    char ip[20];
    bzero(ip, 20);
    tmp = "REMOTE_ADDR=";
    tmp += inet_ntop(AF_INET, (sockaddr_in*)(&(this->client_addr)), ip, 20);
    this->cgi_env_variables.push_back(tmp);
    if (this->fastcgi_param["REMOTE_USER"] != "") {
        tmp = "REMOTE_USER=";
        tmp += fastcgi_param["REMOTE_USER"];
        this->cgi_env_variables.push_back(tmp);
    }
    if (this->fastcgi_param["REMOTE_IDENT"] != "") {
        tmp = "REMOTE_IDENT=";
        tmp += fastcgi_param["REMOTE_IDENT"];
        this->cgi_env_variables.push_back(tmp);
    }
    tmp = "REDIRECT_STATUS=200";
    this->cgi_env_variables.push_back(tmp);
    tmp = "REQUEST_METHOD=";
    tmp += this->method;
    this->cgi_env_variables.push_back(tmp);
    tmp = "REQUEST_URI=";
    tmp += this->path;
    this->cgi_env_variables.push_back(tmp);
    tmp = "SERVER_NAME=";
    tmp += this->host;
    this->cgi_env_variables.push_back(tmp);
    tmp = "SERVER_PORT=";
    c_tmp = std::to_string(this->listen.port);
    tmp += c_tmp;
    this->cgi_env_variables.push_back(tmp);
    this->cgi_env_variables.push_back("SERVER_PROTOCOL=HTTP/1.1");
    this->cgi_env_variables.push_back("SERVER_SOFTWARE=webserv");

}

void    Callback::_meth_cgi_init_http(void) {
    std::string::iterator cursor;

    for (std::list<std::string>::iterator it = this->saved_headers.begin();
            it != this->saved_headers.end(); ++it) {
        if ((cursor = find(it->begin(), it->end(), ':')) != it->end()) {
            it->replace(cursor, cursor + 2, "=");
        }
        cursor = it->begin();
        while (cursor != it->end() && *cursor != '=') {
            if (*cursor >= 'a' && *cursor <= 'z')
                *cursor -= 32;
            if (*cursor == '-')
                *cursor = '_';
            ++cursor;
        }
        *it = "HTTP_" + *it;
    }
    cgi_env_variables.insert(this->cgi_env_variables.begin(),
            this->saved_headers.begin(), this->saved_headers.end());
}

#define CGI_BUF_SIZE 4096
void    Callback::_meth_cgi_save_client_in(void) {
    int     cat_len;
    char    *read_buf = NULL;
    char    *cat_buf = NULL;
    ssize_t bytes_read;

    if (_tmpfile == NULL)
        _tmpfile = new Tmpfile();
    if (_tmpfile->is_write_ready() == false) {
        --_it_recipes;
        return ;
    }
    if (this->client_buffer->size() > 0) {
        cat_buf = cut_buffer_ret(this->client_buffer, this->content_length,
                &(this->client->len_buf_parts));
        cat_len = strlen(cat_buf);
        if (cat_len > 0 && write(_tmpfile->get_fd(), cat_buf, cat_len) < 1) {
            std::cerr << "ERR: cgi save in : write failed : " << std::endl;
            this->status_code = 500;
        } else {
            this->content_length -= cat_len;
        }
        free(cat_buf);
    } else if (*this->is_read_ready == true) {
        if (!(read_buf = (char*)malloc(sizeof(char) * (CGI_BUF_SIZE)))) {
            std::cerr << "ERR: cgi_save(): malloc() failed" << std::endl;
            status_code = 500;
            return ;
        }
        bzero(read_buf, CGI_BUF_SIZE);
        if (CGI_BUF_SIZE < this->content_length)
            bytes_read = read(this->client_fd, read_buf, CGI_BUF_SIZE);
        else
            bytes_read = read(this->client_fd, read_buf, this->content_length);
        if (bytes_read == -1) {
            std::cerr << \
                "ERR: bytes_read : read client error : " << bytes_read << \
            std::endl;
            remove_client(this->clients, this->client_fd, bytes_read);
            _exit();
            free(read_buf);
            return ;
        } else if (bytes_read == 0) {
            _exit();
            free(read_buf);
            return ;
        } else {
            this->client_buffer->push_back(read_buf);
            this->client->len_buf_parts.push_back(bytes_read);
        }
    }
    if (this->content_length > 0) {
        --_it_recipes;
    } else {
        _tmpfile->reset_cursor();
    }
    return ;
}

static int launch_panic(char **envp, char **args, char *bin_path) {
    if (bin_path != NULL)
        free(bin_path);
    if (envp != NULL)
        ft_freestrs(envp);
    if (args != NULL)
        ft_freestrs(args);
    return (1);
}

void    Callback::_meth_cgi_launch(void) {
    char *bin_path = NULL;
    char **envp = NULL;
    char **args = NULL;
    std::list<std::string> lst_args;

    errno = 0;
    if (_out_tmpfile == NULL) {
        _out_tmpfile = new Tmpfile();
    }
    if (_out_tmpfile->is_write_ready() == false) {
        --_it_recipes;
        return;
    }
    if ((_pid = fork()) == 0) {
        if ((bin_path = strdup(this->fastcgi_pass.c_str())) == NULL)
            exit(launch_panic(envp, args, bin_path));
        if ((envp = lststr_to_strs(this->cgi_env_variables)) == NULL)
            exit(launch_panic(envp, args, bin_path));
        lst_args.push_back(bin_path);
        lst_args.push_back(this->root + this->path);
        if ((args = lststr_to_strs(lst_args)) == NULL)
            exit(launch_panic(envp, args, bin_path));
        if (dup2(_tmpfile->get_fd(), 0) == -1 ||
                dup2(_out_tmpfile->get_fd(), 1) == -1) {
            std::cerr << \
                "cgi_launch : dup2 : " << strerror(errno) << \
            std::endl << std::flush;
            exit(launch_panic(envp, args, bin_path));
        }
        if (chdir(ft_dirname(bin_path).c_str()) == -1) {
            std::cerr <<                                            \
                "cgi_launch : execve : " << strerror(errno) << " : " << \
                ft_dirname(bin_path).c_str() <<                         \
            std::endl << std::flush;
            exit(launch_panic(envp, args, bin_path));
        }
        close(_tmpfile->get_fd());
        if (execve(bin_path, args, envp) == -1) {
            std::cerr <<                                       \
                "cgi_launch : execve : " << strerror(errno) << \
            std::endl << std::flush;
            int nbr = write(1, "Status: 500\r\n\r\n", 15);
            if (nbr != 15) {
                std::cerr << "cgi_launch : execve : write failed" << std::endl;
                exit(launch_panic(envp, args, bin_path));
            } else if (nbr == 0) {
                std::logic_error("cgi_launch: didn't write");
            } else if (nbr < 0) {
                std::logic_error("cgi_launch: write() error");
            } else {
                exit(launch_panic(envp, args, bin_path));
            }
        }
    } else if (_pid == -1) {
        std::cerr << \
            "cgi_launch : execve : " << strerror(errno) << \
        std::endl << std::flush;
        this->status_code = 500;
    }
}

void    Callback::_meth_cgi_wait(void) {
    int   status;
    pid_t dead;

    errno = 0;
    dead = waitpid(_pid, &status, WNOHANG);
    if (dead == -1) {
        std::cerr << "error : waitpid() : " << strerror(errno) << std::endl;
        this->status_code = 500;
        return ;
    } else if (dead == _pid) {
        _out_tmpfile->reset_cursor();
    } else if (dead == 0) {
        --_it_recipes;
    }
}

void    Callback::_meth_cgi_send_http(void) {
    char *http_content = NULL;

    if (*this->is_write_ready == false ||
            _out_tmpfile->is_read_ready() == false) {
        --_it_recipes;
    }
    http_content = cgitohttp(_out_tmpfile, &(this->status_code));
    if (http_content) {
        int nbr = send(this->client_fd, http_content, strlen(http_content), MSG_NOSIGNAL);
        if (nbr < 1) {
            std::cerr << "ERR: _meth_cgi_send_http : send" << std::endl;
            remove_client(this->clients, this->client_fd, -1);
            _exit();
        } else if (nbr == 0) {
                remove_client(this->clients, this->client_fd, -1);
                _exit();
        }
        free(http_content);
        _out_tmpfile->reset_cursor();
    }
    return ;
}

#define CGI_SEND_SIZE 50000

void    Callback::_meth_cgi_send_resp(void) {
    int     buf_size;
    char    *buf;
    ssize_t bytes_send;

    if (_is_outfile_read == false) {
        if (_out_tmpfile->is_read_ready() == false) {
            --_it_recipes;
            return ;
        } else {
            if (!(buf = (char*)malloc(sizeof(char) * (CGI_SEND_SIZE + 1)))) {
                std::cerr << "ERR: cgi send resp: malloc() fail" << std::endl;
                this->status_code = 500;
                return ;
            }
            bzero(buf, CGI_SEND_SIZE + 1);
            if ((buf_size = read(_out_tmpfile->get_fd(), buf, CGI_SEND_SIZE))
                    == -1) {
                std::cerr << \
                "ERR: cgi_send : read error : " << buf_size << std::endl;
                this->status_code = 500;
                return ;
            }
            if (buf_size == 0) {
                _is_outfile_read = true;
                free(buf);
            }
            else {
                _sending_buffer.push_back(buf);
                _len_send_buffer.push_back(buf_size);
            }
        }
    }
    if (*(this->is_write_ready) == true && _sending_buffer.size() > 0) {
        if (_sending_buffer.size() == 0) { // Nothing to right in client
            --_it_recipes;
            return ;
        }
        bytes_send = send(this->client_fd, _sending_buffer.front(),
                           _len_send_buffer.front(), MSG_NOSIGNAL);
        if (bytes_send < 1) {
            std::cerr << "ERR: cgi_send : send error" << std::endl;

        } else if (bytes_send == 0) {
            remove_client(this->clients, this->client_fd, -1);
            _exit();
            return ;
        }
        cut_buffer(&(this->_sending_buffer), bytes_send,
                    &(this->_len_send_buffer));
    }
    if (_sending_buffer.size() > 0 || _is_outfile_read == false) {
        --_it_recipes;
        return ;
    }
    return ;
}
