
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



#define	YYFINAL		331
#define	YYFLAG		-32768
#define	YYNTBASE	75

#define YYTRANSLATE(x) ((unsigned)(x) <= 329 ? yytranslate[x] : 126)

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
    66,    67,    68,    69,    70,    71,    72,    73,    74
};

#if YYDEBUG != 0
static const short yyprhs[] = {     0,
     0,     1,     4,     6,     8,    11,    14,    17,    20,    23,
    27,    33,    39,    41,    43,    45,    49,    54,    57,    59,
    63,    65,    69,    71,    74,    78,    80,    84,    88,    92,
    96,   100,   104,   106,   108,   112,   114,   118,   120,   124,
   126,   130,   134,   136,   140,   144,   146,   150,   154,   158,
   161,   164,   167,   170,   172,   177,   182,   186,   191,   193,
   197,   199,   203,   205,   208,   211,   214,   217,   219,   222,
   225,   227,   229,   231,   235,   237,   239,   241,   243,   245,
   247,   249,   251,   254,   258,   261,   265,   269,   271,   279,
   285,   291,   293,   296,   299,   303,   306,   308,   312,   317,
   320,   324,   325,   327,   329,   332,   335,   338,   345,   351,
   360,   363,   369,   376,   378,   382,   385,   390,   393,   396,
   398,   402,   405,   410,   411,   413,   415,   418,   421,   427,
   431,   434,   438,   440,   446,   450,   452,   455,   465,   475,
   480,   483,   493,   498,   505,   513,   523,   524,   532,   535,
   540
};

static const short yyrhs[] = {    -1,
    47,    85,     0,    76,     0,    77,     0,    77,    76,     0,
   121,    33,     0,   102,    33,     0,   106,    33,     0,   111,
    29,     0,    66,    17,    33,     0,    74,    17,    66,     4,
    33,     0,    74,    17,    66,    78,    33,     0,    38,     0,
    79,     0,    17,     0,    17,     9,    78,     0,    58,    21,
   116,    29,     0,     8,    81,     0,    82,     0,    82,     9,
    81,     0,    83,     0,    82,    49,    17,     0,    17,     0,
    32,    17,     0,    85,    47,    84,     0,    85,     0,    86,
    13,    85,     0,    86,    48,    85,     0,    86,    45,    85,
     0,    86,    46,    85,     0,    86,    24,    85,     0,    86,
    16,    85,     0,    86,     0,    87,     0,    86,    43,    87,
     0,    88,     0,    87,     7,    88,     0,    89,     0,    88,
     3,    89,     0,    90,     0,    89,    35,    90,     0,    89,
    34,    90,     0,    91,     0,    90,    28,    91,     0,    90,
    25,    91,     0,    92,     0,    91,     4,    92,     0,    91,
    36,    92,     0,    91,    27,    92,     0,    25,    93,     0,
    28,    93,     0,    41,    93,     0,    62,    93,     0,    93,
     0,    93,    23,    84,    31,     0,    93,    21,    84,    29,
     0,    93,    22,    30,     0,    93,    22,    94,    30,     0,
    95,     0,    84,     9,    94,     0,    84,     0,    93,    49,
    17,     0,    96,     0,    56,    97,     0,    57,    97,     0,
    97,    56,     0,    97,    57,     0,    97,     0,    98,    61,
     0,    98,    60,     0,    98,     0,    83,     0,    99,     0,
    22,    84,    30,     0,    20,     0,    73,     0,    72,     0,
    37,     0,     6,     0,    15,     0,    42,     0,    14,     0,
    23,    31,     0,    23,   100,    31,     0,    21,    29,     0,
    21,   101,    29,     0,    84,     9,   100,     0,    84,     0,
    22,    84,     9,    85,    30,     9,   101,     0,    22,    84,
     9,    85,    30,     0,    39,    17,    21,   103,    29,     0,
   104,     0,   104,   103,     0,   111,    29,     0,    67,   105,
    33,     0,   121,    33,     0,    17,     0,    17,     9,   105,
     0,   107,    21,   108,    29,     0,    44,    17,     0,    44,
    17,    80,     0,     0,   109,     0,   110,     0,   110,   109,
     0,   111,    29,     0,   102,    33,     0,    64,    17,    22,
   114,    30,    33,     0,    64,    17,    22,    30,    33,     0,
    50,    22,    18,    17,    30,    21,   116,    29,     0,   121,
    33,     0,    17,    22,    30,    21,   116,     0,    17,    22,
   112,    30,    21,   116,     0,   113,     0,   113,     9,   112,
     0,    18,    17,     0,    18,    17,    47,    84,     0,    26,
    17,     0,    19,    17,     0,   115,     0,   115,     9,   112,
     0,    18,    17,     0,    18,    17,    47,    84,     0,     0,
   117,     0,   118,     0,   118,   117,     0,    84,    33,     0,
    71,    84,     9,    84,    33,     0,    65,    84,    33,     0,
    63,    33,     0,    63,    84,    33,     0,   122,     0,    70,
    21,   117,    29,   119,     0,    21,   117,    29,     0,   120,
     0,   120,   119,     0,    69,    22,    84,     9,    17,    30,
    21,   117,    29,     0,    11,    22,    17,     9,    17,    30,
    21,   117,    29,     0,    10,    17,    47,    85,     0,   124,
   125,     0,    54,    22,    84,    33,    84,    33,    84,    30,
   125,     0,    55,   125,   124,    33,     0,    52,    22,    84,
    30,   125,   123,     0,    59,    22,    17,     9,    84,    30,
   125,     0,    59,    22,    17,     9,    17,     9,    84,    30,
   125,     0,     0,    53,    52,    22,    84,    30,   125,   123,
     0,    53,   125,     0,    51,    22,    84,    30,     0,    21,
   116,    29,     0
};

#endif

