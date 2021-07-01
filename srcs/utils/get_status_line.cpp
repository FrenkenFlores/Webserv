#include "utils.hpp"

std::string get_status_line(int code) {
	std::string code_msg = get_status_msg(code);
	std::string status_line = "HTTP/1.1 ";

	status_line += std::to_string(code);;
	status_line += " ";
	status_line += code_msg;
	return (status_line);
}