
/*  A Bison parser, made from yacc.dep.y
 by  GNU Bison version 1.25
  */

#define YYBISON 1  /* Identify Bison output.  */

#define yyparse depparse
#define yylex deplex
#define yyerror deperror
#define yylval deplval
#define yychar depchar
#define yydebug depdebug
#define yynerrs depnerrs
#define	NUM	258
#define	RANGE	259
#define	BOOL	260
#define	ID	261
#define	WENN	262
#define	LEQ	263
#define	GEQ	264
#define	NEQ	265
#define	NOT	266
#define	OR	267
#define	AND	268

#line 1 "yacc.dep.y"

#include "kspread_calcerr.h"

extern void setError( int _errno, const char *_txt );
extern void initDependFlex( const char* _code );
extern int deperror ( char *s );

#line 9 "yacc.dep.y"
typedef union
{
     char b;
     double dbl;
     char* name;
     void* range;
     void* param;
} YYSTYPE;
#include <stdio.h>

#ifndef __cplusplus
#ifndef __STDC__
#define const
#endif
#endif



#define	YYFINAL		95
#define	YYFLAG		-32768
#define	YYNTBASE	28

#define YYTRANSLATE(x) ((unsigned)(x) <= 268 ? yytranslate[x] : 40)

static const char yytranslate[] = {     0,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,    23,     2,     2,     2,     2,    19,
    20,    16,    14,     2,    15,     2,    17,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,    21,    24,
    22,    25,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,    18,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,    26,     2,    27,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     1,     2,     3,     4,     5,
     6,     7,     8,     9,    10,    11,    12,    13
};

#if YYDEBUG != 0
static const short yyprhs[] = {     0,
     0,     1,     3,     5,     9,    13,    15,    19,    23,    25,
    28,    30,    34,    36,    40,    49,    54,    58,    60,    64,
    68,    72,    74,    76,    78,    82,    84,    88,    90,    93,
    95,    97,   101,   105,   109,   113,   117,   121,   125,   129,
   133,   137,   139,   143,   152,   157
};

static const short yyrhs[] = {    -1,
    29,     0,    35,     0,    29,    14,    30,     0,    29,    15,
    30,     0,    30,     0,    30,    16,    31,     0,    30,    17,
    31,     0,    31,     0,    15,    32,     0,    32,     0,    32,
    18,    33,     0,    33,     0,    19,    29,    20,     0,     7,
    19,    35,    21,    29,    21,    29,    20,     0,     6,    19,
    34,    20,     0,     6,    19,    20,     0,     3,     0,    34,
    21,    29,     0,    34,    21,    35,     0,    34,    21,     4,
     0,    29,     0,    35,     0,     4,     0,    35,    12,    36,
     0,    36,     0,    36,    13,    37,     0,    37,     0,    11,
    38,     0,    38,     0,    39,     0,    29,    22,    29,     0,
    29,    23,    29,     0,    29,    10,    29,     0,    29,    24,
    29,     0,    29,    25,    29,     0,    29,     8,    29,     0,
    29,     9,    29,     0,    39,    22,    39,     0,    39,    23,
    39,     0,    39,    10,    39,     0,     5,     0,    26,    35,
    27,     0,     7,    26,    35,    21,    35,    21,    35,    27,
     0,     6,    26,    34,    27,     0,     6,    26,    27,     0
};

#endif

#if YYDEBUG != 0
static const short yyrline[] = { 0,
    32,    33,    34,    37,    38,    39,    42,    43,    44,    46,
    47,    50,    51,    54,    55,    56,    57,    58,    61,    62,
    63,    64,    65,    66,    69,    70,    73,    74,    77,    78,
    81,    82,    83,    84,    85,    86,    87,    88,    89,    90,
    91,    94,    95,    96,    97,    98
};
#endif


#if YYDEBUG != 0 || defined (YYERROR_VERBOSE)

static const char * const yytname[] = {   "$","error","$undefined.","NUM","RANGE",
"BOOL","ID","WENN","LEQ","GEQ","NEQ","NOT","OR","AND","'+'","'-'","'*'","'/'",
"'^'","'('","')'","';'","'='","'#'","'<'","'>'","'{'","'}'","input","exp","mul",
"unary","power","atom","args","bool_exp","bool_and","bool_unary","bool_simple",
"bool_atom", NULL
};
#endif

