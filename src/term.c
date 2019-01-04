/* ***************************************************************************
 *
 * @file term.c
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



#include <unistd.h>
#include <stdlib.h>
#include <termios.h>

#include "kris.h"


// Allows us to set the original terminal flags at exit
void disable_raw_term_and_clean_up (void)
{
  size_t i;

  if (tcsetattr (STDIN_FILENO, TCSAFLUSH, &editor.orig_term_attr) == -1)
    error ("Can't set terminal attributes");

  for (i = 0; i < editor.nlines; i++)
    free_line (&editor.lines[i]);

  // Free this memory in case of memory leaks
  free (editor.filename);
  free (editor.lines);
}

// Enable raw terminal mode by changing terminal flags
void init_terminal (void)
{
  struct termios raw_term;

  // Get the original terminal attributes before enabling raw mode
  if (tcgetattr (STDIN_FILENO, &editor.orig_term_attr) == -1)
    error ("Can't get terminal attributes");

  // By using atexit, the terminal will be returned to normal at exit
  atexit (disable_raw_term_and_clean_up);

  // Enable raw terminal with some weird bit level operations
  raw_term = editor.orig_term_attr;
  // Control flags: Set 8 bit chars
  raw_term.c_cflag |= (CS8);
  // Output modes: disable post processing
  raw_term.c_oflag &= ~(OPOST);
  // Local modes: echoing off, canonical off, no extended functions, no signal
  // chars
  raw_term.c_lflag &= ~(ECHO | ICANON | IEXTEN | ISIG);
  // Input modes: no break, no CR to NL, no parity check, no strip char,
  // no start/stop output control
  raw_term.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);

  // Timeout options for reading in
  raw_term.c_cc[VMIN] = 0;    // return when anything passed to term
  raw_term.c_cc[VTIME] = 1;   // length of time to wait for read () to return in tenths of seconds

  // Apply the attributes to the terminal
  editor.curr_term_attr = raw_term;
  if (tcsetattr (STDIN_FILENO, TCSAFLUSH, &raw_term) == -1)
    error ("Can't set terminal attributes");
}
