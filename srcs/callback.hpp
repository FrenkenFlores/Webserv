#ifndef CALLBACK_HPP
#define CALLBACK_HPP
#include "webserv.hpp"


#define CGI_SEND_SIZE 50000
#define PUT_OPEN_FLAGS (O_WRONLY | O_TRUNC)
#define PUT_OPEN_CREAT_FLAGS (O_WRONLY | O_TRUNC | O_CREAT)
#define KEY_STATUS "Status: "
#define CHUNK_CLOSE -3
#define CHUNK_FATAL -2
#define CHUNK_ERROR -1
#define CHUNK_MORE   0
#define CHUNK_ENOUGH 1
#define CHUNK_END    2
#define BUFFER_SIZE 4096
#define MID_TEMPLATE "<a href=\"ELEM_NAME\">ELEM_NAME</a>"
#define TOP_PAGE "<html>\n"                                      \
                 "<head><title>Index of /jpeg/</title></head>\n" \
                 "<body>\n"                                      \
                 "<h1>Index of REQ_PATH</h1>"                    \
                 "<hr><pre><a href=\"../\">../</a>"
#define BOT_HTML "</pre><hr>"                              \
                 "<center></center></body>" \
                 "</html>"
#define TEMPLATE "<html>\r\n"                                               \
                 "<head><title>CODE MSG</title></head>\r\n"                 \
                 "<body><center><h1>CODE MSG</h1></center>\r\n"             \
                 "<hr><center></center></body>\r\n"     \
                 "</html>\r\n"
#define MSG_LEN 3
#define CODE_LEN 4
# define MSG_NOSIGNAL 0


class	Callback {
public:
	Socket socket;
	Server server;
	Header request;

	int	_fd_body;
	int	_put_fd_in;
	int	_fd_to_write;

	bool	_host;
	TmpFile	*_tmpfile;
	TmpFile	*_out_tmpfile;
	std::list<Socket>	socket_list;
	std::list<char*>	client_buffer;
	std::list<t_task_f> _recipes;
	std::list<t_task_f>::iterator _recipes_it;
	std::list<std::string> cgi_env_variables;
	std::map<std::string, std::list<t_task_f> > _meth_funs;
	int		_bytes_read;
	int		_bytes_write;
	int		_chunk_size;
	bool	_is_aborted;
	bool	_is_outfile_read;
	bool    _resp_body;
	std::string _resp_headers;
	size_t	content_length_h;
	pid_t                  _pid; // pid of CGI child
	std::list<char*>       _sending_buffer;
	std::list<ssize_t>     _len_send_buffer;
	std::list<std::string> _dir_listening_page;


	Callback(){};
	Callback(const Callback &src) { *this = src; }
	Callback (Socket &_socket, Header &request, std::list<Socket> &_sockets_list);
	void	exec();
	bool	is_over();
	void	chunk_reading(void);
	void	gen_resp_headers(void);
	void	send_respons(void);
	void	send_respons_body(void);

	void	init_meth_functions(void);
	std::list<t_task_f> init_recipe_put(void);
	void 	meth_put_open_fd(void);
	void 	meth_put_choose_in(void);
	void    meth_put_write_body(void);

	std::list<t_task_f> init_recipe_cgi(void);
	void 	meth_cgi_init_meta(void);
	void 	meth_cgi_init_http(void);
	void 	meth_cgi_save_client_in(void);
	void 	meth_cgi_launch(void);
	void 	meth_cgi_wait(void);
	void 	meth_cgi_send_http(void);
	void 	meth_cgi_send_resp(void);

	std::list<t_task_f>	init_recipe_get(void);
	void meth_get_request_is_valid(void);

	std::list<t_task_f>	init_recipe_head(void);
	std::list<t_task_f>	init_recipe_post(void);
	void	read_body_post(void);

	std::list<t_task_f>	init_recipe_trace(void);
	void	read_client_to_tmpfile(void);
	std::list<t_task_f>	init_recipe_delete(void);
	void meth_delete_request_is_valid(void);
	void meth_delete_remove(void);
	std::list<t_task_f>	init_recipe_options(void);
	void gen_resp_header_options(void);

	std::list<t_task_f>	init_error_request(void);
	void gen_error_header_and_body(void);
	bool if_error_page_exist(void);
	void send_error_page(void);

	int	remove_directory(const char *path);
	void init_socket(Socket &_socket);
	void init_request_header(Header &_request);

	std::list<Location>::iterator
	server_find_route (std::list<Location>::iterator &it, std::list<Location>::iterator &ite);
	void	server_init_route(std::list<Location> location);
	bool	method_allow();
	std::string	find_index_if_exist(void);

};

// callback_utils
std::string get_date(void);
void grh_add_headers(std::list<std::string> &headers, Callback &cb);
std::string lststr_to_strcont(std::list<std::string> const &lst, std::string sep);
int launch_panic(char **envp, char **args, char *bin_path);
bool	host_exist(std::list<char*> &client_buffer);
char    *concate_list_str(std::list<char*> &buffer);
size_t  lststr_len(std::list<std::string> const &lst, std::string const sep);
std::list<std::string>  gen_listening(std::string dir_path);
std::string ft_basename(std::string const path);
std::list<std::string> get_mid_page(char const *dir_path);
std::string get_top_page(std::string dir_path_request);
bool    is_buffer_crlf(std::list<char*> &buffer);
int read_chunk_client(int client_fd, std::list<char*> &buffer, std::list<ssize_t> &len_buf_parts);
int parse_chunk_size(std::list<char*> &buffer, int &chunk_size, std::list<ssize_t> &len_buf_parts);
unsigned int hextodec(char const *hexa);
int getdecfromchar(char c);
int parse_chunk_data(std::list<char*> &buffer, int &chunk_size, TmpFile &tmpfile, std::list<ssize_t> &len_buf_parts);
int parse_chunk_data(std::list<char*> &buffer, int chunk_size, int pipe_fd, std::list<ssize_t> &len_buf_parts);
bool is_crlf_part_first(std::list<char*> &buffer);
bool is_str_hex(char const *str);
char    *cgitohttp(TmpFile *tmpfile, size_t *status_code);
std::string get_content_len(size_t tmpfile_size, int fd);
size_t         get_headers_len(int fd);
std::string get_status(std::string filename, size_t *status_code);
std::string get_status_line(int code);
std::string get_status_msg(int code);
std::string  msg_server_error(int code);
std::string  msg_client_error(int code);
std::string  msg_redirection_error(int code);
std::string  msg_successful(int code);
std::string ft_dirname(std::string const path);
char    **lststr_to_strs(std::list<std::string> lst);
std::string lststr_to_str(std::list<std::string> const &lst, std::string sep);
char **ft_panic(char **start, char **curr);
std::string get_err_page(int code);
std::string get_content_length(int content_length);
char    *strcont_to_str(std::string str);



#endif