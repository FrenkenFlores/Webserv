#include <list>
#include <string>

unsigned int find_str_buffer(std::list<char*> *buffer, std::string to_find) {
    char const   *i_to_find = NULL;
    char const   *i_buf_parts = NULL;
    unsigned int final_len = 0;
    std::list<char*>::iterator it = buffer->begin();
    std::list<char*>::iterator ite = buffer->end();

    i_to_find = to_find.c_str();
    while (it != ite) {
        i_buf_parts = *it;
        while (*i_buf_parts) {
            if (*i_buf_parts == *i_to_find) {               // A same char saw
                ++i_to_find;
                if (*i_to_find == '\0') {                     // Find exit case
                    ++final_len;
                    return (final_len - to_find.size());
                }
            } else if (i_to_find != to_find.c_str()) {      // Find not complet
                i_to_find = to_find.c_str();
            }
            ++final_len;
            ++i_buf_parts;
        }
        ++it;
    }
    return (0);
}