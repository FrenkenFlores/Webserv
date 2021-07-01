#include "Tmpfile.hpp"
#include "utils.hpp"
#include <sys/stat.h>
#include <fcntl.h>

Tmpfile::Tmpfile(void) : _fd(-1) {
    mkdir(Tmpfile::_path, 0777);
    if (errno != 0 && errno != EEXIST)
        ft_error("Tmpfile: mkdir");
    errno = 0;
    while (Tmpfile::_does_nextfile_exist())
        Tmpfile::_update_nextnameprefix();
    this->_filename = Tmpfile::_get_next_name();
    this->_fd = open(_filename.c_str(), O_CREAT | O_TRUNC | O_RDWR, S_IRWXU);
    fcntl(this->_fd, F_SETFL, O_NONBLOCK);
    if (errno != 0)
        ft_error("Tmpfile: open");
}

Tmpfile::Tmpfile(Tmpfile const &src) {
    *this = src;
}

Tmpfile::~Tmpfile(void) {
    if (this->_fd == -1)
        return ;
    close(this->_fd);
    unlink(this->_filename.c_str());
}

Tmpfile   &Tmpfile::operator=(Tmpfile const &rhs) {
    if (this == &rhs)
        return (*this);
    this->_filename = rhs._filename;
    this->_fd = rhs._fd;
    return (*this);
}

int const   &Tmpfile::get_fd(void) const { return (this->_fd); }

std::string const   &Tmpfile::get_filename(void) const {
    return (this->_filename);
}

size_t  Tmpfile::get_size(void) const {
    struct stat file_stat;

    fstat(this->get_fd(), &file_stat);
    return (file_stat.st_size);
}

bool    Tmpfile::is_read_ready(void) const {
    return (is_fd_read_ready(this->_fd));
}

bool    Tmpfile::is_write_ready(void) const {
    return (is_fd_write_ready(this->_fd));
}

void    Tmpfile::reset_cursor(void) {
    if (lseek(this->_fd, 0, SEEK_SET) != -1)
        return ;
    ft_error("lseek");
}

void    Tmpfile::_update_nextnameprefix(void) {
    std::string::iterator last = --Tmpfile::_nextnameprefix.end();

    if (Tmpfile::_nextnameprefix.size() == 80)
        Tmpfile::_nextnameprefix = 'A';
    else if (*last == 'Z')
        Tmpfile::_nextnameprefix += 'A';
    else
        ++(*last);
}

bool    Tmpfile::_does_nextfile_exist(void) {
    struct stat     buff;
    std::string     nextfile = Tmpfile::_get_next_name();

    stat(nextfile.c_str(), &buff);
    if (errno == 0)
        return (true);
    if (errno != ENOENT)
        ft_error("stat");
    errno = 0;
    return (false);
}

std::string Tmpfile::_get_next_name(void) {
    return (Tmpfile::_path + Tmpfile::_nextnameprefix + ".tmp");
}

char 			*Tmpfile::_path = (char*)"/tmp/webserv_tmp/";
std::string         Tmpfile::_nextnameprefix = "A";