#include "utils.hpp"
#include <fstream>
#define TEMPLATE "<html>\r\n"                                               \
                 "<head><title>CODE MSG</title></head>\r\n"                 \
                 "<body><center><h1>CODE MSG</h1></center>\r\n"             \
                 "<hr><center>Error</center></body>\r\n"     \
                 "</html>\r\n"
#define MSG_LEN 3
#define CODE_LEN 4

std::string get_err_page(int code, std::string path) {
    std::string str_code = std::to_string(code);
    size_t      cursor = 0;
    std::string page = TEMPLATE;
    std::fstream file;
    file.open(path);
    if (file.is_open()) {
        std::string  line;
        page.clear();
        while (std::getline(file, line)) {
            page += line;
        }
    } else {
        std::string status_message = get_status_msg(code);

        while ((cursor = page.find("CODE")) != std::string::npos) {
            page.replace(cursor, CODE_LEN, str_code);
        }
        while ((cursor = page.find("MSG")) != std::string::npos) {
            page.replace(cursor, MSG_LEN, status_message);
        }
    }
    return (page);
}