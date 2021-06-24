//
// Created by Frenken Flores on 6/24/21.
//
#pragma once
#include "webserv.hpp"

void                 TmpFile::_update_nextnameprefix(void) {
	std::string::iterator last = --TmpFile::_nextnameprefix.end();

	if (TmpFile::_nextnameprefix.size() == 80)
		TmpFile::_nextnameprefix = 'A';
	else if (*last == 'Z')
		TmpFile::_nextnameprefix += 'A';
	else
		++(*last);
}
std::string          TmpFile::_get_next_name(void) {
	return (TmpFile::_path + TmpFile::_nextnameprefix + ".tmp");
}

bool                 TmpFile::_does_nextfile_exist(void) {
	struct stat     buff;
	std::string     nextfile = _get_next_name();

	stat(nextfile.c_str(), &buff);
	if (errno == 0)
		return (true);
	if (errno != ENOENT)
		std::perror("stat");
	errno = 0;
	return (false);
}