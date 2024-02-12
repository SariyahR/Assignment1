#include "useful_functions.h"
#include <unistd.h>
#include <string.h>
#include <stdio.h>

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