#if YYDEBUG != 0
static const short yyrline[] = { 0,
   163,   167,   171,   178,   182,   191,   195,   199,   203,   207,
   212,   218,   224,   229,   236,   240,   249,   259,   267,   271,
   280,   284,   292,   297,   309,   313,   320,   324,   328,   332,
   336,   340,   344,   352,   356,   364,   368,   376,   380,   388,
   392,   396,   404,   408,   412,   420,   424,   428,   432,   440,
   444,   448,   452,   456,   463,   467,   471,   475,   479,   486,
   491,   498,   503,   510,   514,   518,   523,   528,   535,   540,
   545,   553,   557,   561,   570,   575,   580,   585,   590,   595,
   600,   605,   610,   614,   618,   622,   629,   633,   640,   644,
   654,   662,   666,   674,   678,   682,   689,   694,   703,   711,
   716,   726,   730,   737,   741,   749,   753,   757,   762,   767,
   776,   783,   789,   797,   801,   808,   813,   818,   823,   831,
   835,   842,   847,   855,   859,   866,   870,   878,   882,   886,
   890,   894,   898,   902,   906,   913,   917,   925,   931,   942,
   950,   954,   958,   962,   966,   971,   981,   985,   989,   996,
  1003
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
"T_UNKNOWN","T_CATCH","T_TRY","T_RAISE","T_RANGE","T_CELL","T_FROM","specification",
"definitions","definition","import_list","main","inheritance_spec","qualified_names",
"qualified_name","scoped_name","assign_expr","equal_expr","or_expr","xor_expr",
"and_expr","shift_expr","add_expr","mult_expr","unary_expr","index_expr","func_call_params",
"member_expr","incr_expr","match_expr","primary_expr","literal","array_elements",
"dict_elements","struct_dcl","struct_exports","struct_export","struct_members",
"class_dcl","class_header","class_body","class_exports","class_export","func_dcl",
"func_params","func_param","signal_params","signal_param","func_body","func_lines",
"func_line","catches","single_catch","const_dcl","loops","else","while","loop_body", NULL
};
#endif

static const short yyr1[] = {     0,
    75,    75,    75,    76,    76,    77,    77,    77,    77,    77,
    77,    77,    77,    77,    78,    78,    79,    80,    81,    81,
    82,    82,    83,    83,    84,    84,    85,    85,    85,    85,
    85,    85,    85,    86,    86,    87,    87,    88,    88,    89,
    89,    89,    90,    90,    90,    91,    91,    91,    91,    92,
    92,    92,    92,    92,    93,    93,    93,    93,    93,    94,
    94,    95,    95,    96,    96,    96,    96,    96,    97,    97,
    97,    98,    98,    98,    99,    99,    99,    99,    99,    99,
    99,    99,    99,    99,    99,    99,   100,   100,   101,   101,
   102,   103,   103,   104,   104,   104,   105,   105,   106,   107,
   107,   108,   108,   109,   109,   110,   110,   110,   110,   110,
   110,   111,   111,   112,   112,   113,   113,   113,   113,   114,
   114,   115,   115,   116,   116,   117,   117,   118,   118,   118,
   118,   118,   118,   118,   118,   119,   119,   120,   120,   121,
   122,   122,   122,   122,   122,   122,   123,   123,   123,   124,
   125
};

static const short yyr2[] = {     0,
     0,     2,     1,     1,     2,     2,     2,     2,     2,     3,
     5,     5,     1,     1,     1,     3,     4,     2,     1,     3,
     1,     3,     1,     2,     3,     1,     3,     3,     3,     3,
     3,     3,     1,     1,     3,     1,     3,     1,     3,     1,
     3,     3,     1,     3,     3,     1,     3,     3,     3,     2,
     2,     2,     2,     1,     4,     4,     3,     4,     1,     3,
     1,     3,     1,     2,     2,     2,     2,     1,     2,     2,
     1,     1,     1,     3,     1,     1,     1,     1,     1,     1,
     1,     1,     2,     3,     2,     3,     3,     1,     7,     5,
     5,     1,     2,     2,     3,     2,     1,     3,     4,     2,
     3,     0,     1,     1,     2,     2,     2,     6,     5,     8,
     2,     5,     6,     1,     3,     2,     4,     2,     2,     1,
     3,     2,     4,     0,     1,     1,     2,     2,     5,     3,
     2,     3,     1,     5,     3,     1,     2,     9,     9,     4,
     2,     9,     4,     6,     7,     9,     0,     7,     2,     4,
     3
};

static const short yydefact[] = {     1,
     0,     0,    13,     0,     0,     0,     0,     0,     0,     3,
     4,    14,     0,     0,     0,     0,     0,     0,     0,     0,
   100,    79,    82,    80,    23,    75,     0,     0,     0,     0,
     0,     0,    78,     0,    81,     0,     0,     0,    77,    76,
    72,     2,    33,    34,    36,    38,    40,    43,    46,    54,
    59,    63,    68,    71,    73,   124,     0,     0,     5,     7,
     8,   102,     9,     6,     0,     0,     0,     0,     0,     0,
   114,     0,     0,   101,     0,    85,     0,     0,    26,    83,
    88,     0,    50,    51,    24,    52,    64,    65,    53,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,    66,
    67,    70,    69,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,   125,   126,   133,     0,    10,
     0,     0,     0,     0,     0,   103,   104,     0,     0,   140,
   116,   119,   118,   124,     0,     0,     0,     0,    92,     0,
     0,    18,    19,    21,     0,    86,    74,     0,     0,    84,
    27,    32,    31,    35,    29,    30,    28,    37,    39,    42,
    41,    45,    44,    47,    49,    48,     0,    57,    61,     0,
     0,    62,     0,     0,     0,     0,     0,   124,     0,     0,
   131,     0,     0,     0,     0,   128,    17,   127,   141,     0,
    15,     0,     0,     0,   107,    99,   105,   106,   111,     0,
   112,   124,   115,    97,     0,    91,    93,    94,    96,     0,
     0,     0,    25,    87,    56,     0,    58,    55,     0,   135,
     0,     0,     0,     0,     0,     0,   132,   130,     0,     0,
    11,     0,    12,     0,     0,   117,   113,     0,    95,    20,
    22,     0,    60,   150,     0,     0,   151,   143,     0,     0,
     0,    16,     0,     0,     0,     0,   120,    98,    90,   147,
     0,    23,     0,     0,     0,   134,   136,   129,     0,   122,
   109,     0,     0,     0,     0,   144,     0,     0,     0,     0,
     0,   137,   124,     0,   108,   121,    89,     0,   149,     0,
     0,   145,     0,     0,     0,   123,     0,     0,     0,     0,
     0,   110,     0,   142,   146,     0,     0,     0,     0,     0,
   147,     0,     0,   148,     0,     0,   139,   138,     0,     0,
     0
};

