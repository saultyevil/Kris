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


#include <time.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "kilo.h"


// Append each row of the editor screen to the output terminal
void write_editor_lines (SCREEN_BUF *sb)
{
  char welcome[80];
  int row, padding, welcome_len, file_row;
  size_t line_len;

  for (row = 0; row < editor.n_screen_rows; row++)
  {
    // Index the line to offset due to the current level of scroll
    file_row = row + editor.row_offset;

    // Write a ~ to indicate that this is an empty line
    if (file_row >= editor.nlines)
    {
      // Write the welcome message to the screen - should only show when the
      // text buffer is empty
      if (row == editor.n_screen_rows / 5 && editor.nlines == 0)
      {
        welcome_len = snprintf (welcome, sizeof (welcome),
                                "Kris editor -- version %s", VERSION);
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
    append_to_screen_buf (sb, "\r\n", 2);
  }
}

// Create the standard status message
void set_status_message (char *fmt, ...)
{
  va_list ap;
  va_start (ap, fmt);
  vsnprintf (editor.status_msg, sizeof (editor.status_msg), fmt, ap);
  va_end (ap);
  editor.status_msg_time = time (NULL);
}

void write_message_bar (SCREEN_BUF *sb)
{
  size_t msg_len;

  // Clear the message bar with \x1b[K
  append_to_screen_buf (sb, "\x1b[K", 3);

  // Add the status message to the screen buffer, but only draw this if it has
  // been on screen for less than 5 seconds
  msg_len = strlen (editor.status_msg);
  if (msg_len > editor.n_screen_cols)
    msg_len = (size_t) editor.n_screen_cols;
  if (msg_len && time (NULL) - editor.status_msg_time < 5)
    append_to_screen_buf (sb, editor.status_msg, msg_len);
}

// Write the status bar for filename and line number to the screen buffer
void write_status_bar (SCREEN_BUF *sb)
{
  int status_len, r_len;
  size_t line_len;
  char status[80], line_num[80];

  // \x1b[7m will switch to inverted colours for ther terminal
  append_to_screen_buf (sb, "\x1b[7m", 4);

  // Add the filename and number of lines to the status bar
  status_len = snprintf (status, sizeof (status), "%.20s - %d lines %s",
         editor.filename ? editor.filename : "[No File]", editor.n_screen_rows,
         editor.modified ? "(modified)" : "");
  if (status_len > editor.n_screen_cols)
    status_len = editor.n_screen_cols;
  append_to_screen_buf (sb, status, (size_t) status_len);

  // Add the current line number to the status bar
  r_len = snprintf (line_num, sizeof (line_num), "%d/%d", editor.cy + 1,
                    editor.nlines);

  // Render spaces and the line number status message
  line_len = (size_t) status_len;
  while (line_len < editor.n_screen_cols)
  {
    if (editor.n_screen_cols - line_len == r_len)
    {
      append_to_screen_buf (sb, line_num, (size_t) r_len);
      break;
    }
    else
    {
      append_to_screen_buf (sb, " ", 1);
      line_len++;
    }
  }

  // \x1b[m will revert back to normal formatting
  append_to_screen_buf (sb, "\x1b[m", 3);
  append_to_screen_buf (sb, "\r\n", 2);
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

// Convert the cursor position in the render array to a position in the char
// array
int convert_rx_to_cx (eline *line, int rx)
{
  int cx, cur_rx;

  // Loop over the chars array and increment until cx reaches the same size as
  // rx
  cur_rx = 0;
  for (cx = 0; cx < line->len; cx++)
  {
    if (line->chars[cx] == '\t')
      cur_rx += (TAB_WIDTH - 1) - (cur_rx % TAB_WIDTH);
    cur_rx++;

    if (cur_rx > rx)
      return cx;
  }

  return cx;
}

// Enable scrolling in the editor
void scroll_text_buffer (void)
{
  // Update the cursor to be in the correct position for the render array
  editor.rx = 0;
  if (editor.cy < editor.nlines)
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

  scroll_text_buffer ();

  // Reset the VT100 mode and place the cursor into the home position
  // and write the editor rows and status to the screen buffer
  append_to_screen_buf (&sb, "\x1b[?25l", 6);
  append_to_screen_buf (&sb, "\x1b[H", 3);
  write_editor_lines (&sb);
  write_status_bar (&sb);
  write_message_bar (&sb);

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
void update_to_render_buffer (eline *line)
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

// Insert a char control function
void insert_char (int c)
{
  // If the cursor is on the bottom line, append a new line
  if (editor.cy == editor.nlines)
    append_line_to_text_buffer (editor.nlines, "", 0);

  insert_char_in_line (&editor.lines[editor.cy], editor.cx, c);
  editor.cx++;
}

// Insert a new, empty line to the text buffer
void insert_new_line (void)
{
  eline *line;

  // If we're at the start of a line, append a row above
  if (editor.cx == 0)
    append_line_to_text_buffer (editor.cy, "", 0);
  // Split the line into two lines and then insert a row
  else
  {
    line = &editor.lines[editor.cy];
    append_line_to_text_buffer (editor.cy + 1, &line->chars[editor.cx],
                                line->len - editor.cx);
    line = &editor.lines[editor.cy];
    line->len = (size_t) editor.cx;
    line->chars[line->len] = '\0';
    update_to_render_buffer (line);
  }

  editor.cy++;
  editor.cx = 0;
}

// Delete a char control function
void delete_char (void)
{
  eline *line;

  // Nothing to delete if we are past the last line in the file or if the cursor
  // is in the top left
  if (editor.cy == editor.nlines)
    return;
  if (editor.cx == 0 && editor.cy == 0)
    return;

  line = &editor.lines[editor.cy];
  if (editor.cx > 0)
  {
    delete_char_in_line (line, editor.cx - 1);
    editor.cx--;
  }
  // If at the beginning of the line, append to the previous line and free the
  // extra memory
  else
  {
    editor.cx = (int) editor.lines[editor.cy - 1].len;
    append_string_to_line (&editor.lines[editor.cy - 1], line->chars, line->len);
    delete_line (editor.cy);
    editor.cy--;
  }
}
