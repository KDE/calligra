
/*  A Bison parser, made from yacc.yy
    by GNU Bison version 1.28  */

#define YYBISON 1  /* Identify Bison output.  */

#define	T_MATCH_LINE	257
#define	T_LINE	258
#define	T_INPUT	259
#define	T_AMPERSAND	260
#define	T_ASTERISK	261
#define	T_CASE	262
#define	T_CHARACTER_LITERAL	263
#define	T_CIRCUMFLEX	264
#define	T_COLON	265
#define	T_COMMA	266
#define	T_CONST	267
#define	T_DEFAULT	268
#define	T_ENUM	269
#define	T_EQUAL	270
#define	T_FALSE	271
#define	T_FLOATING_PT_LITERAL	272
#define	T_GREATER_THAN_SIGN	273
#define	T_IDENTIFIER	274
#define	T_IN	275
#define	T_INOUT	276
#define	T_INTEGER_LITERAL	277
#define	T_LEFT_CURLY_BRACKET	278
#define	T_LEFT_PARANTHESIS	279
#define	T_LEFT_SQUARE_BRACKET	280
#define	T_LESS_THAN_SIGN	281
#define	T_MINUS_SIGN	282
#define	T_OUT	283
#define	T_PERCENT_SIGN	284
#define	T_PLUS_SIGN	285
#define	T_RIGHT_CURLY_BRACKET	286
#define	T_RIGHT_PARANTHESIS	287
#define	T_RIGHT_SQUARE_BRACKET	288
#define	T_SCOPE	289
#define	T_SEMICOLON	290
#define	T_SHIFTLEFT	291
#define	T_SHIFTRIGHT	292
#define	T_SOLIDUS	293
#define	T_STRING_LITERAL	294
#define	T_STRUCT	295
#define	T_SWITCH	296
#define	T_TILDE	297
#define	T_TRUE	298
#define	T_VERTICAL_LINE	299
#define	T_LESS_OR_EQUAL	300
#define	T_GREATER_OR_EQUAL	301
#define	T_ASSIGN	302
#define	T_NOTEQUAL	303
#define	T_MEMBER	304
#define	T_WHILE	305
#define	T_IF	306
#define	T_ELSE	307
#define	T_FOR	308
#define	T_DO	309
#define	T_INCR	310
#define	T_DECR	311
#define	T_MAIN	312
#define	T_FOREACH	313
#define	T_SUBST	314
#define	T_MATCH	315
#define	T_NOT	316
#define	T_RETURN	317
#define	T_IMPORT	318
#define	T_VAR	319
#define	T_CATCH	320
#define	T_TRY	321
#define	T_RAISE	322
#define	T_RANGE	323
#define	T_CELL	324
#define	T_FROM	325
#define	T_PLUS_ASSIGN	326
#define	T_MINUS_ASSIGN	327
#define	T_AND	328
#define	T_OR	329
#define	T_DOLLAR	330
#define	T_UNKNOWN	331

#line 1 "yacc.yy"


#include "koscript_parsenode.h"
#include "koscript_parser.h"
#include <stdlib.h>

#ifdef __DECCXX
#include <alloca.h>
#endif

#include <qstring.h>

extern int yylex();

extern QString idl_lexFile;
extern int idl_line_no;

void kscriptInitFlex( const char *_code, int extension, KLocale* );
void kscriptInitFlex( int extension, KLocale* );

void yyerror( const char *s )
{
  theParser->parse_error( idl_lexFile.local8Bit(), s, idl_line_no );
}


#line 29 "yacc.yy"
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



#define	YYFINAL		316
#define	YYFLAG		-32768
#define	YYNTBASE	78

#define YYTRANSLATE(x) ((unsigned)(x) <= 331 ? yytranslate[x] : 121)

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
    77
};

#if YYDEBUG != 0
static const short yyprhs[] = {     0,
     0,     1,     4,     6,     8,    11,    13,    16,    19,    23,
    27,    33,    39,    41,    43,    47,    55,    65,    67,    70,
    74,    78,    82,    84,    88,    90,    94,    96,   100,   104,
   108,   112,   116,   120,   122,   124,   128,   130,   134,   136,
   140,   142,   146,   150,   152,   156,   160,   162,   166,   170,
   174,   177,   180,   183,   186,   188,   193,   198,   202,   207,
   209,   213,   215,   219,   221,   224,   227,   230,   233,   235,
   238,   241,   243,   245,   247,   249,   251,   255,   259,   261,
   263,   265,   267,   269,   271,   273,   275,   278,   282,   285,
   289,   291,   294,   298,   300,   308,   314,   322,   324,   327,
   329,   332,   336,   339,   341,   345,   353,   362,   364,   368,
   371,   376,   379,   382,   383,   385,   387,   390,   393,   399,
   402,   406,   408,   414,   418,   420,   423,   433,   443,   448,
   451,   461,   466,   473,   481,   491,   492,   500,   503,   508
};

