
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
#define	T_NOTEQUAL1	303
#define	T_NOTEQUAL2	304
#define	T_MEMBER	305
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
#define	T_IMPORT	319
#define	T_VAR	320
#define	T_CATCH	321
#define	T_TRY	322
#define	T_RAISE	323
#define	T_RANGE	324
#define	T_CELL	325
#define	T_FROM	326
#define	T_PLUS_ASSIGN	327
#define	T_MINUS_ASSIGN	328
#define	T_AND	329
#define	T_OR	330
#define	T_DOLLAR	331
#define	T_UNKNOWN	332

#line 20 "yacc.yy"


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
void kspread_mode();

void yyerror( const char *s )
{
  theParser->parse_error( idl_lexFile.local8Bit(), s, idl_line_no );
}


#line 48 "yacc.yy"
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



#define	YYFINAL		305
#define	YYFLAG		-32768
#define	YYNTBASE	79

#define YYTRANSLATE(x) ((unsigned)(x) <= 332 ? yytranslate[x] : 123)

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
    77,    78
};

#if YYDEBUG != 0
static const short yyprhs[] = {     0,
     0,     1,     2,     6,     8,    10,    13,    16,    19,    22,
    26,    32,    38,    40,    42,    46,    51,    59,    61,    64,
    68,    72,    76,    78,    82,    84,    88,    90,    94,    98,
   102,   106,   110,   114,   118,   120,   122,   126,   128,   132,
   134,   138,   140,   144,   148,   150,   154,   158,   160,   164,
   168,   172,   175,   178,   181,   184,   186,   191,   196,   200,
   205,   207,   211,   213,   217,   219,   222,   225,   228,   231,
   233,   236,   239,   241,   243,   245,   247,   249,   253,   257,
   259,   261,   263,   265,   267,   269,   271,   273,   276,   280,
   283,   287,   289,   292,   296,   298,   306,   312,   318,   320,
   323,   326,   330,   333,   335,   339,   345,   352,   354,   358,
   361,   366,   369,   372,   373,   375,   377,   380,   383,   389,
   392,   396,   398,   404,   408,   410,   413,   423,   433,   438,
   441,   451,   456,   463,   471,   481,   482,   490,   493,   498
};

static const short yyrhs[] = {    -1,
     0,    48,    80,    89,     0,    81,     0,    82,     0,    82,
    81,     0,   118,    36,     0,   106,    36,     0,   110,    32,
     0,    65,    20,    36,     0,    72,    20,    65,     7,    36,
     0,    72,    20,    65,    83,    36,     0,    84,     0,    20,
     0,    20,    12,    83,     0,    59,    24,   113,    32,     0,
    59,    25,   111,    33,    24,   113,    32,     0,    20,     0,
    35,    20,     0,    87,    48,    86,     0,    87,    73,    86,
     0,    87,    74,    86,     0,    87,     0,    88,    76,    87,
     0,    88,     0,    89,    75,    88,     0,    89,     0,    90,
    16,    89,     0,    90,    49,    89,     0,    90,    50,    89,
     0,    90,    46,    89,     0,    90,    47,    89,     0,    90,
    27,    89,     0,    90,    19,    89,     0,    90,     0,    91,
     0,    90,    45,    91,     0,    92,     0,    91,    10,    92,
     0,    93,     0,    92,     6,    93,     0,    94,     0,    93,
    38,    94,     0,    93,    37,    94,     0,    95,     0,    94,
    31,    95,     0,    94,    28,    95,     0,    96,     0,    95,
     7,    96,     0,    95,    39,    96,     0,    95,    30,    96,
     0,    28,    97,     0,    31,    97,     0,    43,    97,     0,
    63,    97,     0,    97,     0,    97,    26,    86,    34,     0,
    97,    24,    86,    32,     0,    97,    25,    33,     0,    97,
    25,    98,    33,     0,    99,     0,    86,    12,    98,     0,
    86,     0,    97,    51,    20,     0,   100,     0,    57,   101,
     0,    58,   101,     0,   101,    57,     0,   101,    58,     0,
   101,     0,   102,    62,     0,   102,    61,     0,   102,     0,
    85,     0,   103,     0,     5,     0,     3,     0,    27,    90,
    19,     0,    25,    86,    33,     0,    23,     0,    71,     0,
    70,     0,    40,     0,     9,     0,    18,     0,    44,     0,
    17,     0,    26,    34,     0,    26,   104,    34,     0,    24,
    32,     0,    24,   105,    32,     0,     4,     0,    77,    23,
     0,    86,    12,   104,     0,    86,     0,    25,    86,    12,
    89,    33,    12,   105,     0,    25,    86,    12,    89,    33,
     0,    41,    20,    24,   107,    32,     0,   108,     0,   108,
   107,     0,   110,    32,     0,    66,   109,    36,     0,   118,
    36,     0,    20,     0,    20,    12,   109,     0,    20,    25,
    33,    24,   113,     0,    20,    25,   111,    33,    24,   113,
     0,   112,     0,   112,    12,   111,     0,    21,    20,     0,
    21,    20,    48,    86,     0,    29,    20,     0,    22,    20,
     0,     0,   114,     0,   115,     0,   115,   114,     0,    86,
    36,     0,    69,    86,    12,    86,    36,     0,    64,    36,
     0,    64,    86,    36,     0,   119,     0,    68,    24,   114,
    32,   116,     0,    24,   114,    32,     0,   117,     0,   117,
   116,     0,    67,    25,    86,    12,    20,    33,    24,   114,
    32,     0,    14,    25,    20,    12,    20,    33,    24,   114,
    32,     0,    13,    20,    48,    89,     0,   121,   122,     0,
    55,    25,    86,    36,    86,    36,    86,    33,   122,     0,
    56,   122,   121,    36,     0,    53,    25,    86,    33,   122,
   120,     0,    60,    25,    20,    12,    86,    33,   122,     0,
    60,    25,    20,    12,    20,    12,    86,    33,   122,     0,
     0,    54,    53,    25,    86,    33,   122,   120,     0,    54,
   122,     0,    52,    25,    86,    33,     0,    24,   113,    32,
     0
};

#endif

