
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
#define	T_RANGE	334
#define	T_CELL	335

#line 1 "yacc.yy"


#include "kscript_parsenode.h"
#include "kscript_parser.h"
#include <stdlib.h>

#include <qstring.h>

extern int yylex();

extern QString idl_lexFile;
extern int idl_line_no;

void kscriptInitFlex( const char *_code );

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



#define	YYFINAL		386
#define	YYFLAG		-32768
#define	YYNTBASE	81

#define YYTRANSLATE(x) ((unsigned)(x) <= 335 ? yytranslate[x] : 143)

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
    76,    77,    78,    79,    80
};

#if YYDEBUG != 0
static const short yyprhs[] = {     0,
     0,     1,     4,     6,     8,    11,    14,    17,    20,    23,
    26,    30,    32,    34,    39,    44,    47,    51,    52,    54,
    56,    59,    62,    65,    68,    71,    74,    76,    79,    81,
    85,    87,    91,    93,    96,   101,   105,   110,   114,   116,
   120,   124,   128,   132,   136,   140,   142,   144,   148,   150,
   154,   156,   160,   162,   166,   170,   172,   176,   180,   182,
   186,   190,   194,   196,   199,   202,   205,   208,   211,   213,
   218,   223,   227,   232,   234,   238,   240,   244,   246,   249,
   252,   255,   258,   260,   263,   266,   268,   270,   272,   276,
   278,   280,   282,   284,   286,   288,   290,   292,   295,   299,
   302,   306,   310,   312,   320,   326,   330,   333,   335,   339,
   343,   347,   351,   352,   357,   363,   365,   368,   371,   375,
   378,   380,   384,   389,   392,   396,   397,   399,   401,   404,
   407,   410,   417,   423,   432,   435,   441,   448,   450,   454,
   457,   462,   465,   468,   470,   474,   477,   482,   483,   485,
   487,   490,   493,   499,   503,   507,   510,   514,   516,   522,
   526,   528,   531,   541,   551,   556,   561,   567,   574,   583,
   593,   594,   604,   609
};

static const short yyrhs[] = {    -1,
    51,    97,     0,    82,     0,    83,     0,    83,    82,     0,
   138,    37,     0,    85,    37,     0,   119,    37,     0,   123,
    37,     0,   128,    33,     0,    73,    18,    37,     0,    42,
     0,    84,     0,    65,    23,   133,    33,     0,    86,    23,
    87,    33,     0,    22,    18,     0,    22,    18,    90,     0,
     0,    88,     0,    89,     0,    89,    88,     0,   138,    37,
     0,   128,    33,     0,    94,    37,     0,    95,    37,     0,
   119,    37,     0,    42,     0,     9,    91,     0,    92,     0,
    92,    10,    91,     0,    93,     0,    92,    53,    18,     0,
    18,     0,    36,    18,     0,    92,    18,   115,   118,     0,
     5,    92,    18,     0,    32,     5,    92,    18,     0,    97,
    51,    96,     0,    97,     0,    98,    14,    97,     0,    98,
    52,    97,     0,    98,    49,    97,     0,    98,    50,    97,
     0,    98,    26,    97,     0,    98,    17,    97,     0,    98,
     0,    99,     0,    98,    47,    99,     0,   100,     0,    99,
     8,   100,     0,   101,     0,   100,     3,   101,     0,   102,
     0,   101,    39,   102,     0,   101,    38,   102,     0,   103,
     0,   102,    30,   103,     0,   102,    27,   103,     0,   104,
     0,   103,     4,   104,     0,   103,    40,   104,     0,   103,
    29,   104,     0,   105,     0,    54,   105,     0,    27,   106,
     0,    30,   106,     0,    45,   106,     0,    69,   106,     0,
   106,     0,   106,    25,    96,    35,     0,   106,    23,    96,
    33,     0,   106,    24,    34,     0,   106,    24,   107,    34,
     0,   108,     0,    96,    10,   107,     0,    96,     0,   106,
    53,    18,     0,   109,     0,    61,   110,     0,    62,   110,
     0,   110,    61,     0,   110,    62,     0,   110,     0,   111,
    68,     0,   111,    67,     0,   111,     0,    93,     0,   112,
     0,    24,    96,    34,     0,    21,     0,    80,     0,    79,
     0,    41,     0,     7,     0,    16,     0,    46,     0,    15,
     0,    25,    35,     0,    25,   113,    35,     0,    23,    33,
     0,    23,   114,    33,     0,    96,    10,   113,     0,    96,
     0,    24,    96,    10,    97,    34,    10,   114,     0,    24,
    96,    10,    97,    34,     0,    24,   116,    34,     0,    24,
    34,     0,   117,     0,   117,    10,   116,     0,    19,    92,
    18,     0,    28,    92,    18,     0,    20,    92,    18,     0,
     0,    31,    24,    91,    34,     0,    43,    18,    23,   120,
    33,     0,   121,     0,   121,   120,     0,   128,    33,     0,
    74,   122,    37,     0,   138,    37,     0,    18,     0,    18,
    10,   122,     0,   124,    23,   125,    33,     0,    48,    18,
     0,    48,    18,    90,     0,     0,   126,     0,   127,     0,
   127,   126,     0,   128,    33,     0,   119,    37,     0,    71,
    18,    24,   131,    34,    37,     0,    71,    18,    24,    34,
    37,     0,    55,    24,    19,    18,    34,    23,   133,    33,
     0,   138,    37,     0,    18,    24,    34,    23,   133,     0,
    18,    24,   129,    34,    23,   133,     0,   130,     0,   130,
    10,   129,     0,    19,    18,     0,    19,    18,    51,    96,
     0,    28,    18,     0,    20,    18,     0,   132,     0,   132,
    10,   129,     0,    19,    18,     0,    19,    18,    51,    96,
     0,     0,   134,     0,   135,     0,   135,   134,     0,    96,
    37,     0,    78,    96,    10,    96,    37,     0,    55,    96,
    37,     0,    72,    96,    37,     0,    70,    37,     0,    70,
    96,    37,     0,   139,     0,    77,    23,   134,    33,   136,
     0,    23,   134,    33,     0,   137,     0,   137,   136,     0,
    76,    24,    96,    10,    18,    34,    23,   134,    33,     0,
    12,    24,    18,    10,    18,    34,    23,   134,    33,     0,
    11,    18,    51,    97,     0,   141,    23,   133,    33,     0,
    59,   142,    23,   133,    33,     0,    60,    23,   133,    33,
   141,    37,     0,    57,    24,    96,    34,    23,   133,    33,
   140,     0,    66,    24,    96,    10,    96,    34,    23,   133,
    33,     0,     0,    58,    57,    24,    96,    34,    23,   133,
    33,   140,     0,    56,    24,    96,    34,     0,    24,    96,
    37,    96,    37,    96,    34,     0
};

#endif