static const short yyrhs[] = {    -1,
    48,    87,     0,    79,     0,    80,     0,    80,    79,     0,
    36,     0,   116,    36,     0,   104,    36,     0,   108,    32,
    36,     0,    64,    20,    36,     0,    71,    20,    64,     7,
    36,     0,    71,    20,    64,    81,    36,     0,    82,     0,
    20,     0,    20,    12,    81,     0,    58,    36,    24,    36,
   111,    32,    36,     0,    58,    25,   109,    33,    36,    24,
    36,   111,    32,     0,    20,     0,    35,    20,     0,    85,
    48,    84,     0,    85,    72,    84,     0,    85,    73,    84,
     0,    85,     0,    86,    75,    85,     0,    86,     0,    87,
    74,    86,     0,    87,     0,    88,    16,    87,     0,    88,
    49,    87,     0,    88,    46,    87,     0,    88,    47,    87,
     0,    88,    27,    87,     0,    88,    19,    87,     0,    88,
     0,    89,     0,    88,    45,    89,     0,    90,     0,    89,
    10,    90,     0,    91,     0,    90,     6,    91,     0,    92,
     0,    91,    38,    92,     0,    91,    37,    92,     0,    93,
     0,    92,    31,    93,     0,    92,    28,    93,     0,    94,
     0,    93,     7,    94,     0,    93,    39,    94,     0,    93,
    30,    94,     0,    28,    95,     0,    31,    95,     0,    43,
    95,     0,    62,    95,     0,    95,     0,    95,    26,    84,
    34,     0,    95,    24,    84,    32,     0,    95,    25,    33,
     0,    95,    25,    96,    33,     0,    97,     0,    84,    12,
    96,     0,    84,     0,    95,    50,    20,     0,    98,     0,
    56,    99,     0,    57,    99,     0,    99,    56,     0,    99,
    57,     0,    99,     0,   100,    61,     0,   100,    60,     0,
   100,     0,    83,     0,   101,     0,     5,     0,     3,     0,
    27,    88,    19,     0,    25,    84,    33,     0,    23,     0,
    70,     0,    69,     0,    40,     0,     9,     0,    18,     0,
    44,     0,    17,     0,    26,    34,     0,    26,   102,    34,
     0,    24,    32,     0,    24,   103,    32,     0,     4,     0,
    76,    23,     0,    84,    12,   102,     0,    84,     0,    25,
    84,    12,    87,    33,    12,   103,     0,    25,    84,    12,
    87,    33,     0,    41,    20,    36,    24,    36,   105,    32,
     0,   106,     0,   106,   105,     0,    36,     0,   108,    32,
     0,    65,   107,    36,     0,   116,    36,     0,    20,     0,
    20,    12,   107,     0,    20,    25,    33,    36,    24,    36,
   111,     0,    20,    25,   109,    33,    36,    24,    36,   111,
     0,   110,     0,   110,    12,   109,     0,    21,    20,     0,
    21,    20,    48,    84,     0,    29,    20,     0,    22,    20,
     0,     0,   112,     0,   113,     0,   113,   112,     0,    84,
    36,     0,    68,    84,    12,    84,    36,     0,    63,    36,
     0,    63,    84,    36,     0,   117,     0,    67,    24,   112,
    32,   114,     0,    24,   112,    32,     0,   115,     0,   115,
   114,     0,    66,    25,    84,    12,    20,    33,    24,   112,
    32,     0,    14,    25,    20,    12,    20,    33,    24,   112,
    32,     0,    13,    20,    48,    87,     0,   119,   120,     0,
    54,    25,    84,    36,    84,    36,    84,    33,   120,     0,
    55,   120,   119,    36,     0,    52,    25,    84,    33,   120,
   118,     0,    59,    25,    20,    12,    84,    33,   120,     0,
    59,    25,    20,    12,    20,    12,    84,    33,   120,     0,
     0,    53,    52,    25,    84,    33,   120,   118,     0,    53,
   120,     0,    51,    25,    84,    33,     0,    24,   111,    32,
     0
};

#endif

#if YYDEBUG != 0
static const short yyrline[] = { 0,
   162,   166,   170,   177,   184,   208,   212,   216,   220,   224,
   229,   235,   241,   248,   252,   261,   267,   275,   280,   292,
   296,   300,   304,   311,   315,   322,   326,   333,   337,   341,
   345,   349,   353,   357,   365,   369,   377,   381,   389,   393,
   401,   405,   409,   417,   421,   425,   433,   437,   441,   445,
   453,   457,   461,   465,   469,   476,   480,   484,   488,   492,
   499,   504,   511,   516,   523,   527,   531,   536,   541,   548,
   553,   558,   566,   570,   574,   578,   583,   584,   593,   598,
   603,   608,   613,   618,   623,   628,   633,   637,   641,   645,
   649,   653,   661,   665,   672,   676,   686,   695,   702,   725,
   729,   733,   737,   744,   749,   757,   763,   771,   775,   782,
   787,   792,   797,   805,   809,   816,   820,   828,   832,   836,
   840,   844,   848,   852,   859,   863,   871,   877,   888,   896,
   900,   904,   908,   912,   917,   927,   931,   935,   942,   949
};
#endif


#if YYDEBUG != 0 || defined (YYERROR_VERBOSE)

static const char * const yytname[] = {   "$","error","$undefined.","T_MATCH_LINE",
"T_LINE","T_INPUT","T_AMPERSAND","T_ASTERISK","T_CASE","T_CHARACTER_LITERAL",
"T_CIRCUMFLEX","T_COLON","T_COMMA","T_CONST","T_DEFAULT","T_ENUM","T_EQUAL",
"T_FALSE","T_FLOATING_PT_LITERAL","T_GREATER_THAN_SIGN","T_IDENTIFIER","T_IN",
"T_INOUT","T_INTEGER_LITERAL","T_LEFT_CURLY_BRACKET","T_LEFT_PARANTHESIS","T_LEFT_SQUARE_BRACKET",
"T_LESS_THAN_SIGN","T_MINUS_SIGN","T_OUT","T_PERCENT_SIGN","T_PLUS_SIGN","T_RIGHT_CURLY_BRACKET",
"T_RIGHT_PARANTHESIS","T_RIGHT_SQUARE_BRACKET","T_SCOPE","T_SEMICOLON","T_SHIFTLEFT",
"T_SHIFTRIGHT","T_SOLIDUS","T_STRING_LITERAL","T_STRUCT","T_SWITCH","T_TILDE",
"T_TRUE","T_VERTICAL_LINE","T_LESS_OR_EQUAL","T_GREATER_OR_EQUAL","T_ASSIGN",
"T_NOTEQUAL","T_MEMBER","T_WHILE","T_IF","T_ELSE","T_FOR","T_DO","T_INCR","T_DECR",
"T_MAIN","T_FOREACH","T_SUBST","T_MATCH","T_NOT","T_RETURN","T_IMPORT","T_VAR",
"T_CATCH","T_TRY","T_RAISE","T_RANGE","T_CELL","T_FROM","T_PLUS_ASSIGN","T_MINUS_ASSIGN",
"T_AND","T_OR","T_DOLLAR","T_UNKNOWN","specification","definitions","definition",
"import_list","main","scoped_name","assign_expr","bool_or","bool_and","equal_expr",
"or_expr","xor_expr","and_expr","shift_expr","add_expr","mult_expr","unary_expr",
"index_expr","func_call_params","member_expr","incr_expr","match_expr","primary_expr",
"literal","array_elements","dict_elements","struct_dcl","struct_exports","struct_export",
"struct_members","func_dcl","func_params","func_param","func_body","func_lines",
"func_line","catches","single_catch","const_dcl","loops","else","while","loop_body", NULL
};
#endif

