/* ***************************************************************************
 *
 * @file highlight.c
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


#include <ctype.h>

#include "kris.h"


#define HL_HIGHLIGHT_NUMBERS (1<<0)
#define HL_HIGHLIGHT_STRINGS (1<<1)

// C syntax highlighting
char *C_EXTENSIONS[] = {".c", ".h", ".cpp", ".hpp", NULL};
char *C_KEYWORDS[] = {
  "switch", "if", "while", "for", "break", "continue", "return", "else",
   "struct", "union", "typedef", "static", "enum", "class", "case","int|",
   "long|", "double|", "float|", "char|", "unsigned|", "signed|", "void|",
   NULL
};

// Syntax highlighting database
SYNTAX HLDB[] = {
  {
    "C",
    C_EXTENSIONS,
    C_KEYWORDS,
    "//",
    HL_HIGHLIGHT_NUMBERS | HL_HIGHLIGHT_STRINGS
  },
};

#define HLDB_ENTRIES (sizeof (HLDB) / sizeof (HLDB[0]))

// Will return true if the char passed is considered a separator char
int is_separator (int c)
{
  // isspace checks if it's a space
  // strchr checks for an occurrence in a string
  return isspace (c) || c == '\0' || strchr (",.()+-/*=~%<>[];", c) != NULL;
}

// Match the file type and syntax highlighting
void select_syntax_highlighting (void)
{
  int is_ext;
  size_t i, j, k;
  char *ext;
  SYNTAX *s;

  editor.syntax = NULL;
  if (editor.filename == NULL)
    return;

  // Get a pointer to the . part of the filename and loop over the HLDB entries
  // and try to match the extension to one in the HLDB
  ext = strrchr (editor.filename, '.');
  for (i = 0; i < HLDB_ENTRIES; i++)
  {
    s = &HLDB[i];
    j = 0;
    while (s->filematch[j])
    {
      is_ext = (s->filematch[j][0] == '.');
      if ((is_ext && ext && !strcmp (ext, s->filematch[j])) ||
                         (!is_ext && strstr (editor.filename, s->filematch[j])))
      {
        editor.syntax = s;
        // Update all of the lines with the appropriate syntax for use them the
        // file type changes
        for (k = 0; k < editor.nlines; k++)
          update_syntax_highlight (&editor.lines[k]);
        return;
      }

      j++;
    }
  }
}

// Update the syntax highlighting array for a line
void update_syntax_highlight (ELINE *line)
{
  char c;
  unsigned char prev_hl;
  size_t i, j, scs_len, key_len;
  int prev_sep, in_string, kw2;
  char *scs;
  char **keywords;

  // Allocate space for syntax highlight array and originally set all of the
  // chars to have the normal colour
  line->hl = realloc (line->hl, line->r_len);
  memset (line->hl, HL_NORMAL, line->r_len);

  // If syntax highlighting is not set, return
  if (editor.syntax == NULL)
    return;

  // Alias syntax keywords to something shorter
  keywords = editor.syntax->keywords;

  // Alias for single_line_comment to make things a bit neater
  scs = editor.syntax->single_line_comment;
  scs_len = scs ? strlen (scs) : 0;

  // Iterate over the entire line. prev_sep is to check that the previous char
  // is a separator char so we only colour in numbers and not numbers embedded
  // in strings as well
  i = 0;
  prev_sep = TRUE;
  in_string = FALSE;
  while (i < line->r_len)
  {
    c = line->render[i];
    prev_hl = (i > 0) ? line->hl[i - 1] : HL_NORMAL;

    // Single line comments
    if (scs_len && !in_string)
    {
      if (!strncmp (&line->render[i], scs, scs_len))
      {
        memset (&line->hl[i], HL_COMMENT, line->r_len - i);
        break;
      }
    }

    // Strings
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
          // Save the value so we know if to close around enclosing " or '
          in_string = c;
          line->hl[i] = HL_STRING;
          i++;
          continue;
        }
      }
    }

    // Numbers
    if (editor.syntax->flags & HL_HIGHLIGHT_NUMBERS)
    {
      // The 2nd OR is to allow for decimal numbers
      if ((isdigit (c) && (prev_sep || prev_hl == HL_NUMBER)) ||
          (c == '.' && prev_hl == HL_NUMBER))
      {
        line->hl[i] = HL_NUMBER;
        i++;
        prev_sep = FALSE;
        continue;
      }
    }

    // Keywords -- make sure a separator came before the keyword
    if (prev_sep)
    {
      for (j = 0; keywords[j]; j++)
      {
        key_len = strlen (keywords[j]);
        // If the keyword is a keyword2, then there will be a | at the end which
        // indicates it's a kw2, hence decrement key_len by 1 to remove it
        kw2 = keywords[j][key_len - 1] == '|';
        if (kw2)
          key_len--;

        // Keywords require a separator before and after hence check
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
}

// Convert an internal highlighting number to the correct one for the terminal
int get_syntax_colour (int hl)
{
  switch (hl)
  {
    case HL_NUMBER: return 31;  // red
    case HL_MATCH: return 34;   // dark blue
    case HL_STRING: return 35;  // magenta
    case HL_COMMENT: return 32;  // green
    case HL_KEYWORD1: return 33;  // yellow
    case HL_KEYWORD2: return 36;  // light blue
    default: return 37;
  }
}
