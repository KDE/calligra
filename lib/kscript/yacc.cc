
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
#define	T_DOLLAR	333

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



#define	YYFINAL		347
#define	YYFLAG		-32768
#define	YYNTBASE	79

#define YYTRANSLATE(x) ((unsigned)(x) <= 333 ? yytranslate[x] : 132)

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
    76,    77,    78
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
   259,   261,   263,   265,   267,   269,   271,   273,   275,   278,
   282,   285,   289,   292,   296,   298,   306,   312,   318,   320,
   323,   326,   330,   333,   335,   339,   344,   347,   351,   352,
   354,   356,   359,   362,   365,   372,   378,   387,   390,   396,
   403,   405,   409,   412,   417,   420,   423,   425,   429,   432,
   437,   438,   440,   442,   445,   448,   454,   458,   461,   465,
   467,   473,   477,   479,   482,   492,   502,   507,   510,   520,
   525,   532,   540,   550,   551,   559,   562,   567
};

static const short yyrhs[] = {    -1,
    47,    91,     0,    80,     0,    81,     0,    81,    80,     0,
   127,    33,     0,   108,    33,     0,   112,    33,     0,   117,
    29,     0,    66,    17,    33,     0,    74,    17,    66,     4,
    33,     0,    74,    17,    66,    82,    33,     0,    38,     0,
    83,     0,    17,     0,    17,     9,    82,     0,    58,    21,
   122,    29,     0,    58,    22,   118,    30,    21,   122,    29,
     0,     8,    85,     0,    86,     0,    86,     9,    85,     0,
    87,     0,    86,    49,    17,     0,    17,     0,    32,    17,
     0,    89,    47,    88,     0,    89,    75,    88,     0,    89,
     0,    90,    77,    89,     0,    90,     0,    91,    76,    90,
     0,    91,     0,    92,    13,    91,     0,    92,    48,    91,
     0,    92,    45,    91,     0,    92,    46,    91,     0,    92,
    24,    91,     0,    92,    16,    91,     0,    92,     0,    93,
     0,    92,    43,    93,     0,    94,     0,    93,     7,    94,
     0,    95,     0,    94,     3,    95,     0,    96,     0,    95,
    35,    96,     0,    95,    34,    96,     0,    97,     0,    96,
    28,    97,     0,    96,    25,    97,     0,    98,     0,    97,
     4,    98,     0,    97,    36,    98,     0,    97,    27,    98,
     0,    25,    99,     0,    28,    99,     0,    41,    99,     0,
    62,    99,     0,    99,     0,    99,    23,    88,    31,     0,
    99,    21,    88,    29,     0,    99,    22,    30,     0,    99,
    22,   100,    30,     0,   101,     0,    88,     9,   100,     0,
    88,     0,    99,    49,    17,     0,   102,     0,    56,   103,
     0,    57,   103,     0,   103,    56,     0,   103,    57,     0,
   103,     0,   104,    61,     0,   104,    60,     0,   104,     0,
    87,     0,   105,     0,    22,    88,    30,     0,    20,     0,
    73,     0,    72,     0,    37,     0,     6,     0,    15,     0,
    42,     0,    14,     0,    23,    31,     0,    23,   106,    31,
     0,    21,    29,     0,    21,   107,    29,     0,    78,    20,
     0,    88,     9,   106,     0,    88,     0,    22,    88,     9,
    91,    30,     9,   107,     0,    22,    88,     9,    91,    30,
     0,    39,    17,    21,   109,    29,     0,   110,     0,   110,
   109,     0,   117,    29,     0,    67,   111,    33,     0,   127,
    33,     0,    17,     0,    17,     9,   111,     0,   113,    21,
   114,    29,     0,    44,    17,     0,    44,    17,    84,     0,
     0,   115,     0,   116,     0,   116,   115,     0,   117,    29,
     0,   108,    33,     0,    64,    17,    22,   120,    30,    33,
     0,    64,    17,    22,    30,    33,     0,    50,    22,    18,
    17,    30,    21,   122,    29,     0,   127,    33,     0,    17,
    22,    30,    21,   122,     0,    17,    22,   118,    30,    21,
   122,     0,   119,     0,   119,     9,   118,     0,    18,    17,
     0,    18,    17,    47,    88,     0,    26,    17,     0,    19,
    17,     0,   121,     0,   121,     9,   118,     0,    18,    17,
     0,    18,    17,    47,    88,     0,     0,   123,     0,   124,
     0,   124,   123,     0,    88,    33,     0,    71,    88,     9,
    88,    33,     0,    65,    88,    33,     0,    63,    33,     0,
    63,    88,    33,     0,   128,     0,    70,    21,   123,    29,
   125,     0,    21,   123,    29,     0,   126,     0,   126,   125,
     0,    69,    22,    88,     9,    17,    30,    21,   123,    29,
     0,    11,    22,    17,     9,    17,    30,    21,   123,    29,
     0,    10,    17,    47,    91,     0,   130,   131,     0,    54,
    22,    88,    33,    88,    33,    88,    30,   131,     0,    55,
   131,   130,    33,     0,    52,    22,    88,    30,   131,   129,
     0,    59,    22,    17,     9,    88,    30,   131,     0,    59,
    22,    17,     9,    17,     9,    88,    30,   131,     0,     0,
    53,    52,    22,    88,    30,   131,   129,     0,    53,   131,
     0,    51,    22,    88,    30,     0,    21,   122,    29,     0
};

#endif