static const short yyr1[] = {     0,
    78,    78,    78,    79,    79,    80,    80,    80,    80,    80,
    80,    80,    80,    81,    81,    82,    82,    83,    83,    84,
    84,    84,    84,    85,    85,    86,    86,    87,    87,    87,
    87,    87,    87,    87,    88,    88,    89,    89,    90,    90,
    91,    91,    91,    92,    92,    92,    93,    93,    93,    93,
    94,    94,    94,    94,    94,    95,    95,    95,    95,    95,
    96,    96,    97,    97,    98,    98,    98,    98,    98,    99,
    99,    99,   100,   100,   100,   100,   100,   100,   101,   101,
   101,   101,   101,   101,   101,   101,   101,   101,   101,   101,
   101,   101,   102,   102,   103,   103,   104,   105,   105,   106,
   106,   106,   106,   107,   107,   108,   108,   109,   109,   110,
   110,   110,   110,   111,   111,   112,   112,   113,   113,   113,
   113,   113,   113,   113,   114,   114,   115,   115,   116,   117,
   117,   117,   117,   117,   117,   118,   118,   118,   119,   120
};

static const short yyr2[] = {     0,
     0,     2,     1,     1,     2,     1,     2,     2,     3,     3,
     5,     5,     1,     1,     3,     7,     9,     1,     2,     3,
     3,     3,     1,     3,     1,     3,     1,     3,     3,     3,
     3,     3,     3,     1,     1,     3,     1,     3,     1,     3,
     1,     3,     3,     1,     3,     3,     1,     3,     3,     3,
     2,     2,     2,     2,     1,     4,     4,     3,     4,     1,
     3,     1,     3,     1,     2,     2,     2,     2,     1,     2,
     2,     1,     1,     1,     1,     1,     3,     3,     1,     1,
     1,     1,     1,     1,     1,     1,     2,     3,     2,     3,
     1,     2,     3,     1,     7,     5,     7,     1,     2,     1,
     2,     3,     2,     1,     3,     7,     8,     1,     3,     2,
     4,     2,     2,     0,     1,     1,     2,     2,     5,     2,
     3,     1,     5,     3,     1,     2,     9,     9,     4,     2,
     9,     4,     6,     7,     9,     0,     7,     2,     4,     3
};

static const short yydefact[] = {     1,
     0,     0,     6,     0,     0,     0,     0,     0,     3,     4,
    13,     0,     0,     0,     0,     0,     0,    76,    91,    75,
    83,    86,    84,    18,    79,     0,     0,     0,     0,     0,
     0,     0,    82,     0,    85,     0,     0,     0,    81,    80,
     0,    73,     2,    34,    35,    37,    39,    41,    44,    47,
    55,    60,    64,    69,    72,    74,     0,     0,     0,     0,
     5,     8,     0,     7,     0,     0,     0,     0,     0,     0,
   108,     0,     0,    89,     0,     0,    23,    25,    27,    87,
    94,     0,     0,    51,    52,    19,    53,    65,    66,    54,
    92,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,    67,    68,    71,    70,     0,     0,    10,     0,     9,
   129,   110,   113,   112,     0,     0,     0,     0,     0,    90,
    78,     0,     0,     0,     0,     0,     0,    88,    77,    28,
    33,    32,    36,    30,    31,    29,    38,    40,    43,    42,
    46,    45,    48,    50,    49,     0,    58,    62,     0,     0,
    63,     0,   114,     0,    14,     0,     0,     0,     0,   109,
     0,     0,    20,    21,    22,    24,    26,    93,    57,     0,
    59,    56,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,   115,   116,   122,     0,    11,     0,
    12,   111,   114,     0,   100,     0,     0,    98,     0,     0,
     0,    61,     0,     0,     0,     0,     0,     0,   114,     0,
     0,   120,     0,     0,     0,   118,     0,   117,   130,    15,
   106,   114,   104,     0,    97,    99,   101,   103,    96,   114,
     0,   124,     0,     0,     0,     0,     0,     0,   121,     0,
     0,    16,   107,     0,   102,     0,     0,   139,     0,     0,
   140,   132,     0,     0,     0,   105,    95,    17,   136,     0,
    18,     0,     0,     0,   123,   125,   119,     0,   133,     0,
     0,     0,     0,     0,   126,     0,   138,     0,     0,   134,
     0,     0,     0,     0,     0,     0,     0,     0,   131,   135,
     0,     0,     0,     0,     0,   136,     0,     0,   137,     0,
     0,   128,   127,     0,     0,     0
};

static const short yydefgoto[] = {   314,
     9,    10,   166,    11,    42,   193,    77,    78,    79,    44,
    45,    46,    47,    48,    49,    50,    51,   159,    52,    53,
    54,    55,    56,    82,    75,    12,   207,   208,   234,    13,
    70,    71,   194,   195,   196,   275,   276,    14,   197,   279,
   198,   220
};

static const short yypact[] = {    72,
    30,     1,-32768,    37,   412,    13,    57,    67,-32768,    83,
-32768,   -13,    65,    68,    51,   100,    76,-32768,-32768,-32768,
-32768,-32768,-32768,-32768,-32768,    20,   412,   247,   412,   522,
   522,    94,-32768,   522,-32768,   550,   550,   522,-32768,-32768,
   102,-32768,-32768,   198,   118,   128,   111,   -11,     3,-32768,
    92,-32768,-32768,    81,    90,-32768,    47,   131,   116,    97,
-32768,-32768,   132,-32768,   412,   144,   152,   162,   147,   159,
   173,   170,   412,-32768,   154,   165,     6,   120,   125,-32768,
   188,   167,    -8,    92,    92,-32768,    92,-32768,-32768,    92,
-32768,   412,   412,   412,   412,   412,   412,   412,   412,   412,
   412,   412,   412,   412,   412,   412,   412,   412,   302,   412,
   182,-32768,-32768,-32768,-32768,   178,   177,-32768,    91,-32768,
-32768,   155,-32768,-32768,   194,   183,    47,   190,   215,-32768,
-32768,   412,   412,   412,   412,   412,   412,-32768,-32768,-32768,
-32768,-32768,   118,-32768,-32768,-32768,   128,   111,   -11,   -11,
     3,     3,-32768,-32768,-32768,   174,-32768,   216,   197,   200,
-32768,   195,   153,   196,   223,   201,   412,   202,   217,-32768,
     5,   412,-32768,-32768,-32768,-32768,-32768,-32768,-32768,   412,
-32768,-32768,   218,     4,   214,   221,   224,   231,   233,   357,
   235,   412,   204,   228,-32768,   153,-32768,   231,-32768,   241,
-32768,-32768,   153,   212,-32768,   242,   236,     5,   237,   227,
   243,-32768,   244,   412,   245,   412,   412,   412,   153,   232,
   259,-32768,   248,   153,   273,-32768,   250,-32768,-32768,-32768,
-32768,   153,   276,   253,-32768,-32768,-32768,-32768,   280,   153,
     7,-32768,   260,   261,   262,   263,   264,   284,-32768,   265,
   412,-32768,-32768,   242,-32768,   274,   269,-32768,   231,   412,
-32768,-32768,   467,    -2,   266,-32768,-32768,-32768,   255,   277,
   298,   279,   289,   290,-32768,    -2,-32768,   -18,-32768,   412,
   412,   231,   301,   412,-32768,   293,-32768,   291,   299,-32768,
   319,   322,   412,   231,   231,   316,   318,   306,-32768,-32768,
   307,   308,   231,   320,   323,   255,   153,   153,-32768,   311,
   317,-32768,-32768,   348,   350,-32768
};

