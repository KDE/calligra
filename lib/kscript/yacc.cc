
/*  A Bison parser, made from yacc.yy
 by  GNU Bison version 1.25
  */

#define YYBISON 1  /* Identify Bison output.  */

#define	T_AMPERSAND	258
#define	T_ASTERISK	259
#define	T_CASE	260
#define	T_CHARACTER_LITERAL	261
#define	T_CIRCUMFLEX	262
#define	T_COLON	263
#define	T_COMMA	264
#define	T_CONST	265
#define	T_DEFAULT	266
#define	T_ENUM	267
#define	T_EQUAL	268
#define	T_FALSE	269
#define	T_FLOATING_PT_LITERAL	270
#define	T_GREATER_THAN_SIGN	271
#define	T_IDENTIFIER	272
#define	T_IN	273
#define	T_INOUT	274
#define	T_INTEGER_LITERAL	275
#define	T_LEFT_CURLY_BRACKET	276
#define	T_LEFT_PARANTHESIS	277
#define	T_LEFT_SQUARE_BRACKET	278
#define	T_LESS_THAN_SIGN	279
#define	T_MINUS_SIGN	280
#define	T_OUT	281
#define	T_PERCENT_SIGN	282
#define	T_PLUS_SIGN	283
#define	T_RIGHT_CURLY_BRACKET	284
#define	T_RIGHT_PARANTHESIS	285
#define	T_RIGHT_SQUARE_BRACKET	286
#define	T_SCOPE	287
#define	T_SEMICOLON	288
#define	T_SHIFTLEFT	289
#define	T_SHIFTRIGHT	290
#define	T_SOLIDUS	291
#define	T_STRING_LITERAL	292
#define	T_PRAGMA	293
#define	T_STRUCT	294
#define	T_SWITCH	295
#define	T_TILDE	296
#define	T_TRUE	297
#define	T_VERTICAL_LINE	298
#define	T_CLASS	299
#define	T_LESS_OR_EQUAL	300
#define	T_GREATER_OR_EQUAL	301
#define	T_ASSIGN	302
#define	T_NOTEQUAL	303
#define	T_MEMBER	304
#define	T_DELETE	305
#define	T_WHILE	306
#define	T_IF	307
#define	T_ELSE	308
#define	T_FOR	309
#define	T_DO	310
#define	T_INCR	311
#define	T_DECR	312
#define	T_MAIN	313
#define	T_FOREACH	314
#define	T_SUBST	315
#define	T_MATCH	316
#define	T_NOT	317
#define	T_RETURN	318
#define	T_SIGNAL	319
#define	T_EMIT	320
#define	T_IMPORT	321
#define	T_VAR	322
#define	T_UNKNOWN	323
#define	T_CATCH	324
#define	T_TRY	325
#define	T_RAISE	326
#define	T_RANGE	327
#define	T_CELL	328
#define	T_FROM	329
#define	T_PLUS_ASSIGN	330
#define	T_AND	331
#define	T_OR	332

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



#define	YYFINAL		339
#define	YYFLAG		-32768
#define	YYNTBASE	78

#define YYTRANSLATE(x) ((unsigned)(x) <= 332 ? yytranslate[x] : 131)

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
     2,     2,     2,     2,     2,     1,     2,     3,     4,     5,
     6,     7,     8,     9,    10,    11,    12,    13,    14,    15,
    16,    17,    18,    19,    20,    21,    22,    23,    24,    25,
    26,    27,    28,    29,    30,    31,    32,    33,    34,    35,
    36,    37,    38,    39,    40,    41,    42,    43,    44,    45,
    46,    47,    48,    49,    50,    51,    52,    53,    54,    55,
    56,    57,    58,    59,    60,    61,    62,    63,    64,    65,
    66,    67,    68,    69,    70,    71,    72,    73,    74,    75,
    76,    77
};

#if YYDEBUG != 0
static const short yyprhs[] = {     0,
     0,     1,     4,     6,     8,    11,    14,    17,    20,    23,
    27,    33,    39,    41,    43,    45,    49,    54,    57,    59,
    63,    65,    69,    71,    74,    78,    82,    84,    88,    90,
    94,    96,   100,   104,   108,   112,   116,   120,   122,   124,
   128,   130,   134,   136,   140,   142,   146,   150,   152,   156,
   160,   162,   166,   170,   174,   177,   180,   183,   186,   188,
   193,   198,   202,   207,   209,   213,   215,   219,   221,   224,
   227,   230,   233,   235,   238,   241,   243,   245,   247,   251,
   253,   255,   257,   259,   261,   263,   265,   267,   270,   274,
   277,   281,   285,   287,   295,   301,   307,   309,   312,   315,
   319,   322,   324,   328,   333,   336,   340,   341,   343,   345,
   348,   351,   354,   361,   367,   376,   379,   385,   392,   394,
   398,   401,   406,   409,   412,   414,   418,   421,   426,   427,
   429,   431,   434,   437,   443,   447,   450,   454,   456,   462,
   466,   468,   471,   481,   491,   496,   499,   509,   514,   521,
   529,   539,   540,   548,   551,   556
};

