#include <list>
#include <string>

std::string lststr_to_strcont(std::list<std::string> const &lst,
                              std::string sep) {
    std::string concatenate;

    for (std::list<std::string>::const_iterator it = lst.begin();
            it != lst.end(); ++it) {
        concatenate += *it;
        concatenate += sep;
    }
    if (concatenate.size() > 0)
        concatenate.erase(concatenate.size() - sep.size(), sep.size());
    return (concatenate);
}