
/*  A Bison parser, made from yacc.yy
 by  GNU Bison version 1.25
  */

#define YYBISON 1  /* Identify Bison output.  */

#define	T_AMPERSAND	258
#define	T_ASTERISK	259
#define	T_ATTRIBUTE	260
#define	T_CASE	261
#define	T_CHARACTER_LITERAL	262
#define	T_CIRCUMFLEX	263
#define	T_COLON	264
#define	T_COMMA	265
#define	T_CONST	266
#define	T_DEFAULT	267
#define	T_ENUM	268
#define	T_EQUAL	269
#define	T_FALSE	270
#define	T_FLOATING_PT_LITERAL	271
#define	T_GREATER_THAN_SIGN	272
#define	T_IDENTIFIER	273
#define	T_IN	274
#define	T_INOUT	275
#define	T_INTEGER_LITERAL	276
#define	T_INTERFACE	277
#define	T_LEFT_CURLY_BRACKET	278
#define	T_LEFT_PARANTHESIS	279
#define	T_LEFT_SQUARE_BRACKET	280
#define	T_LESS_THAN_SIGN	281
#define	T_MINUS_SIGN	282
#define	T_OUT	283
#define	T_PERCENT_SIGN	284
#define	T_PLUS_SIGN	285
#define	T_RAISES	286
#define	T_READONLY	287
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
#define	T_NEW	309
#define	T_DELETE	310
#define	T_WHILE	311
#define	T_IF	312
#define	T_ELSE	313
#define	T_FOR	314
#define	T_DO	315
#define	T_INCR	316
#define	T_DECR	317
#define	T_SPEC_OPEN	318
#define	T_SPEC_CLOSE	319
#define	T_MAIN	320
#define	T_FOREACH	321
#define	T_SUBST	322
#define	T_MATCH	323
#define	T_NOT	324
#define	T_RETURN	325
#define	T_SIGNAL	326
#define	T_EMIT	327
#define	T_IMPORT	328
#define	T_VAR	329
#define	T_UNKNOWN	330
#define	T_CATCH	331
#define	T_TRY	332
#define	T_RAISE	333

#line 1 "yacc.yy"


#include "kscript_parsenode.h"
#include "kscript_parser.h"

#include <qstring.h>

extern int yylex();

extern QString idl_lexFile;
extern int idl_line_no;

void yyerror( char *s )
{
  theParser->parse_error( idl_lexFile, s, idl_line_no );
}


#line 21 "yacc.yy"
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



#define	YYFINAL		382
#define	YYFLAG		-32768
#define	YYNTBASE	79

#define YYTRANSLATE(x) ((unsigned)(x) <= 333 ? yytranslate[x] : 141)

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
     0,     1,     3,     5,     8,    11,    14,    17,    20,    23,
    27,    29,    31,    36,    41,    44,    48,    49,    51,    53,
    56,    59,    62,    65,    68,    71,    73,    76,    78,    82,
    84,    88,    90,    93,    98,   102,   107,   111,   113,   117,
   121,   125,   129,   133,   137,   139,   141,   145,   147,   151,
   153,   157,   159,   163,   167,   169,   173,   177,   179,   183,
   187,   191,   193,   196,   199,   202,   205,   208,   210,   215,
   220,   224,   229,   231,   235,   237,   241,   243,   246,   249,
   252,   255,   257,   260,   263,   265,   267,   269,   273,   275,
   277,   279,   281,   283,   285,   288,   292,   295,   299,   303,
   305,   313,   319,   323,   326,   328,   332,   336,   340,   344,
   345,   350,   356,   358,   361,   364,   368,   371,   373,   377,
   382,   385,   389,   390,   392,   394,   397,   400,   403,   410,
   416,   425,   428,   434,   441,   443,   447,   450,   455,   458,
   461,   463,   467,   470,   475,   476,   478,   480,   483,   486,
   492,   496,   500,   503,   507,   509,   515,   519,   521,   524,
   534,   544,   549,   554,   560,   567,   576,   586,   587,   597,
   602
};

static const short yyrhs[] = {    -1,
    80,     0,    81,     0,    81,    80,     0,   136,    37,     0,
    83,    37,     0,   117,    37,     0,   121,    37,     0,   126,
    33,     0,    73,    18,    37,     0,    42,     0,    82,     0,
    65,    23,   131,    33,     0,    84,    23,    85,    33,     0,
    22,    18,     0,    22,    18,    88,     0,     0,    86,     0,
    87,     0,    87,    86,     0,   136,    37,     0,   126,    33,
     0,    92,    37,     0,    93,    37,     0,   117,    37,     0,
    42,     0,     9,    89,     0,    90,     0,    90,    10,    89,
     0,    91,     0,    90,    53,    18,     0,    18,     0,    36,
    18,     0,    90,    18,   113,   116,     0,     5,    90,    18,
     0,    32,     5,    90,    18,     0,    95,    51,    94,     0,
    95,     0,    96,    14,    95,     0,    96,    52,    95,     0,
    96,    49,    95,     0,    96,    50,    95,     0,    96,    26,
    95,     0,    96,    17,    95,     0,    96,     0,    97,     0,
    96,    47,    97,     0,    98,     0,    97,     8,    98,     0,
    99,     0,    98,     3,    99,     0,   100,     0,    99,    39,
   100,     0,    99,    38,   100,     0,   101,     0,   100,    30,
   101,     0,   100,    27,   101,     0,   102,     0,   101,     4,
   102,     0,   101,    40,   102,     0,   101,    29,   102,     0,
   103,     0,    54,   103,     0,    27,   104,     0,    30,   104,
     0,    45,   104,     0,    69,   104,     0,   104,     0,   104,
    25,    94,    35,     0,   104,    23,    94,    33,     0,   104,
    24,    34,     0,   104,    24,   105,    34,     0,   106,     0,
    94,    10,   105,     0,    94,     0,   104,    53,    18,     0,
   107,     0,    61,   108,     0,    62,   108,     0,   108,    61,
     0,   108,    62,     0,   108,     0,   109,    68,     0,   109,
    67,     0,   109,     0,    91,     0,   110,     0,    24,    94,
    34,     0,    21,     0,    41,     0,     7,     0,    16,     0,
    46,     0,    15,     0,    25,    35,     0,    25,   111,    35,
     0,    23,    33,     0,    23,   112,    33,     0,    94,    10,
   111,     0,    94,     0,    24,    94,    10,    95,    34,    10,
   112,     0,    24,    94,    10,    95,    34,     0,    24,   114,
    34,     0,    24,    34,     0,   115,     0,   115,    10,   114,
     0,    19,    90,    18,     0,    28,    90,    18,     0,    20,
    90,    18,     0,     0,    31,    24,    89,    34,     0,    43,
    18,    23,   118,    33,     0,   119,     0,   119,   118,     0,
   126,    33,     0,    74,   120,    37,     0,   136,    37,     0,
    18,     0,    18,    10,   120,     0,   122,    23,   123,    33,
     0,    48,    18,     0,    48,    18,    88,     0,     0,   124,
     0,   125,     0,   125,   124,     0,   126,    33,     0,   117,
    37,     0,    71,    18,    24,   129,    34,    37,     0,    71,
    18,    24,    34,    37,     0,    55,    24,    19,    18,    34,
    23,   131,    33,     0,   136,    37,     0,    18,    24,    34,
    23,   131,     0,    18,    24,   127,    34,    23,   131,     0,
   128,     0,   128,    10,   127,     0,    19,    18,     0,    19,
    18,    51,    94,     0,    28,    18,     0,    20,    18,     0,
   130,     0,   130,    10,   127,     0,    19,    18,     0,    19,
    18,    51,    94,     0,     0,   132,     0,   133,     0,   133,
   132,     0,    94,    37,     0,    78,    94,    10,    94,    37,
     0,    55,    94,    37,     0,    72,    94,    37,     0,    70,
    37,     0,    70,    94,    37,     0,   137,     0,    77,    23,
   132,    33,   134,     0,    23,   132,    33,     0,   135,     0,
   135,   134,     0,    76,    24,    94,    10,    18,    34,    23,
   132,    33,     0,    12,    24,    18,    10,    18,    34,    23,
   132,    33,     0,    11,    18,    51,    95,     0,   139,    23,
   131,    33,     0,    59,   140,    23,   131,    33,     0,    60,
    23,   131,    33,   139,    37,     0,    57,    24,    94,    34,
    23,   131,    33,   138,     0,    66,    24,    94,    10,    94,
    34,    23,   131,    33,     0,     0,    58,    57,    24,    94,
    34,    23,   131,    33,   138,     0,    56,    24,    94,    34,
     0,    24,    94,    37,    94,    37,    94,    34,     0
};

