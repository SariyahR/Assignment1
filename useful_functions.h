#ifndef MY_WRITE_H
#define MY_WRITE_H

#include <stddef.h>

int my_write(int fd, const void *buf, size_t len);
void __clean_up_memory(char *current_line, char **lines,
		       size_t *lines_length, size_t lines_len);
void get_lines_from_standar_input();
#endif
