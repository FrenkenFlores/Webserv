#include "webserv.hpp"

#define CRLF "\r\n"

std::string get_word(std::string const &src, std::string::const_iterator it,
					 std::string sep) {
	std::string::const_iterator ite = it;

	while (ite != src.end() && sep.find(*ite) == std::string::npos)
		ite++;
	return (std::string(it, ite));
}

/* clear_buffer
 * This function will free allocated c_string from the buffer and then will pop front element
 */
static void clear_buffer(std::list<char*> &buffer, unsigned int &len,
					  std::list<ssize_t> &len_buf_parts) {
	while (buffer.size() > 0 && len > strlen(buffer.front())) {
		len -= strlen(buffer.front());
		free(buffer.front());
		buffer.pop_front();
		len_buf_parts.pop_front();
	}
}

char	*ft_substr(char const *s, unsigned int start, size_t len)
{
	char	*sub;
	size_t	i;

	if (!s)
		return (nullptr);
	if (strlen(s) < start)
		return (strdup(""));
	if (strlen(s + start) < len)
		len = strlen(s + start);
	i = 0;
	sub = (char*)malloc((len + 1) * sizeof(char));
	if (sub == nullptr)
		return (nullptr);
	while (len > i && *s != '\0')
	{
		sub[i] = s[start + i];
		i++;
	}
	sub[i] = '\0';
	return (sub);
}

/* CUT_BUFFER
 * This function will cut n chars from the start of the buffer.
 */
void    cut_buffer(std::list<char*> &buffer, unsigned int len,
				   std::list<ssize_t> &len_buf_parts) {
	char *sub_string = nullptr;
	unsigned int sub_str_len;

	clear_buffer(buffer, len, len_buf_parts);
	if (buffer.size() > 0) {
		sub_str_len = len_buf_parts.front() - len;
		sub_string = ft_substr(buffer.front(), len, sub_str_len);
		free(buffer.front());
		buffer.pop_front();
		len_buf_parts.pop_front();
		if (strlen(sub_string) > 0) {
			buffer.push_front(sub_string);
			len_buf_parts.push_front(sub_str_len);
		}
		else
			free(sub_string);
	}
}

/* REMOVE_CRLF
 * remove starting '\r', '\n' from the buffer.
 */

bool    remove_crlf(std::list<char*> &buffer,
				   std::list<ssize_t> &len_buf_parts) {
	char         *i_buf_parts = nullptr;
	unsigned int len_to_flush = 0;
	std::list<char*>::iterator    it = buffer.begin();
	std::list<char*>::iterator    ite = buffer.end();

	while (it != ite) {
		i_buf_parts = *it;
		while (*i_buf_parts == '\r' || *i_buf_parts == '\n') {
			++i_buf_parts;
			++len_to_flush;
		}
		if (*i_buf_parts != '\0')
			break;
		++it;
	}
	cut_buffer(buffer, len_to_flush, len_buf_parts);
	return (len_to_flush > 0);
}

/* FIND_STR_BUFFER
 * returns the number of chars between the start of the buffer and
 * the first char of to_find if it found in the buffer. If to_find is not found
 * in the buffer, this function return 0.
 */
unsigned int find_str_buffer(std::list<char*> &buffer, std::string to_find) {
	char const   *i_to_find = nullptr;
	char const   *i_buf_parts = nullptr;
	unsigned int final_len = 0;
	std::list<char*>::iterator it = buffer.begin();
	std::list<char*>::iterator ite = buffer.end();

	i_to_find = to_find.c_str();
	while (it != ite) {
		i_buf_parts = *it;
		while (*i_buf_parts) {
			if (*i_buf_parts == *i_to_find) {
				++i_to_find;
				if (*i_to_find == '\0') {                     // exit case
					++final_len;
					return (final_len - to_find.size());
				}
			} else if (i_to_find != to_find.c_str()) {      // not complete
				i_to_find = to_find.c_str();
			}
			++final_len;
			++i_buf_parts;
		}
		++it;
	}
	return (0);
}

static std::string clear_buffer_ret(std::list<char*> &buffer, unsigned int &len,
							 std::list<ssize_t> &len_buf_parts) {
	std::string save_del;

	save_del.reserve(len);
	while (buffer.size() > 0 && len > len_buf_parts.front()) {
		len -= len_buf_parts.front();
		save_del += buffer.front();
		free(buffer.front());
		buffer.pop_front();
		len_buf_parts.pop_front();
	}
	return (save_del);
}

