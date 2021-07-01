#include <iostream>
#include <errno.h>
#include <sys/select.h>

static int  _select_fd(int fd) {
    fd_set  r_fdset;
    fd_set  w_fdset;
    int     res = 0;
    timeval tv = {0, 0};

    FD_ZERO(&r_fdset); FD_ZERO(&w_fdset);
    FD_SET(fd, &r_fdset); FD_SET(fd, &w_fdset);
    select(fd + 1, &r_fdset, &w_fdset, NULL, &tv);
    errno = 0;
    if (FD_ISSET(fd, &r_fdset))
        res |= 1;
    if (FD_ISSET(fd, &w_fdset))
        res |= 2;
    return (res);
}

bool    is_fd_read_ready(int fd) {
    return (_select_fd(fd) & 1);
}

bool    is_fd_write_ready(int fd) {
    return (_select_fd(fd) & 2);
}
