
/*  A Bison parser, made from yacc.yy
 by  GNU Bison version 1.25
  */

#define YYBISON 1  /* Identify Bison output.  */

#define	T_FILE_OP	258
#define	T_MATCH_LINE	259
#define	T_LINE	260
#define	T_INPUT	261
#define	T_AMPERSAND	262
#define	T_ASTERISK	263
#define	T_CASE	264
#define	T_CHARACTER_LITERAL	265
#define	T_CIRCUMFLEX	266
#define	T_COLON	267
#define	T_COMMA	268
#define	T_CONST	269
#define	T_DEFAULT	270
#define	T_ENUM	271
#define	T_EQUAL	272
#define	T_FALSE	273
#define	T_FLOATING_PT_LITERAL	274
#define	T_GREATER_THAN_SIGN	275
#define	T_IDENTIFIER	276
#define	T_IN	277
#define	T_INOUT	278
#define	T_INTEGER_LITERAL	279
#define	T_LEFT_CURLY_BRACKET	280
#define	T_LEFT_PARANTHESIS	281
#define	T_LEFT_SQUARE_BRACKET	282
#define	T_LESS_THAN_SIGN	283
#define	T_MINUS_SIGN	284
#define	T_OUT	285
#define	T_PERCENT_SIGN	286
#define	T_PLUS_SIGN	287
#define	T_RIGHT_CURLY_BRACKET	288
#define	T_RIGHT_PARANTHESIS	289
#define	T_RIGHT_SQUARE_BRACKET	290
#define	T_SCOPE	291
#define	T_SEMICOLON	292
#define	T_SHIFTLEFT	293
#define	T_SHIFTRIGHT	294
#define	T_SOLIDUS	295
#define	T_STRING_LITERAL	296
#define	T_PRAGMA	297
#define	T_STRUCT	298
#define	T_SWITCH	299
#define	T_TILDE	300
#define	T_TRUE	301
#define	T_VERTICAL_LINE	302
#define	T_CLASS	303
#define	T_LESS_OR_EQUAL	304
#define	T_GREATER_OR_EQUAL	305
#define	T_ASSIGN	306
#define	T_NOTEQUAL	307
#define	T_MEMBER	308
#define	T_DELETE	309
#define	T_WHILE	310
#define	T_IF	311
#define	T_ELSE	312
#define	T_FOR	313
#define	T_DO	314
#define	T_INCR	315
#define	T_DECR	316
#define	T_MAIN	317
#define	T_FOREACH	318
#define	T_SUBST	319
#define	T_MATCH	320
#define	T_NOT	321
#define	T_RETURN	322
#define	T_SIGNAL	323
#define	T_EMIT	324
#define	T_IMPORT	325
#define	T_VAR	326
#define	T_UNKNOWN	327
#define	T_CATCH	328
#define	T_TRY	329
#define	T_RAISE	330
#define	T_RANGE	331
#define	T_CELL	332
#define	T_FROM	333
#define	T_PLUS_ASSIGN	334
#define	T_MINUS_ASSIGN	335
#define	T_AND	336
#define	T_OR	337
#define	T_DOLLAR	338

#line 1 "yacc.yy"


#include "kscript_parsenode.h"
#include "kscript_parser.h"
#include <stdlib.h>

#include <qstring.h>

extern int yylex();

extern const char* idl_lexFile;
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



#define	YYFINAL		358
#define	YYFLAG		-32768
#define	YYNTBASE	84

#define YYTRANSLATE(x) ((unsigned)(x) <= 338 ? yytranslate[x] : 137)

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
    76,    77,    78,    79,    80,    81,    82,    83
};

#if YYDEBUG != 0
static const short yyprhs[] = {     0,
     0,     1,     4,     6,     8,    11,    14,    17,    20,    23,
    27,    33,    39,    41,    43,    45,    49,    54,    62,    65,
    67,    71,    73,    77,    79,    82,    86,    90,    94,    96,
   100,   102,   106,   108,   112,   116,   120,   124,   128,   132,
   134,   136,   140,   142,   146,   148,   152,   154,   158,   162,
   164,   168,   172,   174,   178,   182,   186,   189,   192,   195,
   198,   200,   205,   210,   214,   219,   221,   225,   227,   231,
   233,   236,   239,   242,   245,   247,   250,   253,   255,   257,
   259,   261,   263,   267,   271,   275,   277,   279,   281,   283,
   285,   287,   289,   291,   294,   298,   301,   305,   307,   310,
   314,   316,   324,   330,   336,   338,   341,   344,   348,   351,
   353,   357,   362,   365,   369,   370,   372,   374,   377,   380,
   383,   390,   396,   405,   408,   414,   421,   423,   427,   430,
   435,   438,   441,   443,   447,   450,   455,   456,   458,   460,
   463,   466,   472,   476,   479,   483,   485,   491,   495,   497,
   500,   510,   520,   525,   528,   538,   543,   550,   558,   568,
   569,   577,   580,   585
};

