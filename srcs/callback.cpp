#include "callback.hpp"


#define CGI_SEND_SIZE 50000
#define PUT_OPEN_FLAGS (O_WRONLY | O_TRUNC)
#define PUT_OPEN_CREAT_FLAGS (O_WRONLY | O_TRUNC | O_CREAT)
#define KEY_STATUS "Status: "
#define CHUNK_CLOSE -3
#define CHUNK_FATAL -2
#define CHUNK_ERROR -1
#define CHUNK_MORE   0
#define CHUNK_ENOUGH 1
#define CHUNK_END    2
#define BUFFER_SIZE 4096
#define MID_TEMPLATE "<a href=\"ELEM_NAME\">ELEM_NAME</a>"
#define TOP_PAGE "<html>\n"                                      \
                 "<head><title>Index of /jpeg/</title></head>\n" \
                 "<body>\n"                                      \
                 "<h1>Index of REQ_PATH</h1>"                    \
                 "<hr><pre><a href=\"../\">../</a>"
#define BOT_HTML "</pre><hr>"                              \
                 "<center>drunkserv v6.66</center></body>" \
                 "</html>"

// SHARED METHODS
void    Callback::chunk_reading(void) {
	int status = CHUNK_ENOUGH;

	if (_tmpfile == NULL)
		_tmpfile = new TmpFile();
	if (((is_buffer_crlf(this->client_buffer) == false &&
		  (this->socket.is_read_ready) == false)) ||
		_tmpfile->is_write_ready() == false) {
		--_recipes_it;
		return ;
	}
	if (this->socket.is_read_ready == true &&
		(is_buffer_crlf(this->client_buffer) == false ||
		 (is_buffer_crlf(this->client_buffer) == true &&
		  find_str_buffer(this->client_buffer, "\r\n\r") == false))) {
		status = read_chunk_client(this->socket.response_fd, this->client_buffer,
								   (this->socket.len_buf_parts));
		if (status == CHUNK_CLOSE) {
			remove_client(this->socket_list, this->socket.response_fd, 0);
			_recipes_it = _recipes.end();
			_is_aborted = true;
			return ;
		} else if (status == CHUNK_FATAL) {
			this->request.status_code = 500;
			return ;
		}
	}
	if (is_buffer_crlf(this->client_buffer) == false) {
		--_recipes_it;
		return ;
	}
	if (_chunk_size == -1) {     // Parse chunk size
		status = parse_chunk_size(this->client_buffer, _chunk_size,
								  (this->socket.len_buf_parts));
	} else {                           // Read chunk data
		status = parse_chunk_data(this->client_buffer, _chunk_size, *_tmpfile,
								  (this->socket.len_buf_parts));
	}
	if (this->server.client_max_body_size != -1 &&
		(int)_tmpfile->get_size() > this->server.client_max_body_size) {
		this->request.status_code = 413;
	} else if (status == CHUNK_FATAL) {
		this->request.status_code = 500;
	} else if (status == CHUNK_ERROR) {
		this->request.status_code = 400;
	} else if (status == CHUNK_END) {
		_tmpfile->reset_cursor();
	} else {
		--_recipes_it;
	}
}

void	Callback::gen_resp_headers(void) {
	std::list<std::string> headers;

	headers.push_back(get_status_line(this->request.status_code));
	grh_add_headers(headers, *this);
	headers.push_back("\r\n");
	if (_dir_listening_page.size() > 0) {
		headers.insert(headers.end(), _dir_listening_page.begin(),
					   _dir_listening_page.end());
	}
	this->_resp_headers = lststr_to_strcont(headers, "\r\n");
}

void	Callback::send_respons(void) {
	int         ret;
	struct stat stat;

	if ((this->socket.is_write_ready) == false) {
		_recipes_it--;
		return ;
	}
	if ((ret = send(socket.response_fd, _resp_headers.c_str(),
					_resp_headers.length(), MSG_NOSIGNAL)) < 1) {
		std::cerr << "Error: Respons to client" << std::endl;
		remove_client(this->socket_list, this->socket.response_fd, ret);
		_recipes_it = _recipes.end();
		_is_aborted = true;
		return ;
	}
	if (this->request.method == "GET" && this->request.status_code == 200) {
		if (this->socket.similar_req.client_priority == 0)
			this->socket.similar_req.client_priority = this->socket.response_fd;
		this->socket.similar_req.host = this->request.host;
		this->socket.similar_req.path_respons = this->request.path;
		this->socket.similar_req.original_path = this->request.original_path;

		if (lstat(this->request.path.c_str(), &stat) == -1)
			std::cerr << "Error: lstat _send_respons()" << std::endl;
		this->socket.similar_req.last_state_change = stat.st_ctime;
		this->socket.similar_req.ip_port = this->socket.ip_port;
		this->socket.similar_req.respons = _resp_headers;
	}
}

