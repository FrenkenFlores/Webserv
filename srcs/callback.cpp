#include "Webserv.hpp"


static char **ft_panic(char **start, char **curr) {
	while (curr != start) {
		free(*curr);
		--curr;
	}
	free(start);
	return (NULL);
}

char    **lststr_to_strs(std::list<std::string> lst) {
	char **strs = NULL;
	char **i_strs = NULL;
	std::list<std::string>::iterator it = lst.begin();
	std::list<std::string>::iterator ite = lst.end();

	if (!(strs = (char**)malloc(sizeof(char*) * (lst.size() + 1))))
		return (NULL);
	i_strs = strs;
	while (it != ite) {
		if ((*i_strs = strdup(it->c_str())) == NULL)
			return (ft_panic(strs, i_strs));
		++it;
		++i_strs;
	}
	*i_strs = NULL;
	return (strs);
}

std::string ft_dirname(std::string const path) {
	std::string::const_iterator it_end = path.end();

	if (path.find("/") == std::string::npos)
		return (std::string("."));
	if (path == "/")
		return (std::string("/"));
	--it_end;
	while (*it_end == '/' && it_end != path.begin()) {
		--it_end;
	}
	while (it_end != path.begin()) {
		if (*it_end == '/') {
			break ;
		}
		--it_end;
	}
	return (std::string(path.begin(), it_end));
}

//CGITOHTTP

#define KEY_STATUS "Status: "

static std::string  msg_successful(int code) {
	switch (code) {
		case 100:
			return ("Continue");
		case 101:
			return ("Switching Protols");
		case 200:
			return ("OK");
		case 201:
			return ("Created");
		case 202:
			return ("Accepted");
		case 203:
			return ("Non-Authoritative Information");
		case 204:
			return ("No Content");
		case 205:
			return ("Reset Content");
		case 206:
			return ("Partial Content");
	}
	return ("");
}

static std::string  msg_redirection_error(int code) {
	switch (code) {
		case 301:
			return ("Moved Permanently");
		case 302:
			return ("Found");
		case 303:
			return ("See Other");
		case 304:
			return ("Not Modified");
		case 305:
			return ("Use Proxy");
		case 307:
			return ("Temporary Redirect");
	}
	return ("");
}

static std::string  msg_client_error(int code) {
	switch (code) {
		case 400:
			return ("Bad Request");
		case 401:
			return ("Unauthorized");
		case 402:
			return ("Bad Request");
		case 403:
			return ("Forbidden");
		case 404:
			return ("Not Found");
		case 405:
			return ("Not Allowed");
		case 406:
			return ("Not Acceptable");
		case 407:
			return ("Proxy Authentication Required");
		case 408:
			return ("Request Timeout");
		case 409:
			return ("Conflict");
		case 410:
			return ("Gone");
		case 411:
			return ("Length Required");
		case 412:
			return ("Precondition Failed");
		case 413:
			return ("Payload Too Large");
		case 414:
			return ("URI Too Long");
		case 415:
			return ("Unsupported Media Type");
		case 416:
			return ("Range Not Satisfiable");
		case 417:
			return ("Expectation Failed");
		case 426:
			return ("Upgrade Required");
	}
	return ("");
}

static std::string  msg_server_error(int code) {
	switch (code) {
		case 500:
			return ("Internal Server Error");
		case 501:
			return ("Not Implemented");
		case 502:
			return ("Bad Gateway");
		case 503:
			return ("Service Unavailable");
		case 504:
			return ("Gateway Timeout");
		case 505:
			return ("HTTP Version Not Supported");
	}
	return ("");
}

std::string get_status_msg(int code) {
	if (code / 100 == 1 || code / 100 == 2)
		return (msg_successful(code));
	else if (code / 100 == 3)
		return (msg_redirection_error(code));
	else if (code / 100 == 4)
		return (msg_client_error(code));
	else if (code / 100 == 5)
		return (msg_server_error(code));
	return ("");
}

