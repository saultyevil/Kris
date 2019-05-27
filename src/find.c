/** **************************************************************************
 *
 * @file find.c
 *
 * @date 03/01/2019
 *
 * @author E. J. Parkinson
 *
 * @brief Functions for finding a keyword in the text buffer.
 *
 * ************************************************************************** */

#include <string.h>
#include <stdlib.h>

#include "kris.h"

/** **************************************************************************
 *
 *  @brief             Search for a keyword within the text buffer
 *
 *  @param[in]          *query   A query message?
 *  @param[in]          key      The variable for a key press?
 *
 *  @return             void
 *
 *  @details
 *
 *  I can't really remember how this works right now.
 *
 *  This function iterates over each line in the text buffer and uses the function
 *  strstr to query if the substring query is in the current line of the text
 *  buffer. If it is, then this location is recorded and the cursor is moved to
 *  this string and it is also highlighted.
 *
 * ************************************************************************** */

void
find_keyword_search (char *query, int key)
{
  EDITOR_LINE *line;

  size_t i;

  int current;
  static int last_match = -1;
  static int direction = 1;
  static int saved_hl_line;

  char *match;
  static char *saved_hl = NULL;

  /*
   * If there is a previous highlight, then return the original highlight colour
   */

  if (saved_hl)
  {
    memcpy (editor.lines[saved_hl_line].syn_hl, saved_hl, editor.lines[saved_hl_line].r_len);
    free (saved_hl);
    saved_hl = NULL;
  }

  /*
   * Process the key press - enter or escape will return without doing anything
   */

  if (key == '\r' || key == '\x1b')
  {
    last_match = -1;
    direction = 1;
    return;
  }

  /*
   * Search forwards in the file
   */

  else if (key == ARROW_RIGHT || key == ARROW_DOWN)
  {
    direction = 1;
  }

  /*
   * Search backwards in the file
   */

  else if (key == ARROW_LEFT || key == ARROW_UP)
  {
    direction = -1;
  }

  /*
   * Any other key will search forwards in the file
   */

  else
  {
    last_match = -1;
    direction = 1;
  }

  /*
   * Set last match to be the line where the query was last matched
   */

   if (last_match == -1)
    direction = 1;

   current = last_match;

  for (i = 0; i < editor.nlines; i++)
  {
    /*
     * Increment current line search counter
     */

   current += direction;

    if (current == -1)
    {
      current = editor.nlines - 1;
    }
    else if (current == editor.nlines)
    {
      current = 0;
    }

    /*
     * Use strstr to check if the query substring is on the current row. If it
     * isn't, then strstr returns NULL. To find the index, we can do some
     * pointer arithmetic
     */

    line = &editor.lines[current];
    match = strstr (line->render, query);

    if (match)
    {
      last_match = current;
      editor.cy = current;
      editor.cx = util_convert_rx_to_cx (line, (int) (match - line->render));
      editor.row_offset = editor.nlines;

      /*
       * Set the matched substrings to be HL_MATCH colour
       */

      saved_hl_line = current;
      saved_hl = malloc (line->r_len);
      memcpy (saved_hl, line->syn_hl, line->r_len);
      memset (&line->syn_hl[match - line->render], HL_MATCH, strlen (query));
      break;
    }
  }
}

/** **************************************************************************
 *
 *  @brief              The main find control function for finding a string
 *
 *  @return             void
 *
 *  @details
 *
 *  This is the main control function of the string searching feature. The status
 *  bar is updated using io_status_bar_prompt and find_keyword_search is used
 *  to call this function recursively until the users wants to exit the
 *  search feature.
 *
 * ************************************************************************** */

void
find (void)
{
  int saved_cx;
  int saved_cy;
  int saved_col_offset;
  int saved_row_offset;

  char *query;

  /*
   * Save the original position of the cursor and text buffer before search
   */

  saved_cx = editor.cx;
  saved_cy = editor.cy;
  saved_col_offset = editor.col_offset;
  saved_row_offset = editor.row_offset;

  /*
   * Set status bar prompt and call to keyword search function
   */

  if ((query = io_status_bar_prompt ("Search: %s (ESC to cancel | Arrows keys to search)", find_keyword_search)))
  {
    free (query);
  }

  /*
   * Restore original cursor position upon escape exit of find
   */

  else
  {
    editor.cx = saved_cx;
    editor.cy = saved_cy;
    editor.col_offset = saved_col_offset;
    editor.row_offset = saved_row_offset;
  }
}