static const short yyrhs[] = {    -1,
    51,    96,     0,    85,     0,    86,     0,    86,    85,     0,
   132,    37,     0,   113,    37,     0,   117,    37,     0,   122,
    33,     0,    70,    21,    37,     0,    78,    21,    70,     8,
    37,     0,    78,    21,    70,    87,    37,     0,    42,     0,
    88,     0,    21,     0,    21,    13,    87,     0,    62,    25,
   127,    33,     0,    62,    26,   123,    34,    25,   127,    33,
     0,    12,    90,     0,    91,     0,    91,    13,    90,     0,
    92,     0,    91,    53,    21,     0,    21,     0,    36,    21,
     0,    94,    51,    93,     0,    94,    79,    93,     0,    94,
    80,    93,     0,    94,     0,    95,    82,    94,     0,    95,
     0,    96,    81,    95,     0,    96,     0,    97,    17,    96,
     0,    97,    52,    96,     0,    97,    49,    96,     0,    97,
    50,    96,     0,    97,    28,    96,     0,    97,    20,    96,
     0,    97,     0,    98,     0,    97,    47,    98,     0,    99,
     0,    98,    11,    99,     0,   100,     0,    99,     7,   100,
     0,   101,     0,   100,    39,   101,     0,   100,    38,   101,
     0,   102,     0,   101,    32,   102,     0,   101,    29,   102,
     0,   103,     0,   102,     8,   103,     0,   102,    40,   103,
     0,   102,    31,   103,     0,    29,   104,     0,    32,   104,
     0,    45,   104,     0,    66,   104,     0,   104,     0,   104,
    27,    93,    35,     0,   104,    25,    93,    33,     0,   104,
    26,    34,     0,   104,    26,   105,    34,     0,   106,     0,
    93,    13,   105,     0,    93,     0,   104,    53,    21,     0,
   107,     0,    60,   108,     0,    61,   108,     0,   108,    60,
     0,   108,    61,     0,   108,     0,   109,    65,     0,   109,
    64,     0,   109,     0,    92,     0,   110,     0,     6,     0,
     4,     0,     3,    93,    34,     0,    28,    97,    20,     0,
    26,    93,    34,     0,    24,     0,    77,     0,    76,     0,
    41,     0,    10,     0,    19,     0,    46,     0,    18,     0,
    27,    35,     0,    27,   111,    35,     0,    25,    33,     0,
    25,   112,    33,     0,     5,     0,    83,    24,     0,    93,
    13,   111,     0,    93,     0,    26,    93,    13,    96,    34,
    13,   112,     0,    26,    93,    13,    96,    34,     0,    43,
    21,    25,   114,    33,     0,   115,     0,   115,   114,     0,
   122,    33,     0,    71,   116,    37,     0,   132,    37,     0,
    21,     0,    21,    13,   116,     0,   118,    25,   119,    33,
     0,    48,    21,     0,    48,    21,    89,     0,     0,   120,
     0,   121,     0,   121,   120,     0,   122,    33,     0,   113,
    37,     0,    68,    21,    26,   125,    34,    37,     0,    68,
    21,    26,    34,    37,     0,    54,    26,    22,    21,    34,
    25,   127,    33,     0,   132,    37,     0,    21,    26,    34,
    25,   127,     0,    21,    26,   123,    34,    25,   127,     0,
   124,     0,   124,    13,   123,     0,    22,    21,     0,    22,
    21,    51,    93,     0,    30,    21,     0,    23,    21,     0,
   126,     0,   126,    13,   123,     0,    22,    21,     0,    22,
    21,    51,    93,     0,     0,   128,     0,   129,     0,   129,
   128,     0,    93,    37,     0,    75,    93,    13,    93,    37,
     0,    69,    93,    37,     0,    67,    37,     0,    67,    93,
    37,     0,   133,     0,    74,    25,   128,    33,   130,     0,
    25,   128,    33,     0,   131,     0,   131,   130,     0,    73,
    26,    93,    13,    21,    34,    25,   128,    33,     0,    15,
    26,    21,    13,    21,    34,    25,   128,    33,     0,    14,
    21,    51,    96,     0,   135,   136,     0,    58,    26,    93,
    37,    93,    37,    93,    34,   136,     0,    59,   136,   135,
    37,     0,    56,    26,    93,    34,   136,   134,     0,    63,
    26,    21,    13,    93,    34,   136,     0,    63,    26,    21,
    13,    21,    13,    93,    34,   136,     0,     0,    57,    56,
    26,    93,    34,   136,   134,     0,    57,   136,     0,    55,
    26,    93,    34,     0,    25,   127,    33,     0
};

#endif

#if YYDEBUG != 0
static const short yyrline[] = { 0,
   173,   177,   181,   188,   192,   201,   205,   209,   213,   217,
   222,   228,   234,   239,   246,   250,   259,   265,   273,   281,
   285,   294,   298,   306,   311,   323,   327,   331,   335,   342,
   346,   353,   357,   364,   368,   372,   376,   380,   384,   388,
   396,   400,   408,   412,   420,   424,   432,   436,   440,   448,
   452,   456,   464,   468,   472,   476,   484,   488,   492,   496,
   500,   507,   511,   515,   519,   523,   530,   535,   542,   547,
   554,   558,   562,   567,   572,   579,   584,   589,   597,   601,
   605,   609,   614,   619,   620,   629,   634,   639,   644,   649,
   654,   659,   664,   669,   673,   677,   681,   685,   689,   697,
   701,   708,   712,   722,   730,   734,   742,   746,   750,   757,
   762,   771,   779,   784,   794,   798,   805,   809,   817,   821,
   825,   830,   835,   844,   851,   857,   865,   869,   876,   881,
   886,   891,   899,   903,   910,   915,   923,   927,   934,   938,
   946,   950,   954,   958,   962,   966,   970,   974,   981,   985,
   993,   999,  1010,  1018,  1022,  1026,  1030,  1034,  1039,  1049,
  1053,  1057,  1064,  1071
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
"T_RANGE","T_CELL","T_FROM","T_PLUS_ASSIGN","T_MINUS_ASSIGN","T_AND","T_OR",
"T_DOLLAR","specification","definitions","definition","import_list","main","inheritance_spec",
"qualified_names","qualified_name","scoped_name","assign_expr","bool_or","bool_and",
"equal_expr","or_expr","xor_expr","and_expr","shift_expr","add_expr","mult_expr",
"unary_expr","index_expr","func_call_params","member_expr","incr_expr","match_expr",
"primary_expr","literal","array_elements","dict_elements","struct_dcl","struct_exports",
"struct_export","struct_members","class_dcl","class_header","class_body","class_exports",
"class_export","func_dcl","func_params","func_param","signal_params","signal_param",
"func_body","func_lines","func_line","catches","single_catch","const_dcl","loops",
"else","while","loop_body", NULL
};
#endif

static const short yyr1[] = {     0,
    84,    84,    84,    85,    85,    86,    86,    86,    86,    86,
    86,    86,    86,    86,    87,    87,    88,    88,    89,    90,
    90,    91,    91,    92,    92,    93,    93,    93,    93,    94,
    94,    95,    95,    96,    96,    96,    96,    96,    96,    96,
    97,    97,    98,    98,    99,    99,   100,   100,   100,   101,
   101,   101,   102,   102,   102,   102,   103,   103,   103,   103,
   103,   104,   104,   104,   104,   104,   105,   105,   106,   106,
   107,   107,   107,   107,   107,   108,   108,   108,   109,   109,
   109,   109,   109,   109,   109,   110,   110,   110,   110,   110,
   110,   110,   110,   110,   110,   110,   110,   110,   110,   111,
   111,   112,   112,   113,   114,   114,   115,   115,   115,   116,
   116,   117,   118,   118,   119,   119,   120,   120,   121,   121,
   121,   121,   121,   121,   122,   122,   123,   123,   124,   124,
   124,   124,   125,   125,   126,   126,   127,   127,   128,   128,
   129,   129,   129,   129,   129,   129,   129,   129,   130,   130,
   131,   131,   132,   133,   133,   133,   133,   133,   133,   134,
   134,   134,   135,   136
};

static const short yyr2[] = {     0,
     0,     2,     1,     1,     2,     2,     2,     2,     2,     3,
     5,     5,     1,     1,     1,     3,     4,     7,     2,     1,
     3,     1,     3,     1,     2,     3,     3,     3,     1,     3,
     1,     3,     1,     3,     3,     3,     3,     3,     3,     1,
     1,     3,     1,     3,     1,     3,     1,     3,     3,     1,
     3,     3,     1,     3,     3,     3,     2,     2,     2,     2,
     1,     4,     4,     3,     4,     1,     3,     1,     3,     1,
     2,     2,     2,     2,     1,     2,     2,     1,     1,     1,
     1,     1,     3,     3,     3,     1,     1,     1,     1,     1,
     1,     1,     1,     2,     3,     2,     3,     1,     2,     3,
     1,     7,     5,     5,     1,     2,     2,     3,     2,     1,
     3,     4,     2,     3,     0,     1,     1,     2,     2,     2,
     6,     5,     8,     2,     5,     6,     1,     3,     2,     4,
     2,     2,     1,     3,     2,     4,     0,     1,     1,     2,
     2,     5,     3,     2,     3,     1,     5,     3,     1,     2,
     9,     9,     4,     2,     9,     4,     6,     7,     9,     0,
     7,     2,     4,     3
};