#if YYDEBUG != 0
static const short yyrline[] = { 0,
   180,   184,   188,   195,   199,   208,   212,   216,   220,   224,
   228,   233,   238,   245,   254,   263,   268,   278,   282,   290,
   294,   303,   307,   311,   315,   319,   323,   332,   340,   344,
   353,   357,   365,   370,   382,   391,   396,   405,   409,   416,
   420,   424,   428,   432,   436,   440,   448,   452,   460,   464,
   472,   476,   484,   488,   492,   500,   504,   508,   516,   520,
   524,   528,   535,   539,   548,   552,   556,   560,   564,   571,
   575,   579,   583,   587,   594,   599,   606,   611,   618,   622,
   626,   630,   634,   641,   646,   651,   659,   663,   667,   676,
   681,   686,   691,   696,   701,   706,   711,   716,   720,   724,
   728,   735,   739,   746,   750,   759,   763,   771,   775,   784,
   789,   794,   803,   807,   816,   824,   828,   836,   840,   844,
   851,   856,   865,   873,   878,   888,   892,   899,   903,   911,
   915,   919,   924,   929,   938,   945,   951,   959,   963,   970,
   975,   980,   985,   993,   997,  1004,  1009,  1017,  1021,  1028,
  1032,  1040,  1044,  1048,  1052,  1056,  1060,  1064,  1068,  1072,
  1079,  1083,  1091,  1097,  1108,  1116,  1120,  1124,  1128,  1132,
  1139,  1143,  1150,  1157
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
"T_IMPORT","T_VAR","T_UNKNOWN","T_CATCH","T_TRY","T_RAISE","T_RANGE","T_CELL",
"specification","definitions","definition","main","interface_dcl","interface_header",
"interface_body","interface_exports","interface_export","inheritance_spec","qualified_names",
"qualified_name","scoped_name","corba_func_dcl","attr_dcl","assign_expr","equal_expr",
"or_expr","xor_expr","and_expr","shift_expr","add_expr","mult_expr","new_expr",
"unary_expr","index_expr","func_call_params","member_expr","incr_expr","match_expr",
"primary_expr","literal","array_elements","dict_elements","parameter_dcls","param_dcls",
"param_dcl","raises_expr","struct_dcl","struct_exports","struct_export","struct_members",
"class_dcl","class_header","class_body","class_exports","class_export","func_dcl",
"func_params","func_param","signal_params","signal_param","func_body","func_lines",
"func_line","catches","single_catch","const_dcl","loops","else","while","for", NULL
};
#endif

static const short yyr1[] = {     0,
    81,    81,    81,    82,    82,    83,    83,    83,    83,    83,
    83,    83,    83,    84,    85,    86,    86,    87,    87,    88,
    88,    89,    89,    89,    89,    89,    89,    90,    91,    91,
    92,    92,    93,    93,    94,    95,    95,    96,    96,    97,
    97,    97,    97,    97,    97,    97,    98,    98,    99,    99,
   100,   100,   101,   101,   101,   102,   102,   102,   103,   103,
   103,   103,   104,   104,   105,   105,   105,   105,   105,   106,
   106,   106,   106,   106,   107,   107,   108,   108,   109,   109,
   109,   109,   109,   110,   110,   110,   111,   111,   111,   112,
   112,   112,   112,   112,   112,   112,   112,   112,   112,   112,
   112,   113,   113,   114,   114,   115,   115,   116,   116,   117,
   117,   117,   118,   118,   119,   120,   120,   121,   121,   121,
   122,   122,   123,   124,   124,   125,   125,   126,   126,   127,
   127,   127,   127,   127,   127,   128,   128,   129,   129,   130,
   130,   130,   130,   131,   131,   132,   132,   133,   133,   134,
   134,   135,   135,   135,   135,   135,   135,   135,   135,   135,
   136,   136,   137,   137,   138,   139,   139,   139,   139,   139,
   140,   140,   141,   142
};

static const short yyr2[] = {     0,
     0,     2,     1,     1,     2,     2,     2,     2,     2,     2,
     3,     1,     1,     4,     4,     2,     3,     0,     1,     1,
     2,     2,     2,     2,     2,     2,     1,     2,     1,     3,
     1,     3,     1,     2,     4,     3,     4,     3,     1,     3,
     3,     3,     3,     3,     3,     1,     1,     3,     1,     3,
     1,     3,     1,     3,     3,     1,     3,     3,     1,     3,
     3,     3,     1,     2,     2,     2,     2,     2,     1,     4,
     4,     3,     4,     1,     3,     1,     3,     1,     2,     2,
     2,     2,     1,     2,     2,     1,     1,     1,     3,     1,
     1,     1,     1,     1,     1,     1,     1,     2,     3,     2,
     3,     3,     1,     7,     5,     3,     2,     1,     3,     3,
     3,     3,     0,     4,     5,     1,     2,     2,     3,     2,
     1,     3,     4,     2,     3,     0,     1,     1,     2,     2,
     2,     6,     5,     8,     2,     5,     6,     1,     3,     2,
     4,     2,     2,     1,     3,     2,     4,     0,     1,     1,
     2,     2,     5,     3,     3,     2,     3,     1,     5,     3,
     1,     2,     9,     9,     4,     4,     5,     6,     8,     9,
     0,     9,     4,     7
};

static const short yydefact[] = {     1,
     0,     0,     0,    12,     0,     0,     0,     0,     0,     3,
     4,    13,     0,     0,     0,     0,     0,     0,     0,     0,
     0,    16,     0,   124,    94,    97,    95,    33,    90,     0,
     0,     0,     0,     0,     0,    93,     0,    96,     0,     0,
     0,     0,    92,    91,    87,     2,    46,    47,    49,    51,
    53,    56,    59,    63,    69,    74,    78,    83,    86,    88,
   148,     0,     5,     7,    18,     8,     9,   126,    10,     6,
     0,     0,     0,     0,     0,     0,   138,     0,    17,     0,
   125,     0,   100,     0,     0,    39,    98,   103,     0,    65,
    66,    34,    67,    64,    79,    80,    68,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,    81,    82,    85,
    84,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,   149,   150,   158,     0,    11,     0,
    33,     0,    27,     0,    19,    20,     0,    31,     0,     0,
     0,     0,     0,     0,     0,     0,     0,   127,   128,     0,
     0,   165,   140,   143,   142,   148,     0,     0,    28,    29,
     0,     0,   116,     0,     0,     0,   101,    89,     0,     0,
    99,    40,    45,    44,    48,    42,    43,    41,    50,    52,
    55,    54,    58,    57,    60,    62,    61,     0,    72,    76,
     0,     0,    77,     0,     0,     0,     0,     0,     0,     0,
   148,     0,   156,     0,     0,     0,     0,   152,    14,   151,
   148,     0,     0,    15,    21,     0,     0,    24,    25,    26,
    23,    22,     0,     0,   131,   123,   129,   130,   135,     0,
   136,   148,   139,     0,   121,     0,   115,   117,   118,   120,
     0,    38,   102,    71,     0,    73,    70,     0,   160,   154,
     0,     0,     0,   148,     0,     0,   157,   155,     0,     0,
     0,    36,     0,     0,   113,    32,     0,     0,   141,   137,
    30,     0,   119,     0,    75,   173,     0,     0,     0,     0,
     0,     0,     0,   166,    37,     0,     0,     0,   107,     0,
   108,     0,    35,     0,     0,     0,     0,   144,   122,   105,
   148,     0,   167,     0,     0,     0,     0,   159,   161,   153,
     0,     0,     0,   106,     0,     0,     0,   146,   133,     0,
     0,     0,     0,     0,   168,     0,     0,     0,   162,   110,
   112,   111,   109,     0,   148,     0,   132,   145,   104,   171,
     0,   148,     0,     0,   114,     0,   147,     0,   169,   174,
     0,     0,     0,   134,     0,   170,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,   148,   164,   163,
     0,   171,   172,     0,     0,     0
};