#if YYDEBUG != 0
static const short yyrline[] = { 0,
   169,   173,   177,   184,   188,   197,   201,   205,   209,   213,
   218,   224,   230,   235,   242,   246,   255,   261,   269,   277,
   281,   290,   294,   302,   307,   319,   323,   327,   334,   338,
   345,   349,   356,   360,   364,   368,   372,   376,   380,   388,
   392,   400,   404,   412,   416,   424,   428,   432,   440,   444,
   448,   456,   460,   464,   468,   476,   480,   484,   488,   492,
   499,   503,   507,   511,   515,   522,   527,   534,   539,   546,
   550,   554,   559,   564,   571,   576,   581,   589,   593,   597,
   606,   611,   616,   621,   626,   631,   636,   641,   646,   650,
   654,   658,   662,   670,   674,   681,   685,   695,   703,   707,
   715,   719,   723,   730,   735,   744,   752,   757,   767,   771,
   778,   782,   790,   794,   798,   803,   808,   817,   824,   830,
   838,   842,   849,   854,   859,   864,   872,   876,   883,   888,
   896,   900,   907,   911,   919,   923,   927,   931,   935,   939,
   943,   947,   954,   958,   966,   972,   983,   991,   995,   999,
  1003,  1007,  1012,  1022,  1026,  1030,  1037,  1044
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
"T_AND","T_OR","T_DOLLAR","specification","definitions","definition","import_list",
"main","inheritance_spec","qualified_names","qualified_name","scoped_name","assign_expr",
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
    79,    79,    79,    80,    80,    81,    81,    81,    81,    81,
    81,    81,    81,    81,    82,    82,    83,    83,    84,    85,
    85,    86,    86,    87,    87,    88,    88,    88,    89,    89,
    90,    90,    91,    91,    91,    91,    91,    91,    91,    92,
    92,    93,    93,    94,    94,    95,    95,    95,    96,    96,
    96,    97,    97,    97,    97,    98,    98,    98,    98,    98,
    99,    99,    99,    99,    99,   100,   100,   101,   101,   102,
   102,   102,   102,   102,   103,   103,   103,   104,   104,   104,
   105,   105,   105,   105,   105,   105,   105,   105,   105,   105,
   105,   105,   105,   106,   106,   107,   107,   108,   109,   109,
   110,   110,   110,   111,   111,   112,   113,   113,   114,   114,
   115,   115,   116,   116,   116,   116,   116,   116,   117,   117,
   118,   118,   119,   119,   119,   119,   120,   120,   121,   121,
   122,   122,   123,   123,   124,   124,   124,   124,   124,   124,
   124,   124,   125,   125,   126,   126,   127,   128,   128,   128,
   128,   128,   128,   129,   129,   129,   130,   131
};

static const short yyr2[] = {     0,
     0,     2,     1,     1,     2,     2,     2,     2,     2,     3,
     5,     5,     1,     1,     1,     3,     4,     7,     2,     1,
     3,     1,     3,     1,     2,     3,     3,     1,     3,     1,
     3,     1,     3,     3,     3,     3,     3,     3,     1,     1,
     3,     1,     3,     1,     3,     1,     3,     3,     1,     3,
     3,     1,     3,     3,     3,     2,     2,     2,     2,     1,
     4,     4,     3,     4,     1,     3,     1,     3,     1,     2,
     2,     2,     2,     1,     2,     2,     1,     1,     1,     3,
     1,     1,     1,     1,     1,     1,     1,     1,     2,     3,
     2,     3,     2,     3,     1,     7,     5,     5,     1,     2,
     2,     3,     2,     1,     3,     4,     2,     3,     0,     1,
     1,     2,     2,     2,     6,     5,     8,     2,     5,     6,
     1,     3,     2,     4,     2,     2,     1,     3,     2,     4,
     0,     1,     1,     2,     2,     5,     3,     2,     3,     1,
     5,     3,     1,     2,     9,     9,     4,     2,     9,     4,
     6,     7,     9,     0,     7,     2,     4,     3
};

static const short yydefact[] = {     1,
     0,     0,    13,     0,     0,     0,     0,     0,     0,     3,
     4,    14,     0,     0,     0,     0,     0,     0,     0,     0,
   107,    85,    88,    86,    24,    81,     0,     0,     0,     0,
     0,     0,    84,     0,    87,     0,     0,     0,    83,    82,
     0,    78,     2,    39,    40,    42,    44,    46,    49,    52,
    60,    65,    69,    74,    77,    79,   131,     0,     0,     0,
     5,     7,     8,   109,     9,     6,     0,     0,     0,     0,
     0,     0,   121,     0,     0,   108,     0,    91,     0,     0,
    28,    30,    32,    89,    95,     0,    56,    57,    25,    58,
    70,    71,    59,    93,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,    72,    73,    76,    75,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
   132,   133,   140,     0,     0,    10,     0,     0,     0,     0,
     0,   110,   111,     0,     0,   147,   123,   126,   125,   131,
     0,     0,     0,     0,    99,     0,     0,    19,    20,    22,
     0,    92,    80,     0,     0,     0,     0,     0,    90,    33,
    38,    37,    41,    35,    36,    34,    43,    45,    48,    47,
    51,    50,    53,    55,    54,     0,    63,    67,     0,     0,
    68,     0,     0,     0,     0,     0,   131,     0,     0,   138,
     0,     0,     0,     0,   135,    17,   134,   148,     0,     0,
    15,     0,     0,     0,   114,   106,   112,   113,   118,     0,
   119,   131,   122,   104,     0,    98,   100,   101,   103,     0,
     0,     0,    26,    27,    29,    31,    94,    62,     0,    64,
    61,     0,   142,     0,     0,     0,     0,     0,     0,   139,
   137,     0,     0,   131,    11,     0,    12,     0,     0,   124,
   120,     0,   102,    21,    23,     0,    66,   157,     0,     0,
   158,   150,     0,     0,     0,     0,    16,     0,     0,     0,
     0,   127,   105,    97,   154,     0,    24,     0,     0,     0,
   141,   143,   136,    18,     0,   129,   116,     0,     0,     0,
     0,   151,     0,     0,     0,     0,     0,   144,   131,     0,
   115,   128,    96,     0,   156,     0,     0,   152,     0,     0,
     0,   130,     0,     0,     0,     0,     0,   117,     0,   149,
   153,     0,     0,     0,     0,     0,   154,     0,     0,   155,
     0,     0,   146,   145,     0,     0,     0
};