static const short yydefact[] = {     1,
     0,     0,    13,     0,     0,     0,     0,     0,     0,     3,
     4,    14,     0,     0,     0,     0,     0,     0,     0,     0,
   113,     0,    82,    98,    81,    90,    93,    91,    24,    86,
     0,     0,     0,     0,     0,     0,     0,    89,     0,    92,
     0,     0,     0,    88,    87,     0,    79,     2,    40,    41,
    43,    45,    47,    50,    53,    61,    66,    70,    75,    78,
    80,   137,     0,     0,     0,     5,     7,     8,   115,     9,
     6,     0,     0,     0,     0,     0,     0,   127,     0,     0,
   114,     0,    29,    31,    33,     0,    96,     0,     0,    94,
   101,     0,     0,    57,    58,    25,    59,    71,    72,    60,
    99,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,    73,    74,    77,    76,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,   138,   139,   146,
     0,     0,    10,     0,     0,     0,     0,     0,   116,   117,
     0,     0,   153,   129,   132,   131,   137,     0,     0,     0,
     0,   105,     0,     0,    19,    20,    22,    83,     0,     0,
     0,     0,     0,     0,    97,    85,     0,    95,    84,    34,
    39,    38,    42,    36,    37,    35,    44,    46,    49,    48,
    52,    51,    54,    56,    55,     0,    64,    68,     0,     0,
    69,     0,     0,     0,     0,     0,   137,     0,     0,   144,
     0,     0,     0,     0,   141,    17,   140,   154,     0,     0,
    15,     0,     0,     0,   120,   112,   118,   119,   124,     0,
   125,   137,   128,   110,     0,   104,   106,   107,   109,     0,
     0,    26,    27,    28,    30,    32,     0,   100,    63,     0,
    65,    62,     0,   148,     0,     0,     0,     0,     0,     0,
   145,   143,     0,     0,   137,    11,     0,    12,     0,     0,
   130,   126,     0,   108,    21,    23,     0,    67,   163,     0,
     0,   164,   156,     0,     0,     0,     0,    16,     0,     0,
     0,     0,   133,   111,   103,   160,     0,    24,     0,     0,
     0,   147,   149,   142,    18,     0,   135,   122,     0,     0,
     0,     0,   157,     0,     0,     0,     0,     0,   150,   137,
     0,   121,   134,   102,     0,   162,     0,     0,   158,     0,
     0,     0,   136,     0,     0,     0,     0,     0,   123,     0,
   155,   159,     0,     0,     0,     0,     0,   160,     0,     0,
   161,     0,     0,   152,   151,     0,     0,     0
};

static const short yydefgoto[] = {   356,
    10,    11,   222,    12,    81,   165,   166,    47,   136,    83,
    84,    85,    49,    50,    51,    52,    53,    54,    55,    56,
   199,    57,    58,    59,    60,    61,    92,    88,    13,   161,
   162,   235,    14,    15,   148,   149,   150,    16,    77,    78,
   292,   293,   137,   138,   139,   302,   303,    17,   140,   313,
   141,   208
};

static const short yypact[] = {    -5,
    18,   -11,-32768,    28,    42,   532,    91,    47,    51,-32768,
    -1,-32768,   -12,    16,    49,    43,    45,    57,    37,    69,
   100,   532,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
    94,   532,   372,   532,   118,   118,   104,-32768,   118,-32768,
   622,   622,   118,-32768,-32768,   114,-32768,-32768,   140,   129,
   149,    92,    97,     4,-32768,    65,-32768,-32768,    72,    70,
-32768,   297,    63,   121,    93,-32768,-32768,-32768,    41,-32768,
-32768,   532,   145,   146,   150,   147,   136,   161,     7,    12,
-32768,   141,   -28,    99,    95,   532,-32768,   152,   154,-32768,
   173,   156,   -13,    65,    65,-32768,    65,-32768,-32768,    65,
-32768,   532,   532,   532,   532,   532,   532,   532,   532,   532,
   532,   532,   532,   532,   532,   532,   532,   532,   418,   532,
   172,-32768,-32768,-32768,-32768,   208,   174,   176,   177,   179,
   181,   486,   532,   180,   532,   162,   165,-32768,   297,-32768,
   179,   175,-32768,    19,   184,   194,   182,   183,-32768,    41,
   187,   185,-32768,   166,-32768,-32768,   297,   196,    63,   202,
   197,     7,   198,   201,-32768,     5,-32768,-32768,   532,   532,
   532,   532,   532,   226,-32768,-32768,   532,-32768,-32768,-32768,
-32768,-32768,   129,-32768,-32768,-32768,   149,    92,    97,    97,
     4,     4,-32768,-32768,-32768,   210,-32768,   233,   213,   216,
-32768,   532,   215,   532,   532,   532,   297,   200,   231,-32768,
   219,   220,   297,   245,-32768,-32768,-32768,-32768,   235,   224,
   252,   236,   248,   246,-32768,-32768,-32768,-32768,-32768,   532,
-32768,   297,-32768,   263,   241,-32768,-32768,-32768,-32768,    12,
   258,-32768,-32768,-32768,-32768,-32768,   532,-32768,-32768,   532,
-32768,-32768,    11,-32768,   247,   253,   243,   255,   249,   276,
-32768,-32768,   257,   532,   297,-32768,   273,-32768,   274,    32,
-32768,-32768,   202,-32768,-32768,-32768,   264,-32768,-32768,   179,
   532,-32768,-32768,   578,     2,   260,   271,-32768,   272,   284,
   277,   275,   295,-32768,   298,   256,   280,   306,   286,   301,
   302,-32768,     2,-32768,-32768,   285,   279,-32768,   294,    63,
   308,    -6,-32768,   532,   532,   179,   311,   532,-32768,   297,
   532,-32768,-32768,-32768,   309,-32768,   303,   305,-32768,   323,
   327,   312,-32768,   532,   179,   179,   320,   325,-32768,   310,
-32768,-32768,   313,   314,   179,   324,   326,   256,   297,   297,
-32768,   317,   321,-32768,-32768,   359,   361,-32768
};

static const short yypgoto[] = {-32768,
   351,-32768,    98,-32768,-32768,   127,-32768,   -75,   -22,   207,
   195,    -2,   335,   265,   278,   282,    39,    48,   -86,    44,
   131,-32768,-32768,   111,-32768,-32768,   206,    73,   -61,   223,
-32768,   113,-32768,-32768,-32768,   238,-32768,   -47,   -60,-32768,
-32768,-32768,  -151,  -125,-32768,    86,-32768,   -43,-32768,    46,
   203,  -139
};


#define	YYLAST		705