static const short yydefgoto[] = {   384,
    10,    11,    12,    13,    14,   144,   145,   146,    79,   169,
   170,    45,   149,   150,   133,    86,    47,    48,    49,    50,
    51,    52,    53,    54,    55,   201,    56,    57,    58,    59,
    60,    89,    84,   275,   300,   301,   303,    15,   172,   173,
   246,    16,    17,   157,   158,   159,    18,    76,    77,   307,
   308,   134,   135,   136,   318,   319,    19,   137,   359,   138,
   210
};

static const short yypact[] = {   132,
    -3,    45,     3,-32768,    30,    77,   515,    74,    92,-32768,
   185,-32768,    75,    96,    86,    93,   113,   124,   115,   114,
   119,   158,   146,   158,-32768,-32768,-32768,-32768,-32768,    58,
   515,   110,   607,   607,   164,-32768,   607,-32768,   575,   642,
   642,   607,-32768,-32768,-32768,-32768,    29,   183,   190,    27,
    11,    16,-32768,-32768,    91,-32768,-32768,    80,   -15,-32768,
   329,   167,-32768,-32768,    18,-32768,-32768,    19,-32768,-32768,
   515,   177,   180,   188,   191,   174,   203,   -10,-32768,    -1,
-32768,   515,-32768,   182,   184,   165,-32768,   207,   186,    91,
    91,-32768,    91,-32768,-32768,-32768,    91,   515,   515,   515,
   515,   515,   515,   515,   515,   515,   515,   515,   515,   515,
   515,   515,   515,   515,   395,   515,   201,-32768,-32768,-32768,
-32768,   263,   515,   196,   198,   199,   206,   202,   455,   515,
   208,   515,   193,   209,-32768,   329,-32768,   211,-32768,   -10,
    45,   227,-32768,   213,-32768,    18,    -9,-32768,   204,   210,
   212,   215,   214,   216,   217,   219,   226,-32768,    19,   228,
   225,-32768,   218,-32768,-32768,   329,   229,    44,-32768,    -6,
   245,   231,    -1,   232,   230,   258,-32768,-32768,   515,   515,
-32768,-32768,-32768,-32768,   183,-32768,-32768,-32768,   190,    27,
    11,    11,    16,    16,-32768,-32768,-32768,   238,-32768,   262,
   239,   240,-32768,   515,   241,   243,   515,   515,   515,   253,
   329,   515,-32768,   246,   248,   329,   267,-32768,-32768,-32768,
   329,    -4,   -10,-32768,-32768,   265,   264,-32768,-32768,-32768,
-32768,-32768,   272,   268,-32768,-32768,-32768,-32768,-32768,   515,
-32768,   329,-32768,   -10,   284,   260,-32768,-32768,-32768,-32768,
   515,-32768,-32768,-32768,   515,-32768,-32768,    23,-32768,-32768,
   261,   266,   269,   329,   277,   288,-32768,-32768,   278,   515,
   279,-32768,    14,   166,   270,-32768,   287,     6,-32768,-32768,
-32768,   245,-32768,   280,-32768,-32768,   290,   515,   283,   271,
   515,    -5,   289,-32768,-32768,   -10,   -10,   -10,-32768,   294,
   292,   297,-32768,   296,   313,   300,   305,   324,-32768,   336,
   329,   311,-32768,   312,   317,   331,   333,-32768,    -5,-32768,
    17,    24,    41,-32768,   140,   -10,   335,   309,-32768,   325,
    44,   337,   330,   515,-32768,   341,   348,   515,-32768,-32768,
-32768,-32768,-32768,   334,   329,   515,-32768,-32768,-32768,   314,
   339,   329,   357,   359,-32768,   338,-32768,   319,-32768,-32768,
   344,   360,   361,-32768,   356,-32768,   347,   353,   515,   369,
   370,   362,   329,   329,   371,   349,   364,   329,-32768,-32768,
   367,   314,-32768,   403,   404,-32768
};

static const short yypgoto[] = {-32768,
   394,-32768,-32768,-32768,-32768,-32768,   275,-32768,   388,  -238,
   -53,   -60,-32768,-32768,   -31,     4,-32768,   316,   310,   308,
    54,    78,    99,   384,    87,   169,-32768,-32768,   161,-32768,
-32768,   247,    94,-32768,   103,-32768,-32768,   -37,   257,-32768,
   150,-32768,-32768,-32768,   274,-32768,   -46,  -165,-32768,-32768,
-32768,  -153,  -120,-32768,   116,-32768,   -41,-32768,    52,   147,
-32768
};


#define	YYLAST		722