static const short yydefgoto[] = {   345,
    10,    11,   212,    12,    76,   158,   159,    42,   129,    81,
    82,    83,    44,    45,    46,    47,    48,    49,    50,    51,
   189,    52,    53,    54,    55,    56,    86,    79,    13,   154,
   155,   225,    14,    15,   141,   142,   143,    16,    72,    73,
   281,   282,   130,   131,   132,   291,   292,    17,   133,   302,
   134,   198
};

static const short yypact[] = {   436,
    11,     9,-32768,    18,    31,   192,    72,    47,    62,-32768,
   446,-32768,    86,    99,    74,   113,   102,   111,   115,   128,
   145,-32768,-32768,-32768,-32768,-32768,    59,   192,   306,   106,
   106,   142,-32768,   106,-32768,   168,   168,   106,-32768,-32768,
   141,-32768,-32768,   309,   163,   169,    79,    29,    35,-32768,
    21,-32768,-32768,    61,    64,-32768,   246,    90,   140,   110,
-32768,-32768,-32768,   105,-32768,-32768,   192,   160,   164,   170,
   159,   156,   184,    -2,    20,-32768,   192,-32768,   167,   171,
   -25,   122,   121,-32768,   193,   172,    21,    21,-32768,    21,
-32768,-32768,    21,-32768,   192,   192,   192,   192,   192,   192,
   192,   192,   192,   192,   192,   192,   192,   192,   192,   192,
   192,   344,   192,   191,-32768,-32768,-32768,-32768,     4,   194,
   196,   197,   200,   204,   382,   192,   206,   192,   195,   201,
-32768,   246,-32768,   200,   202,-32768,    30,   209,   218,   203,
   208,-32768,   105,   210,   211,-32768,   212,-32768,-32768,   246,
   226,    90,   233,   222,    -2,   224,   223,-32768,     2,-32768,
   248,-32768,-32768,   192,   192,   192,   192,   192,-32768,-32768,
-32768,-32768,   163,-32768,-32768,-32768,   169,    79,    29,    29,
    35,    35,-32768,-32768,-32768,   229,-32768,   253,   225,   241,
-32768,   192,   244,   192,   192,   192,   246,   230,   260,-32768,
   247,   251,   246,   276,-32768,-32768,-32768,-32768,   265,   256,
   281,   258,   274,   271,-32768,-32768,-32768,-32768,-32768,   192,
-32768,   246,-32768,   285,   263,-32768,-32768,-32768,-32768,    20,
   282,   192,-32768,-32768,-32768,-32768,-32768,-32768,   192,-32768,
-32768,     8,-32768,   277,   280,   273,   275,   297,   304,-32768,
-32768,   286,   192,   246,-32768,   315,-32768,   318,    50,-32768,
-32768,   233,-32768,-32768,-32768,   284,-32768,-32768,   200,   192,
-32768,-32768,   420,     3,   303,   310,-32768,   311,   323,   312,
   314,   333,-32768,   337,   296,   320,   342,   326,   338,   348,
-32768,     3,-32768,-32768,   350,   328,-32768,   340,    90,   355,
   -12,-32768,   192,   192,   200,   363,   192,-32768,   246,   192,
-32768,-32768,-32768,   360,-32768,   353,   357,-32768,   380,   381,
   362,-32768,   192,   200,   200,   375,   376,-32768,   364,-32768,
-32768,   365,   368,   200,   387,   388,   296,   246,   246,-32768,
   383,   384,-32768,-32768,   411,   418,-32768
};

static const short yypgoto[] = {-32768,
   409,-32768,   165,-32768,-32768,   198,-32768,   -70,   -28,   259,
   262,     6,-32768,   329,   330,   327,    26,    45,   -19,   116,
   219,-32768,-32768,   120,-32768,-32768,   268,   131,   -57,   278,
-32768,   185,-32768,-32768,-32768,   307,-32768,   -51,   -56,-32768,
-32768,-32768,  -144,  -116,-32768,   157,-32768,   -44,-32768,   114,
   261,  -130
};


#define	YYLAST		520


static const short yytable[] = {    80,
    85,   135,   193,   208,   160,   221,   140,     1,   197,    22,
   230,    43,   144,   289,     2,   207,   232,    23,    24,   145,
    25,   164,   156,    26,   119,   192,    29,    18,    30,   157,
    19,    31,    78,   210,    20,    32,    25,   163,   108,   314,
    33,   111,   112,   113,    34,    35,   211,    21,   161,   165,
   231,    32,   247,   106,   120,   121,   107,   122,   123,    36,
    37,   109,   124,    59,   153,    38,   125,   279,   126,   114,
   110,   290,   146,   127,   128,    39,    40,   261,    60,   280,
    77,    41,   186,   188,   190,   140,   252,    78,   183,   184,
   185,   144,    57,    58,    64,   223,   201,   202,   145,   204,
   170,   171,   172,   156,   174,   175,   176,    68,    69,   276,
   157,    22,   104,   105,     1,    70,   115,   116,    62,    23,
    24,     2,    25,   117,   118,    26,    27,    28,    29,   179,
   180,    63,    68,    69,    66,   233,   234,    32,   285,    85,
    70,    65,    33,     4,    71,    87,    88,    35,    74,    90,
   181,   182,    75,    93,   138,    91,    92,    67,    89,   160,
    94,    36,    37,   242,   321,   244,   245,   246,   139,   102,
   315,   103,   136,    22,   318,   137,   147,    39,    40,   150,
   148,    23,    24,    41,    25,   151,   149,    26,    27,    28,
    29,   260,   152,   330,   331,   162,   167,    22,   166,    32,
   163,   168,   169,   337,    33,    23,    24,   191,    25,    35,
   188,    26,    27,    28,    29,   194,    30,   195,   196,    31,
   197,   341,   342,    32,   275,   199,   203,   205,    33,   206,
   213,   209,    34,    35,   214,   215,   216,   266,   218,    39,
    40,   286,   312,   219,   288,    41,   222,    36,    37,   224,
   226,    22,   228,    38,   240,   229,   232,   238,   220,    23,
    24,   239,    25,    39,    40,    26,   119,    28,    29,    41,
    30,   241,   243,    31,   316,   317,   249,    32,   320,   250,
   120,   322,    33,   251,   253,   254,    34,    35,   255,   256,
   257,   258,   259,   262,   329,   263,   120,   121,   265,   122,
   123,    36,    37,   271,   124,   270,   268,    38,   125,   269,
   126,    22,   273,   284,   274,   127,   128,    39,    40,    23,
    24,    95,    25,    41,    96,    26,    27,    28,    29,   272,
    30,   211,    97,    31,   278,   293,    84,    32,   294,   296,
   295,   299,    33,   298,   297,   300,    34,    35,   301,    22,
   304,    98,   303,    99,   100,   305,   101,    23,    24,   306,
    25,    36,    37,    26,    27,    28,    29,    38,    30,   307,
   309,    31,   311,   187,   310,    32,    77,    39,    40,   319,
    33,   323,   324,    41,    34,    35,   325,    22,   326,   327,
   328,   332,   333,   334,   335,    23,    24,   336,    25,    36,
    37,    26,    27,    28,    29,    38,    30,   338,   339,    31,
   346,   343,   344,    32,   200,    39,    40,   347,    33,    61,
   277,    41,    34,    35,   235,    22,   173,   264,   236,   178,
   313,   177,   227,    23,    24,   237,   287,    36,    37,    26,
    27,    28,    29,    38,    30,     1,   283,    31,   308,   217,
   340,    32,     2,    39,    40,     1,    33,   267,   248,    41,
    34,    35,     2,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     3,     4,    36,    37,     0,     0,     5,
     0,    38,     6,     3,     4,     0,     0,     0,     0,     5,
     0,    39,    40,     7,     0,     0,     0,    41,     0,     0,
     0,     8,     0,     7,     0,     0,     0,     0,     0,     9,
     0,     8,     0,     0,     0,     0,     0,     0,     0,     9
};