#endif

#if YYDEBUG != 0
static const short yyrline[] = { 0,
   175,   179,   186,   190,   199,   203,   207,   211,   215,   219,
   224,   229,   236,   245,   254,   259,   269,   273,   281,   285,
   294,   298,   302,   306,   310,   314,   323,   331,   335,   344,
   348,   356,   361,   373,   382,   387,   396,   400,   407,   411,
   415,   419,   423,   427,   431,   439,   443,   451,   455,   463,
   467,   475,   479,   483,   491,   495,   499,   507,   511,   515,
   519,   526,   530,   539,   543,   547,   551,   555,   562,   566,
   570,   574,   578,   585,   590,   597,   602,   609,   613,   617,
   621,   625,   632,   637,   642,   650,   654,   658,   667,   672,
   677,   682,   687,   692,   697,   701,   705,   709,   716,   720,
   727,   731,   740,   744,   752,   756,   765,   770,   775,   784,
   788,   797,   805,   809,   817,   821,   825,   832,   837,   846,
   854,   859,   869,   873,   880,   884,   892,   896,   900,   905,
   910,   919,   926,   932,   940,   944,   951,   956,   961,   966,
   974,   978,   985,   990,   998,  1002,  1009,  1013,  1021,  1025,
  1029,  1033,  1037,  1041,  1045,  1049,  1053,  1060,  1064,  1072,
  1078,  1089,  1097,  1101,  1105,  1109,  1113,  1120,  1124,  1131,
  1138
};
#endif


#if YYDEBUG != 0 || defined (YYERROR_VERBOSE)

static const char * const yytname[] = {   "$","error","$undefined.","T_AMPERSAND",
"T_ASTERISK","T_ATTRIBUTE","T_CASE","T_CHARACTER_LITERAL","T_CIRCUMFLEX","T_COLON",
"T_COMMA","T_CONST","T_DEFAULT","T_ENUM","T_EQUAL","T_FALSE","T_FLOATING_PT_LITERAL",
"T_GREATER_THAN_SIGN","T_IDENTIFIER","T_IN","T_INOUT","T_INTEGER_LITERAL","T_INTERFACE",
"T_LEFT_CURLY_BRACKET","T_LEFT_PARANTHESIS","T_LEFT_SQUARE_BRACKET","T_LESS_THAN_SIGN",
"T_MINUS_SIGN","T_OUT","T_PERCENT_SIGN","T_PLUS_SIGN","T_RAISES","T_READONLY",
"T_RIGHT_CURLY_BRACKET","T_RIGHT_PARANTHESIS","T_RIGHT_SQUARE_BRACKET","T_SCOPE",
"T_SEMICOLON","T_SHIFTLEFT","T_SHIFTRIGHT","T_SOLIDUS","T_STRING_LITERAL","T_PRAGMA",
"T_STRUCT","T_SWITCH","T_TILDE","T_TRUE","T_VERTICAL_LINE","T_CLASS","T_LESS_OR_EQUAL",
"T_GREATER_OR_EQUAL","T_ASSIGN","T_NOTEQUAL","T_MEMBER","T_NEW","T_DELETE","T_WHILE",
"T_IF","T_ELSE","T_FOR","T_DO","T_INCR","T_DECR","T_SPEC_OPEN","T_SPEC_CLOSE",
"T_MAIN","T_FOREACH","T_SUBST","T_MATCH","T_NOT","T_RETURN","T_SIGNAL","T_EMIT",
"T_IMPORT","T_VAR","T_UNKNOWN","T_CATCH","T_TRY","T_RAISE","specification","definitions",
"definition","main","interface_dcl","interface_header","interface_body","interface_exports",
"interface_export","inheritance_spec","qualified_names","qualified_name","scoped_name",
"corba_func_dcl","attr_dcl","assign_expr","equal_expr","or_expr","xor_expr",
"and_expr","shift_expr","add_expr","mult_expr","new_expr","unary_expr","index_expr",
"func_call_params","member_expr","incr_expr","match_expr","primary_expr","literal",
"array_elements","dict_elements","parameter_dcls","param_dcls","param_dcl","raises_expr",
"struct_dcl","struct_exports","struct_export","struct_members","class_dcl","class_header",
"class_body","class_exports","class_export","func_dcl","func_params","func_param",
"signal_params","signal_param","func_body","func_lines","func_line","catches",
"single_catch","const_dcl","loops","else","while","for", NULL
};
#endif

static const short yyr1[] = {     0,
    79,    79,    80,    80,    81,    81,    81,    81,    81,    81,
    81,    81,    82,    83,    84,    84,    85,    85,    86,    86,
    87,    87,    87,    87,    87,    87,    88,    89,    89,    90,
    90,    91,    91,    92,    93,    93,    94,    94,    95,    95,
    95,    95,    95,    95,    95,    96,    96,    97,    97,    98,
    98,    99,    99,    99,   100,   100,   100,   101,   101,   101,
   101,   102,   102,   103,   103,   103,   103,   103,   104,   104,
   104,   104,   104,   105,   105,   106,   106,   107,   107,   107,
   107,   107,   108,   108,   108,   109,   109,   109,   110,   110,
   110,   110,   110,   110,   110,   110,   110,   110,   111,   111,
   112,   112,   113,   113,   114,   114,   115,   115,   115,   116,
   116,   117,   118,   118,   119,   119,   119,   120,   120,   121,
   122,   122,   123,   123,   124,   124,   125,   125,   125,   125,
   125,   125,   126,   126,   127,   127,   128,   128,   128,   128,
   129,   129,   130,   130,   131,   131,   132,   132,   133,   133,
   133,   133,   133,   133,   133,   133,   133,   134,   134,   135,
   135,   136,   137,   137,   137,   137,   137,   138,   138,   139,
   140
};

