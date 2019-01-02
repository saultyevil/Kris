/* ***************************************************************************
 *
 * @file editor.c
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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "kilo.h"


// Append each row of the editor screen to the output terminal
void draw_editor_rows (SCREEN_BUF *sb)
{
  char welcome[80];
  int row, padding, welcome_len;
  size_t linelen;

  for (row = 0; row < editor.n_screen_rows; row++)
  {
    // Write a ~ to indicate that this is an empty line
    if (row >= editor.n_editor_rows)
    {
      // Write the welcome message to the screen - should only show when the
      // text buffer is empty
      if (row == editor.n_screen_rows / 3 && editor.n_editor_rows == 0)
      {
        welcome_len = snprintf (welcome, sizeof (welcome),
                                "Kilo editor -- version %s\r\n", VERSION);
        if (welcome_len > editor.n_screen_cols)
          welcome_len = editor.n_screen_cols;

        // Pad the welcome message into the centre of the screen
        if ((padding = (editor.n_screen_cols - welcome_len) / 2))
        {
          append_to_screen_buf (sb, "~", 1);
          padding--;
        }
        while (padding--)
          append_to_screen_buf (sb, " ", 1);

        append_to_screen_buf (sb, welcome, welcome_len);
      }
      else
        append_to_screen_buf (sb, "~", 1);
    }
    // Write out the text buffer, i.e. lines which can be edited
    else
    {
      linelen = editor.lines[row].len;
      if (linelen > editor.n_screen_cols)
        linelen = (size_t) editor.n_screen_cols;
      append_to_screen_buf (sb, editor.lines[row].content, linelen);
    }

    // aaa
    append_to_screen_buf (sb, "\x1b[K", 3);
    if (row < editor.n_screen_rows - 1)
      append_to_screen_buf (sb, "\r\n", 2);

  }
}

// Refresh the editor screen - i.e. the ui?
void refresh_editor_screen (void)
{
  char buf[32];
  SCREEN_BUF sb = SBUF_INIT;

  // Reset the VT100 mode and place the cursor into the home position
  // and write the editor rows to the buffer
  append_to_screen_buf (&sb, "\x1b[?25l", 6);
  append_to_screen_buf (&sb, "\x1b[H", 3);
  draw_editor_rows (&sb);

  // Reposition the cursor in the terminal window
  snprintf (buf, sizeof (buf), "\x1b[%d;%dH", editor.y + 1, editor.x + 1);
  append_to_screen_buf (&sb, buf, (int) strlen (buf));

  // Enable set mode in the terminal (VT100 again) and write out the entire
  // buffer to screen
  append_to_screen_buf (&sb, "\x1b[?25h", 6);
  write (STDOUT_FILENO, sb.buf, sb.len);
  free_screen_buf (&sb);
}

// Append a row of text to the text buffer
void append_to_text_buffer (char *s, size_t linelen)
{
  int line_index;

  // Allocate another line of memory
  editor.lines = realloc (editor.lines, sizeof (erow) *
                          (editor.n_editor_rows + 1));

  // Append text to the new text buffer line
  line_index = editor.n_editor_rows;
  editor.lines[line_index].len = linelen;
  editor.lines[line_index].content = malloc (linelen + 1);
  memcpy (editor.lines[line_index].content, s, linelen);
  editor.lines[line_index].content[linelen] = '\0';
  editor.n_editor_rows++;
}
