
/*  A Bison parser, made from yacc.yy
    by GNU Bison version 1.28  */

#define YYBISON 1  /* Identify Bison output.  */

#define	T_FILE_OP	257
#define	T_MATCH_LINE	258
#define	T_LINE	259
#define	T_INPUT	260
#define	T_AMPERSAND	261
#define	T_ASTERISK	262
#define	T_CASE	263
#define	T_CHARACTER_LITERAL	264
#define	T_CIRCUMFLEX	265
#define	T_COLON	266
#define	T_COMMA	267
#define	T_CONST	268
#define	T_DEFAULT	269
#define	T_ENUM	270
#define	T_EQUAL	271
#define	T_FALSE	272
#define	T_FLOATING_PT_LITERAL	273
#define	T_GREATER_THAN_SIGN	274
#define	T_IDENTIFIER	275
#define	T_IN	276
#define	T_INOUT	277
#define	T_INTEGER_LITERAL	278
#define	T_LEFT_CURLY_BRACKET	279
#define	T_LEFT_PARANTHESIS	280
#define	T_LEFT_SQUARE_BRACKET	281
#define	T_LESS_THAN_SIGN	282
#define	T_MINUS_SIGN	283
#define	T_OUT	284
#define	T_PERCENT_SIGN	285
#define	T_PLUS_SIGN	286
#define	T_RIGHT_CURLY_BRACKET	287
#define	T_RIGHT_PARANTHESIS	288
#define	T_RIGHT_SQUARE_BRACKET	289
#define	T_SCOPE	290
#define	T_SEMICOLON	291
#define	T_SHIFTLEFT	292
#define	T_SHIFTRIGHT	293
#define	T_SOLIDUS	294
#define	T_STRING_LITERAL	295
#define	T_PRAGMA	296
#define	T_STRUCT	297
#define	T_SWITCH	298
#define	T_TILDE	299
#define	T_TRUE	300
#define	T_VERTICAL_LINE	301
#define	T_CLASS	302
#define	T_LESS_OR_EQUAL	303
#define	T_GREATER_OR_EQUAL	304
#define	T_ASSIGN	305
#define	T_NOTEQUAL	306
#define	T_MEMBER	307
#define	T_DELETE	308
#define	T_WHILE	309
#define	T_IF	310
#define	T_ELSE	311
#define	T_FOR	312
#define	T_DO	313
#define	T_INCR	314
#define	T_DECR	315
#define	T_MAIN	316
#define	T_FOREACH	317
#define	T_SUBST	318
#define	T_MATCH	319
#define	T_NOT	320
#define	T_RETURN	321
#define	T_SIGNAL	322
#define	T_EMIT	323
#define	T_IMPORT	324
#define	T_VAR	325
#define	T_UNKNOWN	326
#define	T_CATCH	327
#define	T_TRY	328
#define	T_RAISE	329
#define	T_RANGE	330
#define	T_CELL	331
#define	T_FROM	332
#define	T_PLUS_ASSIGN	333
#define	T_AND	334
#define	T_OR	335
#define	T_DOLLAR	336

#line 1 "yacc.yy"


#include "kscript_parsenode.h"
#include "kscript_parser.h"
#include <stdlib.h>

#include <qstring.h>

extern int yylex();

extern QString idl_lexFile;
extern int idl_line_no;

void kscriptInitFlex( const char *_code, int extension );

void yyerror( const char *s )
{
  theParser->parse_error( idl_lexFile, s, idl_line_no );
}


#line 24 "yacc.yy"
typedef union
{
  QString        *ident;
  KSParseNode    *node;
  KScript::Long   _int;
  QString        *_str;
  ushort          _char;
  KScript::Double _float;
} YYSTYPE;
#ifndef YYDEBUG
#define YYDEBUG 1
#endif

#include <stdio.h>

#ifndef __cplusplus
#ifndef __STDC__
#define const
#endif
#endif



#define	YYFINAL		356
#define	YYFLAG		-32768
#define	YYNTBASE	83

#define YYTRANSLATE(x) ((unsigned)(x) <= 336 ? yytranslate[x] : 136)

static const char yytranslate[] = {     0,
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
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     1,     3,     4,     5,     6,
     7,     8,     9,    10,    11,    12,    13,    14,    15,    16,
    17,    18,    19,    20,    21,    22,    23,    24,    25,    26,
    27,    28,    29,    30,    31,    32,    33,    34,    35,    36,
    37,    38,    39,    40,    41,    42,    43,    44,    45,    46,
    47,    48,    49,    50,    51,    52,    53,    54,    55,    56,
    57,    58,    59,    60,    61,    62,    63,    64,    65,    66,
    67,    68,    69,    70,    71,    72,    73,    74,    75,    76,
    77,    78,    79,    80,    81,    82
};

#if YYDEBUG != 0
static const short yyprhs[] = {     0,
     0,     1,     4,     6,     8,    11,    14,    17,    20,    23,
    27,    33,    39,    41,    43,    45,    49,    54,    62,    65,
    67,    71,    73,    77,    79,    82,    86,    90,    92,    96,
    98,   102,   104,   108,   112,   116,   120,   124,   128,   130,
   132,   136,   138,   142,   144,   148,   150,   154,   158,   160,
   164,   168,   170,   174,   178,   182,   185,   188,   191,   194,
   196,   201,   206,   210,   215,   217,   221,   223,   227,   229,
   232,   235,   238,   241,   243,   246,   249,   251,   253,   255,
   257,   259,   263,   267,   271,   273,   275,   277,   279,   281,
   283,   285,   287,   290,   294,   297,   301,   303,   306,   310,
   312,   320,   326,   332,   334,   337,   340,   344,   347,   349,
   353,   358,   361,   365,   366,   368,   370,   373,   376,   379,
   386,   392,   401,   404,   410,   417,   419,   423,   426,   431,
   434,   437,   439,   443,   446,   451,   452,   454,   456,   459,
   462,   468,   472,   475,   479,   481,   487,   491,   493,   496,
   506,   516,   521,   524,   534,   539,   546,   554,   564,   565,
   573,   576,   581
};

