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
		throw std::logic_error("getline failed");
	if (!line.empty() &&
			   strncmp(&line[0], KEY_STATUS, strlen(KEY_STATUS)) == 0) {
		*status_code = (size_t)atoi(&line[0] + strlen(KEY_STATUS));
		if (get_status_msg(*status_code) != "") {
			status_line = get_status_line(*status_code);
			status_line += "\r\n";
		}
	}
	return (status_line);
}

static size_t         get_headers_len(int fd) {
	size_t  headers_len = 0;
	std::ifstream ifs;
	std::string line;

	if (ifs.is_open())
		std::getline(ifs, line);
	else
		throw std::logic_error("getline failed");
	if (!line.empty()) {
		headers_len += ((line.size() ?: 1) - 1) + strlen("\r\n\r\n");
	}
	return (headers_len);
}

static std::string get_content_len(size_t tmpfile_size, int fd) {
	char        *str_content_len = nullptr;
	std::string http_content;

	str_content_len = &std::to_string(tmpfile_size - (get_headers_len(fd)))[0];
	http_content = "Content-Length: ";
	http_content += str_content_len;
	http_content += "\r\n";
	free(str_content_len);
	return (http_content);
}


char    *cgitohttp(TmpFile *tmpfile, size_t *status_code) {
	std::string http_content;

	http_content += get_status(tmpfile->get_filename(), status_code);
	if (*status_code / 100 != 2)
		return (nullptr);
	if (http_content.empty()) {
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
static bool is_crlf_part_first(std::list<char*> &buffer) {
	char        *i_buf_part;
	std::string crlf = "\r\n";
	std::string str_comp_tmp;
	std::list<char*>::iterator it = buffer.begin();
	std::list<char*>::iterator ite = buffer.end();
	std::list<char*>::iterator it_first = buffer.end();

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
				if (it != buffer.begin())
					return (false);
			}
			++i_buf_part;
		}
		++it;
		if (str_comp_tmp.empty() == true)
			return (false);
	}
	return (str_comp_tmp == crlf && it_first == buffer.begin());
}

/* PARSE_CHUNK_DATA
 * This two functions are the same, they just don't write on the same fd.
 */

int parse_chunk_data(std::list<char*> &buffer, int chunk_size, int pipe_fd,
					 std::list<ssize_t> &len_buf_parts) {
	int          ret_flag = CHUNK_ENOUGH;
	unsigned int          chunk_data_len;
	std::string  chunk_data;
	ssize_t      bytes_write = 1;

	while (!is_crlf_part_first(buffer) && bytes_write > 0) {
		bytes_write = write(pipe_fd, buffer.front(),
							len_buf_parts.front());
		free(buffer.front());
		chunk_size -= len_buf_parts.front();
		buffer.pop_front();
		len_buf_parts.pop_front();
	}
	if (is_crlf_part_first(buffer) == true && chunk_size >= 0 &&
		bytes_write > 0) {
		chunk_data_len = find_str_buffer(buffer, "\r\n");
		if (chunk_data_len > 0) {
			chunk_data = cut_buffer_ret(buffer, chunk_data_len, len_buf_parts);
			bytes_write = write(pipe_fd, &chunk_data[0], chunk_data_len);
			chunk_size -= chunk_data_len;
		}
		cut_buffer(buffer, 2, len_buf_parts);
	}
	if (bytes_write == -1 || bytes_write == 0) {
		std::cerr << \
            "ERR: chunk_data : write failed : " << chunk_size << \
        std::endl;
		ret_flag = CHUNK_FATAL;
	}
	if (chunk_size != 0) {
		std::cerr << \
            "ERR: chunk_data : wrong chunk_len : " << chunk_size << \
        std::endl;
		ret_flag = CHUNK_ERROR;
	}
	chunk_size = -1;
	return (ret_flag);
}

