#include <sys/select.h>
#include <sys/socket.h>
#include <fcntl.h>

#include "utils.hpp"
#include "std_typedefs.hpp"

static void init_fdsets(std::list<Socket> const *lst, fd_set *r_set, fd_set *w_set) {
    std::list<Socket>::const_iterator it = lst->begin(), ite = lst->end();

    FD_ZERO(r_set);
    FD_ZERO(w_set);
    for (; it != ite; ++it) {
        if (it->client_fd == 0) {
            FD_SET(it->entry_socket, r_set);
        } else {
            FD_SET(it->client_fd, r_set);
            FD_SET(it->client_fd, w_set);
        }
    }
}

static int socket_max(std::list<Socket> const *const lst) {
    std::list<Socket>::const_iterator it = lst->begin(), ite = lst->end();
    int max = -1;

    for (; it != ite; ++it) {
        if (it->client_fd == 0 && it->entry_socket > max)
            max = it->entry_socket;
        else if (it->client_fd != 0 && it->client_fd > max)
            max = it->client_fd;
    }
    return (max);
}

static int update_socketlst(std::list<Socket> *const lst, fd_set *r_set,
                             fd_set *w_set) {
    std::list<Socket>::iterator it = lst->begin(), ite = lst->end();
    bool isset_r, isset_w;
    int flag = 0;

    for (; it != ite; ++it) {
        isset_r = FD_ISSET(it->client_fd, r_set);
        isset_w = FD_ISSET(it->client_fd, w_set);

        it->is_read_ready = isset_r;
        it->is_write_ready = isset_w;
        if ((flag & 1) == false && FD_ISSET(it->entry_socket, r_set))
            flag |= 1;
        if ((flag & 2) == false && isset_r && it->is_header_read == false)
            flag |= 2;
    }
    return (flag);
}


bool    ft_select(std::list<Socket> *const clients, s_similar_get_req *similar_req) {
    fd_set  r_fdset, w_fdset;
    struct timeval time = {2, 0};
    int     updated_flag;

    errno = 0;
    init_fdsets(clients, &r_fdset, &w_fdset);
    select(socket_max(clients) + 1, &r_fdset, &w_fdset, NULL, &time);
    updated_flag = update_socketlst(clients, &r_fdset, &w_fdset);
    if (errno == EAGAIN || errno == EINTR || (updated_flag & 1) == false) {
        errno = 0;
        return (updated_flag & 2);
    }
    else if (errno != 0)
        ft_error("select");
    std::list<Socket> new_clients;
    std::list<Socket>::iterator it = clients->begin(), ite = clients->end();
    socklen_t   socklen = sizeof(sockaddr);
    for (; it != ite; ++it) {
        if (it->client_fd != 0 || FD_ISSET(it->entry_socket, &r_fdset) == false)
            continue ;
        Socket nclient = *it;

        nclient.client_fd = accept(nclient.entry_socket, \
                            &nclient.client_addr, &socklen);
        reset_socket(&nclient);
        nclient.similar_req = similar_req;
        if (errno != 0)
            ft_error("accept");
        fcntl(nclient.client_fd, F_SETFL, O_NONBLOCK);
        new_clients.push_back(nclient);
    }
    clients->splice(clients->end(), new_clients);
    return (updated_flag & 2);
}
