#ifndef MY_WRITE_H
#define MY_WRITE_H

#include <stddef.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>

#define BUFFER_SIZE                      (((size_t) 4096))
#define FIRST_ALLOCATION_SIZE            (((size_t) 16))
#define FIRST_ALLOCATION_LINE_ARRAY_SIZE (((size_t) 16))

typedef struct {
  char **lines;
  size_t *lines_length;
  size_t lines_len;
} LineData;


int my_write(int fd, const void *buf, size_t len);

void __clean_up_memory(char *current_line, char **lines,
		       size_t *lines_length, size_t lines_len);

LineData *get_lines_from_standard_input();

void print_error_message_badly_formed_call(const char *head_or_tail);

ssize_t convert_from_string_to_number(const char *str, char **endptr);

#endif
