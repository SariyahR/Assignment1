#include "useful_functions.h"
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <stdint.h>


/* This function writes len bytes from the buffer buf
   to the file descriptor fd.

   It uses write() to accomplish this job.

   It returns 0 if everything went well.
   It returns a negative number (-1) otherwise.
*/
int my_write(int fd, const void *buf, size_t len) {
  size_t bytes_to_be_written;
  size_t bytes_already_written;
  size_t bytes_written_this_time;
  ssize_t write_res;

  /* Loop until all bytes have been written */
  for (bytes_already_written = (size_t) 0, bytes_to_be_written = len;
       bytes_to_be_written > (size_t) 0;
       bytes_already_written += bytes_written_this_time,
       bytes_to_be_written -= bytes_written_this_time) {
    
    /* Using write() function.

       Returns the number of bytes actually written if sucessful.
       Returns -1 if an error occurs.
    */
    write_res = write(fd, &(((char *)buf)[bytes_already_written]),
		      bytes_to_be_written);
    if (write_res < (ssize_t) 0) {
      /* Condition failure, write() returns a negative number*/
      return -1;
    }
    bytes_written_this_time = write_res;
  }
  return 0;
}


/* Add a nice description here later, please Sariyah :)*/
void __clean_up_memory(char *current_line, char **lines,
			      size_t *lines_length, size_t lines_len) {
  size_t i;
  
  if (current_line != NULL) free(current_line);
  if (lines != NULL) {
    for (i=((size_t) 0); i<lines_len; i++) {
      free(lines[i]);
    }
    free(lines);
    free(lines_length);
  }
}


