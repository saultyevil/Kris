/** **************************************************************************
 *
 * @file util.c
 *
 * @date 01/01/2019
 *
 * @author E. J. Parkinson
 *
 * @brief Utility functions for Kris.
 *
 * ************************************************************************** */


#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "kris.h"


//! @brief Refresh the terminal screen
void util_reset_display (void)
{
  /*
   * \x1b is the escape character
   * \x1b[ is an escape sequence
   */

  write (STDOUT_FILENO, "\x1b[2J", 4);   // erase screen
  write (STDOUT_FILENO, "\x1b[H", 3);    // reposition the cursor
}

//! @brief Kill the program and print an error message and string s
void util_exit (char *s)
{
  util_reset_display ();
  perror (s);
  exit (errno);
}

//! @brief Convert the cursor pos in chars array to a pos in render array
int util_convert_cx_to_rx (EDITOR_LINE *line, int cx)
{
  int rx;
  size_t i;

  /*
   * Loop over all of the chars to the left of cx and count how many spaces
   * each tab takes up
   */

  rx = 0;
  for (i = 0; i < cx; i++)
  {
    if (line->chars[i] == '\t')
      rx += (TAB_WIDTH - 1) - (rx % TAB_WIDTH);
    rx++;
  }

  return rx;
}

//! @brief Convert the cursor pos in render array to a pos in the char array
int util_convert_rx_to_cx (EDITOR_LINE *line, int rx)
{
  size_t cx;
  int cur_rx;

  /*
   * Loop over the chars array and increment until cx reaches the same size as
   * rx
   */

  cur_rx = 0;
  for (cx = 0; cx < line->len; cx++)
  {
    if (line->chars[cx] == '\t')
      cur_rx += (TAB_WIDTH - 1) - (cur_rx % TAB_WIDTH);
    cur_rx++;

    if (cur_rx > rx)
      return (int) cx;
  }

  return (int) cx;
}

//! @brief Free memory to avoid any memory leaks at exit
void util_clean_memory (void)
{
  size_t i;

  for (i = 0; i < editor.nlines; i++)
    util_free_line (&editor.lines[i]);

  free (editor.filename);
  free (editor.lines);
}