#if YYDEBUG != 0
static const short yyrline[] = { 0,
   182,   186,   187,   190,   197,   204,   228,   232,   236,   240,
   245,   251,   257,   264,   268,   277,   283,   291,   296,   308,
   312,   316,   320,   327,   331,   338,   342,   349,   353,   357,
   361,   365,   369,   373,   377,   385,   389,   397,   401,   409,
   413,   421,   425,   429,   437,   441,   445,   453,   457,   461,
   465,   473,   477,   481,   485,   489,   496,   500,   504,   508,
   512,   519,   524,   531,   536,   543,   547,   551,   556,   561,
   568,   573,   578,   586,   590,   594,   598,   603,   604,   613,
   618,   623,   628,   633,   638,   643,   648,   653,   657,   661,
   665,   669,   673,   681,   685,   692,   696,   706,   715,   722,
   745,   749,   753,   760,   765,   773,   779,   787,   791,   798,
   803,   808,   813,   821,   825,   832,   836,   844,   848,   852,
   856,   860,   864,   868,   875,   879,   887,   893,   904,   912,
   916,   920,   924,   928,   933,   943,   947,   951,   958,   965
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
"T_NOTEQUAL1","T_NOTEQUAL2","T_MEMBER","T_WHILE","T_IF","T_ELSE","T_FOR","T_DO",
"T_INCR","T_DECR","T_MAIN","T_FOREACH","T_SUBST","T_MATCH","T_NOT","T_RETURN",
"T_IMPORT","T_VAR","T_CATCH","T_TRY","T_RAISE","T_RANGE","T_CELL","T_FROM","T_PLUS_ASSIGN",
"T_MINUS_ASSIGN","T_AND","T_OR","T_DOLLAR","T_UNKNOWN","specification","@1",
"definitions","definition","import_list","main","scoped_name","assign_expr",
"bool_or","bool_and","equal_expr","or_expr","xor_expr","and_expr","shift_expr",
"add_expr","mult_expr","unary_expr","index_expr","func_call_params","member_expr",
"incr_expr","match_expr","primary_expr","literal","array_elements","dict_elements",
"struct_dcl","struct_exports","struct_export","struct_members","func_dcl","func_params",
"func_param","func_body","func_lines","func_line","catches","single_catch","const_dcl",
"loops","else","while","loop_body", NULL
};
#endif

static const short yyr1[] = {     0,
    79,    80,    79,    79,    81,    81,    82,    82,    82,    82,
    82,    82,    82,    83,    83,    84,    84,    85,    85,    86,
    86,    86,    86,    87,    87,    88,    88,    89,    89,    89,
    89,    89,    89,    89,    89,    90,    90,    91,    91,    92,
    92,    93,    93,    93,    94,    94,    94,    95,    95,    95,
    95,    96,    96,    96,    96,    96,    97,    97,    97,    97,
    97,    98,    98,    99,    99,   100,   100,   100,   100,   100,
   101,   101,   101,   102,   102,   102,   102,   102,   102,   103,
   103,   103,   103,   103,   103,   103,   103,   103,   103,   103,
   103,   103,   103,   104,   104,   105,   105,   106,   107,   107,
   108,   108,   108,   109,   109,   110,   110,   111,   111,   112,
   112,   112,   112,   113,   113,   114,   114,   115,   115,   115,
   115,   115,   115,   115,   116,   116,   117,   117,   118,   119,
   119,   119,   119,   119,   119,   120,   120,   120,   121,   122
};

static const short yyr2[] = {     0,
     0,     0,     3,     1,     1,     2,     2,     2,     2,     3,
     5,     5,     1,     1,     3,     4,     7,     1,     2,     3,
     3,     3,     1,     3,     1,     3,     1,     3,     3,     3,
     3,     3,     3,     3,     1,     1,     3,     1,     3,     1,
     3,     1,     3,     3,     1,     3,     3,     1,     3,     3,
     3,     2,     2,     2,     2,     1,     4,     4,     3,     4,
     1,     3,     1,     3,     1,     2,     2,     2,     2,     1,
     2,     2,     1,     1,     1,     1,     1,     3,     3,     1,
     1,     1,     1,     1,     1,     1,     1,     2,     3,     2,
     3,     1,     2,     3,     1,     7,     5,     5,     1,     2,
     2,     3,     2,     1,     3,     5,     6,     1,     3,     2,
     4,     2,     2,     0,     1,     1,     2,     2,     5,     2,
     3,     1,     5,     3,     1,     2,     9,     9,     4,     2,
     9,     4,     6,     7,     9,     0,     7,     2,     4,     3
};

static const short yydefact[] = {     1,
     0,     0,     0,     2,     0,     0,     0,     4,     5,    13,
     0,     0,     0,     0,     0,     0,     0,   114,     0,     0,
     0,     6,     8,     9,     7,     0,     0,     0,     0,     0,
     0,   108,     0,    77,    92,    76,    84,    87,    85,    18,
    80,     0,     0,     0,     0,     0,     0,     0,    83,     0,
    86,     0,     0,     0,    82,    81,     0,    74,     3,    35,
    36,    38,    40,    42,    45,    48,    56,    61,    65,    70,
    73,    75,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,    23,    25,    27,     0,   115,   116,   122,     0,
     0,    10,     0,   129,   110,   113,   112,   114,     0,     0,
     0,     0,    99,     0,     0,     0,    90,     0,     0,    88,
    95,     0,     0,    52,    53,    19,    54,    66,    67,    55,
    93,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,    68,    69,    72,    71,     0,     0,     0,     0,
     0,   114,     0,     0,   120,     0,     0,     0,   118,     0,
     0,     0,     0,     0,    16,   117,   130,     0,     0,    14,
     0,     0,   106,   114,   109,   104,     0,    98,   100,   101,
   103,     0,    91,    79,     0,    89,    78,    28,    34,    33,
    37,    31,    32,    29,    30,    39,    41,    44,    43,    47,
    46,    49,    51,    50,     0,    59,    63,     0,     0,    64,
     0,   124,     0,     0,     0,     0,     0,     0,   121,     0,
     0,    20,    21,    22,    24,    26,   114,    11,     0,    12,
   111,   107,     0,   102,     0,    94,    58,     0,    60,    57,
   139,     0,     0,   140,   132,     0,     0,     0,     0,    15,
   105,     0,    62,   136,     0,    18,     0,     0,     0,   123,
   125,   119,    17,    97,     0,   133,     0,     0,     0,     0,
     0,   126,     0,     0,   138,     0,     0,   134,     0,     0,
    96,     0,     0,     0,     0,     0,     0,   131,   135,     0,
     0,     0,     0,     0,   136,     0,     0,   137,     0,     0,
   128,   127,     0,     0,     0
};

