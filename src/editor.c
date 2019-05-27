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

/** **************************************************************************
 *
 *  @brief              Append a string to the end of the screen buffer
 *
 *  @param[in,out]      *sb     The screen buffer to append the string to
 *  @param[in]          *s      The string to append to the screen buffer
 *  @param[in]          len     The len of the string *s
 *
 *  @return             void
 *
 *  @details
 *
 *  This function appends a correctly formatted string to the screen buffer,
 *  where the string has the number of chars given by len. This is generally
 *  called to write a line of the render(?) array to the screen buffer.
 *
 * ************************************************************************** */

void
editor_add_to_screen_buf (SCREEN_BUF *sb, char *s, size_t len)
{
  char *new;

  /*
   * Allocate more memory for screen buffer and then append the string s
   * to the end of the screen buffer
   */

  if (!(new = realloc (sb->buf, sb->len + len)))
  {
    util_exit ("Couldn't allocate memory for screen buffer");
  }
  else  // else is used here to stop the compiler for complaining about unint vars
  {
    memcpy (&new[sb->len], s, len);
    sb->buf = new;
    sb->len += len;
  }
}

/** **************************************************************************
 *
 *  @brief              Add a line to the render buffer which will be displayed
 *                      to the screen
 *
 *  @param[in,out]      *line     The line which is being updated by the function
 *
 *  @return             void
 *
 *  @details
 *
 *  This function simply copies the text buffer for a line into the render buffer
 *  for the line, whilst appropriately converting the tab characters into the
 *  correct number of spaces as defined by the constant TAB_WIDTH. The render
 *  array is then terminated, and the syntax highlighting updated.
 *
 * ************************************************************************** */

void
editor_add_to_render_buffer (EDITOR_LINE *line)
{
  int ntabs;

  size_t i;
  size_t ii;

  /*
   * Count the number of tab characters in the text buffer
   */

  ntabs = 0;
  for (i = 0; i < line->len; i++)
  {
    if (line->chars[i] == '\t')
    {
      ntabs++;
    }
  }
  /*
   * Allocate enough space for render -- note that line->len already counts 1
   * space for each tab character, so we only need to account for 7 more spaces
   * where we are assuming that tabs are 8 spaces
   *  TODO: allow tab spaces to be changed from 8 to 4, etc
   */

  free (line->render);
  line->render = malloc (line->len + (TAB_WIDTH - 1) * ntabs + 1);

  /*
   * Copy the characters in the text buffer to the render buffer
   */

  ii = 0;
  for (i = 0; i < line->len; i++)
  {
    /*
     * Now convert tab characters into the appropriate number of spaces
     */

    if (line->chars[i] == '\t')
    {
      line->render[ii++] = ' ';
      while (ii % TAB_WIDTH != 0)
        line->render[ii++] = ' ';
    }
    else
    {
      line->render[ii++] = line->chars[i];
    }
  }

  /*
   * Terminate the string and then update the syntax higlighting
   */

  line->render[ii] = '\0';
  line->r_len = ii;
  syntax_update_highlighting (line);
}

/** **************************************************************************
 *
 *  @brief              Insert a char main control function
 *
 *  @param[in]          c    An integer representing an ASCII char to insert

 *  @return             void
 *
 *  @details
 *
 *  Simply appends the provided character c to the text buffer by calling
 *  line_insert_char. If the cursor is positioned on the last line, then a new
 *  line is appended for the character.
 *
 * ************************************************************************** */

void
editor_insert_char (int c)
{
  /*
   * If the cursor is positioned on the very bottom line, append a new line to
   * the text buffer
   */

  if (editor.cy == editor.nlines)
    line_add_to_text_buffer (editor.nlines, "", 0);

  line_insert_char (&editor.lines[editor.cy], editor.cx, c);
  editor.cx++;
}

/** **************************************************************************
 *
 *  @brief              Delete a char main control function
 *
 *  @return             void
 *
 *  @details
 *
 *  Deletes a character by calling the line_delete_char function. If the cursor
 *  is placed at the first character on a line, then that line is appended to
 *  the previous line in the text buffer. Otherwise, a simple character is
 *  removed.
 *
 * ************************************************************************** */

