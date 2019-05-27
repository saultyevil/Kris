/** **************************************************************************
 *
 * @file syntax.h
 *
 * @date 05/01/2019
 *
 * @author E. J. Parkinson
 *
 * @brief Data structures for the highlight database and keywords for
 *        syntax highlighting.
 *
 * ************************************************************************** */


#ifndef KRIS_SYNTAX_H
#define KRIS_SYNTAX_H

#define HL_HIGHLIGHT_NUMBERS (1<<0)
#define HL_HIGHLIGHT_STRINGS (1<<1)

// C syntax highlighting
char *C_EXTENSIONS[] = {".c", ".h", ".cpp", ".hpp", NULL};
char *C_KEYWORDS[] = {
  "switch", "if", "while", "for", "break", "continue", "return", "else",
  "struct", "union", "typedef", "static", "enum", "class", "case", "default",
  "int|", "long|", "double|", "float|", "char|", "unsigned|", "signed|",
  "void|", "NULL|", NULL
};

// Fortran syntax highlighting
char *FORTRAN_EXTENTIONS[] = {".f", ".f90", ".f95", NULL};
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

#endif
