/** **************************************************************************
 *
 * @file lines.c
 *
 * @date 02/01/2019
 *
 * @author E. J. Parkinson
 *
 * @brief Functions for adding chars and lines to the various text buffers.
 *
 * ************************************************************************** */


#include <stdlib.h>
#include <string.h>

#include "kris.h"


//! @brief Append a line of text to the text buffer in editor_config
void line_add_to_text_buffer (int insert_index, char *s, size_t line_len)
{
  int i;

  if (insert_index < 0 || insert_index > editor.nlines)
    return;

  // Allocate extra space, shift the lines down by 1 and insert new line
  editor.lines = realloc (editor.lines,
                          sizeof (EDITOR_LINE) * (editor.nlines + 1));
  memmove (&editor.lines[insert_index + 1], &editor.lines[insert_index],
           sizeof (EDITOR_LINE) * (editor.nlines - insert_index));
  for (i = insert_index + 1; i <= editor.nlines; i++)
    editor.lines[i].idx++;
  editor.lines[insert_index].idx = insert_index;

  // Append text to the new text line
  editor.lines[insert_index].len = line_len;
  editor.lines[insert_index].chars = malloc (line_len + 1);
  memcpy (editor.lines[insert_index].chars, s, line_len);
  editor.lines[insert_index].chars[line_len] = '\0';

  // Update the render buffer
  editor.lines[insert_index].r_len = 0;
  editor.lines[insert_index].render = NULL;
  editor.lines[insert_index].hl = NULL;
  editor.lines[insert_index].hl_open_comment = 0;
  editor_add_to_render_buffer (&editor.lines[insert_index]);

  // Update total number of lines and number of modified lines
  editor.nlines++;
  editor.modified++;
}

//! Insert a char into the text buffer arrays
void line_insert_char (EDITOR_LINE *line, int insert_idx, int c)
{
  if (insert_idx < 0 || insert_idx > line->len)
    insert_idx = (int) line->len;

  // Allocate more memory, shift chars right by 1 and insert new char
  line->chars = realloc (line->chars, line->len + 2);
  memmove (&line->chars[insert_idx + 1], &line->chars[insert_idx],
           line->len - insert_idx + 1);
  line->len++;
  line->chars[insert_idx] = (char) c;
  editor.modified++;
  editor_add_to_render_buffer (line);
}

//! @brief Delete a char in a char buffer array
void line_delete_char (EDITOR_LINE *line, int insert_idx)
{
  if (insert_idx < 0 || insert_idx > line->len)
    return;

  // Essentially just moving every char to the left
  memmove (&line->chars[insert_idx], &line->chars[insert_idx + 1],
           line->len - insert_idx);
  line->len--;
  editor.modified++;
  editor_add_to_render_buffer (line);
}

//! @brief Append a string to the end of a line
void line_add_string_to_text_buffer (EDITOR_LINE *dest_line, char *src,
                                     size_t append_len)
{
  // Allocate more memory and shift lines to fit in the appended string
  dest_line->chars = realloc (dest_line->chars,
                              dest_line->len + append_len + 1);
  memcpy (&dest_line->chars[dest_line->len], src, append_len);
  dest_line->len += append_len;
  dest_line->chars[dest_line->len] = '\0';
  editor.modified++;
  editor_add_to_render_buffer (dest_line);
}

//! @brief Free the memory of a line
void util_free_line (EDITOR_LINE *line)
{
  free (line->chars);
  free (line->render);
  free (line->hl);
}

//! @brief Delete an entire line
void line_delete (int idx)
{
  int i;

  if (idx < 0 || idx > editor.nlines)
    return;

  // Freeing line idx and then shift the other lines
  util_free_line (&editor.lines[idx]);
  memmove (&editor.lines[idx], &editor.lines[idx + 1],
           sizeof (EDITOR_LINE) * (editor.nlines - idx - 1));

  for (i = idx; i < editor.nlines - 1; i++)
    editor.lines[idx].idx--;

  editor.nlines--;
  editor.modified++;
}
