/** **************************************************************************
 *
 * @file kris.h
 *
 * @date 01/01/2019
 *
 * @author E. J. Parkinson
 *
 * @brief Global variables, functions and macro definitions.
 *
 * ************************************************************************** */

#ifndef KRIS_KRIS_H
#define KRIS_KRIS_H

#include <time.h>
#include <stddef.h>
#include <termios.h>

/* **************************************************************************
 *
 * Constant macro definitions
 *
 * ************************************************************************* */

#define TRUE 1
#define FALSE 0
#define SUCCESS 0
#define FAILURE -1
#define VERSION "2.0.0"
#define TAB_WIDTH 8
#define QUIT_TIMES 1

// This is some magical bitshifting macro for control sequences
#define CTRL_KEY(k) ((k) & 0x1f)
// Screen buffer initialisation buffer
#define SBUF_INIT {0, NULL}

/* **************************************************************************
 *
 * Data structures
 *
 * EDITOR_LINE:
 *  Contains all of the data types required to store a text line in memory.
 *
 * SCREEN_BUF:
 *  Contains all of the data required to render the text buffers
 *
 * SYNTAX:
 *  Contains all of the data required to track syntax highlighting
 *
 * EDITOR_CONFIG:
 *  Contains all of the data for the editor.
 *
 * ************************************************************************** */

typedef struct EDITOR_LINE
{
  int idx;             // Line number
  size_t len, r_len;   // Length of the char and render arrays
  char *chars;         // The raw chars read in
  char *render;        // The chars which are displayed (spaces instead of tab)
  unsigned char *syn_hl;   // The syntax highlighting
  int hl_open_comment; // The line index of where an multi line comment starts
} EDITOR_LINE;

typedef struct SCREEN_BUF
{
  size_t len;  // Length of the screen buffer
  char *buf;   // Char array storing the screen buffer
} SCREEN_BUF;

typedef struct SYNTAX
{
  char *filetype;              // The file type of the text buffer
  char **filematch;            // The possible file types for syntax
  char **keywords;             // The keywords to be highlighted
  char *pre_processor;         // Preprocessor symbol
  char *single_line_comment;   // Single line comment symbol
  char *ml_comment_start;      // Multi line comment starting symbol
  char *ml_comment_end;        // Multi line comment ending symbol
  int flags;                   // Highlighting flags
} SYNTAX;

typedef struct EDITOR_CONFIG
{
  EDITOR_LINE *lines;
  char *filename;                  // Filename of the text buffer
  int modified;                    // Bool flag to indicate if file modified
  char status_msg[80];             // Status message for the editor
  time_t status_msg_time;          // Used to check when status message updated
  int cx, cy;                      // Cursor x and y location
  int rx;                          // Cursor location in render array
  int nlines;                      // Number of lines in text buffer
  int row_offset, col_offset;      // Row and col offset for scrolling
  int screen_cols, screen_rows;    // Number of rows and cols for terminal
  struct termios curr_term_attr;   // Raw terminal attributes
  struct termios orig_term_attr;   // Original terminal attributes
  SYNTAX *syntax;                  // Syntax highlighting data
} EDITOR_CONFIG;

EDITOR_CONFIG editor;

/* **************************************************************************
 *
 * Enumerators
 *
 * keymap:
 *  Maps key presses to internal numbers
 *
 * syntax_highlight_colours:
 *  Maps syntax highlighting types to internal numbers
 *
 * ************************************************************************** */

enum keymap
{
  BACK_SPACE  = 127,
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
  HL_NORMAL     = 0,
  HL_NUMBER     = 1,
  HL_MATCH      = 2,
  HL_STRING     = 3,
  HL_COMMENT    = 4,
  HL_KEYWORD1   = 5,
  HL_KEYWORD2   = 6,
  HL_ML_COMMENT = 7,
  HL_PREPROCESS = 8
};


/* **************************************************************************
 *
 * Functions
 *
 * ************************************************************************** */

// E
void editor_delete_char (void);
void editor_init (void);
void editor_insert_char (int c);
void editor_insert_new_line (void);
void editor_refresh_screen (void);
void editor_set_status_message (char *fmt, ...);
void editor_add_to_render_buffer (EDITOR_LINE *line);

// F
void find (void);

// I
int io_read_file (char *filename);
void io_save_file (void);
char *io_status_bar_prompt (char *prompt_msg, void (*callback) (char *, int));

// K
void kp_process_keypress (void);
int kp_read_keypress (void);

// L
void line_add_string_to_text_buffer (EDITOR_LINE *dest_line, char *src,
                                     size_t append_len);
void line_add_to_text_buffer (int insert_index, char *s, size_t line_len);
void line_delete_char (EDITOR_LINE *line, int insert_idx);
void line_delete (int idx);
void line_insert_char (EDITOR_LINE *line, int insert_idx, int c);

// S
int syntax_get_colour (int hl);
void syntax_select_highlighting (void);
void syntax_update_highlighting (EDITOR_LINE *line);

// T
void terminal_init (void);
int terminal_get_cursor_position (int *nrows, int *ncols);
void terminal_update_size (int unused);

// U
void util_clean_memory (void);
int util_convert_cx_to_rx (EDITOR_LINE *line, int cx);
int util_convert_rx_to_cx (EDITOR_LINE *line, int rx);
void util_exit (char *s);
void util_free_line (EDITOR_LINE *line);
void util_reset_display (void);

#endif