static const short yycheck[] = {    28,
    29,    58,   119,   134,    75,   150,    64,    10,    21,     6,
     9,     6,    64,    11,    17,   132,     9,    14,    15,    64,
    17,    47,    74,    20,    21,    22,    23,    17,    25,    74,
    22,    28,    29,     4,    17,    32,    17,    30,     4,    52,
    37,    21,    22,    23,    41,    42,    17,    17,    77,    75,
    49,    32,   197,    25,    51,    52,    28,    54,    55,    56,
    57,    27,    59,    17,    67,    62,    63,    18,    65,    49,
    36,    69,    67,    70,    71,    72,    73,   222,    17,    30,
    22,    78,   111,   112,   113,   143,   203,    29,   108,   109,
   110,   143,    21,    22,    21,   152,   125,   126,   143,   128,
    95,    96,    97,   155,    99,   100,   101,    18,    19,   254,
   155,     6,    34,    35,    10,    26,    56,    57,    33,    14,
    15,    17,    17,    60,    61,    20,    21,    22,    23,   104,
   105,    33,    18,    19,    33,   164,   165,    32,   269,   168,
    26,    29,    37,    39,    30,    30,    31,    42,    21,    34,
   106,   107,     8,    38,    50,    36,    37,    47,    17,   230,
    20,    56,    57,   192,   309,   194,   195,   196,    64,     7,
   301,     3,    33,     6,   305,    66,    17,    72,    73,    21,
    17,    14,    15,    78,    17,    30,    17,    20,    21,    22,
    23,   220,     9,   324,   325,    29,    76,     6,    77,    32,
    30,     9,    31,   334,    37,    14,    15,    17,    17,    42,
   239,    20,    21,    22,    23,    22,    25,    22,    22,    28,
    21,   338,   339,    32,   253,    22,    21,    33,    37,    29,
    22,    30,    41,    42,    17,    33,    29,   232,    29,    72,
    73,   270,   299,    33,   273,    78,    21,    56,    57,    17,
    29,     6,    29,    62,    30,    33,     9,    29,    47,    14,
    15,     9,    17,    72,    73,    20,    21,    22,    23,    78,
    25,    31,    29,    28,   303,   304,    17,    32,   307,    33,
    51,   310,    37,    33,     9,    21,    41,    42,    33,     9,
    33,    18,    22,     9,   323,    33,    51,    52,    17,    54,
    55,    56,    57,    29,    59,    33,    30,    62,    63,    30,
    65,     6,     9,    30,    29,    70,    71,    72,    73,    14,
    15,    13,    17,    78,    16,    20,    21,    22,    23,    33,
    25,    17,    24,    28,    17,    33,    31,    32,    29,    17,
    30,     9,    37,    30,    33,     9,    41,    42,    53,     6,
     9,    43,    33,    45,    46,    30,    48,    14,    15,    22,
    17,    56,    57,    20,    21,    22,    23,    62,    25,    22,
    21,    28,    33,    30,    47,    32,    22,    72,    73,    17,
    37,    22,    30,    78,    41,    42,    30,     6,     9,     9,
    29,    17,    17,    30,    30,    14,    15,    30,    17,    56,
    57,    20,    21,    22,    23,    62,    25,    21,    21,    28,
     0,    29,    29,    32,    33,    72,    73,     0,    37,    11,
   256,    78,    41,    42,   166,     6,    98,   230,   167,   103,
   300,   102,   155,    14,    15,   168,    17,    56,    57,    20,
    21,    22,    23,    62,    25,    10,   262,    28,   292,   143,
   337,    32,    17,    72,    73,    10,    37,   239,   198,    78,
    41,    42,    17,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    38,    39,    56,    57,    -1,    -1,    44,
    -1,    62,    47,    38,    39,    -1,    -1,    -1,    -1,    44,
    -1,    72,    73,    58,    -1,    -1,    -1,    78,    -1,    -1,
    -1,    66,    -1,    58,    -1,    -1,    -1,    -1,    -1,    74,
    -1,    66,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    74
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
#line 170 "yacc.yy"
{
	    theParser->setRootNode( NULL );
	  ;
    break;}
case 2:
#line 174 "yacc.yy"
{
	    theParser->setRootNode( yyvsp[0].node );
	  ;
    break;}
case 3:
#line 178 "yacc.yy"
{
	    theParser->setRootNode( new KSParseNode( definitions, yyvsp[0].node ) );
	  ;
    break;}
case 4:
#line 185 "yacc.yy"
{
	    yyval.node = new KSParseNode( definitions, yyvsp[0].node );
	  ;
    break;}
case 5:
#line 189 "yacc.yy"
{
	    yyval.node = new KSParseNode( definitions, yyvsp[-1].node );
	    yyval.node->setBranch( 2, yyvsp[0].node );
	  ;
    break;}
case 6:
#line 198 "yacc.yy"
{
	    yyval.node = yyvsp[-1].node;
	  ;
    break;}
case 7:
#line 202 "yacc.yy"
{
	    yyval.node = yyvsp[-1].node;
	  ;
    break;}
case 8:
#line 206 "yacc.yy"
{
	    yyval.node = yyvsp[-1].node;
	  ;
    break;}
case 9:
#line 210 "yacc.yy"
{
	    yyval.node = yyvsp[-1].node;
	  ;
    break;}
case 10:
#line 214 "yacc.yy"
{
	    yyval.node = new KSParseNode( import );
	    yyval.node->setIdent( yyvsp[-1].ident );
	  ;
    break;}
case 11:
#line 219 "yacc.yy"
{
	    yyval.node = new KSParseNode( from );
	    yyval.node->setIdent( yyvsp[-3].ident );
	    yyval.node->setStringLiteral( QString( "" ) );
	  ;
    break;}
case 12:
#line 225 "yacc.yy"
{
	    yyval.node = new KSParseNode( from );
	    yyval.node->setIdent( yyvsp[-3].ident );
	    yyval.node->setStringLiteral( yyvsp[-1]._str );
	  ;
    break;}
case 13:
#line 231 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_pragma );
	    yyval.node->setIdent( yyvsp[0]._str );
	  ;
    break;}