static const short yyr1[] = {     0,
    28,    28,    28,    29,    29,    29,    30,    30,    30,    31,
    31,    32,    32,    33,    33,    33,    33,    33,    34,    34,
    34,    34,    34,    34,    35,    35,    36,    36,    37,    37,
    38,    38,    38,    38,    38,    38,    38,    38,    38,    38,
    38,    39,    39,    39,    39,    39
};

static const short yyr2[] = {     0,
     0,     1,     1,     3,     3,     1,     3,     3,     1,     2,
     1,     3,     1,     3,     8,     4,     3,     1,     3,     3,
     3,     1,     1,     1,     3,     1,     3,     1,     2,     1,
     1,     3,     3,     3,     3,     3,     3,     3,     3,     3,
     3,     1,     3,     8,     4,     3
};

static const short yydefact[] = {     1,
    18,    42,     0,     0,     0,     0,     0,     0,     2,     6,
     9,    11,    13,     3,    26,    28,    30,    31,     0,     0,
     0,     0,     0,    29,     0,     0,    10,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,    24,    17,    22,     0,
    23,    46,     0,     0,     0,    14,    43,    37,    38,    34,
     4,     5,    32,    33,    35,    36,     7,     8,    12,    25,
    27,     0,     0,    41,    39,    40,    16,     0,    45,     0,
     0,    21,    19,    20,     0,     0,     0,     0,     0,     0,
    15,    44,     0,     0,     0
};

static const short yydefgoto[] = {    93,
    23,    10,    11,    12,    13,    50,    51,    15,    16,    17,
    18
};

static const short yypact[] = {   106,
-32768,-32768,    61,    71,    11,     6,    50,   106,   114,   -14,
-32768,    15,-32768,    -5,    25,-32768,-32768,     9,    57,    40,
   106,   106,   114,-32768,    33,    35,    15,    87,    -7,    50,
    50,    50,    50,    50,    50,    50,    50,    50,    50,    50,
     6,   106,   106,    22,    22,    22,-32768,-32768,   114,    29,
    -5,-32768,    93,   -11,    -6,-32768,-32768,    60,    60,    60,
   -14,   -14,    60,    60,    60,    60,-32768,-32768,-32768,    25,
-32768,    45,    63,-32768,-32768,-32768,-32768,    89,-32768,    50,
   106,-32768,   114,    -5,    84,    58,    50,   106,   120,    46,
-32768,-32768,   103,   110,-32768
};

static const short yypgoto[] = {-32768,
     4,    52,    79,   100,    75,   107,     0,    88,    83,   126,
    97
};


#define	YYLAST		143


static const short yytable[] = {    14,
    42,    39,    40,     9,    42,    42,    42,    29,     1,    80,
    28,    25,    26,     1,    81,     2,     3,     4,    44,    57,
    54,    55,    49,    49,     7,     6,     2,    72,    73,     7,
    45,    46,    41,    58,    59,    60,     8,    43,    63,    64,
    65,    66,     1,    47,     2,     3,     4,     8,    77,    78,
     5,    19,     1,    21,     6,    25,    26,    42,     7,     1,
    47,     2,     3,     4,     6,     8,    52,     5,     7,    42,
    20,     6,    92,    33,    34,     7,    48,    84,    88,    19,
    86,    83,     8,    85,    61,    62,    20,    90,    22,    21,
    89,     1,    82,     2,     3,     4,    22,    33,    34,     5,
    33,    34,    94,     6,    87,    27,    56,     7,     1,    95,
     2,     3,     4,    78,     8,    69,     5,    67,    68,    79,
     6,    30,    31,    32,     7,    71,    53,    33,    34,    70,
    24,     8,     0,    33,    34,    35,    36,    37,    38,    91,
    74,    75,    76
};

