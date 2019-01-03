#ifndef KRIS_KRIS_H
#define KRIS_KRIS_H


/* ***************************************************************************
 *
 * @file kris.h
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

#include <time.h>
#include <stddef.h>
#include <termios.h>

#define _BSD_SOURCE
#define _GNU_SOURCE

/*
 * Constant macro definitions
 */

#define TRUE 1
#define FALSE 0
#define VERSION "0.8.3"
#define TAB_WIDTH 8
#define QUIT_TIMES 2



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
  unsigned char *hl;
} ELINE;

typedef struct
{
  size_t len;
  char *buf;
} SCREEN_BUF;

typedef struct
{
  char *filetype;
  char **filematch;
  int flags;
} SYNTAX;

typedef struct
{
  ELINE *lines;
  char *filename;
  int modified;
  char status_msg[80];
  time_t status_msg_time;
  int cx, cy;
  int rx;
  int nlines;
  int row_offset, col_offset;
  int n_screen_cols, n_screen_rows;
  struct termios curr_term_attr;
  struct termios orig_term_attr;
  SYNTAX *syntax;
} EDITOR_CONFIG;

EDITOR_CONFIG editor;

enum keymap
{
  BACK_SPACE  = 127 ,
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

enum syntax_highlight_colours
{
  HL_NORMAL   = 0,
  HL_NUMBER   = 1,
  HL_MATCH    = 2,
  HL_STRING   = 3
};


#include "global_functions.h"

#endif