std::string get_status_line(int code) {
	char *str_code = &(std::to_string(code)[0]);
	std::string code_msg = get_status_msg(code);
	std::string status_line = "HTTP/1.1 ";

	status_line += str_code;
	status_line += " ";
	status_line += code_msg;
	free(str_code);
	return (status_line);
}

std::string get_status(std::string filename, size_t *status_code) {
	std::string status_line;
	std::ifstream ifs;
	std::string line;


	ifs.open(filename);
	if (ifs.is_open())
		std::getline(ifs, line);
	else
		throw std::logic_error("cgitohttp(): [1] : getline failed");
	if (!line.empty() &&
			   strncmp(&line[0], KEY_STATUS, strlen(KEY_STATUS)) == 0) {
		*status_code = (size_t)atoi(line + strlen(KEY_STATUS));
		if (get_status_msg(*status_code) != "") {
			status_line = get_status_line(*status_code);
			status_line += "\r\n";
		}
	}
	return (status_line);
}

static int         get_headers_len(int fd) {
	char    *headers = NULL;
	size_t  headers_len = 0;
	int     status;

	if ((status = get_next(fd, &headers, "\r\n\r\n")) == -1) {
		throw std::logic_error("cgitohttp(): [1] : get_next failed");
	} else if (headers) {
		headers_len += ((status ?: 1) - 1) + ft_strlen("\r\n\r\n");
		free(headers);
	}
	return (headers_len);
}

static std::string get_content_len(size_t tmpfile_size, int fd) {
	char        *str_content_len = NULL;
	std::string http_content = "";

	str_content_len = ft_itoa(tmpfile_size - (get_headers_len(fd)));
	http_content = "Content-Length: ";
	http_content += str_content_len;
	http_content += "\r\n";
	free(str_content_len);
	return (http_content);
}


char    *cgitohttp(TmpFile *tmpfile, size_t *status_code) {
	std::string http_content = "";

	http_content += get_status(tmpfile->get_filename(), status_code);
	if (*status_code / 100 != 2)
		return (NULL);
	if (http_content == "") {
		http_content += "HTTP/1.1 200 OK\r\n";
	}
	tmpfile->reset_cursor();
	http_content += get_content_len(tmpfile->get_size(), tmpfile->get_fd());
	tmpfile->reset_cursor();
	return (strdup(http_content.c_str()));
}


#define CHUNK_CLOSE -3
#define CHUNK_FATAL -2
#define CHUNK_ERROR -1
#define CHUNK_MORE   0
#define CHUNK_ENOUGH 1
#define CHUNK_END    2

#define CHUNK_BUF_SIZE 4096

/* IS_STR_HEX
 * Return false is string in parameter does not contain only hex chars. Return
 * true else.
 */
static bool is_str_hex(char const *str) {
	while (*str) {
		if ((*str >= 'a' && *str <= 'f') || (*str >= 'A' && *str <= 'F') ||
			(*str >= '0' && *str <= '9')) {
			++str;
		}
		else
			return (false);
	}
	return (true);
}

/* IS_CRLF_PART_FIRST()
 * This function return true if the first string in the buffer contain at least
 * one char of CRLF.
 */
static bool is_crlf_part_first(std::list<char*> *buffer) {
	char        *i_buf_part;
	std::string crlf = "\r\n";
	std::string str_comp_tmp;
	std::list<char*>::iterator it = buffer->begin();
	std::list<char*>::iterator ite = buffer->end();
	std::list<char*>::iterator it_first = buffer->end();

	str_comp_tmp.reserve(crlf.length());
	while (it != ite && str_comp_tmp != crlf) {
		i_buf_part = *it;
		while (*i_buf_part && str_comp_tmp != crlf) {
			if (*i_buf_part == '\r' || *i_buf_part == '\n') {
				if (str_comp_tmp.size() == 0 && *i_buf_part == '\r')
					it_first = it;
				str_comp_tmp += *i_buf_part;
			} else if (str_comp_tmp.size() != 0) {
				str_comp_tmp.clear();
				if (it != buffer->begin())
					return (false);
			}
			++i_buf_part;
		}
		++it;
		if (str_comp_tmp.empty() == true)
			return (false);
	}
	return (str_comp_tmp == crlf && it_first == buffer->begin());
}

