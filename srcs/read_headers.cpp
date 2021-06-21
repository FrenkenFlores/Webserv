//
// Created by Jamis Heat on 6/21/21.
//

#include "Webserv.hpp"

void init_header_parsers(std::map<std::string, request_header> &str_parsers_map) {

	str_parsers_map["Date"] = &parse_field_date;
	str_parsers_map["Host"] = &parse_field_std_string;
	str_parsers_map["Referer"] = &parse_field_std_string;
	str_parsers_map["User-Agent"] = &parse_field_list_string;
	str_parsers_map["Content-Type"] = &parse_field_list_string;
	str_parsers_map["Authorization"] = &parse_field_list_string;
	str_parsers_map["Accept-Charset"] = &parse_field_list_string;
	str_parsers_map["Content-Length"] = &parse_field_size_t;
	str_parsers_map["Accept-Language"] = &parse_field_list_string;
	str_parsers_map["Transfer-Encoding"] = &parse_field_std_string;
}

static bool        is_sep_header(std::list<char*> *buffer) {
	char *i_buf_parts = NULL;
	std::string crlf_save = "";
	std::list<char*>::iterator it = buffer->begin();
	std::list<char*>::iterator ite = buffer->end();

	while (it != ite && crlf_save != "\r\n\r\n") {
		i_buf_parts = *it;
		while (*i_buf_parts && crlf_save != "\r\n\r\n") {
			if (*i_buf_parts == '\r' || *i_buf_parts == '\n')
				crlf_save += *i_buf_parts;
			else
				crlf_save.clear();
			++i_buf_parts;
		}
		++it;
	}
	return (crlf_save == "\r\n\r\n");
}

#define BUFF_SIZE_SOCKET 1024
/* READ_SOCKET
 * This function will read socket and save data read in the buffer header.
 * Return what recv return.
 */
static ssize_t     read_socket(std::list<char*> *buffer, int client_fd,
							   std::list<ssize_t> *len_buf_parts) {
	char    *read_buffer = NULL;
	ssize_t bytes_read = 0;

	if (!(read_buffer = (char*)malloc(sizeof(char) * (BUFF_SIZE_SOCKET + 1))))
		return (NULL);
	bzero(read_buffer, BUFF_SIZE_SOCKET + 1);
	bytes_read = recv(client_fd, read_buffer, BUFF_SIZE_SOCKET, 0);
	if (bytes_read > 0) {
		buffer->push_back(read_buffer);
		len_buf_parts->push_back(bytes_read);
	} else {
		free(read_buffer);
	}
	return (bytes_read);
}

bool read_headers(std::list<Socket> &socket_list) {
	bool        is_one_req_ready = false;
	ssize_t     bytes_read = 0;
	std::list<Socket>::iterator           it = socket_list.begin();
	std::list<Socket>::iterator           ite = socket_list.end();
	std::map<std::string, request_header> headers_parsers;

	init_header_parsers(headers_parsers);
	while (it != ite) {
		// NON AVALAIBLE CLIENTS SKIPPING
		if (it->is_read_ready == false || it->response_fd == 0 ||
			it->is_header_read == true) {
			++it;
			continue;
		}
		usleep(100); // Avoid too fast reading
		// READ DATA FROM CLIENT
		bytes_read = read_socket(&it->buffer, it->response_fd, &(it->len_buf_parts));
		if (bytes_read == 0 || bytes_read == -1) // End of connection
		{ // End of connection
			remove_client(socket_list, (it++)->response_fd, bytes_read);
			continue;
		}
		// SAVE IF THERE A CRLF HEAD_BODY SEPARATOR READ IN BUFFER
		it->is_header_read = is_sep_header(&it->buffer);
		is_one_req_ready = it->is_header_read;    // At least one req read ret
		// PARSING DATA RECIEVED
		if (it->is_header_read == true) {
			parse_buffer(&(it->buffer), &(it->headers), &headers_parsers,
						 &(it->is_status_line_read), &(it->len_buf_parts));
		}
		if (it->headers.error / 100 != 2)          // Read finished if error
			it->is_header_read = true;

		++it;
	}
	return (is_one_req_ready);
}
