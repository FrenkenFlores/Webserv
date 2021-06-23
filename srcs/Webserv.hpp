#ifndef WEBSERV_HPP
#define WEBSERV_HPP









# define COLOR_RESET    "\033[0m"
# define COLOR_RED      "\033[1;31m"
# define COLOR_BLUE     "\033[1;34m"
# define COLOR_CYAN     "\033[1;36m"
# define COLOR_GREEN    "\033[1;32m"
# define COLOR_WHITE    "\033[1;37m"
# define COLOR_YELLOW   "\033[33m"

# define COLOR_RED_(str) COLOR_RED << str << COLOR_RESET
# define COLOR_BLUE_(str) COLOR_BLUE << str << COLOR_RESET
# define COLOR_CYAN_(str) COLOR_CYAN << str << COLOR_RESET
# define COLOR_GREEN_(str) COLOR_GREEN << str << COLOR_RESET
# define COLOR_WHITE_(str) COLOR_WHITE << str << COLOR_RESET
# define COLOR_YELLOW_(str) COLOR_YELLOW << str << COLOR_RESET
# define COLOR_BOOL_(str) (str ? COLOR_GREEN : COLOR_RED) << str << COLOR_RESET
































#include <sys/time.h>
#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <queue>
#include <map>
#include <list>
#include <stack>
#include <cstdio>
#include <unistd.h>
#include <cstdlib>
#include <cstring>
#include <cerrno>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/wait.h>
#include <arpa/inet.h>
#include <csignal>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include <cmath>
#include <sys/stat.h>
#include <dirent.h>
#include <sstream>

#define DEFAULT_CONFIG_PATH "../configs/default.conf"

class Socket;
class TmpFile;
struct IdenticalGetRequest;
struct Header;

typedef void (*request_header)(std::string line, void * p);
bool read_headers(std::list<Socket> &socket_list);
void remove_client(std::list<Socket> &socket_list, int response_fd,
				   ssize_t bytes_read);
void    similar_get_req_sender(std::list<Socket> &socket_list,
							   IdenticalGetRequest &similar_req);
void    similar_get_req_checker(std::list<Socket> &socket_list,
								IdenticalGetRequest &similar_req);
void	init_header_parsers(std::map<std::string, request_header> &str_parsers_map);
void    parse_field_list_string(std::string line, void *p);
void    parse_field_std_string(std::string line, void *p);
void    parse_field_size_t(std::string line, void *p);
void    parse_field_date(std::string line, void *p);
std::string cut_buffer_ret(std::list<char*> &buffer, unsigned int len,
						   std::list<ssize_t> &len_buf_parts);
void    cut_buffer(std::list<char*> &buffer, unsigned int len,
				   std::list<ssize_t> &len_buf_parts);
char    **lststr_to_strs(std::list<std::string> lst);
std::string ft_dirname(std::string const path);
char    *cgitohttp(TmpFile *tmpfile, size_t *status_code);
unsigned int find_str_buffer(std::list<char*> &buffer, std::string to_find);


void    reset_header(Header &h);
void    reset_socket(Socket &s);





extern bool g_run;
extern int g_worker_connections;

typedef void (*parser_function)(std::string::const_iterator cit, void*);

class	Callback;
typedef void (Callback::*t_task_f)(void);

struct Address {
	std::string ip;
	int port;
};

//bool    operator==(Address const &a, Address const &b) {
//	return (a.ip == b.ip && a.port == b.port);
//}
//
//bool    operator!=(Address const &a, Address const &b) {
//	return (!(a == b));
//}

struct Location {
	int										client_max_body_size;
	std::list<std::string>					index;
	std::list<std::string>					methods;
	std::string								root;
	std::string								route;
	std::string								autoindex;
	std::string								fastcgi_pass;
	std::map<std::string, std::string>		fastcgi_param;
	std::map<std::string, int>				error_page;
	Location () {
		client_max_body_size = 0;
	}
};