static const short yyrhs[] = {    -1,
    47,    90,     0,    79,     0,    80,     0,    80,    79,     0,
   126,    33,     0,   107,    33,     0,   111,    33,     0,   116,
    29,     0,    66,    17,    33,     0,    74,    17,    66,     4,
    33,     0,    74,    17,    66,    81,    33,     0,    38,     0,
    82,     0,    17,     0,    17,     9,    81,     0,    58,    21,
   121,    29,     0,     8,    84,     0,    85,     0,    85,     9,
    84,     0,    86,     0,    85,    49,    17,     0,    17,     0,
    32,    17,     0,    88,    47,    87,     0,    88,    75,    87,
     0,    88,     0,    89,    77,    88,     0,    89,     0,    90,
    76,    89,     0,    90,     0,    91,    13,    90,     0,    91,
    48,    90,     0,    91,    45,    90,     0,    91,    46,    90,
     0,    91,    24,    90,     0,    91,    16,    90,     0,    91,
     0,    92,     0,    91,    43,    92,     0,    93,     0,    92,
     7,    93,     0,    94,     0,    93,     3,    94,     0,    95,
     0,    94,    35,    95,     0,    94,    34,    95,     0,    96,
     0,    95,    28,    96,     0,    95,    25,    96,     0,    97,
     0,    96,     4,    97,     0,    96,    36,    97,     0,    96,
    27,    97,     0,    25,    98,     0,    28,    98,     0,    41,
    98,     0,    62,    98,     0,    98,     0,    98,    23,    87,
    31,     0,    98,    21,    87,    29,     0,    98,    22,    30,
     0,    98,    22,    99,    30,     0,   100,     0,    87,     9,
    99,     0,    87,     0,    98,    49,    17,     0,   101,     0,
    56,   102,     0,    57,   102,     0,   102,    56,     0,   102,
    57,     0,   102,     0,   103,    61,     0,   103,    60,     0,
   103,     0,    86,     0,   104,     0,    22,    87,    30,     0,
    20,     0,    73,     0,    72,     0,    37,     0,     6,     0,
    15,     0,    42,     0,    14,     0,    23,    31,     0,    23,
   105,    31,     0,    21,    29,     0,    21,   106,    29,     0,
    87,     9,   105,     0,    87,     0,    22,    87,     9,    90,
    30,     9,   106,     0,    22,    87,     9,    90,    30,     0,
    39,    17,    21,   108,    29,     0,   109,     0,   109,   108,
     0,   116,    29,     0,    67,   110,    33,     0,   126,    33,
     0,    17,     0,    17,     9,   110,     0,   112,    21,   113,
    29,     0,    44,    17,     0,    44,    17,    83,     0,     0,
   114,     0,   115,     0,   115,   114,     0,   116,    29,     0,
   107,    33,     0,    64,    17,    22,   119,    30,    33,     0,
    64,    17,    22,    30,    33,     0,    50,    22,    18,    17,
    30,    21,   121,    29,     0,   126,    33,     0,    17,    22,
    30,    21,   121,     0,    17,    22,   117,    30,    21,   121,
     0,   118,     0,   118,     9,   117,     0,    18,    17,     0,
    18,    17,    47,    87,     0,    26,    17,     0,    19,    17,
     0,   120,     0,   120,     9,   117,     0,    18,    17,     0,
    18,    17,    47,    87,     0,     0,   122,     0,   123,     0,
   123,   122,     0,    87,    33,     0,    71,    87,     9,    87,
    33,     0,    65,    87,    33,     0,    63,    33,     0,    63,
    87,    33,     0,   127,     0,    70,    21,   122,    29,   124,
     0,    21,   122,    29,     0,   125,     0,   125,   124,     0,
    69,    22,    87,     9,    17,    30,    21,   122,    29,     0,
    11,    22,    17,     9,    17,    30,    21,   122,    29,     0,
    10,    17,    47,    90,     0,   129,   130,     0,    54,    22,
    87,    33,    87,    33,    87,    30,   130,     0,    55,   130,
   129,    33,     0,    52,    22,    87,    30,   130,   128,     0,
    59,    22,    17,     9,    87,    30,   130,     0,    59,    22,
    17,     9,    17,     9,    87,    30,   130,     0,     0,    53,
    52,    22,    87,    30,   130,   128,     0,    53,   130,     0,
    51,    22,    87,    30,     0,    21,   121,    29,     0
};

#endif

#if YYDEBUG != 0
static const short yyrline[] = { 0,
   168,   172,   176,   183,   187,   196,   200,   204,   208,   212,
   217,   223,   229,   234,   241,   245,   254,   264,   272,   276,
   285,   289,   297,   302,   314,   318,   322,   329,   333,   340,
   344,   351,   355,   359,   363,   367,   371,   375,   383,   387,
   395,   399,   407,   411,   419,   423,   427,   435,   439,   443,
   451,   455,   459,   463,   471,   475,   479,   483,   487,   494,
   498,   502,   506,   510,   517,   522,   529,   534,   541,   545,
   549,   554,   559,   566,   571,   576,   584,   588,   592,   601,
   606,   611,   616,   621,   626,   631,   636,   641,   645,   649,
   653,   660,   664,   671,   675,   685,   693,   697,   705,   709,
   713,   720,   725,   734,   742,   747,   757,   761,   768,   772,
   780,   784,   788,   793,   798,   807,   814,   820,   828,   832,
   839,   844,   849,   854,   862,   866,   873,   878,   886,   890,
   897,   901,   909,   913,   917,   921,   925,   929,   933,   937,
   944,   948,   956,   962,   973,   981,   985,   989,   993,   997,
  1002,  1012,  1016,  1020,  1027,  1034
};
#endif


#if YYDEBUG != 0 || defined (YYERROR_VERBOSE)

static const char * const yytname[] = {   "$","error","$undefined.","T_AMPERSAND",
"T_ASTERISK","T_CASE","T_CHARACTER_LITERAL","T_CIRCUMFLEX","T_COLON","T_COMMA",
"T_CONST","T_DEFAULT","T_ENUM","T_EQUAL","T_FALSE","T_FLOATING_PT_LITERAL","T_GREATER_THAN_SIGN",
"T_IDENTIFIER","T_IN","T_INOUT","T_INTEGER_LITERAL","T_LEFT_CURLY_BRACKET","T_LEFT_PARANTHESIS",
"T_LEFT_SQUARE_BRACKET","T_LESS_THAN_SIGN","T_MINUS_SIGN","T_OUT","T_PERCENT_SIGN",
"T_PLUS_SIGN","T_RIGHT_CURLY_BRACKET","T_RIGHT_PARANTHESIS","T_RIGHT_SQUARE_BRACKET",
"T_SCOPE","T_SEMICOLON","T_SHIFTLEFT","T_SHIFTRIGHT","T_SOLIDUS","T_STRING_LITERAL",
"T_PRAGMA","T_STRUCT","T_SWITCH","T_TILDE","T_TRUE","T_VERTICAL_LINE","T_CLASS",
"T_LESS_OR_EQUAL","T_GREATER_OR_EQUAL","T_ASSIGN","T_NOTEQUAL","T_MEMBER","T_DELETE",
"T_WHILE","T_IF","T_ELSE","T_FOR","T_DO","T_INCR","T_DECR","T_MAIN","T_FOREACH",
"T_SUBST","T_MATCH","T_NOT","T_RETURN","T_SIGNAL","T_EMIT","T_IMPORT","T_VAR",
"T_UNKNOWN","T_CATCH","T_TRY","T_RAISE","T_RANGE","T_CELL","T_FROM","T_PLUS_ASSIGN",
"T_AND","T_OR","specification","definitions","definition","import_list","main",
"inheritance_spec","qualified_names","qualified_name","scoped_name","assign_expr",
"bool_or","bool_and","equal_expr","or_expr","xor_expr","and_expr","shift_expr",
"add_expr","mult_expr","unary_expr","index_expr","func_call_params","member_expr",
"incr_expr","match_expr","primary_expr","literal","array_elements","dict_elements",
"struct_dcl","struct_exports","struct_export","struct_members","class_dcl","class_header",
"class_body","class_exports","class_export","func_dcl","func_params","func_param",
"signal_params","signal_param","func_body","func_lines","func_line","catches",
"single_catch","const_dcl","loops","else","while","loop_body", NULL
};
#endif