static const short yyr2[] = {     0,
     0,     1,     1,     2,     2,     2,     2,     2,     2,     3,
     1,     1,     4,     4,     2,     3,     0,     1,     1,     2,
     2,     2,     2,     2,     2,     1,     2,     1,     3,     1,
     3,     1,     2,     4,     3,     4,     3,     1,     3,     3,
     3,     3,     3,     3,     1,     1,     3,     1,     3,     1,
     3,     1,     3,     3,     1,     3,     3,     1,     3,     3,
     3,     1,     2,     2,     2,     2,     2,     1,     4,     4,
     3,     4,     1,     3,     1,     3,     1,     2,     2,     2,
     2,     1,     2,     2,     1,     1,     1,     3,     1,     1,
     1,     1,     1,     1,     2,     3,     2,     3,     3,     1,
     7,     5,     3,     2,     1,     3,     3,     3,     3,     0,
     4,     5,     1,     2,     2,     3,     2,     1,     3,     4,
     2,     3,     0,     1,     1,     2,     2,     2,     6,     5,
     8,     2,     5,     6,     1,     3,     2,     4,     2,     2,
     1,     3,     2,     4,     0,     1,     1,     2,     2,     5,
     3,     3,     2,     3,     1,     5,     3,     1,     2,     9,
     9,     4,     4,     5,     6,     8,     9,     0,     9,     4,
     7
};

static const short yydefact[] = {     1,
     0,     0,     0,    11,     0,     0,     0,     0,     2,     3,
    12,     0,     0,     0,     0,     0,     0,     0,     0,     0,
    15,     0,   121,   145,     0,     4,     6,    17,     7,     8,
   123,     9,     5,     0,     0,     0,     0,     0,     0,   135,
     0,    16,     0,   122,    91,    94,    92,    32,    89,     0,
     0,     0,     0,     0,     0,    90,     0,    93,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,    86,     0,    38,    45,    46,    48,    50,    52,
    55,    58,    62,    68,    73,    77,    82,    85,    87,     0,
   146,   147,   155,     0,    10,     0,    32,     0,    26,     0,
    18,    19,     0,    30,     0,     0,     0,     0,     0,     0,
     0,     0,     0,   124,   125,     0,     0,     0,   162,   137,
   140,   139,   145,     0,     0,    27,    28,     0,     0,   113,
     0,     0,     0,    97,     0,     0,     0,    95,   100,     0,
    64,    65,    33,    66,    63,     0,     0,     0,     0,     0,
   145,    78,    79,     0,    67,   153,     0,     0,     0,     0,
   149,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,    80,    81,    84,    83,    13,   148,   145,     0,
     0,    14,    20,     0,     0,    23,    24,    25,    22,    21,
     0,     0,   128,   120,   126,   127,   132,     0,     0,   133,
   145,   136,     0,   118,     0,   112,   114,   115,   117,     0,
    98,   157,    88,     0,    96,   151,     0,     0,     0,   145,
     0,     0,   154,   152,     0,     0,    37,    39,    44,    43,
    47,    41,    42,    40,    49,    51,    54,    53,    57,    56,
    59,    61,    60,     0,    71,    75,     0,     0,    76,     0,
    35,     0,     0,   110,    31,     0,     0,     0,   138,   134,
    29,     0,   116,     0,    99,   170,     0,     0,     0,     0,
     0,     0,     0,    70,     0,    72,    69,   163,    36,     0,
     0,     0,   104,     0,   105,     0,    34,     0,     0,     0,
     0,   141,   119,     0,   145,     0,   164,     0,     0,     0,
     0,   156,   158,   150,    74,     0,     0,     0,   103,     0,
     0,     0,   143,   130,     0,     0,   102,     0,     0,   165,
     0,     0,     0,   159,   107,   109,   108,   106,     0,   145,
     0,   129,   142,     0,   168,     0,   145,     0,     0,   111,
     0,   144,   101,     0,   166,   171,     0,     0,     0,   131,
     0,   167,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,   145,   161,   160,     0,   168,   169,     0,
     0,     0
};

static const short yydefgoto[] = {   380,
     9,    10,    11,    12,    13,   100,   101,   102,    42,   126,
   127,    73,   105,   106,    74,    75,    76,    77,    78,    79,
    80,    81,    82,    83,    84,   257,    85,    86,    87,    88,
    89,   140,   135,   264,   294,   295,   297,    14,   129,   130,
   215,    15,    16,   113,   114,   115,    17,    39,    40,   301,
   302,    90,    91,    92,   312,   313,    18,    93,   355,    94,
   150
};

static const short yypact[] = {    15,
    -1,     7,    27,-32768,    33,    41,    42,    49,-32768,    15,
-32768,    44,    78,    71,   108,   133,   125,   129,   118,   127,
   164,   153,   164,   330,   143,-32768,-32768,   101,-32768,-32768,
    86,-32768,-32768,   437,   167,   173,   184,   182,   176,   201,
    18,-32768,    11,-32768,-32768,-32768,-32768,-32768,-32768,   272,
   437,    25,   505,   505,   205,-32768,   505,-32768,   486,   437,
   195,   203,   204,   202,   197,   197,   206,   505,   147,   437,
   211,   437,-32768,   198,   186,   200,   231,   239,     0,    88,
    24,-32768,-32768,    97,-32768,-32768,    62,    81,-32768,   213,
-32768,   330,-32768,   225,-32768,    18,     7,   246,-32768,   220,
-32768,   101,     9,-32768,   217,   219,   222,   224,   223,   237,
   244,   232,   238,-32768,    86,   240,   233,     1,-32768,   221,
-32768,-32768,   330,   251,   175,-32768,     3,   257,   243,    11,
   245,   248,   437,-32768,   249,   250,   247,-32768,   267,   254,
    97,    97,-32768,    97,-32768,   261,   437,   437,   437,   263,
   330,-32768,-32768,   437,    97,-32768,   264,   266,   330,   281,
-32768,   437,   437,   437,   437,   437,   437,   437,   437,   437,
   437,   437,   437,   437,   437,   437,   437,   437,   437,   388,
   437,   282,-32768,-32768,-32768,-32768,-32768,-32768,   330,    29,
    18,-32768,-32768,   280,   288,-32768,-32768,-32768,-32768,-32768,
   290,   283,-32768,-32768,-32768,-32768,-32768,   437,   437,-32768,
   330,-32768,    18,   300,   274,-32768,-32768,-32768,-32768,    34,
-32768,-32768,-32768,   437,-32768,-32768,   278,   287,   277,   330,
   289,   305,-32768,-32768,   291,   437,-32768,-32768,-32768,-32768,
   231,-32768,-32768,-32768,   239,     0,    88,    88,    24,    24,
-32768,-32768,-32768,   292,-32768,   313,   296,   301,-32768,   302,
-32768,    73,   162,   308,-32768,   322,    91,   333,-32768,-32768,
-32768,   257,-32768,   437,-32768,-32768,   324,   437,   319,   303,
   437,    -2,   321,-32768,   437,-32768,-32768,-32768,-32768,    18,
    18,    18,-32768,   327,   346,   338,-32768,   329,   347,   331,
   335,   354,-32768,   336,   330,   337,-32768,   340,   339,   343,
   348,-32768,    -2,-32768,-32768,    74,    75,    89,-32768,   178,
    18,   355,   328,-32768,   344,   175,   370,   349,   437,-32768,
   360,   375,   437,-32768,-32768,-32768,-32768,-32768,   363,   330,
   437,-32768,-32768,   364,   352,   367,   330,   384,   395,-32768,
   365,-32768,-32768,   357,-32768,-32768,   383,   399,   401,-32768,
   396,-32768,   387,   389,   437,   402,   403,   393,   330,   330,
   405,   397,   398,   330,-32768,-32768,   404,   352,-32768,   432,
   435,-32768
};

