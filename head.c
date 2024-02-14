#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include "useful_functions.h"

#define BUFFER_SIZE                      (((size_t) 4096))
#define FIRST_ALLOCATION_SIZE            (((size_t) 16))
#define FIRST_ALLOCATION_LINE_ARRAY_SIZE (((size_t) 16))


int main(int argc, char **argv) {
  size_t i;
  size_t k;
  /* Default number of lines */
  int num_lines = 10;

  /* Check if the -n option is provided*/
  if (argc == 3 && strcmp(argv[1], "-n") == 0) {
    /* argv[0]: The program name.
       argv[1]: The -n option.
       argv[2]: The number of lines argument.
    */
    num_lines = atoi(argv[2]);
    if (num_lines <= 0) {
      fprintf(stderr, "Error: Invalid number of lines\n");
      return 1;
    }
  } else if (argc != 1) {
    /* Invalid usage, print error message */
    fprintf(stderr, "Usage: %s [-n <num>]\n", argv[0]);
    return 1;
  }

  /*Process input based on the number of lines specified
    For now, let's just print out the number of lines */
  printf("Number of lines: %d\n", num_lines);

  
  /* Get the line data struct with the info:) */
  LineData *lineDataPtr = get_lines_from_standard_input();

  if (lineDataPtr != NULL) {
    // Access the lines and lines_length arrays
    char **lines = lineDataPtr->lines;
    size_t *lines_length = lineDataPtr->lines_length;
    
     /* We need to write all lines in backward order to standard output.*/
    for (k = 0; k < num_lines; k++) {
      if (my_write(1, lines[k], lines_length[k]) < 0) {
	fprintf(stderr, "Error while reading: %s\n", strerror(errno));
	
         /* Deallocate everything we allocated */
	if(lines != NULL) {
	  for (i=((size_t) 0); i < lineDataPtr->lines_len; i++) {
	    free(lines[i]);
	  }
	  free(lines);
	  free(lines_length);
	  free(lineDataPtr->lines);
	  free(lineDataPtr->lines_length);
	  free(lineDataPtr);
	}
	return 1;
      }
    }
    
  } else {
    // Handle the case where get_lines_from_standard_input() returned NULL
    fprintf(stderr, "Error: Failed to read lines from standard input\n");
  }

  /* Deallocate everything we allocated */
  free(lineDataPtr->lines);
  free(lineDataPtr->lines_length);
  free(lineDataPtr);
 
  
  return 0;
}
