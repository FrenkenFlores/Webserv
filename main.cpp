#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <map>
#include <list>

#define DEFAULT_CONFIG_PATH "configs/default.conf"



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
	std::list<std::string>				index;
	address								listen;
	std::list<std::string>				server_name;
	std::string							root;
	std::string							autoindex;
	std::map<std::string, std::string>	fastcgi_param;
	std::map<std::string, int>			error_page;
	std::list<Location>					location;
};


Server    get_server(std::string::iterator it_conf) {
	typedef void (*parser_function)(std::string::const_iterator cit, void*);

	Server server;
	std::string key;
	std::map<std::string, void*>    server_ptr_select;
	std::map<std::string, parser_function> parser_functions_map;





	server.server_id = 0;
	server.client_max_body_size = -1;		//defaul nginx max_body_size
	server.index.push_back("index.html");


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
	std::cout << conf << std::endl;
	return server_list;
}

int main(int argc, char **argv) {
	if (parse_input(argc, argv)) {
		parse_conf(argv[1]);
	}
	return 0;
}