static const short yypgoto[] = {-32768,
   426,-32768,-32768,-32768,-32768,-32768,   341,-32768,   415,  -205,
   -24,   -27,-32768,-32768,   -49,   -29,-32768,   273,   270,   275,
   -96,     4,   -25,   382,   187,   160,-32768,-32768,   134,-32768,
-32768,   227,   103,-32768,   128,-32768,-32768,   -12,   326,-32768,
   191,-32768,-32768,-32768,   350,-32768,   -19,  -119,-32768,-32768,
-32768,  -116,   -50,-32768,   141,-32768,   -13,-32768,    90,   179,
-32768
};


#define	YYLAST		567


static const short yytable[] = {   136,
   104,   137,   139,   103,   119,   212,   210,   271,   108,   310,
   146,   116,   213,   104,   109,   107,    19,   117,   112,   157,
   158,     1,   160,   131,   208,     1,   194,   176,     2,   132,
    20,    45,     2,   134,   231,    48,     3,   172,   173,    46,
    47,   188,    48,   274,    21,    49,   261,   118,    51,    52,
    22,    53,   177,    55,    54,   195,     4,     5,    23,   138,
    55,   195,     6,   178,    24,    56,    25,   223,   104,    57,
    58,   190,   260,   311,   104,   247,   248,   103,    59,     7,
    27,   195,   108,   220,   128,    65,    66,     8,   109,   107,
   289,   335,   336,    68,   270,   116,     1,   227,   228,   229,
    28,   117,   112,     2,   232,    96,   337,    29,   235,   299,
   131,     1,   237,   279,   174,   339,   132,   175,    97,   179,
   180,   181,   183,   184,   300,   195,   195,   195,     5,   254,
   256,   258,    98,   238,   239,   240,    55,   242,   243,   244,
   110,   195,    99,     5,    30,    35,    36,   185,   186,   182,
   251,   252,   253,    45,    37,    31,   111,    32,   268,   269,
    38,    46,    47,   104,    48,    33,   262,    49,    34,   118,
    51,    52,    41,    53,   139,    43,    54,   249,   250,    95,
   290,   291,    55,   156,   120,   104,   283,    56,   328,   292,
   121,    57,    58,    35,    36,   293,   290,   291,   152,   153,
    59,   122,    37,    45,   123,   292,   343,    65,    66,   124,
   125,    46,    47,   163,    48,    68,   164,    49,   147,   118,
    51,    52,   143,   351,   151,   165,   148,   149,   306,   154,
   357,   309,    55,   159,   161,   256,   162,    56,   170,   141,
   142,   171,    58,   144,   304,   187,   166,   189,   167,   168,
   191,   169,   192,   196,   155,   197,   199,   377,   198,   200,
   201,   202,   104,   104,   104,   316,   317,   318,   203,   207,
   204,   209,   206,   211,   214,   216,   224,   218,    45,   346,
   223,   221,   222,   349,   219,   230,    46,    47,   225,    48,
   236,   352,    49,   104,    50,   133,    52,   226,    53,   259,
   233,    54,   234,   263,   134,   265,   267,    55,   266,   272,
   273,   276,    56,   278,   281,   368,    57,    58,   372,   373,
   277,   280,   285,   282,   284,    59,    60,    61,    62,   286,
    63,    64,    65,    66,   288,   287,    45,    67,   296,   298,
    68,    69,   274,    70,    46,    47,   305,    48,    71,    72,
    49,   307,    50,    51,    52,   320,    53,   314,    61,    54,
   319,   321,   322,   326,   323,    55,   332,   324,   325,   327,
    56,   333,   331,   329,    57,    58,   330,   340,   341,   344,
   342,   345,   347,    59,    60,    61,    62,   208,    63,    64,
    65,    66,   348,   358,    45,    67,   350,   360,    68,    69,
   356,    70,    46,    47,   359,    48,    71,    72,    49,   354,
   118,    51,    52,   361,    53,   362,   363,    54,   364,   365,
   366,   255,   367,    55,   369,   370,   371,   374,    56,   375,
   376,   381,    57,    58,   382,    26,   378,    44,   241,   245,
   145,    59,   193,    45,   315,   246,   353,   338,    65,    66,
   275,    46,    47,   334,    48,   217,    68,    49,   308,   118,
    51,    52,   303,    53,   205,     0,    54,   379,     0,     0,
     0,     0,    55,     0,     0,     0,     0,    56,     0,     0,
     0,    57,    58,     0,     0,     0,     0,     0,     0,     0,
    59,     0,    45,     0,     0,     0,     0,    65,    66,     0,
    46,    47,     0,    48,     0,    68,    49,     0,   118,    51,
    52,    45,    53,     0,     0,    54,     0,     0,     0,    46,
    47,    55,    48,     0,     0,    49,    56,   118,    51,    52,
    57,    58,     0,     0,     0,     0,     0,     0,     0,     0,
    55,     0,     0,     0,     0,    56,    65,    66,     0,     0,
    58,     0,     0,     0,    68,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,    65,    66
};

