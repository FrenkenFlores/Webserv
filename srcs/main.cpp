#include "Webserv.hpp"

void init_server_var_map(std::map<std::string, void*> _map, Server *srv) {
	_map["root"] = &(srv->root);
	_map["index"] = &(srv->index);
	_map["listen"] = &(srv->listen);
	_map["location"] = &(srv->location);
	_map["autoindex"] = &(srv->autoindex);
	_map["error_page"] = &(srv->error_page);
	_map["server_name"] = &(srv->server_name);
	_map["fastcgi_param"] = &(srv->fastcgi_param);
	_map["client_max_body_size"] = &(srv->client_max_body_size);
}

void    parse_root(std::string::const_iterator it, void *ptr) { }

void    parse_index(std::string::const_iterator it, void *ptr) { }

void    parse_listen(std::string::const_iterator it, void *ptr) { }

void    parse_methods(std::string::const_iterator it, void *ptr) { }

void    parse_location(std::string::const_iterator it, void *ptr) { }

void    parse_autoindex(std::string::const_iterator it, void *ptr) { }

void    parse_error_page(std::string::const_iterator it, void *ptr) { }

void    parse_server_name(std::string::const_iterator it, void *ptr) { }

void    parse_fastcgi_pass(std::string::const_iterator it, void *ptr) { }

void    parse_fastcgi_param(std::string::const_iterator it, void *ptr) { }

void    parse_client_max_body_size(std::string::const_iterator it, void *ptr) { }


void init_parser_functions_map(std::map<std::string, parser_function> &_map) {
	_map["root"] = &parse_root;
	_map["index"] = &parse_index;
	_map["listen"] = &parse_listen;
	_map["methods"] = &parse_methods;
	_map["location"] = &parse_location;
	_map["autoindex"] = &parse_autoindex;
	_map["error_page"] = &parse_error_page;
	_map["server_name"] = &parse_server_name;
	_map["fastcgi_pass"] = &parse_fastcgi_pass;
	_map["fastcgi_param"] = &parse_fastcgi_param;
	_map["client_max_body_size"] = &parse_client_max_body_size;
}


std::string get_key(std::string::iterator &it, std::string &input) {
	std::string return_value;
	size_t end;
	while (!std::isalpha(*it) && *it != '_' && *it != '\0')
		++it;
	while ((std::isalpha(*it) || *it == '_') && *it != '\0') {
		return_value += *it;
		++it;
	}
	while (*it != '\n' && *it != '\0')
		++it;
	return return_value;
}


Server    get_server(std::string &conf) {

	Server server;
	std::string key;
	std::string::iterator it = conf.begin();
	std::map<std::string, void*>    server_var_map;
	std::map<std::string, parser_function> parser_functions_map;
	init_server_var_map(server_var_map, &server);
	init_parser_functions_map(parser_functions_map);

	server.server_id = 0;
	server.client_max_body_size = 1;		//default nginx max_body_size
	server.index.push_back("index.html");

	if (get_key(it, conf) != "server")
		throw std::logic_error("Server not found");
	while (*it != '\0') {
		key = get_key(it, conf);
		if (server_var_map.count(key))
			parser_functions_map[key](it, server_var_map[key]);
		std::cout << "Key: " << key << std::endl;
	}
	return server;
}


bool parse_input(int argc, char **argv) {
	if (argc > 2) {
		std::cout << "Usage: " << argv[0] << " " << "file.conf" << std::endl;
		return false;
	} else if (argc == 2 && (strlen(argv[1]) - std::string(argv[1]).find(".conf")) == 5) {
		std::cout << "OK" << std::endl;
		return true;
	} else if (argc == 2 && (strlen(argv[1]) - std::string(argv[1]).find(".conf")) != 5){
		std::cout << "Unknown extension, only .conf extensions allowed" << std::endl;
		return false;
	} else {
		std::cout << "default" << std::endl;
		argv[1] = (char*)DEFAULT_CONFIG_PATH;
		return true;
	}
}

bool check_brackets(std::string &conf) {
	std::stack<char> tracker;
	std::string::iterator it_b = conf.begin();
	std::string::iterator it_e = conf.end();
	while (it_b != it_e) {
		if (*it_b == '{')
			tracker.push(*it_b);
		else if (*it_b == '}') {
			if (tracker.empty())
				return false;
			tracker.pop();
		}
		++it_b;
	}
	return tracker.empty();
}

std::list<Server> parse_conf(std::string path) {
	std::list<Server> server_list;
	std::string conf = "";
	std::ifstream ifs;
	std::string line;

	ifs.open(path);
	if (ifs.is_open()) {
		while (std::getline(ifs, line)) {
			conf += line + "\n";
		}
		ifs.close();
	}
	if (check_brackets(conf))
		get_server(conf);
	else
		throw std::logic_error("Configuration file is not correct");
	std::cout << conf << std::endl;
	return server_list;
}

int main(int argc, char **argv) {
	if (parse_input(argc, argv)) {
		try {
			parse_conf(argv[1]);
		} catch (std::exception &e) {
			std::cerr << e.what() << std::endl;
			return (1);
		}
	}
	return 0;
}