static const short yytable[] = {    82,
   203,   218,   142,    48,   167,   231,   179,   147,     1,    89,
    91,   115,     1,   217,    19,     2,   300,   240,   207,     2,
     1,   151,   169,   247,    67,   152,   220,     2,   193,   194,
   195,   163,    29,   105,   116,   164,     3,     4,    18,   221,
     3,     4,     5,   117,   176,     6,     5,    37,    20,   325,
   170,   171,    68,   290,     1,   258,     7,   241,    73,    74,
     7,     2,    21,   174,     8,   291,    75,    64,     8,   153,
    76,    65,     9,    69,   301,    70,     9,   160,    94,    95,
   272,    71,    97,     4,    73,    74,   100,   263,   147,   118,
   119,   120,    75,    79,   145,   196,   198,   200,   233,   180,
   181,   182,   151,   184,   185,   186,   152,    72,   146,   211,
   212,    80,   214,   287,   163,    62,    63,   121,   164,    86,
    22,    23,    24,    25,    96,   113,    87,    26,   114,   111,
   112,   122,   123,   124,   125,    27,    28,   101,    29,   109,
   296,    30,    31,    32,    33,    34,   242,   243,   244,   189,
   190,    98,    99,    37,    91,   110,   102,   143,    38,   103,
   191,   192,   144,    40,   167,   154,   155,   104,   332,   158,
   156,   157,   326,   159,   168,   173,   329,    41,    42,   253,
   172,   255,   256,   257,   175,   177,   105,   176,   106,   107,
   178,   108,   201,    44,    45,   341,   342,   216,   215,   204,
    46,   205,   206,   207,   213,   348,   209,   271,   219,   223,
    22,    23,    24,    25,   224,   226,   230,    26,   225,   228,
   232,   229,   234,   352,   353,    27,    28,   198,    29,   236,
   238,    30,   126,   202,    33,    34,    35,   239,   247,    36,
    87,   286,   249,    37,   277,   250,   251,   254,    38,   323,
   252,   260,    39,    40,   127,   261,   262,   264,   297,   265,
   266,   299,   127,   128,   267,   129,   130,    41,    42,   269,
   131,   270,   268,    43,   132,   273,   133,   274,   276,   281,
   279,   134,   135,    44,    45,   283,   280,   282,   284,   285,
    46,   327,   328,   221,   289,   331,   304,   295,   333,    22,
    23,    24,    25,   305,   307,   306,    26,   310,   309,   320,
   311,   340,   312,   308,    27,    28,   314,    29,   315,   316,
    30,   126,    32,    33,    34,    35,   317,   318,    36,   321,
   322,   330,    37,    86,   334,   337,   335,    38,   336,   338,
   343,    39,    40,   345,   339,   344,   346,   347,   349,   354,
   350,   127,   128,   355,   129,   130,    41,    42,   357,   131,
   358,    66,    43,   132,   288,   133,   275,   246,    93,   183,
   134,   135,    44,    45,    22,    23,    24,    25,   245,    46,
   278,    26,   248,   324,   237,   294,   187,   227,   319,    27,
    28,   188,    29,   351,     0,    30,    31,    32,    33,    34,
    35,     0,     0,    36,     0,     0,    90,    37,     0,     0,
   259,     0,    38,     0,     0,     0,    39,    40,     0,     0,
    22,    23,    24,    25,     0,     0,     0,    26,     0,     0,
     0,    41,    42,     0,     0,    27,    28,    43,    29,     0,
     0,    30,    31,    32,    33,    34,    35,    44,    45,    36,
     0,   197,     0,    37,    46,     0,     0,     0,    38,     0,
     0,     0,    39,    40,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,    41,    42,     0,
     0,     0,     0,    43,     0,     0,     0,     0,    22,    23,
    24,    25,     0,    44,    45,    26,     0,     0,     0,     0,
    46,     0,     0,    27,    28,     0,    29,     0,     0,    30,
    31,    32,    33,    34,    35,     0,     0,    36,     0,     0,
     0,    37,   210,     0,     0,     0,    38,     0,     0,     0,
    39,    40,     0,     0,    22,    23,    24,    25,     0,     0,
     0,    26,     0,     0,     0,    41,    42,     0,     0,    27,
    28,    43,    29,     0,     0,    30,    31,    32,    33,    34,
    35,    44,    45,    36,     0,     0,     0,    37,    46,     0,
     0,     0,    38,     0,     0,     0,    39,    40,     0,     0,
    22,    23,    24,    25,     0,     0,     0,    26,     0,     0,
     0,    41,    42,     0,     0,    27,    28,    43,   298,     0,
     0,    30,    31,    32,    33,    34,    35,    44,    45,    36,
     0,     0,     0,    37,    46,     0,     0,     0,    38,     0,
     0,     0,    39,    40,    22,    23,    24,    25,     0,     0,
     0,    26,     0,     0,     0,     0,     0,    41,    42,    27,
    28,     0,    29,    43,     0,    30,    31,    32,    33,    34,
     0,     0,     0,    44,    45,     0,     0,    37,     0,     0,
    46,     0,    38,     0,     0,     0,     0,    40,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,    44,    45,     0,
     0,     0,     0,     0,    46
};