static const short yycheck[] = {     0,
    12,    16,    17,     0,    12,    12,    12,     8,     3,    21,
     7,     6,     7,     3,    21,     5,     6,     7,    10,    27,
    21,    22,    19,    20,    19,    15,     5,     6,     7,    19,
    22,    23,    18,    30,    31,    32,    26,    13,    35,    36,
    37,    38,     3,     4,     5,     6,     7,    26,    20,    21,
    11,    19,     3,    19,    15,     6,     7,    12,    19,     3,
     4,     5,     6,     7,    15,    26,    27,    11,    19,    12,
    26,    15,    27,    14,    15,    19,    20,    78,    21,    19,
    81,    78,    26,    80,    33,    34,    26,    88,    26,    19,
    87,     3,     4,     5,     6,     7,    26,    14,    15,    11,
    14,    15,     0,    15,    21,     6,    20,    19,     3,     0,
     5,     6,     7,    21,    26,    41,    11,    39,    40,    27,
    15,     8,     9,    10,    19,    43,    20,    14,    15,    42,
     5,    26,    -1,    14,    15,    22,    23,    24,    25,    20,
    44,    45,    46
};
/* -*-C-*-  Note some compilers choke on comments on `#line' lines.  */
#line 3 "/usr/share/bison/bison.simple"

/* Skeleton output parser for bison,
   Copyright (C) 1984, 1989, 1990 Free Software Foundation, Inc.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.  */

/* As a special exception, when this file is copied by Bison into a
   Bison output file, you may use that output file without restriction.
   This special exception was added by the Free Software Foundation
   in version 1.24 of Bison.  */

#ifndef alloca
#ifdef __GNUC__
#define alloca __builtin_alloca
#else /* not GNU C.  */
#if (!defined (__STDC__) && defined (sparc)) || defined (__sparc__) || defined (__sparc) || defined (__sgi)
#include <alloca.h>
#else /* not sparc */
#if defined (MSDOS) && !defined (__TURBOC__)
#include <malloc.h>
#else /* not MSDOS, or __TURBOC__ */
#if defined(_AIX)
#include <malloc.h>
 #pragma alloca
#else /* not MSDOS, __TURBOC__, or _AIX */
#ifdef __hpux
#ifdef __cplusplus
extern "C" {
void *alloca (unsigned int);
};
#else /* not __cplusplus */
void *alloca ();
#endif /* not __cplusplus */
#endif /* __hpux */
#endif /* not _AIX */
#endif /* not MSDOS, or __TURBOC__ */
#endif /* not sparc.  */
#endif /* not GNU C.  */
#endif /* alloca not defined.  */

/* This is the parser code that is written into each bison parser
  when the %semantic_parser declaration is not specified in the grammar.
  It was written by Richard Stallman by simplifying the hairy parser
  used when %semantic_parser is specified.  */

/* Note: there must be only one dollar sign in this file.
   It is replaced by the list of actions, each action
   as one case of the switch.  */

#define yyerrok		(yyerrstatus = 0)
#define yyclearin	(yychar = YYEMPTY)
#define YYEMPTY		-2
#define YYEOF		0
#define YYACCEPT	return(0)
#define YYABORT 	return(1)
#define YYERROR		goto yyerrlab1
/* Like YYERROR except do call yyerror.
   This remains here temporarily to ease the
   transition to the new meaning of YYERROR, for GCC.
   Once GCC version 2 has supplanted version 1, this can go.  */
#define YYFAIL		goto yyerrlab
#define YYRECOVERING()  (!!yyerrstatus)
#define YYBACKUP(token, value) \
do								\
  if (yychar == YYEMPTY && yylen == 1)				\
    { yychar = (token), yylval = (value);			\
      yychar1 = YYTRANSLATE (yychar);				\
      YYPOPSTACK;						\
      goto yybackup;						\
    }								\
  else								\
    { yyerror ("syntax error: cannot back up"); YYERROR; }	\
while (0)

#define YYTERROR	1
#define YYERRCODE	256

#ifndef YYPURE
#define YYLEX		yylex()
#endif

#ifdef YYPURE
#ifdef YYLSP_NEEDED
#ifdef YYLEX_PARAM
#define YYLEX		yylex(&yylval, &yylloc, YYLEX_PARAM)
#else
#define YYLEX		yylex(&yylval, &yylloc)
#endif
#else /* not YYLSP_NEEDED */
#ifdef YYLEX_PARAM
#define YYLEX		yylex(&yylval, YYLEX_PARAM)
#else
#define YYLEX		yylex(&yylval)
#endif
#endif /* not YYLSP_NEEDED */
#endif

