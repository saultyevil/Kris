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

#include "kris.h"


// Initialise the editor
void editor_init (void)
{
  int unused = 0;  // This var is unused but passed for signal

  // Set a initial values for the editor configuration
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

  /*
   * Get the size of the terminal window and use signal to monitor if the
   * terminal window changes in size to update the size on the fly
   */

  terminal_update_size (unused);
  signal (SIGWINCH, terminal_update_size);
}
