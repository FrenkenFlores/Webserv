#ifndef UTILS_HPP
# define UTILS_HPP

# include <list>
# include <string>
# include <iostream>

# include <errno.h>
# include <string.h>
# include <stdlib.h>

# include "Tmpfile.hpp"
# include "Socket.hpp"
# include "read_headers.hpp"

const std::string whitespaces("\t\n\v\f\r ");

std::string ft_basename(std::string const path);
std::string ft_dirname(std::string const path);
char	*substr(char const *s, unsigned int start, size_t len);
void    ft_error(std::string const &src,
                 std::string const &err = strerror(errno));

void remove_client(std::list<Socket> *clients, int client_fd,
                   ssize_t bytes_read);

char        *strcont_to_str(std::string str);
char        *lststr_to_str(std::list<std::string> const &lst, std::string sep);
std::string lststr_to_strcont(std::list<std::string> const &lst,
                              std::string sep);
unsigned int     hextodec(char const *hexa);

bool    is_fd_read_ready(int fd);
bool    is_fd_write_ready(int fd);

char    *concate_list_str(std::list<char*> *buffer);
bool    is_buffer_crlf(std::list<char*> *buffer);
std::string get_word(std::string const &src, std::string::const_iterator it,
                     std::string sep = whitespaces);

size_t  lststr_len(std::list<std::string> const &lst, std::string const sep);
void    ft_freestrs(char **strs);
char    **lststr_to_strs(std::list<std::string> lst);
std::list<std::string> gen_listening(std::string dir_path);
std::string get_err_page(int code, std::string path);
std::string get_status_msg(int code);
std::string get_status_line(int code);
char *cgitohttp(Tmpfile *tmpfile, size_t *status_code);

#endif