void
editor_delete_char (void)
{
  EDITOR_LINE *line;

  /*
   * There is nothing to delete if the cursor is positioned at the very first
   * or very last char, hence return without editing anything
   */

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

  /*
   * If the cursor is placed at the beginning of the line, append to line to the
   * previous line
   */

  else
  {
    editor.cx = (int) editor.lines[editor.cy - 1].len;
    line_add_string_to_text_buffer (&editor.lines[editor.cy - 1], line->chars, line->len);
    line_delete (editor.cy);
    editor.cy--;
  }
}

/** **************************************************************************
 *
 *  @brief              Insert a new, empty line to the text buffer
 *
 *  @return             void
 *
 *  @details
 *
 *  Appends a new row to the editor text buffer, either by appending a new row
 *  above or below. This function will also split a line into two separate lines
 *  if the cursor is not at the beginning or the end of a line.
 *
 * ************************************************************************** */

void
editor_insert_new_line (void)
{
  EDITOR_LINE *line;

  /*
   * If at the start of a line, i.e. char 0 of the line, then append a row
   * above the current line
   */

  if (editor.cx == 0)
  {
    line_add_to_text_buffer (editor.cy, "", 0);
  }

  /*
   * Otherwise the line is split into two the create two new rows
   */

  else
  {
    line = &editor.lines[editor.cy];
    line_add_to_text_buffer (editor.cy + 1, &line->chars[editor.cx], line->len - editor.cx);
    line = &editor.lines[editor.cy];
    line->len = (size_t) editor.cx;
    line->chars[line->len] = '\0';
    editor_add_to_render_buffer (line);
  }

  editor.cy++;
  editor.cx = 0;
}

/** **************************************************************************
 *
 *  @brief              Set a status message
 *
 *  @param[in]          *fmt    The formatted string which will be the status
 *                              message
 *  @param[in]          ...     The arguments for the formatted string
 *
 *  @return             void
 *
 *  @details
 *
 *  Simply writes the string given by *fmt and its arguments to the status_msg
 *  variable in the global editor variable.
 *
 * ************************************************************************** */

void
editor_set_status_message (char *fmt, ...)
{
  va_list ap;

  va_start (ap, fmt);
  vsnprintf (editor.status_msg, sizeof editor.status_msg, fmt, ap);
  va_end (ap);

  /*
   * Update when this string was written to the status message variable
   */

  editor.status_msg_time = time (NULL);
}

/** **************************************************************************
 *
 *  @brief              Write the status bar for filename and line number to
 *                      screen buffer
 *
 *  @param[in,out]      sb       The screen buffer to update the status bar to
 *
 *  @return             void
 *
 *  @details
 *
 *  This writes the status bar to (hopefully) the end of the screen buffer. It
 *  is written in inverted colours to help make it stand out.
 *
 * ************************************************************************** */

void
editor_update_status_message (SCREEN_BUF *sb)
{
  int r_len;
  int status_len;

  size_t line_len;

  char status[80];
  char line_num[80];


  /*
   * Switch to inverted colours to make the status message stand out
   */

  editor_add_to_screen_buf (sb, "\x1b[7m", 4);

  /*
   * Add the name of the file and the number of lines in the file
   */

  status_len = snprintf (status, sizeof status, "%.20s - %d lines %s", editor.filename ? editor.filename : "[No File]",
                         editor.nlines, editor.modified ? "(modified)" : "");

  if (status_len > editor.screen_cols)
    status_len = editor.screen_cols;

  editor_add_to_screen_buf (sb, status, (size_t) status_len);

  /*
   *  Add the current line number the cursor is positioned on
   */

  r_len = snprintf (line_num, sizeof line_num, "%s | %d/%d", editor.syntax ? editor.syntax->filetype : "Unknown file type",
                    editor.cy + 1, editor.nlines);

  /*
   * Append white space to the screen of the status message to keep drawing
   * the inverted colours
   */

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

  /*
   * Revert back to normal colours
   */

  editor_add_to_screen_buf (sb, "\x1b[m", 3);
  editor_add_to_screen_buf (sb, "\r\n", 2);
}