static const short yyrhs[] = {    -1,
    51,    95,     0,    84,     0,    85,     0,    85,    84,     0,
   131,    37,     0,   112,    37,     0,   116,    37,     0,   121,
    33,     0,    70,    21,    37,     0,    78,    21,    70,     8,
    37,     0,    78,    21,    70,    86,    37,     0,    42,     0,
    87,     0,    21,     0,    21,    13,    86,     0,    62,    25,
   126,    33,     0,    62,    26,   122,    34,    25,   126,    33,
     0,    12,    89,     0,    90,     0,    90,    13,    89,     0,
    91,     0,    90,    53,    21,     0,    21,     0,    36,    21,
     0,    93,    51,    92,     0,    93,    79,    92,     0,    93,
     0,    94,    81,    93,     0,    94,     0,    95,    80,    94,
     0,    95,     0,    96,    17,    95,     0,    96,    52,    95,
     0,    96,    49,    95,     0,    96,    50,    95,     0,    96,
    28,    95,     0,    96,    20,    95,     0,    96,     0,    97,
     0,    96,    47,    97,     0,    98,     0,    97,    11,    98,
     0,    99,     0,    98,     7,    99,     0,   100,     0,    99,
    39,   100,     0,    99,    38,   100,     0,   101,     0,   100,
    32,   101,     0,   100,    29,   101,     0,   102,     0,   101,
     8,   102,     0,   101,    40,   102,     0,   101,    31,   102,
     0,    29,   103,     0,    32,   103,     0,    45,   103,     0,
    66,   103,     0,   103,     0,   103,    27,    92,    35,     0,
   103,    25,    92,    33,     0,   103,    26,    34,     0,   103,
    26,   104,    34,     0,   105,     0,    92,    13,   104,     0,
    92,     0,   103,    53,    21,     0,   106,     0,    60,   107,
     0,    61,   107,     0,   107,    60,     0,   107,    61,     0,
   107,     0,   108,    65,     0,   108,    64,     0,   108,     0,
    91,     0,   109,     0,     6,     0,     4,     0,     3,    92,
    34,     0,    28,    96,    20,     0,    26,    92,    34,     0,
    24,     0,    77,     0,    76,     0,    41,     0,    10,     0,
    19,     0,    46,     0,    18,     0,    27,    35,     0,    27,
   110,    35,     0,    25,    33,     0,    25,   111,    33,     0,
     5,     0,    82,    24,     0,    92,    13,   110,     0,    92,
     0,    26,    92,    13,    95,    34,    13,   111,     0,    26,
    92,    13,    95,    34,     0,    43,    21,    25,   113,    33,
     0,   114,     0,   114,   113,     0,   121,    33,     0,    71,
   115,    37,     0,   131,    37,     0,    21,     0,    21,    13,
   115,     0,   117,    25,   118,    33,     0,    48,    21,     0,
    48,    21,    88,     0,     0,   119,     0,   120,     0,   120,
   119,     0,   121,    33,     0,   112,    37,     0,    68,    21,
    26,   124,    34,    37,     0,    68,    21,    26,    34,    37,
     0,    54,    26,    22,    21,    34,    25,   126,    33,     0,
   131,    37,     0,    21,    26,    34,    25,   126,     0,    21,
    26,   122,    34,    25,   126,     0,   123,     0,   123,    13,
   122,     0,    22,    21,     0,    22,    21,    51,    92,     0,
    30,    21,     0,    23,    21,     0,   125,     0,   125,    13,
   122,     0,    22,    21,     0,    22,    21,    51,    92,     0,
     0,   127,     0,   128,     0,   128,   127,     0,    92,    37,
     0,    75,    92,    13,    92,    37,     0,    69,    92,    37,
     0,    67,    37,     0,    67,    92,    37,     0,   132,     0,
    74,    25,   127,    33,   129,     0,    25,   127,    33,     0,
   130,     0,   130,   129,     0,    73,    26,    92,    13,    21,
    34,    25,   127,    33,     0,    15,    26,    21,    13,    21,
    34,    25,   127,    33,     0,    14,    21,    51,    95,     0,
   134,   135,     0,    58,    26,    92,    37,    92,    37,    92,
    34,   135,     0,    59,   135,   134,    37,     0,    56,    26,
    92,    34,   135,   133,     0,    63,    26,    21,    13,    92,
    34,   135,     0,    63,    26,    21,    13,    21,    13,    92,
    34,   135,     0,     0,    57,    56,    26,    92,    34,   135,
   133,     0,    57,   135,     0,    55,    26,    92,    34,     0,
    25,   126,    33,     0
};

#endif

#if YYDEBUG != 0
static const short yyrline[] = { 0,
   172,   176,   180,   187,   191,   200,   204,   208,   212,   216,
   221,   227,   233,   238,   245,   249,   258,   264,   272,   280,
   284,   293,   297,   305,   310,   322,   326,   330,   337,   341,
   348,   352,   359,   363,   367,   371,   375,   379,   383,   391,
   395,   403,   407,   415,   419,   427,   431,   435,   443,   447,
   451,   459,   463,   467,   471,   479,   483,   487,   491,   495,
   502,   506,   510,   514,   518,   525,   530,   537,   542,   549,
   553,   557,   562,   567,   574,   579,   584,   592,   596,   600,
   604,   609,   614,   615,   624,   629,   634,   639,   644,   649,
   654,   659,   664,   668,   672,   676,   680,   684,   692,   696,
   703,   707,   717,   725,   729,   737,   741,   745,   752,   757,
   766,   774,   779,   789,   793,   800,   804,   812,   816,   820,
   825,   830,   839,   846,   852,   860,   864,   871,   876,   881,
   886,   894,   898,   905,   910,   918,   922,   929,   933,   941,
   945,   949,   953,   957,   961,   965,   969,   976,   980,   988,
   994,  1005,  1013,  1017,  1021,  1025,  1029,  1034,  1044,  1048,
  1052,  1059,  1066
};
#endif


#if YYDEBUG != 0 || defined (YYERROR_VERBOSE)

static const char * const yytname[] = {   "$","error","$undefined.","T_FILE_OP",
"T_MATCH_LINE","T_LINE","T_INPUT","T_AMPERSAND","T_ASTERISK","T_CASE","T_CHARACTER_LITERAL",
"T_CIRCUMFLEX","T_COLON","T_COMMA","T_CONST","T_DEFAULT","T_ENUM","T_EQUAL",
"T_FALSE","T_FLOATING_PT_LITERAL","T_GREATER_THAN_SIGN","T_IDENTIFIER","T_IN",
"T_INOUT","T_INTEGER_LITERAL","T_LEFT_CURLY_BRACKET","T_LEFT_PARANTHESIS","T_LEFT_SQUARE_BRACKET",
"T_LESS_THAN_SIGN","T_MINUS_SIGN","T_OUT","T_PERCENT_SIGN","T_PLUS_SIGN","T_RIGHT_CURLY_BRACKET",
"T_RIGHT_PARANTHESIS","T_RIGHT_SQUARE_BRACKET","T_SCOPE","T_SEMICOLON","T_SHIFTLEFT",
"T_SHIFTRIGHT","T_SOLIDUS","T_STRING_LITERAL","T_PRAGMA","T_STRUCT","T_SWITCH",
"T_TILDE","T_TRUE","T_VERTICAL_LINE","T_CLASS","T_LESS_OR_EQUAL","T_GREATER_OR_EQUAL",
"T_ASSIGN","T_NOTEQUAL","T_MEMBER","T_DELETE","T_WHILE","T_IF","T_ELSE","T_FOR",
"T_DO","T_INCR","T_DECR","T_MAIN","T_FOREACH","T_SUBST","T_MATCH","T_NOT","T_RETURN",
"T_SIGNAL","T_EMIT","T_IMPORT","T_VAR","T_UNKNOWN","T_CATCH","T_TRY","T_RAISE",
"T_RANGE","T_CELL","T_FROM","T_PLUS_ASSIGN","T_AND","T_OR","T_DOLLAR","specification",
"definitions","definition","import_list","main","inheritance_spec","qualified_names",
"qualified_name","scoped_name","assign_expr","bool_or","bool_and","equal_expr",
"or_expr","xor_expr","and_expr","shift_expr","add_expr","mult_expr","unary_expr",
"index_expr","func_call_params","member_expr","incr_expr","match_expr","primary_expr",
"literal","array_elements","dict_elements","struct_dcl","struct_exports","struct_export",
"struct_members","class_dcl","class_header","class_body","class_exports","class_export",
"func_dcl","func_params","func_param","signal_params","signal_param","func_body",
"func_lines","func_line","catches","single_catch","const_dcl","loops","else",
"while","loop_body", NULL
};
#endif

static const short yyr1[] = {     0,
    83,    83,    83,    84,    84,    85,    85,    85,    85,    85,
    85,    85,    85,    85,    86,    86,    87,    87,    88,    89,
    89,    90,    90,    91,    91,    92,    92,    92,    93,    93,
    94,    94,    95,    95,    95,    95,    95,    95,    95,    96,
    96,    97,    97,    98,    98,    99,    99,    99,   100,   100,
   100,   101,   101,   101,   101,   102,   102,   102,   102,   102,
   103,   103,   103,   103,   103,   104,   104,   105,   105,   106,
   106,   106,   106,   106,   107,   107,   107,   108,   108,   108,
   108,   108,   108,   108,   109,   109,   109,   109,   109,   109,
   109,   109,   109,   109,   109,   109,   109,   109,   110,   110,
   111,   111,   112,   113,   113,   114,   114,   114,   115,   115,
   116,   117,   117,   118,   118,   119,   119,   120,   120,   120,
   120,   120,   120,   121,   121,   122,   122,   123,   123,   123,
   123,   124,   124,   125,   125,   126,   126,   127,   127,   128,
   128,   128,   128,   128,   128,   128,   128,   129,   129,   130,
   130,   131,   132,   132,   132,   132,   132,   132,   133,   133,
   133,   134,   135
};