static const short yyr1[] = {     0,
    78,    78,    78,    79,    79,    80,    80,    80,    80,    80,
    80,    80,    80,    80,    81,    81,    82,    83,    84,    84,
    85,    85,    86,    86,    87,    87,    87,    88,    88,    89,
    89,    90,    90,    90,    90,    90,    90,    90,    91,    91,
    92,    92,    93,    93,    94,    94,    94,    95,    95,    95,
    96,    96,    96,    96,    97,    97,    97,    97,    97,    98,
    98,    98,    98,    98,    99,    99,   100,   100,   101,   101,
   101,   101,   101,   102,   102,   102,   103,   103,   103,   104,
   104,   104,   104,   104,   104,   104,   104,   104,   104,   104,
   104,   105,   105,   106,   106,   107,   108,   108,   109,   109,
   109,   110,   110,   111,   112,   112,   113,   113,   114,   114,
   115,   115,   115,   115,   115,   115,   116,   116,   117,   117,
   118,   118,   118,   118,   119,   119,   120,   120,   121,   121,
   122,   122,   123,   123,   123,   123,   123,   123,   123,   123,
   124,   124,   125,   125,   126,   127,   127,   127,   127,   127,
   127,   128,   128,   128,   129,   130
};

static const short yyr2[] = {     0,
     0,     2,     1,     1,     2,     2,     2,     2,     2,     3,
     5,     5,     1,     1,     1,     3,     4,     2,     1,     3,
     1,     3,     1,     2,     3,     3,     1,     3,     1,     3,
     1,     3,     3,     3,     3,     3,     3,     1,     1,     3,
     1,     3,     1,     3,     1,     3,     3,     1,     3,     3,
     1,     3,     3,     3,     2,     2,     2,     2,     1,     4,
     4,     3,     4,     1,     3,     1,     3,     1,     2,     2,
     2,     2,     1,     2,     2,     1,     1,     1,     3,     1,
     1,     1,     1,     1,     1,     1,     1,     2,     3,     2,
     3,     3,     1,     7,     5,     5,     1,     2,     2,     3,
     2,     1,     3,     4,     2,     3,     0,     1,     1,     2,
     2,     2,     6,     5,     8,     2,     5,     6,     1,     3,
     2,     4,     2,     2,     1,     3,     2,     4,     0,     1,
     1,     2,     2,     5,     3,     2,     3,     1,     5,     3,
     1,     2,     9,     9,     4,     2,     9,     4,     6,     7,
     9,     0,     7,     2,     4,     3
};

static const short yydefact[] = {     1,
     0,     0,    13,     0,     0,     0,     0,     0,     0,     3,
     4,    14,     0,     0,     0,     0,     0,     0,     0,     0,
   105,    84,    87,    85,    23,    80,     0,     0,     0,     0,
     0,     0,    83,     0,    86,     0,     0,     0,    82,    81,
    77,     2,    38,    39,    41,    43,    45,    48,    51,    59,
    64,    68,    73,    76,    78,   129,     0,     0,     5,     7,
     8,   107,     9,     6,     0,     0,     0,     0,     0,     0,
   119,     0,     0,   106,     0,    90,     0,     0,    27,    29,
    31,    88,    93,     0,    55,    56,    24,    57,    69,    70,
    58,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,    71,    72,    75,    74,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,   130,   131,   138,
     0,    10,     0,     0,     0,     0,     0,   108,   109,     0,
     0,   145,   121,   124,   123,   129,     0,     0,     0,     0,
    97,     0,     0,    18,    19,    21,     0,    91,    79,     0,
     0,     0,     0,     0,    89,    32,    37,    36,    40,    34,
    35,    33,    42,    44,    47,    46,    50,    49,    52,    54,
    53,     0,    62,    66,     0,     0,    67,     0,     0,     0,
     0,     0,   129,     0,     0,   136,     0,     0,     0,     0,
   133,    17,   132,   146,     0,    15,     0,     0,     0,   112,
   104,   110,   111,   116,     0,   117,   129,   120,   102,     0,
    96,    98,    99,   101,     0,     0,     0,    25,    26,    28,
    30,    92,    61,     0,    63,    60,     0,   140,     0,     0,
     0,     0,     0,     0,   137,   135,     0,     0,    11,     0,
    12,     0,     0,   122,   118,     0,   100,    20,    22,     0,
    65,   155,     0,     0,   156,   148,     0,     0,     0,    16,
     0,     0,     0,     0,   125,   103,    95,   152,     0,    23,
     0,     0,     0,   139,   141,   134,     0,   127,   114,     0,
     0,     0,     0,   149,     0,     0,     0,     0,     0,   142,
   129,     0,   113,   126,    94,     0,   154,     0,     0,   150,
     0,     0,     0,   128,     0,     0,     0,     0,     0,   115,
     0,   147,   151,     0,     0,     0,     0,     0,   152,     0,
     0,   153,     0,     0,   144,   143,     0,     0,     0
};

static const short yydefgoto[] = {   337,
    10,    11,   207,    12,    74,   154,   155,    41,   126,    79,
    80,    81,    43,    44,    45,    46,    47,    48,    49,    50,
   185,    51,    52,    53,    54,    55,    84,    77,    13,   150,
   151,   220,    14,    15,   137,   138,   139,    16,    70,    71,
   274,   275,   127,   128,   129,   284,   285,    17,   130,   294,
   131,   194
};

static const short yypact[] = {    18,
     3,     5,-32768,     8,    19,   394,    39,    47,    60,-32768,
   100,-32768,    35,    53,    80,    59,    74,    62,   154,    91,
   110,-32768,-32768,-32768,-32768,-32768,    84,   394,    17,   464,
   464,   107,-32768,   464,-32768,   105,   105,   464,-32768,-32768,
-32768,-32768,    -2,   127,   158,   106,    44,    51,-32768,   130,
-32768,-32768,    89,    88,-32768,   258,   134,   115,-32768,-32768,
-32768,   104,-32768,-32768,   394,   168,   174,   175,   172,   164,
   186,     0,     1,-32768,   394,-32768,   170,   167,   -23,   123,
   125,-32768,   193,   177,   130,   130,-32768,   130,-32768,-32768,
   130,   394,   394,   394,   394,   394,   394,   394,   394,   394,
   394,   394,   394,   394,   394,   394,   394,   394,   318,   394,
   192,-32768,-32768,-32768,-32768,   190,   181,   199,   201,   203,
   204,   356,   394,   207,   394,   196,   185,-32768,   258,-32768,
   203,-32768,    46,   208,   217,   202,   209,-32768,   104,   211,
   210,-32768,   178,-32768,-32768,   258,   216,    97,   231,   221,
     0,   222,   223,-32768,     4,-32768,   245,-32768,-32768,   394,
   394,   394,   394,   394,-32768,-32768,-32768,-32768,   127,-32768,
-32768,-32768,   158,   106,    44,    44,    51,    51,-32768,-32768,
-32768,   228,-32768,   249,   229,   234,-32768,   394,   237,   394,
   394,   394,   258,   218,   253,-32768,   243,   244,   258,   273,
-32768,-32768,-32768,-32768,   251,   276,   255,   271,   269,-32768,
-32768,-32768,-32768,-32768,   394,-32768,   258,-32768,   283,   260,
-32768,-32768,-32768,-32768,     1,   277,   394,-32768,-32768,-32768,
-32768,-32768,-32768,   394,-32768,-32768,    61,-32768,   266,   267,
   265,   272,   270,   293,-32768,-32768,   275,   394,-32768,   288,
-32768,   289,    -9,-32768,-32768,   231,-32768,-32768,-32768,   278,
-32768,-32768,   203,   394,-32768,-32768,   432,    -3,   274,-32768,
   281,   299,   285,   292,   310,-32768,   316,   284,   294,   317,
   304,   314,   320,-32768,    -3,-32768,   323,   298,-32768,   319,
    97,   325,     9,-32768,   394,   394,   203,   332,   394,-32768,
   258,   394,-32768,-32768,-32768,   329,-32768,   324,   326,-32768,
   344,   348,   334,-32768,   394,   203,   203,   341,   347,-32768,
   331,-32768,-32768,   335,   336,   203,   346,   351,   284,   258,
   258,-32768,   339,   340,-32768,-32768,   382,   383,-32768
};