static const short yycheck[] = {    50,
    28,    51,    52,    28,    34,   125,   123,   213,    28,    12,
    60,    31,    10,    41,    28,    28,    18,    31,    31,    69,
    70,    11,    72,    43,    24,    11,    18,     4,    18,    43,
    24,     7,    18,    33,   151,    18,    22,    38,    39,    15,
    16,    92,    18,    10,    18,    21,    18,    23,    24,    25,
    18,    27,    29,    36,    30,    53,    42,    43,    18,    35,
    36,    53,    48,    40,    23,    41,    18,    34,    96,    45,
    46,    96,   189,    76,   102,   172,   173,   102,    54,    65,
    37,    53,   102,   133,    74,    61,    62,    73,   102,   102,
    18,    18,    18,    69,   211,   115,    11,   147,   148,   149,
    23,   115,   115,    18,   154,     5,    18,    37,   159,    19,
   130,    11,   162,   230,    27,   321,   130,    30,    18,    23,
    24,    25,    61,    62,    34,    53,    53,    53,    43,   179,
   180,   181,    32,   163,   164,   165,    36,   167,   168,   169,
    55,    53,    42,    43,    37,    19,    20,    67,    68,    53,
   176,   177,   178,     7,    28,    23,    71,    33,   208,   209,
    34,    15,    16,   191,    18,    37,   191,    21,    51,    23,
    24,    25,     9,    27,   224,    23,    30,   174,   175,    37,
    19,    20,    36,    37,    18,   213,   236,    41,   305,    28,
    18,    45,    46,    19,    20,    34,    19,    20,    65,    66,
    54,    18,    28,     7,    23,    28,   326,    61,    62,    34,
    10,    15,    16,    14,    18,    69,    17,    21,    24,    23,
    24,    25,    18,   340,    23,    26,    24,    24,   278,    24,
   347,   281,    36,    23,    37,   285,    51,    41,     8,    53,
    54,     3,    46,    57,   274,    33,    47,    23,    49,    50,
     5,    52,    33,    37,    68,    37,    33,   374,    37,    37,
    24,    18,   290,   291,   292,   290,   291,   292,    37,    37,
    33,    51,    33,    23,    18,    33,    10,    33,     7,   329,
    34,    33,    33,   333,    37,    23,    15,    16,    35,    18,
    10,   341,    21,   321,    23,    24,    25,    37,    27,    18,
    37,    30,    37,    24,    33,    18,    24,    36,    19,    10,
    37,    34,    41,    37,    10,   365,    45,    46,   369,   370,
    34,    33,    10,    33,    33,    54,    55,    56,    57,    34,
    59,    60,    61,    62,    33,    35,     7,    66,    31,    18,
    69,    70,    10,    72,    15,    16,    23,    18,    77,    78,
    21,    33,    23,    24,    25,    10,    27,    37,    56,    30,
    34,    24,    34,    10,    18,    36,    24,    37,    34,    34,
    41,    24,    34,    37,    45,    46,    37,    23,    51,    10,
    37,    33,    23,    54,    55,    56,    57,    24,    59,    60,
    61,    62,    18,    10,     7,    66,    34,    33,    69,    70,
    34,    72,    15,    16,    10,    18,    77,    78,    21,    58,
    23,    24,    25,    57,    27,    33,    18,    30,    18,    24,
    34,    34,    34,    36,    23,    23,    34,    23,    41,    33,
    33,     0,    45,    46,     0,    10,    33,    23,   166,   170,
    59,    54,   102,     7,   285,   171,   344,   320,    61,    62,
   224,    15,    16,   313,    18,   130,    69,    21,   280,    23,
    24,    25,   272,    27,   115,    -1,    30,   378,    -1,    -1,
    -1,    -1,    36,    -1,    -1,    -1,    -1,    41,    -1,    -1,
    -1,    45,    46,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    54,    -1,     7,    -1,    -1,    -1,    -1,    61,    62,    -1,
    15,    16,    -1,    18,    -1,    69,    21,    -1,    23,    24,
    25,     7,    27,    -1,    -1,    30,    -1,    -1,    -1,    15,
    16,    36,    18,    -1,    -1,    21,    41,    23,    24,    25,
    45,    46,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    36,    -1,    -1,    -1,    -1,    41,    61,    62,    -1,    -1,
    46,    -1,    -1,    -1,    69,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    61,    62
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
#line 176 "yacc.yy"
{
	    theParser->setRootNode( NULL );
	  ;
    break;}
case 2:
#line 180 "yacc.yy"
{
	    theParser->setRootNode( new KSParseNode( definitions, yyvsp[0].node ) );
	  ;
    break;}
case 3:
#line 187 "yacc.yy"
{
	    yyval.node = new KSParseNode( definitions, yyvsp[0].node );
	  ;
    break;}
case 4:
#line 191 "yacc.yy"
{
	    yyval.node = new KSParseNode( definitions, yyvsp[-1].node );
	    yyval.node->setBranch( 2, yyvsp[0].node );
	  ;
    break;}
case 5:
#line 200 "yacc.yy"
{
	    yyval.node = yyvsp[-1].node;
	  ;
    break;}
case 6:
#line 204 "yacc.yy"
{
	    yyval.node = yyvsp[-1].node;
	  ;
    break;}
case 7:
#line 208 "yacc.yy"
{
	    yyval.node = yyvsp[-1].node;
	  ;
    break;}
case 8:
#line 212 "yacc.yy"
{
	    yyval.node = yyvsp[-1].node;
	  ;
    break;}
case 9:
#line 216 "yacc.yy"
{
	    yyval.node = yyvsp[-1].node;
	  ;
    break;}
case 10:
#line 220 "yacc.yy"
{
	    yyval.node = new KSParseNode( import );
	    yyval.node->setIdent( yyvsp[-1].ident );
	  ;
    break;}
case 11:
#line 225 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_pragma );
	    yyval.node->setIdent( yyvsp[0]._str );
	  ;
    break;}
case 12:
#line 230 "yacc.yy"
{
	    yyval.node = yyvsp[0].node;
	  ;
    break;}
case 13:
#line 237 "yacc.yy"
{
	    yyval.node = new KSParseNode( func_dcl );
	    yyval.node->setBranch( 2, yyvsp[-1].node );
	    yyval.node->setIdent( "main" );
	  ;
    break;}
case 14:
#line 247 "yacc.yy"
{
	    yyval.node = new KSParseNode( interface_dcl, yyvsp[-3].node, yyvsp[-1].node );
	  ;
    break;}
case 15:
#line 255 "yacc.yy"
{
	    yyval.node = new KSParseNode( interface_header );
	    yyval.node->setIdent( yyvsp[0].ident );
	  ;
    break;}
case 16:
#line 260 "yacc.yy"
{
	    yyval.node = new KSParseNode( interface_header );
	    yyval.node->setIdent( yyvsp[-1].ident );
	    yyval.node->setBranch( 1, yyvsp[0].node );
	  ;
    break;}
case 17:
#line 270 "yacc.yy"
{
	    yyval.node = NULL;
	  ;
    break;}
case 18:
#line 274 "yacc.yy"
{
	    yyval.node = yyvsp[0].node;
	  ;
    break;}
case 19:
#line 282 "yacc.yy"
{
	    yyval.node = new KSParseNode( exports, yyvsp[0].node );
	  ;
    break;}
case 20:
#line 286 "yacc.yy"
{
	    yyval.node = new KSParseNode( exports, yyvsp[-1].node );
	    yyval.node->setBranch( 2, yyvsp[0].node );
	  ;
    break;}
case 21:
#line 295 "yacc.yy"
{
	    yyval.node = yyvsp[-1].node;
	  ;
    break;}
case 22:
#line 299 "yacc.yy"
{
	    yyval.node = yyvsp[-1].node;
	  ;
    break;}
case 23:
#line 303 "yacc.yy"
{
	    yyval.node = yyvsp[-1].node;
	  ;
    break;}
case 24:
#line 307 "yacc.yy"
{
	    yyval.node = yyvsp[-1].node;
	  ;
    break;}
case 25:
#line 311 "yacc.yy"
{
	    yyval.node = yyvsp[-1].node;
	  ;
    break;}
case 26:
#line 315 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_pragma );
	    yyval.node->setIdent( yyvsp[0]._str );
	  ;
    break;}