static const short yypgoto[] = {-32768,
   341,-32768,   156,-32768,-32768,   -27,   219,   229,    -3,   324,
   257,   256,   267,    58,    63,    39,   101,   189,-32768,-32768,
   138,-32768,-32768,   220,   107,-32768,   160,-32768,   119,  -157,
   -52,-32768,  -117,  -181,-32768,   103,-32768,  -155,-32768,    64,
   166,  -194
};


#define	YYLAST		626


static const short yytable[] = {    76,
    81,    43,   215,   229,   116,   219,    18,    19,    20,   105,
   139,   273,    21,   209,   228,   210,   103,     1,   172,   104,
    22,    23,    62,    24,     2,    16,    25,   184,   214,    28,
    29,    30,   106,   286,    31,    74,    95,    57,    32,   131,
   205,   107,   250,    33,    73,   129,    34,    35,    58,    15,
   209,    74,   210,   132,   185,   186,    17,   187,   188,    36,
    37,   121,   189,   274,   269,    38,   190,    66,    67,   206,
   191,   192,    39,    40,   170,    68,    59,   133,   134,    41,
   156,   158,   160,   287,     1,   231,    60,   290,   140,   141,
   142,     2,   144,   145,   146,     1,    63,   164,    65,   299,
   300,   246,     2,    64,   173,   174,   175,     3,   306,    81,
   165,    72,     4,    86,   253,   108,   109,   110,     3,     5,
    66,    67,   257,     4,    91,   310,   311,    99,    68,     6,
    84,    85,    69,   100,    87,     7,   112,   113,    90,   202,
     6,   111,     8,   153,   154,   155,     7,   101,   102,   114,
   115,   118,   158,     8,   117,    18,    19,    20,   149,   150,
   119,    21,   223,   122,   225,   151,   152,   120,   211,    22,
    23,   123,    24,    88,    89,    25,   184,    27,    28,    29,
    30,   124,   125,    31,   127,   130,   241,    32,   243,   244,
   245,   126,    33,   128,   135,    34,    35,   131,   136,   137,
   138,   161,   167,   185,   186,   179,   187,   188,    36,    37,
   162,   189,   163,    92,    38,   190,    93,   168,   169,   191,
   192,    39,    40,   265,    94,   171,   172,   180,    41,   181,
   183,   199,   270,   182,   200,   272,   201,   203,   216,   226,
   204,   213,    95,    96,    97,   217,    98,   232,   218,    18,
    19,    20,   288,   289,   219,    21,   292,   221,   224,   227,
   165,   233,   238,    22,    23,   298,    24,   235,   237,    25,
    26,    27,    28,    29,    30,   239,   242,    31,   248,   240,
    80,    32,   185,   249,   251,   252,    33,   254,   255,    34,
    35,   256,   258,   259,   261,   263,   264,   260,    73,   262,
   268,   277,    36,    37,    18,    19,    20,   278,    38,   281,
    21,   282,   280,   283,   284,    39,    40,   293,    22,    23,
   291,    24,    41,   294,    25,    26,    27,    28,    29,    30,
   296,   295,    31,   297,   157,   301,    32,   302,   303,   304,
   305,    33,   312,   307,    34,    35,   308,   315,   313,   316,
    61,   143,    83,   176,   147,   230,   178,    36,    37,    18,
    19,    20,   267,    38,   177,    21,   148,   236,   212,   309,
    39,    40,   266,    22,    23,     0,    24,    41,   285,    25,
    26,    27,    28,    29,    30,   247,     0,    31,     0,     0,
     0,    32,   222,     0,     0,     0,    33,     0,     0,    34,
    35,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,    36,    37,    18,    19,    20,     0,    38,     0,
    21,     0,     0,     0,     0,    39,    40,     0,    22,    23,
     0,    24,    41,     0,    25,    26,    27,    28,    29,    30,
     0,     0,    31,     0,     0,     0,    32,     0,     0,     0,
     0,    33,     0,     0,    34,    35,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,    36,    37,    18,
    19,    20,     0,    38,     0,    21,     0,     0,     0,     0,
    39,    40,     0,    22,    23,     0,   271,    41,     0,    25,
    26,    27,    28,    29,    30,     0,     0,    31,     0,     0,
     0,    32,     0,     0,     0,     0,    33,     0,     0,    34,
    35,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,    36,    37,    18,    19,    20,     0,    38,     0,
    21,     0,     0,     0,     0,    39,    40,     0,    22,    23,
     0,    24,    41,     0,    25,    26,    27,    28,    29,     0,
     0,     0,    18,    19,    20,     0,    32,     0,    21,     0,
     0,    33,     0,     0,     0,    35,    22,    23,     0,    24,
     0,     0,    25,    26,    27,    28,    29,    36,    37,     0,
     0,     0,     0,     0,    32,     0,     0,     0,     0,    33,
    39,    40,     0,    35,     0,     0,     0,    41,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,    39,    40,
     0,     0,     0,     0,     0,    41
};

