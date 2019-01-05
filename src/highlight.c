/* ***************************************************************************
 *
 * @file highlight.c
 *
 * @date 03/01/2019
 *
 * @author E. J. Parkinson
 *
 * @brief Control functions for updating syntax highlighting.
 *
 * ************************************************************************** */


#include <ctype.h>
#include <stdlib.h>
#include <string.h>

#include "kris.h"
#include "syntax.h"

// @brief Return true if the char passed is considered a separator
int is_separator (int c)
{
  return isspace (c) || c == '\0' || strchr (",.()+-/*=~%<>[];", c) != NULL;
}

// @brief Convert an internal highlight num to one for the terminal
int syntax_get_colour (int hl)
{
  switch (hl)
  {
    case HL_PREPROCESS:
    case HL_NUMBER: return 31;    // red
    case HL_MATCH: return 34;     // dark blue
    case HL_STRING: return 35;    // magenta
    case HL_ML_COMMENT:
    case HL_COMMENT: return 32;   // green
    case HL_KEYWORD1: return 33;  // yellow
    case HL_KEYWORD2: return 36;  // light blue
    default: return 37;           // default terminal front colour
  }
}

// @brief Match the file type and syntax highlighting
void syntax_select_highlighting (void)
{
  int is_ext;
  char *file_ext;
  size_t i, j, k;

  // If no filename, return as cannot progress, otherwise find the extension
  editor.syntax = NULL;
  if (editor.filename == NULL)
    return;
  file_ext = strrchr (editor.filename, '.');

  // Attempt to match the file extension with one in the HLDB
  for (i = 0; i < HLDB_ENTRIES; i++)
  {
    j = 0;
    while (HLDB[i].filematch[j])
    {
      // Ensure the HLDB extension has a period
      is_ext = (HLDB[i].filematch[j][0] == '.');
      if ((is_ext && file_ext && !strcmp (file_ext, HLDB[i].filematch[j])) ||
                    (!is_ext && strstr (editor.filename, HLDB[i].filematch[j])))
      {
        // Set the editor syntax and update all of the lines
        editor.syntax = &HLDB[i];
        for (k = 0; k < editor.nlines; k++)
          syntax_update_highlighting (&editor.lines[k]);
        return;
      }
      j++;
    }
  }
}

