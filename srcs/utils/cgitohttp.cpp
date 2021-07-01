#include "utils.hpp"
#include "Tmpfile.hpp"

#define KEY_STATUS "Status: "

static std::string get_status(int fd, size_t *status_code) {
    char 		c;
    std::string line;
    std::string status_line;

    while (read(fd, &c, 1)) {
    	if (c == '\r' || c == '\n')
			break;
		line += c;
    }
	if (!line.empty() &&
            strncmp(&line.c_str()[0], KEY_STATUS, strlen(KEY_STATUS)) == 0) {
        *status_code = (size_t)atoi(&line.c_str()[0] + strlen(KEY_STATUS));
        if (get_status_msg(*status_code) != "") {
            status_line = get_status_line(*status_code);
            status_line += "\r\n";
        }
    }
    return (status_line);
}

static int         get_headers_len(int fd) {
    size_t  headers_len = 0;
    int     status;
    std::string headers;
    char c;

	while (read(fd, &c, 1)) {
		if (c == '\r' || c == '\n')
			break;
		headers += c;
	}
    if (headers.empty())
        headers_len += ((status ?: 1) - 1) + strlen("\r\n\r\n");
    return (headers_len);
}

static std::string get_content_len(size_t tmpfile_size, int fd) {
	std::string str_content_len;
    std::string http_content = "";

    str_content_len = std::to_string(tmpfile_size - (get_headers_len(fd)));
    http_content = "Content-Length: ";
    http_content += str_content_len;
    http_content += "\r\n";
    return (http_content);
}

char    *cgitohttp(Tmpfile *tmpfile, size_t *status_code) {
    std::string http_content;

    http_content += get_status(tmpfile->get_fd(), status_code);
    if (*status_code / 100 != 2)
        return (NULL);
    if (http_content.empty()) {
        http_content += "HTTP/1.1 200 OK\r\n";
    }
    tmpfile->reset_cursor();
    http_content += get_content_len(tmpfile->get_size(), tmpfile->get_fd());
    tmpfile->reset_cursor();
    return (strdup(http_content.c_str()));
}
