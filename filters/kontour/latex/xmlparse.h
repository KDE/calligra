#ifndef xmlparse
#define xmlparse

/*
** Every token is one of the following types.
*/
typedef enum TokenType {
  TT_Word,        /* Anything besides markup and whitespace */
  TT_Space,       /* Spaces, not counting "\n" characters */
  TT_EOL,         /* A sequence of spaces ending with "\n" */
  TT_Markup       /* Any markup sequence */
} TokenType;


/*
** Each token is represented by an instance of the following structure.
*/

typedef struct Token Token;
struct Token {
  int offset;         /* Offset from start of file.  Used for error messages */
  TokenType eType;    /* What type of token is this. */
  char *zText;        /* The actual text of the token */
  Token *pNext;       /* Next token in the list */
};

/*
** Arguments to a markup token are recorded as a list of the
** following structure.
*/
typedef struct Arg Arg;
struct Arg {
  int offset;      /* Index of first char of name -- for error messages */
  char *zName;     /* Name of argument */
  char *zValue;    /* Value of the argument */
  Arg *pNext;      /* Next argument in the list, or NULL */
};

/*
** Markup tokens have a few extra fields, shown by this structure.
*/
typedef struct Markup Markup;
struct Markup {
  Token token;       /* Base class.  Must be first */
  Token *pContent;   /* Tokens between <MARKUP> and </MARKUP> */
  Arg *pArg;         /* Arguments to this token */
};

Token * ParseXml(const char *,  int *);
void PrintXml(Token *, int);

#endif
