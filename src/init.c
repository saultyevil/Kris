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

#include "kilo.h"


// Initialise the editor
void init_editor (void)
{
  // Set the initial cursor position
  editor.cx = 0;
  editor.cy = 0;
  editor.rx = 0;
  editor.row_offset = 0;
  editor.col_offset = 0;
  editor.n_lines = 0;
  editor.lines = NULL;
  editor.filename = NULL;

  // Get the size of the terminal window
  if (get_terminal_size (&editor.n_screen_cols, &editor.n_screen_rows) == -1)
    error ("Couldn't determine the size of the terminal window");
  // Remove a single row as we will draw a status bar on the bottom row
  editor.n_screen_rows -= 1;
}