/* PARSE_CHUNK_DATA
 * This two functions are the same, they just don't write on the same fd.
 */
int parse_chunk_data(std::list<char*> *buffer, int *chunk_size, int pipe_fd,
					 std::list<ssize_t> *len_buf_parts) {
	int          ret_flag = CHUNK_ENOUGH;
	int          chunk_data_len;
	char         *chunk_data = NULL;
	ssize_t      bytes_write = 1;

	while (is_crlf_part_first(buffer) == false && bytes_write > 0) {
		bytes_write = write(pipe_fd, buffer->front(),
							len_buf_parts->front());
		free(buffer->front());
		*chunk_size -= len_buf_parts->front();
		buffer->pop_front();
		len_buf_parts->pop_front();
	}
	if (is_crlf_part_first(buffer) == true && *chunk_size >= 0 &&
		bytes_write > 0) {
		chunk_data_len = find_str_buffer(buffer, "\r\n");
		if (chunk_data_len > 0) {
			chunk_data = cut_buffer_ret(buffer, chunk_data_len, len_buf_parts);
			bytes_write = write(pipe_fd, chunk_data, chunk_data_len);
			free(chunk_data);
			*chunk_size -= chunk_data_len;
		}
		cut_buffer(buffer, 2, len_buf_parts);
	}
	if (bytes_write == -1 || bytes_write == 0) {
		std::cerr << \
            "ERR: chunk_data : write failed : " << *chunk_size << \
        std::endl;
		ret_flag = CHUNK_FATAL;
	}
	if (*chunk_size != 0) {
		std::cerr << \
            "ERR: chunk_data : wrong chunk_len : " << *chunk_size << \
        std::endl;
		ret_flag = CHUNK_ERROR;
	}
	*chunk_size = -1;
	return (ret_flag);
}

int parse_chunk_data(std::list<char*> *buffer, int *chunk_size,
					 c_tmpfile *tmpfile, std::list<ssize_t> *len_buf_parts) {
	int          ret_flag = CHUNK_ENOUGH;
	int          chunk_data_len;
	char         *chunk_data = NULL;
	ssize_t      bytes_write = 1;

	while (is_crlf_part_first(buffer) == false &&
		   tmpfile->is_write_ready() == true && bytes_write > 0) {
		bytes_write = write(tmpfile->get_fd(), buffer->front(),
							len_buf_parts->front());
		free(buffer->front());
		*chunk_size -= len_buf_parts->front();
		buffer->pop_front();
		len_buf_parts->pop_front();
	}
	if (is_crlf_part_first(buffer) == true && *chunk_size >= 0 &&
		tmpfile->is_write_ready() == true && bytes_write > 0) {
		chunk_data_len = find_str_buffer(buffer, "\r\n");
		if (chunk_data_len > 0) {
			chunk_data = cut_buffer_ret(buffer, chunk_data_len, len_buf_parts);
			bytes_write = write(tmpfile->get_fd(), chunk_data, chunk_data_len);
			free(chunk_data);
			*chunk_size -= chunk_data_len;
		}
		cut_buffer(buffer, 2, len_buf_parts);
	}
	if (bytes_write == -1 || bytes_write == 0) {
		std::cerr << \
            "ERR: chunk_data : write failed : " << *chunk_size << \
        std::endl;
		ret_flag = CHUNK_FATAL;
	}
	if (*chunk_size != 0) {
		std::cerr << \
            "ERR: chunk_data : wrong chunk_len : " << *chunk_size << \
        std::endl;
		ret_flag = CHUNK_ERROR;
	}
	*chunk_size = -1;
	return (ret_flag);
}

/* PARSE_CHUNK_SIZE
 * Will parse the buffer to find a chunk size.
 * If there isn't CRLF, it do nothing.
 */