int parse_chunk_data(std::list<char*> &buffer, int &chunk_size,
					 TmpFile &tmpfile, std::list<ssize_t> &len_buf_parts) {
	int          ret_flag = CHUNK_ENOUGH;
	unsigned int          chunk_data_len;
	std::string	chunk_data;
	ssize_t      bytes_write = 1;

	while (is_crlf_part_first(buffer) == false &&
		   tmpfile.is_write_ready() == true && bytes_write > 0) {
		bytes_write = write(tmpfile.get_fd(), buffer.front(),
							len_buf_parts.front());
		free(buffer.front());
		chunk_size -= len_buf_parts.front();
		buffer.pop_front();
		len_buf_parts.pop_front();
	}
	if (is_crlf_part_first(buffer) == true && chunk_size >= 0 &&
		tmpfile.is_write_ready() == true && bytes_write > 0) {
		chunk_data_len = find_str_buffer(buffer, "\r\n");
		if (chunk_data_len > 0) {
			chunk_data = cut_buffer_ret(buffer, chunk_data_len, len_buf_parts);
			bytes_write = write(tmpfile.get_fd(), &chunk_data[0], chunk_data_len);
			chunk_size -= chunk_data_len;
		}
		cut_buffer(buffer, 2, len_buf_parts);
	}
	if (bytes_write == -1 || bytes_write == 0) {
		std::cerr << \
            "ERR: chunk_data : write failed : " << chunk_size << \
        std::endl;
		ret_flag = CHUNK_FATAL;
	}
	if (chunk_size != 0) {
		std::cerr << \
            "ERR: chunk_data : wrong chunk_len : " << chunk_size << \
        std::endl;
		ret_flag = CHUNK_ERROR;
	}
	chunk_size = -1;
	return (ret_flag);
}

/* PARSE_CHUNK_SIZE
 * Will parse the buffer to find a chunk size.
 * If there isn't CRLF, it do nothing.
 */


static int getdecfromchar(char c) {
	char *i_ptr;
	char hex_chars[] = "0123456789ABCDEF";

	i_ptr = hex_chars;
	while (*i_ptr) {
		if (*i_ptr == c)
			return (i_ptr - hex_chars);
		++i_ptr;
	}
	return (0);
}

unsigned int hextodec(char const *hexa) {
	unsigned int n;
	unsigned int result;
	char const *tmp;

	n = 0;
	tmp = hexa;
	result = 0;
	while (*tmp)
		++tmp;
	if (*tmp == '\0')
		--tmp;
	while (tmp >= hexa) {
		result += getdecfromchar(toupper(*tmp)) * pow(16, n);
		--tmp;
		++n;
	}
	return (result);
}

int parse_chunk_size(std::list<char*> &buffer, int &chunk_size,
					 std::list<ssize_t> &len_buf_parts) {
	int          ret_flag = CHUNK_ENOUGH;
	std::string  size_line;
	unsigned int len_to_cut;

	if (**(buffer.begin()) == '0') {
		if (find_str_buffer(buffer, "\r\n\r") == 1)
			return (CHUNK_END);
		else if (find_str_buffer(buffer, "\r\n") == 1)
			return (CHUNK_ENOUGH);
	}
	len_to_cut = find_str_buffer(buffer, "\r\n");
	size_line = cut_buffer_ret(buffer, len_to_cut, len_buf_parts);
	cut_buffer(buffer, 2, len_buf_parts);
	if (is_str_hex(&size_line.c_str()[0]) == false) {              // Size line non hex
		std::cerr << "ERR: parse_chunk_size: size not hex" << std::endl;
		ret_flag = CHUNK_ERROR;
	} else {
		chunk_size = (int)hextodec(&size_line.c_str()[0]);
	}
	return (ret_flag);
}

int read_chunk_client(int client_fd, std::list<char*> &buffer,
					  std::list<ssize_t> &len_buf_parts) {
	int     ret_flag = CHUNK_ENOUGH;
	char    *local_buf = NULL;
	ssize_t bytes_recv;

	if (!(local_buf = (char*)malloc(sizeof(char) * (CHUNK_BUF_SIZE + 1))))
		return (CHUNK_FATAL);
	bzero(local_buf, CHUNK_BUF_SIZE + 1);
	bytes_recv = recv(client_fd, local_buf, CHUNK_BUF_SIZE, 0);
	if (bytes_recv == 0 || bytes_recv == -1) {
		std::cerr << \
            "ERR: read_chunk_client : recv : " << bytes_recv << std::endl;
		ret_flag = CHUNK_CLOSE;
		free(local_buf);
	} else {
		buffer.push_back(local_buf);
		len_buf_parts.push_back(bytes_recv);
	}
	return (ret_flag);
}

