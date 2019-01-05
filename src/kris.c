/** **************************************************************************
 *
 * @file kris.c
 *
 * @date 01/01/2019
 *
 * @author E. J. Parkinson
 *
 * @brief Contains the main function.
 *
 * ************************************************************************** */


#include <errno.h>

#include "kris.h"


//! @brief Main control function of Kris
int main (int argc, char *argv[])
{
  int file_found;

  terminal_init ();
  editor_init ();

  file_found = FALSE;
  if (argc >= 2)
    file_found = io_read_file (argv[1]);

  if (file_found)
    editor_set_status_message (
      "HELP: Ctrl-S to save | Ctrl-F to find | Ctrl-Q to quit");

  while (TRUE)
  {
    editor_refresh_screen ();
    kp_process_keypress ();
    if (errno != 0)
    {
      util_exit ("Unknown error :-(");
      break;
    }
  }

  return 0;
}
