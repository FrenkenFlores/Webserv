#include <string>
#include <iostream>

#include <stdlib.h>

char    *strcont_to_str(std::string str) {
    char *result;
    char *ptr_result;

    if (!(result = (char*)malloc(sizeof(char) * (str.size() + 1))))
        return (NULL);
    ptr_result = result;
    for (std::string::const_iterator it = str.begin(); it != str.end(); ++it) {
        *ptr_result = *it;
        ++ptr_result;
    }
    *ptr_result = '\0';
    return (result);
}