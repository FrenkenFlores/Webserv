//
// Created by Jamis Heat on 6/23/21.
//

#include "Webserv.hpp"

void    reset_header(Header &h) {
	h.path = "";
	h.date = "";
	h.host = "";
	h.status_code = 200;
	h.method = "";
	h.referer = "";
	h.protocol = "";
	h.content_length = 0;
	h.user_agent.clear();
	h.content_type.clear();
	h.authorization.clear();
	h.saved_headers.clear();
	h.transfer_encoding = "";
	h.accept_charset.clear();
	h.accept_language.clear();
	h.is_transfer_mode_saw = false;
};

void    reset_socket(Socket &s) {
	std::list<char*>::iterator it_buf = s.buffer.begin();
	std::list<char*>::iterator ite_buf = s.buffer.end();

	s.server = NULL;
	s.is_read_ready = false;
	s.is_cache_resp = false;
	s.is_write_ready = false;
	s.is_header_read = false;
	reset_header(s.headers);
	s.is_status_line_read = false;
	s.is_callback_created = false;
	s.len_buf_parts.clear();
	while (it_buf != ite_buf) { // Clean header buffer
		free(*it_buf);
		s.buffer.erase(it_buf++);
	}
}
