#ifndef LOCATION_HPP
# define LOCATION_HPP

#include <string>
#include "std_typedefs.hpp"

class    Location {
 public:

    Location(void);
    Location(Location const &src);
    Location &operator=(Location const &src);
    virtual ~Location();

    int             client_max_body_size;
    t_strlst        index;
    t_strlst        methods;
    std::string     root;
    std::string     route;
    std::string     autoindex;
    std::string     fastcgi_pass;
    t_cgi_param     fastcgi_param;
    t_error_page    error_page;
};

#endif
