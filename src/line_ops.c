/* ***************************************************************************
 *
 * @file line_ops.c
 *
 * @date 02/01/2019
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


// Append a row of text to the text buffer
void append_line_to_text_buffer (int insert_index, char *s, size_t line_len)
{
  if (insert_index < 0 || insert_index > editor.nlines)
    return;

  // Allocate extra space
  editor.lines = realloc (editor.lines,  sizeof (ELINE) * (editor.nlines + 1));
  memmove (&editor.lines[insert_index + 1], &editor.lines[insert_index],
           sizeof (ELINE) * (editor.nlines - insert_index));

  // Append text to the new text buffer line
  editor.lines[insert_index].len = line_len;
  editor.lines[insert_index].chars = malloc (line_len + 1);
  memcpy (editor.lines[insert_index].chars, s, line_len);
  editor.lines[insert_index].chars[line_len] = '\0';

  // Update the rendering buffer for special characters
  editor.lines[insert_index].r_len = 0;
  editor.lines[insert_index].render = NULL;
  editor.lines[insert_index].hl = NULL;
  update_to_render_buffer (&editor.lines[insert_index]);

  // Update total number of lines and number of modified lines
  editor.nlines++;
  editor.modified++;
}

// Insert a char into the text buffer arrays
void insert_char_in_line (ELINE *line, int insert_idx, int c)
{
  // Bounds check for insertion index
  if (insert_idx < 0 || insert_idx > line->len)
    insert_idx = (int) line->len;

  // Insert the new char into chars array
  // Use memmove as this is safer for memory overlap issues where we would lose
  // the correct data to copy and update the render buffer
  line->chars = realloc (line->chars, line->len + 2);
  memmove (&line->chars[insert_idx + 1], &line->chars[insert_idx],
           line->len - insert_idx + 1);
  line->len++;
  line->chars[insert_idx] = (char) c;
  update_to_render_buffer (line);
  editor.modified++;
}

// Delete a char in a text buffer array
void delete_char_in_line (ELINE *line, int insert_idx)
{
  // Bounds check for insertion index
  if (insert_idx < 0 || insert_idx > line->len)
    return;

  // We are essentially just moving everything to the left
  memmove (&line->chars[insert_idx], &line->chars[insert_idx + 1],
           line->len - insert_idx);
  line->len--;
  update_to_render_buffer (line);
  editor.modified++;
}

// Append a string to the end of a line
void append_string_to_line (ELINE *dest_line, char *src, size_t append_len)
{
  // Allocate more memory for the line to account for the appended string
  dest_line->chars = realloc (dest_line->chars, dest_line->len + append_len + 1);
  memcpy (&dest_line->chars[dest_line->len], src, append_len);
  dest_line->len += append_len;
  dest_line->chars[dest_line->len] = '\0';
  update_to_render_buffer (dest_line);
  editor.modified++;
}

// Free the memory of a line
void free_line (ELINE *line)
{
  free (line->chars);
  free (line->render);
  free (line->hl);
}

// Delete an entire line
void delete_line (int idx)
{
  // Bounds checking
  if (idx < 0 || idx > editor.nlines)
    return;

  // Essentially, we are freeing a line and then shifting the other lines up
  // by one
  free_line (&editor.lines[idx]);
  memmove (&editor.lines[idx], &editor.lines[idx + 1],
           sizeof (ELINE) * (editor.nlines - idx - 1));
  editor.nlines--;
  editor.modified++;
}