static const short yyr2[] = {     0,
     0,     2,     1,     1,     2,     2,     2,     2,     2,     3,
     5,     5,     1,     1,     1,     3,     4,     7,     2,     1,
     3,     1,     3,     1,     2,     3,     3,     1,     3,     1,
     3,     1,     3,     3,     3,     3,     3,     3,     1,     1,
     3,     1,     3,     1,     3,     1,     3,     3,     1,     3,
     3,     1,     3,     3,     3,     2,     2,     2,     2,     1,
     4,     4,     3,     4,     1,     3,     1,     3,     1,     2,
     2,     2,     2,     1,     2,     2,     1,     1,     1,     1,
     1,     3,     3,     3,     1,     1,     1,     1,     1,     1,
     1,     1,     2,     3,     2,     3,     1,     2,     3,     1,
     7,     5,     5,     1,     2,     2,     3,     2,     1,     3,
     4,     2,     3,     0,     1,     1,     2,     2,     2,     6,
     5,     8,     2,     5,     6,     1,     3,     2,     4,     2,
     2,     1,     3,     2,     4,     0,     1,     1,     2,     2,
     5,     3,     2,     3,     1,     5,     3,     1,     2,     9,
     9,     4,     2,     9,     4,     6,     7,     9,     0,     7,
     2,     4,     3
};

static const short yydefact[] = {     1,
     0,     0,    13,     0,     0,     0,     0,     0,     0,     3,
     4,    14,     0,     0,     0,     0,     0,     0,     0,     0,
   112,     0,    81,    97,    80,    89,    92,    90,    24,    85,
     0,     0,     0,     0,     0,     0,     0,    88,     0,    91,
     0,     0,     0,    87,    86,     0,    78,     2,    39,    40,
    42,    44,    46,    49,    52,    60,    65,    69,    74,    77,
    79,   136,     0,     0,     0,     5,     7,     8,   114,     9,
     6,     0,     0,     0,     0,     0,     0,   126,     0,     0,
   113,     0,    28,    30,    32,     0,    95,     0,     0,    93,
   100,     0,     0,    56,    57,    25,    58,    70,    71,    59,
    98,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,    72,    73,    76,    75,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,   137,   138,   145,
     0,     0,    10,     0,     0,     0,     0,     0,   115,   116,
     0,     0,   152,   128,   131,   130,   136,     0,     0,     0,
     0,   104,     0,     0,    19,    20,    22,    82,     0,     0,
     0,     0,     0,    96,    84,     0,    94,    83,    33,    38,
    37,    41,    35,    36,    34,    43,    45,    48,    47,    51,
    50,    53,    55,    54,     0,    63,    67,     0,     0,    68,
     0,     0,     0,     0,     0,   136,     0,     0,   143,     0,
     0,     0,     0,   140,    17,   139,   153,     0,     0,    15,
     0,     0,     0,   119,   111,   117,   118,   123,     0,   124,
   136,   127,   109,     0,   103,   105,   106,   108,     0,     0,
    26,    27,    29,    31,     0,    99,    62,     0,    64,    61,
     0,   147,     0,     0,     0,     0,     0,     0,   144,   142,
     0,     0,   136,    11,     0,    12,     0,     0,   129,   125,
     0,   107,    21,    23,     0,    66,   162,     0,     0,   163,
   155,     0,     0,     0,     0,    16,     0,     0,     0,     0,
   132,   110,   102,   159,     0,    24,     0,     0,     0,   146,
   148,   141,    18,     0,   134,   121,     0,     0,     0,     0,
   156,     0,     0,     0,     0,     0,   149,   136,     0,   120,
   133,   101,     0,   161,     0,     0,   157,     0,     0,     0,
   135,     0,     0,     0,     0,     0,   122,     0,   154,   158,
     0,     0,     0,     0,     0,   159,     0,     0,   160,     0,
     0,   151,   150,     0,     0,     0
};

static const short yydefgoto[] = {   354,
    10,    11,   221,    12,    81,   165,   166,    47,   136,    83,
    84,    85,    49,    50,    51,    52,    53,    54,    55,    56,
   198,    57,    58,    59,    60,    61,    92,    88,    13,   161,
   162,   234,    14,    15,   148,   149,   150,    16,    77,    78,
   290,   291,   137,   138,   139,   300,   301,    17,   140,   311,
   141,   207
};

static const short yypact[] = {   107,
     6,    -1,-32768,    23,    38,   565,    36,    47,    50,-32768,
   130,-32768,    20,    46,    22,    52,    77,    44,    56,    91,
   108,   565,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
    13,   565,   370,   565,    48,    48,   110,-32768,    48,-32768,
   674,   674,    48,-32768,-32768,   105,-32768,-32768,   257,   124,
   136,    43,     2,    10,-32768,     3,-32768,-32768,    57,    58,
-32768,   295,   104,   109,    75,-32768,-32768,-32768,    -5,-32768,
-32768,   565,   135,   138,   139,   132,   127,   149,    -6,    12,
-32768,   129,   -34,    84,    86,   565,-32768,   137,   134,-32768,
   161,   141,    -7,     3,     3,-32768,     3,-32768,-32768,     3,
-32768,   565,   565,   565,   565,   565,   565,   565,   565,   565,
   565,   565,   565,   565,   565,   565,   565,   565,   435,   565,
   159,-32768,-32768,-32768,-32768,   206,   158,   160,   162,   164,
   165,   500,   565,   168,   565,   150,   157,-32768,   295,-32768,
   164,   163,-32768,    16,   170,   177,   166,   169,-32768,    -5,
   172,   176,-32768,   148,-32768,-32768,   295,   181,   104,   180,
   182,    -6,   184,   183,-32768,     7,-32768,-32768,   565,   565,
   565,   565,   201,-32768,-32768,   565,-32768,-32768,-32768,-32768,
-32768,   124,-32768,-32768,-32768,   136,    43,     2,     2,    10,
    10,-32768,-32768,-32768,   185,-32768,   208,   194,   202,-32768,
   565,   186,   565,   565,   565,   295,   174,   215,-32768,   204,
   207,   295,   232,-32768,-32768,-32768,-32768,   221,   212,   237,
   216,   233,   228,-32768,-32768,-32768,-32768,-32768,   565,-32768,
   295,-32768,   243,   222,-32768,-32768,-32768,-32768,    12,   242,
-32768,-32768,-32768,-32768,   565,-32768,-32768,   565,-32768,-32768,
     8,-32768,   224,   234,   239,   238,   241,   266,-32768,-32768,
   251,   565,   295,-32768,   249,-32768,   265,     1,-32768,-32768,
   180,-32768,-32768,-32768,   253,-32768,-32768,   164,   565,-32768,
-32768,   630,     4,   252,   259,-32768,   261,   272,   271,   262,
   289,-32768,   290,   254,   275,   302,   283,   292,   299,-32768,
     4,-32768,-32768,   301,   277,-32768,   293,   104,   303,   -13,
-32768,   565,   565,   164,   311,   565,-32768,   295,   565,-32768,
-32768,-32768,   307,-32768,   300,   304,-32768,   322,   324,   306,
-32768,   565,   164,   164,   321,   323,-32768,   309,-32768,-32768,
   312,   313,   164,   320,   327,   254,   295,   295,-32768,   315,
   316,-32768,-32768,   357,   359,-32768
};

static const short yypgoto[] = {-32768,
   349,-32768,    98,-32768,-32768,   126,-32768,   -75,   -22,   195,
   196,    -2,   333,   273,   270,   274,    26,    28,   -24,    97,
   133,-32768,-32768,   111,-32768,-32768,   209,    73,   -62,   225,
-32768,   112,-32768,-32768,-32768,   236,-32768,   -47,   -60,-32768,
-32768,-32768,  -151,  -125,-32768,    89,-32768,   -43,-32768,    54,
   197,  -139
};


#define	YYLAST		756