/* If nonreentrant, generate the variables here */

#ifndef YYPURE

int	yychar;			/*  the lookahead symbol		*/
YYSTYPE	yylval;			/*  the semantic value of the		*/
				/*  lookahead symbol			*/

#ifdef YYLSP_NEEDED
YYLTYPE yylloc;			/*  location data for the lookahead	*/
				/*  symbol				*/
#endif

int yynerrs;			/*  number of parse errors so far       */
#endif  /* not YYPURE */

#if YYDEBUG != 0
int yydebug;			/*  nonzero means print parse trace	*/
/* Since this is uninitialized, it does not stop multiple parsers
   from coexisting.  */
#endif

/*  YYINITDEPTH indicates the initial size of the parser's stacks	*/

#ifndef	YYINITDEPTH
#define YYINITDEPTH 200
#endif

/*  YYMAXDEPTH is the maximum size the stacks can grow to
    (effective only if the built-in stack extension method is used).  */

#if YYMAXDEPTH == 0
#undef YYMAXDEPTH
#endif

#ifndef YYMAXDEPTH
#define YYMAXDEPTH 10000
#endif

/* Prevent warning if -Wstrict-prototypes.  */
#ifdef __GNUC__
int yyparse (void);
#endif

#if __GNUC__ > 1		/* GNU C and GNU C++ define this.  */
#define __yy_memcpy(TO,FROM,COUNT)	__builtin_memcpy(TO,FROM,COUNT)
#else				/* not GNU C or C++ */
#ifndef __cplusplus

/* This is the most reliable way to avoid incompatibilities
   in available built-in functions on various systems.  */
static void
__yy_memcpy (to, from, count)
     char *to;
     char *from;
     int count;
{
  register char *f = from;
  register char *t = to;
  register int i = count;

  while (i-- > 0)
    *t++ = *f++;
}

#else /* __cplusplus */

/* This is the most reliable way to avoid incompatibilities
   in available built-in functions on various systems.  */
static void
__yy_memcpy (char *to, char *from, int count)
{
  register char *f = from;
  register char *t = to;
  register int i = count;

  while (i-- > 0)
    *t++ = *f++;
}

#endif
#endif

#line 196 "/usr/share/bison/bison.simple"

/* The user can define YYPARSE_PARAM as the name of an argument to be passed
   into yyparse.  The argument should have type void *.
   It should actually point to an object.
   Grammar actions can access the variable by casting it
   to the proper pointer type.  */

#ifdef YYPARSE_PARAM
#ifdef __cplusplus
#define YYPARSE_PARAM_ARG void *YYPARSE_PARAM
#define YYPARSE_PARAM_DECL
#else /* not __cplusplus */
#define YYPARSE_PARAM_ARG YYPARSE_PARAM
#define YYPARSE_PARAM_DECL void *YYPARSE_PARAM;
#endif /* not __cplusplus */
#else /* not YYPARSE_PARAM */
#define YYPARSE_PARAM_ARG
#define YYPARSE_PARAM_DECL
#endif /* not YYPARSE_PARAM */