/* This function reads the lines from standard input
   and then stores the lines in an array, as well as
   an array containing the lengths of each line.
*/
int get_lines_from_standard_input(char ***lines_ptr, size_t **lines_lengths_ptr,
				  size_t *lines_total_ptr) {
  char buffer[BUFFER_SIZE];
  ssize_t read_res;
  size_t amount_new_chars, i;
  char c;
  char *current_line;
  size_t current_line_size;
  size_t current_line_len;
  char *ptr;
  size_t tmp;
  size_t lines_size;
  char **ptrt;
  size_t *ptrtt;
  char **lines;
  size_t *lines_lengths;
  size_t lines_total;

  lines = NULL;
  lines_lengths = NULL;
  lines_size = (size_t) 0;
  lines_total = (size_t) 0;
  current_line = NULL;
  current_line_size = (size_t) 0;
  current_line_len = (size_t) 0;


  for (;;) {
    /* Read the next chunk of data from standard input */
    read_res = read(0, buffer, sizeof(buffer));

    /* Handle errors */
    if (read_res < ((ssize_t) 0)) {
      fprintf(stderr, "Error while reading: %s\n", strerror(errno));
      /* Deallocate everything we allocated */
      __clean_up_memory(current_line, lines, lines_lengths, lines_total);
      return 1;
    }

    /* If we hit EOF, break out of the read loop */
    if (read_res == ((ssize_t) 0)) 
      break;

    /* Here, we have at least 1 new character */
    amount_new_chars = (size_t) read_res;

    /* Go over all new characters, checking if they start new lines or
       if we need to add them to the current line.
    */
    for (i=((size_t) 0); i<amount_new_chars; i++) {
      c = buffer[i];

      /* Put the character into the current line, even if it is a new
	 line character.
      */
      if ((current_line_len + ((size_t) 1)) > current_line_size) {
	/* We need to allocate memory or the reallocate memory */
	if (current_line == NULL) {
	  /* This is the first allocation for this line 
	     
	     Let's allocate a default length.

	  */
	  current_line_size = FIRST_ALLOCATION_SIZE;
	  ptr = (char *) malloc(current_line_size);
	  if (ptr == NULL) {
	    fprintf(stderr, "Error at allocating memory.\n");
	    /* Deallocate everything we allocated */
	    __clean_up_memory(current_line, lines, lines_lengths, lines_total);
	    return 1;
	  }
	  current_line = ptr;
	} else {
	  tmp = current_line_size + current_line_size;
	  if (tmp < current_line_size) {
	    /* Overflow on our multiplication by 2 */
	    fprintf(stderr, "Error: this system cannot handle lines this long.");
	    /* Deallocate everything we allocated */
	    __clean_up_memory(current_line, lines, lines_lengths, lines_total);
	    return 1;
	  }
	  current_line_size = tmp;
	  ptr = (char *) realloc(current_line, current_line_size);
	  if (ptr == NULL) {
	    fprintf(stderr, "Error at allocating memory.\n");
	    /* Deallocate everything we allocated */
	    __clean_up_memory(current_line, lines, lines_lengths, lines_total);
	    return 1;
	  }
	  current_line = ptr;
	}
      }

      /* Here, we know that we have space for at least one 
	 character in current_line and that current_line has been 
	 allocated as needed.

	 We put the character c into the current line and 
	 increment the current line's length.
      */
      current_line[current_line_len] = c;
      current_line_len++;

      /* If the current character is a newline ('\n'), 
	 we need to end the line and add the line into 
	 an array of all lines.
      */
      if (c == '\n') {
	if ((lines_total + ((size_t) 1)) > lines_size) {
	  /* We need to allocate or reallocate memory */
	  if (lines == NULL) {
	    /* This is the first allocation */
	    lines_size = FIRST_ALLOCATION_LINE_ARRAY_SIZE;
	    lines = (char **) calloc(lines_size, sizeof(char *));
	    if (lines == NULL) {
	      fprintf(stderr, "Error at allocating memory.\n");
	      /* Deallocate everything we allocated */
	      __clean_up_memory(current_line, lines, lines_lengths, lines_total);
	      return 1;
	    }
	    lines_lengths = (size_t *) calloc(lines_size, sizeof(size_t));
	    if (lines_lengths == NULL) {
	      fprintf(stderr, "Error at allocating memory.\n");
	      /* Deallocate everything we allocated */
	      __clean_up_memory(current_line, lines, lines_lengths, lines_total);
	      return 1;
	    }
	  } else {
	    /* This is a reallocation */
	    tmp = lines_size + lines_size;
	    if (tmp < lines_size) {
	      /* Overflow on our multiplication by 2 */
	      fprintf(stderr, "Error: this system cannot handle that many lines.");
	      /* Deallocate everything we allocated */
	      __clean_up_memory(current_line, lines, lines_lengths, lines_total);
	      return 1;
	    }
	    lines_size = tmp;
	    ptrt = (char **) reallocarray(lines, lines_size, sizeof(char *));
	    if (ptrt == NULL) {
	      fprintf(stderr, "Error at allocating memory.\n");
	      /* Deallocate everything we allocated */
	      __clean_up_memory(current_line, lines, lines_lengths, lines_total);
	      return 1;
	    }
	    lines = ptrt;
	    ptrtt = (size_t *) reallocarray(lines_lengths, lines_size, sizeof(size_t));
	    if (ptrtt == NULL) {
	      fprintf(stderr, "Error at allocating memory.\n");
	      /* Deallocate everything we allocated */
	      __clean_up_memory(current_line, lines, lines_lengths, lines_total);
	      return 1;
	    }
	    lines_lengths = ptrtt;
	  }
	}
	
	/* Here, we have an array of lines allocated, into 
	   which we can put our current line.
	*/
	lines[lines_total] = current_line;
	lines_lengths[lines_total] = current_line_len;
	lines_total++;
	current_line = NULL;
	current_line_len = (size_t) 0;
	current_line_size = (size_t) 0;
      }
    }
  }

  /* Here, we have read in all lines from the input.

     Before spitting out all lines in reverse order on standard 
     output, we need to handle one special case.

     If the current line is started but not finished by a newline,
     we need to add that new line to the line and add the line to 
     the array of lines.
     
  */
  if (current_line != NULL) {
    /* The current line is started. Add a new line and the line to the
       lines array. 
    */
    if ((current_line_len + ((size_t) 1)) > current_line_size) {
      /* We need to allocate memory or the reallocate memory */
      if (current_line == NULL) {
	/* This is the first allocation for this line 
	     
	   Let's allocate a default length.

	*/
	current_line_size = FIRST_ALLOCATION_SIZE;
	ptr = (char *) malloc(current_line_size);
	if (ptr == NULL) {
	  fprintf(stderr, "Error at allocating memory.\n");
	  /* Deallocate everything we allocated */
	  __clean_up_memory(current_line, lines, lines_lengths, lines_total);
	  return 1;
	}
	current_line = ptr;
      } else {
	tmp = current_line_size + current_line_size;
	if (tmp < current_line_size) {
	  /* Overflow on our multiplication by 2 */
	  fprintf(stderr, "Error: this system cannot handle lines this long.");
	  /* Deallocate everything we allocated */
	  __clean_up_memory(current_line, lines, lines_lengths, lines_total);
	  return 1;
	}
	current_line_size = tmp;
	ptr = (char *) realloc(current_line, current_line_size);
	if (ptr == NULL) {
	  fprintf(stderr, "Error at allocating memory.\n");
	  /* Deallocate everything we allocated */
	  __clean_up_memory(current_line, lines, lines_lengths, lines_total);
	  return 1;
	}
	current_line = ptr;
      }
    }

    /* Here, we know that we have space for at least one 
       character in current_line and that current_line has been 
       allocated as needed.

       We put a new line character into the current line and 
       increment the current line's length.
    */
    current_line[current_line_len] = '\n';
    current_line_len++;

    /* Put the last line into the array of lines */
    if ((lines_total + ((size_t) 1)) > lines_size) {
      /* We need to allocate or reallocate memory */
      if (lines == NULL) {
	/* This is the first allocation */
	lines_size = FIRST_ALLOCATION_LINE_ARRAY_SIZE;
	lines = (char **) calloc(lines_size, sizeof(char *));
	if (lines == NULL) {
	  fprintf(stderr, "Error at allocating memory.\n");
	  /* Deallocate everything we allocated */
	  __clean_up_memory(current_line, lines, lines_lengths, lines_total);
	  return 1;
	}
	lines_lengths = (size_t *) calloc(lines_size, sizeof(size_t));
	if (lines_lengths == NULL) {
	  fprintf(stderr, "Error at allocating memory.\n");
	  /* Deallocate everything we allocated */
	  __clean_up_memory(current_line, lines, lines_lengths, lines_total);
	  return 1;
	}
      } else {
	/* This is a reallocation */
	tmp = lines_size + lines_size;
	if (tmp < lines_size) {
	  /* Overflow on our multiplication by 2 */
	  fprintf(stderr, "Error: this system cannot handle that many lines.");
	  /* Deallocate everything we allocated */
	  __clean_up_memory(current_line, lines, lines_lengths, lines_total);
	  return 1;
	}
	lines_size = tmp;
	ptrt = (char **) reallocarray(lines, lines_size, sizeof(char *));
	if (ptrt == NULL) {
	  fprintf(stderr, "Error at allocating memory.\n");
	  /* Deallocate everything we allocated */
	  __clean_up_memory(current_line, lines, lines_lengths, lines_total);
	  return 1;
	}
	lines = ptrt;
	ptrtt = (size_t *) reallocarray(lines_lengths, lines_size, sizeof(size_t));
	if (ptrtt == NULL) {
	  fprintf(stderr, "Error at allocating memory.\n");
	  /* Deallocate everything we allocated */
	  __clean_up_memory(current_line, lines, lines_lengths, lines_total);
	  return 1;
	}
	lines_lengths = ptrtt;
      }
    }
	
    /* Here, we have an array of lines allocated, into 
       which we can put our current line.
    */
    lines[lines_total] = current_line;
    lines_lengths[lines_total] = current_line_len;
    lines_total++;
    current_line = NULL;
    current_line_len = (size_t) 0;
    current_line_size = (size_t) 0;
  }

  /* Here, we have the array lines of lines. Each line has a length
     that is stored in lines_length.
  */
  if (current_line != NULL) free(current_line);

  *lines_ptr = lines;
  *lines_lengths_ptr = lines_lengths;
  *lines_total_ptr = lines_total;
  
  return 0;
}


