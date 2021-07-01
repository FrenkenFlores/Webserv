#include <string>
#include <iostream>

std::string ft_basename(std::string const path) {
    std::string::const_iterator it_start = path.end();
    std::string::const_iterator it_end = path.end();

    if (path.size() == 0)
        return (std::string(""));
    --it_start;
    while (*it_start == '/' && it_start != path.begin()) {
        it_end = it_start;
        --it_start;
    }
    while (it_start != path.begin()) {
        if (*it_start == '/') {
            ++it_start;
            break ;
        }
        --it_start;
    }
    return (std::string(it_start, it_end));
}