int parse_chunk_size(std::list<char*> *buffer, int *chunk_size,
					 std::list<ssize_t> *len_buf_parts) {
	int          ret_flag = CHUNK_ENOUGH;
	char         *size_line = NULL;
	unsigned int len_to_cut;

	if (**(buffer->begin()) == '0') {
		if (find_str_buffer(buffer, "\r\n\r") == 1)
			return (CHUNK_END);
		else if (find_str_buffer(buffer, "\r\n") == 1)
			return (CHUNK_ENOUGH);
	}
	len_to_cut = find_str_buffer(buffer, "\r\n");
	size_line = cut_buffer_ret(buffer, len_to_cut, len_buf_parts);
	cut_buffer(buffer, 2, len_buf_parts);
	if (is_str_hex(size_line) == false) {              // Size line non hex
		std::cerr << "ERR: parse_chunk_size: size not hex" << std::endl;
		ret_flag = CHUNK_ERROR;
	} else {
		*chunk_size = (int)hextodec(size_line);
	}
	free(size_line);
	return (ret_flag);
}

/* READ_CHUNK_CLIENT
 * This function will read client and put the content in the buffer gived.
 */
int read_chunk_client(int client_fd, std::list<char*> *buffer,
					  std::list<ssize_t> *len_buf_parts) {
	int     ret_flag = CHUNK_ENOUGH;
	char    *local_buf = NULL;
	ssize_t bytes_recv;

	if (!(local_buf = (char*)malloc(sizeof(char) * (CHUNK_BUF_SIZE + 1))))
		return (CHUNK_FATAL);
	ft_bzero(local_buf, CHUNK_BUF_SIZE + 1);
	bytes_recv = recv(client_fd, local_buf, CHUNK_BUF_SIZE, 0);
	if (bytes_recv == 0 || bytes_recv == -1) {
		std::cerr << \
            "ERR: read_chunk_client : recv : " << bytes_recv << std::endl;
		ret_flag = CHUNK_CLOSE;
		free(local_buf);
	} else {
		buffer->push_back(local_buf);
		len_buf_parts->push_back(bytes_recv);
	}
	return (ret_flag);
}

/* CHUNK_READING
 * This function will parse the client buffer and read the client if there is
 * no content to parse.
 */
void    Callback::chunk_reading(void) {
	if (g_verbose == true)
		std::cout << "TASK : _chunk_reading()" << std::endl;
	int status = CHUNK_ENOUGH;

	if (_tmpfile == NULL)
		_tmpfile = new c_tmpfile();
	if (((is_buffer_crlf(this->client_buffer) == false &&
		  *(this->is_read_ready) == false)) ||
		_tmpfile->is_write_ready() == false) {
		--_it_recipes;
		return ;
	}
	if (*this->is_read_ready == true &&
		(is_buffer_crlf(this->client_buffer) == false ||
		 (is_buffer_crlf(this->client_buffer) == true &&
		  find_str_buffer(this->client_buffer, "\r\n\r") == false))) {
		status = read_chunk_client(this->client_fd, this->client_buffer,
								   &(this->client->len_buf_parts));
		if (status == CHUNK_CLOSE) {
			remove_client(this->clients, this->client_fd, 0);
			_exit();
			return ;
		} else if (status == CHUNK_FATAL) {
			this->status_code = 500;
			return ;
		}
	}
	if (is_buffer_crlf(this->client_buffer) == false) {
		--_it_recipes;
		return ;
	}
	if (_chunk_size == -1) {     // Parse chunk size
		status = parse_chunk_size(this->client_buffer, &_chunk_size,
								  &(this->client->len_buf_parts));
	} else {                           // Read chunk data
		status = parse_chunk_data(this->client_buffer, &_chunk_size, _tmpfile,
								  &(this->client->len_buf_parts));
	}
	if (this->client_max_body_size != -1 &&
		(int)_tmpfile->get_size() > this->client_max_body_size) {
		this->status_code = 413;
	} else if (status == CHUNK_FATAL) {
		this->status_code = 500;
	} else if (status == CHUNK_ERROR) {
		this->status_code = 400;
	} else if (status == CHUNK_END) {
		_tmpfile->reset_cursor();
	} else {
		--_it_recipes;
	}
	return ;
}




