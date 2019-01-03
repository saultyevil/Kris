/* ***************************************************************************
 *
 * @file find.c
 *
 * @date 03/01/2019
 *
 * @author E. J. Parkinson
 *
 * @brief
 *
 * @details
 *
 * ************************************************************************** */

#include <stdlib.h>
#include <string.h>

#include "kris.h"

// Search for a keyword within the text buffer
void keyword_search (char *query, int key)
{
  size_t i;
  int current;
  char *match;
  ELINE *line;
  static int last_match = -1;
  static int direction = 1;
  static int saved_hl_line;
  static char *saved_hl = NULL;

  // If previously highlighted, return the original highlight colour
  if (saved_hl)
  {
    memcpy (editor.lines[saved_hl_line].hl, saved_hl,
            editor.lines[saved_hl_line].r_len);
    free (saved_hl);
    saved_hl = NULL;
  }

  // If the user presses enter or escape, return without doing anything
  if (key == '\r' || key == '\x1b')
  {
    last_match = -1;
    direction = 1;
    return;
  }
    // Search forwards
  else if (key == ARROW_RIGHT || key == ARROW_DOWN)
    direction = 1;
    // Search backwards
  else if (key == ARROW_LEFT || key == ARROW_UP)
    direction = -1;
    // Search forwards
  else
  {
    last_match = -1;
    direction = 1;
  }

  // Set last match to be the line the line number where the query was last
  // matched
  if (last_match == -1)
    direction = 1;
  current = last_match;

  for (i = 0; i < editor.nlines; i++)
  {
    // Increment current line search counter
    current += direction;
    if (current == -1)
      current = editor.nlines - 1;
    else if (current == editor.nlines)
      current = 0;

    line = &editor.lines[current];
    // Use strstr to check if the query substring is on the current row. If it
    // isn't, then strstr returns NULL. To find the index, we can do some
    // pointer arithmetic
    match = strstr (line->render, query);
    if (match)
    {
      last_match = current;
      editor.cy = current;
      editor.cx = convert_rx_to_cx (line, (int) (match - line->render));
      editor.row_offset = editor.nlines;

      // Set the matched substrings to be HL_MATCH colour and save the original
      // colour
      saved_hl_line = current;
      saved_hl = malloc (line->r_len);
      memcpy (saved_hl, line->hl, line->r_len);
      memset (&line->hl[match - line->render], HL_MATCH, strlen (query));
      break;
    }
  }
}

// Find control function
void find (void)
{
  char *query;
  int saved_cx, saved_cy, saved_col_offset, saved_row_offset;

  // Save the original position of the cursor and text buffer
  saved_cx = editor.cx;
  saved_cy = editor.cy;
  saved_col_offset = editor.col_offset;
  saved_row_offset = editor.row_offset;

  if ((query = status_bar_prompt ("Search: %s (ESC to cancel |"
                                  " Arrows to search)", keyword_search)))
    free (query);
    // If a query is not found, i.e. on pressing escape, then restore the cursor
    // to it's original position
  else
  {
    editor.cx = saved_cx;
    editor.cy = saved_cy;
    editor.col_offset = saved_col_offset;
    editor.row_offset = saved_row_offset;
  }
}
