/** **************************************************************************
 *
 * @file key_input.c
 *
 * @date 01/01/2019
 *
 * @author E. J. Parkinson
 *
 * @brief Functions for processing key input.
 *
 * ************************************************************************** */

#include <errno.h>
#include <stdlib.h>
#include <unistd.h>

#include "kris.h"

/** **************************************************************************
 *
 *  @brief              Move the cursor around the terminal
 *
 *  @param[in]
 *  @param[in]
 *
 *  @return             void
 *
 *  @details
 *
 *
 * ************************************************************************** */

void
kp_move_cursor (int key)
{
  size_t line_len;
  EDITOR_LINE *line;

  // Create a line for the case where the cursor is on the last line
  line = (editor.cy >= editor.nlines) ? NULL : &editor.lines[editor.cy];

  switch (key)
  {
    case ARROW_UP:
      if (editor.cy != 0)
        editor.cy--;
      break;
    case ARROW_DOWN:
      if (editor.cy < editor.nlines)
        editor.cy++;
      break;
    case ARROW_RIGHT:
      if (line && editor.cx < line->len)
        editor.cx++;
      else if (line && editor.cx == line->len)  // Go to next line
      {
        editor.cy++;
        editor.cx = 0;
      }
      break;
    case ARROW_LEFT:
      if (editor.cx != 0)
        editor.cx--;
      else if (editor.cy > 0)  // Go to previous line
      {
        editor.cy--;
        editor.cx = (int) editor.lines[editor.cy].len;
      }
      break;
    default:
      break;
  }

  // Snap the cursor to the end of a shorter line
  line = (editor.cy >= editor.nlines) ? NULL : &editor.lines[editor.cy];
  line_len = line ? line->len : 0;
  if (editor.cx > line_len)
    editor.cx = (int) line_len;
}

/** **************************************************************************
 *
 *  @brief              Read a key press from the terminal
 *
 *  @param[in]
 *  @param[in]
 *
 *  @return             void
 *
 *  @details
 *
 *
 * ************************************************************************** */

int
kp_read_keypress (void)
{
  char c;
  char seq[3];
  ssize_t nread;

  // Read in a single char from the terminal
  while ((nread = read (STDIN_FILENO, &c, 1)) != 1)
  {
    if (nread == -1 && errno != EAGAIN)
      util_exit ("Too many chars read in at once, expected 1 char");
  }

  /*
   * If the input is an escape sequence, then we will process this some more.
   * If an escape sequence is not read, then we the char read in is returned.
   */

  if (c == '\x1b')
  {
    if (read (STDIN_FILENO, &seq[0], 1) != 1)
      return '\x1b';
    if (read (STDIN_FILENO, &seq[1], 1) != 1)
      return '\x1b';

    // Deal with the various cases of escape sequences
    if (seq[0] == '[')
    {
      if (seq[1] >= '0' && seq[1] <= '9')
      {
        if (read (STDIN_FILENO, &seq[2], 1) != 1)
          return '\x1b';

        if (seq[2] == '~')
        {
          switch (seq[1])
          {
            case '1':
              return HOME_KEY;
            case '3':
              return DEL_KEY;
            case '4':
              return END_KEY;
            case '5':
              return PAGE_UP;
            case '6':
              return PAGE_DOWN;
            case '7':
              return HOME_KEY;
            case '8':
              return END_KEY;
            default:
              break;
          }
        }
      }
      else
      {
        switch (seq[1])
        {
          case 'A':
            return ARROW_UP;
          case 'B':
            return ARROW_DOWN;
          case 'C':
            return ARROW_RIGHT;
          case 'D':
            return ARROW_LEFT;
          case 'H':
            return HOME_KEY;
          case 'F':
            return END_KEY;
          default:
            return '\x1b';
        }
      }
    }
    else if (seq[0] == 'O')
    {
      switch (seq[1])
      {
        case 'H':
          return HOME_KEY;
        case 'F':
          return END_KEY;
        default:
          return '\x1b';
      }
    }
  }

  return c;
}

/** **************************************************************************
 *
 *  @brief              Process a key input from the terminal
 *
 *  @param[in]
 *  @param[in]
 *
 *  @return             void
 *
 *  @details
 *
 *
 * ************************************************************************** */

void
kp_process_keypress (void)
{
  int c;
  int nreps;
  static int quit_times = QUIT_TIMES;

  switch (c = kp_read_keypress ())
  {
    // Append a new line
    case '\r':
      editor_insert_new_line ();
      break;
    // Exit
    case CTRL_KEY ('q'):
      if (editor.modified && quit_times > 0)
      {
        editor_set_status_message ("File has unsaved changes, "
               "press Ctrl-Q %d more times to quit without saving", quit_times);
        quit_times--;
        return;
      }
      util_reset_display ();
      exit (SUCCESS);
    // Save the text buffer to file
    case CTRL_KEY ('s'):
      io_save_file ();
      break;
    // Find a keyword in the text buffer
    case CTRL_KEY ('f'):
      find ();
      break;
    // Navigate using HOME and END keys for end and start of column
    case HOME_KEY:
      editor.cx = 0;
      break;
    case END_KEY:
      if (editor.cy < editor.nlines)
        editor.cx = (int) editor.lines[editor.cy].len;
      break;
    // Navigate up and down the text buffer using PAGE UP and PAGE DOWN
    case PAGE_UP:
    case PAGE_DOWN:
      if (c == PAGE_UP)
        editor.cy = editor.row_offset;
      else
      {
        editor.cy = editor.row_offset + editor.screen_rows - 1;
        if (editor.cy > editor.nlines)
          editor.cy = editor.nlines;
      }
      nreps = editor.screen_rows;
      while (nreps--)
        kp_move_cursor (c == PAGE_UP ? ARROW_UP : ARROW_DOWN);
      break;
    // Navigate the text buffer using the arrow keys
    case ARROW_UP:
    case ARROW_DOWN:
    case ARROW_RIGHT:
    case ARROW_LEFT:
      kp_move_cursor (c);
      break;
    // Remove chars from the text buffer
    case BACK_SPACE:
    case DEL_KEY:
    case CTRL_KEY('h'):
      // In the case of delete, move the cursor right one step first
      if (c == DEL_KEY)
        kp_move_cursor (ARROW_RIGHT);
      editor_delete_char ();
      break;
    // Ignore these keys
    case CTRL_KEY ('l'):
    case '\x1b':  // Escape key
      break;
    // Insert a char into the text buffer
    default:
      editor_insert_char (c);
      break;
  }

  quit_times = QUIT_TIMES;
}
