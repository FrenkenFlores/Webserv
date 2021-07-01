#ifndef ADDRESS_HPP
# define ADDRESS_HPP

# include <string>

struct  Address {
    int             port;
    std::string     ip;
};

bool    operator==(Address const &a, Address const &b);
bool    operator!=(Address const &a, Address const &b);

#endif