static const short yypgoto[] = {-32768,
   374,-32768,   136,-32768,-32768,   162,-32768,   -68,   -28,   230,
   232,     6,-32768,   301,   295,   302,    54,    67,    24,   152,
   165,-32768,-32768,   139,-32768,-32768,   239,   109,   -56,   254,
-32768,   148,-32768,-32768,-32768,   268,-32768,   -46,  -141,-32768,
-32768,-32768,  -142,  -114,-32768,   121,-32768,   -43,-32768,    81,
   226,  -128
};


#define	YYLAST		537


static const short yytable[] = {    78,
    83,   189,   204,   216,   156,   136,   218,   282,   272,     1,
    92,    42,   225,    93,   203,   140,     2,    25,   141,    18,
   273,    94,    22,   160,    20,   152,    19,     1,   153,   193,
    23,    24,    32,    25,     2,    21,    26,    27,    28,    29,
    95,    30,    96,    97,    31,    98,   157,    82,    32,   205,
   242,   161,   226,    33,   105,     3,     4,    34,    35,    56,
   306,     5,   206,    57,     6,   283,   149,    60,   103,   227,
   142,   104,    36,    37,   255,     7,    58,   106,    38,   182,
   184,   186,   136,     8,   247,    61,   107,    63,    39,    40,
   159,     9,   140,   197,   198,   141,   200,   166,   167,   168,
    62,   170,   171,   172,   152,    75,    64,   153,    65,     1,
    22,    72,    76,     1,    66,    67,     2,    73,    23,    24,
     2,    25,    68,    87,    26,    27,    28,    29,   179,   180,
   181,   228,   229,    99,   278,    83,    32,     3,     4,   101,
   102,    33,     4,     5,   112,   113,    35,   114,   115,   304,
   108,   109,   110,   134,   175,   176,   156,     7,   313,   237,
   100,   239,   240,   241,   307,     8,   132,   135,   310,   177,
   178,    66,    67,     9,    89,    90,    39,    40,   111,    68,
   133,    85,    86,    69,   143,    88,   254,   322,   323,    91,
   144,   145,   146,   147,   148,    22,   159,   329,   158,   162,
   163,   164,   190,    23,    24,   184,    25,   165,   187,    26,
   116,   188,    29,   202,    30,   333,   334,    31,    76,   269,
   191,    32,   192,   193,   215,   195,    33,   199,   201,   208,
    34,    35,   260,   209,   210,   279,   217,   211,   281,   213,
   117,   118,   214,   119,   120,    36,    37,   219,   121,   221,
   223,    38,   122,   227,   123,   224,   233,   234,   235,   124,
   125,    39,    40,    22,   236,   238,   308,   309,   117,   244,
   312,    23,    24,   314,    25,   245,   246,    26,   116,    28,
    29,   248,    30,   249,   250,    31,   321,   251,   252,    32,
   253,   256,   257,   259,    33,   262,   263,   264,    34,    35,
   265,   267,   266,   268,   206,   271,   286,   277,   117,   118,
   287,   119,   120,    36,    37,   288,   121,   289,   291,    38,
   122,   290,   123,    22,   292,   296,   295,   124,   125,    39,
    40,    23,    24,   297,    25,   298,   293,    26,    27,    28,
    29,   299,    30,   301,   302,    31,    75,   183,   311,    32,
   315,   303,   318,   316,    33,   317,   319,   324,    34,    35,
   326,    22,   320,   325,   327,   328,   330,   335,   336,    23,
    24,   331,    25,    36,    37,    26,    27,    28,    29,    38,
    30,   338,   339,    31,    59,   270,   258,    32,   196,    39,
    40,   230,    33,   173,   231,   169,    34,    35,   261,    22,
   305,   174,   232,   276,   222,   300,   212,    23,    24,   332,
    25,    36,    37,    26,    27,    28,    29,    38,    30,   243,
     0,    31,     0,     0,     0,    32,     0,    39,    40,     0,
    33,     0,     0,     0,    34,    35,     0,    22,     0,     0,
     0,     0,     0,     0,     0,    23,    24,     0,   280,    36,
    37,    26,    27,    28,    29,    38,    30,     0,     0,    31,
     0,     0,     0,    32,     0,    39,    40,     0,    33,    22,
     0,     0,    34,    35,     0,     0,     0,    23,    24,     0,
    25,     0,     0,    26,    27,    28,    29,    36,    37,     0,
     0,     0,     0,    38,     0,    32,     0,     0,     0,     0,
    33,     0,     0,    39,    40,    35,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,    36,
    37,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,    39,    40
};