static const short yycheck[] = {    22,
   126,   141,    63,     6,    80,   157,    20,    69,    14,    32,
    33,     8,    14,   139,    26,    21,    15,    13,    25,    21,
    14,    69,    51,    13,    37,    69,     8,    21,   115,   116,
   117,    79,    21,    47,    31,    79,    42,    43,    21,    21,
    42,    43,    48,    40,    34,    51,    48,    36,    21,    56,
    79,    80,    37,    22,    14,   207,    62,    53,    22,    23,
    62,    21,    21,    86,    70,    34,    30,    21,    70,    72,
    34,    21,    78,    25,    73,    33,    78,    71,    35,    36,
   232,    37,    39,    43,    22,    23,    43,   213,   150,    25,
    26,    27,    30,    25,    54,   118,   119,   120,   159,   102,
   103,   104,   150,   106,   107,   108,   150,    51,    68,   132,
   133,    12,   135,   265,   162,    25,    26,    53,   162,    26,
     3,     4,     5,     6,    21,    29,    33,    10,    32,    38,
    39,    60,    61,    64,    65,    18,    19,    24,    21,    11,
   280,    24,    25,    26,    27,    28,   169,   170,   171,   111,
   112,    41,    42,    36,   177,     7,    17,    37,    41,    20,
   113,   114,    70,    46,   240,    21,    21,    28,   320,    34,
    21,    25,   312,    13,    34,    81,   316,    60,    61,   202,
    82,   204,   205,   206,    33,    13,    47,    34,    49,    50,
    35,    52,    21,    76,    77,   335,   336,    33,    37,    26,
    83,    26,    26,    25,    25,   345,    26,   230,    34,    26,
     3,     4,     5,     6,    21,    33,    51,    10,    37,    33,
    25,    37,    21,   349,   350,    18,    19,   250,    21,    33,
    33,    24,    25,    26,    27,    28,    29,    37,    13,    32,
    33,   264,    33,    36,   247,    13,    34,    33,    41,   310,
    35,    21,    45,    46,    55,    37,    37,    13,   281,    25,
    37,   284,    55,    56,    13,    58,    59,    60,    61,    22,
    63,    26,    37,    66,    67,    13,    69,    37,    21,    37,
    34,    74,    75,    76,    77,    37,    34,    33,    13,    33,
    83,   314,   315,    21,    21,   318,    37,    34,   321,     3,
     4,     5,     6,    33,    21,    34,    10,    13,    34,    25,
    13,   334,    57,    37,    18,    19,    37,    21,    13,    34,
    24,    25,    26,    27,    28,    29,    26,    26,    32,    51,
    37,    21,    36,    26,    26,    13,    34,    41,    34,    13,
    21,    45,    46,    34,    33,    21,    34,    34,    25,    33,
    25,    55,    56,    33,    58,    59,    60,    61,     0,    63,
     0,    11,    66,    67,   267,    69,   240,   173,    34,   105,
    74,    75,    76,    77,     3,     4,     5,     6,   172,    83,
   250,    10,   177,   311,   162,   273,   109,   150,   303,    18,
    19,   110,    21,   348,    -1,    24,    25,    26,    27,    28,
    29,    -1,    -1,    32,    -1,    -1,    35,    36,    -1,    -1,
   208,    -1,    41,    -1,    -1,    -1,    45,    46,    -1,    -1,
     3,     4,     5,     6,    -1,    -1,    -1,    10,    -1,    -1,
    -1,    60,    61,    -1,    -1,    18,    19,    66,    21,    -1,
    -1,    24,    25,    26,    27,    28,    29,    76,    77,    32,
    -1,    34,    -1,    36,    83,    -1,    -1,    -1,    41,    -1,
    -1,    -1,    45,    46,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    60,    61,    -1,
    -1,    -1,    -1,    66,    -1,    -1,    -1,    -1,     3,     4,
     5,     6,    -1,    76,    77,    10,    -1,    -1,    -1,    -1,
    83,    -1,    -1,    18,    19,    -1,    21,    -1,    -1,    24,
    25,    26,    27,    28,    29,    -1,    -1,    32,    -1,    -1,
    -1,    36,    37,    -1,    -1,    -1,    41,    -1,    -1,    -1,
    45,    46,    -1,    -1,     3,     4,     5,     6,    -1,    -1,
    -1,    10,    -1,    -1,    -1,    60,    61,    -1,    -1,    18,
    19,    66,    21,    -1,    -1,    24,    25,    26,    27,    28,
    29,    76,    77,    32,    -1,    -1,    -1,    36,    83,    -1,
    -1,    -1,    41,    -1,    -1,    -1,    45,    46,    -1,    -1,
     3,     4,     5,     6,    -1,    -1,    -1,    10,    -1,    -1,
    -1,    60,    61,    -1,    -1,    18,    19,    66,    21,    -1,
    -1,    24,    25,    26,    27,    28,    29,    76,    77,    32,
    -1,    -1,    -1,    36,    83,    -1,    -1,    -1,    41,    -1,
    -1,    -1,    45,    46,     3,     4,     5,     6,    -1,    -1,
    -1,    10,    -1,    -1,    -1,    -1,    -1,    60,    61,    18,
    19,    -1,    21,    66,    -1,    24,    25,    26,    27,    28,
    -1,    -1,    -1,    76,    77,    -1,    -1,    36,    -1,    -1,
    83,    -1,    41,    -1,    -1,    -1,    -1,    46,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    76,    77,    -1,
    -1,    -1,    -1,    -1,    83
};
/* -*-C-*-  Note some compilers choke on comments on `#line' lines.  */
#line 3 "/usr/share/bison.simple"

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

#ifndef YYPARSE_RETURN_TYPE
#define YYPARSE_RETURN_TYPE int
#endif

/* Prevent warning if -Wstrict-prototypes.  */
#ifdef __GNUC__
YYPARSE_RETURN_TYPE yyparse (void);
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

#line 196 "/usr/share/bison.simple"

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

YYPARSE_RETURN_TYPE
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
#line 174 "yacc.yy"
{
	    theParser->setRootNode( NULL );
	  ;
    break;}
case 2:
#line 178 "yacc.yy"
{
	    theParser->setRootNode( yyvsp[0].node );
	  ;
    break;}
case 3:
#line 182 "yacc.yy"
{
	    theParser->setRootNode( new KSParseNode( definitions, yyvsp[0].node ) );
	  ;
    break;}
case 4:
#line 189 "yacc.yy"
{
	    yyval.node = new KSParseNode( definitions, yyvsp[0].node );
	  ;
    break;}
case 5:
#line 193 "yacc.yy"
{
	    yyval.node = new KSParseNode( definitions, yyvsp[-1].node );
	    yyval.node->setBranch( 2, yyvsp[0].node );
	  ;
    break;}
case 6:
#line 202 "yacc.yy"
{
	    yyval.node = yyvsp[-1].node;
	  ;
    break;}
case 7:
#line 206 "yacc.yy"
{
	    yyval.node = yyvsp[-1].node;
	  ;
    break;}
case 8:
#line 210 "yacc.yy"
{
	    yyval.node = yyvsp[-1].node;
	  ;
    break;}
case 9:
#line 214 "yacc.yy"
{
	    yyval.node = yyvsp[-1].node;
	  ;
    break;}
case 10:
#line 218 "yacc.yy"
{
	    yyval.node = new KSParseNode( import );
	    yyval.node->setIdent( yyvsp[-1].ident );
	  ;
    break;}
case 11:
#line 223 "yacc.yy"
{
	    yyval.node = new KSParseNode( from );
	    yyval.node->setIdent( yyvsp[-3].ident );
	    yyval.node->setStringLiteral( QString( "" ) );
	  ;
    break;}
case 12:
#line 229 "yacc.yy"
{
	    yyval.node = new KSParseNode( from );
	    yyval.node->setIdent( yyvsp[-3].ident );
	    yyval.node->setStringLiteral( yyvsp[-1]._str );
	  ;
    break;}
case 13:
#line 235 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_pragma );
	    yyval.node->setIdent( yyvsp[0]._str );
	  ;
    break;}
case 14:
#line 240 "yacc.yy"
{
	    yyval.node = yyvsp[0].node;
	  ;
    break;}
case 15:
#line 247 "yacc.yy"
{
		yyval._str = yyvsp[0].ident;
	  ;
    break;}
case 16:
#line 251 "yacc.yy"
{
		(*yyvsp[-2].ident) += "/";
		(*yyvsp[-2].ident) += (*yyvsp[0]._str);
		yyval._str = yyvsp[-2].ident;
	  ;
    break;}
case 17:
#line 260 "yacc.yy"
{
	    yyval.node = new KSParseNode( func_dcl );
	    yyval.node->setBranch( 2, yyvsp[-1].node );
	    yyval.node->setIdent( "main" );
	  ;
    break;}
case 18:
#line 266 "yacc.yy"
{
	    yyval.node = new KSParseNode( func_dcl, yyvsp[-4].node, yyvsp[-1].node );
	    yyval.node->setIdent( "main" );
	  ;
    break;}
case 19:
#line 274 "yacc.yy"
{
	    yyval.node = yyvsp[0].node;
	  ;
    break;}
case 20:
#line 282 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_qualified_names, yyvsp[0].node );
	  ;
    break;}
case 21:
#line 286 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_qualified_names, yyvsp[-2].node );
	    yyval.node->setBranch( 2, yyvsp[0].node );
	  ;
    break;}
case 22:
#line 295 "yacc.yy"
{
	    yyval.node = yyvsp[0].node;
	  ;
    break;}