static const short yydefgoto[] = {   303,
    17,     8,     9,   171,    10,    58,    82,    83,    84,    85,
    60,    61,    62,    63,    64,    65,    66,    67,   208,    68,
    69,    70,    71,    72,   112,   108,    11,   102,   103,   177,
    12,    31,    32,    86,    87,    88,   260,   261,    13,    89,
   266,    90,   153
};

static const short yypact[] = {    -4,
   -10,    -1,    -8,-32768,    48,    21,    40,-32768,    10,-32768,
    -7,    45,    47,    38,   112,    76,   476,   295,   114,    67,
    58,-32768,-32768,-32768,-32768,   476,    85,    92,   108,   106,
    99,   125,     1,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
-32768,   -17,   476,   364,   476,   135,   135,   131,-32768,   135,
-32768,   561,   561,   135,-32768,-32768,   133,-32768,-32768,    75,
   166,   171,    52,    -9,    13,-32768,     8,-32768,-32768,    91,
   105,-32768,   226,   156,   157,   158,   161,   162,    22,   164,
   476,   150,    53,   113,   115,   159,-32768,   295,-32768,   161,
   163,-32768,    64,-32768,   146,-32768,-32768,   295,   173,   114,
   181,   170,     1,   172,   174,   476,-32768,   176,   178,-32768,
   197,   179,     9,     8,     8,-32768,     8,-32768,-32768,     8,
-32768,   476,   476,   476,   476,   476,   476,   476,   476,   476,
   476,   476,   476,   476,   476,   476,   476,   476,   476,   420,
   476,   194,-32768,-32768,-32768,-32768,   476,   183,   476,   476,
   476,   295,   165,   196,-32768,   182,   295,   207,-32768,   476,
   476,   476,   476,   476,-32768,-32768,-32768,   199,   184,   209,
   198,   476,-32768,   295,-32768,   220,   200,-32768,-32768,-32768,
-32768,   221,-32768,-32768,   476,-32768,-32768,-32768,-32768,-32768,
   166,-32768,-32768,-32768,-32768,   171,    52,    -9,    -9,    13,
    13,-32768,-32768,-32768,   205,-32768,   228,   208,   204,-32768,
    62,-32768,   212,   214,   206,   216,   219,   244,-32768,   227,
   476,-32768,-32768,-32768,-32768,-32768,   295,-32768,   240,-32768,
-32768,-32768,   181,-32768,   476,-32768,-32768,   476,-32768,-32768,
-32768,   161,   476,-32768,-32768,   532,     3,   229,   230,-32768,
-32768,   231,-32768,   213,   232,   251,   238,   247,   248,-32768,
     3,-32768,-32768,   262,    11,-32768,   476,   476,   161,   255,
   476,-32768,   252,   260,-32768,   243,   254,-32768,   268,   276,
-32768,   476,   161,   161,   271,   272,   269,-32768,-32768,   273,
   274,   161,   277,   281,   213,   295,   295,-32768,   261,   278,
-32768,-32768,   308,   309,-32768
};

static const short yypgoto[] = {-32768,
-32768,   302,-32768,    87,-32768,-32768,   -43,   151,   153,   -13,
   279,   202,   195,   201,    36,    37,    27,   100,    90,-32768,
-32768,   121,-32768,-32768,   144,    60,-32768,   233,-32768,    98,
   -27,   -12,-32768,   -96,   -70,-32768,    73,-32768,   -22,-32768,
    42,   187,   -85
};


#define	YYLAST		638


static const short yytable[] = {   109,
   111,   173,   148,    59,   167,   104,    91,   106,     1,    14,
   105,    16,    94,     1,   107,     2,   258,   166,   134,   136,
     2,   135,     1,    15,    34,    35,    36,   187,    23,     2,
    37,   139,   140,   141,   152,   156,     3,   158,    38,    39,
    20,    40,   137,     4,    41,    42,    43,    44,    45,    46,
     3,   138,    47,   125,     5,   216,    48,   155,   142,    21,
     6,    49,   182,   274,    50,    51,   101,     7,     5,   259,
   169,    18,    19,   235,     6,   104,    24,   232,    52,    53,
   105,     7,    25,   170,    54,    26,   220,   175,   132,   133,
   122,    55,    56,   123,   184,   205,   207,   209,    57,    33,
   160,   124,    92,   211,    95,   213,   214,   215,   188,   189,
   190,    96,   192,   193,   194,   195,   222,   223,   224,   125,
   126,   127,    93,   128,   129,   161,   162,    97,   231,    98,
   249,    99,    27,    28,    27,    28,   100,    34,    35,    36,
    29,   111,    29,    37,    30,   114,   115,   143,   144,   117,
   116,    38,    39,   120,    40,   121,   254,    41,    42,    43,
    44,    45,   202,   203,   204,   145,   146,   198,   199,    48,
   200,   201,   118,   119,    49,   130,   131,   248,    51,   275,
   149,   150,   151,   278,   152,   159,   154,   157,   163,   164,
   165,    52,    53,   172,   207,   168,   174,   288,   289,   255,
   176,   178,   257,   180,    55,    56,   295,   183,   185,   181,
   184,    57,   186,   210,   212,   218,    74,   219,   221,   228,
   229,   252,   227,   276,   277,   299,   300,   280,    34,    35,
    36,   233,   235,   230,    37,   234,   237,   240,   287,   238,
   239,   243,    38,    39,   241,    40,   242,   244,    41,    73,
   147,    44,    45,    46,   245,   246,    47,   107,   247,   170,
    48,   263,   268,   264,   262,    49,   265,   267,    50,    51,
   269,   270,   271,   273,   279,   283,   106,    74,    75,   285,
    76,    77,    52,    53,   282,    78,   284,   286,    54,    79,
   290,   291,   301,    80,    81,    55,    56,    34,    35,    36,
   296,   292,    57,    37,   297,   293,   294,   304,   305,   302,
    22,    38,    39,   225,    40,   250,   226,    41,    73,    43,
    44,    45,    46,   113,   196,    47,   191,   253,   236,    48,
   251,   197,   281,   272,    49,   179,   298,    50,    51,   217,
     0,     0,     0,     0,     0,     0,    74,    75,     0,    76,
    77,    52,    53,     0,    78,     0,     0,    54,    79,     0,
     0,     0,    80,    81,    55,    56,    34,    35,    36,     0,
     0,    57,    37,     0,     0,     0,     0,     0,     0,     0,
    38,    39,     0,    40,     0,     0,    41,    42,    43,    44,
    45,    46,     0,     0,    47,     0,     0,   110,    48,     0,
     0,     0,     0,    49,     0,     0,    50,    51,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
    52,    53,    34,    35,    36,     0,    54,     0,    37,     0,
     0,     0,     0,    55,    56,     0,    38,    39,     0,    40,
    57,     0,    41,    42,    43,    44,    45,    46,     0,     0,
    47,     0,   206,     0,    48,     0,     0,     0,     0,    49,
     0,     0,    50,    51,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,    52,    53,    34,    35,
    36,     0,    54,     0,    37,     0,     0,     0,     0,    55,
    56,     0,    38,    39,     0,    40,    57,     0,    41,    42,
    43,    44,    45,    46,     0,     0,    47,     0,     0,     0,
    48,     0,     0,     0,     0,    49,     0,     0,    50,    51,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,    52,    53,    34,    35,    36,     0,    54,     0,
    37,     0,     0,     0,     0,    55,    56,     0,    38,    39,
     0,   256,    57,     0,    41,    42,    43,    44,    45,    46,
     0,     0,    47,    34,    35,    36,    48,     0,     0,    37,
     0,    49,     0,     0,    50,    51,     0,    38,    39,     0,
    40,     0,     0,    41,    42,    43,    44,    45,    52,    53,
     0,     0,     0,     0,    54,    48,     0,     0,     0,     0,
    49,    55,    56,     0,    51,     0,     0,     0,    57,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
    55,    56,     0,     0,     0,     0,     0,    57
};