static const short yydefgoto[] = {   329,
    10,    11,   202,    12,    74,   152,   153,    41,   124,    79,
    43,    44,    45,    46,    47,    48,    49,    50,   180,    51,
    52,    53,    54,    55,    82,    77,    13,   148,   149,   215,
    14,    15,   135,   136,   137,    16,    70,    71,   266,   267,
   125,   126,   127,   276,   277,    17,   128,   286,   129,   189
};

static const short yypact[] = {    47,
    15,    24,-32768,    17,    45,   467,    -1,    49,    54,-32768,
    98,-32768,    57,    71,    95,    91,    96,    93,    26,   114,
   135,-32768,-32768,-32768,-32768,-32768,    52,   467,   353,   537,
   537,   128,-32768,   537,-32768,    16,    16,   537,-32768,-32768,
-32768,-32768,   101,   141,   148,    41,    -2,    14,-32768,    38,
-32768,-32768,    50,    63,-32768,   293,   119,    87,-32768,-32768,
-32768,     4,-32768,-32768,   467,   143,   144,   146,   147,   137,
   156,     0,    -5,-32768,   467,-32768,   140,   145,   123,-32768,
   162,   142,    38,    38,-32768,    38,-32768,-32768,    38,   467,
   467,   467,   467,   467,   467,   467,   467,   467,   467,   467,
   467,   467,   467,   467,   467,   467,   391,   467,   157,-32768,
-32768,-32768,-32768,   233,   154,   155,   158,   160,   161,   429,
   467,   163,   467,   159,   149,-32768,   293,-32768,   160,-32768,
    12,   165,   171,   164,   150,-32768,     4,   167,   166,-32768,
   153,-32768,-32768,   293,   168,    51,   173,   172,     0,   174,
   169,-32768,     2,-32768,   182,-32768,-32768,   467,   467,-32768,
-32768,-32768,-32768,   141,-32768,-32768,-32768,   148,    41,    -2,
    -2,    14,    14,-32768,-32768,-32768,   175,-32768,   184,   176,
   177,-32768,   467,   178,   467,   467,   467,   293,   170,   188,
-32768,   180,   181,   293,   185,-32768,-32768,-32768,-32768,   183,
   200,   186,   197,   195,-32768,-32768,-32768,-32768,-32768,   467,
-32768,   293,-32768,   209,   187,-32768,-32768,-32768,-32768,    -5,
   205,   467,-32768,-32768,-32768,   467,-32768,-32768,    33,-32768,
   193,   194,   192,   198,   199,   217,-32768,-32768,   201,   467,
-32768,   216,-32768,   218,    10,-32768,-32768,   173,-32768,-32768,
-32768,   204,-32768,-32768,   160,   467,-32768,-32768,   505,    -4,
   203,-32768,   207,   221,   208,   210,   234,-32768,   235,   189,
   212,   237,   219,   229,   230,-32768,    -4,-32768,   236,   220,
-32768,   231,    51,   246,     3,-32768,   467,   467,   160,   252,
   467,-32768,   293,   467,-32768,-32768,-32768,   249,-32768,   242,
   243,-32768,   267,   268,   251,-32768,   467,   160,   160,   261,
   264,-32768,   253,-32768,-32768,   256,   263,   160,   270,   273,
   189,   293,   293,-32768,   271,   272,-32768,-32768,   282,   297,
-32768
};

static const short yypgoto[] = {-32768,
   291,-32768,    67,-32768,-32768,    92,-32768,   -70,   -28,     7,
-32768,   224,   214,   222,    28,    32,     6,    88,    97,-32768,
-32768,   102,-32768,-32768,   179,    35,   -54,   190,-32768,    74,
-32768,-32768,-32768,   191,-32768,   -53,  -142,-32768,-32768,-32768,
  -139,  -112,-32768,    55,-32768,   -37,-32768,     5,   138,  -123
};


#define	YYLAST		610