int
yyparse(YYPARSE_PARAM_ARG)
     YYPARSE_PARAM_DECL
{
  register int yystate;
  register int yyn;
  register short *yyssp;
  register YYSTYPE *yyvsp;
  int yyerrstatus;	/*  number of tokens to shift before error messages enabled */
  int yychar1 = 0;		/*  lookahead token as an internal (translated) token number */

  short	yyssa[YYINITDEPTH];	/*  the state stack			*/
  YYSTYPE yyvsa[YYINITDEPTH];	/*  the semantic value stack		*/

  short *yyss = yyssa;		/*  refer to the stacks thru separate pointers */
  YYSTYPE *yyvs = yyvsa;	/*  to allow yyoverflow to reallocate them elsewhere */

#ifdef YYLSP_NEEDED
  YYLTYPE yylsa[YYINITDEPTH];	/*  the location stack			*/
  YYLTYPE *yyls = yylsa;
  YYLTYPE *yylsp;

#define YYPOPSTACK   (yyvsp--, yyssp--, yylsp--)
#else
#define YYPOPSTACK   (yyvsp--, yyssp--)
#endif

  int yystacksize = YYINITDEPTH;

#ifdef YYPURE
  int yychar;
  YYSTYPE yylval;
  int yynerrs;
#ifdef YYLSP_NEEDED
  YYLTYPE yylloc;
#endif
#endif

  YYSTYPE yyval;		/*  the variable used to return		*/
				/*  semantic values from the action	*/
				/*  routines				*/

  int yylen;

#if YYDEBUG != 0
  if (yydebug)
    fprintf(stderr, "Starting parse\n");
#endif

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY;		/* Cause a token to be read.  */

  /* Initialize stack pointers.
     Waste one element of value and location stack
     so that they stay on the same level as the state stack.
     The wasted elements are never initialized.  */

  yyssp = yyss - 1;
  yyvsp = yyvs;
#ifdef YYLSP_NEEDED
  yylsp = yyls;
#endif

/* Push a new state, which is found in  yystate  .  */
/* In all cases, when you get here, the value and location stacks
   have just been pushed. so pushing a state here evens the stacks.  */
yynewstate:

  *++yyssp = yystate;

  if (yyssp >= yyss + yystacksize - 1)
    {
      /* Give user a chance to reallocate the stack */
      /* Use copies of these so that the &'s don't force the real ones into memory. */
      YYSTYPE *yyvs1 = yyvs;
      short *yyss1 = yyss;
#ifdef YYLSP_NEEDED
      YYLTYPE *yyls1 = yyls;
#endif

      /* Get the current used size of the three stacks, in elements.  */
      int size = yyssp - yyss + 1;

#ifdef yyoverflow
      /* Each stack pointer address is followed by the size of
	 the data in use in that stack, in bytes.  */
#ifdef YYLSP_NEEDED
      /* This used to be a conditional around just the two extra args,
	 but that might be undefined if yyoverflow is a macro.  */
      yyoverflow("parser stack overflow",
		 &yyss1, size * sizeof (*yyssp),
		 &yyvs1, size * sizeof (*yyvsp),
		 &yyls1, size * sizeof (*yylsp),
		 &yystacksize);
#else
      yyoverflow("parser stack overflow",
		 &yyss1, size * sizeof (*yyssp),
		 &yyvs1, size * sizeof (*yyvsp),
		 &yystacksize);
#endif

      yyss = yyss1; yyvs = yyvs1;
#ifdef YYLSP_NEEDED
      yyls = yyls1;
#endif
#else /* no yyoverflow */
      /* Extend the stack our own way.  */
      if (yystacksize >= YYMAXDEPTH)
	{
	  yyerror("parser stack overflow");
	  return 2;
	}
      yystacksize *= 2;
      if (yystacksize > YYMAXDEPTH)
	yystacksize = YYMAXDEPTH;
      yyss = (short *) alloca (yystacksize * sizeof (*yyssp));
      __yy_memcpy ((char *)yyss, (char *)yyss1, size * sizeof (*yyssp));
      yyvs = (YYSTYPE *) alloca (yystacksize * sizeof (*yyvsp));
      __yy_memcpy ((char *)yyvs, (char *)yyvs1, size * sizeof (*yyvsp));
#ifdef YYLSP_NEEDED
      yyls = (YYLTYPE *) alloca (yystacksize * sizeof (*yylsp));
      __yy_memcpy ((char *)yyls, (char *)yyls1, size * sizeof (*yylsp));
#endif
#endif /* no yyoverflow */

      yyssp = yyss + size - 1;
      yyvsp = yyvs + size - 1;
#ifdef YYLSP_NEEDED
      yylsp = yyls + size - 1;
#endif

#if YYDEBUG != 0
      if (yydebug)
	fprintf(stderr, "Stack size increased to %d\n", yystacksize);
#endif

      if (yyssp >= yyss + yystacksize - 1)
	YYABORT;
    }

#if YYDEBUG != 0
  if (yydebug)
    fprintf(stderr, "Entering state %d\n", yystate);
#endif

  goto yybackup;
 yybackup:

/* Do appropriate processing given the current state.  */
/* Read a lookahead token if we need one and don't already have one.  */
/* yyresume: */

  /* First try to decide what to do without reference to lookahead token.  */

  yyn = yypact[yystate];
  if (yyn == YYFLAG)
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* yychar is either YYEMPTY or YYEOF
     or a valid token in external form.  */

  if (yychar == YYEMPTY)
    {
#if YYDEBUG != 0
      if (yydebug)
	fprintf(stderr, "Reading a token: ");
#endif
      yychar = YYLEX;
    }

  /* Convert token to internal form (in yychar1) for indexing tables with */

  if (yychar <= 0)		/* This means end of input. */
    {
      yychar1 = 0;
      yychar = YYEOF;		/* Don't call YYLEX any more */

#if YYDEBUG != 0
      if (yydebug)
	fprintf(stderr, "Now at end of input.\n");
#endif
    }
  else
    {
      yychar1 = YYTRANSLATE(yychar);

#if YYDEBUG != 0
      if (yydebug)
	{
	  fprintf (stderr, "Next token is %d (%s", yychar, yytname[yychar1]);
	  /* Give the individual parser a way to print the precise meaning
	     of a token, for further debugging info.  */
#ifdef YYPRINT
	  YYPRINT (stderr, yychar, yylval);
#endif
	  fprintf (stderr, ")\n");
	}
#endif
    }

  yyn += yychar1;
  if (yyn < 0 || yyn > YYLAST || yycheck[yyn] != yychar1)
    goto yydefault;

  yyn = yytable[yyn];

  /* yyn is what to do for this token type in this state.
     Negative => reduce, -yyn is rule number.
     Positive => shift, yyn is new state.
       New state is final state => don't bother to shift,
       just return success.
     0, or most negative number => error.  */

  if (yyn < 0)
    {
      if (yyn == YYFLAG)
	goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }
  else if (yyn == 0)
    goto yyerrlab;

  if (yyn == YYFINAL)
    YYACCEPT;

  /* Shift the lookahead token.  */

#if YYDEBUG != 0
  if (yydebug)
    fprintf(stderr, "Shifting token %d (%s), ", yychar, yytname[yychar1]);
#endif

  /* Discard the token being shifted unless it is eof.  */
  if (yychar != YYEOF)
    yychar = YYEMPTY;

  *++yyvsp = yylval;
#ifdef YYLSP_NEEDED
  *++yylsp = yylloc;
#endif

  /* count tokens shifted since error; after three, turn off error status.  */
  if (yyerrstatus) yyerrstatus--;

  yystate = yyn;
  goto yynewstate;

/* Do the default action for the current state.  */
yydefault:

  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;

/* Do a reduction.  yyn is the number of a rule to reduce with.  */
yyreduce:
  yylen = yyr2[yyn];
  if (yylen > 0)
    yyval = yyvsp[1-yylen]; /* implement default value of the action */

#if YYDEBUG != 0
  if (yydebug)
    {
      int i;

      fprintf (stderr, "Reducing via rule %d (line %d), ",
	       yyn, yyrline[yyn]);

      /* Print the symbols being reduced, and their result.  */
      for (i = yyprhs[yyn]; yyrhs[i] > 0; i++)
	fprintf (stderr, "%s ", yytname[yyrhs[i]]);
      fprintf (stderr, " -> %s\n", yytname[yyr1[yyn]]);
    }
#endif


  switch (yyn) {

case 1:
#line 32 "yacc.dep.y"
{ ;
    break;}
case 2:
#line 33 "yacc.dep.y"
{ ;
    break;}
case 3:
#line 34 "yacc.dep.y"
{ ;
    break;}
case 4:
#line 37 "yacc.dep.y"
{ ;
    break;}
case 5:
#line 38 "yacc.dep.y"
{ ;
    break;}
case 6:
#line 39 "yacc.dep.y"
{ ;
    break;}
case 7:
#line 42 "yacc.dep.y"
{ ;
    break;}
case 8:
#line 43 "yacc.dep.y"
{ ;
    break;}
case 9:
#line 44 "yacc.dep.y"
{ ;
    break;}
case 10:
#line 46 "yacc.dep.y"
{ ;
    break;}
case 11:
#line 47 "yacc.dep.y"
{ ;
    break;}
case 12:
#line 50 "yacc.dep.y"
{ ;
    break;}
case 13:
#line 51 "yacc.dep.y"
{ ;
    break;}
case 14:
#line 54 "yacc.dep.y"
{ ;
    break;}
case 15:
#line 55 "yacc.dep.y"
{ ;
    break;}
case 16:
#line 56 "yacc.dep.y"
{ ;
    break;}
case 17:
#line 57 "yacc.dep.y"
{ ;
    break;}
case 18:
#line 58 "yacc.dep.y"
{ ;
    break;}
case 19:
#line 61 "yacc.dep.y"
{ ;
    break;}
case 20:
#line 62 "yacc.dep.y"
{ ;
    break;}
case 21:
#line 63 "yacc.dep.y"
{ ;
    break;}
case 22:
#line 64 "yacc.dep.y"
{ ;
    break;}
case 23:
#line 65 "yacc.dep.y"
{ ;
    break;}
case 24:
#line 66 "yacc.dep.y"
{ ;
    break;}
case 25:
#line 69 "yacc.dep.y"
{ ;
    break;}
case 26:
#line 70 "yacc.dep.y"
{ ;
    break;}
case 27:
#line 73 "yacc.dep.y"
{ ;
    break;}
case 28:
#line 74 "yacc.dep.y"
{ ;
    break;}
case 29:
#line 77 "yacc.dep.y"
{ ;
    break;}
case 30:
#line 78 "yacc.dep.y"
{ ;
    break;}
case 31:
#line 81 "yacc.dep.y"
{ ;
    break;}
case 32:
#line 82 "yacc.dep.y"
{ ;
    break;}
case 33:
#line 83 "yacc.dep.y"
{ ;
    break;}
case 34:
#line 84 "yacc.dep.y"
{ ;
    break;}
case 35:
#line 85 "yacc.dep.y"
{ ;
    break;}
case 36:
#line 86 "yacc.dep.y"
{ ;
    break;}
case 37:
#line 87 "yacc.dep.y"
{ ;
    break;}
case 38:
#line 88 "yacc.dep.y"
{ ;
    break;}
case 39:
#line 89 "yacc.dep.y"
{ ;
    break;}
case 40:
#line 90 "yacc.dep.y"
{ ;
    break;}
case 41:
#line 91 "yacc.dep.y"
{ ;
    break;}
case 42:
#line 94 "yacc.dep.y"
{ ;
    break;}
case 43:
#line 95 "yacc.dep.y"
{ ;
    break;}
case 44:
#line 96 "yacc.dep.y"
{ ;
    break;}
case 45:
#line 97 "yacc.dep.y"
{ ;
    break;}
case 46:
#line 98 "yacc.dep.y"
{ ;
    break;}
}
   /* the action file gets copied in in place of this dollarsign */
#line 498 "/usr/share/bison/bison.simple"

  yyvsp -= yylen;
  yyssp -= yylen;
#ifdef YYLSP_NEEDED
  yylsp -= yylen;
#endif

#if YYDEBUG != 0
  if (yydebug)
    {
      short *ssp1 = yyss - 1;
      fprintf (stderr, "state stack now");
      while (ssp1 != yyssp)
	fprintf (stderr, " %d", *++ssp1);
      fprintf (stderr, "\n");
    }
#endif

  *++yyvsp = yyval;

#ifdef YYLSP_NEEDED
  yylsp++;
  if (yylen == 0)
    {
      yylsp->first_line = yylloc.first_line;
      yylsp->first_column = yylloc.first_column;
      yylsp->last_line = (yylsp-1)->last_line;
      yylsp->last_column = (yylsp-1)->last_column;
      yylsp->text = 0;
    }
  else
    {
      yylsp->last_line = (yylsp+yylen-1)->last_line;
      yylsp->last_column = (yylsp+yylen-1)->last_column;
    }
#endif

  /* Now "shift" the result of the reduction.
     Determine what state that goes to,
     based on the state we popped back to
     and the rule number reduced by.  */

  yyn = yyr1[yyn];

  yystate = yypgoto[yyn - YYNTBASE] + *yyssp;
  if (yystate >= 0 && yystate <= YYLAST && yycheck[yystate] == *yyssp)
    yystate = yytable[yystate];
  else
    yystate = yydefgoto[yyn - YYNTBASE];

  goto yynewstate;

yyerrlab:   /* here on detecting error */

  if (! yyerrstatus)
    /* If not already recovering from an error, report this error.  */
    {
      ++yynerrs;

#ifdef YYERROR_VERBOSE
      yyn = yypact[yystate];

      if (yyn > YYFLAG && yyn < YYLAST)
	{
	  int size = 0;
	  char *msg;
	  int x, count;

	  count = 0;
	  /* Start X at -yyn if nec to avoid negative indexes in yycheck.  */
	  for (x = (yyn < 0 ? -yyn : 0);
	       x < (sizeof(yytname) / sizeof(char *)); x++)
	    if (yycheck[x + yyn] == x)
	      size += strlen(yytname[x]) + 15, count++;
	  msg = (char *) malloc(size + 15);
	  if (msg != 0)
	    {
	      strcpy(msg, "parse error");

	      if (count < 5)
		{
		  count = 0;
		  for (x = (yyn < 0 ? -yyn : 0);
		       x < (sizeof(yytname) / sizeof(char *)); x++)
		    if (yycheck[x + yyn] == x)
		      {
			strcat(msg, count == 0 ? ", expecting `" : " or `");
			strcat(msg, yytname[x]);
			strcat(msg, "'");
			count++;
		      }
		}
	      yyerror(msg);
	      free(msg);
	    }
	  else
	    yyerror ("parse error; also virtual memory exceeded");
	}
      else
#endif /* YYERROR_VERBOSE */
	yyerror("parse error");
    }

  goto yyerrlab1;
yyerrlab1:   /* here on error raised explicitly by an action */

  if (yyerrstatus == 3)
    {
      /* if just tried and failed to reuse lookahead token after an error, discard it.  */

      /* return failure if at end of input */
      if (yychar == YYEOF)
	YYABORT;

#if YYDEBUG != 0
      if (yydebug)
	fprintf(stderr, "Discarding token %d (%s).\n", yychar, yytname[yychar1]);
#endif

      yychar = YYEMPTY;
    }

  /* Else will try to reuse lookahead token
     after shifting the error token.  */

  yyerrstatus = 3;		/* Each real token shifted decrements this */

  goto yyerrhandle;

yyerrdefault:  /* current state does not do anything special for the error token. */

#if 0
  /* This is wrong; only states that explicitly want error tokens
     should shift them.  */
  yyn = yydefact[yystate];  /* If its default is to accept any token, ok.  Otherwise pop it.*/
  if (yyn) goto yydefault;
#endif

yyerrpop:   /* pop the current state because it cannot handle the error token */

  if (yyssp == yyss) YYABORT;
  yyvsp--;
  yystate = *--yyssp;
#ifdef YYLSP_NEEDED
  yylsp--;
#endif

#if YYDEBUG != 0
  if (yydebug)
    {
      short *ssp1 = yyss - 1;
      fprintf (stderr, "Error: state stack now");
      while (ssp1 != yyssp)
	fprintf (stderr, " %d", *++ssp1);
      fprintf (stderr, "\n");
    }
#endif

yyerrhandle:

  yyn = yypact[yystate];
  if (yyn == YYFLAG)
    goto yyerrdefault;

  yyn += YYTERROR;
  if (yyn < 0 || yyn > YYLAST || yycheck[yyn] != YYTERROR)
    goto yyerrdefault;

  yyn = yytable[yyn];
  if (yyn < 0)
    {
      if (yyn == YYFLAG)
	goto yyerrpop;
      yyn = -yyn;
      goto yyreduce;
    }
  else if (yyn == 0)
    goto yyerrpop;

  if (yyn == YYFINAL)
    YYACCEPT;

#if YYDEBUG != 0
  if (yydebug)
    fprintf(stderr, "Shifting error token, ");
#endif

  *++yyvsp = yylval;
#ifdef YYLSP_NEEDED
  *++yylsp = yylloc;
#endif

  yystate = yyn;
  goto yynewstate;
}
#line 101 "yacc.dep.y"


int deperror ( char *s )  /* Called by yyparse on error */
{
  printf ("ERROR: %s\n", s);
  setError( ERR_SYNTAX, "" );
  return 0;
}

void dependMainParse( const char *_code )
{
  initDependFlex( _code );
  depparse();
}