static const short yytable[] = {    82,
   202,   217,   142,    48,   167,   230,   147,     1,     1,    89,
    91,   206,   178,   216,     2,     2,   169,   115,   298,   239,
   245,   151,   288,   219,    19,   152,    18,   118,   119,   120,
   113,   163,    29,   114,   289,   164,   220,     4,    86,   105,
   116,   175,   323,    20,   170,    87,    69,    37,   145,   117,
    22,    23,    24,    25,   256,   121,    67,    26,    21,   240,
    62,    63,   146,   173,   160,    27,    28,    64,    29,   153,
    65,    30,    31,    32,    33,    34,   299,    73,    74,   270,
   111,   112,    68,    37,    70,    75,   261,   147,    38,    76,
   192,   193,   194,    40,    72,   195,   197,   199,   232,   179,
   180,   181,   151,   183,   184,   185,   152,    41,    42,   210,
   211,   285,   213,    71,   163,    79,   122,   123,   164,    80,
     1,   124,   125,    44,    45,    73,    74,     2,   101,    46,
    96,    94,    95,    75,   109,    97,   188,   189,   294,   100,
   190,   191,   110,     1,   144,   143,   241,   242,     3,     4,
     2,    98,    99,    91,     5,   154,   157,     6,   155,   156,
   158,   159,   168,   167,   171,   172,   330,   175,     7,   174,
   324,     3,     4,   176,   327,   177,     8,     5,   251,   200,
   253,   254,   255,   203,     9,   204,   214,   205,   206,   215,
   208,     7,   212,   339,   340,   222,   218,   223,   229,     8,
   233,   225,   224,   346,   227,   231,   269,     9,    22,    23,
    24,    25,   228,   245,   235,    26,   237,   247,   252,   238,
   248,   350,   351,    27,    28,   197,    29,   249,   127,    30,
   126,   201,    33,    34,    35,   258,   250,    36,    87,   284,
   259,    37,   275,   260,   262,   263,    38,   321,   264,   265,
    39,    40,   266,   268,   267,   271,   295,   277,   272,   297,
   127,   128,   274,   129,   130,    41,    42,   278,   131,   220,
   280,    43,   132,   102,   133,   279,   103,   281,   282,   134,
   135,    44,    45,   283,   104,   287,   293,    46,   302,   325,
   326,   303,   305,   329,   304,   307,   331,    22,    23,    24,
    25,   308,   309,   105,    26,   106,   107,   306,   108,   338,
   310,   312,    27,    28,   313,    29,   314,   315,    30,   126,
    32,    33,    34,    35,   316,   318,    36,   319,    86,   320,
    37,   328,   332,   333,   335,    38,   336,   334,   337,    39,
    40,   341,   343,   342,   347,   344,   345,   352,   353,   127,
   128,   348,   129,   130,    41,    42,   355,   131,   356,    66,
    43,   132,   286,   133,   273,   243,    93,   244,   134,   135,
    44,    45,    22,    23,    24,    25,    46,   182,   186,    26,
   276,   322,   292,   187,   246,   226,   236,    27,    28,   317,
    29,     0,     0,    30,    31,    32,    33,    34,    35,   349,
     0,    36,     0,   257,    90,    37,     0,     0,     0,     0,
    38,     0,     0,     0,    39,    40,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,    41,
    42,     0,     0,     0,     0,    43,     0,    22,    23,    24,
    25,     0,     0,     0,    26,    44,    45,     0,     0,     0,
     0,    46,    27,    28,     0,    29,     0,     0,    30,    31,
    32,    33,    34,    35,     0,     0,    36,     0,   196,     0,
    37,     0,     0,     0,     0,    38,     0,     0,     0,    39,
    40,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,    41,    42,     0,     0,     0,     0,
    43,     0,    22,    23,    24,    25,     0,     0,     0,    26,
    44,    45,     0,     0,     0,     0,    46,    27,    28,     0,
    29,     0,     0,    30,    31,    32,    33,    34,    35,     0,
     0,    36,     0,     0,     0,    37,   209,     0,     0,     0,
    38,     0,     0,     0,    39,    40,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,    41,
    42,     0,     0,     0,     0,    43,     0,    22,    23,    24,
    25,     0,     0,     0,    26,    44,    45,     0,     0,     0,
     0,    46,    27,    28,     0,    29,     0,     0,    30,    31,
    32,    33,    34,    35,     0,     0,    36,     0,     0,     0,
    37,     0,     0,     0,     0,    38,     0,     0,     0,    39,
    40,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,    41,    42,     0,     0,     0,     0,
    43,     0,    22,    23,    24,    25,     0,     0,     0,    26,
    44,    45,     0,     0,     0,     0,    46,    27,    28,     0,
   296,     0,     0,    30,    31,    32,    33,    34,    35,     0,
     0,    36,     0,     0,     0,    37,     0,     0,     0,     0,
    38,     0,     0,     0,    39,    40,    22,    23,    24,    25,
     0,     0,     0,    26,     0,     0,     0,     0,     0,    41,
    42,    27,    28,     0,    29,    43,     0,    30,    31,    32,
    33,    34,     0,     0,     0,    44,    45,     0,     0,    37,
     0,    46,     0,     0,    38,     0,     0,     0,     0,    40,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,    44,
    45,     0,     0,     0,     0,    46
};

static const short yycheck[] = {    22,
   126,   141,    63,     6,    80,   157,    69,    14,    14,    32,
    33,    25,    20,   139,    21,    21,    51,     8,    15,    13,
    13,    69,    22,     8,    26,    69,    21,    25,    26,    27,
    29,    79,    21,    32,    34,    79,    21,    43,    26,    47,
    31,    34,    56,    21,    79,    33,    25,    36,    54,    40,
     3,     4,     5,     6,   206,    53,    37,    10,    21,    53,
    25,    26,    68,    86,    71,    18,    19,    21,    21,    72,
    21,    24,    25,    26,    27,    28,    73,    22,    23,   231,
    38,    39,    37,    36,    33,    30,   212,   150,    41,    34,
   115,   116,   117,    46,    51,   118,   119,   120,   159,   102,
   103,   104,   150,   106,   107,   108,   150,    60,    61,   132,
   133,   263,   135,    37,   162,    25,    60,    61,   162,    12,
    14,    64,    65,    76,    77,    22,    23,    21,    24,    82,
    21,    35,    36,    30,    11,    39,   111,   112,   278,    43,
   113,   114,     7,    14,    70,    37,   169,   170,    42,    43,
    21,    41,    42,   176,    48,    21,    25,    51,    21,    21,
    34,    13,    34,   239,    81,    80,   318,    34,    62,    33,
   310,    42,    43,    13,   314,    35,    70,    48,   201,    21,
   203,   204,   205,    26,    78,    26,    37,    26,    25,    33,
    26,    62,    25,   333,   334,    26,    34,    21,    51,    70,
    21,    33,    37,   343,    33,    25,   229,    78,     3,     4,
     5,     6,    37,    13,    33,    10,    33,    33,    33,    37,
    13,   347,   348,    18,    19,   248,    21,    34,    55,    24,
    25,    26,    27,    28,    29,    21,    35,    32,    33,   262,
    37,    36,   245,    37,    13,    25,    41,   308,    37,    13,
    45,    46,    37,    26,    22,    13,   279,    34,    37,   282,
    55,    56,    21,    58,    59,    60,    61,    34,    63,    21,
    33,    66,    67,    17,    69,    37,    20,    37,    13,    74,
    75,    76,    77,    33,    28,    21,    34,    82,    37,   312,
   313,    33,    21,   316,    34,    34,   319,     3,     4,     5,
     6,    13,    13,    47,    10,    49,    50,    37,    52,   332,
    57,    37,    18,    19,    13,    21,    34,    26,    24,    25,
    26,    27,    28,    29,    26,    25,    32,    51,    26,    37,
    36,    21,    26,    34,    13,    41,    13,    34,    33,    45,
    46,    21,    34,    21,    25,    34,    34,    33,    33,    55,
    56,    25,    58,    59,    60,    61,     0,    63,     0,    11,
    66,    67,   265,    69,   239,   171,    34,   172,    74,    75,
    76,    77,     3,     4,     5,     6,    82,   105,   109,    10,
   248,   309,   271,   110,   176,   150,   162,    18,    19,   301,
    21,    -1,    -1,    24,    25,    26,    27,    28,    29,   346,
    -1,    32,    -1,   207,    35,    36,    -1,    -1,    -1,    -1,
    41,    -1,    -1,    -1,    45,    46,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    60,
    61,    -1,    -1,    -1,    -1,    66,    -1,     3,     4,     5,
     6,    -1,    -1,    -1,    10,    76,    77,    -1,    -1,    -1,
    -1,    82,    18,    19,    -1,    21,    -1,    -1,    24,    25,
    26,    27,    28,    29,    -1,    -1,    32,    -1,    34,    -1,
    36,    -1,    -1,    -1,    -1,    41,    -1,    -1,    -1,    45,
    46,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    60,    61,    -1,    -1,    -1,    -1,
    66,    -1,     3,     4,     5,     6,    -1,    -1,    -1,    10,
    76,    77,    -1,    -1,    -1,    -1,    82,    18,    19,    -1,
    21,    -1,    -1,    24,    25,    26,    27,    28,    29,    -1,
    -1,    32,    -1,    -1,    -1,    36,    37,    -1,    -1,    -1,
    41,    -1,    -1,    -1,    45,    46,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    60,
    61,    -1,    -1,    -1,    -1,    66,    -1,     3,     4,     5,
     6,    -1,    -1,    -1,    10,    76,    77,    -1,    -1,    -1,
    -1,    82,    18,    19,    -1,    21,    -1,    -1,    24,    25,
    26,    27,    28,    29,    -1,    -1,    32,    -1,    -1,    -1,
    36,    -1,    -1,    -1,    -1,    41,    -1,    -1,    -1,    45,
    46,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    60,    61,    -1,    -1,    -1,    -1,
    66,    -1,     3,     4,     5,     6,    -1,    -1,    -1,    10,
    76,    77,    -1,    -1,    -1,    -1,    82,    18,    19,    -1,
    21,    -1,    -1,    24,    25,    26,    27,    28,    29,    -1,
    -1,    32,    -1,    -1,    -1,    36,    -1,    -1,    -1,    -1,
    41,    -1,    -1,    -1,    45,    46,     3,     4,     5,     6,
    -1,    -1,    -1,    10,    -1,    -1,    -1,    -1,    -1,    60,
    61,    18,    19,    -1,    21,    66,    -1,    24,    25,    26,
    27,    28,    -1,    -1,    -1,    76,    77,    -1,    -1,    36,
    -1,    82,    -1,    -1,    41,    -1,    -1,    -1,    -1,    46,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    76,
    77,    -1,    -1,    -1,    -1,    82
};
/* -*-C-*-  Note some compilers choke on comments on `#line' lines.  */
#line 3 "//usr/lib/bison.simple"
/* This file comes from bison-1.28.  */

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
   Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.  */