case 14:
#line 236 "yacc.yy"
{
	    yyval.node = yyvsp[0].node;
	  ;
    break;}
case 15:
#line 243 "yacc.yy"
{
		yyval._str = yyvsp[0].ident;
	  ;
    break;}
case 16:
#line 247 "yacc.yy"
{
		(*yyvsp[-2].ident) += "/";
		(*yyvsp[-2].ident) += (*yyvsp[0]._str);
		yyval._str = yyvsp[-2].ident;
	  ;
    break;}
case 17:
#line 256 "yacc.yy"
{
	    yyval.node = new KSParseNode( func_dcl );
	    yyval.node->setBranch( 2, yyvsp[-1].node );
	    yyval.node->setIdent( "main" );
	  ;
    break;}
case 18:
#line 262 "yacc.yy"
{
	    yyval.node = new KSParseNode( func_dcl, yyvsp[-4].node, yyvsp[-1].node );
	    yyval.node->setIdent( "main" );
	  ;
    break;}
case 19:
#line 270 "yacc.yy"
{
	    yyval.node = yyvsp[0].node;
	  ;
    break;}
case 20:
#line 278 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_qualified_names, yyvsp[0].node );
	  ;
    break;}
case 21:
#line 282 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_qualified_names, yyvsp[-2].node );
	    yyval.node->setBranch( 2, yyvsp[0].node );
	  ;
    break;}
case 22:
#line 291 "yacc.yy"
{
	    yyval.node = yyvsp[0].node;
	  ;
    break;}
case 23:
#line 295 "yacc.yy"
{
	    yyval.node = new KSParseNode( member_expr, yyvsp[-2].node );
	    yyval.node->setIdent( yyvsp[0].ident );
	  ;
    break;}
case 24:
#line 303 "yacc.yy"
{
	    yyval.node = new KSParseNode( scoped_name );
	    yyval.node->setIdent( yyvsp[0].ident );
	  ;
    break;}
case 25:
#line 308 "yacc.yy"
{
	    yyval.node = new KSParseNode( scoped_name );
	    QString name = "::";
	    name += *(yyvsp[0].ident);
	    delete yyvsp[0].ident;
	    yyval.node->setIdent( name );
	  ;
    break;}
case 26:
#line 320 "yacc.yy"
{
	    yyval.node = new KSParseNode( assign_expr, yyvsp[-2].node, yyvsp[0].node );
	  ;
    break;}
case 27:
#line 324 "yacc.yy"
{
	    yyval.node = new KSParseNode( plus_assign, yyvsp[-2].node, yyvsp[0].node );
	  ;
    break;}
case 28:
#line 328 "yacc.yy"
{
	    yyval.node = yyvsp[0].node;
	  ;
    break;}
case 29:
#line 335 "yacc.yy"
{
	    yyval.node = new KSParseNode( bool_or, yyvsp[-2].node, yyvsp[0].node );
	  ;
    break;}
case 30:
#line 339 "yacc.yy"
{
	    yyval.node = yyvsp[0].node;
	  ;
    break;}
case 31:
#line 346 "yacc.yy"
{
	    yyval.node = new KSParseNode( bool_and, yyvsp[-2].node, yyvsp[0].node );
	  ;
    break;}
case 32:
#line 350 "yacc.yy"
{
	    yyval.node = yyvsp[0].node;
	  ;
    break;}
case 33:
#line 357 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_equal, yyvsp[-2].node, yyvsp[0].node );
	  ;
    break;}
case 34:
#line 361 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_notequal, yyvsp[-2].node, yyvsp[0].node );
	  ;
    break;}
case 35:
#line 365 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_less_or_equal, yyvsp[-2].node, yyvsp[0].node );
	  ;
    break;}
case 36:
#line 369 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_greater_or_equal, yyvsp[-2].node, yyvsp[0].node );
	  ;
    break;}
case 37:
#line 373 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_less, yyvsp[-2].node, yyvsp[0].node );
	  ;
    break;}
case 38:
#line 377 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_greater, yyvsp[-2].node, yyvsp[0].node );
	  ;
    break;}