static const short yycheck[] = {    27,
    28,     5,   184,   198,    57,    24,     3,     4,     5,     7,
    19,    14,     9,   171,   196,   171,    28,    13,    12,    31,
    17,    18,    36,    20,    20,    25,    23,    24,    25,    26,
    27,    28,    30,    52,    31,    32,    45,    25,    35,    33,
    36,    39,   224,    40,    25,    73,    43,    44,    36,    20,
   208,    32,   208,    48,    51,    52,    20,    54,    55,    56,
    57,    65,    59,    66,   259,    62,    63,    21,    22,    65,
    67,    68,    69,    70,   127,    29,    20,    72,    73,    76,
   108,   109,   110,   278,    13,   203,    20,   282,    92,    93,
    94,    20,    96,    97,    98,    13,    32,     7,    48,   294,
   295,   219,    20,    36,   132,   133,   134,    36,   303,   137,
    20,    36,    41,    20,   232,    24,    25,    26,    36,    48,
    21,    22,   240,    41,    23,   307,   308,    10,    29,    58,
    30,    31,    33,     6,    34,    64,    56,    57,    38,   167,
    58,    50,    71,   105,   106,   107,    64,    37,    38,    60,
    61,    36,   180,    71,    24,     3,     4,     5,   101,   102,
    64,     9,   190,    20,   192,   103,   104,    36,   172,    17,
    18,    20,    20,    36,    37,    23,    24,    25,    26,    27,
    28,    20,    36,    31,    12,    32,   214,    35,   216,   217,
   218,    33,    40,    24,    75,    43,    44,    33,    74,    12,
    34,    20,    48,    51,    52,    32,    54,    55,    56,    57,
    33,    59,    36,    16,    62,    63,    19,    24,    36,    67,
    68,    69,    70,   251,    27,    36,    12,    12,    76,    33,
    36,    36,   260,    34,    12,   263,    36,    36,    25,    36,
    24,    24,    45,    46,    47,    25,    49,    36,    25,     3,
     4,     5,   280,   281,    24,     9,   284,    25,    24,    32,
    20,    20,    36,    17,    18,   293,    20,    32,    32,    23,
    24,    25,    26,    27,    28,    33,    32,    31,    20,    36,
    34,    35,    51,    36,    12,    36,    40,    12,    36,    43,
    44,    12,    33,    33,    32,    12,    32,    36,    25,    36,
    32,    36,    56,    57,     3,     4,     5,    53,    62,    12,
     9,    33,    36,    25,    25,    69,    70,    25,    17,    18,
    20,    20,    76,    33,    23,    24,    25,    26,    27,    28,
    12,    33,    31,    12,    33,    20,    35,    20,    33,    33,
    33,    40,    32,    24,    43,    44,    24,     0,    32,     0,
    10,    95,    29,   135,    99,   200,   137,    56,    57,     3,
     4,     5,   256,    62,   136,     9,   100,   208,   180,   306,
    69,    70,   254,    17,    18,    -1,    20,    76,   276,    23,
    24,    25,    26,    27,    28,   220,    -1,    31,    -1,    -1,
    -1,    35,    36,    -1,    -1,    -1,    40,    -1,    -1,    43,
    44,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    56,    57,     3,     4,     5,    -1,    62,    -1,
     9,    -1,    -1,    -1,    -1,    69,    70,    -1,    17,    18,
    -1,    20,    76,    -1,    23,    24,    25,    26,    27,    28,
    -1,    -1,    31,    -1,    -1,    -1,    35,    -1,    -1,    -1,
    -1,    40,    -1,    -1,    43,    44,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    56,    57,     3,
     4,     5,    -1,    62,    -1,     9,    -1,    -1,    -1,    -1,
    69,    70,    -1,    17,    18,    -1,    20,    76,    -1,    23,
    24,    25,    26,    27,    28,    -1,    -1,    31,    -1,    -1,
    -1,    35,    -1,    -1,    -1,    -1,    40,    -1,    -1,    43,
    44,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    56,    57,     3,     4,     5,    -1,    62,    -1,
     9,    -1,    -1,    -1,    -1,    69,    70,    -1,    17,    18,
    -1,    20,    76,    -1,    23,    24,    25,    26,    27,    -1,
    -1,    -1,     3,     4,     5,    -1,    35,    -1,     9,    -1,
    -1,    40,    -1,    -1,    -1,    44,    17,    18,    -1,    20,
    -1,    -1,    23,    24,    25,    26,    27,    56,    57,    -1,
    -1,    -1,    -1,    -1,    35,    -1,    -1,    -1,    -1,    40,
    69,    70,    -1,    44,    -1,    -1,    -1,    76,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    69,    70,
    -1,    -1,    -1,    -1,    -1,    76
};
/* -*-C-*-  Note some compilers choke on comments on `#line' lines.  */
#line 3 "/usr/lib/bison.simple"
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

#line 217 "/usr/lib/bison.simple"

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
#line 163 "yacc.yy"
{
	    theParser->setRootNode( NULL );
	  ;
    break;}
case 2:
#line 167 "yacc.yy"
{
	    theParser->setRootNode( yyvsp[0].node );
	  ;
    break;}
case 3:
#line 171 "yacc.yy"
{
	    theParser->setRootNode( new KSParseNode( definitions, yyvsp[0].node ) );
	  ;
    break;}
case 4:
#line 178 "yacc.yy"
{
	    if ( yyvsp[0].node != 0 )
	    {
	    	yyval.node = new KSParseNode( definitions, yyvsp[0].node );
	    }
	  ;
    break;}
case 5:
#line 185 "yacc.yy"
{
	    if ( yyvsp[-1].node != 0 && yyvsp[0].node != 0 )
	    {
	    	yyval.node = new KSParseNode( definitions, yyvsp[-1].node );
	    	yyval.node->setBranch( 2, yyvsp[0].node );
	    }
	    else if ( yyvsp[-1].node != 0 )
	    {
	    	yyval.node = new KSParseNode( definitions, yyvsp[-1].node );
	    }
	    else if ( yyvsp[0].node != 0 )
	    {
	    	yyval.node = new KSParseNode( definitions, yyvsp[0].node );
	    }
	    else
	    {
		yyval.node = 0;
	    }
	  ;
    break;}
case 6:
#line 209 "yacc.yy"
{
	    yyval.node = 0;
	  ;
    break;}
case 7:
#line 213 "yacc.yy"
{
	    yyval.node = yyvsp[-1].node;
	  ;
    break;}
case 8:
#line 217 "yacc.yy"
{
	    yyval.node = yyvsp[-1].node;
	  ;
    break;}
case 9:
#line 221 "yacc.yy"
{
	    yyval.node = yyvsp[-2].node;
	  ;
    break;}
case 10:
#line 225 "yacc.yy"
{
	    yyval.node = new KSParseNode( import );
	    yyval.node->setIdent( yyvsp[-1].ident );
	  ;
    break;}
case 11:
#line 230 "yacc.yy"
{
	    yyval.node = new KSParseNode( from );
	    yyval.node->setIdent( yyvsp[-3].ident );
	    yyval.node->setStringLiteral( QString( "" ) );
	  ;
    break;}
case 12:
#line 236 "yacc.yy"
{
	    yyval.node = new KSParseNode( from );
	    yyval.node->setIdent( yyvsp[-3].ident );
	    yyval.node->setStringLiteral( yyvsp[-1]._str );
	  ;
    break;}