static const short yycheck[] = {    28,
    29,   116,   131,   146,    73,    62,   148,    11,    18,    10,
    13,     6,     9,    16,   129,    62,    17,    17,    62,    17,
    30,    24,     6,    47,    17,    72,    22,    10,    72,    21,
    14,    15,    32,    17,    17,    17,    20,    21,    22,    23,
    43,    25,    45,    46,    28,    48,    75,    31,    32,     4,
   193,    75,    49,    37,     4,    38,    39,    41,    42,    21,
    52,    44,    17,    17,    47,    69,    67,    33,    25,     9,
    65,    28,    56,    57,   217,    58,    17,    27,    62,   108,
   109,   110,   139,    66,   199,    33,    36,    29,    72,    73,
    30,    74,   139,   122,   123,   139,   125,    92,    93,    94,
    21,    96,    97,    98,   151,    22,    33,   151,    47,    10,
     6,    21,    29,    10,    18,    19,    17,     8,    14,    15,
    17,    17,    26,    17,    20,    21,    22,    23,   105,   106,
   107,   160,   161,     7,   263,   164,    32,    38,    39,    34,
    35,    37,    39,    44,    56,    57,    42,    60,    61,   291,
    21,    22,    23,    50,   101,   102,   225,    58,   301,   188,
     3,   190,   191,   192,   293,    66,    33,    64,   297,   103,
   104,    18,    19,    74,    36,    37,    72,    73,    49,    26,
    66,    30,    31,    30,    17,    34,   215,   316,   317,    38,
    17,    17,    21,    30,     9,     6,    30,   326,    29,    77,
    76,     9,    22,    14,    15,   234,    17,    31,    17,    20,
    21,    22,    23,    29,    25,   330,   331,    28,    29,   248,
    22,    32,    22,    21,    47,    22,    37,    21,    33,    22,
    41,    42,   227,    17,    33,   264,    21,    29,   267,    29,
    51,    52,    33,    54,    55,    56,    57,    17,    59,    29,
    29,    62,    63,     9,    65,    33,    29,     9,    30,    70,
    71,    72,    73,     6,    31,    29,   295,   296,    51,    17,
   299,    14,    15,   302,    17,    33,    33,    20,    21,    22,
    23,     9,    25,    33,     9,    28,   315,    33,    18,    32,
    22,     9,    33,    17,    37,    30,    30,    33,    41,    42,
    29,     9,    33,    29,    17,    17,    33,    30,    51,    52,
    30,    54,    55,    56,    57,    17,    59,    33,     9,    62,
    63,    30,    65,     6,     9,     9,    33,    70,    71,    72,
    73,    14,    15,    30,    17,    22,    53,    20,    21,    22,
    23,    22,    25,    21,    47,    28,    22,    30,    17,    32,
    22,    33,     9,    30,    37,    30,     9,    17,    41,    42,
    30,     6,    29,    17,    30,    30,    21,    29,    29,    14,
    15,    21,    17,    56,    57,    20,    21,    22,    23,    62,
    25,     0,     0,    28,    11,   250,   225,    32,    33,    72,
    73,   162,    37,    99,   163,    95,    41,    42,   234,     6,
   292,   100,   164,   256,   151,   285,   139,    14,    15,   329,
    17,    56,    57,    20,    21,    22,    23,    62,    25,   194,
    -1,    28,    -1,    -1,    -1,    32,    -1,    72,    73,    -1,
    37,    -1,    -1,    -1,    41,    42,    -1,     6,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    14,    15,    -1,    17,    56,
    57,    20,    21,    22,    23,    62,    25,    -1,    -1,    28,
    -1,    -1,    -1,    32,    -1,    72,    73,    -1,    37,     6,
    -1,    -1,    41,    42,    -1,    -1,    -1,    14,    15,    -1,
    17,    -1,    -1,    20,    21,    22,    23,    56,    57,    -1,
    -1,    -1,    -1,    62,    -1,    32,    -1,    -1,    -1,    -1,
    37,    -1,    -1,    72,    73,    42,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    56,
    57,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    72,    73
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
#line 169 "yacc.yy"
{
	    theParser->setRootNode( NULL );
	  ;
    break;}
case 2:
#line 173 "yacc.yy"
{
	    theParser->setRootNode( yyvsp[0].node );
	  ;
    break;}
case 3:
#line 177 "yacc.yy"
{
	    theParser->setRootNode( new KSParseNode( definitions, yyvsp[0].node ) );
	  ;
    break;}
case 4:
#line 184 "yacc.yy"
{
	    yyval.node = new KSParseNode( definitions, yyvsp[0].node );
	  ;
    break;}
case 5:
#line 188 "yacc.yy"
{
	    yyval.node = new KSParseNode( definitions, yyvsp[-1].node );
	    yyval.node->setBranch( 2, yyvsp[0].node );
	  ;
    break;}
case 6:
#line 197 "yacc.yy"
{
	    yyval.node = yyvsp[-1].node;
	  ;
    break;}
case 7:
#line 201 "yacc.yy"
{
	    yyval.node = yyvsp[-1].node;
	  ;
    break;}
case 8:
#line 205 "yacc.yy"
{
	    yyval.node = yyvsp[-1].node;
	  ;
    break;}
case 9:
#line 209 "yacc.yy"
{
	    yyval.node = yyvsp[-1].node;
	  ;
    break;}
case 10:
#line 213 "yacc.yy"
{
	    yyval.node = new KSParseNode( import );
	    yyval.node->setIdent( yyvsp[-1].ident );
	  ;
    break;}
case 11:
#line 218 "yacc.yy"
{
	    yyval.node = new KSParseNode( from );
	    yyval.node->setIdent( yyvsp[-3].ident );
	    yyval.node->setStringLiteral( QString( "" ) );
	  ;
    break;}
case 12:
#line 224 "yacc.yy"
{
	    yyval.node = new KSParseNode( from );
	    yyval.node->setIdent( yyvsp[-3].ident );
	    yyval.node->setStringLiteral( yyvsp[-1]._str );
	  ;
    break;}
case 13:
#line 230 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_pragma );
	    yyval.node->setIdent( yyvsp[0]._str );
	  ;
    break;}
case 14:
#line 235 "yacc.yy"
{
	    yyval.node = yyvsp[0].node;
	  ;
    break;}
case 15:
#line 242 "yacc.yy"
{
		yyval._str = yyvsp[0].ident;
	  ;
    break;}
case 16:
#line 246 "yacc.yy"
{
		(*yyvsp[-2].ident) += "/";
		(*yyvsp[-2].ident) += (*yyvsp[0]._str);
		yyval._str = yyvsp[-2].ident;
	  ;
    break;}
case 17:
#line 255 "yacc.yy"
{
	    yyval.node = new KSParseNode( func_dcl );
	    yyval.node->setBranch( 2, yyvsp[-1].node );
	    yyval.node->setIdent( "main" );
	  ;
    break;}
case 18:
#line 265 "yacc.yy"
{
	    yyval.node = yyvsp[0].node;
	  ;
    break;}
case 19:
#line 273 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_qualified_names, yyvsp[0].node );
	  ;
    break;}
case 20:
#line 277 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_qualified_names, yyvsp[-2].node );
	    yyval.node->setBranch( 2, yyvsp[0].node );
	  ;
    break;}
case 21:
#line 286 "yacc.yy"
{
	    yyval.node = yyvsp[0].node;
	  ;
    break;}
case 22:
#line 290 "yacc.yy"
{
	    yyval.node = new KSParseNode( member_expr, yyvsp[-2].node );
	    yyval.node->setIdent( yyvsp[0].ident );
	  ;
    break;}