case 23:
#line 299 "yacc.yy"
{
	    yyval.node = new KSParseNode( member_expr, yyvsp[-2].node );
	    yyval.node->setIdent( yyvsp[0].ident );
	  ;
    break;}
case 24:
#line 307 "yacc.yy"
{
	    yyval.node = new KSParseNode( scoped_name );
	    yyval.node->setIdent( yyvsp[0].ident );
	  ;
    break;}
case 25:
#line 312 "yacc.yy"
{
	    yyval.node = new KSParseNode( scoped_name );
	    QString name = "::";
	    name += *(yyvsp[0].ident);
	    delete yyvsp[0].ident;
	    yyval.node->setIdent( name );
	  ;
    break;}
case 26:
#line 324 "yacc.yy"
{
	    yyval.node = new KSParseNode( assign_expr, yyvsp[-2].node, yyvsp[0].node );
	  ;
    break;}
case 27:
#line 328 "yacc.yy"
{
	    yyval.node = new KSParseNode( plus_assign, yyvsp[-2].node, yyvsp[0].node );
	  ;
    break;}
case 28:
#line 332 "yacc.yy"
{
	    yyval.node = new KSParseNode( minus_assign, yyvsp[-2].node, yyvsp[0].node );
	  ;
    break;}
case 29:
#line 336 "yacc.yy"
{
	    yyval.node = yyvsp[0].node;
	  ;
    break;}
case 30:
#line 343 "yacc.yy"
{
	    yyval.node = new KSParseNode( bool_or, yyvsp[-2].node, yyvsp[0].node );
	  ;
    break;}
case 31:
#line 347 "yacc.yy"
{
	    yyval.node = yyvsp[0].node;
	  ;
    break;}
case 32:
#line 354 "yacc.yy"
{
	    yyval.node = new KSParseNode( bool_and, yyvsp[-2].node, yyvsp[0].node );
	  ;
    break;}
case 33:
#line 358 "yacc.yy"
{
	    yyval.node = yyvsp[0].node;
	  ;
    break;}
case 34:
#line 365 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_equal, yyvsp[-2].node, yyvsp[0].node );
	  ;
    break;}
case 35:
#line 369 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_notequal, yyvsp[-2].node, yyvsp[0].node );
	  ;
    break;}
case 36:
#line 373 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_less_or_equal, yyvsp[-2].node, yyvsp[0].node );
	  ;
    break;}
case 37:
#line 377 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_greater_or_equal, yyvsp[-2].node, yyvsp[0].node );
	  ;
    break;}
case 38:
#line 381 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_less, yyvsp[-2].node, yyvsp[0].node );
	  ;
    break;}
case 39:
#line 385 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_greater, yyvsp[-2].node, yyvsp[0].node );
	  ;
    break;}
case 40:
#line 389 "yacc.yy"
{
	    yyval.node = yyvsp[0].node;
	  ;
    break;}
case 41:
#line 397 "yacc.yy"
{
	    yyval.node = yyvsp[0].node;
	  ;
    break;}
case 42:
#line 401 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_vertical_line, yyvsp[-2].node, yyvsp[0].node );
	  ;
    break;}
case 43:
#line 409 "yacc.yy"
{
	    yyval.node = yyvsp[0].node;
	  ;
    break;}
case 44:
#line 413 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_circumflex, yyvsp[-2].node, yyvsp[0].node );
	  ;
    break;}
case 45:
#line 421 "yacc.yy"
{
	    yyval.node = yyvsp[0].node;
	  ;
    break;}
case 46:
#line 425 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_ampersand, yyvsp[-2].node, yyvsp[0].node );
	  ;
    break;}
case 47:
#line 433 "yacc.yy"
{
	    yyval.node = yyvsp[0].node;
	  ;
    break;}
case 48:
#line 437 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_shiftright, yyvsp[-2].node, yyvsp[0].node );
	  ;
    break;}
case 49:
#line 441 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_shiftleft, yyvsp[-2].node, yyvsp[0].node );
	  ;
    break;}
case 50:
#line 449 "yacc.yy"
{
	    yyval.node = yyvsp[0].node;
	  ;
    break;}
case 51:
#line 453 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_plus_sign, yyvsp[-2].node, yyvsp[0].node );
	  ;
    break;}
case 52:
#line 457 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_minus_sign, yyvsp[-2].node, yyvsp[0].node );
	  ;
    break;}
case 53:
#line 465 "yacc.yy"
{
	    yyval.node = yyvsp[0].node;
	  ;
    break;}
case 54:
#line 469 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_asterik, yyvsp[-2].node, yyvsp[0].node );
	  ;
    break;}
case 55:
#line 473 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_solidus, yyvsp[-2].node, yyvsp[0].node );
	  ;
    break;}
case 56:
#line 477 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_percent_sign, yyvsp[-2].node, yyvsp[0].node );
	  ;
    break;}
case 57:
#line 485 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_minus_sign, yyvsp[0].node );
	  ;
    break;}
case 58:
#line 489 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_plus_sign, yyvsp[0].node );
	  ;
    break;}
case 59:
#line 493 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_tilde, yyvsp[0].node );
	  ;
    break;}
case 60:
#line 497 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_not, yyvsp[0].node );
	  ;
    break;}
case 61:
#line 501 "yacc.yy"
{
	    yyval.node = yyvsp[0].node;
	  ;
    break;}
case 62:
#line 508 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_array, yyvsp[-3].node, yyvsp[-1].node );
	  ;
    break;}
case 63:
#line 512 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_dict, yyvsp[-3].node, yyvsp[-1].node );
	  ;
    break;}
case 64:
#line 516 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_func_call, yyvsp[-2].node );
	  ;
    break;}
case 65:
#line 520 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_func_call, yyvsp[-3].node, yyvsp[-1].node );
	  ;
    break;}
case 66:
#line 524 "yacc.yy"
{
	    yyval.node = yyvsp[0].node;
	  ;
    break;}
case 67:
#line 531 "yacc.yy"
{
	    yyval.node = new KSParseNode( func_call_params, yyvsp[-2].node, yyvsp[0].node );
	  ;
    break;}
case 68:
#line 536 "yacc.yy"
{
	    yyval.node = new KSParseNode( func_call_params, yyvsp[0].node );
	  ;
    break;}
case 69:
#line 543 "yacc.yy"
{
	    yyval.node = new KSParseNode( member_expr, yyvsp[-2].node );
	    yyval.node->setIdent( yyvsp[0].ident );
	  ;
    break;}
case 70:
#line 548 "yacc.yy"
{
	    yyval.node = yyvsp[0].node;
	  ;
    break;}
case 71:
#line 555 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_incr, yyvsp[0].node );
	  ;
    break;}
case 72:
#line 559 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_decr, yyvsp[0].node );
	  ;
    break;}
case 73:
#line 563 "yacc.yy"
{
	    /* Setting $1 twice indicates that this is a postfix operator */
	    yyval.node = new KSParseNode( t_incr, yyvsp[-1].node, yyvsp[-1].node );
	  ;
    break;}