static const short yycheck[] = {    43,
    44,    98,    73,    17,    90,    33,    19,    25,    13,    20,
    33,    20,    26,    13,    32,    20,    14,    88,    28,     7,
    20,    31,    13,    25,     3,     4,     5,    19,    36,    20,
     9,    24,    25,    26,    24,    79,    41,    81,    17,    18,
    20,    20,    30,    48,    23,    24,    25,    26,    27,    28,
    41,    39,    31,    45,    59,   152,    35,    36,    51,    20,
    65,    40,   106,    53,    43,    44,    66,    72,    59,    67,
     7,    24,    25,    12,    65,   103,    32,   174,    57,    58,
   103,    72,    36,    20,    63,    48,   157,   100,    37,    38,
    16,    70,    71,    19,    33,   139,   140,   141,    77,    24,
    48,    27,    36,   147,    20,   149,   150,   151,   122,   123,
   124,    20,   126,   127,   128,   129,   160,   161,   162,    45,
    46,    47,    65,    49,    50,    73,    74,    20,   172,    24,
   227,    33,    21,    22,    21,    22,    12,     3,     4,     5,
    29,   185,    29,     9,    33,    46,    47,    57,    58,    50,
    20,    17,    18,    54,    20,    23,   242,    23,    24,    25,
    26,    27,   136,   137,   138,    61,    62,   132,   133,    35,
   134,   135,    52,    53,    40,    10,     6,   221,    44,   265,
    25,    25,    25,   269,    24,    36,    25,    24,    76,    75,
    32,    57,    58,    48,   238,    33,    24,   283,   284,   243,
    20,    32,   246,    32,    70,    71,   292,    32,    12,    36,
    33,    77,    34,    20,    32,    20,    52,    36,    12,    36,
    12,   235,    24,   267,   268,   296,   297,   271,     3,     4,
     5,    12,    12,    36,     9,    36,    32,    34,   282,    12,
    33,    36,    17,    18,    33,    20,    33,    32,    23,    24,
    25,    26,    27,    28,    36,    12,    31,    32,    32,    20,
    35,    32,    12,    33,    36,    40,    54,    36,    43,    44,
    33,    25,    25,    12,    20,    33,    25,    52,    53,    12,
    55,    56,    57,    58,    25,    60,    33,    12,    63,    64,
    20,    20,    32,    68,    69,    70,    71,     3,     4,     5,
    24,    33,    77,     9,    24,    33,    33,     0,     0,    32,
     9,    17,    18,   163,    20,   229,   164,    23,    24,    25,
    26,    27,    28,    45,   130,    31,   125,   238,   185,    35,
   233,   131,   273,   261,    40,   103,   295,    43,    44,   153,
    -1,    -1,    -1,    -1,    -1,    -1,    52,    53,    -1,    55,
    56,    57,    58,    -1,    60,    -1,    -1,    63,    64,    -1,
    -1,    -1,    68,    69,    70,    71,     3,     4,     5,    -1,
    -1,    77,     9,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    17,    18,    -1,    20,    -1,    -1,    23,    24,    25,    26,
    27,    28,    -1,    -1,    31,    -1,    -1,    34,    35,    -1,
    -1,    -1,    -1,    40,    -1,    -1,    43,    44,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    57,    58,     3,     4,     5,    -1,    63,    -1,     9,    -1,
    -1,    -1,    -1,    70,    71,    -1,    17,    18,    -1,    20,
    77,    -1,    23,    24,    25,    26,    27,    28,    -1,    -1,
    31,    -1,    33,    -1,    35,    -1,    -1,    -1,    -1,    40,
    -1,    -1,    43,    44,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    57,    58,     3,     4,
     5,    -1,    63,    -1,     9,    -1,    -1,    -1,    -1,    70,
    71,    -1,    17,    18,    -1,    20,    77,    -1,    23,    24,
    25,    26,    27,    28,    -1,    -1,    31,    -1,    -1,    -1,
    35,    -1,    -1,    -1,    -1,    40,    -1,    -1,    43,    44,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    57,    58,     3,     4,     5,    -1,    63,    -1,
     9,    -1,    -1,    -1,    -1,    70,    71,    -1,    17,    18,
    -1,    20,    77,    -1,    23,    24,    25,    26,    27,    28,
    -1,    -1,    31,     3,     4,     5,    35,    -1,    -1,     9,
    -1,    40,    -1,    -1,    43,    44,    -1,    17,    18,    -1,
    20,    -1,    -1,    23,    24,    25,    26,    27,    57,    58,
    -1,    -1,    -1,    -1,    63,    35,    -1,    -1,    -1,    -1,
    40,    70,    71,    -1,    44,    -1,    -1,    -1,    77,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    70,    71,    -1,    -1,    -1,    -1,    -1,    77
};
/* -*-C-*-  Note some compilers choke on comments on `#line' lines.  */
#line 3 "/usr/share/bison.simple"
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