static const short yytable[] = {    78,
    81,   184,   154,   213,   211,   199,   274,   134,   138,     1,
   220,    25,    42,     1,   198,   200,     2,   103,   150,    56,
     2,    22,   101,   188,   139,   102,    32,   264,   201,    23,
    24,    18,    25,    20,   151,    26,    27,    28,    29,   265,
   104,   222,     4,    66,    67,    19,   155,    32,   234,   105,
   221,    68,    33,   132,   298,    69,     1,    35,   106,   107,
   108,    21,   157,     2,   275,    57,   147,   133,    66,    67,
    58,   140,   247,    75,    99,   100,    68,   177,   179,   181,
    76,   239,   134,   138,     3,     4,   109,    39,    40,    60,
     5,   192,   193,     6,   195,   150,   161,   162,   163,   139,
   165,   166,   167,    61,     7,   110,   111,     1,   174,   175,
   176,   151,     8,    90,     2,    62,    91,    83,    84,    63,
     9,    86,   112,   113,    92,    89,   170,   171,    64,   223,
    81,   270,   172,   173,    72,     3,     4,    87,    88,    65,
   296,     5,    73,    93,    85,    94,    95,    97,    96,   154,
    98,   130,   131,   305,   229,     7,   231,   232,   233,   141,
   142,   299,   143,     8,   146,   302,   145,   144,   156,   158,
   159,     9,   160,   182,   157,   185,   186,   197,   206,   187,
   188,   246,   190,   194,   314,   315,   203,   204,   212,   214,
   222,   196,   226,   240,   321,   208,   205,   179,   209,   210,
   216,   219,   218,   225,   236,   227,   230,   228,   242,   325,
   326,   261,   237,   238,   244,   241,   245,   248,   243,   249,
   115,   251,   254,   255,   256,   259,   257,   271,   252,   260,
   273,   258,   201,   269,   263,   278,   279,   280,    22,   282,
   281,   285,   283,   284,   287,   288,    23,    24,   289,    25,
   290,   291,    26,   114,   183,    29,   293,    30,   300,   301,
    31,    76,   304,   295,    32,   306,   294,    75,   303,    33,
   307,   308,   309,    34,    35,   310,   311,   316,   313,   312,
   317,   330,   318,   115,   116,   319,   117,   118,    36,    37,
   322,   119,   320,   323,    38,   120,   331,   121,    22,   327,
   328,    59,   122,   123,    39,    40,    23,    24,   262,    25,
   168,   250,    26,   114,    28,    29,   164,    30,   297,   169,
    31,   268,   253,     0,    32,   324,   235,   207,     0,    33,
     0,   292,     0,    34,    35,     0,     0,   224,   217,     0,
     0,     0,     0,   115,   116,     0,   117,   118,    36,    37,
     0,   119,     0,     0,    38,   120,     0,   121,    22,     0,
     0,     0,   122,   123,    39,    40,    23,    24,     0,    25,
     0,     0,    26,    27,    28,    29,     0,    30,     0,     0,
    31,     0,     0,    80,    32,     0,     0,     0,     0,    33,
     0,     0,     0,    34,    35,     0,    22,     0,     0,     0,
     0,     0,     0,     0,    23,    24,     0,    25,    36,    37,
    26,    27,    28,    29,    38,    30,     0,     0,    31,     0,
   178,     0,    32,     0,    39,    40,     0,    33,     0,     0,
     0,    34,    35,     0,    22,     0,     0,     0,     0,     0,
     0,     0,    23,    24,     0,    25,    36,    37,    26,    27,
    28,    29,    38,    30,     0,     0,    31,     0,     0,     0,
    32,   191,    39,    40,     0,    33,     0,     0,     0,    34,
    35,     0,    22,     0,     0,     0,     0,     0,     0,     0,
    23,    24,     0,    25,    36,    37,    26,    27,    28,    29,
    38,    30,     0,     0,    31,     0,     0,     0,    32,     0,
    39,    40,     0,    33,     0,     0,     0,    34,    35,     0,
    22,     0,     0,     0,     0,     0,     0,     0,    23,    24,
     0,   272,    36,    37,    26,    27,    28,    29,    38,    30,
     0,     0,    31,     0,     0,     0,    32,     0,    39,    40,
     0,    33,    22,     0,     0,    34,    35,     0,     0,     0,
    23,    24,     0,    25,     0,     0,    26,    27,    28,    29,
    36,    37,     0,     0,     0,     0,    38,     0,    32,     0,
     0,     0,     0,    33,     0,     0,    39,    40,    35,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,    36,    37,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,    39,    40
};

