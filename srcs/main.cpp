#include "Webserv.hpp"

void init_server_var_map(std::map<std::string, void*> &_map, Server *srv) {
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


std::string get_value(std::string::const_iterator it) {
	std::string value;
	while (*it != ' ')
		--it;
	while (*it != '\n' && *it != '\0') {
		++it;
		value += *it;
	}
	return value;
}

void    parse_root(std::string::const_iterator it, void *ptr) {
	std::cout << "parse_root" << std::endl;
	std::string *value = reinterpret_cast<std::string *>(ptr);
	*value = get_value(it);
}
// parse multiple indexes
void    parse_index(std::string::const_iterator it, void *ptr) {
	std::cout << "parse_index" << std::endl;
	std::list<std::string> *value = reinterpret_cast<std::list<std::string> *>(ptr);

	std::string str;
	while (*(--it) != '\n');									// carriage return
	while (isspace(*it) && *(++it) != '\n' && *it != '\0')		// pass spaces
		++it;
	while (!isspace(*it) && *(it) != '\n' && *it != '\0')		// pass key
		++it;
	while (*(it) != '\n' && *it != '\0') {
		str = "";
		while (!isspace(*it) && *it != ';') {
			str += *it;
			++it;
		}
		if (!str.empty()) {
			value->push_back(str);
		}
		++it;
	}
	//check list;
//	std::cout << "index: " << std::endl;
//	std::cout << "size: " << value->size() << std::endl;
//	for (auto i : *value)
//		std::cout << "--->" << i << std::endl;
}

void    parse_listen(std::string::const_iterator it, void *ptr) {
	std::cout << "parse_listen" << std::endl;
	address *value = reinterpret_cast<address *>(ptr);
	std::string str = get_value(it);
	int i = 0;
	while (str[i] != ':')
		value->ip += str[i++];
	value->port = atoi(&str[i + 1]);
//	check ip/port
//	std::cout << "ip/port: " << std::endl;
//	std::cout << value->ip << ":" << value->port << std::endl;
}

void    parse_methods(std::string::const_iterator it, void *ptr) {
	std::cout << "parse_methods" << std::endl;

}

void    parse_location(std::string::const_iterator it, void *ptr) {
	std::cout << "parse_location" << std::endl;
}

void    parse_autoindex(std::string::const_iterator it, void *ptr) {
	std::cout << "parse_autoindex" << std::endl;
	std::string *value = reinterpret_cast<std::string *>(ptr);
	*value = get_value(it);
}

void    parse_error_page(std::string::const_iterator it, void *ptr) {
	std::cout << "parse_error_page" << std::endl;
	std::map<std::string, int> *value = reinterpret_cast<std::map<std::string, int> *>(ptr);
	std::string str;
	int error_code;

	while (*(--it) != '\n');									// carriage return
	while (isspace(*it) && *(++it) != '\n' && *it != '\0');		// pass spaces
	while (!isspace(*it) && *(++it) != '\n' && *it != '\0');	// pass key
	while (isspace(*it) && *(++it) != '\n' && *it != '\0');		// pass spaces

	str = "";
	while (isnumber(*it) && *it != ';') {
		str += *it;
		++it;
	}
	error_code = atoi(&str[0]);
	str = "";
	while (isspace(*it) && *(++it) != '\n' && *it != '\0');		// pass spaces
	while (!isspace(*it) && *it != ';') {
		str += *it;
		++it;
	}
	(*value)[str] = error_code;
//	std::cout << "error page:" << std::endl;
//	std::cout << str << ":" << (*value)[str] << std::endl;
}
// parse multiple server names
void    parse_server_name(std::string::const_iterator it, void *ptr) {
	std::cout << "parse_server_name" << std::endl;
	std::list<std::string> *value = reinterpret_cast<std::list<std::string> *>(ptr);

	std::string str;
	while (*(--it) != '\n');									// carriage return
	while (isspace(*it) && *(++it) != '\n' && *it != '\0')		// pass spaces
		++it;
	while (!isspace(*it) && *(it) != '\n' && *it != '\0')		// pass key
		++it;
	while (*(it) != '\n' && *it != '\0') {
		str = "";
		while (!isspace(*it) && *it != ';') {
			str += *it;
			++it;
		}
		if (!str.empty()) {
			value->push_back(str);
		}
		++it;
	}
//	check list;
//	std::cout << "server_name: " << std::endl;
//	std::cout << "size: " << value->size() << std::endl;
//	for (auto i : *value)
//		std::cout << "--->" << i << std::endl;
}

void    parse_fastcgi_pass(std::string::const_iterator it, void *ptr) {
	std::cout << "parse_fastcgi_pass" << std::endl;
}

void    parse_fastcgi_param(std::string::const_iterator it, void *ptr) {
	std::cout << "parse_fastcgi_param" << std::endl;
}

void    parse_client_max_body_size(std::string::const_iterator it, void *ptr) {
	std::cout << "parse_client_max_body_size" << std::endl;
	int *value = reinterpret_cast<int *>(ptr);
	std::string nbr = get_value(it);
	*value = atoi(&nbr[0]);
//	check max_body_size
//	std::cout << "max_body_size: " << std::endl;
//	std::cout << *value << std::endl;
}


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
//	server.index.push_back("index.html");

	if (get_key(it, conf) != "server")
		throw std::logic_error("Server not found");
	while (*it != '\0') {
		key = get_key(it, conf);
		if (server_var_map.count(key)) {
			parser_functions_map[key](it, server_var_map[key]);
		}
//		std::cout << "Check autoindex: " << server.autoindex << std::endl;
//		std::cout << "Check root: " << server.root << std::endl;
//		std::cout << "Key: " << key << std::endl;
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
	bool flag = false;
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
			flag = true;
		}
		++it_b;
	}
	if (flag)
		return tracker.empty();		// OK
	return false;

}

std::list<Server> parse_conf(std::string path, std::list<Server> &server_list) {
	std::string full_conf;				//for bracket checking
	std::string conf;
	std::ifstream ifs;
	std::string line;
	int server_id = 0;

	ifs.open(path);
	if (ifs.is_open()) {
		while (std::getline(ifs, line)) {
			conf += line + "\n";
			full_conf += line + "\n";
			if (check_brackets(conf)) {
				server_list.push_back(get_server(conf));
				server_list.back().server_id = server_id;
				server_id++;
				conf.clear();
			}
		}
		ifs.close();
	}
	if (!check_brackets(full_conf))
		throw std::logic_error("Configuration file is not correct");
	return server_list;
}

int main(int argc, char **argv) {
	std::list<Server> server_list;
	if (parse_input(argc, argv)) {
		try {
			server_list = parse_conf(argv[1], server_list);
//			std::cout << server_list.front().listen.ip << ":" << server_list.front().listen.port << std::endl;
		} catch (std::exception &e) {
			std::cerr << e.what() << std::endl;
			return (1);
		}
	}
	return 0;
}