#line 217 "/usr/share/bison.simple"

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
#line 183 "yacc.yy"
{
            theParser->setRootNode( NULL );
          ;
    break;}
case 2:
#line 186 "yacc.yy"
{ kspread_mode(); ;
    break;}
case 3:
#line 187 "yacc.yy"
{
            theParser->setRootNode( yyvsp[0].node );
          ;
    break;}
case 4:
#line 191 "yacc.yy"
{
            theParser->setRootNode( new KSParseNode( definitions, yyvsp[0].node ) );
          ;
    break;}
case 5:
#line 198 "yacc.yy"
{
            if ( yyvsp[0].node != 0 )
            {
                yyval.node = new KSParseNode( definitions, yyvsp[0].node );
            }
          ;
    break;}
case 6:
#line 205 "yacc.yy"
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
case 7:
#line 229 "yacc.yy"
{
            yyval.node = yyvsp[-1].node;
          ;
    break;}
case 8:
#line 233 "yacc.yy"
{
            yyval.node = yyvsp[-1].node;
          ;
    break;}
case 9:
#line 237 "yacc.yy"
{
            yyval.node = yyvsp[-1].node;
          ;
    break;}
case 10:
#line 241 "yacc.yy"
{
            yyval.node = new KSParseNode( import );
            yyval.node->setIdent( yyvsp[-1].ident );
          ;
    break;}
case 11:
#line 246 "yacc.yy"
{
            yyval.node = new KSParseNode( from );
            yyval.node->setIdent( yyvsp[-3].ident );
            yyval.node->setStringLiteral( QString( "" ) );
          ;
    break;}
case 12:
#line 252 "yacc.yy"
{
            yyval.node = new KSParseNode( from );
            yyval.node->setIdent( yyvsp[-3].ident );
            yyval.node->setStringLiteral( yyvsp[-1]._str );
          ;
    break;}
case 13:
#line 258 "yacc.yy"
{
            yyval.node = yyvsp[0].node;
          ;
    break;}
case 14:
#line 265 "yacc.yy"
{
                yyval._str = yyvsp[0].ident;
          ;
    break;}
case 15:
#line 269 "yacc.yy"
{
                (*yyvsp[-2].ident) += "/";
                (*yyvsp[-2].ident) += (*yyvsp[0]._str);
                yyval._str = yyvsp[-2].ident;
          ;
    break;}
case 16:
#line 278 "yacc.yy"
{
            yyval.node = new KSParseNode( func_dcl );
            yyval.node->setBranch( 2, yyvsp[-1].node );
            yyval.node->setIdent( "main" );
          ;
    break;}
case 17:
#line 284 "yacc.yy"
{
            yyval.node = new KSParseNode( func_dcl, yyvsp[-4].node, yyvsp[-1].node );
            yyval.node->setIdent( "main" );
          ;
    break;}
case 18:
#line 292 "yacc.yy"
{
            yyval.node = new KSParseNode( scoped_name );
            yyval.node->setIdent( yyvsp[0].ident );
          ;
    break;}
case 19:
#line 297 "yacc.yy"
{
            yyval.node = new KSParseNode( scoped_name );
            QString name = "::";
            name += *(yyvsp[0].ident);
            delete yyvsp[0].ident;
            yyval.node->setIdent( name );
          ;
    break;}
case 20:
#line 309 "yacc.yy"
{
            yyval.node = new KSParseNode( assign_expr, yyvsp[-2].node, yyvsp[0].node );
          ;
    break;}
case 21:
#line 313 "yacc.yy"
{
            yyval.node = new KSParseNode( plus_assign, yyvsp[-2].node, yyvsp[0].node );
          ;
    break;}
case 22:
#line 317 "yacc.yy"
{
            yyval.node = new KSParseNode( minus_assign, yyvsp[-2].node, yyvsp[0].node );
          ;
    break;}
case 23:
#line 321 "yacc.yy"
{
            yyval.node = yyvsp[0].node;
          ;
    break;}
case 24:
#line 328 "yacc.yy"
{
            yyval.node = new KSParseNode( bool_or, yyvsp[-2].node, yyvsp[0].node );
          ;
    break;}
case 25:
#line 332 "yacc.yy"
{
            yyval.node = yyvsp[0].node;
          ;
    break;}
case 26:
#line 339 "yacc.yy"
{
            yyval.node = new KSParseNode( bool_and, yyvsp[-2].node, yyvsp[0].node );
          ;
    break;}
case 27:
#line 343 "yacc.yy"
{
            yyval.node = yyvsp[0].node;
          ;
    break;}
case 28:
#line 350 "yacc.yy"
{
            yyval.node = new KSParseNode( t_equal, yyvsp[-2].node, yyvsp[0].node );
          ;
    break;}
case 29:
#line 354 "yacc.yy"
{
            yyval.node = new KSParseNode( t_notequal, yyvsp[-2].node, yyvsp[0].node );
          ;
    break;}
case 30:
#line 358 "yacc.yy"
{
            yyval.node = new KSParseNode( t_notequal, yyvsp[-2].node, yyvsp[0].node );
          ;
    break;}
case 31:
#line 362 "yacc.yy"
{
            yyval.node = new KSParseNode( t_less_or_equal, yyvsp[-2].node, yyvsp[0].node );
          ;
    break;}
case 32:
#line 366 "yacc.yy"
{
            yyval.node = new KSParseNode( t_greater_or_equal, yyvsp[-2].node, yyvsp[0].node );
          ;
    break;}
