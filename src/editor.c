/** **************************************************************************
 *
 * @file editor.c
 *
 * @date 01/01/2019
 *
 * @author E. J. Parkinson
 *
 * @brief Main editing functions for Kris.
 *
 * ************************************************************************** */


#include <ctype.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "kris.h"


//! @brief Append a string to the end of the screen buffer
void editor_add_to_screen_buf (SCREEN_BUF *sb, char *s, size_t len)
{
  char *new;

  // Allocate more memory and append string s to the end of the screen buf
  if (!(new = realloc (sb->buf, sb->len + len)))
    util_exit ("Couldn't allocate memory for screen buffer");
  memcpy (&new[sb->len], s, len);
  sb->buf = new;
  sb->len += len;
}

//! @brief Add a line to the render buffer which will be displayed to the screen
void editor_add_to_render_buffer (EDITOR_LINE *line)
{
  int ntabs;
  size_t i, ii;

  // Count the number of tab characters on the line
  ntabs = 0;
  for (i = 0; i < line->len; i++)
    if (line->chars[i] == '\t')
      ntabs++;

  /*
   * Allocate enough space for render -- note that line->len already counts 1
   * space for each tab character, so we only need to account for 7 more spaces
   * where we are assuming that tabs are 8 spaces
   *  TODO: allow tab spaces to be changed from 8
   */

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

  // Terminate the string and update the syntax highlighting
  line->render[ii] = '\0';
  line->r_len = ii;
  syntax_update_highlighting (line);
}

//! @brief Insert a char control function
void editor_insert_char (int c)
{
  // If the cursor is on the bottom line, append a new line
  if (editor.cy == editor.nlines)
    line_add_to_text_buffer (editor.nlines, "", 0);

  line_insert_char (&editor.lines[editor.cy], editor.cx, c);
  editor.cx++;
}

//! @brief Delete a char control function
void editor_delete_char (void)
{
  EDITOR_LINE *line;

  // Nothing to delete if we are at the very first or very last char
  if (editor.cy == editor.nlines)
    return;
  if (editor.cx == 0 && editor.cy == 0)
    return;

  line = &editor.lines[editor.cy];
  if (editor.cx > 0)
  {
    line_delete_char (line, editor.cx - 1);
    editor.cx--;
  }
  // If at the beginning of the line, append to the previous line
  else
  {
    editor.cx = (int) editor.lines[editor.cy - 1].len;
    line_add_string_to_text_buffer (&editor.lines[editor.cy - 1], line->chars, line->len);
    line_delete (editor.cy);
    editor.cy--;
  }
}

//! @brief Insert a new, empty line to the text buffer
void editor_insert_new_line (void)
{
  EDITOR_LINE *line;

  // If we're at the start of a line, append a row above
  if (editor.cx == 0)
    line_add_to_text_buffer (editor.cy, "", 0);
    // Split the line into two lines and then insert a row
  else
  {
    line = &editor.lines[editor.cy];
    line_add_to_text_buffer (editor.cy + 1, &line->chars[editor.cx],
                             line->len - editor.cx);
    line = &editor.lines[editor.cy];
    line->len = (size_t) editor.cx;
    line->chars[line->len] = '\0';
    editor_add_to_render_buffer (line);
  }

  editor.cy++;
  editor.cx = 0;
}

//! @brief Set a status message
void editor_set_status_message (char *fmt, ...)
{
  va_list ap;
  va_start (ap, fmt);
  vsnprintf (editor.status_msg, sizeof (editor.status_msg), fmt, ap);
  va_end (ap);
  editor.status_msg_time = time (NULL);
}

//! @brief Write the status bar for filename and line number to screen buffer
void editor_update_status_message (SCREEN_BUF *sb)
{
  size_t line_len;
  int status_len, r_len;
  char status[80], line_num[80];

  // \x1b[7m will switch to inverted colours
  editor_add_to_screen_buf (sb, "\x1b[7m", 4);

  // Add the filename and number of lines to the status bar
  status_len = snprintf (status, sizeof (status), "%.20s - %d lines %s",
                 editor.filename ? editor.filename : "[No File]", editor.nlines,
                 editor.modified ? "(modified)" : "");
  if (status_len > editor.screen_cols)
    status_len = editor.screen_cols;
  editor_add_to_screen_buf (sb, status, (size_t) status_len);

  // Add the current line number to the status bar
  r_len = snprintf (line_num, sizeof (line_num), "%s | %d/%d",
                  editor.syntax ? editor.syntax->filetype : "Unknown file type",
                  editor.cy + 1, editor.nlines);

  // Add spaces and the line number to the screen buffer
  line_len = (size_t) status_len;
  while (line_len < editor.screen_cols)
  {
    if (editor.screen_cols - line_len == r_len)
    {
      editor_add_to_screen_buf (sb, line_num, (size_t) r_len);
      break;
    }
    else
    {
      editor_add_to_screen_buf (sb, " ", 1);
      line_len++;
    }
  }

  // Revert back to normal formatting
  editor_add_to_screen_buf (sb, "\x1b[m", 3);
  editor_add_to_screen_buf (sb, "\r\n", 2);
}

//! @brief Update the message in the message bar
void editor_update_message_bar (SCREEN_BUF *sb)
{
  size_t msg_len;

  // Clear the message bar with \x1b[K
  editor_add_to_screen_buf (sb, "\x1b[K", 3);

  /*
   * Add the status message to the screen buffer, but only draw this if it has
   * been on screen for less than 5 seconds
   */

  msg_len = strlen (editor.status_msg);
  if (msg_len > editor.screen_cols)
    msg_len = (size_t) editor.screen_cols;
  if (msg_len && time (NULL) - editor.status_msg_time < 5)
    editor_add_to_screen_buf (sb, editor.status_msg, msg_len);
}