static const short yycheck[] = {    28,
    29,   114,    73,   146,   144,   129,    11,    62,    62,    10,
     9,    17,     6,    10,   127,     4,    17,     4,    72,    21,
    17,     6,    25,    21,    62,    28,    32,    18,    17,    14,
    15,    17,    17,    17,    72,    20,    21,    22,    23,    30,
    27,     9,    39,    18,    19,    22,    75,    32,   188,    36,
    49,    26,    37,    50,    52,    30,    10,    42,    21,    22,
    23,    17,    30,    17,    69,    17,    67,    64,    18,    19,
    17,    65,   212,    22,    34,    35,    26,   106,   107,   108,
    29,   194,   137,   137,    38,    39,    49,    72,    73,    33,
    44,   120,   121,    47,   123,   149,    90,    91,    92,   137,
    94,    95,    96,    33,    58,    56,    57,    10,   103,   104,
   105,   149,    66,    13,    17,    21,    16,    30,    31,    29,
    74,    34,    60,    61,    24,    38,    99,   100,    33,   158,
   159,   255,   101,   102,    21,    38,    39,    36,    37,    47,
   283,    44,     8,    43,    17,    45,    46,     7,    48,   220,
     3,    33,    66,   293,   183,    58,   185,   186,   187,    17,
    17,   285,    17,    66,     9,   289,    30,    21,    29,    47,
     9,    74,    31,    17,    30,    22,    22,    29,    29,    22,
    21,   210,    22,    21,   308,   309,    22,    17,    21,    17,
     9,    33,     9,     9,   318,    29,    33,   226,    33,    47,
    29,    33,    29,    29,    17,    30,    29,    31,     9,   322,
   323,   240,    33,    33,    18,    33,    22,     9,    33,    33,
    51,    17,    30,    30,    33,     9,    29,   256,   222,    29,
   259,    33,    17,    30,    17,    33,    30,    17,     6,    30,
    33,    53,     9,     9,    33,     9,    14,    15,    30,    17,
    22,    22,    20,    21,    22,    23,    21,    25,   287,   288,
    28,    29,   291,    33,    32,   294,    47,    22,    17,    37,
    22,    30,    30,    41,    42,     9,     9,    17,   307,    29,
    17,     0,    30,    51,    52,    30,    54,    55,    56,    57,
    21,    59,    30,    21,    62,    63,     0,    65,     6,    29,
    29,    11,    70,    71,    72,    73,    14,    15,   242,    17,
    97,   220,    20,    21,    22,    23,    93,    25,   284,    98,
    28,   248,   226,    -1,    32,   321,   189,   137,    -1,    37,
    -1,   277,    -1,    41,    42,    -1,    -1,   159,   149,    -1,
    -1,    -1,    -1,    51,    52,    -1,    54,    55,    56,    57,
    -1,    59,    -1,    -1,    62,    63,    -1,    65,     6,    -1,
    -1,    -1,    70,    71,    72,    73,    14,    15,    -1,    17,
    -1,    -1,    20,    21,    22,    23,    -1,    25,    -1,    -1,
    28,    -1,    -1,    31,    32,    -1,    -1,    -1,    -1,    37,
    -1,    -1,    -1,    41,    42,    -1,     6,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    14,    15,    -1,    17,    56,    57,
    20,    21,    22,    23,    62,    25,    -1,    -1,    28,    -1,
    30,    -1,    32,    -1,    72,    73,    -1,    37,    -1,    -1,
    -1,    41,    42,    -1,     6,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    14,    15,    -1,    17,    56,    57,    20,    21,
    22,    23,    62,    25,    -1,    -1,    28,    -1,    -1,    -1,
    32,    33,    72,    73,    -1,    37,    -1,    -1,    -1,    41,
    42,    -1,     6,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    14,    15,    -1,    17,    56,    57,    20,    21,    22,    23,
    62,    25,    -1,    -1,    28,    -1,    -1,    -1,    32,    -1,
    72,    73,    -1,    37,    -1,    -1,    -1,    41,    42,    -1,
     6,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    14,    15,
    -1,    17,    56,    57,    20,    21,    22,    23,    62,    25,
    -1,    -1,    28,    -1,    -1,    -1,    32,    -1,    72,    73,
    -1,    37,     6,    -1,    -1,    41,    42,    -1,    -1,    -1,
    14,    15,    -1,    17,    -1,    -1,    20,    21,    22,    23,
    56,    57,    -1,    -1,    -1,    -1,    62,    -1,    32,    -1,
    -1,    -1,    -1,    37,    -1,    -1,    72,    73,    42,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    56,    57,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    72,    73
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
#line 164 "yacc.yy"
{
	    theParser->setRootNode( NULL );
	  ;
    break;}
case 2:
#line 168 "yacc.yy"
{
	    theParser->setRootNode( yyvsp[0].node );
	  ;
    break;}
case 3:
#line 172 "yacc.yy"
{
	    theParser->setRootNode( new KSParseNode( definitions, yyvsp[0].node ) );
	  ;
    break;}
case 4:
#line 179 "yacc.yy"
{
	    yyval.node = new KSParseNode( definitions, yyvsp[0].node );
	  ;
    break;}
case 5:
#line 183 "yacc.yy"
{
	    yyval.node = new KSParseNode( definitions, yyvsp[-1].node );
	    yyval.node->setBranch( 2, yyvsp[0].node );
	  ;
    break;}
case 6:
#line 192 "yacc.yy"
{
	    yyval.node = yyvsp[-1].node;
	  ;
    break;}
case 7:
#line 196 "yacc.yy"
{
	    yyval.node = yyvsp[-1].node;
	  ;
    break;}
case 8:
#line 200 "yacc.yy"
{
	    yyval.node = yyvsp[-1].node;
	  ;
    break;}
case 9:
#line 204 "yacc.yy"
{
	    yyval.node = yyvsp[-1].node;
	  ;
    break;}
case 10:
#line 208 "yacc.yy"
{
	    yyval.node = new KSParseNode( import );
	    yyval.node->setIdent( yyvsp[-1].ident );
	  ;
    break;}
case 11:
#line 213 "yacc.yy"
{
	    yyval.node = new KSParseNode( from );
	    yyval.node->setIdent( yyvsp[-3].ident );
	    yyval.node->setStringLiteral( QString( "" ) );
	  ;
    break;}
case 12:
#line 219 "yacc.yy"
{
	    yyval.node = new KSParseNode( from );
	    yyval.node->setIdent( yyvsp[-3].ident );
	    yyval.node->setStringLiteral( yyvsp[-1]._str );
	  ;
    break;}
case 13:
#line 225 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_pragma );
	    yyval.node->setIdent( yyvsp[0]._str );
	  ;
    break;}
case 14:
#line 230 "yacc.yy"
{
	    yyval.node = yyvsp[0].node;
	  ;
    break;}
case 15:
#line 237 "yacc.yy"
{
		yyval._str = yyvsp[0].ident;
	  ;
    break;}
case 16:
#line 241 "yacc.yy"
{
		(*yyvsp[-2].ident) += "/";
		(*yyvsp[-2].ident) += (*yyvsp[0]._str);
		yyval._str = yyvsp[-2].ident;
	  ;
    break;}
case 17:
#line 250 "yacc.yy"
{
	    yyval.node = new KSParseNode( func_dcl );
	    yyval.node->setBranch( 2, yyvsp[-1].node );
	    yyval.node->setIdent( "main" );
	  ;
    break;}
case 18:
#line 260 "yacc.yy"
{
	    yyval.node = yyvsp[0].node;
	  ;
    break;}
case 19:
#line 268 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_qualified_names, yyvsp[0].node );
	  ;
    break;}
case 20:
#line 272 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_qualified_names, yyvsp[-2].node );
	    yyval.node->setBranch( 2, yyvsp[0].node );
	  ;
    break;}
case 21:
#line 281 "yacc.yy"
{
	    yyval.node = yyvsp[0].node;
	  ;
    break;}
case 22:
#line 285 "yacc.yy"
{
	    yyval.node = new KSParseNode( member_expr, yyvsp[-2].node );
	    yyval.node->setIdent( yyvsp[0].ident );
	  ;
    break;}
case 23:
#line 293 "yacc.yy"
{
	    yyval.node = new KSParseNode( scoped_name );
	    yyval.node->setIdent( yyvsp[0].ident );
	  ;
    break;}
case 24:
#line 298 "yacc.yy"
{
	    yyval.node = new KSParseNode( scoped_name );
	    QString name = "::";
	    name += *(yyvsp[0].ident);
	    delete yyvsp[0].ident;
	    yyval.node->setIdent( name );
	  ;
    break;}
case 25:
#line 310 "yacc.yy"
{
	    yyval.node = new KSParseNode( assign_expr, yyvsp[-2].node, yyvsp[0].node );
	  ;
    break;}
case 26:
#line 314 "yacc.yy"
{
	    yyval.node = yyvsp[0].node;
	  ;
    break;}
case 27:
#line 321 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_equal, yyvsp[-2].node, yyvsp[0].node );
	  ;
    break;}
case 28:
#line 325 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_notequal, yyvsp[-2].node, yyvsp[0].node );
	  ;
    break;}
case 29:
#line 329 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_less_or_equal, yyvsp[-2].node, yyvsp[0].node );
	  ;
    break;}