static const short yytable[] = {    85,
    88,   205,   243,   244,   148,   281,   316,    28,   226,     1,
    46,   147,   241,   272,    20,   220,     2,   148,   152,   111,
    22,   160,   140,   153,   305,    35,   161,   151,     1,     1,
   156,   295,   251,   174,   340,   141,     2,   109,   175,   306,
   110,   341,    98,   227,   112,    99,   227,    23,   227,   142,
   176,   120,   121,    35,   100,   113,   178,   265,   342,   143,
     5,     5,    72,    73,   107,   108,   227,   271,    21,   227,
   317,    74,   171,   154,   162,   101,   227,   102,   103,   148,
   104,    82,   198,   200,   202,   148,   222,   344,   280,   155,
    83,   206,   147,   227,    24,   269,    61,   214,   215,   152,
   217,   182,   183,   184,   153,   186,   187,   188,   151,    62,
   289,    64,   160,   114,   115,   116,    25,   161,    65,    90,
    91,   156,    66,    93,    26,    27,   174,    28,    97,    67,
    29,   175,    30,    31,    32,    68,    33,    72,    73,    34,
   118,   119,     1,   117,    87,    35,    74,   252,    88,     2,
    36,    70,    75,     3,    37,    38,    69,   333,   296,   297,
   191,   192,   148,    39,    71,   348,    78,   298,    80,   273,
    40,    41,   258,     4,     5,   261,   262,   263,    42,     6,
   266,    92,     7,   148,   296,   297,   193,   194,    43,    44,
   105,   356,   106,   298,   163,     1,     8,   164,   361,   299,
    95,    96,     2,   139,     9,   165,     3,   167,   279,   195,
   196,   197,   168,   166,   177,   179,   180,   178,   203,   207,
   181,   208,   209,   200,   381,   212,     4,     5,   211,   218,
   216,   223,     6,   221,   234,   148,   148,   148,   293,   233,
   228,   219,   321,   322,   323,   224,   229,   231,   230,     8,
   232,   242,   376,   377,   284,   235,   312,     9,   236,   315,
   238,   239,   245,   247,   249,   148,   250,   251,   240,    25,
   254,   255,   256,   259,   257,   264,   270,    26,    27,   260,
    28,   276,   267,    29,   268,   122,   204,    32,   274,    33,
   277,   278,    34,   282,   286,    83,   283,   291,    35,   287,
   302,   325,   351,    36,   304,   288,   354,    37,    38,   290,
   292,   294,   311,   310,   357,   313,    39,   123,   124,   125,
   326,   126,   127,    40,    41,   320,   124,   324,   128,   327,
   328,    42,   129,   331,   130,    25,   329,   372,   330,   131,
   132,    43,    44,    26,    27,   332,    28,   334,   335,    29,
   336,   122,    31,    32,   337,    33,   338,   345,    34,   346,
    82,   347,   350,   352,    35,   353,   362,   355,   363,    36,
   364,   358,   360,    37,    38,   365,   366,   367,   368,   369,
   370,   379,    39,   123,   124,   125,   371,   126,   127,    40,
    41,   373,   374,   378,   128,   375,   380,    42,   129,   382,
   130,    25,   385,   386,    63,   131,   132,    43,    44,    26,
    27,    81,    28,   190,   189,    29,   185,    30,    31,    32,
   225,    33,    94,   285,    34,   349,   253,   343,   199,   248,
    35,   309,   237,   383,   339,    36,   314,     0,     0,    37,
    38,     0,     0,     0,     0,     0,     0,     0,    39,     0,
     0,     0,     0,     0,     0,    40,    41,     0,     0,     0,
     0,    25,     0,    42,     0,     0,     0,     0,     0,    26,
    27,     0,    28,    43,    44,    29,     0,    30,    31,    32,
     0,    33,     0,     0,    34,     0,     0,     0,     0,     0,
    35,   213,     0,     0,     0,    36,     0,     0,     0,    37,
    38,     0,     0,     0,     0,     0,     0,     0,    39,     0,
     0,     0,     0,     0,     0,    40,    41,     0,     0,     0,
     0,    25,     0,    42,     0,     0,     0,     0,     0,    26,
    27,     0,    28,    43,    44,    29,     0,    30,    31,    32,
     0,    33,     0,     0,    34,     0,     0,     0,     0,     0,
    35,     0,     0,     0,     0,    36,     0,     0,     0,    37,
    38,     0,     0,     0,     0,     0,     0,     0,    39,     0,
     0,     0,     0,     0,     0,    40,    41,     0,     0,     0,
     0,    25,     0,    42,     0,     0,     0,     0,     0,    26,
    27,     0,    28,    43,    44,    29,     0,    30,    31,    32,
     0,    33,     0,     0,    34,     0,     0,     0,     0,     0,
    35,     0,     0,    25,     0,    36,     0,     0,     0,    37,
    38,    26,    27,     0,    28,     0,     0,    29,     0,    30,
    31,    32,     0,     0,     0,    40,    41,     0,     0,     0,
     0,     0,    35,    42,     0,     0,     0,    36,    25,     0,
     0,     0,    38,    43,    44,     0,    26,    27,     0,    28,
     0,     0,    29,     0,    30,    31,    32,    40,    41,     0,
     0,     0,     0,     0,     0,     0,     0,    35,     0,     0,
     0,     0,    36,     0,     0,    43,    44,    38,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
    43,    44
};