void	Callback::send_respons_body(void) {

	char            buf[BUFFER_SIZE + 1];
	int             bytes_read;
	int             ret;

	if (_resp_body != true) {
		return ;
	}
	if (_fd_body == 0) {                      // Open requested file
		errno = 0;
		_fd_body = open(this->request.path.c_str(), O_RDONLY);
		fcntl(_fd_body, F_SETFL, O_NONBLOCK);
		if (_fd_body == -1) {
			std::cerr <<                                                  \
                "ERR : _SEND_RESP_BODY : open() : " << strerror(errno) << \
            std::endl;
			this->request.status_code = 500;
			--_recipes_it;
			return ;
		}
	}
	if (is_fd_read_ready(_fd_body) == false || // Client and file ready?
		this->socket.is_write_ready == false ) {
		--_recipes_it;
		return ;
	}
	bzero(buf, BUFFER_SIZE + 1);
	bytes_read = read(_fd_body, buf, BUFFER_SIZE);
	if (bytes_read > 0) {
		if ((ret = send(this->socket.response_fd, buf, bytes_read, MSG_NOSIGNAL)) < 1) {
			std::cerr << "_send_respons_body : send() failed" << std::endl;
			remove_client(this->socket_list, this->socket.response_fd, ret);
			_recipes_it = _recipes.end();
			_is_aborted = true;
			return ;
		}
		if (this->request.method == "GET" &&
			this->socket.similar_req.client_priority == this->socket.response_fd) {
			this->socket.similar_req.respons.append(buf);
			this->socket.similar_req.client_priority = 0;
		}
		if (bytes_read > 0 && bytes_read == BUFFER_SIZE)
			--_recipes_it;
	} else if (bytes_read == -1) {
		std::cerr << "_send_respons_body : read() failed" << std::endl;
		this->request.status_code = 500;
		--_recipes_it;
		return ;
	} else if (bytes_read == 0) {
		close(_fd_body);
	}
	return ;
}



// GET METHOD
std::list<t_task_f>     Callback::init_recipe_get(void) {
	std::list<t_task_f>     tasks;

	tasks.push_back(&Callback::meth_get_request_is_valid);
	tasks.push_back(&Callback::gen_resp_headers);
	tasks.push_back(&Callback::send_respons);
	tasks.push_back(&Callback::send_respons_body);
	return tasks;
}

void                Callback::meth_get_request_is_valid(void) {
	struct stat     stat;
	std::string     tmp_path;
	this->request.path.insert(0, this->server.root);

	if (this->request.method == "GET")                      // Response have body?
		_resp_body = true;
	tmp_path = this->request.path;
	errno = 0;
	if (lstat(this->request.path.c_str(), &stat) == -1) {   // Path exist? (DIR|FILE)
		std::cerr <<                                        \
            "ERR: get first lstat : " << strerror(errno) << \
        std::endl;
		this->request.status_code = 404;
		return ;
	}
	if (S_ISDIR(stat.st_mode) == true) {
		this->request.path = find_index_if_exist();
		if (this->request.path == tmp_path && this->server.autoindex == "on") { // Index not found
			_dir_listening_page = gen_listening(this->request.path);
			_resp_body = false;
			content_length_h = lststr_len(_dir_listening_page, "\r\n");
			return ;
		} else if (this->request.path == tmp_path) {
			this->request.status_code = 404;
			return ;
		}
	}
	if (lstat(this->request.path.c_str(), &stat) == -1) {   // Path exist? (FILE)
		this->request.status_code = 404;
		return ;
	} else if ((stat.st_mode & S_IRUSR) == false) { // Do we have rights on it?
		this->request.status_code = 403;
		return ;
	}
	this->content_length_h = stat.st_size;
}

