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
  init_terminal ();
  init_editor ();

  if (argc >= 2)
    open_file (argv[1]);

  set_status_message ("HELP: Ctrl-S to save | Ctrl-F to find | Ctrl-Q to quit");

  while (TRUE)
  {
    draw_editor_screen ();
    process_keypress ();
    if (errno != 0)
      break;
  }

  return 0;
}