/* As a special exception, when this file is copied by Bison into a
   Bison output file, you may use that output file without restriction.
   This special exception was added by the Free Software Foundation
   in version 1.24 of Bison.  */

/* This is the parser code that is written into each bison parser
  when the %semantic_parser declaration is not specified in the grammar.
  It was written by Richard Stallman by simplifying the hairy parser
  used when %semantic_parser is specified.  */

#ifndef YYSTACK_USE_ALLOCA
#ifdef alloca
#define YYSTACK_USE_ALLOCA
#else /* alloca not defined */
#ifdef __GNUC__
#define YYSTACK_USE_ALLOCA
#define alloca __builtin_alloca
#else /* not GNU C.  */
#if (!defined (__STDC__) && defined (sparc)) || defined (__sparc__) || defined (__sparc) || defined (__sgi) || (defined (__sun) && defined (__i386))
#define YYSTACK_USE_ALLOCA
#include <alloca.h>
#else /* not sparc */
/* We think this test detects Watcom and Microsoft C.  */
/* This used to test MSDOS, but that is a bad idea
   since that symbol is in the user namespace.  */
#if (defined (_MSDOS) || defined (_MSDOS_)) && !defined (__TURBOC__)
#if 0 /* No need for malloc.h, which pollutes the namespace;
	 instead, just don't use alloca.  */
#include <malloc.h>
#endif
#else /* not MSDOS, or __TURBOC__ */
#if defined(_AIX)
/* I don't know what this was needed for, but it pollutes the namespace.
   So I turned it off.   rms, 2 May 1997.  */
/* #include <malloc.h>  */
 #pragma alloca
#define YYSTACK_USE_ALLOCA
#else /* not MSDOS, or __TURBOC__, or _AIX */
#if 0
#ifdef __hpux /* haible@ilog.fr says this works for HPUX 9.05 and up,
		 and on HPUX 10.  Eventually we can turn this on.  */
#define YYSTACK_USE_ALLOCA
#define alloca __builtin_alloca
#endif /* __hpux */
#endif
#endif /* not _AIX */
#endif /* not MSDOS, or __TURBOC__ */
#endif /* not sparc */
#endif /* not GNU C */
#endif /* alloca not defined */
#endif /* YYSTACK_USE_ALLOCA not defined */

#ifdef YYSTACK_USE_ALLOCA
#define YYSTACK_ALLOC alloca
#else
#define YYSTACK_ALLOC malloc
#endif

/* Note: there must be only one dollar sign in this file.
   It is replaced by the list of actions, each action
   as one case of the switch.  */

#define yyerrok		(yyerrstatus = 0)
#define yyclearin	(yychar = YYEMPTY)
#define YYEMPTY		-2
#define YYEOF		0
#define YYACCEPT	goto yyacceptlab
#define YYABORT 	goto yyabortlab
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

/* Define __yy_memcpy.  Note that the size argument
   should be passed with type unsigned int, because that is what the non-GCC
   definitions require.  With GCC, __builtin_memcpy takes an arg
   of type size_t, but it can handle unsigned int.  */

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
     unsigned int count;
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
__yy_memcpy (char *to, char *from, unsigned int count)
{
  register char *t = to;
  register char *f = from;
  register int i = count;

  while (i-- > 0)
    *t++ = *f++;
}

#endif
#endif

#line 217 "//usr/lib/bison.simple"

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

/* Prevent warning if -Wstrict-prototypes.  */
#ifdef __GNUC__
#ifdef YYPARSE_PARAM
int yyparse (void *);
#else
int yyparse (void);
#endif
#endif

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
  int yyfree_stacks = 0;

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
	  if (yyfree_stacks)
	    {
	      free (yyss);
	      free (yyvs);
#ifdef YYLSP_NEEDED
	      free (yyls);
#endif
	    }
	  return 2;
	}
      yystacksize *= 2;
      if (yystacksize > YYMAXDEPTH)
	yystacksize = YYMAXDEPTH;
#ifndef YYSTACK_USE_ALLOCA
      yyfree_stacks = 1;
#endif
      yyss = (short *) YYSTACK_ALLOC (yystacksize * sizeof (*yyssp));
      __yy_memcpy ((char *)yyss, (char *)yyss1,
		   size * (unsigned int) sizeof (*yyssp));
      yyvs = (YYSTYPE *) YYSTACK_ALLOC (yystacksize * sizeof (*yyvsp));
      __yy_memcpy ((char *)yyvs, (char *)yyvs1,
		   size * (unsigned int) sizeof (*yyvsp));
#ifdef YYLSP_NEEDED
      yyls = (YYLTYPE *) YYSTACK_ALLOC (yystacksize * sizeof (*yylsp));
      __yy_memcpy ((char *)yyls, (char *)yyls1,
		   size * (unsigned int) sizeof (*yylsp));
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
#line 173 "yacc.yy"
{
	    theParser->setRootNode( NULL );
	  ;
    break;}
case 2:
#line 177 "yacc.yy"
{
	    theParser->setRootNode( yyvsp[0].node );
	  ;
    break;}
case 3:
#line 181 "yacc.yy"
{
	    theParser->setRootNode( new KSParseNode( definitions, yyvsp[0].node ) );
	  ;
    break;}
case 4:
#line 188 "yacc.yy"
{
	    yyval.node = new KSParseNode( definitions, yyvsp[0].node );
	  ;
    break;}
case 5:
#line 192 "yacc.yy"
{
	    yyval.node = new KSParseNode( definitions, yyvsp[-1].node );
	    yyval.node->setBranch( 2, yyvsp[0].node );
	  ;
    break;}
case 6:
#line 201 "yacc.yy"
{
	    yyval.node = yyvsp[-1].node;
	  ;
    break;}