struct Server {
	int									server_id;
	int									client_max_body_size;
	std::list<std::string>				index;			// list of indexes
	std::list<std::string>				server_name;	// list of server names
	Address								ip_port;
	std::string							root;
	std::string							autoindex;
	std::string							fastcgi_pass;
	std::map<std::string, std::string>	fastcgi_param;
	std::map<std::string, int>			error_page;
	std::list<std::string>				methods;
	std::list<Location>					location;
	Server () {
		server_id = 0;
		client_max_body_size = 0;
		ip_port.port = -1;
	}
	Server (Server &server) {
		this->root = server.root;
		this->index = server.index;
		this->server_id = server.server_id;
		this->ip_port = server.ip_port;
		this->location = server.location;
		this->autoindex = server.autoindex;
		this->error_page = server.error_page;
		this->server_name = server.server_name;
		this->fastcgi_param = server.fastcgi_param;
		this->client_max_body_size = server.client_max_body_size;
	}
};

struct Header {
	bool					is_transfer_mode_saw;
	size_t					status_code; // var that will be status-code
	size_t					content_length;
	// Status Line data
	std::string				method;
	std::string				path;
	std::string				protocol;
	// Headers data
	std::string				date;
	std::string				host;
	std::string				referer;
	std::string				transfer_encoding;
	std::list<std::string>	accept_charset;
	std::list<std::string>	accept_language;
	std::list<std::string>	authorization;
	std::list<std::string>	content_type;
	std::list<std::string>	user_agent;
	std::list<std::string>	saved_headers;
	std::string				original_path;
//	size_t					status_code;
	// Response variables
	size_t      content_length_h;
	std::string location_h;
	std::string last_modified_h;


	Header() {
		is_transfer_mode_saw = false;
		status_code = 200;
		content_length = 0;
	}
	Header (Header &request) {
		this->date = request.date;
		this->host = request.host;
		this->path = request.path;
		this->method = request.method;
		this->referer = request.referer;
		this->status_code = request.status_code;
		this->protocol = request.protocol;
		this->user_agent = request.user_agent;
		this->content_type = request.content_type;
		this->authorization = request.authorization;
		this->saved_headers = request.saved_headers;
		this->accept_charset = request.accept_charset;
		this->content_length = request.content_length;
		this->accept_language = request.accept_language;
		if (strcmp(request.transfer_encoding.c_str(), "chunked") != 0 &&
			request.transfer_encoding.empty() == false)
			this->status_code = 400;
		else
			this->transfer_encoding = request.transfer_encoding;
	}
};

struct IdenticalGetRequest {
	int             client_priority;
	std::string     host;
	std::string     respons;
	std::string     path_respons;
	std::string     original_path;
	Address			ip_port;
	time_t          last_state_change;
	IdenticalGetRequest () {
		client_priority = 0;
		ip_port.port = -1;
	}
};


struct Socket {
	int					listen_fd;
	int					response_fd;
	bool				is_cache_resp;
	bool				is_header_read;
	bool				is_read_ready;
	bool				is_write_ready;
	bool				is_status_line_read;
	bool				is_callback_created;
	Address				ip_port;
	Server				*server;
	sockaddr_in			address;
	Header				headers;
	std::list<char*>	buffer;
	IdenticalGetRequest	similar_req;
	std::list<ssize_t>	len_buf_parts;
	Socket () {
		listen_fd = 0;
		response_fd = 0;
		is_read_ready = false;
		is_write_ready = false;
		is_cache_resp = false;
		is_header_read = false;
		is_status_line_read = false;
		is_callback_created = false;
		len_buf_parts.clear();
		buffer.clear();
		server = nullptr;
	}
	Socket (Socket &_socket) {
		*this = _socket;
		this->response_fd = _socket.response_fd;
		this->address = _socket.address;
		this->server = _socket.server;
		this->listen_fd = _socket.listen_fd;
		this->is_read_ready = _socket.is_read_ready;
		this->is_write_ready = _socket.is_write_ready;
		this->is_header_read = _socket.is_header_read;
	}
};

