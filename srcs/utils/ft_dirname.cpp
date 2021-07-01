#include <string>

std::string ft_dirname(std::string const path) {
    std::string::const_iterator it_end = path.end();

    if (path.find("/") == std::string::npos)
        return (std::string("."));
    if (path == "/")
        return (std::string("/"));
    --it_end;
    while (*it_end == '/' && it_end != path.begin()) {
        --it_end;
    }
    while (it_end != path.begin()) {
        if (*it_end == '/') {
            break ;
        }
        --it_end;
    }
    return (std::string(path.begin(), it_end));
}