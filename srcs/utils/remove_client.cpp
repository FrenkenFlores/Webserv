#include "../classes/Callback.hpp"

void remove_client(std::list<Socket> *clients, int client_fd,
                   ssize_t bytes_read) {
    std::string                   closing_cause;
    std::list<char*>::iterator    it_buf;
    std::list<char*>::iterator    ite_buf;
    std::list<Socket>::iterator client = clients->begin();
    std::list<Socket>::iterator client_ite = clients->end();

    while (client != client_ite) {
        if (client->client_fd == client_fd)
            break;
        ++client;
    }
    if (bytes_read == -1)
        closing_cause = "read error.";
    else if (bytes_read == 0)
        closing_cause = "client closed the connection.";
    else
        closing_cause = get_status_msg(bytes_read);
    std::cerr <<                                          \
        "[" << client->client_fd << "] " <<               \
        "Connection closed due to : " << closing_cause << \
    std::endl;
    it_buf = client->buffer.begin();
    ite_buf = client->buffer.end();
    while (it_buf != ite_buf) {
        free(*it_buf);
        client->buffer.erase(it_buf++);
    }
    close(client->client_fd);
    clients->erase(client);
    return ;
}