case 7:
#line 205 "yacc.yy"
{
	    yyval.node = yyvsp[-1].node;
	  ;
    break;}
case 8:
#line 209 "yacc.yy"
{
	    yyval.node = yyvsp[-1].node;
	  ;
    break;}
case 9:
#line 213 "yacc.yy"
{
	    yyval.node = yyvsp[-1].node;
	  ;
    break;}
case 10:
#line 217 "yacc.yy"
{
	    yyval.node = new KSParseNode( import );
	    yyval.node->setIdent( yyvsp[-1].ident );
	  ;
    break;}
case 11:
#line 222 "yacc.yy"
{
	    yyval.node = new KSParseNode( from );
	    yyval.node->setIdent( yyvsp[-3].ident );
	    yyval.node->setStringLiteral( QString( "" ) );
	  ;
    break;}
case 12:
#line 228 "yacc.yy"
{
	    yyval.node = new KSParseNode( from );
	    yyval.node->setIdent( yyvsp[-3].ident );
	    yyval.node->setStringLiteral( yyvsp[-1]._str );
	  ;
    break;}
case 13:
#line 234 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_pragma );
	    yyval.node->setIdent( yyvsp[0]._str );
	  ;
    break;}
case 14:
#line 239 "yacc.yy"
{
	    yyval.node = yyvsp[0].node;
	  ;
    break;}
case 15:
#line 246 "yacc.yy"
{
		yyval._str = yyvsp[0].ident;
	  ;
    break;}
case 16:
#line 250 "yacc.yy"
{
		(*yyvsp[-2].ident) += "/";
		(*yyvsp[-2].ident) += (*yyvsp[0]._str);
		yyval._str = yyvsp[-2].ident;
	  ;
    break;}
case 17:
#line 259 "yacc.yy"
{
	    yyval.node = new KSParseNode( func_dcl );
	    yyval.node->setBranch( 2, yyvsp[-1].node );
	    yyval.node->setIdent( "main" );
	  ;
    break;}
case 18:
#line 265 "yacc.yy"
{
	    yyval.node = new KSParseNode( func_dcl, yyvsp[-4].node, yyvsp[-1].node );
	    yyval.node->setIdent( "main" );
	  ;
    break;}
case 19:
#line 273 "yacc.yy"
{
	    yyval.node = yyvsp[0].node;
	  ;
    break;}
case 20:
#line 281 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_qualified_names, yyvsp[0].node );
	  ;
    break;}
case 21:
#line 285 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_qualified_names, yyvsp[-2].node );
	    yyval.node->setBranch( 2, yyvsp[0].node );
	  ;
    break;}
case 22:
#line 294 "yacc.yy"
{
	    yyval.node = yyvsp[0].node;
	  ;
    break;}
case 23:
#line 298 "yacc.yy"
{
	    yyval.node = new KSParseNode( member_expr, yyvsp[-2].node );
	    yyval.node->setIdent( yyvsp[0].ident );
	  ;
    break;}
case 24:
#line 306 "yacc.yy"
{
	    yyval.node = new KSParseNode( scoped_name );
	    yyval.node->setIdent( yyvsp[0].ident );
	  ;
    break;}
case 25:
#line 311 "yacc.yy"
{
	    yyval.node = new KSParseNode( scoped_name );
	    QString name = "::";
	    name += *(yyvsp[0].ident);
	    delete yyvsp[0].ident;
	    yyval.node->setIdent( name );
	  ;
    break;}
case 26:
#line 323 "yacc.yy"
{
	    yyval.node = new KSParseNode( assign_expr, yyvsp[-2].node, yyvsp[0].node );
	  ;
    break;}
case 27:
#line 327 "yacc.yy"
{
	    yyval.node = new KSParseNode( plus_assign, yyvsp[-2].node, yyvsp[0].node );
	  ;
    break;}
case 28:
#line 331 "yacc.yy"
{
	    yyval.node = yyvsp[0].node;
	  ;
    break;}
case 29:
#line 338 "yacc.yy"
{
	    yyval.node = new KSParseNode( bool_or, yyvsp[-2].node, yyvsp[0].node );
	  ;
    break;}
case 30:
#line 342 "yacc.yy"
{
	    yyval.node = yyvsp[0].node;
	  ;
    break;}
case 31:
#line 349 "yacc.yy"
{
	    yyval.node = new KSParseNode( bool_and, yyvsp[-2].node, yyvsp[0].node );
	  ;
    break;}
case 32:
#line 353 "yacc.yy"
{
	    yyval.node = yyvsp[0].node;
	  ;
    break;}
case 33:
#line 360 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_equal, yyvsp[-2].node, yyvsp[0].node );
	  ;
    break;}
case 34:
#line 364 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_notequal, yyvsp[-2].node, yyvsp[0].node );
	  ;
    break;}
case 35:
#line 368 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_less_or_equal, yyvsp[-2].node, yyvsp[0].node );
	  ;
    break;}
case 36:
#line 372 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_greater_or_equal, yyvsp[-2].node, yyvsp[0].node );
	  ;
    break;}
case 37:
#line 376 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_less, yyvsp[-2].node, yyvsp[0].node );
	  ;
    break;}
case 38:
#line 380 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_greater, yyvsp[-2].node, yyvsp[0].node );
	  ;
    break;}
case 39:
#line 384 "yacc.yy"
{
	    yyval.node = yyvsp[0].node;
	  ;
    break;}
case 40:
#line 392 "yacc.yy"
{
	    yyval.node = yyvsp[0].node;
	  ;
    break;}
case 41:
#line 396 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_vertical_line, yyvsp[-2].node, yyvsp[0].node );
	  ;
    break;}
case 42:
#line 404 "yacc.yy"
{
	    yyval.node = yyvsp[0].node;
	  ;
    break;}
case 43:
#line 408 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_circumflex, yyvsp[-2].node, yyvsp[0].node );
	  ;
    break;}
case 44:
#line 416 "yacc.yy"
{
	    yyval.node = yyvsp[0].node;
	  ;
    break;}
case 45:
#line 420 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_ampersand, yyvsp[-2].node, yyvsp[0].node );
	  ;
    break;}
case 46:
#line 428 "yacc.yy"
{
	    yyval.node = yyvsp[0].node;
	  ;
    break;}
case 47:
#line 432 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_shiftright, yyvsp[-2].node, yyvsp[0].node );
	  ;
    break;}
case 48:
#line 436 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_shiftleft, yyvsp[-2].node, yyvsp[0].node );
	  ;
    break;}
case 49:
#line 444 "yacc.yy"
{
	    yyval.node = yyvsp[0].node;
	  ;
    break;}
case 50:
#line 448 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_plus_sign, yyvsp[-2].node, yyvsp[0].node );
	  ;
    break;}
case 51:
#line 452 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_minus_sign, yyvsp[-2].node, yyvsp[0].node );
	  ;
    break;}
case 52:
#line 460 "yacc.yy"
{
	    yyval.node = yyvsp[0].node;
	  ;
    break;}
case 53:
#line 464 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_asterik, yyvsp[-2].node, yyvsp[0].node );
	  ;
    break;}
case 54:
#line 468 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_solidus, yyvsp[-2].node, yyvsp[0].node );
	  ;
    break;}
case 55:
#line 472 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_percent_sign, yyvsp[-2].node, yyvsp[0].node );
	  ;
    break;}
case 56:
#line 480 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_minus_sign, yyvsp[0].node );
	  ;
    break;}
case 57:
#line 484 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_plus_sign, yyvsp[0].node );
	  ;
    break;}
case 58:
#line 488 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_tilde, yyvsp[0].node );
	  ;
    break;}
case 59:
#line 492 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_not, yyvsp[0].node );
	  ;
    break;}
case 60:
#line 496 "yacc.yy"
{
	    yyval.node = yyvsp[0].node;
	  ;
    break;}
case 61:
#line 503 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_array, yyvsp[-3].node, yyvsp[-1].node );
	  ;
    break;}
case 62:
#line 507 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_dict, yyvsp[-3].node, yyvsp[-1].node );
	  ;
    break;}
case 63:
#line 511 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_func_call, yyvsp[-2].node );
	  ;
    break;}