case 30:
#line 333 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_greater_or_equal, yyvsp[-2].node, yyvsp[0].node );
	  ;
    break;}
case 31:
#line 337 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_less, yyvsp[-2].node, yyvsp[0].node );
	  ;
    break;}
case 32:
#line 341 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_greater, yyvsp[-2].node, yyvsp[0].node );
	  ;
    break;}
case 33:
#line 345 "yacc.yy"
{
	    yyval.node = yyvsp[0].node;
	  ;
    break;}
case 34:
#line 353 "yacc.yy"
{
	    yyval.node = yyvsp[0].node;
	  ;
    break;}
case 35:
#line 357 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_vertical_line, yyvsp[-2].node, yyvsp[0].node );
	  ;
    break;}
case 36:
#line 365 "yacc.yy"
{
	    yyval.node = yyvsp[0].node;
	  ;
    break;}
case 37:
#line 369 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_circumflex, yyvsp[-2].node, yyvsp[0].node );
	  ;
    break;}
case 38:
#line 377 "yacc.yy"
{
	    yyval.node = yyvsp[0].node;
	  ;
    break;}
case 39:
#line 381 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_ampersand, yyvsp[-2].node, yyvsp[0].node );
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
	    yyval.node = new KSParseNode( t_shiftright, yyvsp[-2].node, yyvsp[0].node );
	  ;
    break;}
case 42:
#line 397 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_shiftleft, yyvsp[-2].node, yyvsp[0].node );
	  ;
    break;}
case 43:
#line 405 "yacc.yy"
{
	    yyval.node = yyvsp[0].node;
	  ;
    break;}
case 44:
#line 409 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_plus_sign, yyvsp[-2].node, yyvsp[0].node );
	  ;
    break;}
case 45:
#line 413 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_minus_sign, yyvsp[-2].node, yyvsp[0].node );
	  ;
    break;}
case 46:
#line 421 "yacc.yy"
{
	    yyval.node = yyvsp[0].node;
	  ;
    break;}
case 47:
#line 425 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_asterik, yyvsp[-2].node, yyvsp[0].node );
	  ;
    break;}
case 48:
#line 429 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_solidus, yyvsp[-2].node, yyvsp[0].node );
	  ;
    break;}
case 49:
#line 433 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_percent_sign, yyvsp[-2].node, yyvsp[0].node );
	  ;
    break;}
case 50:
#line 441 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_minus_sign, yyvsp[0].node );
	  ;
    break;}
case 51:
#line 445 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_plus_sign, yyvsp[0].node );
	  ;
    break;}
case 52:
#line 449 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_tilde, yyvsp[0].node );
	  ;
    break;}
case 53:
#line 453 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_not, yyvsp[0].node );
	  ;
    break;}
case 54:
#line 457 "yacc.yy"
{
	    yyval.node = yyvsp[0].node;
	  ;
    break;}
case 55:
#line 464 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_array, yyvsp[-3].node, yyvsp[-1].node );
	  ;
    break;}
case 56:
#line 468 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_dict, yyvsp[-3].node, yyvsp[-1].node );
	  ;
    break;}
case 57:
#line 472 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_func_call, yyvsp[-2].node );
	  ;
    break;}
case 58:
#line 476 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_func_call, yyvsp[-3].node, yyvsp[-1].node );
	  ;
    break;}
case 59:
#line 480 "yacc.yy"
{
	    yyval.node = yyvsp[0].node;
	  ;
    break;}
case 60:
#line 487 "yacc.yy"
{
	    yyval.node = new KSParseNode( func_call_params, yyvsp[-2].node, yyvsp[0].node );
	  ;
    break;}
case 61:
#line 492 "yacc.yy"
{
	    yyval.node = new KSParseNode( func_call_params, yyvsp[0].node );
	  ;
    break;}
case 62:
#line 499 "yacc.yy"
{
	    yyval.node = new KSParseNode( member_expr, yyvsp[-2].node );
	    yyval.node->setIdent( yyvsp[0].ident );
	  ;
    break;}
case 63:
#line 504 "yacc.yy"
{
	    yyval.node = yyvsp[0].node;
	  ;
    break;}
case 64:
#line 511 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_incr, yyvsp[0].node );
	  ;
    break;}
case 65:
#line 515 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_decr, yyvsp[0].node );
	  ;
    break;}
case 66:
#line 519 "yacc.yy"
{
	    /* Setting $1 twice indicates that this is a postfix operator */
	    yyval.node = new KSParseNode( t_incr, yyvsp[-1].node, yyvsp[-1].node );
	  ;
    break;}
case 67:
#line 524 "yacc.yy"
{
	    /* Setting $1 twice indicates that this is a postfix operator */
	    yyval.node = new KSParseNode( t_decr, yyvsp[-1].node, yyvsp[-1].node );
	  ;
    break;}
case 68:
#line 529 "yacc.yy"
{
	    yyval.node = yyvsp[0].node;
	  ;
    break;}
case 69:
#line 536 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_match, yyvsp[-1].node );
	    yyval.node->setIdent( yyvsp[0].ident );
	  ;
    break;}
case 70:
#line 541 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_subst, yyvsp[-1].node );
	    yyval.node->setIdent( yyvsp[0].ident );
	  ;
    break;}
case 71:
#line 546 "yacc.yy"
{
	    yyval.node = yyvsp[0].node;
	  ;
    break;}
case 72:
#line 554 "yacc.yy"
{
	    yyval.node = yyvsp[0].node;
	  ;
    break;}
case 73:
#line 558 "yacc.yy"
{
	    yyval.node = yyvsp[0].node;
	  ;
    break;}
case 74:
#line 562 "yacc.yy"
{
	    yyval.node = yyvsp[-1].node;
	  ;
    break;}
case 75:
#line 571 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_integer_literal );
	    yyval.node->setIntegerLiteral( yyvsp[0]._int );
	  ;
    break;}
case 76:
#line 576 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_cell );
	    yyval.node->setStringLiteral( yyvsp[0]._str );
	  ;
    break;}