case 27:
#line 324 "yacc.yy"
{
	    yyval.node = yyvsp[0].node;
	  ;
    break;}
case 28:
#line 332 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_qualified_names, yyvsp[0].node );
	  ;
    break;}
case 29:
#line 336 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_qualified_names, yyvsp[-2].node );
	    yyval.node->setBranch( 2, yyvsp[0].node );
	  ;
    break;}
case 30:
#line 345 "yacc.yy"
{
	    yyval.node = yyvsp[0].node;
	  ;
    break;}
case 31:
#line 349 "yacc.yy"
{
	    yyval.node = new KSParseNode( member_expr, yyvsp[-2].node );
	    yyval.node->setIdent( yyvsp[0].ident );
	  ;
    break;}
case 32:
#line 357 "yacc.yy"
{
	    yyval.node = new KSParseNode( scoped_name );
	    yyval.node->setIdent( yyvsp[0].ident );
	  ;
    break;}
case 33:
#line 362 "yacc.yy"
{
	    yyval.node = new KSParseNode( scoped_name );
	    QString name = "::";
	    name += *(yyvsp[0].ident);
	    delete yyvsp[0].ident;
	    yyval.node->setIdent( name );
	  ;
    break;}
case 34:
#line 374 "yacc.yy"
{
	    yyval.node = new KSParseNode( corba_func_dcl, yyvsp[-3].node, yyvsp[-1].node, yyvsp[0].node );
	    yyval.node->setIdent( yyvsp[-2].ident );
	  ;
    break;}
case 35:
#line 383 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_attribute, yyvsp[-1].node );
	    yyval.node->setIdent( yyvsp[0].ident );
	  ;
    break;}
case 36:
#line 388 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_readonly_attribute, yyvsp[-1].node );
	    yyval.node->setIdent( yyvsp[0].ident );
	  ;
    break;}
case 37:
#line 397 "yacc.yy"
{
	    yyval.node = new KSParseNode( assign_expr, yyvsp[-2].node, yyvsp[0].node );
	  ;
    break;}
case 38:
#line 401 "yacc.yy"
{
	    yyval.node = yyvsp[0].node;
	  ;
    break;}
case 39:
#line 408 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_equal, yyvsp[-2].node, yyvsp[0].node );
	  ;
    break;}
case 40:
#line 412 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_notequal, yyvsp[-2].node, yyvsp[0].node );
	  ;
    break;}
case 41:
#line 416 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_less_or_equal, yyvsp[-2].node, yyvsp[0].node );
	  ;
    break;}
case 42:
#line 420 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_greater_or_equal, yyvsp[-2].node, yyvsp[0].node );
	  ;
    break;}
case 43:
#line 424 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_less, yyvsp[-2].node, yyvsp[0].node );
	  ;
    break;}
case 44:
#line 428 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_greater, yyvsp[-2].node, yyvsp[0].node );
	  ;
    break;}
case 45:
#line 432 "yacc.yy"
{
	    yyval.node = yyvsp[0].node;
	  ;
    break;}
case 46:
#line 440 "yacc.yy"
{
	    yyval.node = yyvsp[0].node;
	  ;
    break;}
case 47:
#line 444 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_vertical_line, yyvsp[-2].node, yyvsp[0].node );
	  ;
    break;}
case 48:
#line 452 "yacc.yy"
{
	    yyval.node = yyvsp[0].node;
	  ;
    break;}
case 49:
#line 456 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_circumflex, yyvsp[-2].node, yyvsp[0].node );
	  ;
    break;}
case 50:
#line 464 "yacc.yy"
{
	    yyval.node = yyvsp[0].node;
	  ;
    break;}
case 51:
#line 468 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_ampersand, yyvsp[-2].node, yyvsp[0].node );
	  ;
    break;}
case 52:
#line 476 "yacc.yy"
{
	    yyval.node = yyvsp[0].node;
	  ;
    break;}
case 53:
#line 480 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_shiftright, yyvsp[-2].node, yyvsp[0].node );
	  ;
    break;}
case 54:
#line 484 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_shiftleft, yyvsp[-2].node, yyvsp[0].node );
	  ;
    break;}
case 55:
#line 492 "yacc.yy"
{
	    yyval.node = yyvsp[0].node;
	  ;
    break;}
case 56:
#line 496 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_plus_sign, yyvsp[-2].node, yyvsp[0].node );
	  ;
    break;}
case 57:
#line 500 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_minus_sign, yyvsp[-2].node, yyvsp[0].node );
	  ;
    break;}
case 58:
#line 508 "yacc.yy"
{
	    yyval.node = yyvsp[0].node;
	  ;
    break;}
case 59:
#line 512 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_asterik, yyvsp[-2].node, yyvsp[0].node );
	  ;
    break;}
case 60:
#line 516 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_solidus, yyvsp[-2].node, yyvsp[0].node );
	  ;
    break;}
case 61:
#line 520 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_percent_sign, yyvsp[-2].node, yyvsp[0].node );
	  ;
    break;}
case 62:
#line 527 "yacc.yy"
{
	    yyval.node = yyvsp[0].node;
	  ;
    break;}
case 63:
#line 531 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_new, yyvsp[0].node );
	  ;
    break;}
case 64:
#line 540 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_minus_sign, yyvsp[0].node );
	  ;
    break;}
case 65:
#line 544 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_plus_sign, yyvsp[0].node );
	  ;
    break;}
case 66:
#line 548 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_tilde, yyvsp[0].node );
	  ;
    break;}
case 67:
#line 552 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_not, yyvsp[0].node );
	  ;
    break;}
case 68:
#line 556 "yacc.yy"
{
	    yyval.node = yyvsp[0].node;
	  ;
    break;}
case 69:
#line 563 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_array, yyvsp[-3].node, yyvsp[-1].node );
	  ;
    break;}
case 70:
#line 567 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_dict, yyvsp[-3].node, yyvsp[-1].node );
	  ;
    break;}
case 71:
#line 571 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_func_call, yyvsp[-2].node );
	  ;
    break;}
case 72:
#line 575 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_func_call, yyvsp[-3].node, yyvsp[-1].node );
	  ;
    break;}
case 73:
#line 579 "yacc.yy"
{
	    yyval.node = yyvsp[0].node;
	  ;
    break;}
case 74:
#line 586 "yacc.yy"
{
	    yyval.node = new KSParseNode( func_call_params, yyvsp[-2].node, yyvsp[0].node );
	  ;
    break;}
case 75:
#line 591 "yacc.yy"
{
	    yyval.node = new KSParseNode( func_call_params, yyvsp[0].node );
	  ;
    break;}
case 76:
#line 598 "yacc.yy"
{
	    yyval.node = new KSParseNode( member_expr, yyvsp[-2].node );
	    yyval.node->setIdent( yyvsp[0].ident );
	  ;
    break;}
case 77:
#line 603 "yacc.yy"
{
	    yyval.node = yyvsp[0].node;
	  ;
    break;}
case 78:
#line 610 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_incr, yyvsp[0].node );
	  ;
    break;}