static const short yycheck[] = {    31,
    32,   122,   168,    10,    65,   244,    12,    18,    18,    11,
     7,    65,   166,    18,    18,   136,    18,    78,    65,     4,
    18,    68,     5,    65,    19,    36,    68,    65,    11,    11,
    68,    18,    10,    80,    18,    18,    18,    27,    80,    34,
    30,    18,    14,    53,    29,    17,    53,    18,    53,    32,
    82,    67,    68,    36,    26,    40,    34,   211,    18,    42,
    43,    43,    19,    20,    38,    39,    53,   221,    24,    53,
    76,    28,    74,    55,    71,    47,    53,    49,    50,   140,
    52,    24,   114,   115,   116,   146,   140,   326,   242,    71,
    33,   123,   146,    53,    18,   216,    23,   129,   130,   146,
   132,    98,    99,   100,   146,   102,   103,   104,   146,    18,
   264,    37,   159,    23,    24,    25,     7,   159,    23,    33,
    34,   159,    37,    37,    15,    16,   173,    18,    42,    37,
    21,   173,    23,    24,    25,    23,    27,    19,    20,    30,
    61,    62,    11,    53,    35,    36,    28,   179,   180,    18,
    41,    37,    34,    22,    45,    46,    33,   311,    19,    20,
   107,   108,   223,    54,    51,   331,     9,    28,    23,   223,
    61,    62,   204,    42,    43,   207,   208,   209,    69,    48,
   212,    18,    51,   244,    19,    20,   109,   110,    79,    80,
     8,   345,     3,    28,    18,    11,    65,    18,   352,    34,
    40,    41,    18,    37,    73,    18,    22,    34,   240,   111,
   112,   113,    10,    23,    33,    51,    10,    34,    18,    24,
    35,    24,    24,   255,   378,    24,    42,    43,    23,    37,
    23,     5,    48,    23,    18,   296,   297,   298,   270,    24,
    37,    33,   296,   297,   298,    33,    37,    33,    37,    65,
    37,    23,   373,   374,   251,    37,   288,    73,    33,   291,
    33,    37,    18,    33,    33,   326,    37,    10,    51,     7,
    33,    10,    34,    33,    35,    23,    10,    15,    16,    37,
    18,    18,    37,    21,    37,    23,    24,    25,    24,    27,
    19,    24,    30,    10,    34,    33,    37,    10,    36,    34,
    31,    10,   334,    41,    18,    37,   338,    45,    46,    33,
    33,    33,    23,    34,   346,    33,    54,    55,    56,    57,
    24,    59,    60,    61,    62,    37,    56,    34,    66,    34,
    18,    69,    70,    10,    72,     7,    37,   369,    34,    77,
    78,    79,    80,    15,    16,    10,    18,    37,    37,    21,
    34,    23,    24,    25,    24,    27,    24,    23,    30,    51,
    24,    37,    33,    23,    36,    18,    10,    34,    10,    41,
    33,    58,    34,    45,    46,    57,    33,    18,    18,    24,
    34,    33,    54,    55,    56,    57,    34,    59,    60,    61,
    62,    23,    23,    23,    66,    34,    33,    69,    70,    33,
    72,     7,     0,     0,    11,    77,    78,    79,    80,    15,
    16,    24,    18,   106,   105,    21,   101,    23,    24,    25,
   146,    27,    39,   255,    30,   332,   180,   325,    34,   173,
    36,   282,   159,   382,   319,    41,   290,    -1,    -1,    45,
    46,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    54,    -1,
    -1,    -1,    -1,    -1,    -1,    61,    62,    -1,    -1,    -1,
    -1,     7,    -1,    69,    -1,    -1,    -1,    -1,    -1,    15,
    16,    -1,    18,    79,    80,    21,    -1,    23,    24,    25,
    -1,    27,    -1,    -1,    30,    -1,    -1,    -1,    -1,    -1,
    36,    37,    -1,    -1,    -1,    41,    -1,    -1,    -1,    45,
    46,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    54,    -1,
    -1,    -1,    -1,    -1,    -1,    61,    62,    -1,    -1,    -1,
    -1,     7,    -1,    69,    -1,    -1,    -1,    -1,    -1,    15,
    16,    -1,    18,    79,    80,    21,    -1,    23,    24,    25,
    -1,    27,    -1,    -1,    30,    -1,    -1,    -1,    -1,    -1,
    36,    -1,    -1,    -1,    -1,    41,    -1,    -1,    -1,    45,
    46,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    54,    -1,
    -1,    -1,    -1,    -1,    -1,    61,    62,    -1,    -1,    -1,
    -1,     7,    -1,    69,    -1,    -1,    -1,    -1,    -1,    15,
    16,    -1,    18,    79,    80,    21,    -1,    23,    24,    25,
    -1,    27,    -1,    -1,    30,    -1,    -1,    -1,    -1,    -1,
    36,    -1,    -1,     7,    -1,    41,    -1,    -1,    -1,    45,
    46,    15,    16,    -1,    18,    -1,    -1,    21,    -1,    23,
    24,    25,    -1,    -1,    -1,    61,    62,    -1,    -1,    -1,
    -1,    -1,    36,    69,    -1,    -1,    -1,    41,     7,    -1,
    -1,    -1,    46,    79,    80,    -1,    15,    16,    -1,    18,
    -1,    -1,    21,    -1,    23,    24,    25,    61,    62,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    36,    -1,    -1,
    -1,    -1,    41,    -1,    -1,    79,    80,    46,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    79,    80
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
#line 181 "yacc.yy"
{
	    theParser->setRootNode( NULL );
	  ;
    break;}
case 2:
#line 185 "yacc.yy"
{
	    theParser->setRootNode( yyvsp[0].node );
	  ;
    break;}
case 3:
#line 189 "yacc.yy"
{
	    theParser->setRootNode( new KSParseNode( definitions, yyvsp[0].node ) );
	  ;
    break;}
case 4:
#line 196 "yacc.yy"
{
	    yyval.node = new KSParseNode( definitions, yyvsp[0].node );
	  ;
    break;}
case 5:
#line 200 "yacc.yy"
{
	    yyval.node = new KSParseNode( definitions, yyvsp[-1].node );
	    yyval.node->setBranch( 2, yyvsp[0].node );
	  ;
    break;}
case 6:
#line 209 "yacc.yy"
{
	    yyval.node = yyvsp[-1].node;
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
	    yyval.node = yyvsp[-1].node;
	  ;
    break;}
case 10:
#line 225 "yacc.yy"
{
	    yyval.node = yyvsp[-1].node;
	  ;
    break;}
case 11:
#line 229 "yacc.yy"
{
	    yyval.node = new KSParseNode( import );
	    yyval.node->setIdent( yyvsp[-1].ident );
	  ;
    break;}
case 12:
#line 234 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_pragma );
	    yyval.node->setIdent( yyvsp[0]._str );
	  ;
    break;}
case 13:
#line 239 "yacc.yy"
{
	    yyval.node = yyvsp[0].node;
	  ;
    break;}
case 14:
#line 246 "yacc.yy"
{
	    yyval.node = new KSParseNode( func_dcl );
	    yyval.node->setBranch( 2, yyvsp[-1].node );
	    yyval.node->setIdent( "main" );
	  ;
    break;}
case 15:
#line 256 "yacc.yy"
{
	    yyval.node = new KSParseNode( interface_dcl, yyvsp[-3].node, yyvsp[-1].node );
	  ;
    break;}
case 16:
#line 264 "yacc.yy"
{
	    yyval.node = new KSParseNode( interface_header );
	    yyval.node->setIdent( yyvsp[0].ident );
	  ;
    break;}
case 17:
#line 269 "yacc.yy"
{
	    yyval.node = new KSParseNode( interface_header );
	    yyval.node->setIdent( yyvsp[-1].ident );
	    yyval.node->setBranch( 1, yyvsp[0].node );
	  ;
    break;}
case 18:
#line 279 "yacc.yy"
{
	    yyval.node = NULL;
	  ;
    break;}
case 19:
#line 283 "yacc.yy"
{
	    yyval.node = yyvsp[0].node;
	  ;
    break;}
case 20:
#line 291 "yacc.yy"
{
	    yyval.node = new KSParseNode( exports, yyvsp[0].node );
	  ;
    break;}
case 21:
#line 295 "yacc.yy"
{
	    yyval.node = new KSParseNode( exports, yyvsp[-1].node );
	    yyval.node->setBranch( 2, yyvsp[0].node );
	  ;
    break;}
case 22:
#line 304 "yacc.yy"
{
	    yyval.node = yyvsp[-1].node;
	  ;
    break;}
case 23:
#line 308 "yacc.yy"
{
	    yyval.node = yyvsp[-1].node;
	  ;
    break;}
case 24:
#line 312 "yacc.yy"
{
	    yyval.node = yyvsp[-1].node;
	  ;
    break;}
case 25:
#line 316 "yacc.yy"
{
	    yyval.node = yyvsp[-1].node;
	  ;
    break;}
case 26:
#line 320 "yacc.yy"
{
	    yyval.node = yyvsp[-1].node;
	  ;
    break;}
case 27:
#line 324 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_pragma );
	    yyval.node->setIdent( yyvsp[0]._str );
	  ;
    break;}
case 28:
#line 333 "yacc.yy"
{
	    yyval.node = yyvsp[0].node;
	  ;
    break;}
case 29:
#line 341 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_qualified_names, yyvsp[0].node );
	  ;
    break;}
case 30:
#line 345 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_qualified_names, yyvsp[-2].node );
	    yyval.node->setBranch( 2, yyvsp[0].node );
	  ;
    break;}
case 31:
#line 354 "yacc.yy"
{
	    yyval.node = yyvsp[0].node;
	  ;
    break;}
case 32:
#line 358 "yacc.yy"
{
	    yyval.node = new KSParseNode( member_expr, yyvsp[-2].node );
	    yyval.node->setIdent( yyvsp[0].ident );
	  ;
    break;}
