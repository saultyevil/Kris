/* ***************************************************************************
 *
 * @file key_input.c
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
#include <stdlib.h>
#include <unistd.h>

#include "kilo.h"


// Move the cursor around the terminal
void move_cursor (int key)
{
  size_t line_len;

  // Create a row variable for the case where the cursor is on the last line
  // of the file and use ternary operator to point to the last line if so
  erow *line;
  line = (editor.cy >= editor.n_editor_rows) ? NULL : &editor.lines[editor.cy];

  switch (key)
  {
    case ARROW_UP:
      if (editor.cy != 0)
        editor.cy--;
      break;
    case ARROW_DOWN:
      if (editor.cy < editor.n_editor_rows)
        editor.cy++;
      break;
    case ARROW_RIGHT:
      if (line && editor.cx < line->len)
        editor.cx++;
      // Add ability to press right to go to the start of the next line
      else if (line && editor.cx == line->len)
      {
        editor.cy++;
        editor.cx = 0;
      }
      break;
    case ARROW_LEFT:
      if (editor.cx != 0)
        editor.cx--;
      // Add ability to press left to go to the end of the previous line
      else if (editor.cy > 0)
      {
        editor.cy--;
        editor.cx = (int) editor.lines[editor.cy].len;
      }
      break;
    default:
      break;
  }

  // Snap the cursor to the end of the line if moving from a longer to a shorter
  // line whilst scrolling
  line = (editor.cy >= editor.n_editor_rows) ? NULL : &editor.lines[editor.cy];
  line_len = line ? line->len : 0;
  if (editor.cx > line_len)
    editor.cx = (int) line_len;
}

// Read a key press from the terminal
int read_keypress (void)
{
  char c;
  char seq[3];
  ssize_t nread;

  while ((nread = read (STDIN_FILENO, &c, 1)) != 1)
  {
    if (nread == -1 && errno != EAGAIN)
      error ("Too many chars read in at once, expected 1 char");
  }

  // If the input is an escape sequence, then we will process this some more.
  // If an escape sequence is not read, then we the char read in is returned.
  if (c == '\x1b')
  {
    if (read (STDIN_FILENO, &seq[0], 1) != 1)
      return '\x1b';
    if (read (STDIN_FILENO, &seq[1], 1) != 1)
      return '\x1b';

    // Deal with the various cases of escape sequences which are possible
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
            case '1': return HOME_KEY;
            case '3': return DEL_KEY;
            case '4': return END_KEY;
            case '5': return PAGE_UP;
            case '6': return PAGE_DOWN;
            case '7': return HOME_KEY;
            case '8': return END_KEY;
            default: break;
          }
        }
      }
      else
      {
        switch (seq[1])
        {
          case 'A': return ARROW_UP;
          case 'B': return ARROW_DOWN;
          case 'C': return ARROW_RIGHT;
          case 'D': return ARROW_LEFT;
          case 'H': return HOME_KEY;
          case 'F': return END_KEY;
          default: return '\x1b';
        }
      }
    }
    else if (seq[0] == 'O')
    {
      switch (seq[1])
      {
        case 'H': return HOME_KEY;
        case 'F': return END_KEY;
        default: return '\x1b';
      }
    }
  }

  return c;
}

// Process a key input from the terminal
void process_keypress (void)
{
  int c;
  int nreps;

  c = read_keypress ();

  switch (c)
  {
    // Quit the editor
    case CTRL_KEY ('q'):
      reset_display ();
      exit (0);
      // Other keys...
    case HOME_KEY:
      editor.cy = 0;
      break;
    case END_KEY:
      editor.cy = editor.n_screen_rows - 1;
      break;
    case PAGE_UP:
    case PAGE_DOWN:
      nreps = editor.n_screen_rows;
      while (nreps--)
        move_cursor (c == PAGE_UP ? ARROW_UP : ARROW_DOWN);
      break;
    case ARROW_UP:
    case ARROW_DOWN:
    case ARROW_RIGHT:
    case ARROW_LEFT:
      move_cursor (c);
    default:
      break;
  }
}
