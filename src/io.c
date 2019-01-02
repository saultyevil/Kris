/* ***************************************************************************
 *
 * @file io.c
 *
 * @date 01/01/2019
 *
 * @author E. J. Parkinson
 *
 * @brief
 *
 * @details
 *
 * ************************************************************************** */



#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "kilo.h"


void open_file (char *filename)
{
  FILE *input_file;
  char  *line;
  size_t linecap;
  ssize_t linelen;

  if (!(input_file = fopen (filename, "r")))
    error ("Unable to open input file");

  // Read in EACH line of the input file and append to the text buffer
  line = NULL;
  linecap = 0;
  while ((linelen = getline (&line, &linecap, input_file) != -1))
  {
    // Strip off the return or new line chars and append to the text buffer
    while (linelen > 0 && (line[linelen - 1] == '\n' ||
                           line[linelen - 1] == '\r'))
      linelen--;
    append_to_text_buffer (line, (size_t) linelen);
  }

  free (line);
  if (fclose (input_file))
    error ("Couldn't close input file");
}

// Append a string to the screen buffer
void append_to_screen_buf (SCREEN_BUF *sb, char *s, size_t len)
{
  char *new;
  if (!(new = realloc (sb->buf, sb->len + len)))
    error ("Couldn't allocate memory for screen buffer");

  // Copy string s onto the end of the screen buffer and update the buffer
  // pointer in the SCREEN_BUF struct
  memcpy (&new[sb->len], s, len);
  sb->buf = new;
  sb->len += len;
}

// Free the screen buffer
void free_screen_buf (SCREEN_BUF *sb)
{
  free (sb->buf);
}
