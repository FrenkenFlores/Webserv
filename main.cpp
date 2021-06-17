#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <list>

#define DEFAULT_CONFIG_PATH "configs/default.conf"

struct Server { };


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
	std::vector<std::string> conf;
	std::ifstream ifs;
	std::string line;

	ifs.open(path);
	if (ifs.is_open()) {
		while (std::getline(ifs, line)) {
			conf.push_back(line);
		}
		ifs.close();
	}
	for (std::string i : conf) {
		std::cout << i << std::endl;
	}
	return server_list;
}

int main(int argc, char **argv) {
	if (parse_input(argc, argv)) {
		parse_conf(argv[1]);
	}
	return 0;
}