bool    is_buffer_crlf(std::list<char*> &buffer) {
	char  str_crlf[] = "\r\n";
	char  *i_buf_parts = NULL;
	short i_crlf = 0;
	std::list<char*>::iterator it_buf = buffer.begin();
	std::list<char*>::iterator ite_buf = buffer.end();

	while (it_buf != ite_buf && str_crlf[i_crlf] != '\0') {
		i_buf_parts = *it_buf;
		while (*i_buf_parts != '\0' && str_crlf[i_crlf] != '\0') {
			if (*i_buf_parts == str_crlf[i_crlf]) {
				++i_crlf;
			} else if (i_crlf != 0) {
				i_crlf = 0;
			}
			++i_buf_parts;
		}
		++it_buf;
	}
	if (str_crlf[i_crlf] == '\0')
		return (true);
	return (false);
}

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




// GET METHOD

std::string         Callback::find_index_if_exist(void) {
	std::list<std::string>::iterator    it, ite;
	std::string                         tmp_path = this->request.path;
	std::string::iterator               it_path;
	struct stat                         stat;

	it = this->server.index.begin();
	ite = this->server.index.end();
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
		tmp_path = this->request.path;
	}
	return (tmp_path);
}















#define TOP_PAGE "<html>\n"                                      \
                 "<head><title>Index of /jpeg/</title></head>\n" \
                 "<body>\n"                                      \
                 "<h1>Index of REQ_PATH</h1>"                    \
                 "<hr><pre><a href=\"../\">../</a>"

static std::string get_top_page(std::string dir_path_request) {
	size_t      cursor;
	std::string top_page = TOP_PAGE;

	cursor = top_page.find("REQ_PATH");
	top_page.replace(cursor, 8, dir_path_request);
	return (top_page);
}

#define MID_TEMPLATE "<a href=\"ELEM_NAME\">ELEM_NAME</a>"

/* GET_MID_PAGE
 * This function create a list of elem that represent all elements in current
 * directory. Each element start by the template MID_TEMPLATE and their name
 * replace "ELEM_NAME" and add a slash if this element is a directory.
 */
static std::list<std::string> get_mid_page(char const *dir_path) {
	DIR                    *curr_dir;
	size_t                 cursor;
	std::string            tmp_elem;
	std::string            tmp_elem_name;
	std::string            tmp_elem_path;
	struct stat            stat_curr_file;
	struct dirent          *it_dir;
	std::list<std::string> mid_page;

	errno = 0;
	if ((curr_dir = opendir(dir_path)) == NULL)
		throw std::logic_error(strerror(errno));
	while ((it_dir = readdir(curr_dir))) {
		tmp_elem = MID_TEMPLATE;
		tmp_elem_name = it_dir->d_name;
		if (tmp_elem_name == "." || tmp_elem_name == "..")
			continue ;
		// Get full path of current file to call lstat
		tmp_elem_path = dir_path;
		tmp_elem_path += "/";
		tmp_elem_path += tmp_elem_name;
		// Call lstat to know if the file is a dir
		if ((lstat(tmp_elem_path.c_str(), &stat_curr_file)) == -1)
			throw std::logic_error(std::string("lstat() ") + strerror(errno));
		if (S_ISDIR(stat_curr_file.st_mode) == true)
			tmp_elem_name += "/";
		// Replace "ELEM_NAME" in mid template by the name of the current file
		while ((cursor = tmp_elem.find("ELEM_NAME")) != std::string::npos)
			tmp_elem.replace(cursor, strlen("ELEM_NAME"), tmp_elem_name);
		mid_page.push_back(tmp_elem);
	}
	closedir(curr_dir);
	return (mid_page);
}

