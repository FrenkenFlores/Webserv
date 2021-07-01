#include "Address.hpp"

bool    operator==(Address const &a, Address const &b) {
    return (a.ip == b.ip && a.port == b.port);
}

bool    operator!=(Address const &a, Address const &b) {
    return (!(a == b));
}