// GET METHOD

std::string         Callback::find_index_if_exist(void) {
	std::list<std::string>::iterator    it, ite;
	std::string                         tmp_path = this->path;
	std::string::iterator               it_path;
	struct stat                         stat;

	it = this->index.begin();
	ite = this->index.end();
	it_path = tmp_path.end();
	if (*(--(tmp_path.end())) != '/')
		tmp_path.insert(tmp_path.end(), '/');
	for (; it != ite ; ++it)
	{
		tmp_path.insert(tmp_path.length(), *it);
		if (lstat(tmp_path.c_str(), &stat) == 0) {
			this->content_length_h = stat.st_size;
			break ;
		}
		tmp_path = this->path;
	}
	return (tmp_path);
}

void                Callback::meth_get_request_is_valid(void) {
	if (g_verbose)
		std::cout << "TASK : _meth_get_request_is_valid" << std::endl;
	struct stat     stat;
	std::string     tmp_path;
	this->path.insert(0, this->root);

	if (this->method == "GET")                      // Response have body?
		_resp_body = true;
	tmp_path = this->path;
	errno = 0;
	if (lstat(this->path.c_str(), &stat) == -1) {   // Path exist? (DIR|FILE)
		std::cerr <<                                        \
            "ERR: get first lstat : " << strerror(errno) << \
        std::endl;
		this->status_code = 404;
		return ;
	}
	if (S_ISDIR(stat.st_mode) == true) {
		this->path = _find_index_if_exist();
		if (this->path == tmp_path && this->autoindex == "on") { // Index not found
			_dir_listening_page = gen_listening(this->path);
			_resp_body = false;
			content_length_h = lststr_len(_dir_listening_page, "\r\n");
			return ;
		} else if (this->path == tmp_path) {
			this->status_code = 404;
			return ;
		}
	}
	if (lstat(this->path.c_str(), &stat) == -1) {   // Path exist? (FILE)
		this->status_code = 404;
		return ;
	} else if ((stat.st_mode & S_IRUSR) == false) { // Do we have rights on it?
		if (g_verbose) {
			std::cout << "Error: no reading rights" << std::endl;
		}
		this->status_code = 403;
		return ;
	}
	this->content_length_h = stat.st_size;
}

std::list<Callback::t_task_f>     Callback::init_recipe_get(void) {
	std::list<t_task_f>     tasks;

	tasks.push_back(&Callback::_meth_get_request_is_valid);
	tasks.push_back(&Callback::_gen_resp_headers);
	tasks.push_back(&Callback::_send_respons);
	tasks.push_back(&Callback::_send_respons_body);
	return tasks;
}



// PUT METHOD

#define PUT_OPEN_FLAGS (O_WRONLY | O_TRUNC)
#define PUT_OPEN_CREAT_FLAGS (O_WRONLY | O_TRUNC | O_CREAT)

void        Callback::meth_put_open_fd(void) {
	int         flags;
	mode_t      mode;
	struct stat stat;

	errno = 0;
	bzero(&stat, sizeof(struct stat));
	this->path.insert(0, this->root);
	if (lstat(this->path.c_str(), &stat) == 0 && // File exist and not a dir
		S_ISDIR(stat.st_mode) == false) {
		this->status_code = 204;
		flags = PUT_OPEN_FLAGS;
		mode = 0;
	} else if (S_ISDIR(stat.st_mode) == true) {  // Path is dir
		this->status_code = 409;
		return ;
	} else {                                     // File does not exist
		this->status_code = 201;
		flags = PUT_OPEN_CREAT_FLAGS;
		mode = S_IRWXU;
	}
	if ((_fd_to_write = open(this->path.c_str(), flags, mode)) == -1) {
		if (g_verbose)
			std::cerr << \
            "open() [" << this->path.c_str() << "] : " << strerror(errno) << \
            std::endl;
		this->status_code = 500;
		return ;
	}
	fcntl(_fd_to_write, F_SETFL, O_NONBLOCK);
}

