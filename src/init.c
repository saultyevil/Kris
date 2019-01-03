/* ***************************************************************************
 *
 * @file init.c
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



#include <stdlib.h>

#include "kris.h"


// Initialise the editor
void init_editor (void)
{
  // Set a bunch of initial values for the editor variables
  editor.cx = 0;
  editor.cy = 0;
  editor.rx = 0;
  editor.row_offset = 0;
  editor.col_offset = 0;
  editor.nlines = 0;
  editor.lines = NULL;
  editor.filename = NULL;
  editor.modified = FALSE;
  editor.status_msg[0] = '\0';
  editor.status_msg_time = 0;
  editor.syntax = NULL;

  // Get the size of the terminal window
  if (get_terminal_size (&editor.n_screen_cols, &editor.n_screen_rows) == -1)
    error ("Couldn't determine the size of the terminal window");

  // Remove a two rows as we will draw a status bar and message on these rows
  editor.n_screen_rows -= 2;
}