std::string         Callback::find_index_if_exist(void) {
	std::list<std::string>::iterator    it, ite;
	std::string                         tmp_path = this->request.path;
	struct stat                         stat;

	it = this->server.index.begin();
	ite = this->server.index.end();
	if (*(--(tmp_path.end())) != '/')
		tmp_path.insert(tmp_path.end(), '/');
	for (; it != ite ; ++it)
	{
		tmp_path.insert(tmp_path.length(), *it);
		if (lstat(tmp_path.c_str(), &stat) == 0) {
			this->content_length_h = stat.st_size;
			break ;
		}
		tmp_path = this->request.path;
	}
	return (tmp_path);
}



// PUT METHOD
std::list<t_task_f>         Callback::init_recipe_put(void){
	std::list<t_task_f> tasks;

	if (this->request.transfer_encoding == "chunked")
		tasks.push_back(&Callback::chunk_reading);
	tasks.push_back(&Callback::meth_put_open_fd);
	tasks.push_back(&Callback::meth_put_choose_in);
	tasks.push_back(&Callback::meth_put_write_body);
	tasks.push_back(&Callback::gen_resp_headers);
	tasks.push_back(&Callback::send_respons);
	tasks.push_back(&Callback::send_respons_body);
	return (tasks);
}

void	Callback::meth_put_open_fd(void) {
	int         flags;
	mode_t      mode;
	struct stat stat;

	errno = 0;
	bzero(&stat, sizeof(struct stat));
	this->request.path.insert(0, this->server.root);
	if (lstat(this->request.path.c_str(), &stat) == 0 && // File exist and not a dir
		S_ISDIR(stat.st_mode) == false) {
		this->request.status_code = 204;
		flags = PUT_OPEN_FLAGS;
		mode = 0;
	} else if (S_ISDIR(stat.st_mode) == true) {  // Path is dir
		this->request.status_code = 409;
		return ;
	} else {                                     // File does not exist
		this->request.status_code = 201;
		flags = PUT_OPEN_CREAT_FLAGS;
		mode = S_IRWXU;
	}
	if ((_fd_to_write = open(this->request.path.c_str(), flags, mode)) == -1) {
		this->request.status_code = 500;
		return ;
	}
	fcntl(_fd_to_write, F_SETFL, O_NONBLOCK);
}
void	Callback::meth_put_choose_in(void) {
	if (this->request.transfer_encoding == "chunked") {
		_put_fd_in = _tmpfile->get_fd();
	} else {
		_put_fd_in = this->socket.response_fd;
	}
}
void    Callback::meth_put_write_body(void) {
	char buf[BUFFER_SIZE];
	std::string buffer;
	ssize_t ret_read;

	if (this->request.transfer_encoding == "chunked") {     // [IN]  Tmpfile ready?
		if (_tmpfile->is_read_ready() == false) {
			--_recipes_it;
			return ;
		}
	}
	if (is_fd_write_ready(_fd_to_write) == false) { // [OUT] Target ready?
		--_recipes_it;
		return ;
	}
	if (this->client_buffer.empty() == false) {
		buffer = cut_buffer_ret(this->client_buffer,
								(int)this->request.content_length, (this->socket.len_buf_parts));
		_bytes_read = buffer.length();
		if (this->server.client_max_body_size != -1 &&
			_bytes_read > (int)this->server.client_max_body_size) {
			this->request.status_code = 413;
			close(_fd_to_write);
			return ;
		}
		if (_bytes_read != 0) {
			if (write(_fd_to_write, &buffer[0], _bytes_read) <= 0) {
				std::cerr << "_meth_put_write_body : write() failed" << std::endl;
				this->request.status_code = 500;
				return ;
			}
		}
	}
	if (_bytes_read == (int)this->request.content_length) {
		close(_fd_to_write);
		return ;
	}
	if (this->socket.is_read_ready == false) {
		--_recipes_it;
		return ;
	}
	if (this->socket.is_read_ready == true &&
		_bytes_read < (int)this->request.content_length) {
		ret_read = read(_put_fd_in, buf, BUFFER_SIZE);
		if (ret_read == -1 || ret_read == 0) {
			std::cerr << \
                "ERR: put_write_body : read : " << ret_read << std::endl;
			remove_client(this->socket_list, this->socket.response_fd, _bytes_read);
			close(_fd_to_write);
			_recipes_it = _recipes.end();
			_is_aborted = true;
		}
		_bytes_read += ret_read;
		if (this->server.client_max_body_size != -1 &&
			_bytes_read > (int)this->server.client_max_body_size) {
			this->request.status_code = 413;
			close(_fd_to_write);
			return ;
		}
		if (write(_fd_to_write, &buffer[0], _bytes_read) <= 0) {
			std::cerr << "_meth_put_write_body : write() failed" << std::endl;
			this->request.status_code = 500;
			close(_fd_to_write);
			return ;
		}
		if (_bytes_read < (int)this->request.content_length) {
			--_recipes_it;
			return ;
		}
	}
	close(_fd_to_write);
}

