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



#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "kilo.h"


// Append each row of the editor screen to the output terminal
void write_editor_rows (SCREEN_BUF *sb)
{
  char welcome[80];
  int row, padding, welcome_len, file_row;
  size_t line_len;

  for (row = 0; row < editor.n_screen_rows; row++)
  {
    // Index the line to offset due to the current level of scroll
    file_row = row + editor.row_offset;

    // Write a ~ to indicate that this is an empty line
    if (file_row >= editor.n_editor_rows)
    {
      // Write the welcome message to the screen - should only show when the
      // text buffer is empty
      if (row == editor.n_screen_rows / 5 && editor.n_editor_rows == 0)
      {
        welcome_len = snprintf (welcome, sizeof (welcome),
                                "Kilo editor -- version %s", VERSION);
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
        append_to_screen_buf (sb, welcome, (size_t) welcome_len);
      }
      else
        append_to_screen_buf (sb, "~", 1);
    }
    // Add the text buffer row to the screen buffer
    else
    {
      line_len = editor.lines[file_row].r_len - editor.col_offset;
      if (line_len < 0)
        line_len = 0;
      if (line_len > editor.n_screen_cols)
        line_len = (size_t) editor.n_screen_cols;
      append_to_screen_buf (sb,
                  &editor.lines[file_row].render[editor.col_offset], line_len);
    }

    // aaa
    append_to_screen_buf (sb, "\x1b[K", 3);
    if (row < editor.n_screen_rows - 1)
      append_to_screen_buf (sb, "\r\n", 2);

  }
}

// Convert the cursor position in the chars array to a position in the render
// array
int convert_cx_to_rx (eline *line, int cx)
{
  int rx;
  size_t i;

  // Loop over all of the chars to the left of cx and count how many spaces
  // each tab takes up
  rx = 0;
  for (i = 0; i < cx; i++)
  {
    // Figure out how many spaces are to the right of the tab character
    if (line->chars[i] == '\t')
      rx += (TAB_WIDTH - 1) - (rx % TAB_WIDTH);
    rx++;
  }

  return rx;
}

// Enable scrolling in the editor
void scroll_editor_rows (void)
{
  // Update the cursor to be in the correct position for the render array
  editor.rx = 0;
  if (editor.cy < editor.n_editor_rows)
    editor.rx = convert_cx_to_rx (&editor.lines[editor.cy], editor.cx);

  // Check if the cursor is in the bounds of the visible window
  if (editor.cy < editor.row_offset)
    editor.row_offset = editor.cy;
  if (editor.cy >= editor.row_offset + editor.n_screen_rows)
    editor.row_offset = editor.cy - editor.n_screen_rows + 1;
  if (editor.rx < editor.col_offset)
    editor.col_offset = editor.rx;
  if (editor.rx > editor.col_offset + editor.n_screen_cols)
    editor.col_offset = editor.rx - editor.n_screen_cols + 1;
}

// Refresh the editor screen - i.e. the ui?
void draw_editor_screen (void)
{
  char buf[32];
  SCREEN_BUF sb = SBUF_INIT;

  scroll_editor_rows ();

  // Reset the VT100 mode and place the cursor into the home position
  // and write the editor rows to the buffer
  append_to_screen_buf (&sb, "\x1b[?25l", 6);
  append_to_screen_buf (&sb, "\x1b[H", 3);
  write_editor_rows (&sb);

  // Reposition the cursor in the terminal window
  snprintf (buf, sizeof (buf), "\x1b[%d;%dH",
      (editor.cy - editor.row_offset) + 1, (editor.rx - editor.col_offset) + 1);
  append_to_screen_buf (&sb, buf, (int) strlen (buf));

  // Enable set mode in the terminal (VT100 again) and write out the entire
  // buffer to screen
  append_to_screen_buf (&sb, "\x1b[?25h", 6);
  write (STDOUT_FILENO, sb.buf, sb.len);
  free_screen_buf (&sb);
}




// Update a row to replace special characters, i.e. turn \t into spaces
void update_render_row (eline *line)
{
  int ntabs;
  size_t i, ii;

  // Count the number of tab characters on the line
  ntabs = 0;
  for (i = 0; i < line->len; i++)
    if (line->chars[i] == '\t')
      ntabs++;

  // Allocate enough space for render -- note that line->len already counts 1
  // space for each tab character, so we only need to account for 7 more spaces
  // where we are assuming that tabs are 8 spaces
  // TODO: allow tab spaces to be changed from 8
  free (line->render);
  line->render = malloc (line->len + (TAB_WIDTH - 1) * ntabs + 1);

  // Copy the line chars into the render buffer
  ii = 0;
  for (i = 0; i < line->len; i++)
  {
    // Convert tab characters into spaces
    if (line->chars[i] == '\t')
    {
      line->render[ii++] = ' ';
      while (ii % TAB_WIDTH != 0)
        line->render[ii++] = ' ';
    }
    else
      line->render[ii++] = line->chars[i];
  }

  line->render[ii] = '\0';
  line->r_len = ii;
}

// Append a row of text to the text buffer
void append_to_text_buffer (char *s, size_t linelen)
{
  int line_index;

  // Allocate another line of memory
  editor.lines = realloc (editor.lines, sizeof (eline) *
                                                    (editor.n_editor_rows + 1));

  // Append text to the new text buffer line
  line_index = editor.n_editor_rows;
  editor.lines[line_index].len = linelen;
  editor.lines[line_index].chars = malloc (linelen + 1);
  memcpy (editor.lines[line_index].chars, s, linelen);
  editor.lines[line_index].chars[linelen] = '\0';

  // Update the rendering buffer for special characters
  editor.lines[line_index].r_len = 0;
  editor.lines[line_index].render = NULL;
  update_render_row (&editor.lines[line_index]);

  editor.n_editor_rows++;
}
