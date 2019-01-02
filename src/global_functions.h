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
void append_string_to_line (eline *dest_line, char *src, size_t append_len);
void append_to_screen_buf (SCREEN_BUF *sb, char *s, size_t len);
void append_line_to_text_buffer (int insert_index, char *s, size_t line_len);
// D
void delete_char (void);
void delete_char_in_line (eline *line, int insert_idx);
void delete_line (int idx);
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
void insert_char (int c);
void insert_char_in_line (eline *line, int insert_idx, int c);
void insert_new_line (void);
// O
void open_file (char *filename);
// P
void process_keypress (void);
// R
int read_keypress (void);
void reset_display (void);
// S
void save_file (void);
void set_status_message (char *fmt, ...);
char *status_bar_prompt (char *prompt_msg);
// U
void update_to_render_buffer (eline *line);

#endif //KILO_FUNCTIONS_H
