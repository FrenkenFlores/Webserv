#include <list>
#include <string>

static char **ft_panic(char **start, char **curr) {
    while (curr != start) {
        free(*curr);
        --curr;
    }
    free(start);
    return (NULL);
}

char    **lststr_to_strs(std::list<std::string> lst) {
    char **strs = NULL;
    char **i_strs = NULL;
    std::list<std::string>::iterator it = lst.begin();
    std::list<std::string>::iterator ite = lst.end();

    if (!(strs = (char**)malloc(sizeof(char*) * (lst.size() + 1))))
        return (NULL);
   i_strs = strs;
    while (it != ite) {
        if ((*i_strs = strdup(it->c_str())) == NULL)
            return (ft_panic(strs, i_strs));
        ++it;
        ++i_strs;
    }
    *i_strs = NULL;
    return (strs);
}