case 13:
#line 242 "yacc.yy"
{
	    yyval.node = yyvsp[0].node;
	  ;
    break;}
case 14:
#line 249 "yacc.yy"
{
		yyval._str = yyvsp[0].ident;
	  ;
    break;}
case 15:
#line 253 "yacc.yy"
{
		(*yyvsp[-2].ident) += "/";
		(*yyvsp[-2].ident) += (*yyvsp[0]._str);
		yyval._str = yyvsp[-2].ident;
	  ;
    break;}
case 16:
#line 262 "yacc.yy"
{
	    yyval.node = new KSParseNode( func_dcl );
	    yyval.node->setBranch( 2, yyvsp[-2].node );
	    yyval.node->setIdent( "main" );
	  ;
    break;}
case 17:
#line 268 "yacc.yy"
{
	    yyval.node = new KSParseNode( func_dcl, yyvsp[-6].node, yyvsp[-1].node );
	    yyval.node->setIdent( "main" );
	  ;
    break;}
case 18:
#line 276 "yacc.yy"
{
	    yyval.node = new KSParseNode( scoped_name );
	    yyval.node->setIdent( yyvsp[0].ident );
	  ;
    break;}
case 19:
#line 281 "yacc.yy"
{
	    yyval.node = new KSParseNode( scoped_name );
	    QString name = "::";
	    name += *(yyvsp[0].ident);
	    delete yyvsp[0].ident;
	    yyval.node->setIdent( name );
	  ;
    break;}
case 20:
#line 293 "yacc.yy"
{
	    yyval.node = new KSParseNode( assign_expr, yyvsp[-2].node, yyvsp[0].node );
	  ;
    break;}
case 21:
#line 297 "yacc.yy"
{
	    yyval.node = new KSParseNode( plus_assign, yyvsp[-2].node, yyvsp[0].node );
	  ;
    break;}
case 22:
#line 301 "yacc.yy"
{
	    yyval.node = new KSParseNode( minus_assign, yyvsp[-2].node, yyvsp[0].node );
	  ;
    break;}
case 23:
#line 305 "yacc.yy"
{
	    yyval.node = yyvsp[0].node;
	  ;
    break;}
case 24:
#line 312 "yacc.yy"
{
	    yyval.node = new KSParseNode( bool_or, yyvsp[-2].node, yyvsp[0].node );
	  ;
    break;}
case 25:
#line 316 "yacc.yy"
{
	    yyval.node = yyvsp[0].node;
	  ;
    break;}
case 26:
#line 323 "yacc.yy"
{
	    yyval.node = new KSParseNode( bool_and, yyvsp[-2].node, yyvsp[0].node );
	  ;
    break;}
case 27:
#line 327 "yacc.yy"
{
	    yyval.node = yyvsp[0].node;
	  ;
    break;}
case 28:
#line 334 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_equal, yyvsp[-2].node, yyvsp[0].node );
	  ;
    break;}
case 29:
#line 338 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_notequal, yyvsp[-2].node, yyvsp[0].node );
	  ;
    break;}
case 30:
#line 342 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_less_or_equal, yyvsp[-2].node, yyvsp[0].node );
	  ;
    break;}
case 31:
#line 346 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_greater_or_equal, yyvsp[-2].node, yyvsp[0].node );
	  ;
    break;}
case 32:
#line 350 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_less, yyvsp[-2].node, yyvsp[0].node );
	  ;
    break;}
case 33:
#line 354 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_greater, yyvsp[-2].node, yyvsp[0].node );
	  ;
    break;}
case 34:
#line 358 "yacc.yy"
{
	    yyval.node = yyvsp[0].node;
	  ;
    break;}
case 35:
#line 366 "yacc.yy"
{
	    yyval.node = yyvsp[0].node;
	  ;
    break;}
case 36:
#line 370 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_vertical_line, yyvsp[-2].node, yyvsp[0].node );
	  ;
    break;}
case 37:
#line 378 "yacc.yy"
{
	    yyval.node = yyvsp[0].node;
	  ;
    break;}
case 38:
#line 382 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_circumflex, yyvsp[-2].node, yyvsp[0].node );
	  ;
    break;}
case 39:
#line 390 "yacc.yy"
{
	    yyval.node = yyvsp[0].node;
	  ;
    break;}
case 40:
#line 394 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_ampersand, yyvsp[-2].node, yyvsp[0].node );
	  ;
    break;}
case 41:
#line 402 "yacc.yy"
{
	    yyval.node = yyvsp[0].node;
	  ;
    break;}
case 42:
#line 406 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_shiftright, yyvsp[-2].node, yyvsp[0].node );
	  ;
    break;}
case 43:
#line 410 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_shiftleft, yyvsp[-2].node, yyvsp[0].node );
	  ;
    break;}
case 44:
#line 418 "yacc.yy"
{
	    yyval.node = yyvsp[0].node;
	  ;
    break;}
case 45:
#line 422 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_plus_sign, yyvsp[-2].node, yyvsp[0].node );
	  ;
    break;}
case 46:
#line 426 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_minus_sign, yyvsp[-2].node, yyvsp[0].node );
	  ;
    break;}
case 47:
#line 434 "yacc.yy"
{
	    yyval.node = yyvsp[0].node;
	  ;
    break;}
case 48:
#line 438 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_asterik, yyvsp[-2].node, yyvsp[0].node );
	  ;
    break;}
case 49:
#line 442 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_solidus, yyvsp[-2].node, yyvsp[0].node );
	  ;
    break;}
case 50:
#line 446 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_percent_sign, yyvsp[-2].node, yyvsp[0].node );
	  ;
    break;}
case 51:
#line 454 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_minus_sign, yyvsp[0].node );
	  ;
    break;}
case 52:
#line 458 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_plus_sign, yyvsp[0].node );
	  ;
    break;}
case 53:
#line 462 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_tilde, yyvsp[0].node );
	  ;
    break;}
case 54:
#line 466 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_not, yyvsp[0].node );
	  ;
    break;}
case 55:
#line 470 "yacc.yy"
{
	    yyval.node = yyvsp[0].node;
	  ;
    break;}
case 56:
#line 477 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_array, yyvsp[-3].node, yyvsp[-1].node );
	  ;
    break;}
case 57:
#line 481 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_dict, yyvsp[-3].node, yyvsp[-1].node );
	  ;
    break;}
case 58:
#line 485 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_func_call, yyvsp[-2].node );
	  ;
    break;}
