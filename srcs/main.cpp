#include "Webserv.hpp"


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
		throw std::logic_error("Configuration file is uncorrect");
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