class TmpFile {
protected:
	static const char *const    _path;
	static std::string          _nextnameprefix;
	static void                 _update_nextnameprefix(void);
	static std::string          _get_next_name(void);
	static bool                 _does_nextfile_exist(void);
	std::string                 _filename;
	int                         _fd;

public:
	TmpFile(void);
	TmpFile(TmpFile const &src);
	TmpFile	&operator=(TmpFile const &rhs);
	virtual ~TmpFile(void);

	int const           &get_fd(void) const;
	std::string const   &get_filename(void) const;
	bool                is_read_ready(void) const;
	bool                is_write_ready(void) const;
	size_t              get_size(void) const;
	void                reset_cursor(void);
};


class	Callback {
public:
	Socket socket;
	Server server;
	Header request;

	int	_fd_body;
	int	_put_fd_in;
	int	_fd_to_write;

	bool	_host;
	TmpFile	*_tmpfile;
	TmpFile	*_out_tmpfile;
	std::list<Socket>	socket_list;
	std::list<char*>	client_buffer;
	std::list<t_task_f> _recipes;
	std::list<t_task_f>::iterator _recipes_it;
	std::list<std::string> cgi_env_variables;
	std::map<std::string, std::list<t_task_f> > _meth_funs;
	size_t 	_bytes_read;
	int		_bytes_write;
	int		_chunk_size;
	bool	_is_aborted;
	bool	_is_outfile_read;
	bool    _resp_body;
	size_t	content_length_h;
	std::string location_h;
	std::string last_modified_h;
	std::string _resp_headers;
	pid_t                  _pid; // pid of CGI child
	std::list<char*>       _sending_buffer;
	std::list<ssize_t>     _len_send_buffer;
	std::list<std::string> _dir_listening_page;



	Callback() { }
	Callback(Socket socket) { }
	Callback (Socket &_socket, Header &request, std::list<Socket> &_sockets_list) {
		this->_fd_body = 0;
		this->_host = false;
		this->_tmpfile = nullptr;
		this->_bytes_read = 0;
		this->_chunk_size = -1;
		this->_bytes_write = 0;
		this->socket_list = _sockets_list;
		this->_is_aborted = false;
		this->_out_tmpfile = nullptr;
		this->_is_outfile_read = false;
		this->request.original_path = request.path;
		init_socket(socket);                 // Init client socket variables
		init_request_header(request);          // Init request headers
		if (this->socket.server) {
			this->server = Server(*this->socket.server);
			server_init_route(this->server.location);
		}
		if (this->server.client_max_body_size != -1 &&
			this->request.content_length > (size_t)this->server.client_max_body_size) {
			this->request.status_code = 413;
		}
		if (this->server.fastcgi_pass.empty() && method_allow()) {  // CGI case
			_recipes = init_recipe_cgi();
		} else {                                // Init recipes
			init_meth_functions();
			_recipes = meth_funs[this->request.method];
		}
		if (_recipes.empty() == true) {         // Case when methods is not known
			_recipes = init_error_request();
		}
		_recipes_it = _recipes.begin();
		return ;
	}
	void exec() { }
	bool is_over() { }
	void    init_meth_functions(void) {
		if (this->request.status_code / 100 != 2)
			return ;
		if (this->request.host.empty()) {
			this->request.status_code = 400;
			return ;
		}
		if (method_allow() == false)
			return ;
		_meth_funs["PUT"] = init_recipe_put();
		_meth_funs["GET"] = init_recipe_get();
		_meth_funs["HEAD"] = init_recipe_head();
		_meth_funs["POST"] = init_recipe_post();
		_meth_funs["TRACE"] = init_recipe_trace();
		_meth_funs["DELETE"] = init_recipe_delete();
		_meth_funs["OPTIONS"] = init_recipe_options();
	}