// @brief Update the syntax highlight array for a single line
void syntax_update_highlighting (EDITOR_LINE *line)
{
  char c;
  char **keywords;
  unsigned char prev_hl;
  char *scs, *mcs, *mce, *pp;
  int prev_sep, in_string, in_comment, kw2, changed;
  size_t i, j, scs_len, mcs_len, mce_len, key_len, pp_len;

  // If syntax highlighting is not set, return
  if (editor.syntax == NULL)
    return;

  // Allocate space for syntax highlight array and initialise to no highlighting
  line->hl = realloc (line->hl, line->r_len);
  memset (line->hl, HL_NORMAL, line->r_len);

  // Create aliases and find length of the strings
  keywords = editor.syntax->keywords;
  scs = editor.syntax->single_line_comment;
  mcs = editor.syntax->ml_comment_start;
  mce = editor.syntax->ml_comment_end;
  pp = editor.syntax->pre_processor;
  scs_len = scs ? strlen (scs) : 0;
  mcs_len = mcs ? strlen (mcs) : 0;
  mce_len = mce ? strlen (mce) : 0;
  pp_len = pp ? strlen (pp) : 0;

  /*
   * Loop over the entire render line. prev_sep is to check that the previous
   * char is a separator char so we only colour in numbers and not numbers
   * embedded in strings as well. in_comment is initialised to true if the
   * previous line has an unclosed multiline comment
   */

  i = 0;
  prev_sep = TRUE;
  in_string = FALSE;
  in_comment = (line->idx > 0 && editor.lines[line->idx - 1].hl_open_comment);
  while (i < line->r_len)
  {
    c = line->render[i];
    prev_hl = (i > 0) ? line->hl[i - 1] : HL_NORMAL;

    // Process for preprocessor directives
    if (pp_len && !in_string && !in_comment)
    {
      if (!strncmp (&line->render[i], pp, pp_len))
      {
        memset (&line->hl[i], HL_PREPROCESS, line->r_len - i);
        break;
      }
    }

    // Process for single line comments
    if (scs_len && !in_string && !in_comment)
    {
      if (!strncmp (&line->render[i], scs, scs_len))
      {
        memset (&line->hl[i], HL_COMMENT, line->r_len - i);
        break;
      }
      // Hacky fix to enable f77 style comments
      else if (line->render[i] == 'c' && i == 0 &&
                                   !strcmp (editor.syntax->filetype, "FORTRAN"))
      {
        memset (&line->hl[i], HL_COMMENT, line->r_len - i);
        break;
      }
    }

    // Process for multi line comments
    if (mcs_len && mce_len && !in_string)
    {
      if (in_comment)
      {
        line->hl[i] = HL_ML_COMMENT;
        if (!strncmp (&line->render[i], mce, mce_len))
        {
          memset (&line->hl[i], HL_ML_COMMENT, mce_len);
          i += mce_len;
          in_comment = FALSE;
          prev_sep = TRUE;
        }
        else
        {
          i++;
          continue;
        }
      }
      else if (!strncmp (&line->render[i], mcs, mce_len))
      {
        memset (&line->hl[i], HL_ML_COMMENT, mcs_len);
        i+= mcs_len;
        in_comment = TRUE;
        continue;
      }
    }

    // Process for strings, can be enclosed by " or '
    if (editor.syntax->flags & HL_HIGHLIGHT_STRINGS)
    {
      if (in_string)
      {
        // Deal with escape sequences for quotes
        if (c == '\\' && i + 1 < line->r_len)
        {
          line->hl[i + 1] = HL_STRING;
          i += 2;
          continue;
        }
        line->hl[i] = HL_STRING;
        if (c == in_string)
          in_string = FALSE;
        i++;
        prev_sep = TRUE;
        continue;
      }
      else
      {
        if (c == '"' || c == '\'')
        {
          // Save the value so we know if to close around another " or '
          in_string = c;
          line->hl[i] = HL_STRING;
          i++;
          continue;
        }
      }
    }

    // Process for numbers not part of strings or variable names
    if (editor.syntax->flags & HL_HIGHLIGHT_NUMBERS)
    {
      // The 2nd OR is for decimal numbers and 3rd is for scientific numbers
      if ((isdigit (c) && (prev_sep || prev_hl == HL_NUMBER)) ||
          (c == '.' && prev_hl == HL_NUMBER) || (c == 'e' && prev_hl == HL_NUMBER))
      {
        line->hl[i] = HL_NUMBER;
        i++;
        prev_sep = FALSE;
        continue;
      }
    }

    // Process for keywords, ensure a separator comes before the keyword
    if (prev_sep)
    {
      for (j = 0; keywords[j]; j++)
      {
        /*
         * If the keyword is a keyword2, then there will be a | at the end which
         * indicates it's a kw2, hence decrement key_len by 1 to remove |
         */

        key_len = strlen (keywords[j]);
        kw2 = (keywords[j][key_len - 1] == '|');
        if (kw2)
          key_len--;

        // Keywords require a separator before and after
        if (!strncmp (&line->render[i], keywords[j], key_len) &&
                                       is_separator (line->render[i + key_len]))
        {
          memset (&line->hl[i], kw2 ? HL_KEYWORD2 : HL_KEYWORD1, key_len);
          i += key_len;
          break;
        }
      }
      if (keywords[j] != NULL)
      {
        prev_sep = FALSE;
        continue;
      }
    }

    prev_sep = is_separator (c);
    i++;
  }

  /*
   * Track to see if the ml comment has been closed.. if not make a recursive
   * call to update the next line until the ml comment is closed
   */

  changed = (line->hl_open_comment != in_comment);
  line->hl_open_comment = in_comment;
  if (changed && line->idx + 1 < editor.nlines)
    syntax_update_highlighting (&editor.lines[line->idx + 1]);
}
