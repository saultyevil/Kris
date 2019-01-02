#ifndef KILO_KILO_H
#define KILO_KILO_H


/* ***************************************************************************
 *
 * @file kilo.h
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


/*
 * Some includes
 */

#include <stddef.h>
#include <termios.h>

#define _BSD_SOURCE
#define _GNU_SOURCE

/*
 * Constant macro definitions
 */

#define TRUE 1
#define FALSE 0
#define VERSION "0.0.1"
#define TAB_WIDTH 8

/*
 * Macro definitions
 */

#define CTRL_KEY(k) ((k) & 0x1f)  // This is some magical bitshifting macro for control
#define SBUF_INIT {0, NULL}       // Init a screen buffer

/*
 * Structures
 */

typedef struct
{
  size_t len, r_len;
  char *chars;
  char *render;
} eline;

typedef struct
{
  eline *lines;
  char *filename;
  int cx, cy;
  int rx;
  int n_lines;
  int row_offset, col_offset;
  int n_screen_cols, n_screen_rows;
  struct termios curr_term_attr;
  struct termios orig_term_attr;
} EDITOR_CONFIG;

EDITOR_CONFIG editor;

typedef struct
{
  size_t len;
  char *buf;
} SCREEN_BUF;

SCREEN_BUF obuf;


enum keymap
{
  ARROW_LEFT  = 1000,
  ARROW_RIGHT = 1001,
  ARROW_UP    = 1002,
  ARROW_DOWN  = 1003,
  PAGE_UP     = 1004,
  PAGE_DOWN   = 1005,
  HOME_KEY    = 1006,
  END_KEY     = 1007,
  DEL_KEY     = 1008
};

#include "global_functions.h"

#endif //KILO_KILO_H