case 64:
#line 515 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_func_call, yyvsp[-3].node, yyvsp[-1].node );
	  ;
    break;}
case 65:
#line 519 "yacc.yy"
{
	    yyval.node = yyvsp[0].node;
	  ;
    break;}
case 66:
#line 526 "yacc.yy"
{
	    yyval.node = new KSParseNode( func_call_params, yyvsp[-2].node, yyvsp[0].node );
	  ;
    break;}
case 67:
#line 531 "yacc.yy"
{
	    yyval.node = new KSParseNode( func_call_params, yyvsp[0].node );
	  ;
    break;}
case 68:
#line 538 "yacc.yy"
{
	    yyval.node = new KSParseNode( member_expr, yyvsp[-2].node );
	    yyval.node->setIdent( yyvsp[0].ident );
	  ;
    break;}
case 69:
#line 543 "yacc.yy"
{
	    yyval.node = yyvsp[0].node;
	  ;
    break;}
case 70:
#line 550 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_incr, yyvsp[0].node );
	  ;
    break;}
case 71:
#line 554 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_decr, yyvsp[0].node );
	  ;
    break;}
case 72:
#line 558 "yacc.yy"
{
	    /* Setting $1 twice indicates that this is a postfix operator */
	    yyval.node = new KSParseNode( t_incr, yyvsp[-1].node, yyvsp[-1].node );
	  ;
    break;}
case 73:
#line 563 "yacc.yy"
{
	    /* Setting $1 twice indicates that this is a postfix operator */
	    yyval.node = new KSParseNode( t_decr, yyvsp[-1].node, yyvsp[-1].node );
	  ;
    break;}
case 74:
#line 568 "yacc.yy"
{
	    yyval.node = yyvsp[0].node;
	  ;
    break;}
case 75:
#line 575 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_match, yyvsp[-1].node );
	    yyval.node->setIdent( yyvsp[0]._str );
	  ;
    break;}
case 76:
#line 580 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_subst, yyvsp[-1].node );
	    yyval.node->setIdent( yyvsp[0].ident );
	  ;
    break;}
case 77:
#line 585 "yacc.yy"
{
	    yyval.node = yyvsp[0].node;
	  ;
    break;}
case 78:
#line 593 "yacc.yy"
{
	    yyval.node = yyvsp[0].node;
	  ;
    break;}
case 79:
#line 597 "yacc.yy"
{
	    yyval.node = yyvsp[0].node;
	  ;
    break;}
case 80:
#line 601 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_input );
	  ;
    break;}
case 81:
#line 605 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_match_line );
	    yyval.node->setIdent( yyvsp[0]._str );
	  ;
    break;}
case 82:
#line 610 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_file_op, yyvsp[-1].node );
	    yyval.node->setIdent( yyvsp[-2]._str );
	  ;
    break;}
case 83:
#line 614 "yacc.yy"
{ ;
    break;}
case 84:
#line 616 "yacc.yy"
{
	    yyval.node = yyvsp[-1].node;
	  ;
    break;}
case 85:
#line 625 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_integer_literal );
	    yyval.node->setIntegerLiteral( yyvsp[0]._int );
	  ;
    break;}
case 86:
#line 630 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_cell );
	    yyval.node->setStringLiteral( yyvsp[0]._str );
	  ;
    break;}
case 87:
#line 635 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_range );
	    yyval.node->setStringLiteral( yyvsp[0]._str );
	  ;
    break;}
case 88:
#line 640 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_string_literal );
	    yyval.node->setStringLiteral( yyvsp[0]._str );
	  ;
    break;}
case 89:
#line 645 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_character_literal );
	    yyval.node->setCharacterLiteral( yyvsp[0]._char );
	  ;
    break;}
case 90:
#line 650 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_floating_pt_literal );
	    yyval.node->setFloatingPtLiteral( yyvsp[0]._float );
	  ;
    break;}
case 91:
#line 655 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_boolean_literal );
	    yyval.node->setBooleanLiteral( true );
	  ;
    break;}
case 92:
#line 660 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_boolean_literal );
	    yyval.node->setBooleanLiteral( false );
	  ;
    break;}
case 93:
#line 665 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_array_const );
	  ;
    break;}
case 94:
#line 669 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_array_const, yyvsp[-1].node );
	  ;
    break;}
case 95:
#line 673 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_dict_const );
	  ;
    break;}
case 96:
#line 677 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_dict_const, yyvsp[-1].node );
	  ;
    break;}
case 97:
#line 681 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_line );
	  ;
    break;}
case 98:
#line 685 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_regexp_group );
	    yyval.node->setIntegerLiteral( yyvsp[0]._int );
	  ;
    break;}
case 99:
#line 693 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_array_element, yyvsp[-2].node, yyvsp[0].node );
	  ;
    break;}
case 100:
#line 697 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_array_element, yyvsp[0].node  );
	  ;
    break;}
case 101:
#line 704 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_dict_element, yyvsp[-5].node, yyvsp[-3].node, yyvsp[0].node );
	  ;
    break;}
case 102:
#line 708 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_dict_element, yyvsp[-3].node, yyvsp[-1].node );
	  ;
    break;}
case 103:
#line 718 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_struct, yyvsp[-1].node );
	    yyval.node->setIdent( yyvsp[-3].ident );
	  ;
    break;}
case 104:
#line 726 "yacc.yy"
{
	    yyval.node = new KSParseNode( exports, yyvsp[0].node );
	  ;
    break;}
case 105:
#line 730 "yacc.yy"
{
	    yyval.node = new KSParseNode( exports, yyvsp[-1].node );
	    yyval.node->setBranch( 2, yyvsp[0].node );
	  ;
    break;}
case 106:
#line 738 "yacc.yy"
{
	    yyval.node = yyvsp[-1].node;
	  ;
    break;}
case 107:
#line 742 "yacc.yy"
{
	    yyval.node = yyvsp[-1].node;
	  ;
    break;}
case 108:
#line 746 "yacc.yy"
{
	    yyval.node = yyvsp[-1].node;
	  ;
    break;}
case 109:
#line 753 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_struct_members );
	    yyval.node->setIdent( yyvsp[0].ident );
	  ;
    break;}
case 110:
#line 758 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_struct_members, yyvsp[0].node );
	    yyval.node->setIdent( yyvsp[-2].ident );
	  ;
    break;}
case 111:
#line 767 "yacc.yy"
{
	    yyval.node = new KSParseNode( class_dcl, yyvsp[-3].node, yyvsp[-1].node );
	  ;
    break;}
case 112:
#line 775 "yacc.yy"
{
	    yyval.node = new KSParseNode( class_header );
	    yyval.node->setIdent( yyvsp[0].ident );
	  ;
    break;}
case 113:
#line 780 "yacc.yy"
{
	    yyval.node = new KSParseNode( class_header );
	    yyval.node->setIdent( yyvsp[-1].ident );
	    yyval.node->setBranch( 1, yyvsp[0].node );
	  ;
    break;}
case 114:
#line 790 "yacc.yy"
{
	    yyval.node = NULL;
	  ;
    break;}
case 115:
#line 794 "yacc.yy"
{
	    yyval.node = yyvsp[0].node;
	  ;
    break;}
case 116:
#line 801 "yacc.yy"
{
	    yyval.node = new KSParseNode( exports, yyvsp[0].node );
	  ;
    break;}
case 117:
#line 805 "yacc.yy"
{
	    yyval.node = new KSParseNode( exports, yyvsp[-1].node );
	    yyval.node->setBranch( 2, yyvsp[0].node );
	  ;
    break;}
case 118:
#line 813 "yacc.yy"
{
	    yyval.node = yyvsp[-1].node;
	  ;
    break;}
case 119:
#line 817 "yacc.yy"
{
	    yyval.node = yyvsp[-1].node;
	  ;
    break;}
case 120:
#line 821 "yacc.yy"
{
	    yyval.node = new KSParseNode( signal_dcl, yyvsp[-2].node );
	    yyval.node->setIdent( yyvsp[-4].ident );
	  ;
    break;}
