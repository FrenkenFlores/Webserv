#include <string>
#include <iostream>
#include <stdlib.h>

void    ft_error(std::string const &src, std::string const &err) {
    std::cerr << src << ": " << err << std::endl;
    exit(1);
}
