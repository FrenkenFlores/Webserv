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
	int									client_max_body_size;
	int									server_id;
	std::list<std::string>				index;			// list of indexes
	std::list<std::string>				server_name;	// list of server names
	address								listen;
	std::string							root;
	std::string							autoindex;
	std::map<std::string, std::string>	fastcgi_param;
	std::map<std::string, int>			error_page;
	std::list<Location>					location;
};



std::string get_key(std::string::iterator &it, std::string &input);

#endif