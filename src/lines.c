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

/** **************************************************************************
 *
 *  @brief              Append a line of text to the text buffer in editor_config
 *
 *  @param[in]          insert_index    The line index of where to insert the new
 *                                      line
 *  @param[in]          *s              The buffer to append to the text buffer
 *  @param[in]          line_len        The number of characters to append
 *
 *  @return             void
 *
 *  @details
 *
 *  Adds some text to the text buffer. This is done by allocating extra memory
 *  to hold an extra line and the current line and lines afterwards in the text
 *  buffer are shifted downward by one. The new line is then appended into the
 *  text buffer on the line given by insert_index. The render buffer is then
 *  updated as well as the total number of lines.
 *
 * ************************************************************************** */

void
line_add_to_text_buffer (int insert_index, char *s, size_t line_len)
{
  int i;

  if (insert_index < 0 || insert_index > editor.nlines)
    return;

  /*
   * Allocate extra space, shift the lines down by 1 and insert new line
   */

  editor.lines = realloc (editor.lines, (editor.nlines + 1) * sizeof (EDITOR_LINE));
  memmove (&editor.lines[insert_index + 1], &editor.lines[insert_index],
           sizeof (EDITOR_LINE) * (editor.nlines - insert_index));
  for (i = insert_index + 1; i <= editor.nlines; i++)
    editor.lines[i].idx++;
  editor.lines[insert_index].idx = insert_index;

  /*
   * Append text to the new text line
   */

  editor.lines[insert_index].len = line_len;
  editor.lines[insert_index].chars = malloc (line_len + 1);
  memcpy (editor.lines[insert_index].chars, s, line_len);
  editor.lines[insert_index].chars[line_len] = '\0';

  /*
   * Update the render buffer
   */

  editor.lines[insert_index].r_len = 0;
  editor.lines[insert_index].render = NULL;
  editor.lines[insert_index].syn_hl = NULL;
  editor.lines[insert_index].hl_open_comment = 0;
  editor_add_to_render_buffer (&editor.lines[insert_index]);

  /*
   * Update total number of lines and number of modified lines
   */

  editor.nlines++;
  editor.modified++;
}

/** **************************************************************************
 *
 *  @brief              Insert a char into the text buffer array
 *
 *  @param[in, out]     *line           The line in the text buffer to update
 *  @param[in]          insert_idx      The index of where to insert the character
 *  @param[in]          c               An integer representation of an ASCII
 *                                      character to insert
 *
 *  @return             void
 *
 *  @details
 *
 *  This function operates similar to line_add_to_text_buffer in that extra
 *  memory is allocated, for a single line in the text buffer this time, and
 *  then a character c is inserted into the line at insert_idx. Finally, the
 *  render buffer is updated with the line.
 *
 * ************************************************************************** */

void
line_insert_char (EDITOR_LINE *line, int insert_idx, int c)
{
  if (insert_idx < 0 || insert_idx > line->len)
    insert_idx = (int) line->len;

  /*
   * Allocate more memory, shift chars right by 1 and insert new char
   */

  line->chars = realloc (line->chars, line->len + 2);
  memmove (&line->chars[insert_idx + 1], &line->chars[insert_idx], line->len - insert_idx + 1);
  line->len++;
  line->chars[insert_idx] = (char) c;
  editor.modified++;
  editor_add_to_render_buffer (line);
}

/** **************************************************************************
 *
 *  @brief              Delete a char in a text buffer array
 *
 *  @param[in, out]     *line
 *  @param[in]          insert_idx
 *
 *  @return             void
 *
 *  @details
 *
 *  Deletes a character in a line in the text buffer. This is done essentially
 *  by shifting the line from insert_idx to the left.
 *
 *  TODO: memmove might not reallocate memory to be smaller
 *
 * ************************************************************************** */

void
line_delete_char (EDITOR_LINE *line, int insert_idx)
{
  if (insert_idx < 0 || insert_idx > line->len)
    return;

  /*
   * Essentially just moving every char to the left
   */

  memmove (&line->chars[insert_idx], &line->chars[insert_idx + 1], line->len - insert_idx);
  line->len--;
  editor.modified++;
  editor_add_to_render_buffer (line);
}

/** **************************************************************************
 *
 *  @brief              Append a string to the end of a line
 *
 *  @param[in, out]    *dest_line    The line to append *src to
 *  @param[in]         *src          The string which is being appended to
 *                                   *dest_line
 *  @param[in]         append_len    The length of the *src string
 *
 *  @return             void
 *
 *  @details
 *
 *  This function allocate more memory for the line in the text buffer and then
 *  uses memcpy to append the string *src to the end of *dest_line. The render
 *  buffer is then updated.
 *
 * ************************************************************************** */

void
line_add_string_to_text_buffer (EDITOR_LINE *dest_line, char *src, size_t append_len)
{
  /*
   * Allocate more memory and shift lines to fit in the appended string
   */

  dest_line->chars = realloc (dest_line->chars, dest_line->len + append_len + 1);
  memcpy (&dest_line->chars[dest_line->len], src, append_len);
  dest_line->len += append_len;
  dest_line->chars[dest_line->len] = '\0';
  editor.modified++;
  editor_add_to_render_buffer (dest_line);
}

/** **************************************************************************
 *
 *  @brief              Free the memory of a line
 *
 *  @param[in]          *line      The line to free from memory
 *
 *  @return             void
 *
 *  @details
 *
 *  Frees the various text buffers - char, render and syn_hl - from memory. This
 *  should free any malloc'd memory.
 *
 * ************************************************************************** */

void
util_free_line (EDITOR_LINE *line)
{
  free (line->chars);
  free (line->render);
  free (line->syn_hl);
}

/** **************************************************************************
 *
 *  @brief              Delete an entire line
 *
 *  @param[in]          idx      The index number of the line to delete
 *
 *  @return             void
 *
 *  @details
 *
 *  The line is free'd from memory and then all subsequent lines are shifted by
 *  backwards in memory and their index's are updated.
 *
 *  TODO: memmove might not reallocate memory to be smaller
 *
 * ************************************************************************** */

void
line_delete_line (int idx)
{
  int i;

  if (idx < 0 || idx > editor.nlines)
    return;

  /*
   * Freeing line idx and then shift the other lines
   */

  util_free_line (&editor.lines[idx]);
  memmove (&editor.lines[idx], &editor.lines[idx + 1], sizeof (EDITOR_LINE) * (editor.nlines - idx - 1));

  for (i = idx; i < editor.nlines - 1; i++)
    editor.lines[idx].idx--;

  editor.nlines--;
  editor.modified++;
}