case 59:
#line 489 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_func_call, yyvsp[-3].node, yyvsp[-1].node );
	  ;
    break;}
case 60:
#line 493 "yacc.yy"
{
	    yyval.node = yyvsp[0].node;
	  ;
    break;}
case 61:
#line 500 "yacc.yy"
{
	    yyval.node = new KSParseNode( func_call_params, yyvsp[-2].node, yyvsp[0].node );
	  ;
    break;}
case 62:
#line 505 "yacc.yy"
{
	    yyval.node = new KSParseNode( func_call_params, yyvsp[0].node );
	  ;
    break;}
case 63:
#line 512 "yacc.yy"
{
	    yyval.node = new KSParseNode( member_expr, yyvsp[-2].node );
	    yyval.node->setIdent( yyvsp[0].ident );
	  ;
    break;}
case 64:
#line 517 "yacc.yy"
{
	    yyval.node = yyvsp[0].node;
	  ;
    break;}
case 65:
#line 524 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_incr, yyvsp[0].node );
	  ;
    break;}
case 66:
#line 528 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_decr, yyvsp[0].node );
	  ;
    break;}
case 67:
#line 532 "yacc.yy"
{
	    /* Setting $1 twice indicates that this is a postfix operator */
	    yyval.node = new KSParseNode( t_incr, yyvsp[-1].node, yyvsp[-1].node );
	  ;
    break;}
case 68:
#line 537 "yacc.yy"
{
	    /* Setting $1 twice indicates that this is a postfix operator */
	    yyval.node = new KSParseNode( t_decr, yyvsp[-1].node, yyvsp[-1].node );
	  ;
    break;}
case 69:
#line 542 "yacc.yy"
{
	    yyval.node = yyvsp[0].node;
	  ;
    break;}
case 70:
#line 549 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_match, yyvsp[-1].node );
	    yyval.node->setIdent( yyvsp[0]._str );
	  ;
    break;}
case 71:
#line 554 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_subst, yyvsp[-1].node );
	    yyval.node->setIdent( yyvsp[0].ident );
	  ;
    break;}
case 72:
#line 559 "yacc.yy"
{
	    yyval.node = yyvsp[0].node;
	  ;
    break;}
case 73:
#line 567 "yacc.yy"
{
	    yyval.node = yyvsp[0].node;
	  ;
    break;}
case 74:
#line 571 "yacc.yy"
{
	    yyval.node = yyvsp[0].node;
	  ;
    break;}
case 75:
#line 575 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_input );
	  ;
    break;}
case 76:
#line 579 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_match_line );
	    yyval.node->setIdent( yyvsp[0]._str );
	  ;
    break;}
case 77:
#line 583 "yacc.yy"
{ ;
    break;}
case 78:
#line 585 "yacc.yy"
{
	    yyval.node = yyvsp[-1].node;
	  ;
    break;}
case 79:
#line 594 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_integer_literal );
	    yyval.node->setIntegerLiteral( yyvsp[0]._int );
	  ;
    break;}
case 80:
#line 599 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_cell );
	    yyval.node->setStringLiteral( yyvsp[0]._str );
	  ;
    break;}
case 81:
#line 604 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_range );
	    yyval.node->setStringLiteral( yyvsp[0]._str );
	  ;
    break;}
case 82:
#line 609 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_string_literal );
	    yyval.node->setStringLiteral( yyvsp[0]._str );
	  ;
    break;}
case 83:
#line 614 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_character_literal );
	    yyval.node->setCharacterLiteral( yyvsp[0]._char );
	  ;
    break;}
case 84:
#line 619 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_floating_pt_literal );
	    yyval.node->setFloatingPtLiteral( yyvsp[0]._float );
	  ;
    break;}
case 85:
#line 624 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_boolean_literal );
	    yyval.node->setBooleanLiteral( true );
	  ;
    break;}
case 86:
#line 629 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_boolean_literal );
	    yyval.node->setBooleanLiteral( false );
	  ;
    break;}
case 87:
#line 634 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_array_const );
	  ;
    break;}
case 88:
#line 638 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_array_const, yyvsp[-1].node );
	  ;
    break;}
case 89:
#line 642 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_dict_const );
	  ;
    break;}
case 90:
#line 646 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_dict_const, yyvsp[-1].node );
	  ;
    break;}
case 91:
#line 650 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_line );
	  ;
    break;}
case 92:
#line 654 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_regexp_group );
	    yyval.node->setIntegerLiteral( yyvsp[0]._int );
	  ;
    break;}
case 93:
#line 662 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_array_element, yyvsp[-2].node, yyvsp[0].node );
	  ;
    break;}
case 94:
#line 666 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_array_element, yyvsp[0].node  );
	  ;
    break;}
case 95:
#line 673 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_dict_element, yyvsp[-5].node, yyvsp[-3].node, yyvsp[0].node );
	  ;
    break;}
case 96:
#line 677 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_dict_element, yyvsp[-3].node, yyvsp[-1].node );
	  ;
    break;}
case 97:
#line 687 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_struct, yyvsp[-1].node );
	    yyval.node->setIdent( yyvsp[-5].ident );
	  ;
    break;}
case 98:
#line 696 "yacc.yy"
{
	    if ( yyvsp[0].node != 0 )
	    {
	    	yyval.node = new KSParseNode( exports, yyvsp[0].node );
	    }
	  ;
    break;}
case 99:
#line 703 "yacc.yy"
{
	    if ( yyvsp[-1].node != 0 && yyvsp[0].node != 0 )
	    {
	    	yyval.node = new KSParseNode( exports, yyvsp[-1].node );
	    	yyval.node->setBranch( 2, yyvsp[0].node );
	    }
	    else if ( yyvsp[-1].node != 0 )
	    {
	    	yyval.node = new KSParseNode( exports, yyvsp[-1].node );
	    }
	    else if ( yyvsp[0].node != 0 )
	    {
	    	yyval.node = new KSParseNode( exports, yyvsp[0].node );
	    }
	    else
	    {
		yyval.node = 0;
	    }
	  ;
    break;}
case 100:
#line 726 "yacc.yy"
{
	    yyval.node = 0;
	  ;
    break;}
case 101:
#line 730 "yacc.yy"
{
	    yyval.node = yyvsp[-1].node;
	  ;
    break;}
case 102:
#line 734 "yacc.yy"
{
	    yyval.node = yyvsp[-1].node;
	  ;
    break;}
case 103:
#line 738 "yacc.yy"
{
	    yyval.node = yyvsp[-1].node;
	  ;
    break;}