case 39:
#line 381 "yacc.yy"
{
	    yyval.node = yyvsp[0].node;
	  ;
    break;}
case 40:
#line 389 "yacc.yy"
{
	    yyval.node = yyvsp[0].node;
	  ;
    break;}
case 41:
#line 393 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_vertical_line, yyvsp[-2].node, yyvsp[0].node );
	  ;
    break;}
case 42:
#line 401 "yacc.yy"
{
	    yyval.node = yyvsp[0].node;
	  ;
    break;}
case 43:
#line 405 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_circumflex, yyvsp[-2].node, yyvsp[0].node );
	  ;
    break;}
case 44:
#line 413 "yacc.yy"
{
	    yyval.node = yyvsp[0].node;
	  ;
    break;}
case 45:
#line 417 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_ampersand, yyvsp[-2].node, yyvsp[0].node );
	  ;
    break;}
case 46:
#line 425 "yacc.yy"
{
	    yyval.node = yyvsp[0].node;
	  ;
    break;}
case 47:
#line 429 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_shiftright, yyvsp[-2].node, yyvsp[0].node );
	  ;
    break;}
case 48:
#line 433 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_shiftleft, yyvsp[-2].node, yyvsp[0].node );
	  ;
    break;}
case 49:
#line 441 "yacc.yy"
{
	    yyval.node = yyvsp[0].node;
	  ;
    break;}
case 50:
#line 445 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_plus_sign, yyvsp[-2].node, yyvsp[0].node );
	  ;
    break;}
case 51:
#line 449 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_minus_sign, yyvsp[-2].node, yyvsp[0].node );
	  ;
    break;}
case 52:
#line 457 "yacc.yy"
{
	    yyval.node = yyvsp[0].node;
	  ;
    break;}
case 53:
#line 461 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_asterik, yyvsp[-2].node, yyvsp[0].node );
	  ;
    break;}
case 54:
#line 465 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_solidus, yyvsp[-2].node, yyvsp[0].node );
	  ;
    break;}
case 55:
#line 469 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_percent_sign, yyvsp[-2].node, yyvsp[0].node );
	  ;
    break;}
case 56:
#line 477 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_minus_sign, yyvsp[0].node );
	  ;
    break;}
case 57:
#line 481 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_plus_sign, yyvsp[0].node );
	  ;
    break;}
case 58:
#line 485 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_tilde, yyvsp[0].node );
	  ;
    break;}
case 59:
#line 489 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_not, yyvsp[0].node );
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
	    yyval.node = new KSParseNode( t_array, yyvsp[-3].node, yyvsp[-1].node );
	  ;
    break;}
case 62:
#line 504 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_dict, yyvsp[-3].node, yyvsp[-1].node );
	  ;
    break;}
case 63:
#line 508 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_func_call, yyvsp[-2].node );
	  ;
    break;}
case 64:
#line 512 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_func_call, yyvsp[-3].node, yyvsp[-1].node );
	  ;
    break;}
case 65:
#line 516 "yacc.yy"
{
	    yyval.node = yyvsp[0].node;
	  ;
    break;}
case 66:
#line 523 "yacc.yy"
{
	    yyval.node = new KSParseNode( func_call_params, yyvsp[-2].node, yyvsp[0].node );
	  ;
    break;}
case 67:
#line 528 "yacc.yy"
{
	    yyval.node = new KSParseNode( func_call_params, yyvsp[0].node );
	  ;
    break;}
case 68:
#line 535 "yacc.yy"
{
	    yyval.node = new KSParseNode( member_expr, yyvsp[-2].node );
	    yyval.node->setIdent( yyvsp[0].ident );
	  ;
    break;}
case 69:
#line 540 "yacc.yy"
{
	    yyval.node = yyvsp[0].node;
	  ;
    break;}
case 70:
#line 547 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_incr, yyvsp[0].node );
	  ;
    break;}
case 71:
#line 551 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_decr, yyvsp[0].node );
	  ;
    break;}
case 72:
#line 555 "yacc.yy"
{
	    /* Setting $1 twice indicates that this is a postfix operator */
	    yyval.node = new KSParseNode( t_incr, yyvsp[-1].node, yyvsp[-1].node );
	  ;
    break;}
case 73:
#line 560 "yacc.yy"
{
	    /* Setting $1 twice indicates that this is a postfix operator */
	    yyval.node = new KSParseNode( t_decr, yyvsp[-1].node, yyvsp[-1].node );
	  ;
    break;}
case 74:
#line 565 "yacc.yy"
{
	    yyval.node = yyvsp[0].node;
	  ;
    break;}
case 75:
#line 572 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_match, yyvsp[-1].node );
	    yyval.node->setIdent( yyvsp[0]._str );
	  ;
    break;}
case 76:
#line 577 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_subst, yyvsp[-1].node );
	    yyval.node->setIdent( yyvsp[0].ident );
	  ;
    break;}
case 77:
#line 582 "yacc.yy"
{
	    yyval.node = yyvsp[0].node;
	  ;
    break;}
case 78:
#line 590 "yacc.yy"
{
	    yyval.node = yyvsp[0].node;
	  ;
    break;}
case 79:
#line 594 "yacc.yy"
{
	    yyval.node = yyvsp[0].node;
	  ;
    break;}
case 80:
#line 598 "yacc.yy"
{
	    yyval.node = yyvsp[-1].node;
	  ;
    break;}
case 81:
#line 607 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_integer_literal );
	    yyval.node->setIntegerLiteral( yyvsp[0]._int );
	  ;
    break;}
case 82:
#line 612 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_cell );
	    yyval.node->setStringLiteral( yyvsp[0]._str );
	  ;
    break;}
case 83:
#line 617 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_range );
	    yyval.node->setStringLiteral( yyvsp[0]._str );
	  ;
    break;}
case 84:
#line 622 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_string_literal );
	    yyval.node->setStringLiteral( yyvsp[0]._str );
	  ;
    break;}
