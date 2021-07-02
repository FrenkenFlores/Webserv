#ifndef LOCATION_HPP
# define LOCATION_HPP

#include <string>
#include <list>
#include <map>

class    Location {
 public:

    Location(void);
    Location(Location const &src);
    Location &operator=(Location const &src);
    virtual ~Location();

    int             client_max_body_size;
    std::list<std::string>        index;
    std::list<std::string>        methods;
    std::string     root;
    std::string     route;
    std::string     autoindex;
    std::string     fastcgi_pass;
    std::map<std::string, std::string>     fastcgi_param;
    std::map<int, std::string>     error_page;
};

#endif