/* _METH_PUT_CHOOSE_IN
 * This function will set which file descriptor in to take as input to write
 * data in the targeted file. Input file can be chunked request in a tmpfile
 * or a directly the client_fd body.
 */
void    Callback::_meth_put_choose_in(void) {
	if (g_verbose)
		std::cout << "TASK : _meth_put_choose_in()" << std::endl;

	if (this->transfer_encoding == "chunked") {
		_put_fd_in = _tmpfile->get_fd();
	} else {
		_put_fd_in = this->client_fd;
	}
}

/* _METH_PUT_WRITE_BODY()
 */
void    Callback::meth_put_write_body(void) {
	if (g_verbose)
		std::cout << "TASK : _meth_put_write_body()" << std::endl;
	char buf[4096];
	char *buffer;
	ssize_t ret_read;

	if (this->transfer_encoding == "chunked") {     // [IN]  Tmpfile ready?
		if (_tmpfile->is_read_ready() == false) {
			--_it_recipes;
			return ;
		}
	}
	if (is_fd_write_ready(_fd_to_write) == false) { // [OUT] Target ready?
		--_it_recipes;
		return ;
	}
	if (this->client_buffer->empty() == false) {
		buffer = cut_buffer_ret(this->client_buffer,
								(int)this->content_length, &(this->client->len_buf_parts));
		_bytes_read = ft_strlen(buffer);
		if (this->client_max_body_size != -1 &&
			_bytes_read > (int)this->client_max_body_size) {
			this->status_code = 413;
			close(_fd_to_write);
			return ;
		}
		if (_bytes_read != 0) {
			if (write(_fd_to_write, buffer, _bytes_read) <= 0) {
				std::cerr << "_meth_put_write_body : write() failed" << std::endl;
				free(buffer);
				this->status_code = 500;
				return ;
			}
		}
		free(buffer);
	}
	if (_bytes_read == (int)this->content_length) {
		close(_fd_to_write);
		return ;
	}
	if (*this->is_read_ready == false) {
		--_it_recipes;
		return ;
	}
	if (*this->is_read_ready == true &&
		_bytes_read < (int)this->content_length) {
		ret_read = read(_put_fd_in, buf, 4096);
		if (ret_read == -1 || ret_read == 0) {
			std::cerr << \
                "ERR: put_write_body : read : " << ret_read << std::endl;
			remove_client(this->clients, this->client_fd, _bytes_read);
			close(_fd_to_write);
			_exit();
		}
		_bytes_read += ret_read;
		if (this->client_max_body_size != -1 &&
			_bytes_read > (int)this->client_max_body_size) {
			this->status_code = 413;
			close(_fd_to_write);
			return ;
		}
		if (write(_fd_to_write, buffer, _bytes_read) <= 0) {
			std::cerr << "_meth_put_write_body : write() failed" << std::endl;
			this->status_code = 500;
			close(_fd_to_write);
			return ;
		}
		if (_bytes_read < (int)this->content_length) {
			--_it_recipes;
			return ;
		}
	}
	close(_fd_to_write);
}

std::list<Callback::t_task_f>         Callback::_init_recipe_put(void){
	std::list<t_task_f> tasks;

	if (this->transfer_encoding == "chunked")
		tasks.push_back(&Callback::_chunk_reading);
	tasks.push_back(&Callback::_meth_put_open_fd);
	tasks.push_back(&Callback::_meth_put_choose_in);
	tasks.push_back(&Callback::_meth_put_write_body);
	tasks.push_back(&Callback::_gen_resp_headers);
	tasks.push_back(&Callback::_send_respons);
	tasks.push_back(&Callback::_send_respons_body);
	return (tasks);
}