case 121:
#line 826 "yacc.yy"
{
	    yyval.node = new KSParseNode( signal_dcl, 0 );
	    yyval.node->setIdent( yyvsp[-3].ident );
	  ;
    break;}
case 122:
#line 831 "yacc.yy"
{
	    yyval.node = new KSParseNode( destructor_dcl );
	    KSParseNode* n = new KSParseNode( func_param_in );
	    n->setIdent( yyvsp[-4].ident );
	    yyval.node->setBranch( 1, n );
	    yyval.node->setBranch( 2, yyvsp[-1].node );
	    yyval.node->setIdent( "delete" );
	  ;
    break;}
case 123:
#line 840 "yacc.yy"
{
	    yyval.node = yyvsp[-1].node;
	  ;
    break;}
case 124:
#line 847 "yacc.yy"
{
	    yyval.node = new KSParseNode( func_dcl );
	    yyval.node->setBranch( 2, yyvsp[0].node );
	    yyval.node->setIdent( yyvsp[-4].ident );
	  ;
    break;}
case 125:
#line 853 "yacc.yy"
{
	    yyval.node = new KSParseNode( func_dcl, yyvsp[-3].node, yyvsp[0].node );
	    yyval.node->setIdent( yyvsp[-5].ident );
	  ;
    break;}
case 126:
#line 861 "yacc.yy"
{
	    yyval.node = new KSParseNode( func_params, yyvsp[0].node );
	  ;
    break;}
case 127:
#line 865 "yacc.yy"
{
	    yyval.node = new KSParseNode( func_params, yyvsp[-2].node, yyvsp[0].node );
	  ;
    break;}
case 128:
#line 872 "yacc.yy"
{
	    yyval.node = new KSParseNode( func_param_in );
	    yyval.node->setIdent( yyvsp[0].ident );
	  ;
    break;}
case 129:
#line 877 "yacc.yy"
{
	    yyval.node = new KSParseNode( func_param_in, yyvsp[0].node );
	    yyval.node->setIdent( yyvsp[-2].ident );
	  ;
    break;}
case 130:
#line 882 "yacc.yy"
{
	    yyval.node = new KSParseNode( func_param_out );
	    yyval.node->setIdent( yyvsp[0].ident );
	  ;
    break;}
case 131:
#line 887 "yacc.yy"
{
	    yyval.node = new KSParseNode( func_param_inout );
	    yyval.node->setIdent( yyvsp[0].ident );
	  ;
    break;}
case 132:
#line 895 "yacc.yy"
{
	    yyval.node = new KSParseNode( signal_params, yyvsp[0].node );
	  ;
    break;}
case 133:
#line 899 "yacc.yy"
{
	    yyval.node = new KSParseNode( signal_params, yyvsp[-2].node, yyvsp[0].node );
	  ;
    break;}
case 134:
#line 906 "yacc.yy"
{
	    yyval.node = new KSParseNode( signal_param );
	    yyval.node->setIdent( yyvsp[0].ident );
	  ;
    break;}
case 135:
#line 911 "yacc.yy"
{
	    yyval.node = new KSParseNode( signal_param, yyvsp[0].node );
	    yyval.node->setIdent( yyvsp[-2].ident );
	  ;
    break;}
case 136:
#line 919 "yacc.yy"
{
	    yyval.node = NULL;
	  ;
    break;}
case 137:
#line 923 "yacc.yy"
{
	    yyval.node = yyvsp[0].node;
	  ;
    break;}
case 138:
#line 930 "yacc.yy"
{
	    yyval.node = new KSParseNode( func_lines, yyvsp[0].node );
	  ;
    break;}
case 139:
#line 934 "yacc.yy"
{
	    yyval.node = new KSParseNode( func_lines, yyvsp[-1].node );
	    yyval.node->setBranch( 2, yyvsp[0].node );
	  ;
    break;}
case 140:
#line 942 "yacc.yy"
{
	    yyval.node = yyvsp[-1].node;
	  ;
    break;}
case 141:
#line 946 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_raise, yyvsp[-3].node, yyvsp[-1].node );
	  ;
    break;}
case 142:
#line 950 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_emit, yyvsp[-1].node );
	  ;
    break;}
case 143:
#line 954 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_return );
	  ;
    break;}
case 144:
#line 958 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_return, yyvsp[-1].node );
	  ;
    break;}
case 145:
#line 962 "yacc.yy"
{
	    yyval.node = yyvsp[0].node;
	  ;
    break;}
case 146:
#line 966 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_try, yyvsp[-2].node, yyvsp[0].node );
	  ;
    break;}
case 147:
#line 970 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_scope, yyvsp[-1].node );
	  ;
    break;}
case 148:
#line 977 "yacc.yy"
{
	    yyval.node = yyvsp[0].node;
	  ;
    break;}
case 149:
#line 981 "yacc.yy"
{
	    yyval.node = yyvsp[-1].node;
	    yyval.node->setBranch( 4, yyvsp[0].node );
	  ;
    break;}
case 150:
#line 990 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_catch, yyvsp[-6].node, yyvsp[-1].node );
	    yyval.node->setIdent( yyvsp[-4].ident );
	  ;
    break;}
case 151:
#line 996 "yacc.yy"
{
	    KSParseNode* x = new KSParseNode( scoped_name );
	    x->setIdent( yyvsp[-6].ident );
	    yyval.node = new KSParseNode( t_catch_default, x, yyvsp[-1].node );
	    yyval.node->setIdent( yyvsp[-4].ident );
	  ;
    break;}
case 152:
#line 1006 "yacc.yy"
{
	    yyval.node = new KSParseNode( const_dcl, yyvsp[0].node );
	    yyval.node->setIdent( yyvsp[-2].ident );
	  ;
    break;}
case 153:
#line 1014 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_while, yyvsp[-1].node, yyvsp[0].node );
	  ;
    break;}
case 154:
#line 1018 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_for, yyvsp[-6].node, yyvsp[-4].node, yyvsp[-2].node, yyvsp[0].node );
	  ;
    break;}
case 155:
#line 1022 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_do, yyvsp[-2].node, yyvsp[-1].node );
	  ;
    break;}
case 156:
#line 1026 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_if, yyvsp[-3].node, yyvsp[-1].node, yyvsp[0].node );
	  ;
    break;}
case 157:
#line 1030 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_foreach, yyvsp[-2].node, yyvsp[0].node );
	    yyval.node->setIdent( yyvsp[-4].ident );
	  ;
    break;}
case 158:
#line 1035 "yacc.yy"
{
	    /* We set $9 twice to indicate thet this is the foreach for maps */
	    yyval.node = new KSParseNode( t_foreach, yyvsp[-2].node, yyvsp[0].node, yyvsp[0].node );
	    yyval.node->setIdent( yyvsp[-6].ident );
	    yyval.node->setStringLiteral( yyvsp[-4].ident );
	  ;
    break;}
case 159:
#line 1045 "yacc.yy"
{
	    yyval.node = NULL;
	  ;
    break;}
case 160:
#line 1049 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_if, yyvsp[-3].node, yyvsp[-1].node, yyvsp[0].node );
	  ;
    break;}
case 161:
#line 1053 "yacc.yy"
{
	    yyval.node = yyvsp[0].node;
	  ;
    break;}
case 162:
#line 1060 "yacc.yy"
{
	    yyval.node = yyvsp[-1].node;
	  ;
    break;}
case 163:
#line 1067 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_scope, yyvsp[-1].node );
	  ;
    break;}
}
   /* the action file gets copied in in place of this dollarsign */
#line 543 "//usr/lib/bison.simple"

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

 yyacceptlab:
  /* YYACCEPT comes here.  */
  if (yyfree_stacks)
    {
      free (yyss);
      free (yyvs);
#ifdef YYLSP_NEEDED
      free (yyls);
#endif
    }
  return 0;

 yyabortlab:
  /* YYABORT comes here.  */
  if (yyfree_stacks)
    {
      free (yyss);
      free (yyvs);
#ifdef YYLSP_NEEDED
      free (yyls);
#endif
    }
  return 1;
}
#line 1071 "yacc.yy"


void kscriptParse( const char *_code, int extension )
{
    kscriptInitFlex( _code, extension );
    yyparse();
}
