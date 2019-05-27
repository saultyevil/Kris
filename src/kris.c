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
#include <stdlib.h>

#include "kris.h"

/** **************************************************************************
 *
 *  @brief              Main control function of Kris
 *
 *  @param[in]          argc    The number of command line arguments provided
 *                              to the program
 *  @param[in]          argv    The command line provided to the program
 *
 *  @return             EXIT_SUCCESS or EXIT_FAILURE as defined in stdlib.h
 *
 *  @details
 *
 *  This is the main function of Kris. It initialises the terminal and basic
 *  editor variables and will either create a new text buffer or read one in
 *  from file.
 *
 *  The function then performs a loop where the screen is refreshed after
 *  some keyboard input has been refreshed. This happens each time a character
 *  is input, which means Kris updates on a char by char basis.
 *
 * ************************************************************************** */

int
main (int argc, char *argv[])
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
      return EXIT_FAILURE;
    }
  }

  return EXIT_SUCCESS;
}

/** **************************************************************************
 *
 *  @brief
 *
 *  @param[in]
 *  @param[in]
 *
 *  @return             void
 *
 *  @details
 *
 *
 * ************************************************************************** */