case 33:
#line 366 "yacc.yy"
{
	    yyval.node = new KSParseNode( scoped_name );
	    yyval.node->setIdent( yyvsp[0].ident );
	  ;
    break;}
case 34:
#line 371 "yacc.yy"
{
	    yyval.node = new KSParseNode( scoped_name );
	    QString name = "::";
	    name += *(yyvsp[0].ident);
	    delete yyvsp[0].ident;
	    yyval.node->setIdent( name );
	  ;
    break;}
case 35:
#line 383 "yacc.yy"
{
	    yyval.node = new KSParseNode( corba_func_dcl, yyvsp[-3].node, yyvsp[-1].node, yyvsp[0].node );
	    yyval.node->setIdent( yyvsp[-2].ident );
	  ;
    break;}
case 36:
#line 392 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_attribute, yyvsp[-1].node );
	    yyval.node->setIdent( yyvsp[0].ident );
	  ;
    break;}
case 37:
#line 397 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_readonly_attribute, yyvsp[-1].node );
	    yyval.node->setIdent( yyvsp[0].ident );
	  ;
    break;}
case 38:
#line 406 "yacc.yy"
{
	    yyval.node = new KSParseNode( assign_expr, yyvsp[-2].node, yyvsp[0].node );
	  ;
    break;}
case 39:
#line 410 "yacc.yy"
{
	    yyval.node = yyvsp[0].node;
	  ;
    break;}
case 40:
#line 417 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_equal, yyvsp[-2].node, yyvsp[0].node );
	  ;
    break;}
case 41:
#line 421 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_notequal, yyvsp[-2].node, yyvsp[0].node );
	  ;
    break;}
case 42:
#line 425 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_less_or_equal, yyvsp[-2].node, yyvsp[0].node );
	  ;
    break;}
case 43:
#line 429 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_greater_or_equal, yyvsp[-2].node, yyvsp[0].node );
	  ;
    break;}
case 44:
#line 433 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_less, yyvsp[-2].node, yyvsp[0].node );
	  ;
    break;}
case 45:
#line 437 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_greater, yyvsp[-2].node, yyvsp[0].node );
	  ;
    break;}
case 46:
#line 441 "yacc.yy"
{
	    yyval.node = yyvsp[0].node;
	  ;
    break;}
case 47:
#line 449 "yacc.yy"
{
	    yyval.node = yyvsp[0].node;
	  ;
    break;}
case 48:
#line 453 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_vertical_line, yyvsp[-2].node, yyvsp[0].node );
	  ;
    break;}
case 49:
#line 461 "yacc.yy"
{
	    yyval.node = yyvsp[0].node;
	  ;
    break;}
case 50:
#line 465 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_circumflex, yyvsp[-2].node, yyvsp[0].node );
	  ;
    break;}
case 51:
#line 473 "yacc.yy"
{
	    yyval.node = yyvsp[0].node;
	  ;
    break;}
case 52:
#line 477 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_ampersand, yyvsp[-2].node, yyvsp[0].node );
	  ;
    break;}
case 53:
#line 485 "yacc.yy"
{
	    yyval.node = yyvsp[0].node;
	  ;
    break;}
case 54:
#line 489 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_shiftright, yyvsp[-2].node, yyvsp[0].node );
	  ;
    break;}
case 55:
#line 493 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_shiftleft, yyvsp[-2].node, yyvsp[0].node );
	  ;
    break;}
case 56:
#line 501 "yacc.yy"
{
	    yyval.node = yyvsp[0].node;
	  ;
    break;}
case 57:
#line 505 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_plus_sign, yyvsp[-2].node, yyvsp[0].node );
	  ;
    break;}
case 58:
#line 509 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_minus_sign, yyvsp[-2].node, yyvsp[0].node );
	  ;
    break;}
case 59:
#line 517 "yacc.yy"
{
	    yyval.node = yyvsp[0].node;
	  ;
    break;}
case 60:
#line 521 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_asterik, yyvsp[-2].node, yyvsp[0].node );
	  ;
    break;}
case 61:
#line 525 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_solidus, yyvsp[-2].node, yyvsp[0].node );
	  ;
    break;}
case 62:
#line 529 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_percent_sign, yyvsp[-2].node, yyvsp[0].node );
	  ;
    break;}
case 63:
#line 536 "yacc.yy"
{
	    yyval.node = yyvsp[0].node;
	  ;
    break;}
case 64:
#line 540 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_new, yyvsp[0].node );
	  ;
    break;}
case 65:
#line 549 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_minus_sign, yyvsp[0].node );
	  ;
    break;}
case 66:
#line 553 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_plus_sign, yyvsp[0].node );
	  ;
    break;}
case 67:
#line 557 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_tilde, yyvsp[0].node );
	  ;
    break;}
case 68:
#line 561 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_not, yyvsp[0].node );
	  ;
    break;}
case 69:
#line 565 "yacc.yy"
{
	    yyval.node = yyvsp[0].node;
	  ;
    break;}
case 70:
#line 572 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_array, yyvsp[-3].node, yyvsp[-1].node );
	  ;
    break;}
case 71:
#line 576 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_dict, yyvsp[-3].node, yyvsp[-1].node );
	  ;
    break;}
case 72:
#line 580 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_func_call, yyvsp[-2].node );
	  ;
    break;}
case 73:
#line 584 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_func_call, yyvsp[-3].node, yyvsp[-1].node );
	  ;
    break;}
case 74:
#line 588 "yacc.yy"
{
	    yyval.node = yyvsp[0].node;
	  ;
    break;}
case 75:
#line 595 "yacc.yy"
{
	    yyval.node = new KSParseNode( func_call_params, yyvsp[-2].node, yyvsp[0].node );
	  ;
    break;}
case 76:
#line 600 "yacc.yy"
{
	    yyval.node = new KSParseNode( func_call_params, yyvsp[0].node );
	  ;
    break;}
case 77:
#line 607 "yacc.yy"
{
	    yyval.node = new KSParseNode( member_expr, yyvsp[-2].node );
	    yyval.node->setIdent( yyvsp[0].ident );
	  ;
    break;}
case 78:
#line 612 "yacc.yy"
{
	    yyval.node = yyvsp[0].node;
	  ;
    break;}
case 79:
#line 619 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_incr, yyvsp[0].node );
	  ;
    break;}
case 80:
#line 623 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_decr, yyvsp[0].node );
	  ;
    break;}
case 81:
#line 627 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_incr, yyvsp[-1].node );
	  ;
    break;}
case 82:
#line 631 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_decr, yyvsp[-1].node );
	  ;
    break;}
case 83:
#line 635 "yacc.yy"
{
	    yyval.node = yyvsp[0].node;
	  ;
    break;}
case 84:
#line 642 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_match, yyvsp[-1].node );
	    yyval.node->setIdent( yyvsp[0].ident );
	  ;
    break;}
case 85:
#line 647 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_subst, yyvsp[-1].node );
	    yyval.node->setIdent( yyvsp[0].ident );
	  ;
    break;}
