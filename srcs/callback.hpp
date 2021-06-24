#ifndef CALLBACK_HPP
#define CALLBACK_HPP
#include "Webserv.hpp"








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



	Callback() { }
	Callback(Socket socket) { }
	Callback (Socket &_socket, Header &request, std::list<Socket> &_sockets_list) {
		this->_fd_body = 0;
		this->_host = false;
		this->_tmpfile = nullptr;
		this->_bytes_read = 0;
		this->_chunk_size = -1;
		this->_bytes_write = 0;
		this->socket_list = _sockets_list;
		this->_is_aborted = false;
		this->_out_tmpfile = nullptr;
		this->_is_outfile_read = false;
		this->request.original_path = request.path;
		init_socket(socket);                 // Init client socket variables
		init_request_header(request);          // Init request headers
		if (this->socket.server) {
			this->server = Server(*this->socket.server);
			server_init_route(this->server.location);
		}
		if (this->server.client_max_body_size != -1 &&
			this->request.content_length > (size_t)this->server.client_max_body_size) {
			this->request.status_code = 413;
		}
		if (this->server.fastcgi_pass.empty() && method_allow()) {  // CGI case
			_recipes = init_recipe_cgi();
		} else {                                // Init recipes
			init_meth_functions();
			_recipes = meth_funs[this->method];
		}
		if (_recipes.empty() == true) {         // Case when methods is not known
			_recipes = init_error_request();
		}
		_recipes_it = _recipes.begin();
		return ;
	}
	void exec() { }
	bool is_over() { }
	void    init_meth_functions(void) {
		if (this->request.status_code / 100 != 2)
			return ;
		if (this->request.host.empty()) {
			this->request.status_code = 400;
			return ;
		}
		if (method_allow() == false)
			return ;
		_meth_funs["PUT"] = init_recipe_put();
		_meth_funs["GET"] = init_recipe_get();
		_meth_funs["HEAD"] = init_recipe_head();
		_meth_funs["POST"] = init_recipe_post();
		_meth_funs["TRACE"] = init_recipe_trace();
		_meth_funs["DELETE"] = init_recipe_delete();
		_meth_funs["OPTIONS"] = init_recipe_options();
	}
	void 	chunk_reading(void);
	void 	gen_resp_headers(void);
	void 	send_respons(void);
	void 	send_respons_body(void);

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
	std::list<t_task_f>	init_recipe_options(void);




	void init_socket(Socket &_socket) {
		this->socket = Socket(_socket);
		this->content_length_h = 0;
		this->client_buffer = _socket.buffer;
	}
	void init_request_header(Header &_request) {
		this->_resp_body = false;
		request = Header(_request);
	}

	std::list<Location>::iterator
	server_find_route
			(std::list<Location>::iterator &it,
			 std::list<Location>::iterator &ite) {
		std::list<Location>::iterator     it_find;
		std::string                         tmp_path;
		size_t                              i = 0;
		int                                 status = 0;

		it_find = ite;
		if ((i = this->request.path.find_first_of('/', 1)) != std::string::npos)
			tmp_path.insert(0, this->request.path, 0, i);
		else
			tmp_path = this->request.path;
		for (; it != ite; ++it)
		{
			if (strncmp((*it).route.c_str(), ".", 1) == 0) { // location management by file
				std::string tmp_string;
				size_t found = this->request.path.find_last_of('.');
				if (found != std::string::npos) {
					tmp_string.insert(0, this->request.path, found, this->request.path.length());

					if ((strncmp(tmp_string.c_str(), (*it).route.c_str(),
								 tmp_string.length())) == 0 &&
						(tmp_string.length() == (*it).route.length())) {
						it_find = it;
						break ;
					}
				}
			}
			if (strcmp((*it).route.c_str(), "/") == 0)
				it_find = it;
			if ((strncmp(tmp_path.c_str(), (*it).route.c_str(),
						 tmp_path.length())) == 0 &&
				(tmp_path.length() == (*it).route.length())) {
				it_find = it;
				status = 1;
			}
		}
		if (status == 1 && i != std::string::npos)
			this->request.path.erase(0, i);
		if (status == 1 && i == std::string::npos)
			this->request.path.clear();
		return (it_find);
	}

	void        server_init_route(std::list<Location> location) {
		std::list<Location>::iterator     it, ite;

		it = location.begin();
		ite = location.end();
		it = server_find_route(it, ite);
		if (it != ite) {
			server.client_max_body_size = (*it).client_max_body_size;
			if ((*it).index.begin() != (*it).index.end())
				server.index = (*it).index;
			if ((*it).methods.empty() == false)
				server.methods = (*it).methods;
			if ((*it).root.empty() == false)
				server.root = (*it).root;
			if ((*it).autoindex.empty() == false)
				server.autoindex = (*it).autoindex;
			if ((*it).fastcgi_param.empty() == false)
				server.fastcgi_param = (*it).fastcgi_param;
			if ((*it).error_page.empty() == false)
				server.error_page = (*it).error_page;
			if ((*it).fastcgi_pass.empty() == false)
				server.fastcgi_pass = (*it).fastcgi_pass;
		}
	}
	bool    method_allow();
	std::string	find_index_if_exist(void);

};

// callback_utils
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
char **ft_panic(char **start, char **curr);










class    TaskQueue {
public:
	void    exec_task() { }
	void    push(std::list<Socket> &clients) {
		Callback cb_temp;
		std::list<Socket>::iterator it_sockets = clients.begin();
		std::list<Socket>::iterator ite_sockets = clients.end();

		while (it_sockets != ite_sockets) {
			if (it_sockets->is_header_read && !it_sockets->is_callback_created &&
				it_sockets->is_cache_resp == false) {
				cb_temp = Callback((*it_sockets),
								   (it_sockets->headers), clients);
				it_sockets->is_callback_created = true;
				_tasks.push(cb_temp);
			}
			++it_sockets;
		}
	}
	size_t  size(void) const { return _tasks.size(); }

private:
	std::queue<Callback>	_tasks;
};





#endif