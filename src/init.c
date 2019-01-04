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


#include <signal.h>
#include <stdlib.h>

#include "kris.h"


// Initialise the editor
void init_editor (void)
{
  int unused = 0;

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

  // Get the size of the terminal window and use signal to monitor if the
  // terminal window changes in size or not
  update_terminal_size (unused);
  signal (SIGWINCH, update_terminal_size);
}