//! @brief Enable scrolling in the editor
void editor_scroll_text_buffer (void)
{
  // Update the cursor to be in the correct position for the render array
  editor.rx = 0;
  if (editor.cy < editor.nlines)
    editor.rx = util_convert_cx_to_rx (&editor.lines[editor.cy], editor.cx);

  // Check if the cursor is in the bounds of the visible window
  if (editor.cy < editor.row_offset)
    editor.row_offset = editor.cy;
  if (editor.cy >= editor.row_offset + editor.screen_rows)
    editor.row_offset = editor.cy - editor.screen_rows + 1;

  // Scrolling for render buffer
  if (editor.rx < editor.col_offset)
    editor.col_offset = editor.rx;
  if (editor.rx > editor.col_offset + editor.screen_cols)
    editor.col_offset = editor.rx - editor.screen_cols + 1;
}

//! @brief Update the entire screen buffer with the text buffer
void editor_update_screen_buffer (SCREEN_BUF *sb)
{
  char *c, symbol;
  unsigned char *hl;
  char welcome[80], buf[16];
  int line, padding, colour, current_colour;
  size_t i, line_len, buf_len, col_len, welcome_len, file_row;

  for (line = 0; line < editor.screen_rows; line++)
  {
    // Index the line to offset for the current level of scrolling
    file_row = (size_t) line + editor.row_offset;

    // Write a ~ to indicate empty line
    if (file_row >= editor.nlines)
    {
      // Write welcome message, only show when text buffer is empty
      if (line == editor.screen_rows / 5 && editor.nlines == 0)
      {
        welcome_len = (size_t ) snprintf (welcome, sizeof (welcome),
                                          "Kris editor -- version %s", VERSION);
        if (welcome_len > editor.screen_cols)
          welcome_len = (size_t) editor.screen_cols;

        // Pad the welcome message into the centre of the screen
        if ((padding = (int) (editor.screen_cols - welcome_len) / 2))
        {
          editor_add_to_screen_buf (sb, "~", 1);
          padding--;
        }

        while (padding--)
          editor_add_to_screen_buf (sb, " ", 1);
        editor_add_to_screen_buf (sb, welcome, (size_t) welcome_len);
      }
      else
        editor_add_to_screen_buf (sb, "~", 1);
    }
    // Add a line of the text buffer to the screen buffer char by char
    else
    {
      line_len = editor.lines[file_row].r_len - editor.col_offset;
      if (line_len < 0)
        line_len = 0;
      if (line_len > editor.screen_cols)
        line_len = (size_t) editor.screen_cols;

      c = &editor.lines[file_row].render[editor.col_offset];
      hl = &editor.lines[file_row].hl[editor.col_offset];
      current_colour = -1;

      // Iterate over each char in the render array and process each char
      // individually
      for (i = 0; i < line_len; i++)
      {
        // Handle control sequences (nonprintable chars) a bit better
        if ( iscntrl (c[i]))
        {
          symbol = (char) ((c[i] <= 26) ? '@' + c[i] : '?');
          editor_add_to_screen_buf (sb, "\x1b[7m", 4);
          editor_add_to_screen_buf (sb, &symbol, 1);
          editor_add_to_screen_buf (sb, "\x1b[m", 3);

          // Re-enable regular text formatting
          if (current_colour != -1)
          {
            col_len = (size_t) snprintf (buf, sizeof (buf), "\x1b[%dm",
                                         current_colour);
            editor_add_to_screen_buf (sb, buf, col_len);
          }
        }
        // Append to the screen buffer using the default colour
        else if (hl[i] == HL_NORMAL)
        {
          if (current_colour != -1)
          {
            editor_add_to_screen_buf (sb, "\x1b[39m", 5);
            current_colour = -1;
          }

          editor_add_to_screen_buf (sb, &c[i], 1);
        }
        // Append to the screen buffer with syntax highlighting
        else
        {
          colour = syntax_get_colour (hl[i]);

          if (colour != current_colour)
          {
            current_colour = colour;
            buf_len = (size_t) snprintf (buf, sizeof (buf), "\x1b[%dm", colour);
            editor_add_to_screen_buf (sb, buf, buf_len);
          }

          editor_add_to_screen_buf (sb, &c[i], 1);
        }
      }

      editor_add_to_screen_buf (sb, "\x1b[39m", 5);
    }

    // Append new line
    editor_add_to_screen_buf (sb, "\x1b[K", 3);
    editor_add_to_screen_buf (sb, "\r\n", 2);
  }
}

//! @brief Refresh the entire editor screen -- redraw everything
void editor_refresh_screen (void)
{
  char buf[32];
  SCREEN_BUF sb = SBUF_INIT;

  editor_scroll_text_buffer ();
  /*
   * Reset the VT100 mode and place the cursor into the home position
   * and write the editor rows and status to the screen buffer
   */

  editor_add_to_screen_buf (&sb, "\x1b[?25l", 6);
  editor_add_to_screen_buf (&sb, "\x1b[H", 3);
  editor_update_screen_buffer (&sb);
  editor_update_status_message (&sb);
  editor_update_message_bar (&sb);

  // Reposition the cursor in the terminal window
  snprintf (buf, sizeof (buf), "\x1b[%d;%dH",
      (editor.cy - editor.row_offset) + 1, (editor.rx - editor.col_offset) + 1);
  editor_add_to_screen_buf (&sb, buf, (int) strlen (buf));

  /*
   * Enable set mode in the terminal (VT100 again) and write out the entire
   * buffer to screen
   */

  editor_add_to_screen_buf (&sb, "\x1b[?25h", 6);
   write (STDOUT_FILENO, sb.buf, sb.len);
  free (sb.buf);
}