case 86:
#line 652 "yacc.yy"
{
	    yyval.node = yyvsp[0].node;
	  ;
    break;}
case 87:
#line 660 "yacc.yy"
{
	    yyval.node = yyvsp[0].node;
	  ;
    break;}
case 88:
#line 664 "yacc.yy"
{
	    yyval.node = yyvsp[0].node;
	  ;
    break;}
case 89:
#line 668 "yacc.yy"
{
	    yyval.node = yyvsp[-1].node;
	  ;
    break;}
case 90:
#line 677 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_integer_literal );
	    yyval.node->setIntegerLiteral( yyvsp[0]._int );
	  ;
    break;}
case 91:
#line 682 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_cell );
	    yyval.node->setStringLiteral( yyvsp[0]._str );
	  ;
    break;}
case 92:
#line 687 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_range );
	    yyval.node->setStringLiteral( yyvsp[0]._str );
	  ;
    break;}
case 93:
#line 692 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_string_literal );
	    yyval.node->setStringLiteral( yyvsp[0]._str );
	  ;
    break;}
case 94:
#line 697 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_character_literal );
	    yyval.node->setCharacterLiteral( yyvsp[0]._char );
	  ;
    break;}
case 95:
#line 702 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_floating_pt_literal );
	    yyval.node->setFloatingPtLiteral( yyvsp[0]._float );
	  ;
    break;}
case 96:
#line 707 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_boolean_literal );
	    yyval.node->setBooleanLiteral( true );
	  ;
    break;}
case 97:
#line 712 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_boolean_literal );
	    yyval.node->setBooleanLiteral( false );
	  ;
    break;}
case 98:
#line 717 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_array_const );
	  ;
    break;}
case 99:
#line 721 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_array_const, yyvsp[-1].node );
	  ;
    break;}
case 100:
#line 725 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_dict_const );
	  ;
    break;}
case 101:
#line 729 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_dict_const, yyvsp[-1].node );
	  ;
    break;}
case 102:
#line 736 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_array_element, yyvsp[-2].node, yyvsp[0].node );
	  ;
    break;}
case 103:
#line 740 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_array_element, yyvsp[0].node  );
	  ;
    break;}
case 104:
#line 747 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_dict_element, yyvsp[-5].node, yyvsp[-3].node, yyvsp[0].node );
	  ;
    break;}
case 105:
#line 751 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_dict_element, yyvsp[-3].node, yyvsp[-1].node );
	  ;
    break;}
case 106:
#line 760 "yacc.yy"
{
	    yyval.node = yyvsp[-1].node;
	  ;
    break;}
case 107:
#line 764 "yacc.yy"
{
	    yyval.node = NULL;
	  ;
    break;}
case 108:
#line 772 "yacc.yy"
{
	    yyval.node = new KSParseNode( param_dcls, yyvsp[0].node );
	  ;
    break;}
case 109:
#line 776 "yacc.yy"
{
	    yyval.node = new KSParseNode( param_dcls, yyvsp[-2].node );
	    yyval.node->setBranch( 2, yyvsp[0].node );
	  ;
    break;}
case 110:
#line 785 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_in_param_dcl, yyvsp[-1].node );
	    yyval.node->setIdent( yyvsp[0].ident );
	  ;
    break;}
case 111:
#line 790 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_out_param_dcl, yyvsp[-1].node );
	    yyval.node->setIdent( yyvsp[0].ident );
	  ;
    break;}
case 112:
#line 795 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_inout_param_dcl, yyvsp[-1].node );
	    yyval.node->setIdent( yyvsp[0].ident );
	  ;
    break;}
case 113:
#line 804 "yacc.yy"
{
	    yyval.node = NULL;
	  ;
    break;}
case 114:
#line 808 "yacc.yy"
{
	    yyval.node = new KSParseNode( raises_expr, yyvsp[-1].node );
	  ;
    break;}
case 115:
#line 817 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_struct, yyvsp[-1].node );
	    yyval.node->setIdent( yyvsp[-3].ident );
	  ;
    break;}
case 116:
#line 825 "yacc.yy"
{
	    yyval.node = new KSParseNode( exports, yyvsp[0].node );
	  ;
    break;}
case 117:
#line 829 "yacc.yy"
{
	    yyval.node = new KSParseNode( exports, yyvsp[-1].node );
	    yyval.node->setBranch( 2, yyvsp[0].node );
	  ;
    break;}
case 118:
#line 837 "yacc.yy"
{
	    yyval.node = yyvsp[-1].node;
	  ;
    break;}
case 119:
#line 841 "yacc.yy"
{
	    yyval.node = yyvsp[-1].node;
	  ;
    break;}
case 120:
#line 845 "yacc.yy"
{
	    yyval.node = yyvsp[-1].node;
	  ;
    break;}
case 121:
#line 852 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_struct_members );
	    yyval.node->setIdent( yyvsp[0].ident );
	  ;
    break;}
case 122:
#line 857 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_struct_members, yyvsp[0].node );
	    yyval.node->setIdent( yyvsp[-2].ident );
	  ;
    break;}
case 123:
#line 866 "yacc.yy"
{
	    yyval.node = new KSParseNode( class_dcl, yyvsp[-3].node, yyvsp[-1].node );
	  ;
    break;}
case 124:
#line 874 "yacc.yy"
{
	    yyval.node = new KSParseNode( class_header );
	    yyval.node->setIdent( yyvsp[0].ident );
	  ;
    break;}
case 125:
#line 879 "yacc.yy"
{
	    yyval.node = new KSParseNode( class_header );
	    yyval.node->setIdent( yyvsp[-1].ident );
	    yyval.node->setBranch( 1, yyvsp[0].node );
	  ;
    break;}
case 126:
#line 889 "yacc.yy"
{
	    yyval.node = NULL;
	  ;
    break;}
case 127:
#line 893 "yacc.yy"
{
	    yyval.node = yyvsp[0].node;
	  ;
    break;}
case 128:
#line 900 "yacc.yy"
{
	    yyval.node = new KSParseNode( exports, yyvsp[0].node );
	  ;
    break;}
case 129:
#line 904 "yacc.yy"
{
	    yyval.node = new KSParseNode( exports, yyvsp[-1].node );
	    yyval.node->setBranch( 2, yyvsp[0].node );
	  ;
    break;}
case 130:
#line 912 "yacc.yy"
{
	    yyval.node = yyvsp[-1].node;
	  ;
    break;}
case 131:
#line 916 "yacc.yy"
{
	    yyval.node = yyvsp[-1].node;
	  ;
    break;}
case 132:
#line 920 "yacc.yy"
{
	    yyval.node = new KSParseNode( signal_dcl, yyvsp[-2].node );
	    yyval.node->setIdent( yyvsp[-4].ident );
	  ;
    break;}
case 133:
#line 925 "yacc.yy"
{
	    yyval.node = new KSParseNode( signal_dcl, 0 );
	    yyval.node->setIdent( yyvsp[-3].ident );
	  ;
    break;}