// ALLOW
bool    Callback::method_allow() {
	std::list<std::string>::iterator it, ite;
	bool               allow;

	it = this->server.methods.begin();
	ite = this->server.methods.end();
	allow = false;
	for (; it != ite; ++it)
		if (request.method == *it)
			allow = true;
	if (allow == false)
		this->request.status_code = 405;
	return (allow);
}

// CGI METHOD
std::list<t_task_f> Callback::init_recipe_cgi(void) {
	std::list<t_task_f> recipe;

	recipe.push_back(&Callback::meth_cgi_init_meta);
	recipe.push_back(&Callback::meth_cgi_init_http);
	if (this->request.transfer_encoding == "chunked") {
		recipe.push_back(&Callback::chunk_reading);
	} else {
		recipe.push_back(&Callback::meth_cgi_save_client_in);
	}
	recipe.push_back(&Callback::meth_cgi_launch);
	recipe.push_back(&Callback::meth_cgi_wait);
	recipe.push_back(&Callback::meth_cgi_send_http);
	recipe.push_back(&Callback::meth_cgi_send_resp);
	return (recipe);
}

void    Callback::meth_cgi_init_meta(void) {
	char        *c_tmp;
	std::string tmp;
	std::string::iterator it_tmp;

	//AUTH_TYPE
	if (this->request.authorization.size() > 0) {
		tmp = "AUTH_TYPE=" + this->request.authorization.back();
		this->cgi_env_variables.push_back(tmp);
	}
	// CONTENT_LENGTH
	tmp = "CONTENT_LENGTH=";
	c_tmp = &std::to_string(this->request.content_length)[0];
	tmp += c_tmp;
	free(c_tmp);
	this->cgi_env_variables.push_back(tmp);
	// CONTENT_TYPE
	if (this->request.content_type.size() > 0) {
		tmp = "CONTENT_TYPE=";
		for (std::list<std::string>::iterator it = this->request.content_type.begin();
			 it != this->request.content_type.end(); ++it) {
			tmp += *it;
			if (it != this->request.content_type.end())
				tmp += ";";
		}
		this->cgi_env_variables.push_back(tmp);
	}
	// GATEWAY_INTERFACE
	tmp = "GATEWAY_INTERFACE=CGI/1.1";
	this->cgi_env_variables.push_back(tmp);
	// PATH_INFO
	if (this->request.path.size() != 0)
		tmp = "PATH_INFO=" + this->request.path;
	else
		tmp = "PATH_INFO=/";           // To change this go c_callback:205
	this->cgi_env_variables.push_back(tmp);
	// PATH_TRANSLATED
	tmp = "PATH_TRANSLATED=" + this->server.root + this->request.path;
	if (find(this->server.index.begin(), this->server.index.end(), std::string("index.php"))
		!= this->server.index.end() && *(--this->request.path.end()) == '/') {
		tmp += "index.php";
	}
	this->cgi_env_variables.push_back(tmp);
	// QUERY STRING
	it_tmp = this->request.path.end();
	while (it_tmp != this->request.path.begin() && *it_tmp != '?')
		it_tmp--;
	if (*it_tmp == '?') {
		it_tmp++;
		tmp = "QUERY_STRING=";
		tmp += std::string(it_tmp, this->request.path.end());
		this->cgi_env_variables.push_back(tmp);
	}
	// REMOTE_ADDR
	char ip[20];
	bzero(ip, 20);
	tmp = "REMOTE_ADDR=";
	tmp += inet_ntop(AF_INET, &(this->socket.address), ip, 20);
	this->cgi_env_variables.push_back(tmp);
	// REMOTE_USER
	if (this->server.fastcgi_param["REMOTE_USER"].empty()) {
		tmp = "REMOTE_USER=";
		tmp += server.fastcgi_param["REMOTE_USER"];
		this->cgi_env_variables.push_back(tmp);
	}
	// REMOTE_IDENT
	if (this->server.fastcgi_param["REMOTE_IDENT"].empty()) {
		tmp = "REMOTE_IDENT=";
		tmp += server.fastcgi_param["REMOTE_IDENT"];
		this->cgi_env_variables.push_back(tmp);
	}
	// REDIRECT_STATUS                                     // PHP MANDATORY VAR
	tmp = "REDIRECT_STATUS=200";
	this->cgi_env_variables.push_back(tmp);
	// REQUEST_METHOD
	tmp = "REQUEST_METHOD=";
	tmp += this->request.method;
	this->cgi_env_variables.push_back(tmp);
	// REQUEST_URI
	tmp = "REQUEST_URI=";
	tmp += this->request.path;
	this->cgi_env_variables.push_back(tmp);
	// SERVER_NAME
	tmp = "SERVER_NAME=";
	tmp += this->request.host;
	this->cgi_env_variables.push_back(tmp);
	// SERVER_PORT
	tmp = "SERVER_PORT=";
	c_tmp = &std::to_string(this->server.ip_port.port)[0];
	tmp += c_tmp;
	free(c_tmp);
	this->cgi_env_variables.push_back(tmp);
	// SERVER_PROTOCOL
	this->cgi_env_variables.push_back("SERVER_PROTOCOL=HTTP/1.1");
	// SERVER_SOFTWARE
	this->cgi_env_variables.push_back("SERVER_SOFTWARE=drunkserv");
}
void    Callback::meth_cgi_init_http(void) {
	std::string::iterator cursor;

	for (std::list<std::string>::iterator it = this->request.saved_headers.begin();
		 it != this->request.saved_headers.end(); ++it) {
		if ((cursor = find(it->begin(), it->end(), ':')) != it->end()) {
			it->replace(cursor, cursor + 2, "=");
		}
		cursor = it->begin();
		while (cursor != it->end() && *cursor != '=') { // key prepare
			if (*cursor >= 'a' && *cursor <= 'z') // UPPERCASE KEY
				*cursor -= 32;
			if (*cursor == '-') // - to _
				*cursor = '_';
			++cursor;
		}
		*it = "HTTP_" + *it;
	}
	cgi_env_variables.insert(this->cgi_env_variables.begin(),
							 this->request.saved_headers.begin(), this->request.saved_headers.end());
}
void    Callback::meth_cgi_save_client_in(void) {
	int     cat_len;
	char    *read_buf = nullptr;
	char    *cat_buf = nullptr;
	ssize_t bytes_read;

	if (_tmpfile == nullptr)
		_tmpfile = new TmpFile();
	if (_tmpfile->is_write_ready() == false) {
		--_recipes_it;
		return ;
	}
	if (this->client_buffer.size() > 0) {                   // Buffer reading
		cat_buf = &cut_buffer_ret(this->client_buffer, this->request.content_length,
								  (this->socket.len_buf_parts))[0];
		cat_len = strlen(cat_buf);
		if (cat_len > 0 && write(_tmpfile->get_fd(), cat_buf, cat_len) < 1) {
			std::cerr << "ERR: cgi save in : write failed : " << std::endl;
			this->request.status_code = 500;
		} else {
			this->request.content_length -= cat_len;
		}
		free(cat_buf);
	} else if (this->socket.is_read_ready == true) {               // Client reading
		if (!(read_buf = (char*)malloc(sizeof(char) * (BUFFER_SIZE)))) {
			std::cerr << "ERR: cgi_save(): malloc() failed" << std::endl;
			request.status_code = 500;
			return ;
		}
		bzero(read_buf, BUFFER_SIZE);
		if (BUFFER_SIZE < this->request.content_length)
			bytes_read = read(this->socket.response_fd, read_buf, BUFFER_SIZE);
		else
			bytes_read = read(this->socket.response_fd, read_buf, this->request.content_length);
		if (bytes_read == 0 || bytes_read == -1) {
			std::cerr << \
                "ERR: bytes_read : read client error : " << bytes_read << \
            std::endl;
			remove_client(this->socket_list, this->socket.response_fd, bytes_read);
			_recipes_it = _recipes.end();
			_is_aborted = true;
			free(read_buf);
			return ;
		} else {
			this->socket.buffer.push_back(read_buf);
			this->socket.len_buf_parts.push_back(bytes_read);
		}
	}
	if (this->request.content_length > 0) {                         // Read again
		--_recipes_it;
	} else {
		_tmpfile->reset_cursor();
	}
}
void    Callback::meth_cgi_launch(void) {
	char *bin_path = nullptr;
	char **envp = nullptr;
	char **args = nullptr;
	std::list<std::string> lst_args;

	errno = 0;
	if (_out_tmpfile == nullptr) {
		_out_tmpfile = new TmpFile();
	}
	if (_out_tmpfile->is_write_ready() == false) {
		--_recipes_it;
		return;
	}
	if ((_pid = fork()) == 0) { // CHILD
		if ((bin_path = strdup(this->server.fastcgi_pass.c_str())) == nullptr)
			exit(launch_panic(envp, args, bin_path));
		if ((envp = lststr_to_strs(this->cgi_env_variables)) == nullptr)
			exit(launch_panic(envp, args, bin_path));
		lst_args.push_back(bin_path);
		lst_args.push_back(this->server.root + this->request.path);
		if ((args = lststr_to_strs(lst_args)) == nullptr)
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
			if (write(1, "Status: 500\r\n\r\n", 15) != 15) {
				std::cerr << "cgi_launch : execve : write failed" << std::endl;
				exit(launch_panic(envp, args, bin_path));
			} else {
				exit(launch_panic(envp, args, bin_path));
			}
		}
	} else if (_pid == -1) { // ERROR
		std::cerr << \
            "cgi_launch : execve : " << strerror(errno) << \
        std::endl << std::flush;
		this->request.status_code = 500;
	}
}
void    Callback::meth_cgi_wait(void) {
	int   status;
	pid_t dead;

	errno = 0;
	dead = waitpid(_pid, &status, WNOHANG);
	if (dead == -1) {
		std::cerr << "error : waitpid() : " << strerror(errno) << std::endl;
		this->request.status_code = 500;
		return ;
	} else if (dead == _pid) {
		_out_tmpfile->reset_cursor();
	} else if (dead == 0) {
		--_recipes_it;
	}
}
void    Callback::meth_cgi_send_http(void) {
	char *http_content = nullptr;

	if (this->socket.is_write_ready == false ||
		_out_tmpfile->is_read_ready() == false) {
		--_recipes_it;
	}
	http_content = cgitohttp(_out_tmpfile, &(this->request.status_code));
	if (http_content) {
		if (send(this->socket.response_fd, http_content, strlen(http_content),
				 MSG_NOSIGNAL) < 1) {
			std::cerr << "ERR: _meth_cgi_send_http : send" << std::endl;
			remove_client(this->socket_list, this->socket.response_fd, -1);
			_recipes_it = _recipes.end();
			_is_aborted = true;
		}
		free(http_content);
		_out_tmpfile->reset_cursor();
	}
}
void    Callback::meth_cgi_send_resp(void) {
	int     buf_size;
	char    *buf;
	ssize_t bytes_send;

	if (_is_outfile_read == false) { // outfile reading
		if (_out_tmpfile->is_read_ready() == false) {
			--_recipes_it;
			return ;
		} else {
			if (!(buf = (char*)malloc(sizeof(char) * (CGI_SEND_SIZE + 1)))) {
				std::cerr << "ERR: cgi send resp: malloc() fail" << std::endl;
				this->request.status_code = 500;
				return ;
			}
			bzero(buf, CGI_SEND_SIZE + 1);
			if ((buf_size = read(_out_tmpfile->get_fd(), buf, CGI_SEND_SIZE))
				== -1) {
				std::cerr << \
                "ERR: cgi_send : read error : " << buf_size << std::endl;
				this->request.status_code = 500;
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
	if (this->socket.is_write_ready && _sending_buffer.size() > 0) {
		if (_sending_buffer.empty()) { // Nothing to right in client
			--_recipes_it;
			return ;
		}
		if ((bytes_send = send(this->socket.response_fd, _sending_buffer.front(),
							   _len_send_buffer.front(), MSG_NOSIGNAL)) < 1) {
			std::cerr << "ERR: cgi_send : send error" << std::endl;
			remove_client(this->socket_list, this->socket.response_fd, -1);
			_recipes_it = _recipes.end();
			_is_aborted = true;
			return ;
		}
		cut_buffer((this->_sending_buffer), bytes_send,
				   (this->_len_send_buffer));
	}
	if (_sending_buffer.empty() || _is_outfile_read == false) {
		--_recipes_it;
		return ;
	}
}


// RECIPE HEAD METHOD
std::list<t_task_f>     Callback::init_recipe_head(void) {
	std::list<t_task_f> tasks;

	tasks.push_back(&Callback::meth_get_request_is_valid);
	tasks.push_back(&Callback::gen_resp_headers);
	tasks.push_back(&Callback::send_respons);
	return (tasks);
}

// RECIPE POST METHOD
std::list<t_task_f>     Callback::init_recipe_post(void) {
	std::list<t_task_f>     tasks;

	if (this->request.transfer_encoding == "chunked") {
		tasks.push_back(&Callback::chunk_reading);
	} else {
		tasks.push_back(&Callback::read_body_post);
	}
	tasks.push_back(&Callback::gen_resp_headers);
	tasks.push_back(&Callback::send_respons);
	return tasks;
}

// RECIPE BODY METHOD
void	Callback::read_body_post(void) {

	int     buf_size;
	int     ret_read;
	char    buf[BUFFER_SIZE];
	std::string    buffer;

	if (this->client_buffer.empty() == false) {
		buffer = cut_buffer_ret(this->client_buffer,
								(int)this->request.content_length, (this->socket.len_buf_parts));
		_bytes_read = buffer.length();
	}
	if (this->server.client_max_body_size != -1 &&
		_bytes_read > (int)this->server.client_max_body_size) {
		this->request.status_code = 413;
		return ;
	} else if (_bytes_read == (int)this->request.content_length)
		return ;
	if (this->request.content_length > BUFFER_SIZE)
		buf_size = BUFFER_SIZE;
	else
		buf_size = this->request.content_length;
	if (this->socket.is_read_ready == true) {
		if ((ret_read = recv(socket.response_fd, &buf, buf_size, 0)) >= 1) {
			_bytes_read += ret_read;
			if (_bytes_read < (int)this->request.content_length)
				--_recipes_it;
			if (this->server.client_max_body_size != -1 &&
				_bytes_read > (int)this->server.client_max_body_size) {
				this->request.status_code = 413;
				return ;
			}
		}
		if (ret_read == 0 || ret_read == -1) {
			std::cerr << "ERR: read_body_post : recv : " << ret_read << \
                std::endl;
			remove_client(this->socket_list, this->socket.response_fd, 0);
			_recipes_it = _recipes.end();
			_is_aborted = true;
			return ;
		}
	}
}

std::list<t_task_f>     Callback::init_recipe_trace(void) {
	std::list<t_task_f> tasks;

	tasks.push_back(&Callback::read_client_to_tmpfile);
	tasks.push_back(&Callback::gen_resp_headers);
	tasks.push_back(&Callback::send_respons);
	tasks.push_back(&Callback::send_respons_body);
	return (tasks);
}
void	Callback::read_client_to_tmpfile(void){
	char            *buf;
	int             bytes_read;

	if (_bytes_read == (int)this->request.content_length)
		return;
	if ((this->socket.is_read_ready) == false) {
		_recipes_it--;
		return ;
	}
	_tmpfile = new TmpFile();
	std::string tmp;

	if (this->request.path == "")
		this->request.path = "/";
	tmp = this->request.method + " " + this->request.path + " " + this->request.protocol + "\n";
	if (write(_tmpfile->get_fd(), tmp.c_str(), tmp.size()) < 1)
		std::cerr << "Error: write() in _write_request_line()" << std::endl;

	if (this->client_buffer.empty() == false) {
		if (host_exist(client_buffer) == true)
			_host = true;
		buf = concate_list_str(this->client_buffer);
		bytes_read = strlen(buf);
		if (write(_tmpfile->get_fd(), buf, bytes_read) < 1)
			std::cerr << "error: _read_client_to_tmpfile | write()" <<std::endl;
		free(buf);
	}
	if (_host == false) {
		this->request.status_code = 400;
	} else {
		this->request.path = _tmpfile->get_filename();
		this->_resp_body = true;
	}
}
