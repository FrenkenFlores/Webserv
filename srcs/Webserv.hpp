#ifndef WEBSERV_HPP
#define WEBSERV_HPP

#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <map>
#include <list>
#include <stack>

#define DEFAULT_CONFIG_PATH "../configs/default.conf"


typedef void (*parser_function)(std::string::const_iterator cit, void*);


struct address {
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
};


struct Server {
	int									server_id;
	int									client_max_body_size;
	std::list<std::string>				index;			// list of indexes
	std::list<std::string>				server_name;	// list of server names
	address								listen;
	std::string							root;
	std::string							autoindex;
	std::map<std::string, std::string>	fastcgi_param;
	std::map<std::string, int>			error_page;
	std::list<Location>					location;
};


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
std::list<Server>	parse_conf(std::string path, std::list<Server> &server_list);
#endif