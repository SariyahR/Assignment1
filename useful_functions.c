#include "useful_functions.h"
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>


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


LineData *get_lines_from_standard_input() {
  char buffer[BUFFER_SIZE];
  ssize_t read_res;
  size_t amount_new_chars, i, k;
  char c;
  char *current_line;
  size_t current_line_size;
  size_t current_line_len;
  char *ptr;
  size_t tmp;
  char **lines;
  size_t *lines_length;
  size_t lines_size;
  size_t lines_len;
  char **ptrt;
  size_t *ptrtt;

  lines = NULL;
  lines_length = NULL;
  lines_size = (size_t) 0;
  lines_len = (size_t) 0;
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
      __clean_up_memory(current_line, lines, lines_length, lines_len);
      return NULL;
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
	    __clean_up_memory(current_line, lines, lines_length, lines_len);
	    return NULL;
	  }
	  current_line = ptr;
	} else {
	  tmp = current_line_size + current_line_size;
	  if (tmp < current_line_size) {
	    /* Overflow on our multiplication by 2 */
	    fprintf(stderr, "Error: this system cannot handle lines this long.");
	    /* Deallocate everything we allocated */
	    __clean_up_memory(current_line, lines, lines_length, lines_len);
	    return NULL;
	  }
	  current_line_size = tmp;
	  ptr = (char *) realloc(current_line, current_line_size);
	  if (ptr == NULL) {
	    fprintf(stderr, "Error at allocating memory.\n");
	    /* Deallocate everything we allocated */
	    __clean_up_memory(current_line, lines, lines_length, lines_len);
	    return NULL;
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
	if ((lines_len + ((size_t) 1)) > lines_size) {
	  /* We need to allocate or reallocate memory */
	  if (lines == NULL) {
	    /* This is the first allocation */
	    lines_size = FIRST_ALLOCATION_LINE_ARRAY_SIZE;
	    lines = (char **) calloc(lines_size, sizeof(char *));
	    if (lines == NULL) {
	      fprintf(stderr, "Error at allocating memory.\n");
	      /* Deallocate everything we allocated */
	      __clean_up_memory(current_line, lines, lines_length, lines_len);
	      return NULL;
	    }
	    lines_length = (size_t *) calloc(lines_size, sizeof(size_t));
	    if (lines_length == NULL) {
	      fprintf(stderr, "Error at allocating memory.\n");
	      /* Deallocate everything we allocated */
	      __clean_up_memory(current_line, lines, lines_length, lines_len);
	      return NULL;
	    }
	  } else {
	    /* This is a reallocation */
	    tmp = lines_size + lines_size;
	    if (tmp < lines_size) {
	      /* Overflow on our multiplication by 2 */
	      fprintf(stderr, "Error: this system cannot handle that many lines.");
	      /* Deallocate everything we allocated */
	      __clean_up_memory(current_line, lines, lines_length, lines_len);
	      return NULL;
	    }
	    lines_size = tmp;
	    ptrt = (char **) reallocarray(lines, lines_size, sizeof(char *));
	    if (ptrt == NULL) {
	      fprintf(stderr, "Error at allocating memory.\n");
	      /* Deallocate everything we allocated */
	      __clean_up_memory(current_line, lines, lines_length, lines_len);
	      return NULL;
	    }
	    lines = ptrt;
	    ptrtt = (size_t *) reallocarray(lines_length, lines_size, sizeof(size_t));
	    if (ptrtt == NULL) {
	      fprintf(stderr, "Error at allocating memory.\n");
	      /* Deallocate everything we allocated */
	      __clean_up_memory(current_line, lines, lines_length, lines_len);
	      return NULL;
	    }
	    lines_length = ptrtt;
	  }
	}
	
	/* Here, we have an array of lines allocated, into 
	   which we can put our current line.
	*/
	lines[lines_len] = current_line;
	lines_length[lines_len] = current_line_len;
	lines_len++;
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
	  __clean_up_memory(current_line, lines, lines_length, lines_len);
	  return NULL;
	}
	current_line = ptr;
      } else {
	tmp = current_line_size + current_line_size;
	if (tmp < current_line_size) {
	  /* Overflow on our multiplication by 2 */
	  fprintf(stderr, "Error: this system cannot handle lines this long.");
	  /* Deallocate everything we allocated */
	  __clean_up_memory(current_line, lines, lines_length, lines_len);
	  return NULL;
	}
	current_line_size = tmp;
	ptr = (char *) realloc(current_line, current_line_size);
	if (ptr == NULL) {
	  fprintf(stderr, "Error at allocating memory.\n");
	  /* Deallocate everything we allocated */
	  __clean_up_memory(current_line, lines, lines_length, lines_len);
	  return NULL;
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
    if ((lines_len + ((size_t) 1)) > lines_size) {
      /* We need to allocate or reallocate memory */
      if (lines == NULL) {
	/* This is the first allocation */
	lines_size = FIRST_ALLOCATION_LINE_ARRAY_SIZE;
	lines = (char **) calloc(lines_size, sizeof(char *));
	if (lines == NULL) {
	  fprintf(stderr, "Error at allocating memory.\n");
	  /* Deallocate everything we allocated */
	  __clean_up_memory(current_line, lines, lines_length, lines_len);
	  return NULL;
	}
	lines_length = (size_t *) calloc(lines_size, sizeof(size_t));
	if (lines_length == NULL) {
	  fprintf(stderr, "Error at allocating memory.\n");
	  /* Deallocate everything we allocated */
	  __clean_up_memory(current_line, lines, lines_length, lines_len);
	  return NULL;
	}
      } else {
	/* This is a reallocation */
	tmp = lines_size + lines_size;
	if (tmp < lines_size) {
	  /* Overflow on our multiplication by 2 */
	  fprintf(stderr, "Error: this system cannot handle that many lines.");
	  /* Deallocate everything we allocated */
	  __clean_up_memory(current_line, lines, lines_length, lines_len);
	  return NULL;
	}
	lines_size = tmp;
	ptrt = (char **) reallocarray(lines, lines_size, sizeof(char *));
	if (ptrt == NULL) {
	  fprintf(stderr, "Error at allocating memory.\n");
	  /* Deallocate everything we allocated */
	  __clean_up_memory(current_line, lines, lines_length, lines_len);
	  return NULL;
	}
	lines = ptrt;
	ptrtt = (size_t *) reallocarray(lines_length, lines_size, sizeof(size_t));
	if (ptrtt == NULL) {
	  fprintf(stderr, "Error at allocating memory.\n");
	  /* Deallocate everything we allocated */
	  __clean_up_memory(current_line, lines, lines_length, lines_len);
	  return NULL;
	}
	lines_length = ptrtt;
      }
    }
	
    /* Here, we have an array of lines allocated, into 
       which we can put our current line.
    */
    lines[lines_len] = current_line;
    lines_length[lines_len] = current_line_len;
    lines_len++;
    current_line = NULL;
    current_line_len = (size_t) 0;
    current_line_size = (size_t) 0;
  }

  /* Here, we have the array lines of lines. Each line has a length
     that is stored in lines_length.
  */
  
  // Allocate memory for the LineData object
  LineData *result = (LineData *)malloc(sizeof(LineData));
  if (result == NULL) {
    fprintf(stderr, "Error: Memory allocation failed\n");
    // Clean up any resources if needed
    __clean_up_memory(current_line, lines, lines_length, lines_len);
    return NULL; // Return NULL to indicate failure
  }

  // Populate the LineData object
  result->lines = lines;
  result->lines_length = lines_length;
  result->lines_len = lines_len;
  
  if (current_line != NULL) free(current_line);
  
  return result; // Return a pointer to the LineData object
}