case 77:
#line 581 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_range );
	    yyval.node->setStringLiteral( yyvsp[0]._str );
	  ;
    break;}
case 78:
#line 586 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_string_literal );
	    yyval.node->setStringLiteral( yyvsp[0]._str );
	  ;
    break;}
case 79:
#line 591 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_character_literal );
	    yyval.node->setCharacterLiteral( yyvsp[0]._char );
	  ;
    break;}
case 80:
#line 596 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_floating_pt_literal );
	    yyval.node->setFloatingPtLiteral( yyvsp[0]._float );
	  ;
    break;}
case 81:
#line 601 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_boolean_literal );
	    yyval.node->setBooleanLiteral( true );
	  ;
    break;}
case 82:
#line 606 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_boolean_literal );
	    yyval.node->setBooleanLiteral( false );
	  ;
    break;}
case 83:
#line 611 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_array_const );
	  ;
    break;}
case 84:
#line 615 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_array_const, yyvsp[-1].node );
	  ;
    break;}
case 85:
#line 619 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_dict_const );
	  ;
    break;}
case 86:
#line 623 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_dict_const, yyvsp[-1].node );
	  ;
    break;}
case 87:
#line 630 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_array_element, yyvsp[-2].node, yyvsp[0].node );
	  ;
    break;}
case 88:
#line 634 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_array_element, yyvsp[0].node  );
	  ;
    break;}
case 89:
#line 641 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_dict_element, yyvsp[-5].node, yyvsp[-3].node, yyvsp[0].node );
	  ;
    break;}
case 90:
#line 645 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_dict_element, yyvsp[-3].node, yyvsp[-1].node );
	  ;
    break;}
case 91:
#line 655 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_struct, yyvsp[-1].node );
	    yyval.node->setIdent( yyvsp[-3].ident );
	  ;
    break;}
case 92:
#line 663 "yacc.yy"
{
	    yyval.node = new KSParseNode( exports, yyvsp[0].node );
	  ;
    break;}
case 93:
#line 667 "yacc.yy"
{
	    yyval.node = new KSParseNode( exports, yyvsp[-1].node );
	    yyval.node->setBranch( 2, yyvsp[0].node );
	  ;
    break;}
case 94:
#line 675 "yacc.yy"
{
	    yyval.node = yyvsp[-1].node;
	  ;
    break;}
case 95:
#line 679 "yacc.yy"
{
	    yyval.node = yyvsp[-1].node;
	  ;
    break;}
case 96:
#line 683 "yacc.yy"
{
	    yyval.node = yyvsp[-1].node;
	  ;
    break;}
case 97:
#line 690 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_struct_members );
	    yyval.node->setIdent( yyvsp[0].ident );
	  ;
    break;}
case 98:
#line 695 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_struct_members, yyvsp[0].node );
	    yyval.node->setIdent( yyvsp[-2].ident );
	  ;
    break;}
case 99:
#line 704 "yacc.yy"
{
	    yyval.node = new KSParseNode( class_dcl, yyvsp[-3].node, yyvsp[-1].node );
	  ;
    break;}
case 100:
#line 712 "yacc.yy"
{
	    yyval.node = new KSParseNode( class_header );
	    yyval.node->setIdent( yyvsp[0].ident );
	  ;
    break;}
case 101:
#line 717 "yacc.yy"
{
	    yyval.node = new KSParseNode( class_header );
	    yyval.node->setIdent( yyvsp[-1].ident );
	    yyval.node->setBranch( 1, yyvsp[0].node );
	  ;
    break;}
case 102:
#line 727 "yacc.yy"
{
	    yyval.node = NULL;
	  ;
    break;}
case 103:
#line 731 "yacc.yy"
{
	    yyval.node = yyvsp[0].node;
	  ;
    break;}
case 104:
#line 738 "yacc.yy"
{
	    yyval.node = new KSParseNode( exports, yyvsp[0].node );
	  ;
    break;}
case 105:
#line 742 "yacc.yy"
{
	    yyval.node = new KSParseNode( exports, yyvsp[-1].node );
	    yyval.node->setBranch( 2, yyvsp[0].node );
	  ;
    break;}
case 106:
#line 750 "yacc.yy"
{
	    yyval.node = yyvsp[-1].node;
	  ;
    break;}
case 107:
#line 754 "yacc.yy"
{
	    yyval.node = yyvsp[-1].node;
	  ;
    break;}
case 108:
#line 758 "yacc.yy"
{
	    yyval.node = new KSParseNode( signal_dcl, yyvsp[-2].node );
	    yyval.node->setIdent( yyvsp[-4].ident );
	  ;
    break;}
case 109:
#line 763 "yacc.yy"
{
	    yyval.node = new KSParseNode( signal_dcl, 0 );
	    yyval.node->setIdent( yyvsp[-3].ident );
	  ;
    break;}
case 110:
#line 768 "yacc.yy"
{
	    yyval.node = new KSParseNode( destructor_dcl );
	    KSParseNode* n = new KSParseNode( func_param_in );
	    n->setIdent( yyvsp[-4].ident );
	    yyval.node->setBranch( 1, n );
	    yyval.node->setBranch( 2, yyvsp[-1].node );
	    yyval.node->setIdent( "delete" );
	  ;
    break;}
case 111:
#line 777 "yacc.yy"
{
	    yyval.node = yyvsp[-1].node;
	  ;
    break;}
case 112:
#line 784 "yacc.yy"
{
	    yyval.node = new KSParseNode( func_dcl );
	    yyval.node->setBranch( 2, yyvsp[0].node );
	    yyval.node->setIdent( yyvsp[-4].ident );
	  ;
    break;}
case 113:
#line 790 "yacc.yy"
{
	    yyval.node = new KSParseNode( func_dcl, yyvsp[-3].node, yyvsp[0].node );
	    yyval.node->setIdent( yyvsp[-5].ident );
	  ;
    break;}
case 114:
#line 798 "yacc.yy"
{
	    yyval.node = new KSParseNode( func_params, yyvsp[0].node );
	  ;
    break;}
