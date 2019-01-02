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
  char *line;
  size_t line_cap;
  ssize_t line_len;
  FILE *input_file;

  free (editor.filename);
  // strdup duplicates a string for a new pointer
  editor.filename = strdup (filename);

  if (!(input_file = fopen (filename, "r")))
    error ("Unable to open input file");

  // Read in EACH line of the input file and append to the text buffer
  line = NULL;
  line_cap = 0;
  while ((line_len = getline (&line, &line_cap, input_file) != -1))
  {
    // TODO: replace this strlen with working version of getline return value
    line_len = strlen (line);
    // Strip off the return or new line chars and append to the text buffer
    while (line_len > 0 &&
                     (line[line_len - 1] == '\n' || line[line_len - 1] == '\r'))
      line_len--;
    append_to_text_buffer (line, (size_t) line_len);
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
