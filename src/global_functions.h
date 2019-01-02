#ifndef KILO_FUNCTIONS_H
#define KILO_FUNCTIONS_H


/* ***************************************************************************
 *
 * @file global_functions.h
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


#include <stddef.h>


// A
void append_to_screen_buf (SCREEN_BUF *sb, char *s, size_t len);
void append_to_text_buffer (char *s, size_t linelen);
// D
void draw_editor_screen (void);
// E
void error (char *s);
// F
void free_screen_buf (SCREEN_BUF *sb);
// G
int get_terminal_size (int *ncols, int *nrows);
// I
void init_editor (void);
void init_terminal (void);
// O
void open_file (char *filename);
// P
void process_keypress (void);
// R
int read_keypress (void);
void reset_display (void);

#endif //KILO_FUNCTIONS_H
