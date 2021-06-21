#include "Webserv.hpp"



std::string get_header(std::list<char*> &buffer, bool is_status_line_read,
					   std::list<ssize_t> &len_buf_parts) { }




void parse_buffer(std::list<char*> &buffer, Header &headers,
						 std::map<std::string, request_header> &headers_parsers,
						 bool is_status_line_read, std::list<ssize_t> &len_buf_parts) {
	std::string                  header_str;
	std::map<std::string, void*> header_parser_map;

	header_str = get_header(buffer, is_status_line_read, len_buf_parts);
}