case 79:
#line 614 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_decr, yyvsp[0].node );
	  ;
    break;}
case 80:
#line 618 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_incr, yyvsp[-1].node );
	  ;
    break;}
case 81:
#line 622 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_decr, yyvsp[-1].node );
	  ;
    break;}
case 82:
#line 626 "yacc.yy"
{
	    yyval.node = yyvsp[0].node;
	  ;
    break;}
case 83:
#line 633 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_match, yyvsp[-1].node );
	    yyval.node->setIdent( yyvsp[0].ident );
	  ;
    break;}
case 84:
#line 638 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_subst, yyvsp[-1].node );
	    yyval.node->setIdent( yyvsp[0].ident );
	  ;
    break;}
case 85:
#line 643 "yacc.yy"
{
	    yyval.node = yyvsp[0].node;
	  ;
    break;}
case 86:
#line 651 "yacc.yy"
{
	    yyval.node = yyvsp[0].node;
	  ;
    break;}
case 87:
#line 655 "yacc.yy"
{
	    yyval.node = yyvsp[0].node;
	  ;
    break;}
case 88:
#line 659 "yacc.yy"
{
	    yyval.node = yyvsp[-1].node;
	  ;
    break;}
case 89:
#line 668 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_integer_literal );
	    yyval.node->setIntegerLiteral( yyvsp[0]._int );
	  ;
    break;}
case 90:
#line 673 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_string_literal );
	    yyval.node->setStringLiteral( yyvsp[0]._str );
	  ;
    break;}
case 91:
#line 678 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_character_literal );
	    yyval.node->setCharacterLiteral( yyvsp[0]._char );
	  ;
    break;}
case 92:
#line 683 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_floating_pt_literal );
	    yyval.node->setFloatingPtLiteral( yyvsp[0]._float );
	  ;
    break;}
case 93:
#line 688 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_boolean_literal );
	    yyval.node->setBooleanLiteral( true );
	  ;
    break;}
case 94:
#line 693 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_boolean_literal );
	    yyval.node->setBooleanLiteral( false );
	  ;
    break;}
case 95:
#line 698 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_array_const );
	  ;
    break;}
case 96:
#line 702 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_array_const, yyvsp[-1].node );
	  ;
    break;}
case 97:
#line 706 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_dict_const );
	  ;
    break;}
case 98:
#line 710 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_dict_const, yyvsp[-1].node );
	  ;
    break;}
case 99:
#line 717 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_array_element, yyvsp[-2].node, yyvsp[0].node );
	  ;
    break;}
case 100:
#line 721 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_array_element, yyvsp[0].node  );
	  ;
    break;}
case 101:
#line 728 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_dict_element, yyvsp[-5].node, yyvsp[-3].node, yyvsp[0].node );
	  ;
    break;}
case 102:
#line 732 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_dict_element, yyvsp[-3].node, yyvsp[-1].node );
	  ;
    break;}
case 103:
#line 741 "yacc.yy"
{
	    yyval.node = yyvsp[-1].node;
	  ;
    break;}
case 104:
#line 745 "yacc.yy"
{
	    yyval.node = NULL;
	  ;
    break;}
case 105:
#line 753 "yacc.yy"
{
	    yyval.node = new KSParseNode( param_dcls, yyvsp[0].node );
	  ;
    break;}
case 106:
#line 757 "yacc.yy"
{
	    yyval.node = new KSParseNode( param_dcls, yyvsp[-2].node );
	    yyval.node->setBranch( 2, yyvsp[0].node );
	  ;
    break;}
case 107:
#line 766 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_in_param_dcl, yyvsp[-1].node );
	    yyval.node->setIdent( yyvsp[0].ident );
	  ;
    break;}
case 108:
#line 771 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_out_param_dcl, yyvsp[-1].node );
	    yyval.node->setIdent( yyvsp[0].ident );
	  ;
    break;}
case 109:
#line 776 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_inout_param_dcl, yyvsp[-1].node );
	    yyval.node->setIdent( yyvsp[0].ident );
	  ;
    break;}
case 110:
#line 785 "yacc.yy"
{
	    yyval.node = NULL;
	  ;
    break;}
case 111:
#line 789 "yacc.yy"
{
	    yyval.node = new KSParseNode( raises_expr, yyvsp[-1].node );
	  ;
    break;}
case 112:
#line 798 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_struct, yyvsp[-1].node );
	    yyval.node->setIdent( yyvsp[-3].ident );
	  ;
    break;}
case 113:
#line 806 "yacc.yy"
{
	    yyval.node = new KSParseNode( exports, yyvsp[0].node );
	  ;
    break;}
case 114:
#line 810 "yacc.yy"
{
	    yyval.node = new KSParseNode( exports, yyvsp[-1].node );
	    yyval.node->setBranch( 2, yyvsp[0].node );
	  ;
    break;}
case 115:
#line 818 "yacc.yy"
{
	    yyval.node = yyvsp[-1].node;
	  ;
    break;}
case 116:
#line 822 "yacc.yy"
{
	    yyval.node = yyvsp[-1].node;
	  ;
    break;}
case 117:
#line 826 "yacc.yy"
{
	    yyval.node = yyvsp[-1].node;
	  ;
    break;}
case 118:
#line 833 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_struct_members );
	    yyval.node->setIdent( yyvsp[0].ident );
	  ;
    break;}
case 119:
#line 838 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_struct_members, yyvsp[0].node );
	    yyval.node->setIdent( yyvsp[-2].ident );
	  ;
    break;}
case 120:
#line 847 "yacc.yy"
{
	    yyval.node = new KSParseNode( class_dcl, yyvsp[-3].node, yyvsp[-1].node );
	  ;
    break;}
case 121:
#line 855 "yacc.yy"
{
	    yyval.node = new KSParseNode( class_header );
	    yyval.node->setIdent( yyvsp[0].ident );
	  ;
    break;}
case 122:
#line 860 "yacc.yy"
{
	    yyval.node = new KSParseNode( class_header );
	    yyval.node->setIdent( yyvsp[-1].ident );
	    yyval.node->setBranch( 1, yyvsp[0].node );
	  ;
    break;}
case 123:
#line 870 "yacc.yy"
{
	    yyval.node = NULL;
	  ;
    break;}
case 124:
#line 874 "yacc.yy"
{
	    yyval.node = yyvsp[0].node;
	  ;
    break;}
case 125:
#line 881 "yacc.yy"
{
	    yyval.node = new KSParseNode( exports, yyvsp[0].node );
	  ;
    break;}
case 126:
#line 885 "yacc.yy"
{
	    yyval.node = new KSParseNode( exports, yyvsp[-1].node );
	    yyval.node->setBranch( 2, yyvsp[0].node );
	  ;
    break;}
case 127:
#line 893 "yacc.yy"
{
	    yyval.node = yyvsp[-1].node;
	  ;
    break;}
case 128:
#line 897 "yacc.yy"
{
	    yyval.node = yyvsp[-1].node;
	  ;
    break;}
case 129:
#line 901 "yacc.yy"
{
	    yyval.node = new KSParseNode( signal_dcl, yyvsp[-2].node );
	    yyval.node->setIdent( yyvsp[-4].ident );
	  ;
    break;}