case 74:
#line 568 "yacc.yy"
{
	    /* Setting $1 twice indicates that this is a postfix operator */
	    yyval.node = new KSParseNode( t_decr, yyvsp[-1].node, yyvsp[-1].node );
	  ;
    break;}
case 75:
#line 573 "yacc.yy"
{
	    yyval.node = yyvsp[0].node;
	  ;
    break;}
case 76:
#line 580 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_match, yyvsp[-1].node );
	    yyval.node->setIdent( yyvsp[0]._str );
	  ;
    break;}
case 77:
#line 585 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_subst, yyvsp[-1].node );
	    yyval.node->setIdent( yyvsp[0].ident );
	  ;
    break;}
case 78:
#line 590 "yacc.yy"
{
	    yyval.node = yyvsp[0].node;
	  ;
    break;}
case 79:
#line 598 "yacc.yy"
{
	    yyval.node = yyvsp[0].node;
	  ;
    break;}
case 80:
#line 602 "yacc.yy"
{
	    yyval.node = yyvsp[0].node;
	  ;
    break;}
case 81:
#line 606 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_input );
	  ;
    break;}
case 82:
#line 610 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_match_line );
	    yyval.node->setIdent( yyvsp[0]._str );
	  ;
    break;}
case 83:
#line 615 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_file_op, yyvsp[-1].node );
	    yyval.node->setIdent( yyvsp[-2]._str );
	  ;
    break;}
case 84:
#line 619 "yacc.yy"
{ ;
    break;}
case 85:
#line 621 "yacc.yy"
{
	    yyval.node = yyvsp[-1].node;
	  ;
    break;}
case 86:
#line 630 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_integer_literal );
	    yyval.node->setIntegerLiteral( yyvsp[0]._int );
	  ;
    break;}
case 87:
#line 635 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_cell );
	    yyval.node->setStringLiteral( yyvsp[0]._str );
	  ;
    break;}
case 88:
#line 640 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_range );
	    yyval.node->setStringLiteral( yyvsp[0]._str );
	  ;
    break;}
case 89:
#line 645 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_string_literal );
	    yyval.node->setStringLiteral( yyvsp[0]._str );
	  ;
    break;}
case 90:
#line 650 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_character_literal );
	    yyval.node->setCharacterLiteral( yyvsp[0]._char );
	  ;
    break;}
case 91:
#line 655 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_floating_pt_literal );
	    yyval.node->setFloatingPtLiteral( yyvsp[0]._float );
	  ;
    break;}
case 92:
#line 660 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_boolean_literal );
	    yyval.node->setBooleanLiteral( true );
	  ;
    break;}
case 93:
#line 665 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_boolean_literal );
	    yyval.node->setBooleanLiteral( false );
	  ;
    break;}
case 94:
#line 670 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_array_const );
	  ;
    break;}
case 95:
#line 674 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_array_const, yyvsp[-1].node );
	  ;
    break;}
case 96:
#line 678 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_dict_const );
	  ;
    break;}
case 97:
#line 682 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_dict_const, yyvsp[-1].node );
	  ;
    break;}
case 98:
#line 686 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_line );
	  ;
    break;}
case 99:
#line 690 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_regexp_group );
	    yyval.node->setIntegerLiteral( yyvsp[0]._int );
	  ;
    break;}
case 100:
#line 698 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_array_element, yyvsp[-2].node, yyvsp[0].node );
	  ;
    break;}
case 101:
#line 702 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_array_element, yyvsp[0].node  );
	  ;
    break;}
case 102:
#line 709 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_dict_element, yyvsp[-5].node, yyvsp[-3].node, yyvsp[0].node );
	  ;
    break;}
case 103:
#line 713 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_dict_element, yyvsp[-3].node, yyvsp[-1].node );
	  ;
    break;}
case 104:
#line 723 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_struct, yyvsp[-1].node );
	    yyval.node->setIdent( yyvsp[-3].ident );
	  ;
    break;}
case 105:
#line 731 "yacc.yy"
{
	    yyval.node = new KSParseNode( exports, yyvsp[0].node );
	  ;
    break;}
case 106:
#line 735 "yacc.yy"
{
	    yyval.node = new KSParseNode( exports, yyvsp[-1].node );
	    yyval.node->setBranch( 2, yyvsp[0].node );
	  ;
    break;}
case 107:
#line 743 "yacc.yy"
{
	    yyval.node = yyvsp[-1].node;
	  ;
    break;}
case 108:
#line 747 "yacc.yy"
{
	    yyval.node = yyvsp[-1].node;
	  ;
    break;}
case 109:
#line 751 "yacc.yy"
{
	    yyval.node = yyvsp[-1].node;
	  ;
    break;}
case 110:
#line 758 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_struct_members );
	    yyval.node->setIdent( yyvsp[0].ident );
	  ;
    break;}
case 111:
#line 763 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_struct_members, yyvsp[0].node );
	    yyval.node->setIdent( yyvsp[-2].ident );
	  ;
    break;}
case 112:
#line 772 "yacc.yy"
{
	    yyval.node = new KSParseNode( class_dcl, yyvsp[-3].node, yyvsp[-1].node );
	  ;
    break;}
case 113:
#line 780 "yacc.yy"
{
	    yyval.node = new KSParseNode( class_header );
	    yyval.node->setIdent( yyvsp[0].ident );
	  ;
    break;}
case 114:
#line 785 "yacc.yy"
{
	    yyval.node = new KSParseNode( class_header );
	    yyval.node->setIdent( yyvsp[-1].ident );
	    yyval.node->setBranch( 1, yyvsp[0].node );
	  ;
    break;}
case 115:
#line 795 "yacc.yy"
{
	    yyval.node = NULL;
	  ;
    break;}
case 116:
#line 799 "yacc.yy"
{
	    yyval.node = yyvsp[0].node;
	  ;
    break;}
case 117:
#line 806 "yacc.yy"
{
	    yyval.node = new KSParseNode( exports, yyvsp[0].node );
	  ;
    break;}
case 118:
#line 810 "yacc.yy"
{
	    yyval.node = new KSParseNode( exports, yyvsp[-1].node );
	    yyval.node->setBranch( 2, yyvsp[0].node );
	  ;
    break;}
case 119:
#line 818 "yacc.yy"
{
	    yyval.node = yyvsp[-1].node;
	  ;
    break;}
case 120:
#line 822 "yacc.yy"
{
	    yyval.node = yyvsp[-1].node;
	  ;
    break;}
case 121:
#line 826 "yacc.yy"
{
	    yyval.node = new KSParseNode( signal_dcl, yyvsp[-2].node );
	    yyval.node->setIdent( yyvsp[-4].ident );
	  ;
    break;}
case 122:
#line 831 "yacc.yy"
{
	    yyval.node = new KSParseNode( signal_dcl, 0 );
	    yyval.node->setIdent( yyvsp[-3].ident );
	  ;
    break;}
case 123:
#line 836 "yacc.yy"
{
	    yyval.node = new KSParseNode( destructor_dcl );
	    KSParseNode* n = new KSParseNode( func_param_in );
	    n->setIdent( yyvsp[-4].ident );
	    yyval.node->setBranch( 1, n );
	    yyval.node->setBranch( 2, yyvsp[-1].node );
	    yyval.node->setIdent( "delete" );
	  ;
    break;}
