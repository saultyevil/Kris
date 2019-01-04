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
#include <stdlib.h>
#include <string.h>

#include "kris.h"


#define HL_HIGHLIGHT_NUMBERS (1<<0)
#define HL_HIGHLIGHT_STRINGS (1<<1)

// C syntax highlighting
char *C_EXTENSIONS[] = {".c", ".h", ".cpp", ".hpp", NULL};
char *C_KEYWORDS[] = {
  "switch", "if", "while", "for", "break", "continue", "return", "else",
   "struct", "union", "typedef", "static", "enum", "class", "case", "int|",
   "long|", "double|", "float|", "char|", "unsigned|", "signed|", "void|",
   NULL
};

// Fortran syntax highlighting
char *FORTRAN_EXTENTIONS[] = {".f", ".f90", ".f95"};
char *FORTRAN_KEYWORDS[] = {
  "assign", "backspace", "block", "data", "call", "close", "common", "continue",
  "data", "dimension", "do", "else", "else", "if", "end", "endfile", "endif",
  "entry", "equivalence", "external", "format", "function", "goto", "if",
  "implicit", "inquire", "intrinsic", "open", "parameter", "pause", "print",
  "program", "read", "return", "rewind", "rewrite", "save", "stop",
  "subroutine", "then", "write", "allocatable", "allocate", "case",
  "contains", "cycle", "deallocate", "elsewhere", "exit", "include",
  "interface", "intent", "module", "namelist", "nullify", "only", "operator",
  "optional", "pointer", "private", "procedure", "public", "recursive",
  "result", "select", "sequence", "target", "use", "while", "where",
  "elemental", "forall", "pure", "integer|", "real|", "double precision|",
  "complex|", "logical|", "character|", "ASSIGN", "BACKSPACE", "BLOCK", "DATA",
  "CALL", "CLOSE", "COMMON", "CONTINUE", "DATA", "DIMENSION", "DO", "ELSE",
  "ELSE", "IF", "END", "ENDFILE", "ENDIF", "ENTRY", "EQUIVALENCE", "EXTERNAL",
  "FORMAT", "FUNCTION", "GOTO", "IF", "IMPLICIT", "INQUIRE", "INTRINSIC",
  "OPEN", "PARAMETER", "PAUSE", "PRINT", "PROGRAM", "READ", "RETURN", "REWIND",
  "REWRITE", "SAVE", "STOP", "SUBROUTINE", "THEN", "WRITE", "ALLOCATABLE",
  "ALLOCATE", "CASE", "CONTAINS", "CYCLE", "DEALLOCATE", "ELSEWHERE", "EXIT",
  "INCLUDE", "INTERFACE", "INTENT", "MODULE", "NAMELIST", "NULLIFY", "ONLY",
  "OPERATOR", "OPTIONAL", "POINTER", "PRIVATE", "PROCEDURE", "PUBLIC",
  "RECURSIVE", "RESULT", "SELECT", "SEQUENCE", "TARGET", "USE", "WHILE",
  "WHERE", "ELEMENTAL", "FORALL", "PURE", "INTEGER|", "REAL|",
  "DOUBLE PRECISION|", "COMPLEX|", "LOGICAL|", "CHARACTER|", NULL
};