case 130:
#line 906 "yacc.yy"
{
	    yyval.node = new KSParseNode( signal_dcl, 0 );
	    yyval.node->setIdent( yyvsp[-3].ident );
	  ;
    break;}
case 131:
#line 911 "yacc.yy"
{
	    yyval.node = new KSParseNode( destructor_dcl );
	    KSParseNode* n = new KSParseNode( func_param_in );
	    n->setIdent( yyvsp[-4].ident );
	    yyval.node->setBranch( 1, n );
	    yyval.node->setBranch( 2, yyvsp[-1].node );
	    yyval.node->setIdent( "delete" );
	  ;
    break;}
case 132:
#line 920 "yacc.yy"
{
	    yyval.node = yyvsp[-1].node;
	  ;
    break;}
case 133:
#line 927 "yacc.yy"
{
	    yyval.node = new KSParseNode( func_dcl );
	    yyval.node->setBranch( 2, yyvsp[0].node );
	    yyval.node->setIdent( yyvsp[-4].ident );
	  ;
    break;}
case 134:
#line 933 "yacc.yy"
{
	    yyval.node = new KSParseNode( func_dcl, yyvsp[-3].node, yyvsp[0].node );
	    yyval.node->setIdent( yyvsp[-5].ident );
	  ;
    break;}
case 135:
#line 941 "yacc.yy"
{
	    yyval.node = new KSParseNode( func_params, yyvsp[0].node );
	  ;
    break;}
case 136:
#line 945 "yacc.yy"
{
	    yyval.node = new KSParseNode( func_params, yyvsp[-2].node, yyvsp[0].node );
	  ;
    break;}
case 137:
#line 952 "yacc.yy"
{
	    yyval.node = new KSParseNode( func_param_in );
	    yyval.node->setIdent( yyvsp[0].ident );
	  ;
    break;}
case 138:
#line 957 "yacc.yy"
{
	    yyval.node = new KSParseNode( func_param_in, yyvsp[0].node );
	    yyval.node->setIdent( yyvsp[-2].ident );
	  ;
    break;}
case 139:
#line 962 "yacc.yy"
{
	    yyval.node = new KSParseNode( func_param_out );
	    yyval.node->setIdent( yyvsp[0].ident );
	  ;
    break;}
case 140:
#line 967 "yacc.yy"
{
	    yyval.node = new KSParseNode( func_param_inout );
	    yyval.node->setIdent( yyvsp[0].ident );
	  ;
    break;}
case 141:
#line 975 "yacc.yy"
{
	    yyval.node = new KSParseNode( signal_params, yyvsp[0].node );
	  ;
    break;}
case 142:
#line 979 "yacc.yy"
{
	    yyval.node = new KSParseNode( signal_params, yyvsp[-2].node, yyvsp[0].node );
	  ;
    break;}
case 143:
#line 986 "yacc.yy"
{
	    yyval.node = new KSParseNode( signal_param );
	    yyval.node->setIdent( yyvsp[0].ident );
	  ;
    break;}
case 144:
#line 991 "yacc.yy"
{
	    yyval.node = new KSParseNode( signal_param, yyvsp[0].node );
	    yyval.node->setIdent( yyvsp[-2].ident );
	  ;
    break;}
case 145:
#line 999 "yacc.yy"
{
	    yyval.node = NULL;
	  ;
    break;}
case 146:
#line 1003 "yacc.yy"
{
	    yyval.node = yyvsp[0].node;
	  ;
    break;}
case 147:
#line 1010 "yacc.yy"
{
	    yyval.node = new KSParseNode( func_lines, yyvsp[0].node );
	  ;
    break;}
case 148:
#line 1014 "yacc.yy"
{
	    yyval.node = new KSParseNode( func_lines, yyvsp[-1].node );
	    yyval.node->setBranch( 2, yyvsp[0].node );
	  ;
    break;}
case 149:
#line 1022 "yacc.yy"
{
	    yyval.node = yyvsp[-1].node;
	  ;
    break;}
case 150:
#line 1026 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_raise, yyvsp[-3].node, yyvsp[-1].node );
	  ;
    break;}
case 151:
#line 1030 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_delete, yyvsp[-1].node );
	  ;
    break;}
case 152:
#line 1034 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_emit, yyvsp[-1].node );
	  ;
    break;}
case 153:
#line 1038 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_return );
	  ;
    break;}
case 154:
#line 1042 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_return, yyvsp[-1].node );
	  ;
    break;}
case 155:
#line 1046 "yacc.yy"
{
	    yyval.node = yyvsp[0].node;
	  ;
    break;}
case 156:
#line 1050 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_try, yyvsp[-2].node, yyvsp[0].node );
	  ;
    break;}
case 157:
#line 1054 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_scope, yyvsp[-1].node );
	  ;
    break;}
case 158:
#line 1061 "yacc.yy"
{
	    yyval.node = yyvsp[0].node;
	  ;
    break;}
case 159:
#line 1065 "yacc.yy"
{
	    yyval.node = yyvsp[-1].node;
	    yyval.node->setBranch( 4, yyvsp[0].node );
	  ;
    break;}
case 160:
#line 1074 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_catch, yyvsp[-6].node, yyvsp[-1].node );
	    yyval.node->setIdent( yyvsp[-4].ident );
	  ;
    break;}
case 161:
#line 1080 "yacc.yy"
{
	    KSParseNode* x = new KSParseNode( scoped_name );
	    x->setIdent( yyvsp[-6].ident );
	    yyval.node = new KSParseNode( t_catch_default, x, yyvsp[-1].node );
	    yyval.node->setIdent( yyvsp[-4].ident );
	  ;
    break;}
case 162:
#line 1090 "yacc.yy"
{
	    yyval.node = new KSParseNode( const_dcl, yyvsp[0].node );
	    yyval.node->setIdent( yyvsp[-2].ident );
	  ;
    break;}
case 163:
#line 1098 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_while, yyvsp[-3].node, yyvsp[-1].node );
	  ;
    break;}
case 164:
#line 1102 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_for, yyvsp[-3].node, yyvsp[-1].node );
	  ;
    break;}
case 165:
#line 1106 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_do, yyvsp[-3].node, yyvsp[-1].node );
	  ;
    break;}
case 166:
#line 1110 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_if, yyvsp[-5].node, yyvsp[-2].node, yyvsp[0].node );
	  ;
    break;}
case 167:
#line 1114 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_foreach, yyvsp[-6].node, yyvsp[-4].node, yyvsp[-1].node );
	  ;
    break;}
case 168:
#line 1121 "yacc.yy"
{
	    yyval.node = NULL;
	  ;
    break;}
case 169:
#line 1125 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_else, yyvsp[-5].node, yyvsp[-2].node, yyvsp[0].node );
	  ;
    break;}
case 170:
#line 1132 "yacc.yy"
{
	    yyval.node = yyvsp[-1].node;
	  ;
    break;}
case 171:
#line 1139 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_for_head, yyvsp[-5].node, yyvsp[-3].node, yyvsp[-1].node );
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
#line 1143 "yacc.yy"