/* This function prints a message to tell the user how
   to call head or tail correctly after they have called it
   incorrrectly.
*/
void print_error_message_badly_formed_call(const char *head_or_tail) {
  printf("\nThis is not a well-formed call to %s.\n", head_or_tail);
  printf("Here are some examples of valid calls:\n");
  printf("./%s -n 42 nanpa\n", head_or_tail);
  printf("./%s nanpa -n 42\n", head_or_tail);
  printf("./%s -n 42\n", head_or_tail);
  printf("./%s nanpa\n", head_or_tail);
  printf("./%s\n", head_or_tail);
}


/* This function converts a string to a number */
ssize_t convert_from_string_to_number(const char *str, char **endptr) {
  ssize_t result = 0;
  int sign = 1;

  /* Skip leading whitespaces */
  while (*str == ' ') {
    str++;
  }

  /* Check for the sign */
  if (*str == '-') {
    sign = -1;
    str++;
  }

  /* Convert digits to numbers */
  while (*str >= '0' && *str <= '9') {
    /* Check for overflow */
    if (result > (SIZE_MAX - (*str - '0')) / 10) {
      /* Overflow detected */
      if (endptr != NULL) {
	*endptr = (char *)str;
      }
      return sign == 1 ? SIZE_MAX : 0; 
    }
    result = result * 10 + (*str - '0');
    str++;
  }
  /* Set endptr to point to the character after the last
     digit found
  */
  if (endptr != NULL) {
    *endptr = (char *)str;
  }

  return result * sign;
}


int print_certain_number_of_lines(size_t num_lines_to_print,
				  bool starts_from_beginning) {
  
   /*Process input based on the number of lines specified
      For now, let's just print out the number of lines */
    printf("Number of lines: %zu\n", num_lines_to_print);

    size_t i, k, start, stop;

    char **lines = NULL;
    size_t *lines_lengths = NULL;
    size_t lines_total = 0;

    /* Get the line data struct with the info */
    get_lines_from_standard_input(&lines, &lines_lengths, &lines_total);


    if (starts_from_beginning) {
      start = 0;
      stop = num_lines_to_print;
    } else {
      start = lines_total - num_lines_to_print;
      stop =lines_total;
    }
    
    /* We need to write the first num_lines lines to standard out.*/
    for (k = start; k < stop; k++) {
        if (my_write(1, lines[k], lines_lengths[k]) < 0) {
	fprintf(stderr, "Error while reading: %s\n", strerror(errno));

	/* Deallocate everything we allocated */
	for (i = 0; i < lines_total; i++) {
	  free(lines[i]);
	}
	free(lines);
	free(lines_lengths);
	return 1;
      }
    }

    // Free memory allocated for lines and lines_length
   for (i = 0; i < lines_total; i++) {
     free(lines[i]);
   }
   free(lines);
   free(lines_lengths);
   return 0;
  
}