case 85:
#line 627 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_character_literal );
	    yyval.node->setCharacterLiteral( yyvsp[0]._char );
	  ;
    break;}
case 86:
#line 632 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_floating_pt_literal );
	    yyval.node->setFloatingPtLiteral( yyvsp[0]._float );
	  ;
    break;}
case 87:
#line 637 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_boolean_literal );
	    yyval.node->setBooleanLiteral( true );
	  ;
    break;}
case 88:
#line 642 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_boolean_literal );
	    yyval.node->setBooleanLiteral( false );
	  ;
    break;}
case 89:
#line 647 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_array_const );
	  ;
    break;}
case 90:
#line 651 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_array_const, yyvsp[-1].node );
	  ;
    break;}
case 91:
#line 655 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_dict_const );
	  ;
    break;}
case 92:
#line 659 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_dict_const, yyvsp[-1].node );
	  ;
    break;}
case 93:
#line 663 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_regexp_group );
	    yyval.node->setIntegerLiteral( yyvsp[0]._int );
	  ;
    break;}
case 94:
#line 671 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_array_element, yyvsp[-2].node, yyvsp[0].node );
	  ;
    break;}
case 95:
#line 675 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_array_element, yyvsp[0].node  );
	  ;
    break;}
case 96:
#line 682 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_dict_element, yyvsp[-5].node, yyvsp[-3].node, yyvsp[0].node );
	  ;
    break;}
case 97:
#line 686 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_dict_element, yyvsp[-3].node, yyvsp[-1].node );
	  ;
    break;}
case 98:
#line 696 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_struct, yyvsp[-1].node );
	    yyval.node->setIdent( yyvsp[-3].ident );
	  ;
    break;}
case 99:
#line 704 "yacc.yy"
{
	    yyval.node = new KSParseNode( exports, yyvsp[0].node );
	  ;
    break;}
case 100:
#line 708 "yacc.yy"
{
	    yyval.node = new KSParseNode( exports, yyvsp[-1].node );
	    yyval.node->setBranch( 2, yyvsp[0].node );
	  ;
    break;}
case 101:
#line 716 "yacc.yy"
{
	    yyval.node = yyvsp[-1].node;
	  ;
    break;}
case 102:
#line 720 "yacc.yy"
{
	    yyval.node = yyvsp[-1].node;
	  ;
    break;}
case 103:
#line 724 "yacc.yy"
{
	    yyval.node = yyvsp[-1].node;
	  ;
    break;}
case 104:
#line 731 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_struct_members );
	    yyval.node->setIdent( yyvsp[0].ident );
	  ;
    break;}
case 105:
#line 736 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_struct_members, yyvsp[0].node );
	    yyval.node->setIdent( yyvsp[-2].ident );
	  ;
    break;}
case 106:
#line 745 "yacc.yy"
{
	    yyval.node = new KSParseNode( class_dcl, yyvsp[-3].node, yyvsp[-1].node );
	  ;
    break;}
case 107:
#line 753 "yacc.yy"
{
	    yyval.node = new KSParseNode( class_header );
	    yyval.node->setIdent( yyvsp[0].ident );
	  ;
    break;}
case 108:
#line 758 "yacc.yy"
{
	    yyval.node = new KSParseNode( class_header );
	    yyval.node->setIdent( yyvsp[-1].ident );
	    yyval.node->setBranch( 1, yyvsp[0].node );
	  ;
    break;}
case 109:
#line 768 "yacc.yy"
{
	    yyval.node = NULL;
	  ;
    break;}
case 110:
#line 772 "yacc.yy"
{
	    yyval.node = yyvsp[0].node;
	  ;
    break;}
case 111:
#line 779 "yacc.yy"
{
	    yyval.node = new KSParseNode( exports, yyvsp[0].node );
	  ;
    break;}
case 112:
#line 783 "yacc.yy"
{
	    yyval.node = new KSParseNode( exports, yyvsp[-1].node );
	    yyval.node->setBranch( 2, yyvsp[0].node );
	  ;
    break;}
case 113:
#line 791 "yacc.yy"
{
	    yyval.node = yyvsp[-1].node;
	  ;
    break;}
case 114:
#line 795 "yacc.yy"
{
	    yyval.node = yyvsp[-1].node;
	  ;
    break;}
case 115:
#line 799 "yacc.yy"
{
	    yyval.node = new KSParseNode( signal_dcl, yyvsp[-2].node );
	    yyval.node->setIdent( yyvsp[-4].ident );
	  ;
    break;}
case 116:
#line 804 "yacc.yy"
{
	    yyval.node = new KSParseNode( signal_dcl, 0 );
	    yyval.node->setIdent( yyvsp[-3].ident );
	  ;
    break;}
case 117:
#line 809 "yacc.yy"
{
	    yyval.node = new KSParseNode( destructor_dcl );
	    KSParseNode* n = new KSParseNode( func_param_in );
	    n->setIdent( yyvsp[-4].ident );
	    yyval.node->setBranch( 1, n );
	    yyval.node->setBranch( 2, yyvsp[-1].node );
	    yyval.node->setIdent( "delete" );
	  ;
    break;}
case 118:
#line 818 "yacc.yy"
{
	    yyval.node = yyvsp[-1].node;
	  ;
    break;}
case 119:
#line 825 "yacc.yy"
{
	    yyval.node = new KSParseNode( func_dcl );
	    yyval.node->setBranch( 2, yyvsp[0].node );
	    yyval.node->setIdent( yyvsp[-4].ident );
	  ;
    break;}
case 120:
#line 831 "yacc.yy"
{
	    yyval.node = new KSParseNode( func_dcl, yyvsp[-3].node, yyvsp[0].node );
	    yyval.node->setIdent( yyvsp[-5].ident );
	  ;
    break;}
case 121:
#line 839 "yacc.yy"
{
	    yyval.node = new KSParseNode( func_params, yyvsp[0].node );
	  ;
    break;}
case 122:
#line 843 "yacc.yy"
{
	    yyval.node = new KSParseNode( func_params, yyvsp[-2].node, yyvsp[0].node );
	  ;
    break;}