// Python syntax highlighting
char *PYTHON_EXTENSIONS[] = { ".py", NULL };
char *PYTHON_KEYWORDS[] = {
  "and", "as", "assert", "break", "class", "continue", "def", "del", "elif",
  "else:", "except", "exec", "finally", "for", "from", "global", "if", "import",
  "in", "is", "lambda", "not", "or", "pass", "print", "raise", "return", "try",
  "while", "with", "yield", "async", "await", "nonlocal", "range", "xrange",
  "reduce", "map", "filter", "all", "any", "sum", "dir", "abs", "breakpoint",
  "compile", "delattr", "divmod", "format", "eval", "getattr", "hasattr","hash",
  "help", "id", "input", "isinstance", "issubclass", "len", "locals", "max",
  "min", "next", "open", "pow", "repr", "reversed", "round", "setattr", "slice",
  "sorted", "super", "vars", "zip", "__import__", "reload", "raw_input",
  "execfile", "file", "cmp", "basestring", "buffer|", "bytearray|", "bytes|",
  "complex|", "float|", "frozenset|", "int|", "list|", "long|", "None|", "set|",
  "str|", "chr|", "tuple|", "bool|", "False|", "True|", "type|", "unicode|",
  "dict|", "ascii|", "bin|", "callable|", "classmethod|", "enumerate|", "hex|",
  "oct|", "ord|", "iter|", "memoryview|", "object|", "property|",
  "staticmethod|", "unichr|", NULL
};

// Syntax highlighting database
SYNTAX HLDB[] = {
  {
    "C",
    C_EXTENSIONS,
    C_KEYWORDS,
    "#", "//", "/*", "*/",
    HL_HIGHLIGHT_NUMBERS | HL_HIGHLIGHT_STRINGS
  },
  {
    "FORTRAN",
    FORTRAN_EXTENTIONS,
    FORTRAN_KEYWORDS,
    "!", "!", "", "",
    HL_HIGHLIGHT_NUMBERS | HL_HIGHLIGHT_STRINGS
  },
  {
    "PY",
    PYTHON_EXTENSIONS,
    PYTHON_KEYWORDS,
    "", "#", "\"\"\"", "\"\"\"",
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
  size_t i, j, scs_len, mcs_len, mce_len, key_len, pp_len;
  int prev_sep, in_string, in_comment, kw2, changed;
  char *scs, *mcs, *mce, *pp;
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
  mcs = editor.syntax->ml_comment_start;
  mcs_len = mcs ? strlen (mcs) : 0;
  mce = editor.syntax->ml_comment_end;
  mce_len = mce ? strlen (mce) : 0;
  pp = editor.syntax->pre_processor;
  pp_len = pp ? strlen (pp) : 0;

  // Iterate over the entire line. prev_sep is to check that the previous char
  // is a separator char so we only colour in numbers and not numbers embedded
  // in strings as well
  i = 0;
  prev_sep = TRUE;
  in_string = FALSE;
  // Initialise in_comment to be true if the previous row has an unclosed ml
  // comment
  in_comment = (line->idx > 0 && editor.lines[line->idx - 1].hl_open_comment);
  while (i < line->r_len)
  {
    c = line->render[i];
    prev_hl = (i > 0) ? line->hl[i - 1] : HL_NORMAL;

    // Preprocessor
    if (pp_len && !in_string && !in_comment)
    {
      if (!strncmp (&line->render[i], pp, pp_len))
      {
        memset (&line->hl[i], HL_PREPROCESS, line->r_len - i);
        break;
      }
    }

    // Single line comments
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

    // Multi line comments
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
          (c == '.' && prev_hl == HL_NUMBER) || (c == 'e' && prev_hl == HL_NUMBER))
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

  // Track to see if the ml comment has been closed.. if not make a recursive
  // call to update the next line until the ml comment is closed
  changed = (line->hl_open_comment != in_comment);
  line->hl_open_comment = in_comment;
  if (changed && line->idx + 1 < editor.nlines)
    update_syntax_highlight (&editor.lines[line->idx + 1]);
}

// Convert an internal highlighting number to the correct one for the terminal
int get_syntax_colour (int hl)
{
  switch (hl)
  {
    case HL_PREPROCESS:
    case HL_NUMBER: return 31;  // red
    case HL_MATCH: return 34;   // dark blue
    case HL_STRING: return 35;  // magenta
    case HL_ML_COMMENT:
    case HL_COMMENT: return 32;  // green
    case HL_KEYWORD1: return 33;  // yellow
    case HL_KEYWORD2: return 36;  // light blue
    default: return 37;
  }
}