/* CUT_BUFFER_RET
 * This function will cut n chars from the start of the buffer and return them.
 */
std::string cut_buffer_ret(std::list<char*> &buffer, unsigned int len,
					   std::list<ssize_t> &len_buf_parts) {
	char *sub_string = nullptr;
	unsigned int sub_str_len;
	std::string save_del;

	save_del += clear_buffer_ret(buffer, len, len_buf_parts);
	if (buffer.size() > 0) {
		sub_str_len = len_buf_parts.front() - len;
		sub_string = ft_substr(buffer.front(), 0, len);
		save_del += sub_string;
		free(sub_string);
		sub_string = ft_substr(buffer.front(), len, sub_str_len);
		free(buffer.front());
		buffer.pop_front();
		len_buf_parts.pop_front();
		if (strlen(sub_string) > 0) {
			buffer.push_front(sub_string);
			len_buf_parts.push_front(sub_str_len);
		}
		else
			free(sub_string);
	}
	return (save_del);
}

/* GET_LINE
 * This function return a line if a CRLF is in the buffer. Else return
 * empty string. If is_status_line_read parameter is true, this function will flush CRLF
 * and return the line.
 */

std::string get_line(std::list<char*> &buffer, bool is_status_line_read,
					   std::list<ssize_t> &len_buf_parts) {
	std::string  final_header;
	unsigned int header_len = 0;

	if (is_status_line_read == false)
		remove_crlf(buffer, len_buf_parts);
	if (find_str_buffer(buffer, CRLF) == 0)
		return ("");
	header_len = find_str_buffer(buffer, CRLF);
	final_header = cut_buffer_ret(buffer, header_len, len_buf_parts);
	cut_buffer(buffer, 2, len_buf_parts);
	return (final_header);
}



void init_header_ptrs(std::map<std::string, void *> &str_ptrs_map, Header &headers) {
	str_ptrs_map["Path"] = &(headers.path);
	str_ptrs_map["Date"] = &(headers.date);
	str_ptrs_map["Host"] = &(headers.host);
	str_ptrs_map["Error"] = &(headers.status_code);
	str_ptrs_map["Method"] = &(headers.method);
	str_ptrs_map["Referer"] = &(headers.referer);
	str_ptrs_map["Protocol"] = &(headers.protocol);
	str_ptrs_map["User-Agent"] = &(headers.user_agent);
	str_ptrs_map["Content-Type"] = &(headers.content_type);
	str_ptrs_map["Authorization"] = &(headers.authorization);
	str_ptrs_map["Accept-Charset"] = &(headers.accept_charset);
	str_ptrs_map["Content-Length"] = &(headers.content_length);
	str_ptrs_map["Accept-Language"] = &(headers.accept_language);
	str_ptrs_map["Transfer-Encoding"] = &(headers.transfer_encoding);
	str_ptrs_map["trans_mode_saw"] = &(headers.is_transfer_mode_saw);
}

static void     check_request_line(std::map<std::string,void *> &req_header) {
	size_t *error_code = static_cast<size_t *>(req_header["Error"]);
	std::string *method = static_cast<std::string *>(req_header["Method"]);
	std::string *path = static_cast<std::string *>(req_header["Path"]);
	std::string *protocol = static_cast<std::string *>(req_header["Protocol"]);

	if (method->empty() == true) {
		*error_code = 400;
	} else if (path->empty() == true ||
			   (strncmp(path->c_str(), "/", 1) != 0)) {
		*error_code = 400;
	} else if (protocol->empty() == true) {
		*error_code = 400;
	} else if (strcmp(protocol->c_str(), "HTTP/1.1") != 0) {
		*error_code = 505;
	}
}

static void    parse_protocol(std::string const &line,
							  std::string::const_iterator it,
							  std::map<std::string, void *> &header_ptrs) {
	std::string     protocol;
	std::string     sep(" ");

	protocol = get_word(line, it, sep);
	for (; it != line.end(); ++it)
		if (sep.find(*it) != std::string::npos)
			break ;
	if (it != line.end()) {
		size_t *error_code = static_cast<size_t *>(header_ptrs["Error"]);
		*error_code = 400;
	}
	std::string * ptr = static_cast<std::string *>(header_ptrs["Protocol"]);
	*ptr = protocol;
}