case 134:
#line 930 "yacc.yy"
{
	    yyval.node = new KSParseNode( destructor_dcl );
	    KSParseNode* n = new KSParseNode( func_param_in );
	    n->setIdent( yyvsp[-4].ident );
	    yyval.node->setBranch( 1, n );
	    yyval.node->setBranch( 2, yyvsp[-1].node );
	    yyval.node->setIdent( "delete" );
	  ;
    break;}
case 135:
#line 939 "yacc.yy"
{
	    yyval.node = yyvsp[-1].node;
	  ;
    break;}
case 136:
#line 946 "yacc.yy"
{
	    yyval.node = new KSParseNode( func_dcl );
	    yyval.node->setBranch( 2, yyvsp[0].node );
	    yyval.node->setIdent( yyvsp[-4].ident );
	  ;
    break;}
case 137:
#line 952 "yacc.yy"
{
	    yyval.node = new KSParseNode( func_dcl, yyvsp[-3].node, yyvsp[0].node );
	    yyval.node->setIdent( yyvsp[-5].ident );
	  ;
    break;}
case 138:
#line 960 "yacc.yy"
{
	    yyval.node = new KSParseNode( func_params, yyvsp[0].node );
	  ;
    break;}
case 139:
#line 964 "yacc.yy"
{
	    yyval.node = new KSParseNode( func_params, yyvsp[-2].node, yyvsp[0].node );
	  ;
    break;}
case 140:
#line 971 "yacc.yy"
{
	    yyval.node = new KSParseNode( func_param_in );
	    yyval.node->setIdent( yyvsp[0].ident );
	  ;
    break;}
case 141:
#line 976 "yacc.yy"
{
	    yyval.node = new KSParseNode( func_param_in, yyvsp[0].node );
	    yyval.node->setIdent( yyvsp[-2].ident );
	  ;
    break;}
case 142:
#line 981 "yacc.yy"
{
	    yyval.node = new KSParseNode( func_param_out );
	    yyval.node->setIdent( yyvsp[0].ident );
	  ;
    break;}
case 143:
#line 986 "yacc.yy"
{
	    yyval.node = new KSParseNode( func_param_inout );
	    yyval.node->setIdent( yyvsp[0].ident );
	  ;
    break;}
case 144:
#line 994 "yacc.yy"
{
	    yyval.node = new KSParseNode( signal_params, yyvsp[0].node );
	  ;
    break;}
case 145:
#line 998 "yacc.yy"
{
	    yyval.node = new KSParseNode( signal_params, yyvsp[-2].node, yyvsp[0].node );
	  ;
    break;}
case 146:
#line 1005 "yacc.yy"
{
	    yyval.node = new KSParseNode( signal_param );
	    yyval.node->setIdent( yyvsp[0].ident );
	  ;
    break;}
case 147:
#line 1010 "yacc.yy"
{
	    yyval.node = new KSParseNode( signal_param, yyvsp[0].node );
	    yyval.node->setIdent( yyvsp[-2].ident );
	  ;
    break;}
case 148:
#line 1018 "yacc.yy"
{
	    yyval.node = NULL;
	  ;
    break;}
case 149:
#line 1022 "yacc.yy"
{
	    yyval.node = yyvsp[0].node;
	  ;
    break;}
case 150:
#line 1029 "yacc.yy"
{
	    yyval.node = new KSParseNode( func_lines, yyvsp[0].node );
	  ;
    break;}
case 151:
#line 1033 "yacc.yy"
{
	    yyval.node = new KSParseNode( func_lines, yyvsp[-1].node );
	    yyval.node->setBranch( 2, yyvsp[0].node );
	  ;
    break;}
case 152:
#line 1041 "yacc.yy"
{
	    yyval.node = yyvsp[-1].node;
	  ;
    break;}
case 153:
#line 1045 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_raise, yyvsp[-3].node, yyvsp[-1].node );
	  ;
    break;}
case 154:
#line 1049 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_delete, yyvsp[-1].node );
	  ;
    break;}
case 155:
#line 1053 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_emit, yyvsp[-1].node );
	  ;
    break;}
case 156:
#line 1057 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_return );
	  ;
    break;}
case 157:
#line 1061 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_return, yyvsp[-1].node );
	  ;
    break;}
case 158:
#line 1065 "yacc.yy"
{
	    yyval.node = yyvsp[0].node;
	  ;
    break;}
case 159:
#line 1069 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_try, yyvsp[-2].node, yyvsp[0].node );
	  ;
    break;}
case 160:
#line 1073 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_scope, yyvsp[-1].node );
	  ;
    break;}
case 161:
#line 1080 "yacc.yy"
{
	    yyval.node = yyvsp[0].node;
	  ;
    break;}
case 162:
#line 1084 "yacc.yy"
{
	    yyval.node = yyvsp[-1].node;
	    yyval.node->setBranch( 4, yyvsp[0].node );
	  ;
    break;}
case 163:
#line 1093 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_catch, yyvsp[-6].node, yyvsp[-1].node );
	    yyval.node->setIdent( yyvsp[-4].ident );
	  ;
    break;}
case 164:
#line 1099 "yacc.yy"
{
	    KSParseNode* x = new KSParseNode( scoped_name );
	    x->setIdent( yyvsp[-6].ident );
	    yyval.node = new KSParseNode( t_catch_default, x, yyvsp[-1].node );
	    yyval.node->setIdent( yyvsp[-4].ident );
	  ;
    break;}
case 165:
#line 1109 "yacc.yy"
{
	    yyval.node = new KSParseNode( const_dcl, yyvsp[0].node );
	    yyval.node->setIdent( yyvsp[-2].ident );
	  ;
    break;}
case 166:
#line 1117 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_while, yyvsp[-3].node, yyvsp[-1].node );
	  ;
    break;}
case 167:
#line 1121 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_for, yyvsp[-3].node, yyvsp[-1].node );
	  ;
    break;}
case 168:
#line 1125 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_do, yyvsp[-3].node, yyvsp[-1].node );
	  ;
    break;}
case 169:
#line 1129 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_if, yyvsp[-5].node, yyvsp[-2].node, yyvsp[0].node );
	  ;
    break;}
case 170:
#line 1133 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_foreach, yyvsp[-6].node, yyvsp[-4].node, yyvsp[-1].node );
	  ;
    break;}
case 171:
#line 1140 "yacc.yy"
{
	    yyval.node = NULL;
	  ;
    break;}
case 172:
#line 1144 "yacc.yy"
{
	    yyval.node = new KSParseNode( t_else, yyvsp[-5].node, yyvsp[-2].node, yyvsp[0].node );
	  ;
    break;}
case 173:
#line 1151 "yacc.yy"
{
	    yyval.node = yyvsp[-1].node;
	  ;
    break;}
case 174:
#line 1158 "yacc.yy"
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
#line 1162 "yacc.yy"


void kscriptParse( const char *_code )
{
    kscriptInitFlex( _code );
    yyparse();
}
