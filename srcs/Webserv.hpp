#ifndef WEBSERV_HPP
#define WEBSERV_HPP

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

#define DEFAULT_CONFIG_PATH "../configs/default.conf"

class Socket;

bool read_headers(std::list<Socket> &socket_list);
void remove_client(std::list<Socket> &socket_list, int response_fd,
				   ssize_t bytes_read);





extern bool g_run;
extern int g_worker_connections;

typedef void (*parser_function)(std::string::const_iterator cit, void*);
typedef void (*request_header)(std::string line, void * p);


struct Address {
	std::string ip;
	int port;
};

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
	std::map<std::string, std::string>	fastcgi_param;
	std::map<std::string, int>			error_page;
	std::list<Location>					location;
	Server () {
		server_id = 0;
		client_max_body_size = 0;
		ip_port.port = -1;
	}
};

struct Header {
	bool					is_transfer_mode_saw;
	size_t					error; // var that will be status-code
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
	size_t					status_code;
	// Response variables
	size_t      content_length_h;
	std::string location_h;
	std::string last_modified_h;


	Header() {
		is_transfer_mode_saw = false;
		error = 0;
		content_length = 0;
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
	}
};

class	Callback {
public:
	Socket socket;
	Server server;
	Header request;

	Callback(Socket socket) {}
	void exec() { }
	bool is_over() { }
};




class    TaskQueue {
public:
	void    exec_task() { }
	void    push(std::list<Socket> &clients) { }
	size_t  size(void) const { return _tasks.size(); }

private:
	std::queue<Callback>	_tasks;
};


// parser functions
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
void	launch_server(std::list<Socket> &socket_list);

#endif