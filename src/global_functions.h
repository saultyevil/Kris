#ifndef KRIS_FUNCTIONS_H
#define KRIS_FUNCTIONS_H


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


// A
void append_string_to_line (ELINE *dest_line, char *src, size_t append_len);
void append_to_screen_buf (SCREEN_BUF *sb, char *s, size_t len);
void append_line_to_text_buffer (int insert_index, char *s, size_t line_len);
// C
int convert_rx_to_cx (ELINE *line, int rx);
// D
void delete_char (void);
void delete_char_in_line (ELINE *line, int insert_idx);
void delete_line (int idx);
void refresh_editor_screen (void);
// E
void error (char *s);
// F
void find (void);
void free_screen_buf (SCREEN_BUF *sb);
void free_line (ELINE *line);
// G
int get_syntax_colour (int hl);
int get_terminal_size (int *ncols, int *nrows);
// I
void init_editor (void);
void init_terminal (void);
void insert_char (int c);
void insert_char_in_line (ELINE *line, int insert_idx, int c);
void insert_new_line (void);
// O
int open_file (char *filename);
// P
void process_keypress (void);
// R
int read_keypress (void);
void reset_display (void);
// S
void save_file (void);
void select_syntax_highlighting (void);
void set_status_message (char *fmt, ...);
char *status_bar_prompt (char *prompt_msg, void (*callback)(char *, int));
// U
void update_syntax_highlight (ELINE *line);
void update_to_render_buffer (ELINE *line);
void update_terminal_size (int unused);

#endif //KILO_FUNCTIONS_H