/** **************************************************************************
 *
 *  @brief              Update the message in the message bar
 *
 *  @param[in,out]      sb       The screen buffer to update the message to
 *
 *  @return             void
 *
 *  @details
 *
 *  Simply adds a message to the status bar at the bottom of the editor. This
 *  message is only displayed for 5 seconds and then it will be removed. But as
 *  Kris only refreshes when a character is processed, the message will stay up
 *  for longer than 5 seconds until a character is processes.
 *
 * ************************************************************************** */

void
editor_update_message_bar (SCREEN_BUF *sb)
{
  size_t msg_len;

  /*
   * This will clear the message bar
   */

  editor_add_to_screen_buf (sb, "\x1b[K", 3);

  /*
   * Add the status message to the screen buffer, but only draw this if it has
   * been on screen for less than 5 seconds
   */

  msg_len = strlen (editor.status_msg);

  if (msg_len > editor.screen_cols)
    msg_len = (size_t) editor.screen_cols;

  /*
   * If the message has been up for less than 5 seconds, then append the message
   * to the screen buffer
   */

  if (msg_len && time (NULL) - editor.status_msg_time < 5)
    editor_add_to_screen_buf (sb, editor.status_msg, msg_len);
}

/** **************************************************************************
 *
 *  @brief              Enable scrolling in the editor
 *
 *  @return             void
 *
 *  @details
 *
 *  This function simply updates the variables associated with the level of
 *  scroll and the position of the cursor depending on the level of scroll.
 *  Note that this function only updates members of the global editor variable.
 *
 * ************************************************************************** */

void editor_scroll_text_buffer (void)
{
  /*
   * Update the cusor to be in the correct position of the render array
   */

  editor.rx = 0;
  if (editor.cy < editor.nlines)
    editor.rx = util_convert_cx_to_rx (&editor.lines[editor.cy], editor.cx);

  /*
   * Check the cursor is within the bounds of the terminal window
   */

  if (editor.cy < editor.row_offset)
    editor.row_offset = editor.cy;

  if (editor.cy >= editor.row_offset + editor.screen_rows)
    editor.row_offset = editor.cy - editor.screen_rows + 1;

  /*
   * Update the offset variable which controls the level of scroll in the
   * screen buffer, text buffer, etc
   */

  if (editor.rx < editor.col_offset)
    editor.col_offset = editor.rx;

  if (editor.rx > editor.col_offset + editor.screen_cols)
    editor.col_offset = editor.rx - editor.screen_cols + 1;
}

/** **************************************************************************
 *
 *  @brief              Update the entire screen buffer with the text buffer
 *
 *  @param[in,out]      sb    This is screen buffer variable to write to
 *
 *  @return             void
 *
 *  @details
 *
 *  The screen buffer is updated by looping over the number of rows of terminal
 *  which are being used to render the editor. This function thus indexes to the
 *  correct level of indexing for the required amount of scroll and then loops
 *  over each line and appends each line character by character, whilst taking
 *  into account any possible syntax highlighting.
 *
 * ************************************************************************** */

