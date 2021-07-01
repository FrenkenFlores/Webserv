//
// Created by FrenkenFlores on 01.07.2021.
//
#include <stdlib.h>
#include <string.h>
char	*substr(char const *s, unsigned int start, size_t len) {
	char *sub;
	size_t i;
	size_t s_len;

	i = 0;
	if (s == NULL)
		return (NULL);
	s_len = strlen(s);
	if ((sub = (char *) calloc(len + 1, sizeof(*sub))) == NULL)
		return (NULL);
	if (start < s_len)
		while (i < len && s[start])
			sub[i++] = s[start++];
	return (sub);
}