case 33:
#line 370 "yacc.yy"
{
            yyval.node = new KSParseNode( t_less, yyvsp[-2].node, yyvsp[0].node );
          ;
    break;}
case 34:
#line 374 "yacc.yy"
{
            yyval.node = new KSParseNode( t_greater, yyvsp[-2].node, yyvsp[0].node );
          ;
    break;}
case 35:
#line 378 "yacc.yy"
{
            yyval.node = yyvsp[0].node;
          ;
    break;}
case 36:
#line 386 "yacc.yy"
{
            yyval.node = yyvsp[0].node;
          ;
    break;}
case 37:
#line 390 "yacc.yy"
{
            yyval.node = new KSParseNode( t_vertical_line, yyvsp[-2].node, yyvsp[0].node );
          ;
    break;}
case 38:
#line 398 "yacc.yy"
{
            yyval.node = yyvsp[0].node;
          ;
    break;}
case 39:
#line 402 "yacc.yy"
{
            yyval.node = new KSParseNode( t_circumflex, yyvsp[-2].node, yyvsp[0].node );
          ;
    break;}
case 40:
#line 410 "yacc.yy"
{
            yyval.node = yyvsp[0].node;
          ;
    break;}
case 41:
#line 414 "yacc.yy"
{
            yyval.node = new KSParseNode( t_ampersand, yyvsp[-2].node, yyvsp[0].node );
          ;
    break;}
case 42:
#line 422 "yacc.yy"
{
            yyval.node = yyvsp[0].node;
          ;
    break;}
case 43:
#line 426 "yacc.yy"
{
            yyval.node = new KSParseNode( t_shiftright, yyvsp[-2].node, yyvsp[0].node );
          ;
    break;}
case 44:
#line 430 "yacc.yy"
{
            yyval.node = new KSParseNode( t_shiftleft, yyvsp[-2].node, yyvsp[0].node );
          ;
    break;}
case 45:
#line 438 "yacc.yy"
{
            yyval.node = yyvsp[0].node;
          ;
    break;}
case 46:
#line 442 "yacc.yy"
{
            yyval.node = new KSParseNode( t_plus_sign, yyvsp[-2].node, yyvsp[0].node );
          ;
    break;}
case 47:
#line 446 "yacc.yy"
{
            yyval.node = new KSParseNode( t_minus_sign, yyvsp[-2].node, yyvsp[0].node );
          ;
    break;}
case 48:
#line 454 "yacc.yy"
{
            yyval.node = yyvsp[0].node;
          ;
    break;}
case 49:
#line 458 "yacc.yy"
{
            yyval.node = new KSParseNode( t_asterik, yyvsp[-2].node, yyvsp[0].node );
          ;
    break;}
case 50:
#line 462 "yacc.yy"
{
            yyval.node = new KSParseNode( t_solidus, yyvsp[-2].node, yyvsp[0].node );
          ;
    break;}
case 51:
#line 466 "yacc.yy"
{
            yyval.node = new KSParseNode( t_percent_sign, yyvsp[-2].node, yyvsp[0].node );
          ;
    break;}
case 52:
#line 474 "yacc.yy"
{
            yyval.node = new KSParseNode( t_minus_sign, yyvsp[0].node );
          ;
    break;}
case 53:
#line 478 "yacc.yy"
{
            yyval.node = new KSParseNode( t_plus_sign, yyvsp[0].node );
          ;
    break;}
case 54:
#line 482 "yacc.yy"
{
            yyval.node = new KSParseNode( t_tilde, yyvsp[0].node );
          ;
    break;}
case 55:
#line 486 "yacc.yy"
{
            yyval.node = new KSParseNode( t_not, yyvsp[0].node );
          ;
    break;}
case 56:
#line 490 "yacc.yy"
{
            yyval.node = yyvsp[0].node;
          ;
    break;}
case 57:
#line 497 "yacc.yy"
{
            yyval.node = new KSParseNode( t_array, yyvsp[-3].node, yyvsp[-1].node );
          ;
    break;}
case 58:
#line 501 "yacc.yy"
{
            yyval.node = new KSParseNode( t_dict, yyvsp[-3].node, yyvsp[-1].node );
          ;
    break;}
case 59:
#line 505 "yacc.yy"
{
            yyval.node = new KSParseNode( t_func_call, yyvsp[-2].node );
          ;
    break;}
case 60:
#line 509 "yacc.yy"
{
            yyval.node = new KSParseNode( t_func_call, yyvsp[-3].node, yyvsp[-1].node );
          ;
    break;}
case 61:
#line 513 "yacc.yy"
{
            yyval.node = yyvsp[0].node;
          ;
    break;}
case 62:
#line 520 "yacc.yy"
{
            yyval.node = new KSParseNode( func_call_params, yyvsp[-2].node, yyvsp[0].node );
          ;
    break;}
case 63:
#line 525 "yacc.yy"
{
            yyval.node = new KSParseNode( func_call_params, yyvsp[0].node );
          ;
    break;}
case 64:
#line 532 "yacc.yy"
{
            yyval.node = new KSParseNode( member_expr, yyvsp[-2].node );
            yyval.node->setIdent( yyvsp[0].ident );
          ;
    break;}
case 65:
#line 537 "yacc.yy"
{
            yyval.node = yyvsp[0].node;
          ;
    break;}
case 66:
#line 544 "yacc.yy"
{
            yyval.node = new KSParseNode( t_incr, yyvsp[0].node );
          ;
    break;}
case 67:
#line 548 "yacc.yy"
{
            yyval.node = new KSParseNode( t_decr, yyvsp[0].node );
          ;
    break;}
case 68:
#line 552 "yacc.yy"
{
            /* Setting $1 twice indicates that this is a postfix operator */
            yyval.node = new KSParseNode( t_incr, yyvsp[-1].node, yyvsp[-1].node );
          ;
    break;}
case 69:
#line 557 "yacc.yy"
{
            /* Setting $1 twice indicates that this is a postfix operator */
            yyval.node = new KSParseNode( t_decr, yyvsp[-1].node, yyvsp[-1].node );
          ;
    break;}
case 70:
#line 562 "yacc.yy"
{
            yyval.node = yyvsp[0].node;
          ;
    break;}