void
editor_update_screen_buffer (SCREEN_BUF *sb)
{
  int iline;
  int padding;
  int char_colour;
  int current_colour;

  size_t i;
  size_t line_len;
  size_t buf_len;
  size_t col_len;
  size_t welcome_len;
  size_t file_row;

  char *c;
  char symbol;
  char welcome[80];
  char tmpbuf[16];
  unsigned char *hl;

  for (iline = 0; iline < editor.screen_rows; iline++)
  {
    /*
     * Index the line to offset for the current level of scroll in the file
     */

    file_row = (size_t) iline + editor.row_offset;

    /*
     * If the rendered line is more than the number of lines in the text buffer,
     * then render a ~ to indicate that the line is empty
     */

    if (file_row >= editor.nlines)
    {
      /*
       * If there are no lines in the text buffer, then write a welcome message
       */

      if (iline == editor.screen_rows / 5 && editor.nlines == 0)
      {
        welcome_len = (size_t ) snprintf (welcome, sizeof (welcome), "Kris editor -- version %s", VERSION);

        if (welcome_len > editor.screen_cols)
          welcome_len = (size_t) editor.screen_cols;

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
      {
        editor_add_to_screen_buf (sb, "~", 1);
      }
    }

    /*
     * Else, add a line of the text buffer to the screen buffer char by char
     */

    else
    {
      line_len = editor.lines[file_row].r_len - editor.col_offset;

      if (line_len < 0)
        line_len = 0;

      if (line_len > editor.screen_cols)
        line_len = (size_t) editor.screen_cols;

      c = &editor.lines[file_row].render[editor.col_offset];
      hl = &editor.lines[file_row].syn_hl[editor.col_offset];
      current_colour = -1;

      /*
       * This loop iterates over each char in the render array, and then processes
       * each character individually for syntax highlighting or special characters
       */

      for (i = 0; i < line_len; i++)
      {
        /*
         * This is to allow the editor to handle control sequences (non-printable
         * characters) a bit better
         */

        if ( iscntrl (c[i]))
        {
          symbol = (char) ((c[i] <= 26) ? '@' + c[i] : '?');
          editor_add_to_screen_buf (sb, "\x1b[7m", 4);
          editor_add_to_screen_buf (sb, &symbol, 1);
          editor_add_to_screen_buf (sb, "\x1b[m", 3);

          /*
           * If there is no current colour, renable normal text formatting
           */

          if (current_colour != -1)
          {
            col_len = (size_t) snprintf (tmpbuf, sizeof tmpbuf, "\x1b[%dm", current_colour);
            editor_add_to_screen_buf (sb, tmpbuf, col_len);
          }
        }

        /*
         * Append normal text escape character to the screen buffer
         */

        else if (hl[i] == HL_NORMAL)
        {
          if (current_colour != -1)
          {
            editor_add_to_screen_buf (sb, "\x1b[39m", 5);
            current_colour = -1;
          }

          editor_add_to_screen_buf (sb, &c[i], 1);
        }

        /*
         * Append the syntax highlighting escape characters to the screen buffer
         */

        else
        {
          char_colour = syntax_get_colour (hl[i]);

          if (char_colour != current_colour)
          {
            current_colour = char_colour;
            buf_len = (size_t) snprintf (tmpbuf, sizeof tmpbuf, "\x1b[%dm", char_colour);
            editor_add_to_screen_buf (sb, tmpbuf, buf_len);
          }

          editor_add_to_screen_buf (sb, &c[i], 1);
        }
      }

      editor_add_to_screen_buf (sb, "\x1b[39m", 5);
    }

    /*
     * Append a new line character to the screen buffer
     */

    editor_add_to_screen_buf (sb, "\x1b[K", 3);
    editor_add_to_screen_buf (sb, "\r\n", 2);
  }
}

/** **************************************************************************
 *
 *  @brief              Refresh the entire editor screen -- redraw everything
 *
 *  @return             void
 *
 *  @details
 *
 *  This function redraws the entire editor screen. This includes when a
 *  character in the text buffer has been moved, when the cursor has been moved
 *  or even when the terminal has been resized.
 *
 * ************************************************************************** */

void
editor_refresh_screen (void)
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

  /*
   * Reposition the cursor in the terminal window
   */

  snprintf (buf, sizeof buf, "\x1b[%d;%dH", (editor.cy - editor.row_offset) + 1, (editor.rx - editor.col_offset) + 1);
  editor_add_to_screen_buf (&sb, buf, (int) strlen (buf));

  /*
   * Enable set mode in the terminal (VT100 again) and write out the entire
   * buffer to screen
   */

  editor_add_to_screen_buf (&sb, "\x1b[?25h", 6);
  write (STDOUT_FILENO, sb.buf, sb.len);

  free (sb.buf);
}
