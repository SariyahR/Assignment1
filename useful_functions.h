#ifndef MY_WRITE_H
#define MY_WRITE_H

#include <stddef.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <stdbool.h>

#define BUFFER_SIZE                      (((size_t) 4096))
#define FIRST_ALLOCATION_SIZE            (((size_t) 16))
#define FIRST_ALLOCATION_LINE_ARRAY_SIZE (((size_t) 16))
//this is for the read file function
#define CONTENT_DEFAULT_SIZE (((size_t) 16))

int my_write(int fd, const void *buf, size_t len);

void __clean_up_memory(char *current_line, char **lines,
		       size_t *lines_length, size_t lines_len);

int read_from_file(char *filename, char **content_ptr,
			size_t *content_len_ptr);

int get_lines_from_file(char *filename, char ***lines_ptr, size_t **lines_lengths_ptr,
                        size_t *lines_total_ptr);

int get_lines_from_standard_input(char ***lines, size_t **lines_lengths, size_t *lines_total);

void print_error_message_badly_formed_call(const char *head_or_tail);

ssize_t convert_from_string_to_number(const char *str, char **endptr);

int print_certain_number_of_lines(size_t num_lines_to_print,
				  bool starts_from_beginning, char **lines,
				  size_t *lines_lengths, size_t lines_total);



#endif
