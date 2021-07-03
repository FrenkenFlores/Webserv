#include "read_headers.hpp"
#include "../utils/utils.hpp"

static int  check_if_host_exist(std::string prefix, void *p) {
    if (prefix == "Host") {
        std::string *ptr = static_cast<std::string*>(p);
        if (ptr->empty() == false)
            return (1);
    }
    return (0);
}

int    parse_header(std::string line,
                    std::map<std::string, void *> *header_ptrs,
                    std::map<std::string, f_request_header> *header_parsers) {
    std::string     prefix;
    std::string     sep(":");

    prefix = get_word(line, line.begin(), sep);
    if (prefix == "Transfer-Encoding" || prefix == "Content-Length") {
        if (*((bool*)(*header_ptrs)["trans_mode_saw"]) == false)
            *((bool*)(*header_ptrs)["trans_mode_saw"]) = true;
        else
            return (1);
    }
    if (header_parsers->find(prefix) != header_parsers->end()) {
        if ((check_if_host_exist(prefix, (*header_ptrs)[prefix]) == 1))
            return (1);
        else
            (*header_parsers)[prefix](line, (*header_ptrs)[prefix]);
    }
    return (0);
}