	std::list<t_task_f> init_recipe_cgi() {
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
	void init_socket(Socket &_socket) {
		this->socket = Socket(_socket);
		this->content_length_h = 0;
		this->client_buffer = _socket.buffer;
	}
	void init_request_header(Header &_request) {
		this->_resp_body = false;
		request = Header(_request);
	}

	std::list<Location>::iterator
	server_find_route
	(std::list<Location>::iterator &it,
	std::list<Location>::iterator &ite) {
		std::list<Location>::iterator     it_find;
		std::string                         tmp_path;
		size_t                              i = 0;
		int                                 status = 0;

		it_find = ite;
		if ((i = this->request.path.find_first_of('/', 1)) != std::string::npos)
			tmp_path.insert(0, this->request.path, 0, i);
		else
			tmp_path = this->request.path;
		for (; it != ite; ++it)
		{
			if (strncmp((*it).route.c_str(), ".", 1) == 0) { // location management by file
				std::string tmp_string;
				size_t found = this->request.path.find_last_of('.');
				if (found != std::string::npos) {
					tmp_string.insert(0, this->request.path, found, this->request.path.length());

					if ((strncmp(tmp_string.c_str(), (*it).route.c_str(),
									tmp_string.length())) == 0 &&
						(tmp_string.length() == (*it).route.length())) {
						it_find = it;
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
			this->request.path.erase(0, i);
		if (status == 1 && i == std::string::npos)
			this->request.path.clear();
		return (it_find);
	}

	void        server_init_route(std::list<Location> location) {
		std::list<Location>::iterator     it, ite;

		it = location.begin();
		ite = location.end();
		it = server_find_route(it, ite);
		if (it != ite) {
			server.client_max_body_size = (*it).client_max_body_size;
			if ((*it).index.begin() != (*it).index.end())
				server.index = (*it).index;
			if ((*it).methods.empty() == false)
				server.methods = (*it).methods;
			if ((*it).root.empty() == false)
				server.root = (*it).root;
			if ((*it).autoindex.empty() == false)
				server.autoindex = (*it).autoindex;
			if ((*it).fastcgi_param.empty() == false)
				server.fastcgi_param = (*it).fastcgi_param;
			if ((*it).error_page.empty() == false)
				server.error_page = (*it).error_page;
			if ((*it).fastcgi_pass.empty() == false)
				server.fastcgi_pass = (*it).fastcgi_pass;
		}
	}
	bool    method_allow() {
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
	void    meth_cgi_init_meta() {
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

// Init additionnal var of from http request in cgi_env_variables.
	void    meth_cgi_init_http(void) {
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

#define CGI_BUF_SIZE 4096

/* _METH_CGI_SAVE_CLIENT_IN
 * This function will save the entire client body in a tmpfile to be gived as
 * input for CGI execution. File cursor must be reset at the
 * end of body reading.
 * End of body is gived by content-length cause this task isn't called if the
 * request is chunked.
 */
	void    meth_cgi_save_client_in(void) {
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
			if (!(read_buf = (char*)malloc(sizeof(char) * (CGI_BUF_SIZE)))) {
				std::cerr << "ERR: cgi_save(): malloc() failed" << std::endl;
				request.status_code = 500;
				return ;
			}
			bzero(read_buf, CGI_BUF_SIZE);
			if (CGI_BUF_SIZE < this->request.content_length)
				bytes_read = read(this->socket.response_fd, read_buf, CGI_BUF_SIZE);
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

	static int launch_panic(char **envp, char **args, char *bin_path) {
		if (bin_path != nullptr)
			free(bin_path);
		if (envp != nullptr) {
			char **temp = envp;

			if (envp == nullptr) {
				return 1;
			}
			while (*temp) {
				free(*temp);
				temp++;
			}
			free(envp);
		}
		if (args != nullptr){
			char **temp = args;

			if (args == nullptr) {
				return 1;
			}
			while (*temp) {
				free(*temp);
				temp++;
			}
			free(envp);
		}
		return (1);
	}

	void    meth_cgi_launch() {
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

	void    meth_cgi_wait() {
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

	void    meth_cgi_send_http(void) {
		char *http_content = nullptr;

		if (this->socket.is_write_ready == false ||
			_out_tmpfile->is_read_ready() == false) {
			--_recipes_it;
		}
		http_content = cgitohttp(_out_tmpfile, &(this->request.status_code));
		if (http_content) {
			if (send(this->socket.response_fd, http_content, strlen(http_content),
					 0) < 1) {
				std::cerr << "ERR: _meth_cgi_send_http : send" << std::endl;
				remove_client(this->socket_list, this->socket.response_fd, -1);
				_recipes_it = _recipes.end();
				_is_aborted = true;
			}
			free(http_content);
			_out_tmpfile->reset_cursor();
		}
	}

#define CGI_SEND_SIZE 50000

	void    meth_cgi_send_resp(void) {
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
								   _len_send_buffer.front(), 0)) < 1) {
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
	void    chunk_reading(void); //callback.cpp
	std::string	find_index_if_exist(void);
	void meth_get_request_is_valid(void);
	std::list<t_task_f>	init_recipe_get(void);

	std::list<t_task_f> init_recipe_put(void);
	void    meth_put_write_body(void);
	void    meth_put_choose_in(void);
	void    meth_put_open_fd(void);
	void    gen_resp_headers(void);
	void    send_respons(void);
	void	send_respons_body(void);

	};




class    TaskQueue {
public:
	void    exec_task() { }
	void    push(std::list<Socket> &clients) {
		Callback cb_temp;
		std::list<Socket>::iterator it_sockets = clients.begin();
		std::list<Socket>::iterator ite_sockets = clients.end();

		while (it_sockets != ite_sockets) {
			if (it_sockets->is_header_read && !it_sockets->is_callback_created &&
				it_sockets->is_cache_resp == false) {
				cb_temp = Callback((*it_sockets),
										 (it_sockets->headers), clients);
				it_sockets->is_callback_created = true;
				_tasks.push(cb_temp);
			}
			++it_sockets;
		}
	}
	size_t  size(void) const { return _tasks.size(); }

private:
	std::queue<Callback>	_tasks;
};


// parser functions
void parse_buffer(std::list<char*> &buffer, Header &headers,
				  std::map<std::string, request_header> &headers_parsers,
				  bool &is_status_line_read, std::list<ssize_t> &len_buf_parts);
std::string 		get_key(std::string::iterator &it, std::string &input);
std::string			get_value(std::string::const_iterator it);
void				init_server_var_map(std::map<std::string, void*> &_map, Server *srv);
void				parse_root(std::string::const_iterator it, void *ptr);
void				parse_index(std::string::const_iterator it, void *ptr);
void				parse_listen(std::string::const_iterator it, void *ptr);
void				parse_methods(std::string::const_iterator it, void *ptr);
void				parse_location(std::string::const_iterator it, void *ptr);
void				parse_autoindex(std::string::const_iterator it, void *ptr);
void				parse_error_page(std::string::const_iterator it, void *ptr);
void				parse_server_name(std::string::const_iterator it, void *ptr);
void				parse_fastcgi_pass(std::string::const_iterator it, void *ptr);
void				parse_fastcgi_param(std::string::const_iterator it, void *ptr);
void				parse_client_max_body_size(std::string::const_iterator it, void *ptr);
void				init_parser_functions_map(std::map<std::string, parser_function> &_map);
Location			get_location(std::string &conf);
Server				get_server(std::string &conf);
bool				parse_input(int argc, char **argv);
bool				check_brackets(std::string &conf);

// core functions

void	parse_conf(std::string path, std::list<Server> &server_list);
void 	init_socket_list(std::list<Server> &server_list, std::list<Socket>	&socket_list);
void	launch_server(std::list<Server> &server_list, std::list<Socket> &socket_list);
void	assign_server_to_socket(std::list<Server> &server_list, std::list<Socket> &socket_list);

void parse_buffer(std::list<char*> &buffer, Header &headers,
				  std::map<std::string, request_header> &headers_parsers,
				  bool &is_status_line_read, std::list<ssize_t> &len_buf_parts);
#endif