case 71:
#line 569 "yacc.yy"
{
            yyval.node = new KSParseNode( t_match, yyvsp[-1].node );
            yyval.node->setIdent( yyvsp[0]._str );
          ;
    break;}
case 72:
#line 574 "yacc.yy"
{
            yyval.node = new KSParseNode( t_subst, yyvsp[-1].node );
            yyval.node->setIdent( yyvsp[0].ident );
          ;
    break;}
case 73:
#line 579 "yacc.yy"
{
            yyval.node = yyvsp[0].node;
          ;
    break;}
case 74:
#line 587 "yacc.yy"
{
            yyval.node = yyvsp[0].node;
          ;
    break;}
case 75:
#line 591 "yacc.yy"
{
            yyval.node = yyvsp[0].node;
          ;
    break;}
case 76:
#line 595 "yacc.yy"
{
            yyval.node = new KSParseNode( t_input );
          ;
    break;}
case 77:
#line 599 "yacc.yy"
{
            yyval.node = new KSParseNode( t_match_line );
            yyval.node->setIdent( yyvsp[0]._str );
          ;
    break;}
case 78:
#line 603 "yacc.yy"
{ ;
    break;}
case 79:
#line 605 "yacc.yy"
{
            yyval.node = yyvsp[-1].node;
          ;
    break;}
case 80:
#line 614 "yacc.yy"
{
            yyval.node = new KSParseNode( t_integer_literal );
            yyval.node->setIntegerLiteral( yyvsp[0]._int );
          ;
    break;}
case 81:
#line 619 "yacc.yy"
{
            yyval.node = new KSParseNode( t_cell );
            yyval.node->setStringLiteral( yyvsp[0]._str );
          ;
    break;}
case 82:
#line 624 "yacc.yy"
{
            yyval.node = new KSParseNode( t_range );
            yyval.node->setStringLiteral( yyvsp[0]._str );
          ;
    break;}
case 83:
#line 629 "yacc.yy"
{
            yyval.node = new KSParseNode( t_string_literal );
            yyval.node->setStringLiteral( yyvsp[0]._str );
          ;
    break;}
case 84:
#line 634 "yacc.yy"
{
            yyval.node = new KSParseNode( t_character_literal );
            yyval.node->setCharacterLiteral( yyvsp[0]._char );
          ;
    break;}
case 85:
#line 639 "yacc.yy"
{
            yyval.node = new KSParseNode( t_floating_pt_literal );
            yyval.node->setFloatingPtLiteral( yyvsp[0]._float );
          ;
    break;}
case 86:
#line 644 "yacc.yy"
{
            yyval.node = new KSParseNode( t_boolean_literal );
            yyval.node->setBooleanLiteral( true );
          ;
    break;}
case 87:
#line 649 "yacc.yy"
{
            yyval.node = new KSParseNode( t_boolean_literal );
            yyval.node->setBooleanLiteral( false );
          ;
    break;}
case 88:
#line 654 "yacc.yy"
{
            yyval.node = new KSParseNode( t_array_const );
          ;
    break;}
case 89:
#line 658 "yacc.yy"
{
            yyval.node = new KSParseNode( t_array_const, yyvsp[-1].node );
          ;
    break;}
case 90:
#line 662 "yacc.yy"
{
            yyval.node = new KSParseNode( t_dict_const );
          ;
    break;}
case 91:
#line 666 "yacc.yy"
{
            yyval.node = new KSParseNode( t_dict_const, yyvsp[-1].node );
          ;
    break;}
case 92:
#line 670 "yacc.yy"
{
            yyval.node = new KSParseNode( t_line );
          ;
    break;}
case 93:
#line 674 "yacc.yy"
{
            yyval.node = new KSParseNode( t_regexp_group );
            yyval.node->setIntegerLiteral( yyvsp[0]._int );
          ;
    break;}
case 94:
#line 682 "yacc.yy"
{
            yyval.node = new KSParseNode( t_array_element, yyvsp[-2].node, yyvsp[0].node );
          ;
    break;}
case 95:
#line 686 "yacc.yy"
{
            yyval.node = new KSParseNode( t_array_element, yyvsp[0].node  );
          ;
    break;}
case 96:
#line 693 "yacc.yy"
{
            yyval.node = new KSParseNode( t_dict_element, yyvsp[-5].node, yyvsp[-3].node, yyvsp[0].node );
          ;
    break;}
case 97:
#line 697 "yacc.yy"
{
            yyval.node = new KSParseNode( t_dict_element, yyvsp[-3].node, yyvsp[-1].node );
          ;
    break;}
case 98:
#line 707 "yacc.yy"
{
            yyval.node = new KSParseNode( t_struct, yyvsp[-1].node );
            yyval.node->setIdent( yyvsp[-3].ident );
          ;
    break;}
case 99:
#line 716 "yacc.yy"
{
            if ( yyvsp[0].node != 0 )
            {
                yyval.node = new KSParseNode( exports, yyvsp[0].node );
            }
          ;
    break;}
case 100:
#line 723 "yacc.yy"
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
case 101:
#line 746 "yacc.yy"
{
            yyval.node = yyvsp[-1].node;
          ;
    break;}
case 102:
#line 750 "yacc.yy"
{
            yyval.node = yyvsp[-1].node;
          ;
    break;}
case 103:
#line 754 "yacc.yy"
{
            yyval.node = yyvsp[-1].node;
          ;
    break;}
case 104:
#line 761 "yacc.yy"
{
            yyval.node = new KSParseNode( t_struct_members );
            yyval.node->setIdent( yyvsp[0].ident );
          ;
    break;}
case 105:
#line 766 "yacc.yy"
{
            yyval.node = new KSParseNode( t_struct_members, yyvsp[0].node );
            yyval.node->setIdent( yyvsp[-2].ident );
          ;
    break;}
case 106:
#line 774 "yacc.yy"
{
            yyval.node = new KSParseNode( func_dcl );
            yyval.node->setBranch( 2, yyvsp[0].node );
            yyval.node->setIdent( yyvsp[-4].ident );
          ;
    break;}
case 107:
#line 780 "yacc.yy"
{
            yyval.node = new KSParseNode( func_dcl, yyvsp[-3].node, yyvsp[0].node );
            yyval.node->setIdent( yyvsp[-5].ident );
          ;
    break;}