case 23:
#line 298 "yacc.yy"
{
	    yyval.node = new KSParseNode( scoped_name );
	    yyval.node->setIdent( yyvsp[0].ident );
	  ;
    break;}
case 24:
#line 303 "yacc.yy"
{
	    yyval.node = new KSParseNode( scoped_name );
	    QString name = "::";
	    name += *(yyvsp[0].ident);
	    delete yyvsp[0].ident;
	    yyval.node->setIdent( name );
	  ;
    break;}
case 25:
#line 315 "yacc.yy"
{
	    yyval.node = new KSParseNode( assign_expr, yyvsp[-2].node, yyvsp[0].node );
	  ;
    break;}
case 26:
#line 319 "yacc.yy"
{
	    yyval.node = new KSParseNode( plus_assign, yyvsp[-2].node, yyvsp[0].node );
	  ;
    break;}
case 27:
#line 323 "yacc.yy"
{
	    yyval.node = yyvsp[0].node;
	  ;
    break;}
case 28:
#line 330 "yacc.yy"
{
	    yyval.node = new KSParseNode( bool_or, yyvsp[-2].node, yyvsp[0].node );
	  ;
    break;}
case 29:
#line 334 "yacc.yy"
{
	    yyval.node = yyvsp[0].node;
	  ;
    break;}
case 30:
#line 341 "yacc.yy"
{
	    yyval.node = new KSParseNode( bool_and, yyvsp[-2].node, yyvsp[0].node );
	  ;
    break;}
case 31:
#line 345 "yacc.yy"
{
	    yyval.node = yyvsp[0].node;
	  ;
    break;}
case 32:
#line 352 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_equal, yyvsp[-2].node, yyvsp[0].node );
	  ;
    break;}
case 33:
#line 356 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_notequal, yyvsp[-2].node, yyvsp[0].node );
	  ;
    break;}
case 34:
#line 360 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_less_or_equal, yyvsp[-2].node, yyvsp[0].node );
	  ;
    break;}
case 35:
#line 364 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_greater_or_equal, yyvsp[-2].node, yyvsp[0].node );
	  ;
    break;}
case 36:
#line 368 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_less, yyvsp[-2].node, yyvsp[0].node );
	  ;
    break;}
case 37:
#line 372 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_greater, yyvsp[-2].node, yyvsp[0].node );
	  ;
    break;}
case 38:
#line 376 "yacc.yy"
{
	    yyval.node = yyvsp[0].node;
	  ;
    break;}
case 39:
#line 384 "yacc.yy"
{
	    yyval.node = yyvsp[0].node;
	  ;
    break;}
case 40:
#line 388 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_vertical_line, yyvsp[-2].node, yyvsp[0].node );
	  ;
    break;}
case 41:
#line 396 "yacc.yy"
{
	    yyval.node = yyvsp[0].node;
	  ;
    break;}
case 42:
#line 400 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_circumflex, yyvsp[-2].node, yyvsp[0].node );
	  ;
    break;}
case 43:
#line 408 "yacc.yy"
{
	    yyval.node = yyvsp[0].node;
	  ;
    break;}
case 44:
#line 412 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_ampersand, yyvsp[-2].node, yyvsp[0].node );
	  ;
    break;}
case 45:
#line 420 "yacc.yy"
{
	    yyval.node = yyvsp[0].node;
	  ;
    break;}
case 46:
#line 424 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_shiftright, yyvsp[-2].node, yyvsp[0].node );
	  ;
    break;}
case 47:
#line 428 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_shiftleft, yyvsp[-2].node, yyvsp[0].node );
	  ;
    break;}
case 48:
#line 436 "yacc.yy"
{
	    yyval.node = yyvsp[0].node;
	  ;
    break;}
case 49:
#line 440 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_plus_sign, yyvsp[-2].node, yyvsp[0].node );
	  ;
    break;}
case 50:
#line 444 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_minus_sign, yyvsp[-2].node, yyvsp[0].node );
	  ;
    break;}
case 51:
#line 452 "yacc.yy"
{
	    yyval.node = yyvsp[0].node;
	  ;
    break;}
case 52:
#line 456 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_asterik, yyvsp[-2].node, yyvsp[0].node );
	  ;
    break;}
case 53:
#line 460 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_solidus, yyvsp[-2].node, yyvsp[0].node );
	  ;
    break;}
case 54:
#line 464 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_percent_sign, yyvsp[-2].node, yyvsp[0].node );
	  ;
    break;}
case 55:
#line 472 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_minus_sign, yyvsp[0].node );
	  ;
    break;}
case 56:
#line 476 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_plus_sign, yyvsp[0].node );
	  ;
    break;}
case 57:
#line 480 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_tilde, yyvsp[0].node );
	  ;
    break;}
case 58:
#line 484 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_not, yyvsp[0].node );
	  ;
    break;}
case 59:
#line 488 "yacc.yy"
{
	    yyval.node = yyvsp[0].node;
	  ;
    break;}
case 60:
#line 495 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_array, yyvsp[-3].node, yyvsp[-1].node );
	  ;
    break;}
case 61:
#line 499 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_dict, yyvsp[-3].node, yyvsp[-1].node );
	  ;
    break;}
case 62:
#line 503 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_func_call, yyvsp[-2].node );
	  ;
    break;}
case 63:
#line 507 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_func_call, yyvsp[-3].node, yyvsp[-1].node );
	  ;
    break;}
case 64:
#line 511 "yacc.yy"
{
	    yyval.node = yyvsp[0].node;
	  ;
    break;}
case 65:
#line 518 "yacc.yy"
{
	    yyval.node = new KSParseNode( func_call_params, yyvsp[-2].node, yyvsp[0].node );
	  ;
    break;}
case 66:
#line 523 "yacc.yy"
{
	    yyval.node = new KSParseNode( func_call_params, yyvsp[0].node );
	  ;
    break;}
case 67:
#line 530 "yacc.yy"
{
	    yyval.node = new KSParseNode( member_expr, yyvsp[-2].node );
	    yyval.node->setIdent( yyvsp[0].ident );
	  ;
    break;}
case 68:
#line 535 "yacc.yy"
{
	    yyval.node = yyvsp[0].node;
	  ;
    break;}
case 69:
#line 542 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_incr, yyvsp[0].node );
	  ;
    break;}
case 70:
#line 546 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_decr, yyvsp[0].node );
	  ;
    break;}
case 71:
#line 550 "yacc.yy"
{
	    /* Setting $1 twice indicates that this is a postfix operator */
	    yyval.node = new KSParseNode( t_incr, yyvsp[-1].node, yyvsp[-1].node );
	  ;
    break;}
