//
// Created by Jamis Heat on 6/21/21.
//

#include "Webserv.hpp"

/* REMOVE_CLIENT
 * This function will remove the given client from the list of client cause of
 * EOF/connection closed or recv return an error.
 * Bytes_read is the return of the read/recv on client_fd.
 */
void remove_client(std::list<Socket> &socket_list, int response_fd,
				   ssize_t bytes_read) {
	std::string                   closing_cause;
	std::list<char*>::iterator    it_buf;
	std::list<char*>::iterator    ite_buf;
	std::list<Socket>::iterator client = socket_list.begin();
	std::list<Socket>::iterator client_ite = socket_list.end();

	while (client != client_ite) {
		if (client->response_fd == response_fd)
			break;
		++client;
	}
	if (bytes_read == -1)
		closing_cause = "read error.";
	else if (bytes_read == 0)
		closing_cause = "client closed the connection.";
	else
		closing_cause = "other error";//TODO if necessary add all kinds of errors here
	std::cerr <<                                          \
        "[" << client->response_fd << "] " <<               \
        "Connection closed due to : " << closing_cause << \
    std::endl;
	it_buf = client->buffer.begin();
	ite_buf = client->buffer.end();
	while (it_buf != ite_buf) {
		free(*it_buf);
		client->buffer.erase(it_buf++);
	}
	close(client->response_fd);
	socket_list.erase(client);
	return ;
}