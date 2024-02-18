#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <stdint.h>
#include "useful_functions.h"

#define BUFFER_SIZE                      (((size_t) 4096))
#define FIRST_ALLOCATION_SIZE            (((size_t) 16))
#define FIRST_ALLOCATION_LINE_ARRAY_SIZE (((size_t) 16))

int main(int argc, char **argv) {
    size_t i;
    size_t k;
    int num_lines;
    int converted_num;
    char *endptr;

    /* The next conditional statements make sure to
       handle all well-formed calls to head */

    /* This means filename and number of lines are specified */
    if (argc == 4) {
      if (strcmp(argv[1], "-n") == 0 ) {
	/* argv[0]: The program name
	   argv[1]: The -n option
	   argv[2]: The number of lines argument
	   argv[3]: The file name
	*/
	
      } else if (strcmp(argv[2], "-n") == 0) {
	/* argv[0]: The program name
	   argv[1]: The file name
	   argv[2]: The -n option
	   argv[3]: The file name
	*/
	
      } else {
	/* Has four arguments, but not in the correct format*/
	print_error_message_badly_formed_call("head");
	return 1;
      }
      
    } else if (argc == 3) {
      /* argv[0]: The program name
	 argv[1]: The -n option
	 argv[2]: The number of lines argument
      */
      converted_num = convert_from_string_to_number(argv[2], &endptr);

      if (endptr == argv[2]) {
	/* No valid digits found in the string */
	printf("No digits were found\n");
	return 1;
	
      } else if (*endptr != '\0') {
	/* A valid number mixed with invalid characters */
        printf("Invalid characters after number: %s\n", endptr);
	return 1;
	
      } else if (converted_num < 0) {
	/* If negative number of lines is entered do nothing*/
	printf("Doing nothing because negative.\n");
	return 0;
	
      } else if (converted_num >= UINT64_MAX) {
	num_lines = converted_num;
	printf("The number you entered is too large.\n");
	return 1;
      } else {
	/* Everything went well, number was converted
	 correctly and is within range
	*/
	num_lines = converted_num;
      }
      
    } else if (argc == 2) {
      /* argv[0]: The program name
	 argv[1]: The file name
      */
    } else if (argc == 1) {
      /* argv[0]: The program name
	 Default number of lines is 10
      */
      num_lines = 10;
      
    } else {
      print_error_message_badly_formed_call("head");
      return 1;
    }

    /*Process input based on the number of lines specified
      For now, let's just print out the number of lines */
    printf("Number of lines: %d\n", num_lines);

    /* Get the line data struct with the info */
    LineData *lineDataPtr = get_lines_from_standard_input();

    if (lineDataPtr != NULL) {
        // Access the lines and lines_length arrays
        char **lines = lineDataPtr->lines;
        size_t *lines_length = lineDataPtr->lines_length;
	size_t lines_total = lineDataPtr->lines_len;
	
        /* We need to write all lines in backward order to standard output.*/
        for (k = 0; k < num_lines; k++) {
            if (my_write(1, lines[k], lines_length[k]) < 0) {
                fprintf(stderr, "Error while reading: %s\n", strerror(errno));

                /* Deallocate everything we allocated */
                for (i = 0; i < lines_total; i++) {
                    free(lines[i]);
                }
                free(lines);
                free(lines_length);
                free(lineDataPtr);
                return 1;
            }
        }

        // Free memory allocated for lines and lines_length
        for (i = 0; i < lineDataPtr->lines_len; i++) {
            free(lines[i]);
        }
        free(lines);
        free(lines_length);

        // Free memory allocated for lineDataPtr
        free(lineDataPtr);

    } else {
        // Handle the case where get_lines_from_standard_input() returned NULL
        fprintf(stderr, "Error: Failed to read lines from standard input\n");
	return 1;
    }

    return 0;
}
 