case 72:
#line 555 "yacc.yy"
{
	    /* Setting $1 twice indicates that this is a postfix operator */
	    yyval.node = new KSParseNode( t_decr, yyvsp[-1].node, yyvsp[-1].node );
	  ;
    break;}
case 73:
#line 560 "yacc.yy"
{
	    yyval.node = yyvsp[0].node;
	  ;
    break;}
case 74:
#line 567 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_match, yyvsp[-1].node );
	    yyval.node->setIdent( yyvsp[0].ident );
	  ;
    break;}
case 75:
#line 572 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_subst, yyvsp[-1].node );
	    yyval.node->setIdent( yyvsp[0].ident );
	  ;
    break;}
case 76:
#line 577 "yacc.yy"
{
	    yyval.node = yyvsp[0].node;
	  ;
    break;}
case 77:
#line 585 "yacc.yy"
{
	    yyval.node = yyvsp[0].node;
	  ;
    break;}
case 78:
#line 589 "yacc.yy"
{
	    yyval.node = yyvsp[0].node;
	  ;
    break;}
case 79:
#line 593 "yacc.yy"
{
	    yyval.node = yyvsp[-1].node;
	  ;
    break;}
case 80:
#line 602 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_integer_literal );
	    yyval.node->setIntegerLiteral( yyvsp[0]._int );
	  ;
    break;}
case 81:
#line 607 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_cell );
	    yyval.node->setStringLiteral( yyvsp[0]._str );
	  ;
    break;}
case 82:
#line 612 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_range );
	    yyval.node->setStringLiteral( yyvsp[0]._str );
	  ;
    break;}
case 83:
#line 617 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_string_literal );
	    yyval.node->setStringLiteral( yyvsp[0]._str );
	  ;
    break;}
case 84:
#line 622 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_character_literal );
	    yyval.node->setCharacterLiteral( yyvsp[0]._char );
	  ;
    break;}
case 85:
#line 627 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_floating_pt_literal );
	    yyval.node->setFloatingPtLiteral( yyvsp[0]._float );
	  ;
    break;}
case 86:
#line 632 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_boolean_literal );
	    yyval.node->setBooleanLiteral( true );
	  ;
    break;}
case 87:
#line 637 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_boolean_literal );
	    yyval.node->setBooleanLiteral( false );
	  ;
    break;}
case 88:
#line 642 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_array_const );
	  ;
    break;}
case 89:
#line 646 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_array_const, yyvsp[-1].node );
	  ;
    break;}
case 90:
#line 650 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_dict_const );
	  ;
    break;}
case 91:
#line 654 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_dict_const, yyvsp[-1].node );
	  ;
    break;}
case 92:
#line 661 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_array_element, yyvsp[-2].node, yyvsp[0].node );
	  ;
    break;}
case 93:
#line 665 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_array_element, yyvsp[0].node  );
	  ;
    break;}
case 94:
#line 672 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_dict_element, yyvsp[-5].node, yyvsp[-3].node, yyvsp[0].node );
	  ;
    break;}
case 95:
#line 676 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_dict_element, yyvsp[-3].node, yyvsp[-1].node );
	  ;
    break;}
case 96:
#line 686 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_struct, yyvsp[-1].node );
	    yyval.node->setIdent( yyvsp[-3].ident );
	  ;
    break;}
case 97:
#line 694 "yacc.yy"
{
	    yyval.node = new KSParseNode( exports, yyvsp[0].node );
	  ;
    break;}
case 98:
#line 698 "yacc.yy"
{
	    yyval.node = new KSParseNode( exports, yyvsp[-1].node );
	    yyval.node->setBranch( 2, yyvsp[0].node );
	  ;
    break;}
case 99:
#line 706 "yacc.yy"
{
	    yyval.node = yyvsp[-1].node;
	  ;
    break;}
case 100:
#line 710 "yacc.yy"
{
	    yyval.node = yyvsp[-1].node;
	  ;
    break;}
case 101:
#line 714 "yacc.yy"
{
	    yyval.node = yyvsp[-1].node;
	  ;
    break;}
case 102:
#line 721 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_struct_members );
	    yyval.node->setIdent( yyvsp[0].ident );
	  ;
    break;}
case 103:
#line 726 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_struct_members, yyvsp[0].node );
	    yyval.node->setIdent( yyvsp[-2].ident );
	  ;
    break;}
case 104:
#line 735 "yacc.yy"
{
	    yyval.node = new KSParseNode( class_dcl, yyvsp[-3].node, yyvsp[-1].node );
	  ;
    break;}
case 105:
#line 743 "yacc.yy"
{
	    yyval.node = new KSParseNode( class_header );
	    yyval.node->setIdent( yyvsp[0].ident );
	  ;
    break;}
case 106:
#line 748 "yacc.yy"
{
	    yyval.node = new KSParseNode( class_header );
	    yyval.node->setIdent( yyvsp[-1].ident );
	    yyval.node->setBranch( 1, yyvsp[0].node );
	  ;
    break;}
case 107:
#line 758 "yacc.yy"
{
	    yyval.node = NULL;
	  ;
    break;}
case 108:
#line 762 "yacc.yy"
{
	    yyval.node = yyvsp[0].node;
	  ;
    break;}
case 109:
#line 769 "yacc.yy"
{
	    yyval.node = new KSParseNode( exports, yyvsp[0].node );
	  ;
    break;}
case 110:
#line 773 "yacc.yy"
{
	    yyval.node = new KSParseNode( exports, yyvsp[-1].node );
	    yyval.node->setBranch( 2, yyvsp[0].node );
	  ;
    break;}
case 111:
#line 781 "yacc.yy"
{
	    yyval.node = yyvsp[-1].node;
	  ;
    break;}
case 112:
#line 785 "yacc.yy"
{
	    yyval.node = yyvsp[-1].node;
	  ;
    break;}
case 113:
#line 789 "yacc.yy"
{
	    yyval.node = new KSParseNode( signal_dcl, yyvsp[-2].node );
	    yyval.node->setIdent( yyvsp[-4].ident );
	  ;
    break;}
case 114:
#line 794 "yacc.yy"
{
	    yyval.node = new KSParseNode( signal_dcl, 0 );
	    yyval.node->setIdent( yyvsp[-3].ident );
	  ;
    break;}
case 115:
#line 799 "yacc.yy"
{
	    yyval.node = new KSParseNode( destructor_dcl );
	    KSParseNode* n = new KSParseNode( func_param_in );
	    n->setIdent( yyvsp[-4].ident );
	    yyval.node->setBranch( 1, n );
	    yyval.node->setBranch( 2, yyvsp[-1].node );
	    yyval.node->setIdent( "delete" );
	  ;
    break;}
case 116:
#line 808 "yacc.yy"
{
	    yyval.node = yyvsp[-1].node;
	  ;
    break;}