case 104:
#line 745 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_struct_members );
	    yyval.node->setIdent( yyvsp[0].ident );
	  ;
    break;}
case 105:
#line 750 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_struct_members, yyvsp[0].node );
	    yyval.node->setIdent( yyvsp[-2].ident );
	  ;
    break;}
case 106:
#line 758 "yacc.yy"
{
	    yyval.node = new KSParseNode( func_dcl );
	    yyval.node->setBranch( 2, yyvsp[0].node );
	    yyval.node->setIdent( yyvsp[-6].ident );
	  ;
    break;}
case 107:
#line 764 "yacc.yy"
{
	    yyval.node = new KSParseNode( func_dcl, yyvsp[-5].node, yyvsp[0].node );
	    yyval.node->setIdent( yyvsp[-7].ident );
	  ;
    break;}
case 108:
#line 772 "yacc.yy"
{
	    yyval.node = new KSParseNode( func_params, yyvsp[0].node );
	  ;
    break;}
case 109:
#line 776 "yacc.yy"
{
	    yyval.node = new KSParseNode( func_params, yyvsp[-2].node, yyvsp[0].node );
	  ;
    break;}
case 110:
#line 783 "yacc.yy"
{
	    yyval.node = new KSParseNode( func_param_in );
	    yyval.node->setIdent( yyvsp[0].ident );
	  ;
    break;}
case 111:
#line 788 "yacc.yy"
{
	    yyval.node = new KSParseNode( func_param_in, yyvsp[0].node );
	    yyval.node->setIdent( yyvsp[-2].ident );
	  ;
    break;}
case 112:
#line 793 "yacc.yy"
{
	    yyval.node = new KSParseNode( func_param_out );
	    yyval.node->setIdent( yyvsp[0].ident );
	  ;
    break;}
case 113:
#line 798 "yacc.yy"
{
	    yyval.node = new KSParseNode( func_param_inout );
	    yyval.node->setIdent( yyvsp[0].ident );
	  ;
    break;}
case 114:
#line 806 "yacc.yy"
{
	    yyval.node = NULL;
	  ;
    break;}
case 115:
#line 810 "yacc.yy"
{
	    yyval.node = yyvsp[0].node;
	  ;
    break;}
case 116:
#line 817 "yacc.yy"
{
	    yyval.node = new KSParseNode( func_lines, yyvsp[0].node );
	  ;
    break;}
case 117:
#line 821 "yacc.yy"
{
	    yyval.node = new KSParseNode( func_lines, yyvsp[-1].node );
	    yyval.node->setBranch( 2, yyvsp[0].node );
	  ;
    break;}
case 118:
#line 829 "yacc.yy"
{
	    yyval.node = yyvsp[-1].node;
	  ;
    break;}
case 119:
#line 833 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_raise, yyvsp[-3].node, yyvsp[-1].node );
	  ;
    break;}
case 120:
#line 837 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_return );
	  ;
    break;}
case 121:
#line 841 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_return, yyvsp[-1].node );
	  ;
    break;}
case 122:
#line 845 "yacc.yy"
{
	    yyval.node = yyvsp[0].node;
	  ;
    break;}
case 123:
#line 849 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_try, yyvsp[-2].node, yyvsp[0].node );
	  ;
    break;}
case 124:
#line 853 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_scope, yyvsp[-1].node );
	  ;
    break;}
case 125:
#line 860 "yacc.yy"
{
	    yyval.node = yyvsp[0].node;
	  ;
    break;}
case 126:
#line 864 "yacc.yy"
{
	    yyval.node = yyvsp[-1].node;
	    yyval.node->setBranch( 4, yyvsp[0].node );
	  ;
    break;}
case 127:
#line 873 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_catch, yyvsp[-6].node, yyvsp[-1].node );
	    yyval.node->setIdent( yyvsp[-4].ident );
	  ;
    break;}
case 128:
#line 879 "yacc.yy"
{
	    KSParseNode* x = new KSParseNode( scoped_name );
	    x->setIdent( yyvsp[-6].ident );
	    yyval.node = new KSParseNode( t_catch_default, x, yyvsp[-1].node );
	    yyval.node->setIdent( yyvsp[-4].ident );
	  ;
    break;}
case 129:
#line 889 "yacc.yy"
{
	    yyval.node = new KSParseNode( const_dcl, yyvsp[0].node );
	    yyval.node->setIdent( yyvsp[-2].ident );
	  ;
    break;}
case 130:
#line 897 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_while, yyvsp[-1].node, yyvsp[0].node );
	  ;
    break;}
case 131:
#line 901 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_for, yyvsp[-6].node, yyvsp[-4].node, yyvsp[-2].node, yyvsp[0].node );
	  ;
    break;}
case 132:
#line 905 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_do, yyvsp[-2].node, yyvsp[-1].node );
	  ;
    break;}
case 133:
#line 909 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_if, yyvsp[-3].node, yyvsp[-1].node, yyvsp[0].node );
	  ;
    break;}
case 134:
#line 913 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_foreach, yyvsp[-2].node, yyvsp[0].node );
	    yyval.node->setIdent( yyvsp[-4].ident );
	  ;
    break;}
case 135:
#line 918 "yacc.yy"
{
	    /* We set $9 twice to indicate thet this is the foreach for maps */
	    yyval.node = new KSParseNode( t_foreach, yyvsp[-2].node, yyvsp[0].node, yyvsp[0].node );
	    yyval.node->setIdent( yyvsp[-6].ident );
	    yyval.node->setStringLiteral( yyvsp[-4].ident );
	  ;
    break;}
case 136:
#line 928 "yacc.yy"
{
	    yyval.node = NULL;
	  ;
    break;}
case 137:
#line 932 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_if, yyvsp[-3].node, yyvsp[-1].node, yyvsp[0].node );
	  ;
    break;}
case 138:
#line 936 "yacc.yy"
{
	    yyval.node = yyvsp[0].node;
	  ;
    break;}
case 139:
#line 943 "yacc.yy"
{
	    yyval.node = yyvsp[-1].node;
	  ;
    break;}
case 140:
#line 950 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_scope, yyvsp[-1].node );
	  ;
    break;}
}
   /* the action file gets copied in in place of this dollarsign */
#line 543 "/usr/lib/bison.simple"

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
#line 954 "yacc.yy"


void kscriptParse( const char *_code, int extension, KLocale* locale )
{
    kscriptInitFlex( _code, extension, locale );
    yyparse();
}

void kscriptParse( int extension, KLocale* locale )
{
    kscriptInitFlex( extension, locale );
    yyparse();
}
