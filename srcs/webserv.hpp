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
#include <time.h>
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
bool    is_fd_read_ready(int fd);
bool    is_fd_write_ready(int fd);


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
	Address() : ip(""), port(-1) { }
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
	std::map<int, std::string>				error_page;
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
	std::map<int, std::string>			error_page;
	std::list<std::string>				methods;
	std::list<Location>					location;
	Server () {
		server_id = 0;
		client_max_body_size = 0;
		ip_port.port = -1;
	}
	Server (const Server &server) {
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
	Socket (const Socket &_socket) {
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
	TmpFile(void)  : _fd(-1) {
		mkdir(TmpFile::_path, 0777);
		if (errno != 0 && errno != EEXIST)
			std::perror("TmpFile: mkdir");
		errno = 0;
		while (TmpFile::_does_nextfile_exist())
			TmpFile::_update_nextnameprefix();
		this->_filename = TmpFile::_get_next_name();
		this->_fd = open(_filename.c_str(), O_CREAT | O_TRUNC | O_RDWR, S_IRWXU);
		fcntl(this->_fd, F_SETFL, O_NONBLOCK);
		if (errno != 0)
			std::perror("TmpFile: open");
	}
	TmpFile(TmpFile const &src) {
		*this = src;
	}
	virtual ~TmpFile(void) {
		if (this->_fd == -1)
			return ;
		close(this->_fd);
		unlink(this->_filename.c_str());
	}

	TmpFile           &operator=(TmpFile const &rhs){
		if (this == &rhs)
			return (*this);
		this->_filename = rhs._filename;
		this->_fd = rhs._fd;
		return (*this);
	}
	int const           &get_fd(void) const {
		return (this->_fd);
	}

	std::string const   &get_filename(void) const {
		return (this->_filename);
	}
	bool                is_read_ready(void) const {
		return (is_fd_read_ready(this->_fd));
	};

	bool                is_write_ready(void) const{
		return (is_fd_write_ready(this->_fd));
	}

	size_t              get_size(void) const {
		struct stat file_stat;

		fstat(this->get_fd(), &file_stat);
		return (file_stat.st_size);
	}

	void                reset_cursor(void){
		if (lseek(this->_fd, 0, SEEK_SET) != -1)
			return ;
		std::perror(
				"lseek");
	}
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