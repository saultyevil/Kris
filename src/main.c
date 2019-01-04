/* ***************************************************************************
 *
 * @file main.c
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

#include "kris.h"


// Main control function of kilo
int main (int argc, char *argv[])
{
  int file_found;

  init_terminal ();
  init_editor ();

  file_found = FALSE;
  if (argc >= 2)
    file_found = open_file (argv[1]);

  if (file_found)
    set_status_message (
                      "HELP: Ctrl-S to save | Ctrl-F to find | Ctrl-Q to quit");

  while (TRUE)
  {
    refresh_editor_screen ();
    process_keypress ();
    if (errno != 0)
    {
      error ("Unknown error :-(");
      break;
    }
  }

  return 0;
}
