
/*
** Header file for inclusion with kword_xml2html.c
**
** Copyright (C) 1999 Lezlie Fort
**
** This library is free software; you can redistribute it and/or
** modify it under the terms of the GNU Library General Public
** License as published by the Free Software Foundation; either
** version 2 of the License, or (at your option) any later version.
**
** This library is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
** Library General Public License for more details.
**
** To receive a copy of the GNU Library General Public License, write to the
** Free Software Foundation, Inc., 59 Temple Place - Suite 330,
** Boston, MA  02111-1307, USA.
**
*/

#ifndef kword_xml2html
#define kword_xml2html

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

Token *ParseXml(
  char *zFile,        /* Complete text of the file being parsed */
  int *piFile         /* Index of next unparsed character in zFile */
);

#endif