case 117:
#line 815 "yacc.yy"
{
	    yyval.node = new KSParseNode( func_dcl );
	    yyval.node->setBranch( 2, yyvsp[0].node );
	    yyval.node->setIdent( yyvsp[-4].ident );
	  ;
    break;}
case 118:
#line 821 "yacc.yy"
{
	    yyval.node = new KSParseNode( func_dcl, yyvsp[-3].node, yyvsp[0].node );
	    yyval.node->setIdent( yyvsp[-5].ident );
	  ;
    break;}
case 119:
#line 829 "yacc.yy"
{
	    yyval.node = new KSParseNode( func_params, yyvsp[0].node );
	  ;
    break;}
case 120:
#line 833 "yacc.yy"
{
	    yyval.node = new KSParseNode( func_params, yyvsp[-2].node, yyvsp[0].node );
	  ;
    break;}
case 121:
#line 840 "yacc.yy"
{
	    yyval.node = new KSParseNode( func_param_in );
	    yyval.node->setIdent( yyvsp[0].ident );
	  ;
    break;}
case 122:
#line 845 "yacc.yy"
{
	    yyval.node = new KSParseNode( func_param_in, yyvsp[0].node );
	    yyval.node->setIdent( yyvsp[-2].ident );
	  ;
    break;}
case 123:
#line 850 "yacc.yy"
{
	    yyval.node = new KSParseNode( func_param_out );
	    yyval.node->setIdent( yyvsp[0].ident );
	  ;
    break;}
case 124:
#line 855 "yacc.yy"
{
	    yyval.node = new KSParseNode( func_param_inout );
	    yyval.node->setIdent( yyvsp[0].ident );
	  ;
    break;}
case 125:
#line 863 "yacc.yy"
{
	    yyval.node = new KSParseNode( signal_params, yyvsp[0].node );
	  ;
    break;}
case 126:
#line 867 "yacc.yy"
{
	    yyval.node = new KSParseNode( signal_params, yyvsp[-2].node, yyvsp[0].node );
	  ;
    break;}
case 127:
#line 874 "yacc.yy"
{
	    yyval.node = new KSParseNode( signal_param );
	    yyval.node->setIdent( yyvsp[0].ident );
	  ;
    break;}
case 128:
#line 879 "yacc.yy"
{
	    yyval.node = new KSParseNode( signal_param, yyvsp[0].node );
	    yyval.node->setIdent( yyvsp[-2].ident );
	  ;
    break;}
case 129:
#line 887 "yacc.yy"
{
	    yyval.node = NULL;
	  ;
    break;}
case 130:
#line 891 "yacc.yy"
{
	    yyval.node = yyvsp[0].node;
	  ;
    break;}
case 131:
#line 898 "yacc.yy"
{
	    yyval.node = new KSParseNode( func_lines, yyvsp[0].node );
	  ;
    break;}
case 132:
#line 902 "yacc.yy"
{
	    yyval.node = new KSParseNode( func_lines, yyvsp[-1].node );
	    yyval.node->setBranch( 2, yyvsp[0].node );
	  ;
    break;}
case 133:
#line 910 "yacc.yy"
{
	    yyval.node = yyvsp[-1].node;
	  ;
    break;}
case 134:
#line 914 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_raise, yyvsp[-3].node, yyvsp[-1].node );
	  ;
    break;}
case 135:
#line 918 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_emit, yyvsp[-1].node );
	  ;
    break;}
case 136:
#line 922 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_return );
	  ;
    break;}
case 137:
#line 926 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_return, yyvsp[-1].node );
	  ;
    break;}
case 138:
#line 930 "yacc.yy"
{
	    yyval.node = yyvsp[0].node;
	  ;
    break;}
case 139:
#line 934 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_try, yyvsp[-2].node, yyvsp[0].node );
	  ;
    break;}
case 140:
#line 938 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_scope, yyvsp[-1].node );
	  ;
    break;}
case 141:
#line 945 "yacc.yy"
{
	    yyval.node = yyvsp[0].node;
	  ;
    break;}
case 142:
#line 949 "yacc.yy"
{
	    yyval.node = yyvsp[-1].node;
	    yyval.node->setBranch( 4, yyvsp[0].node );
	  ;
    break;}
case 143:
#line 958 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_catch, yyvsp[-6].node, yyvsp[-1].node );
	    yyval.node->setIdent( yyvsp[-4].ident );
	  ;
    break;}
case 144:
#line 964 "yacc.yy"
{
	    KSParseNode* x = new KSParseNode( scoped_name );
	    x->setIdent( yyvsp[-6].ident );
	    yyval.node = new KSParseNode( t_catch_default, x, yyvsp[-1].node );
	    yyval.node->setIdent( yyvsp[-4].ident );
	  ;
    break;}
case 145:
#line 974 "yacc.yy"
{
	    yyval.node = new KSParseNode( const_dcl, yyvsp[0].node );
	    yyval.node->setIdent( yyvsp[-2].ident );
	  ;
    break;}
case 146:
#line 982 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_while, yyvsp[-1].node, yyvsp[0].node );
	  ;
    break;}
case 147:
#line 986 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_for, yyvsp[-6].node, yyvsp[-4].node, yyvsp[-2].node, yyvsp[0].node );
	  ;
    break;}
case 148:
#line 990 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_do, yyvsp[-2].node, yyvsp[-1].node );
	  ;
    break;}
case 149:
#line 994 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_if, yyvsp[-3].node, yyvsp[-1].node, yyvsp[0].node );
	  ;
    break;}
case 150:
#line 998 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_foreach, yyvsp[-2].node, yyvsp[0].node );
	    yyval.node->setIdent( yyvsp[-4].ident );
	  ;
    break;}
case 151:
#line 1003 "yacc.yy"
{
	    /* We set $9 twice to indicate thet this is the foreach for maps */
	    yyval.node = new KSParseNode( t_foreach, yyvsp[-2].node, yyvsp[0].node, yyvsp[0].node );
	    yyval.node->setIdent( yyvsp[-6].ident );
	    yyval.node->setStringLiteral( yyvsp[-4].ident );
	  ;
    break;}
case 152:
#line 1013 "yacc.yy"
{
	    yyval.node = NULL;
	  ;
    break;}
case 153:
#line 1017 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_if, yyvsp[-3].node, yyvsp[-1].node, yyvsp[0].node );
	  ;
    break;}
case 154:
#line 1021 "yacc.yy"
{
	    yyval.node = yyvsp[0].node;
	  ;
    break;}
case 155:
#line 1028 "yacc.yy"
{
	    yyval.node = yyvsp[-1].node;
	  ;
    break;}
case 156:
#line 1035 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_scope, yyvsp[-1].node );
	  ;
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
#line 1039 "yacc.yy"


void kscriptParse( const char *_code, int extension )
{
    kscriptInitFlex( _code, extension );
    yyparse();
}