case 124:
#line 845 "yacc.yy"
{
	    yyval.node = yyvsp[-1].node;
	  ;
    break;}
case 125:
#line 852 "yacc.yy"
{
	    yyval.node = new KSParseNode( func_dcl );
	    yyval.node->setBranch( 2, yyvsp[0].node );
	    yyval.node->setIdent( yyvsp[-4].ident );
	  ;
    break;}
case 126:
#line 858 "yacc.yy"
{
	    yyval.node = new KSParseNode( func_dcl, yyvsp[-3].node, yyvsp[0].node );
	    yyval.node->setIdent( yyvsp[-5].ident );
	  ;
    break;}
case 127:
#line 866 "yacc.yy"
{
	    yyval.node = new KSParseNode( func_params, yyvsp[0].node );
	  ;
    break;}
case 128:
#line 870 "yacc.yy"
{
	    yyval.node = new KSParseNode( func_params, yyvsp[-2].node, yyvsp[0].node );
	  ;
    break;}
case 129:
#line 877 "yacc.yy"
{
	    yyval.node = new KSParseNode( func_param_in );
	    yyval.node->setIdent( yyvsp[0].ident );
	  ;
    break;}
case 130:
#line 882 "yacc.yy"
{
	    yyval.node = new KSParseNode( func_param_in, yyvsp[0].node );
	    yyval.node->setIdent( yyvsp[-2].ident );
	  ;
    break;}
case 131:
#line 887 "yacc.yy"
{
	    yyval.node = new KSParseNode( func_param_out );
	    yyval.node->setIdent( yyvsp[0].ident );
	  ;
    break;}
case 132:
#line 892 "yacc.yy"
{
	    yyval.node = new KSParseNode( func_param_inout );
	    yyval.node->setIdent( yyvsp[0].ident );
	  ;
    break;}
case 133:
#line 900 "yacc.yy"
{
	    yyval.node = new KSParseNode( signal_params, yyvsp[0].node );
	  ;
    break;}
case 134:
#line 904 "yacc.yy"
{
	    yyval.node = new KSParseNode( signal_params, yyvsp[-2].node, yyvsp[0].node );
	  ;
    break;}
case 135:
#line 911 "yacc.yy"
{
	    yyval.node = new KSParseNode( signal_param );
	    yyval.node->setIdent( yyvsp[0].ident );
	  ;
    break;}
case 136:
#line 916 "yacc.yy"
{
	    yyval.node = new KSParseNode( signal_param, yyvsp[0].node );
	    yyval.node->setIdent( yyvsp[-2].ident );
	  ;
    break;}
case 137:
#line 924 "yacc.yy"
{
	    yyval.node = NULL;
	  ;
    break;}
case 138:
#line 928 "yacc.yy"
{
	    yyval.node = yyvsp[0].node;
	  ;
    break;}
case 139:
#line 935 "yacc.yy"
{
	    yyval.node = new KSParseNode( func_lines, yyvsp[0].node );
	  ;
    break;}
case 140:
#line 939 "yacc.yy"
{
	    yyval.node = new KSParseNode( func_lines, yyvsp[-1].node );
	    yyval.node->setBranch( 2, yyvsp[0].node );
	  ;
    break;}
case 141:
#line 947 "yacc.yy"
{
	    yyval.node = yyvsp[-1].node;
	  ;
    break;}
case 142:
#line 951 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_raise, yyvsp[-3].node, yyvsp[-1].node );
	  ;
    break;}
case 143:
#line 955 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_emit, yyvsp[-1].node );
	  ;
    break;}
case 144:
#line 959 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_return );
	  ;
    break;}
case 145:
#line 963 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_return, yyvsp[-1].node );
	  ;
    break;}
case 146:
#line 967 "yacc.yy"
{
	    yyval.node = yyvsp[0].node;
	  ;
    break;}
case 147:
#line 971 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_try, yyvsp[-2].node, yyvsp[0].node );
	  ;
    break;}
case 148:
#line 975 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_scope, yyvsp[-1].node );
	  ;
    break;}
case 149:
#line 982 "yacc.yy"
{
	    yyval.node = yyvsp[0].node;
	  ;
    break;}
case 150:
#line 986 "yacc.yy"
{
	    yyval.node = yyvsp[-1].node;
	    yyval.node->setBranch( 4, yyvsp[0].node );
	  ;
    break;}
case 151:
#line 995 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_catch, yyvsp[-6].node, yyvsp[-1].node );
	    yyval.node->setIdent( yyvsp[-4].ident );
	  ;
    break;}
case 152:
#line 1001 "yacc.yy"
{
	    KSParseNode* x = new KSParseNode( scoped_name );
	    x->setIdent( yyvsp[-6].ident );
	    yyval.node = new KSParseNode( t_catch_default, x, yyvsp[-1].node );
	    yyval.node->setIdent( yyvsp[-4].ident );
	  ;
    break;}
case 153:
#line 1011 "yacc.yy"
{
	    yyval.node = new KSParseNode( const_dcl, yyvsp[0].node );
	    yyval.node->setIdent( yyvsp[-2].ident );
	  ;
    break;}
case 154:
#line 1019 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_while, yyvsp[-1].node, yyvsp[0].node );
	  ;
    break;}
case 155:
#line 1023 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_for, yyvsp[-6].node, yyvsp[-4].node, yyvsp[-2].node, yyvsp[0].node );
	  ;
    break;}
case 156:
#line 1027 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_do, yyvsp[-2].node, yyvsp[-1].node );
	  ;
    break;}
case 157:
#line 1031 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_if, yyvsp[-3].node, yyvsp[-1].node, yyvsp[0].node );
	  ;
    break;}
case 158:
#line 1035 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_foreach, yyvsp[-2].node, yyvsp[0].node );
	    yyval.node->setIdent( yyvsp[-4].ident );
	  ;
    break;}
case 159:
#line 1040 "yacc.yy"
{
	    /* We set $9 twice to indicate thet this is the foreach for maps */
	    yyval.node = new KSParseNode( t_foreach, yyvsp[-2].node, yyvsp[0].node, yyvsp[0].node );
	    yyval.node->setIdent( yyvsp[-6].ident );
	    yyval.node->setStringLiteral( yyvsp[-4].ident );
	  ;
    break;}
case 160:
#line 1050 "yacc.yy"
{
	    yyval.node = NULL;
	  ;
    break;}
case 161:
#line 1054 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_if, yyvsp[-3].node, yyvsp[-1].node, yyvsp[0].node );
	  ;
    break;}
case 162:
#line 1058 "yacc.yy"
{
	    yyval.node = yyvsp[0].node;
	  ;
    break;}
case 163:
#line 1065 "yacc.yy"
{
	    yyval.node = yyvsp[-1].node;
	  ;
    break;}
case 164:
#line 1072 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_scope, yyvsp[-1].node );
	  ;
    break;}
}
   /* the action file gets copied in in place of this dollarsign */
#line 498 "/usr/share/bison.simple"

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
#line 1076 "yacc.yy"


void kscriptParse( const char *_code, int extension )
{
    kscriptInitFlex( _code, extension );
    yyparse();
}