case 108:
#line 788 "yacc.yy"
{
            yyval.node = new KSParseNode( func_params, yyvsp[0].node );
          ;
    break;}
case 109:
#line 792 "yacc.yy"
{
            yyval.node = new KSParseNode( func_params, yyvsp[-2].node, yyvsp[0].node );
          ;
    break;}
case 110:
#line 799 "yacc.yy"
{
            yyval.node = new KSParseNode( func_param_in );
            yyval.node->setIdent( yyvsp[0].ident );
          ;
    break;}
case 111:
#line 804 "yacc.yy"
{
            yyval.node = new KSParseNode( func_param_in, yyvsp[0].node );
            yyval.node->setIdent( yyvsp[-2].ident );
          ;
    break;}
case 112:
#line 809 "yacc.yy"
{
            yyval.node = new KSParseNode( func_param_out );
            yyval.node->setIdent( yyvsp[0].ident );
          ;
    break;}
case 113:
#line 814 "yacc.yy"
{
            yyval.node = new KSParseNode( func_param_inout );
            yyval.node->setIdent( yyvsp[0].ident );
          ;
    break;}
case 114:
#line 822 "yacc.yy"
{
            yyval.node = NULL;
          ;
    break;}
case 115:
#line 826 "yacc.yy"
{
            yyval.node = yyvsp[0].node;
          ;
    break;}
case 116:
#line 833 "yacc.yy"
{
            yyval.node = new KSParseNode( func_lines, yyvsp[0].node );
          ;
    break;}
case 117:
#line 837 "yacc.yy"
{
            yyval.node = new KSParseNode( func_lines, yyvsp[-1].node );
            yyval.node->setBranch( 2, yyvsp[0].node );
          ;
    break;}
case 118:
#line 845 "yacc.yy"
{
            yyval.node = yyvsp[-1].node;
          ;
    break;}
case 119:
#line 849 "yacc.yy"
{
            yyval.node = new KSParseNode( t_raise, yyvsp[-3].node, yyvsp[-1].node );
          ;
    break;}
case 120:
#line 853 "yacc.yy"
{
            yyval.node = new KSParseNode( t_return );
          ;
    break;}
case 121:
#line 857 "yacc.yy"
{
            yyval.node = new KSParseNode( t_return, yyvsp[-1].node );
          ;
    break;}
case 122:
#line 861 "yacc.yy"
{
            yyval.node = yyvsp[0].node;
          ;
    break;}
case 123:
#line 865 "yacc.yy"
{
            yyval.node = new KSParseNode( t_try, yyvsp[-2].node, yyvsp[0].node );
          ;
    break;}
case 124:
#line 869 "yacc.yy"
{
            yyval.node = new KSParseNode( t_scope, yyvsp[-1].node );
          ;
    break;}
case 125:
#line 876 "yacc.yy"
{
            yyval.node = yyvsp[0].node;
          ;
    break;}
case 126:
#line 880 "yacc.yy"
{
            yyval.node = yyvsp[-1].node;
            yyval.node->setBranch( 4, yyvsp[0].node );
          ;
    break;}
case 127:
#line 889 "yacc.yy"
{
            yyval.node = new KSParseNode( t_catch, yyvsp[-6].node, yyvsp[-1].node );
            yyval.node->setIdent( yyvsp[-4].ident );
          ;
    break;}
case 128:
#line 895 "yacc.yy"
{
            KSParseNode* x = new KSParseNode( scoped_name );
            x->setIdent( yyvsp[-6].ident );
            yyval.node = new KSParseNode( t_catch_default, x, yyvsp[-1].node );
            yyval.node->setIdent( yyvsp[-4].ident );
          ;
    break;}
case 129:
#line 905 "yacc.yy"
{
            yyval.node = new KSParseNode( const_dcl, yyvsp[0].node );
            yyval.node->setIdent( yyvsp[-2].ident );
          ;
    break;}
case 130:
#line 913 "yacc.yy"
{
            yyval.node = new KSParseNode( t_while, yyvsp[-1].node, yyvsp[0].node );
          ;
    break;}
case 131:
#line 917 "yacc.yy"
{
            yyval.node = new KSParseNode( t_for, yyvsp[-6].node, yyvsp[-4].node, yyvsp[-2].node, yyvsp[0].node );
          ;
    break;}
case 132:
#line 921 "yacc.yy"
{
            yyval.node = new KSParseNode( t_do, yyvsp[-2].node, yyvsp[-1].node );
          ;
    break;}
case 133:
#line 925 "yacc.yy"
{
            yyval.node = new KSParseNode( t_if, yyvsp[-3].node, yyvsp[-1].node, yyvsp[0].node );
          ;
    break;}
case 134:
#line 929 "yacc.yy"
{
            yyval.node = new KSParseNode( t_foreach, yyvsp[-2].node, yyvsp[0].node );
            yyval.node->setIdent( yyvsp[-4].ident );
          ;
    break;}
case 135:
#line 934 "yacc.yy"
{
            /* We set $9 twice to indicate thet this is the foreach for maps */
            yyval.node = new KSParseNode( t_foreach, yyvsp[-2].node, yyvsp[0].node, yyvsp[0].node );
            yyval.node->setIdent( yyvsp[-6].ident );
            yyval.node->setStringLiteral( yyvsp[-4].ident );
          ;
    break;}
case 136:
#line 944 "yacc.yy"
{
            yyval.node = NULL;
          ;
    break;}
case 137:
#line 948 "yacc.yy"
{
            yyval.node = new KSParseNode( t_if, yyvsp[-3].node, yyvsp[-1].node, yyvsp[0].node );
          ;
    break;}
case 138:
#line 952 "yacc.yy"
{
            yyval.node = yyvsp[0].node;
          ;
    break;}
case 139:
#line 959 "yacc.yy"
{
            yyval.node = yyvsp[-1].node;
          ;
    break;}
case 140:
#line 966 "yacc.yy"
{
            yyval.node = new KSParseNode( t_scope, yyvsp[-1].node );
          ;
    break;}
}
   /* the action file gets copied in in place of this dollarsign */
#line 543 "/usr/share/bison.simple"

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
#line 970 "yacc.yy"


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
