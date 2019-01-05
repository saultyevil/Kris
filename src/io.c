/** **************************************************************************
 *
 * @file io.c
 *
 * @date 01/01/2019
 *
 * @author E. J. Parkinson
 *
 * @brief Functions for IO, i.e. reading and saving files.
 *
 * ************************************************************************** */


#include <ctype.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "kris.h"


//! @brief Display a prompt in the status bar, and input text
char *io_status_bar_prompt (char *prompt_msg, void (*callback) (char *, int))
{
  int c;
  char *buf;
  size_t buf_len, buf_size;

  buf_len = 0;
  buf_size = 128;
  buf = malloc (buf_size);
  buf[0] = '\0';

  // Keep looping until buf can be returned
  while (TRUE)
  {
    editor_set_status_message (prompt_msg, buf);
    editor_refresh_screen ();

    c = kp_read_keypress ();

    // Allow user to delete in the status prompt
    if (c == DEL_KEY || c == CTRL_KEY ('h') || c == BACK_SPACE)
    {
      if (buf_len != 0)
        buf[--buf_len] = '\0';
    }
    // Cancel and return if escape is pressed
    else if (c == '\x1b')
    {
      editor_set_status_message ("");
      if (callback)
        callback (buf, c);
      free (buf);
      return NULL;
    }
    // Return buf if enter is pressed
    else if (c == '\r')
    {
      if (buf_len != 0)
      {
        editor_set_status_message ("");
        if (callback)
          callback (buf, c);
        return buf;
      }
    }
    // If not a control sequence add chars to buf
    else if (!iscntrl (c) && c < 128)
    {
      if (buf_len == buf_size - 1)
      {
        buf_size *= 2;
        buf = realloc (buf, buf_size);
      }

      buf[buf_len++] = (char) c;
      buf[buf_len] = '\0';
    }

    if (callback)
      callback (buf, c);
  }
}

//! @brief Open a file and read into the text buffer
int io_read_file (char *filename)
{
  char *line;
  size_t line_cap;
  ssize_t line_len;
  FILE *input_file;

  free (editor.filename);
  editor.filename = strdup (filename);  // Safer than strcpy?

  // Open the file and update the syntax highlighting
  if (!(input_file = fopen (filename, "r")))
  {
    // Handle if the file can't be found
    editor_set_status_message ("Couldn't open file %s: %s", editor.filename,
                               strerror (errno));
    editor.filename = NULL;
    errno = 0; // Set errno to 0, otherwise the main loop will exit
    return FALSE;
  }

  syntax_select_highlighting ();

  // Read in EACH line of the input file and append to the text buffer
  line = NULL;
  line_cap = 0;
  while ((line_len = getline (&line, &line_cap, input_file) != -1))
  {
    line_len = strlen (line);  // TODO: fix getline return value returning 1

    // Strip off the return or new line chars and add to the text buffer
    while (line_len > 0 &&
                     (line[line_len - 1] == '\n' || line[line_len - 1] == '\r'))
      line_len--;
    line_add_to_text_buffer (editor.nlines, line, (size_t) line_len);
  }

  free (line);
  if (fclose (input_file))
    util_exit ("Couldn't close input file");
  editor.modified = FALSE;

  return TRUE;
}

//! @brief Convert an array of ELINEs into a single string
char *io_convert_elines_to_string (size_t *buf_len)
{
  char *buf, *p;
  size_t i, tot_len;

  // Figure out the total number of chars in the text buffer
  tot_len = 0;
  for (i = 0; i < editor.nlines; i++)
    tot_len += editor.lines[i].len + 1;
  *buf_len = tot_len;

  /*
   * Copy the contents of each row to the end of the buffer and append a new
   * line character at the end of each row
   */

  p = buf = malloc (tot_len);
  for (i = 0; i < editor.nlines; i++)
  {
    memcpy (p, editor.lines[i].chars, editor.lines[i].len);
    p += editor.lines[i].len;
    *p = '\n';
    p++;
  }

  return buf;
}

//! @brief Save the text buffer to file
void io_save_file (void)
{
  char *buf;
  int file_desc;
  size_t buf_len;

  // Prompt for filename and set syntax highlighting
  if (editor.filename == NULL)
  {
    editor.filename = io_status_bar_prompt ("Save as: %s", NULL);
    if (editor.filename == NULL)
    {
      editor_set_status_message ("Save aborted");
      return;
    }

    syntax_select_highlighting ();
  }

  /*
   * Retrieve the text buffer in the form of strings, create the file in 0644
   * mode and then write to file
   */

  buf = io_convert_elines_to_string (&buf_len);
  if (((file_desc = open (editor.filename, O_RDWR | O_CREAT, 0644)) != -1))
  {
    if (ftruncate (file_desc, (off_t) buf_len) != -1)
    {
      if (write (file_desc, buf, buf_len) == buf_len)
      {
        close (file_desc);
        free (buf);
        editor.modified = FALSE;
        editor_set_status_message ("%d bytes written to disk", buf_len);
        return;
      }
    }

    close (file_desc);
  }

  free (buf);
  editor_set_status_message (
                            "Can't save file. I/O error: %s", strerror (errno));
}
