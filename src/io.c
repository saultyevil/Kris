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


#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

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
  editor.modified = FALSE;
}

// Convert array of eline data types into a single string
char *rows_to_strings (size_t *buf_len)
{
  char *buf, *p;
  size_t i, tot_len;

  // Figure out the total number of chars in the text buffer
  tot_len = 0;
  for (i = 0; i < editor.n_lines; i++)
    tot_len += editor.lines[i].len + 1;
  *buf_len = tot_len;

  // Copy the contents of each row to the end of the buffer and append a new
  // line character at the end of each row
  p = buf = malloc (tot_len);
  for (i = 0; i < editor.n_lines; i++)
  {
    memcpy (p, editor.lines[i].chars, editor.lines[i].len);
    p += editor.lines[i].len;
    *p = '\n';
    p++;
  }

  return buf;
}

// Save the text buffer to file
void save_file (void)
{
  size_t buf_len;
  char *buf;
  int file_desc;

  if (editor.filename == NULL)  // TODO: prompt user for file name
    return;

  // Retrieve the text buffer in the form of strings, create the file in 0644
  // mode and then write to file
  buf = rows_to_strings (&buf_len);
  if (((file_desc = open (editor.filename, O_RDWR | O_CREAT, 0644)) != -1))
  {
    if (ftruncate (file_desc, buf_len) != -1)
    {
      if (write (file_desc, buf, buf_len) == buf_len)
      {
        close (file_desc);
        free (buf);
        editor.modified = FALSE;
        set_status_message ("%d bytes written to disk", buf_len);
        return;
      }
    }
    close (file_desc);
  }

  free (buf);
  set_status_message ("Can't save file! I/O error: %s", strerror (errno));
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