#define BOT_HTML "</pre><hr>"                              \
                 "<center>drunkserv v6.66</center></body>" \
                 "</html>"

std::string ft_basename(std::string const path) {
	std::string::const_iterator it_start = path.end();
	std::string::const_iterator it_end = path.end();

	if (path.size() == 0)
		return (std::string(""));
	--it_start;
	while (*it_start == '/' && it_start != path.begin()) {
		it_end = it_start;
		--it_start;
	}
	while (it_start != path.begin()) {
		if (*it_start == '/') {
			++it_start;
			break ;
		}
		--it_start;
	}
	return (std::string(it_start, it_end));
}
std::list<std::string>  gen_listening(std::string dir_path) {
	std::string             dir_name;
	std::list<std::string>  page;
	std::list<std::string>  mid_page;

	dir_name = ft_basename(dir_path);
	page.push_back(get_top_page(dir_name));
	try {
		mid_page = get_mid_page(dir_path.c_str());
	} catch (std::exception &e) {
		std::cerr << "gen_listening() : " << e.what() << std::endl;
		return (std::list<std::string>());
	}
	page.insert(page.end(), mid_page.begin(), mid_page.end());
	page.push_back(BOT_HTML);
	for (std::list<std::string>::iterator it = page.begin(); it != page.end(); it++)
		std::cout << *it << std::endl;
	return (page);
}










size_t  lststr_len(std::list<std::string> const &lst, std::string const sep) {
	size_t result;

	result = 0;
	for (std::list<std::string>::const_iterator it = lst.begin();
		 it != lst.end(); ++it) {
		result += it->size();
		result += sep.size();
	}
	return (result);
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

std::list<t_task_f>     Callback::init_recipe_get(void) {
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

/* _METH_PUT_CHOOSE_IN
 * This function will set which file descriptor in to take as input to write
 * data in the targeted file. Input file can be chunked request in a tmpfile
 * or a directly the client_fd body.
 */
void    Callback::_meth_put_choose_in(void) {
	if (this->request.transfer_encoding == "chunked") {
		_put_fd_in = _tmpfile->get_fd();
	} else {
		_put_fd_in = this->socket.response_fd;
	}
}

/* _METH_PUT_WRITE_BODY()
 */
void    Callback::meth_put_write_body(void) {
	char buf[4096];
	char *buffer;
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
	if (this->client_buffer->empty() == false) {
		buffer = cut_buffer_ret(this->client_buffer,
								(int)this->request.content_length, &(this->client->len_buf_parts));
		_bytes_read = strlen(buffer);
		if (this->client_max_body_size != -1 &&
			_bytes_read > (int)this->client_max_body_size) {
			this->request.status_code = 413;
			close(_fd_to_write);
			return ;
		}
		if (_bytes_read != 0) {
			if (write(_fd_to_write, buffer, _bytes_read) <= 0) {
				std::cerr << "_meth_put_write_body : write() failed" << std::endl;
				free(buffer);
				this->request.status_code = 500;
				return ;
			}
		}
		free(buffer);
	}
	if (_bytes_read == (int)this->request.content_length) {
		close(_fd_to_write);
		return ;
	}
	if (*this->is_read_ready == false) {
		--_recipes_it;
		return ;
	}
	if (*this->is_read_ready == true &&
		_bytes_read < (int)this->request.content_length) {
		ret_read = read(_put_fd_in, buf, 4096);
		if (ret_read == -1 || ret_read == 0) {
			std::cerr << \
                "ERR: put_write_body : read : " << ret_read << std::endl;
			remove_client(this->socket_list, this->socket.response_fd, _bytes_read);
			close(_fd_to_write);
			_exit();
		}
		_bytes_read += ret_read;
		if (this->server.client_max_body_size != -1 &&
			_bytes_read > (int)this->server.client_max_body_size) {
			this->request.status_code = 413;
			close(_fd_to_write);
			return ;
		}
		if (write(_fd_to_write, buffer, _bytes_read) <= 0) {
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

std::list<t_task_f>         Callback::init_recipe_put(void){
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