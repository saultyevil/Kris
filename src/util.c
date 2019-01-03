/* ***************************************************************************
 *
 * @file util.c
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

#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ioctl.h>

#include "kilo.h"


// Kill the program and print an error message and string s
void error (char *s)
{
  reset_display ();
  perror (s);
  exit (errno);
}

// Refresh the terminal screen
void reset_display (void)
{
  // \x1b is the escape character
  // \x1b[ is an escape sequence
  write (STDOUT_FILENO, "\x1b[2J", 4);   // erase screen
  write (STDOUT_FILENO, "\x1b[H", 3);    // reposition the cursor to default
}

// Get the current position of the cursor
int get_cursor_position (int *nrows, int *ncols)
{
  int i;
  char buf[32];

  if (write (STDOUT_FILENO, "\x1b[6n", 4) != 4)
    return -1;

  i = 0;
  while (i < sizeof (buf) - 1)
  {
    if (read (STDIN_FILENO, &buf[i], 1) != 1)
      break;
    if (buf[i] == 'R')
      break;
    i++;
  }

  buf[i] = '\0';

  if (buf[0] != '\x1b' || buf[1] != '[')
    return -1;
  if (sscanf (&buf[2], "%d;%d", ncols, nrows) != 2)
    return -1;

  return 0;
}

// Determine the size of the terminal window
int get_terminal_size (int *ncols, int *nrows)
{
  struct winsize ws;

  // If ioctl fails on the machine, we have to do this ugly workaround where we
  // keep moving the cursor until the reach the end
  if (ioctl (STDOUT_FILENO, TIOCGWINSZ, &ws) == -1 || ws.ws_col == 0)
  {
    if (write (STDOUT_FILENO, "\x1b[999C\x1b[999B", 12) != 12)
      return -1;
    return get_cursor_position (nrows, ncols);
  }
  else
  {
    *ncols = ws.ws_col;
    *nrows = ws.ws_row;
  }

  return 0;
}
