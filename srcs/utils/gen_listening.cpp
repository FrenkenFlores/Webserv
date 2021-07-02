#include <list>
#include <string>
#include <iostream>

#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/errno.h>

#include "utils.hpp"

#define TOP_PAGE "<html>\n"                                      \
                 "<head><title>Index of /jpeg/</title></head>\n" \
                 "<body>\n"                                      \
                 "<h1>Index of REQ_PATH</h1>"                    \
                 "<hr><pre><a href=\"../\">../</a>"

static std::string get_top_page(std::string dir_path_request) {
    size_t      cursor;
    std::string top_page = TOP_PAGE;

    cursor = top_page.find("REQ_PATH");
    top_page.replace(cursor, 8, dir_path_request);
    return (top_page);
}

#define MID_TEMPLATE "<a href=\"ELEM_NAME\">ELEM_NAME</a>"
static std::list<std::string> get_mid_page(char const *dir_path) {
    DIR                    *curr_dir;
    size_t                 cursor;
    std::string            tmp_elem;
    std::string            tmp_elem_name;
    std::string            tmp_elem_path;
    struct stat            stat_curr_file;
    struct dirent          *it_dir;
    std::list<std::string> mid_page;

    errno = 0;
    if ((curr_dir = opendir(dir_path)) == NULL)
        throw std::logic_error(strerror(errno));
    while ((it_dir = readdir(curr_dir))) {
        tmp_elem = MID_TEMPLATE;
        tmp_elem_name = it_dir->d_name;
        if (tmp_elem_name == "." || tmp_elem_name == "..")
            continue ;
        tmp_elem_path = dir_path;
        tmp_elem_path += "/";
        tmp_elem_path += tmp_elem_name;
        if ((lstat(tmp_elem_path.c_str(), &stat_curr_file)) == -1)
            throw std::logic_error(std::string("lstat() ") + strerror(errno));
        if (S_ISDIR(stat_curr_file.st_mode) == true)
            tmp_elem_name += "/";
        while ((cursor = tmp_elem.find("ELEM_NAME")) != std::string::npos)
            tmp_elem.replace(cursor, strlen("ELEM_NAME"), tmp_elem_name);
        mid_page.push_back(tmp_elem);
    }
    closedir(curr_dir);
    return (mid_page);
}

#define BOT_HTML "</pre><hr>"                              \
                 "<center>webserv v6.66</center></body>" \
                 "</html>"

std::list<std::string>  gen_listening(std::string dir_path) {
    std::string             dir_name;
    std::list<std::string>  page;
    std::list<std::string>  mid_page;

    dir_name = ft_basename(dir_path);
    page.push_back(get_top_page(dir_name));
    try {
        mid_page = get_mid_page(dir_path.c_str());
    } catch (std::exception &e) {
        std::cerr << "gen_listening() : " << e.what() << std::endl;
        return (std::list<std::string>());
    }
    page.insert(page.end(), mid_page.begin(), mid_page.end());
    page.push_back(BOT_HTML);
    for (std::list<std::string>::iterator itb = page.begin(); itb != page.end(); itb++)
    	std::cout << *itb << std::endl;
    return (page);
}