case 115:
#line 802 "yacc.yy"
{
	    yyval.node = new KSParseNode( func_params, yyvsp[-2].node, yyvsp[0].node );
	  ;
    break;}
case 116:
#line 809 "yacc.yy"
{
	    yyval.node = new KSParseNode( func_param_in );
	    yyval.node->setIdent( yyvsp[0].ident );
	  ;
    break;}
case 117:
#line 814 "yacc.yy"
{
	    yyval.node = new KSParseNode( func_param_in, yyvsp[0].node );
	    yyval.node->setIdent( yyvsp[-2].ident );
	  ;
    break;}
case 118:
#line 819 "yacc.yy"
{
	    yyval.node = new KSParseNode( func_param_out );
	    yyval.node->setIdent( yyvsp[0].ident );
	  ;
    break;}
case 119:
#line 824 "yacc.yy"
{
	    yyval.node = new KSParseNode( func_param_inout );
	    yyval.node->setIdent( yyvsp[0].ident );
	  ;
    break;}
case 120:
#line 832 "yacc.yy"
{
	    yyval.node = new KSParseNode( signal_params, yyvsp[0].node );
	  ;
    break;}
case 121:
#line 836 "yacc.yy"
{
	    yyval.node = new KSParseNode( signal_params, yyvsp[-2].node, yyvsp[0].node );
	  ;
    break;}
case 122:
#line 843 "yacc.yy"
{
	    yyval.node = new KSParseNode( signal_param );
	    yyval.node->setIdent( yyvsp[0].ident );
	  ;
    break;}
case 123:
#line 848 "yacc.yy"
{
	    yyval.node = new KSParseNode( signal_param, yyvsp[0].node );
	    yyval.node->setIdent( yyvsp[-2].ident );
	  ;
    break;}
case 124:
#line 856 "yacc.yy"
{
	    yyval.node = NULL;
	  ;
    break;}
case 125:
#line 860 "yacc.yy"
{
	    yyval.node = yyvsp[0].node;
	  ;
    break;}
case 126:
#line 867 "yacc.yy"
{
	    yyval.node = new KSParseNode( func_lines, yyvsp[0].node );
	  ;
    break;}
case 127:
#line 871 "yacc.yy"
{
	    yyval.node = new KSParseNode( func_lines, yyvsp[-1].node );
	    yyval.node->setBranch( 2, yyvsp[0].node );
	  ;
    break;}
case 128:
#line 879 "yacc.yy"
{
	    yyval.node = yyvsp[-1].node;
	  ;
    break;}
case 129:
#line 883 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_raise, yyvsp[-3].node, yyvsp[-1].node );
	  ;
    break;}
case 130:
#line 887 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_emit, yyvsp[-1].node );
	  ;
    break;}
case 131:
#line 891 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_return );
	  ;
    break;}
case 132:
#line 895 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_return, yyvsp[-1].node );
	  ;
    break;}
case 133:
#line 899 "yacc.yy"
{
	    yyval.node = yyvsp[0].node;
	  ;
    break;}
case 134:
#line 903 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_try, yyvsp[-2].node, yyvsp[0].node );
	  ;
    break;}
case 135:
#line 907 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_scope, yyvsp[-1].node );
	  ;
    break;}
case 136:
#line 914 "yacc.yy"
{
	    yyval.node = yyvsp[0].node;
	  ;
    break;}
case 137:
#line 918 "yacc.yy"
{
	    yyval.node = yyvsp[-1].node;
	    yyval.node->setBranch( 4, yyvsp[0].node );
	  ;
    break;}
case 138:
#line 927 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_catch, yyvsp[-6].node, yyvsp[-1].node );
	    yyval.node->setIdent( yyvsp[-4].ident );
	  ;
    break;}
case 139:
#line 933 "yacc.yy"
{
	    KSParseNode* x = new KSParseNode( scoped_name );
	    x->setIdent( yyvsp[-6].ident );
	    yyval.node = new KSParseNode( t_catch_default, x, yyvsp[-1].node );
	    yyval.node->setIdent( yyvsp[-4].ident );
	  ;
    break;}
case 140:
#line 943 "yacc.yy"
{
	    yyval.node = new KSParseNode( const_dcl, yyvsp[0].node );
	    yyval.node->setIdent( yyvsp[-2].ident );
	  ;
    break;}
case 141:
#line 951 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_while, yyvsp[-1].node, yyvsp[0].node );
	  ;
    break;}
case 142:
#line 955 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_for, yyvsp[-6].node, yyvsp[-4].node, yyvsp[-2].node, yyvsp[0].node );
	  ;
    break;}
case 143:
#line 959 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_do, yyvsp[-2].node, yyvsp[-1].node );
	  ;
    break;}
case 144:
#line 963 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_if, yyvsp[-3].node, yyvsp[-1].node, yyvsp[0].node );
	  ;
    break;}
case 145:
#line 967 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_foreach, yyvsp[-2].node, yyvsp[0].node );
	    yyval.node->setIdent( yyvsp[-4].ident );
	  ;
    break;}
case 146:
#line 972 "yacc.yy"
{
	    /* We set $9 twice to indicate thet this is the foreach for maps */
	    yyval.node = new KSParseNode( t_foreach, yyvsp[-2].node, yyvsp[0].node, yyvsp[0].node );
	    yyval.node->setIdent( yyvsp[-6].ident );
	    yyval.node->setStringLiteral( yyvsp[-4].ident );
	  ;
    break;}
case 147:
#line 982 "yacc.yy"
{
	    yyval.node = NULL;
	  ;
    break;}
case 148:
#line 986 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_if, yyvsp[-3].node, yyvsp[-1].node, yyvsp[0].node );
	  ;
    break;}
case 149:
#line 990 "yacc.yy"
{
	    yyval.node = yyvsp[0].node;
	  ;
    break;}
case 150:
#line 997 "yacc.yy"
{
	    yyval.node = yyvsp[-1].node;
	  ;
    break;}
case 151:
#line 1004 "yacc.yy"
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
#line 1008 "yacc.yy"


void kscriptParse( const char *_code, int extension )
{
    kscriptInitFlex( _code, extension );
    yyparse();
}