case 123:
#line 850 "yacc.yy"
{
	    yyval.node = new KSParseNode( func_param_in );
	    yyval.node->setIdent( yyvsp[0].ident );
	  ;
    break;}
case 124:
#line 855 "yacc.yy"
{
	    yyval.node = new KSParseNode( func_param_in, yyvsp[0].node );
	    yyval.node->setIdent( yyvsp[-2].ident );
	  ;
    break;}
case 125:
#line 860 "yacc.yy"
{
	    yyval.node = new KSParseNode( func_param_out );
	    yyval.node->setIdent( yyvsp[0].ident );
	  ;
    break;}
case 126:
#line 865 "yacc.yy"
{
	    yyval.node = new KSParseNode( func_param_inout );
	    yyval.node->setIdent( yyvsp[0].ident );
	  ;
    break;}
case 127:
#line 873 "yacc.yy"
{
	    yyval.node = new KSParseNode( signal_params, yyvsp[0].node );
	  ;
    break;}
case 128:
#line 877 "yacc.yy"
{
	    yyval.node = new KSParseNode( signal_params, yyvsp[-2].node, yyvsp[0].node );
	  ;
    break;}
case 129:
#line 884 "yacc.yy"
{
	    yyval.node = new KSParseNode( signal_param );
	    yyval.node->setIdent( yyvsp[0].ident );
	  ;
    break;}
case 130:
#line 889 "yacc.yy"
{
	    yyval.node = new KSParseNode( signal_param, yyvsp[0].node );
	    yyval.node->setIdent( yyvsp[-2].ident );
	  ;
    break;}
case 131:
#line 897 "yacc.yy"
{
	    yyval.node = NULL;
	  ;
    break;}
case 132:
#line 901 "yacc.yy"
{
	    yyval.node = yyvsp[0].node;
	  ;
    break;}
case 133:
#line 908 "yacc.yy"
{
	    yyval.node = new KSParseNode( func_lines, yyvsp[0].node );
	  ;
    break;}
case 134:
#line 912 "yacc.yy"
{
	    yyval.node = new KSParseNode( func_lines, yyvsp[-1].node );
	    yyval.node->setBranch( 2, yyvsp[0].node );
	  ;
    break;}
case 135:
#line 920 "yacc.yy"
{
	    yyval.node = yyvsp[-1].node;
	  ;
    break;}
case 136:
#line 924 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_raise, yyvsp[-3].node, yyvsp[-1].node );
	  ;
    break;}
case 137:
#line 928 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_emit, yyvsp[-1].node );
	  ;
    break;}
case 138:
#line 932 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_return );
	  ;
    break;}
case 139:
#line 936 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_return, yyvsp[-1].node );
	  ;
    break;}
case 140:
#line 940 "yacc.yy"
{
	    yyval.node = yyvsp[0].node;
	  ;
    break;}
case 141:
#line 944 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_try, yyvsp[-2].node, yyvsp[0].node );
	  ;
    break;}
case 142:
#line 948 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_scope, yyvsp[-1].node );
	  ;
    break;}
case 143:
#line 955 "yacc.yy"
{
	    yyval.node = yyvsp[0].node;
	  ;
    break;}
case 144:
#line 959 "yacc.yy"
{
	    yyval.node = yyvsp[-1].node;
	    yyval.node->setBranch( 4, yyvsp[0].node );
	  ;
    break;}
case 145:
#line 968 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_catch, yyvsp[-6].node, yyvsp[-1].node );
	    yyval.node->setIdent( yyvsp[-4].ident );
	  ;
    break;}
case 146:
#line 974 "yacc.yy"
{
	    KSParseNode* x = new KSParseNode( scoped_name );
	    x->setIdent( yyvsp[-6].ident );
	    yyval.node = new KSParseNode( t_catch_default, x, yyvsp[-1].node );
	    yyval.node->setIdent( yyvsp[-4].ident );
	  ;
    break;}
case 147:
#line 984 "yacc.yy"
{
	    yyval.node = new KSParseNode( const_dcl, yyvsp[0].node );
	    yyval.node->setIdent( yyvsp[-2].ident );
	  ;
    break;}
case 148:
#line 992 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_while, yyvsp[-1].node, yyvsp[0].node );
	  ;
    break;}
case 149:
#line 996 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_for, yyvsp[-6].node, yyvsp[-4].node, yyvsp[-2].node, yyvsp[0].node );
	  ;
    break;}
case 150:
#line 1000 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_do, yyvsp[-2].node, yyvsp[-1].node );
	  ;
    break;}
case 151:
#line 1004 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_if, yyvsp[-3].node, yyvsp[-1].node, yyvsp[0].node );
	  ;
    break;}
case 152:
#line 1008 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_foreach, yyvsp[-2].node, yyvsp[0].node );
	    yyval.node->setIdent( yyvsp[-4].ident );
	  ;
    break;}
case 153:
#line 1013 "yacc.yy"
{
	    /* We set $9 twice to indicate thet this is the foreach for maps */
	    yyval.node = new KSParseNode( t_foreach, yyvsp[-2].node, yyvsp[0].node, yyvsp[0].node );
	    yyval.node->setIdent( yyvsp[-6].ident );
	    yyval.node->setStringLiteral( yyvsp[-4].ident );
	  ;
    break;}
case 154:
#line 1023 "yacc.yy"
{
	    yyval.node = NULL;
	  ;
    break;}
case 155:
#line 1027 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_if, yyvsp[-3].node, yyvsp[-1].node, yyvsp[0].node );
	  ;
    break;}
case 156:
#line 1031 "yacc.yy"
{
	    yyval.node = yyvsp[0].node;
	  ;
    break;}
case 157:
#line 1038 "yacc.yy"
{
	    yyval.node = yyvsp[-1].node;
	  ;
    break;}
case 158:
#line 1045 "yacc.yy"
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
#line 1049 "yacc.yy"


void kscriptParse( const char *_code, int extension )
{
    kscriptInitFlex( _code, extension );
    yyparse();
}