static void    parse_path(std::string line,
						  std::map<std::string, void *> &header_ptrs) {
	std::string     path;
	std::string     sep(" ");
	std::size_t     found = line.find(sep);
	std::string::const_iterator it, ite;

	if (found == std::string::npos)
		return ;
	it = line.begin() + found;
	ite = line.end();
	while (it != ite && sep.find(*it) != std::string::npos)
		++it;
	path = get_word(line, it, sep);
	if (path == *(static_cast<std::string *>(header_ptrs["Method"])))
		return ;
	std::string * ptr = static_cast<std::string *>(header_ptrs["Path"]);
	*ptr = path;
	while (it != ite && sep.find(*it) == std::string::npos)
		++it;
	while (it != ite && sep.find(*it) != std::string::npos)
		++it;
	parse_protocol(line, it, header_ptrs);
}

static std::list<std::string>   init_meth_names(void) {
	std::list<std::string>  meth_names_lst;

	meth_names_lst.push_back("GET");
	meth_names_lst.push_back("PUT");
	meth_names_lst.push_back("HEAD");
	meth_names_lst.push_back("POST");
	meth_names_lst.push_back("TRACE");
	meth_names_lst.push_back("DELETE");
	meth_names_lst.push_back("OPTIONS");
	return (meth_names_lst);
}

void    parse_status_line(std::string line,
						  std::map<std::string, void*> &header_ptrs) {
	std::string             prefix;
	std::string             sep(" ");
	std::list<std::string>  lst_prefix = init_meth_names();
	std::list<std::string>::iterator it = lst_prefix.begin();
	std::list<std::string>::iterator ite = lst_prefix.end();
	size_t *error_code = static_cast<size_t *>((header_ptrs)["Error"]);

	prefix = get_word(line, line.begin(), sep);
	while (it != ite) {
		if (prefix == *it) {
			std::string * ptr = (std::string*)((header_ptrs)["Method"]);
			*ptr = prefix;
			parse_path(line, header_ptrs);
			break ;
		}
		++it;
	}
	if (it == ite && prefix.empty() == false)
		*error_code = 405;
	else if (it == ite && prefix.empty() == true)
		*error_code = 400;
	else
		check_request_line(header_ptrs);
}







/* CHECK_IF_HOST_EXIST()
 * This function check if Host header is already saved. Host header duplication
 * cause 400;
 */
static int  check_if_host_exist(std::string prefix, void *p) {
	if (prefix == "Host") {
		std::string *ptr = static_cast<std::string*>(p);
		if (ptr->empty() == false)
			return (1);
	}
	return (0);
}

/* PARSE_REQUEST_HEADER()
 * parses the header in line and edit Headers variables.
 *
 * It return 1 if the line given contain an HTTP header. Return 0 else
 */
int    parse_header(std::string line,
					std::map<std::string, void *> header_ptrs,
					std::map<std::string, request_header> header_parsers) {
	std::string     prefix;
	std::string     sep(":");

	prefix = get_word(line, line.begin(), sep);
	if (prefix == "Transfer-Encoding" || prefix == "Content-Length") {
		if (*((bool*)(header_ptrs)["trans_mode_saw"]) == false)
			*((bool*)(header_ptrs)["trans_mode_saw"]) = true;
		else
			return (1);
	}
	if (header_parsers.find(prefix) != header_parsers.end()) {
		if ((check_if_host_exist(prefix, (header_ptrs)[prefix]) == 1))
			return (1);
		else
			(header_parsers)[prefix](line, (header_ptrs)[prefix]);
	}
	return (0);
}

void parse_buffer(std::list<char*> &buffer, Header &headers,
				  std::map<std::string, request_header> &headers_parsers,
				  bool &is_status_line_read, std::list<ssize_t> &len_buf_parts) {
	std::string                  header_to_parse;
	std::map<std::string, void*> headers_ptrs;

	init_header_ptrs(headers_ptrs, headers);
	header_to_parse = get_line(buffer, is_status_line_read, len_buf_parts);

	while (!header_to_parse.empty() && headers.status_code / 100 == 2) {
		if (!is_status_line_read) {            // Status line parsing
			parse_status_line(header_to_parse, headers_ptrs);
			is_status_line_read = true;
			if (headers.method == "TRACE") {
				headers.host = "tmp";
				return ;
			}
		} else {                                        // Common head parsing
			if (parse_header(header_to_parse, headers_ptrs, headers_parsers)
				== 1) {                             // Double host status_code
				headers.status_code = 400;
			}
			headers.saved_headers.push_back(header_to_parse);
		}
		header_to_parse = get_line(buffer, is_status_line_read,
									 len_buf_parts);
	}
	cut_buffer(buffer, 2, len_buf_parts);
}