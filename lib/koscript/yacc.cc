
/*  A Bison parser, made from yacc.yy
 by  GNU Bison version 1.25
  */

#define YYBISON 1  /* Identify Bison output.  */

#define T_MATCH_LINE    258
#define T_LINE  259
#define T_INPUT 260
#define T_AMPERSAND     261
#define T_ASTERISK      262
#define T_CASE  263
#define T_CHARACTER_LITERAL     264
#define T_CIRCUMFLEX    265
#define T_COLON 266
#define T_COMMA 267
#define T_CONST 268
#define T_DEFAULT       269
#define T_ENUM  270
#define T_EQUAL 271
#define T_FALSE 272
#define T_FLOATING_PT_LITERAL   273
#define T_GREATER_THAN_SIGN     274
#define T_IDENTIFIER    275
#define T_IN    276
#define T_INOUT 277
#define T_INTEGER_LITERAL       278
#define T_LEFT_CURLY_BRACKET    279
#define T_LEFT_PARANTHESIS      280
#define T_LEFT_SQUARE_BRACKET   281
#define T_LESS_THAN_SIGN        282
#define T_MINUS_SIGN    283
#define T_OUT   284
#define T_PERCENT_SIGN  285
#define T_PLUS_SIGN     286
#define T_RIGHT_CURLY_BRACKET   287
#define T_RIGHT_PARANTHESIS     288
#define T_RIGHT_SQUARE_BRACKET  289
#define T_SCOPE 290
#define T_SEMICOLON     291
#define T_SHIFTLEFT     292
#define T_SHIFTRIGHT    293
#define T_SOLIDUS       294
#define T_STRING_LITERAL        295
#define T_STRUCT        296
#define T_SWITCH        297
#define T_TILDE 298
#define T_TRUE  299
#define T_VERTICAL_LINE 300
#define T_LESS_OR_EQUAL 301
#define T_GREATER_OR_EQUAL      302
#define T_ASSIGN        303
#define T_NOTEQUAL      304
#define T_MEMBER        305
#define T_WHILE 306
#define T_IF    307
#define T_ELSE  308
#define T_FOR   309
#define T_DO    310
#define T_INCR  311
#define T_DECR  312
#define T_MAIN  313
#define T_FOREACH       314
#define T_SUBST 315
#define T_MATCH 316
#define T_NOT   317
#define T_RETURN        318
#define T_IMPORT        319
#define T_VAR   320
#define T_CATCH 321
#define T_TRY   322
#define T_RAISE 323
#define T_RANGE 324
#define T_CELL  325
#define T_FROM  326
#define T_PLUS_ASSIGN   327
#define T_MINUS_ASSIGN  328
#define T_AND   329
#define T_OR    330
#define T_DOLLAR        331
#define T_UNKNOWN       332

#line 1 "yacc.yy"


#include "koscript_parsenode.h"
#include "koscript_parser.h"
#include <stdlib.h>

#include <qstring.h>

extern int yylex();

extern QString idl_lexFile;
extern int idl_line_no;

void kscriptInitFlex( const char *_code, int extension );

void yyerror( const char *s )
{
  theParser->parse_error( idl_lexFile.local8Bit(), s, idl_line_no );
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



#define YYFINAL         302
#define YYFLAG          -32768
#define YYNTBASE        78

#define YYTRANSLATE(x) ((unsigned)(x) <= 332 ? yytranslate[x] : 121)

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
     0,     1,     4,     6,     8,    11,    14,    17,    20,    24,
    30,    36,    38,    40,    44,    49,    57,    59,    62,    66,
    70,    74,    76,    80,    82,    86,    88,    92,    96,   100,
   104,   108,   112,   114,   116,   120,   122,   126,   128,   132,
   134,   138,   142,   144,   148,   152,   154,   158,   162,   166,
   169,   172,   175,   178,   180,   185,   190,   194,   199,   201,
   205,   207,   211,   213,   216,   219,   222,   225,   227,   230,
   233,   235,   237,   239,   241,   243,   247,   251,   253,   255,
   257,   259,   261,   263,   265,   267,   270,   274,   277,   281,
   283,   286,   290,   292,   300,   306,   312,   314,   317,   320,
   324,   327,   329,   333,   339,   346,   348,   352,   355,   360,
   363,   366,   367,   369,   371,   374,   377,   383,   386,   390,
   392,   398,   402,   404,   407,   417,   427,   432,   435,   445,
   450,   457,   465,   475,   476,   484,   487,   492
};

static const short yyrhs[] = {    -1,
    48,    87,     0,    79,     0,    80,     0,    80,    79,     0,
   116,    36,     0,   104,    36,     0,   108,    32,     0,    64,
    20,    36,     0,    71,    20,    64,     7,    36,     0,    71,
    20,    64,    81,    36,     0,    82,     0,    20,     0,    20,
    12,    81,     0,    58,    24,   111,    32,     0,    58,    25,
   109,    33,    24,   111,    32,     0,    20,     0,    35,    20,
     0,    85,    48,    84,     0,    85,    72,    84,     0,    85,
    73,    84,     0,    85,     0,    86,    75,    85,     0,    86,
     0,    87,    74,    86,     0,    87,     0,    88,    16,    87,
     0,    88,    49,    87,     0,    88,    46,    87,     0,    88,
    47,    87,     0,    88,    27,    87,     0,    88,    19,    87,
     0,    88,     0,    89,     0,    88,    45,    89,     0,    90,
     0,    89,    10,    90,     0,    91,     0,    90,     6,    91,
     0,    92,     0,    91,    38,    92,     0,    91,    37,    92,
     0,    93,     0,    92,    31,    93,     0,    92,    28,    93,
     0,    94,     0,    93,     7,    94,     0,    93,    39,    94,
     0,    93,    30,    94,     0,    28,    95,     0,    31,    95,
     0,    43,    95,     0,    62,    95,     0,    95,     0,    95,
    26,    84,    34,     0,    95,    24,    84,    32,     0,    95,
    25,    33,     0,    95,    25,    96,    33,     0,    97,     0,
    84,    12,    96,     0,    84,     0,    95,    50,    20,     0,
    98,     0,    56,    99,     0,    57,    99,     0,    99,    56,
     0,    99,    57,     0,    99,     0,   100,    61,     0,   100,
    60,     0,   100,     0,    83,     0,   101,     0,     5,     0,
     3,     0,    27,    88,    19,     0,    25,    84,    33,     0,
    23,     0,    70,     0,    69,     0,    40,     0,     9,     0,
    18,     0,    44,     0,    17,     0,    26,    34,     0,    26,
   102,    34,     0,    24,    32,     0,    24,   103,    32,     0,
     4,     0,    76,    23,     0,    84,    12,   102,     0,    84,
     0,    25,    84,    12,    87,    33,    12,   103,     0,    25,
    84,    12,    87,    33,     0,    41,    20,    24,   105,    32,
     0,   106,     0,   106,   105,     0,   108,    32,     0,    65,
   107,    36,     0,   116,    36,     0,    20,     0,    20,    12,
   107,     0,    20,    25,    33,    24,   111,     0,    20,    25,
   109,    33,    24,   111,     0,   110,     0,   110,    12,   109,
     0,    21,    20,     0,    21,    20,    48,    84,     0,    29,
    20,     0,    22,    20,     0,     0,   112,     0,   113,     0,
   113,   112,     0,    84,    36,     0,    68,    84,    12,    84,
    36,     0,    63,    36,     0,    63,    84,    36,     0,   117,
     0,    67,    24,   112,    32,   114,     0,    24,   112,    32,
     0,   115,     0,   115,   114,     0,    66,    25,    84,    12,
    20,    33,    24,   112,    32,     0,    14,    25,    20,    12,
    20,    33,    24,   112,    32,     0,    13,    20,    48,    87,
     0,   119,   120,     0,    54,    25,    84,    36,    84,    36,
    84,    33,   120,     0,    55,   120,   119,    36,     0,    52,
    25,    84,    33,   120,   118,     0,    59,    25,    20,    12,
    84,    33,   120,     0,    59,    25,    20,    12,    20,    12,
    84,    33,   120,     0,     0,    53,    52,    25,    84,    33,
   120,   118,     0,    53,   120,     0,    51,    25,    84,    33,
     0,    24,   111,    32,     0
};

#endif

#if YYDEBUG != 0
static const short yyrline[] = { 0,
   157,   161,   165,   172,   176,   185,   189,   193,   197,   202,
   208,   214,   221,   225,   234,   240,   248,   253,   265,   269,
   273,   277,   284,   288,   295,   299,   306,   310,   314,   318,
   322,   326,   330,   338,   342,   350,   354,   362,   366,   374,
   378,   382,   390,   394,   398,   406,   410,   414,   418,   426,
   430,   434,   438,   442,   449,   453,   457,   461,   465,   472,
   477,   484,   489,   496,   500,   504,   509,   514,   521,   526,
   531,   539,   543,   547,   551,   556,   557,   566,   571,   576,
   581,   586,   591,   596,   601,   606,   610,   614,   618,   622,
   626,   634,   638,   645,   649,   659,   667,   671,   679,   683,
   687,   694,   699,   707,   713,   721,   725,   732,   737,   742,
   747,   755,   759,   766,   770,   778,   782,   786,   790,   794,
   798,   802,   809,   813,   821,   827,   838,   846,   850,   854,
   858,   862,   867,   877,   881,   885,   892,   899
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
    80,    80,    81,    81,    82,    82,    83,    83,    84,    84,
    84,    84,    85,    85,    86,    86,    87,    87,    87,    87,
    87,    87,    87,    88,    88,    89,    89,    90,    90,    91,
    91,    91,    92,    92,    92,    93,    93,    93,    93,    94,
    94,    94,    94,    94,    95,    95,    95,    95,    95,    96,
    96,    97,    97,    98,    98,    98,    98,    98,    99,    99,
    99,   100,   100,   100,   100,   100,   100,   101,   101,   101,
   101,   101,   101,   101,   101,   101,   101,   101,   101,   101,
   101,   102,   102,   103,   103,   104,   105,   105,   106,   106,
   106,   107,   107,   108,   108,   109,   109,   110,   110,   110,
   110,   111,   111,   112,   112,   113,   113,   113,   113,   113,
   113,   113,   114,   114,   115,   115,   116,   117,   117,   117,
   117,   117,   117,   118,   118,   118,   119,   120
};

static const short yyr2[] = {     0,
     0,     2,     1,     1,     2,     2,     2,     2,     3,     5,
     5,     1,     1,     3,     4,     7,     1,     2,     3,     3,
     3,     1,     3,     1,     3,     1,     3,     3,     3,     3,
     3,     3,     1,     1,     3,     1,     3,     1,     3,     1,
     3,     3,     1,     3,     3,     1,     3,     3,     3,     2,
     2,     2,     2,     1,     4,     4,     3,     4,     1,     3,
     1,     3,     1,     2,     2,     2,     2,     1,     2,     2,
     1,     1,     1,     1,     1,     3,     3,     1,     1,     1,
     1,     1,     1,     1,     1,     2,     3,     2,     3,     1,
     2,     3,     1,     7,     5,     5,     1,     2,     2,     3,
     2,     1,     3,     5,     6,     1,     3,     2,     4,     2,
     2,     0,     1,     1,     2,     2,     5,     2,     3,     1,
     5,     3,     1,     2,     9,     9,     4,     2,     9,     4,
     6,     7,     9,     0,     7,     2,     4,     3
};

static const short yydefact[] = {     1,
     0,     0,     0,     0,     0,     0,     0,     3,     4,    12,
     0,     0,     0,     0,     0,     0,    75,    90,    74,    82,
    85,    83,    17,    78,     0,     0,     0,     0,     0,     0,
     0,    81,     0,    84,     0,     0,     0,    80,    79,     0,
    72,     2,    33,    34,    36,    38,    40,    43,    46,    54,
    59,    63,    68,    71,    73,   112,     0,     0,     0,     5,
     7,     8,     6,     0,     0,     0,     0,     0,     0,   106,
     0,     0,    88,     0,     0,    22,    24,    26,    86,    93,
     0,     0,    50,    51,    18,    52,    64,    65,    53,    91,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
    66,    67,    70,    69,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,   113,   114,   120,     0,     0,
     9,     0,   127,   108,   111,   110,   112,     0,     0,     0,
     0,    97,     0,     0,     0,    89,    77,     0,     0,     0,
     0,     0,     0,    87,    76,    27,    32,    31,    35,    29,
    30,    28,    37,    39,    42,    41,    45,    44,    47,    49,
    48,     0,    57,    61,     0,     0,    62,     0,     0,     0,
     0,     0,   112,     0,     0,   118,     0,     0,     0,   116,
    15,   115,   128,     0,     0,    13,     0,     0,   104,   112,
   107,   102,     0,    96,    98,    99,   101,     0,    19,    20,
    21,    23,    25,    92,    56,     0,    58,    55,     0,   122,
     0,     0,     0,     0,     0,     0,   119,     0,     0,   112,
    10,     0,    11,   109,   105,     0,   100,     0,    60,   137,
     0,     0,   138,   130,     0,     0,     0,     0,    14,   103,
    95,   134,     0,    17,     0,     0,     0,   121,   123,   117,
    16,     0,     0,   131,     0,     0,     0,     0,     0,   124,
    94,     0,   136,     0,     0,   132,     0,     0,     0,     0,
     0,     0,     0,     0,   129,   133,     0,     0,     0,     0,
     0,   134,     0,     0,   135,     0,     0,   126,   125,     0,
     0,     0
};

static const short yydefgoto[] = {   300,
     8,     9,   197,    10,    41,   124,    76,    77,    78,    43,
    44,    45,    46,    47,    48,    49,    50,   175,    51,    52,
    53,    54,    55,    81,    74,    11,   141,   142,   203,    12,
    69,    70,   125,   126,   127,   258,   259,    13,   128,   264,
   129,   184
};

static const short yypact[] = {    -1,
    -2,    -5,    17,   525,   101,    35,    48,-32768,    21,-32768,
    55,    11,    84,    36,    88,   113,-32768,-32768,-32768,-32768,
-32768,-32768,-32768,-32768,    33,   525,   360,   525,     4,     4,
   119,-32768,     4,-32768,   144,   144,     4,-32768,-32768,   120,
-32768,-32768,    89,    49,   139,    69,    41,     3,-32768,    28,
-32768,-32768,    57,    68,-32768,   292,    72,   110,    86,-32768,
-32768,-32768,-32768,   525,   131,   140,   143,   135,   132,   161,
    12,   525,-32768,   142,   145,   -22,   100,   102,-32768,   165,
   148,    19,    28,    28,-32768,    28,-32768,-32768,    28,-32768,
   525,   525,   525,   525,   525,   525,   525,   525,   525,   525,
   525,   525,   525,   525,   525,   525,   525,   415,   525,   163,
-32768,-32768,-32768,-32768,   224,   160,   162,   164,   167,   168,
   470,   170,   525,   150,   166,-32768,   292,-32768,   167,   159,
-32768,    16,-32768,   147,-32768,-32768,   292,   172,    72,   177,
   169,    12,   173,   171,   187,-32768,-32768,   525,   525,   525,
   525,   525,   525,-32768,-32768,-32768,-32768,-32768,    49,-32768,
-32768,-32768,   139,    69,    41,    41,     3,     3,-32768,-32768,
-32768,   174,-32768,   188,   175,   176,-32768,   525,   179,   525,
   525,   525,   292,   151,   184,-32768,   181,   292,   197,-32768,
-32768,-32768,-32768,   194,   185,   200,   186,   525,-32768,   292,
-32768,   211,   189,-32768,-32768,-32768,-32768,   525,-32768,-32768,
-32768,-32768,-32768,-32768,-32768,   525,-32768,-32768,    23,-32768,
   191,   193,   195,   198,   196,   222,-32768,   203,   525,   292,
-32768,   216,-32768,-32768,-32768,   177,-32768,   204,-32768,-32768,
   167,   525,-32768,-32768,   580,     1,   202,   213,-32768,-32768,
   234,   201,   221,   246,   227,   236,   237,-32768,     1,-32768,
-32768,   238,    -7,-32768,   525,   525,   167,   245,   525,-32768,
-32768,   241,-32768,   239,   240,-32768,   257,   258,   525,   167,
   167,   251,   254,   244,-32768,-32768,   249,   252,   167,   260,
   264,   201,   292,   292,-32768,   266,   267,-32768,-32768,   289,
   290,-32768
};

static const short yypgoto[] = {-32768,
   293,-32768,    71,-32768,-32768,   -26,   153,   154,     7,   277,
   214,   209,   212,    30,    39,   -16,    82,    98,-32768,-32768,
    97,-32768,-32768,   178,    51,-32768,   180,-32768,    85,   -66,
   -53,-32768,  -134,  -113,-32768,    65,-32768,   -55,-32768,    34,
   141,  -123
};


#define YYLAST          656


static const short yytable[] = {    75,
    80,   179,   199,   130,   143,   193,    17,    18,    19,   104,
    42,     1,    20,   192,   256,   144,   183,    14,     2,    15,
    21,    22,   195,    23,     1,   148,    24,    25,    26,    27,
    28,     2,   105,     1,   208,   196,    16,   155,    31,     3,
     2,   106,    62,    32,   272,   145,     4,    34,   224,   149,
   150,   107,   108,   109,    58,   147,     5,    72,    98,    35,
    36,     3,     6,    94,    73,   235,   257,    59,   102,     7,
   133,   103,    38,    39,   228,   143,   140,   110,     5,    40,
   172,   174,   176,    64,     6,   201,   144,   169,   170,   171,
    61,     7,    65,    66,   187,   248,   189,   156,   157,   158,
    67,   160,   161,   162,    91,   100,   101,    92,    65,    66,
    83,    84,   111,   112,    86,    93,    67,   252,    89,    63,
    68,   209,   210,   211,    56,    57,    80,   113,   114,   165,
   166,    87,    88,    94,    95,    96,    71,    97,    85,   273,
   167,   168,    90,   276,    99,   131,    17,    18,    19,   132,
   134,   219,    20,   221,   222,   223,   285,   286,   137,   135,
    21,    22,   136,    23,   138,   292,    24,    25,    26,    27,
    28,   234,   139,   146,   151,   152,   153,   147,    31,   296,
   297,   154,   177,    32,   180,   190,   181,    34,   182,   174,
   183,   194,   185,   188,   198,   200,   202,   191,   208,   216,
   204,   116,   247,   226,   206,   215,   207,   217,   229,   218,
   220,   232,    38,    39,   238,   253,   227,   230,   255,    40,
   231,   233,   236,   240,   237,   241,    17,    18,    19,   243,
   242,   244,    20,   245,   246,   196,   251,   260,   274,   275,
    21,    22,   278,    23,   261,   262,    24,   115,   178,    27,
    28,    29,   284,   263,    30,    73,   265,   266,    31,   267,
   268,   269,    72,    32,   277,   279,    33,    34,   282,   283,
   287,   280,   281,   288,   116,   117,   289,   118,   119,    35,
    36,   290,   120,   293,   291,    37,   121,   294,   301,   302,
   122,   123,    38,    39,    17,    18,    19,   298,   299,    40,
    20,    60,   249,   212,    82,   213,   163,   159,    21,    22,
   164,    23,   271,   239,    24,   115,    26,    27,    28,    29,
   250,   205,    30,   270,   225,   295,    31,     0,     0,     0,
   214,    32,     0,     0,    33,    34,     0,     0,     0,     0,
     0,     0,   116,   117,     0,   118,   119,    35,    36,     0,
   120,     0,     0,    37,   121,     0,     0,     0,   122,   123,
    38,    39,    17,    18,    19,     0,     0,    40,    20,     0,
     0,     0,     0,     0,     0,     0,    21,    22,     0,    23,
     0,     0,    24,    25,    26,    27,    28,    29,     0,     0,
    30,     0,     0,    79,    31,     0,     0,     0,     0,    32,
     0,     0,    33,    34,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,    35,    36,    17,    18,    19,
     0,    37,     0,    20,     0,     0,     0,     0,    38,    39,
     0,    21,    22,     0,    23,    40,     0,    24,    25,    26,
    27,    28,    29,     0,     0,    30,     0,   173,     0,    31,
     0,     0,     0,     0,    32,     0,     0,    33,    34,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
    35,    36,    17,    18,    19,     0,    37,     0,    20,     0,
     0,     0,     0,    38,    39,     0,    21,    22,     0,    23,
    40,     0,    24,    25,    26,    27,    28,    29,     0,     0,
    30,     0,     0,     0,    31,   186,     0,     0,     0,    32,
     0,     0,    33,    34,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,    35,    36,    17,    18,    19,
     0,    37,     0,    20,     0,     0,     0,     0,    38,    39,
     0,    21,    22,     0,    23,    40,     0,    24,    25,    26,
    27,    28,    29,     0,     0,    30,     0,     0,     0,    31,
     0,     0,     0,     0,    32,     0,     0,    33,    34,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
    35,    36,    17,    18,    19,     0,    37,     0,    20,     0,
     0,     0,     0,    38,    39,     0,    21,    22,     0,   254,
    40,     0,    24,    25,    26,    27,    28,    29,     0,     0,
    30,     0,     0,     0,    31,     0,     0,     0,     0,    32,
     0,     0,    33,    34,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,    35,    36,     0,     0,     0,
     0,    37,     0,     0,     0,     0,     0,     0,    38,    39,
     0,     0,     0,     0,     0,    40
};

static const short yycheck[] = {    26,
    27,   115,   137,    57,    71,   129,     3,     4,     5,     7,
     4,    13,     9,   127,    14,    71,    24,    20,    20,    25,
    17,    18,     7,    20,    13,    48,    23,    24,    25,    26,
    27,    20,    30,    13,    12,    20,    20,    19,    35,    41,
    20,    39,    32,    40,    52,    72,    48,    44,   183,    72,
    73,    24,    25,    26,    20,    33,    58,    25,    10,    56,
    57,    41,    64,    45,    32,   200,    66,    20,    28,    71,
    64,    31,    69,    70,   188,   142,    65,    50,    58,    76,
   107,   108,   109,    48,    64,   139,   142,   104,   105,   106,
    36,    71,    21,    22,   121,   230,   123,    91,    92,    93,
    29,    95,    96,    97,    16,    37,    38,    19,    21,    22,
    29,    30,    56,    57,    33,    27,    29,   241,    37,    36,
    33,   148,   149,   150,    24,    25,   153,    60,    61,   100,
   101,    35,    36,    45,    46,    47,    24,    49,    20,   263,
   102,   103,    23,   267,     6,    36,     3,     4,     5,    64,
    20,   178,     9,   180,   181,   182,   280,   281,    24,    20,
    17,    18,    20,    20,    33,   289,    23,    24,    25,    26,
    27,   198,    12,    32,    75,    74,    12,    33,    35,   293,
   294,    34,    20,    40,    25,    36,    25,    44,    25,   216,
    24,    33,    25,    24,    48,    24,    20,    32,    12,    12,
    32,    51,   229,    20,    32,    32,    36,    33,    12,    34,
    32,    12,    69,    70,   208,   242,    36,    24,   245,    76,
    36,    36,    12,    33,    36,    33,     3,     4,     5,    32,
    36,    36,     9,    12,    32,    20,    33,    36,   265,   266,
    17,    18,   269,    20,    32,    12,    23,    24,    25,    26,
    27,    28,   279,    53,    31,    32,    36,    12,    35,    33,
    25,    25,    25,    40,    20,    25,    43,    44,    12,    12,
    20,    33,    33,    20,    51,    52,    33,    54,    55,    56,
    57,    33,    59,    24,    33,    62,    63,    24,     0,     0,
    67,    68,    69,    70,     3,     4,     5,    32,    32,    76,
     9,     9,   232,   151,    28,   152,    98,    94,    17,    18,
    99,    20,   262,   216,    23,    24,    25,    26,    27,    28,
   236,   142,    31,   259,   184,   292,    35,    -1,    -1,    -1,
   153,    40,    -1,    -1,    43,    44,    -1,    -1,    -1,    -1,
    -1,    -1,    51,    52,    -1,    54,    55,    56,    57,    -1,
    59,    -1,    -1,    62,    63,    -1,    -1,    -1,    67,    68,
    69,    70,     3,     4,     5,    -1,    -1,    76,     9,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    17,    18,    -1,    20,
    -1,    -1,    23,    24,    25,    26,    27,    28,    -1,    -1,
    31,    -1,    -1,    34,    35,    -1,    -1,    -1,    -1,    40,
    -1,    -1,    43,    44,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    56,    57,     3,     4,     5,
    -1,    62,    -1,     9,    -1,    -1,    -1,    -1,    69,    70,
    -1,    17,    18,    -1,    20,    76,    -1,    23,    24,    25,
    26,    27,    28,    -1,    -1,    31,    -1,    33,    -1,    35,
    -1,    -1,    -1,    -1,    40,    -1,    -1,    43,    44,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    56,    57,     3,     4,     5,    -1,    62,    -1,     9,    -1,
    -1,    -1,    -1,    69,    70,    -1,    17,    18,    -1,    20,
    76,    -1,    23,    24,    25,    26,    27,    28,    -1,    -1,
    31,    -1,    -1,    -1,    35,    36,    -1,    -1,    -1,    40,
    -1,    -1,    43,    44,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    56,    57,     3,     4,     5,
    -1,    62,    -1,     9,    -1,    -1,    -1,    -1,    69,    70,
    -1,    17,    18,    -1,    20,    76,    -1,    23,    24,    25,
    26,    27,    28,    -1,    -1,    31,    -1,    -1,    -1,    35,
    -1,    -1,    -1,    -1,    40,    -1,    -1,    43,    44,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    56,    57,     3,     4,     5,    -1,    62,    -1,     9,    -1,
    -1,    -1,    -1,    69,    70,    -1,    17,    18,    -1,    20,
    76,    -1,    23,    24,    25,    26,    27,    28,    -1,    -1,
    31,    -1,    -1,    -1,    35,    -1,    -1,    -1,    -1,    40,
    -1,    -1,    43,    44,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    56,    57,    -1,    -1,    -1,
    -1,    62,    -1,    -1,    -1,    -1,    -1,    -1,    69,    70,
    -1,    -1,    -1,    -1,    -1,    76
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

#define yyerrok         (yyerrstatus = 0)
#define yyclearin       (yychar = YYEMPTY)
#define YYEMPTY         -2
#define YYEOF           0
#define YYACCEPT        return(0)
#define YYABORT         return(1)
#define YYERROR         goto yyerrlab1
/* Like YYERROR except do call yyerror.
   This remains here temporarily to ease the
   transition to the new meaning of YYERROR, for GCC.
   Once GCC version 2 has supplanted version 1, this can go.  */
#define YYFAIL          goto yyerrlab
#define YYRECOVERING()  (!!yyerrstatus)
#define YYBACKUP(token, value) \
do                                                              \
  if (yychar == YYEMPTY && yylen == 1)                          \
    { yychar = (token), yylval = (value);                       \
      yychar1 = YYTRANSLATE (yychar);                           \
      YYPOPSTACK;                                               \
      goto yybackup;                                            \
    }                                                           \
  else                                                          \
    { yyerror ("syntax error: cannot back up"); YYERROR; }      \
while (0)

#define YYTERROR        1
#define YYERRCODE       256

#ifndef YYPURE
#define YYLEX           yylex()
#endif

#ifdef YYPURE
#ifdef YYLSP_NEEDED
#ifdef YYLEX_PARAM
#define YYLEX           yylex(&yylval, &yylloc, YYLEX_PARAM)
#else
#define YYLEX           yylex(&yylval, &yylloc)
#endif
#else /* not YYLSP_NEEDED */
#ifdef YYLEX_PARAM
#define YYLEX           yylex(&yylval, YYLEX_PARAM)
#else
#define YYLEX           yylex(&yylval)
#endif
#endif /* not YYLSP_NEEDED */
#endif

/* If nonreentrant, generate the variables here */

#ifndef YYPURE

int     yychar;                 /*  the lookahead symbol                */
YYSTYPE yylval;                 /*  the semantic value of the           */
                                /*  lookahead symbol                    */

#ifdef YYLSP_NEEDED
YYLTYPE yylloc;                 /*  location data for the lookahead     */
                                /*  symbol                              */
#endif

int yynerrs;                    /*  number of parse errors so far       */
#endif  /* not YYPURE */

#if YYDEBUG != 0
int yydebug;                    /*  nonzero means print parse trace     */
/* Since this is uninitialized, it does not stop multiple parsers
   from coexisting.  */
#endif

/*  YYINITDEPTH indicates the initial size of the parser's stacks       */

#ifndef YYINITDEPTH
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

#if __GNUC__ > 1                /* GNU C and GNU C++ define this.  */
#define __yy_memcpy(TO,FROM,COUNT)      __builtin_memcpy(TO,FROM,COUNT)
#else                           /* not GNU C or C++ */
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
  int yyerrstatus;      /*  number of tokens to shift before error messages enabled */
  int yychar1 = 0;              /*  lookahead token as an internal (translated) token number */

  short yyssa[YYINITDEPTH];     /*  the state stack                     */
  YYSTYPE yyvsa[YYINITDEPTH];   /*  the semantic value stack            */

  short *yyss = yyssa;          /*  refer to the stacks thru separate pointers */
  YYSTYPE *yyvs = yyvsa;        /*  to allow yyoverflow to reallocate them elsewhere */

#ifdef YYLSP_NEEDED
  YYLTYPE yylsa[YYINITDEPTH];   /*  the location stack                  */
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

  YYSTYPE yyval;                /*  the variable used to return         */
                                /*  semantic values from the action     */
                                /*  routines                            */

  int yylen;

#if YYDEBUG != 0
  if (yydebug)
    fprintf(stderr, "Starting parse\n");
#endif

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY;             /* Cause a token to be read.  */

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

  if (yychar <= 0)              /* This means end of input. */
    {
      yychar1 = 0;
      yychar = YYEOF;           /* Don't call YYLEX any more */

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
#line 158 "yacc.yy"
{
            theParser->setRootNode( NULL );
          ;
    break;}
case 2:
#line 162 "yacc.yy"
{
            theParser->setRootNode( yyvsp[0].node );
          ;
    break;}
case 3:
#line 166 "yacc.yy"
{
            theParser->setRootNode( new KSParseNode( definitions, yyvsp[0].node ) );
          ;
    break;}
case 4:
#line 173 "yacc.yy"
{
            yyval.node = new KSParseNode( definitions, yyvsp[0].node );
          ;
    break;}
case 5:
#line 177 "yacc.yy"
{
            yyval.node = new KSParseNode( definitions, yyvsp[-1].node );
            yyval.node->setBranch( 2, yyvsp[0].node );
          ;
    break;}
case 6:
#line 186 "yacc.yy"
{
            yyval.node = yyvsp[-1].node;
          ;
    break;}
case 7:
#line 190 "yacc.yy"
{
            yyval.node = yyvsp[-1].node;
          ;
    break;}
case 8:
#line 194 "yacc.yy"
{
            yyval.node = yyvsp[-1].node;
          ;
    break;}
case 9:
#line 198 "yacc.yy"
{
            yyval.node = new KSParseNode( import );
            yyval.node->setIdent( yyvsp[-1].ident );
          ;
    break;}
case 10:
#line 203 "yacc.yy"
{
            yyval.node = new KSParseNode( from );
            yyval.node->setIdent( yyvsp[-3].ident );
            yyval.node->setStringLiteral( QString( "" ) );
          ;
    break;}
case 11:
#line 209 "yacc.yy"
{
            yyval.node = new KSParseNode( from );
            yyval.node->setIdent( yyvsp[-3].ident );
            yyval.node->setStringLiteral( yyvsp[-1]._str );
          ;
    break;}
case 12:
#line 215 "yacc.yy"
{
            yyval.node = yyvsp[0].node;
          ;
    break;}
case 13:
#line 222 "yacc.yy"
{
                yyval._str = yyvsp[0].ident;
          ;
    break;}
case 14:
#line 226 "yacc.yy"
{
                (*yyvsp[-2].ident) += "/";
                (*yyvsp[-2].ident) += (*yyvsp[0]._str);
                yyval._str = yyvsp[-2].ident;
          ;
    break;}
case 15:
#line 235 "yacc.yy"
{
            yyval.node = new KSParseNode( func_dcl );
            yyval.node->setBranch( 2, yyvsp[-1].node );
            yyval.node->setIdent( "main" );
          ;
    break;}
case 16:
#line 241 "yacc.yy"
{
            yyval.node = new KSParseNode( func_dcl, yyvsp[-4].node, yyvsp[-1].node );
            yyval.node->setIdent( "main" );
          ;
    break;}
case 17:
#line 249 "yacc.yy"
{
            yyval.node = new KSParseNode( scoped_name );
            yyval.node->setIdent( yyvsp[0].ident );
          ;
    break;}
case 18:
#line 254 "yacc.yy"
{
            yyval.node = new KSParseNode( scoped_name );
            QString name = "::";
            name += *(yyvsp[0].ident);
            delete yyvsp[0].ident;
            yyval.node->setIdent( name );
          ;
    break;}
case 19:
#line 266 "yacc.yy"
{
            yyval.node = new KSParseNode( assign_expr, yyvsp[-2].node, yyvsp[0].node );
          ;
    break;}
case 20:
#line 270 "yacc.yy"
{
            yyval.node = new KSParseNode( plus_assign, yyvsp[-2].node, yyvsp[0].node );
          ;
    break;}
case 21:
#line 274 "yacc.yy"
{
            yyval.node = new KSParseNode( minus_assign, yyvsp[-2].node, yyvsp[0].node );
          ;
    break;}
case 22:
#line 278 "yacc.yy"
{
            yyval.node = yyvsp[0].node;
          ;
    break;}
case 23:
#line 285 "yacc.yy"
{
            yyval.node = new KSParseNode( bool_or, yyvsp[-2].node, yyvsp[0].node );
          ;
    break;}
case 24:
#line 289 "yacc.yy"
{
            yyval.node = yyvsp[0].node;
          ;
    break;}
case 25:
#line 296 "yacc.yy"
{
            yyval.node = new KSParseNode( bool_and, yyvsp[-2].node, yyvsp[0].node );
          ;
    break;}
case 26:
#line 300 "yacc.yy"
{
            yyval.node = yyvsp[0].node;
          ;
    break;}
case 27:
#line 307 "yacc.yy"
{
            yyval.node = new KSParseNode( t_equal, yyvsp[-2].node, yyvsp[0].node );
          ;
    break;}
case 28:
#line 311 "yacc.yy"
{
            yyval.node = new KSParseNode( t_notequal, yyvsp[-2].node, yyvsp[0].node );
          ;
    break;}
case 29:
#line 315 "yacc.yy"
{
            yyval.node = new KSParseNode( t_less_or_equal, yyvsp[-2].node, yyvsp[0].node );
          ;
    break;}
case 30:
#line 319 "yacc.yy"
{
            yyval.node = new KSParseNode( t_greater_or_equal, yyvsp[-2].node, yyvsp[0].node );
          ;
    break;}
case 31:
#line 323 "yacc.yy"
{
            yyval.node = new KSParseNode( t_less, yyvsp[-2].node, yyvsp[0].node );
          ;
    break;}
case 32:
#line 327 "yacc.yy"
{
            yyval.node = new KSParseNode( t_greater, yyvsp[-2].node, yyvsp[0].node );
          ;
    break;}
case 33:
#line 331 "yacc.yy"
{
            yyval.node = yyvsp[0].node;
          ;
    break;}
case 34:
#line 339 "yacc.yy"
{
            yyval.node = yyvsp[0].node;
          ;
    break;}
case 35:
#line 343 "yacc.yy"
{
            yyval.node = new KSParseNode( t_vertical_line, yyvsp[-2].node, yyvsp[0].node );
          ;
    break;}
case 36:
#line 351 "yacc.yy"
{
            yyval.node = yyvsp[0].node;
          ;
    break;}
case 37:
#line 355 "yacc.yy"
{
            yyval.node = new KSParseNode( t_circumflex, yyvsp[-2].node, yyvsp[0].node );
          ;
    break;}
case 38:
#line 363 "yacc.yy"
{
            yyval.node = yyvsp[0].node;
          ;
    break;}
case 39:
#line 367 "yacc.yy"
{
            yyval.node = new KSParseNode( t_ampersand, yyvsp[-2].node, yyvsp[0].node );
          ;
    break;}
case 40:
#line 375 "yacc.yy"
{
            yyval.node = yyvsp[0].node;
          ;
    break;}
case 41:
#line 379 "yacc.yy"
{
            yyval.node = new KSParseNode( t_shiftright, yyvsp[-2].node, yyvsp[0].node );
          ;
    break;}
case 42:
#line 383 "yacc.yy"
{
            yyval.node = new KSParseNode( t_shiftleft, yyvsp[-2].node, yyvsp[0].node );
          ;
    break;}
case 43:
#line 391 "yacc.yy"
{
            yyval.node = yyvsp[0].node;
          ;
    break;}
case 44:
#line 395 "yacc.yy"
{
            yyval.node = new KSParseNode( t_plus_sign, yyvsp[-2].node, yyvsp[0].node );
          ;
    break;}
case 45:
#line 399 "yacc.yy"
{
            yyval.node = new KSParseNode( t_minus_sign, yyvsp[-2].node, yyvsp[0].node );
          ;
    break;}
case 46:
#line 407 "yacc.yy"
{
            yyval.node = yyvsp[0].node;
          ;
    break;}
case 47:
#line 411 "yacc.yy"
{
            yyval.node = new KSParseNode( t_asterik, yyvsp[-2].node, yyvsp[0].node );
          ;
    break;}
case 48:
#line 415 "yacc.yy"
{
            yyval.node = new KSParseNode( t_solidus, yyvsp[-2].node, yyvsp[0].node );
          ;
    break;}
case 49:
#line 419 "yacc.yy"
{
            yyval.node = new KSParseNode( t_percent_sign, yyvsp[-2].node, yyvsp[0].node );
          ;
    break;}
case 50:
#line 427 "yacc.yy"
{
            yyval.node = new KSParseNode( t_minus_sign, yyvsp[0].node );
          ;
    break;}
case 51:
#line 431 "yacc.yy"
{
            yyval.node = new KSParseNode( t_plus_sign, yyvsp[0].node );
          ;
    break;}
case 52:
#line 435 "yacc.yy"
{
            yyval.node = new KSParseNode( t_tilde, yyvsp[0].node );
          ;
    break;}
case 53:
#line 439 "yacc.yy"
{
            yyval.node = new KSParseNode( t_not, yyvsp[0].node );
          ;
    break;}
case 54:
#line 443 "yacc.yy"
{
            yyval.node = yyvsp[0].node;
          ;
    break;}
case 55:
#line 450 "yacc.yy"
{
            yyval.node = new KSParseNode( t_array, yyvsp[-3].node, yyvsp[-1].node );
          ;
    break;}
case 56:
#line 454 "yacc.yy"
{
            yyval.node = new KSParseNode( t_dict, yyvsp[-3].node, yyvsp[-1].node );
          ;
    break;}
case 57:
#line 458 "yacc.yy"
{
            yyval.node = new KSParseNode( t_func_call, yyvsp[-2].node );
          ;
    break;}
case 58:
#line 462 "yacc.yy"
{
            yyval.node = new KSParseNode( t_func_call, yyvsp[-3].node, yyvsp[-1].node );
          ;
    break;}
case 59:
#line 466 "yacc.yy"
{
            yyval.node = yyvsp[0].node;
          ;
    break;}
case 60:
#line 473 "yacc.yy"
{
            yyval.node = new KSParseNode( func_call_params, yyvsp[-2].node, yyvsp[0].node );
          ;
    break;}
case 61:
#line 478 "yacc.yy"
{
            yyval.node = new KSParseNode( func_call_params, yyvsp[0].node );
          ;
    break;}
case 62:
#line 485 "yacc.yy"
{
            yyval.node = new KSParseNode( member_expr, yyvsp[-2].node );
            yyval.node->setIdent( yyvsp[0].ident );
          ;
    break;}
case 63:
#line 490 "yacc.yy"
{
            yyval.node = yyvsp[0].node;
          ;
    break;}
case 64:
#line 497 "yacc.yy"
{
            yyval.node = new KSParseNode( t_incr, yyvsp[0].node );
          ;
    break;}
case 65:
#line 501 "yacc.yy"
{
            yyval.node = new KSParseNode( t_decr, yyvsp[0].node );
          ;
    break;}
case 66:
#line 505 "yacc.yy"
{
            /* Setting $1 twice indicates that this is a postfix operator */
            yyval.node = new KSParseNode( t_incr, yyvsp[-1].node, yyvsp[-1].node );
          ;
    break;}
case 67:
#line 510 "yacc.yy"
{
            /* Setting $1 twice indicates that this is a postfix operator */
            yyval.node = new KSParseNode( t_decr, yyvsp[-1].node, yyvsp[-1].node );
          ;
    break;}
case 68:
#line 515 "yacc.yy"
{
            yyval.node = yyvsp[0].node;
          ;
    break;}
case 69:
#line 522 "yacc.yy"
{
            yyval.node = new KSParseNode( t_match, yyvsp[-1].node );
            yyval.node->setIdent( yyvsp[0]._str );
          ;
    break;}
case 70:
#line 527 "yacc.yy"
{
            yyval.node = new KSParseNode( t_subst, yyvsp[-1].node );
            yyval.node->setIdent( yyvsp[0].ident );
          ;
    break;}
case 71:
#line 532 "yacc.yy"
{
            yyval.node = yyvsp[0].node;
          ;
    break;}
case 72:
#line 540 "yacc.yy"
{
            yyval.node = yyvsp[0].node;
          ;
    break;}
case 73:
#line 544 "yacc.yy"
{
            yyval.node = yyvsp[0].node;
          ;
    break;}
case 74:
#line 548 "yacc.yy"
{
            yyval.node = new KSParseNode( t_input );
          ;
    break;}
case 75:
#line 552 "yacc.yy"
{
            yyval.node = new KSParseNode( t_match_line );
            yyval.node->setIdent( yyvsp[0]._str );
          ;
    break;}
case 76:
#line 556 "yacc.yy"
{ ;
    break;}
case 77:
#line 558 "yacc.yy"
{
            yyval.node = yyvsp[-1].node;
          ;
    break;}
case 78:
#line 567 "yacc.yy"
{
            yyval.node = new KSParseNode( t_integer_literal );
            yyval.node->setIntegerLiteral( yyvsp[0]._int );
          ;
    break;}
case 79:
#line 572 "yacc.yy"
{
            yyval.node = new KSParseNode( t_cell );
            yyval.node->setStringLiteral( yyvsp[0]._str );
          ;
    break;}
case 80:
#line 577 "yacc.yy"
{
            yyval.node = new KSParseNode( t_range );
            yyval.node->setStringLiteral( yyvsp[0]._str );
          ;
    break;}
case 81:
#line 582 "yacc.yy"
{
            yyval.node = new KSParseNode( t_string_literal );
            yyval.node->setStringLiteral( yyvsp[0]._str );
          ;
    break;}
case 82:
#line 587 "yacc.yy"
{
            yyval.node = new KSParseNode( t_character_literal );
            yyval.node->setCharacterLiteral( yyvsp[0]._char );
          ;
    break;}
case 83:
#line 592 "yacc.yy"
{
            yyval.node = new KSParseNode( t_floating_pt_literal );
            yyval.node->setFloatingPtLiteral( yyvsp[0]._float );
          ;
    break;}
case 84:
#line 597 "yacc.yy"
{
            yyval.node = new KSParseNode( t_boolean_literal );
            yyval.node->setBooleanLiteral( true );
          ;
    break;}
case 85:
#line 602 "yacc.yy"
{
            yyval.node = new KSParseNode( t_boolean_literal );
            yyval.node->setBooleanLiteral( false );
          ;
    break;}
case 86:
#line 607 "yacc.yy"
{
            yyval.node = new KSParseNode( t_array_const );
          ;
    break;}
case 87:
#line 611 "yacc.yy"
{
            yyval.node = new KSParseNode( t_array_const, yyvsp[-1].node );
          ;
    break;}
case 88:
#line 615 "yacc.yy"
{
            yyval.node = new KSParseNode( t_dict_const );
          ;
    break;}
case 89:
#line 619 "yacc.yy"
{
            yyval.node = new KSParseNode( t_dict_const, yyvsp[-1].node );
          ;
    break;}
case 90:
#line 623 "yacc.yy"
{
            yyval.node = new KSParseNode( t_line );
          ;
    break;}
case 91:
#line 627 "yacc.yy"
{
            yyval.node = new KSParseNode( t_regexp_group );
            yyval.node->setIntegerLiteral( yyvsp[0]._int );
          ;
    break;}
case 92:
#line 635 "yacc.yy"
{
            yyval.node = new KSParseNode( t_array_element, yyvsp[-2].node, yyvsp[0].node );
          ;
    break;}
case 93:
#line 639 "yacc.yy"
{
            yyval.node = new KSParseNode( t_array_element, yyvsp[0].node  );
          ;
    break;}
case 94:
#line 646 "yacc.yy"
{
            yyval.node = new KSParseNode( t_dict_element, yyvsp[-5].node, yyvsp[-3].node, yyvsp[0].node );
          ;
    break;}
case 95:
#line 650 "yacc.yy"
{
            yyval.node = new KSParseNode( t_dict_element, yyvsp[-3].node, yyvsp[-1].node );
          ;
    break;}
case 96:
#line 660 "yacc.yy"
{
            yyval.node = new KSParseNode( t_struct, yyvsp[-1].node );
            yyval.node->setIdent( yyvsp[-3].ident );
          ;
    break;}
case 97:
#line 668 "yacc.yy"
{
            yyval.node = new KSParseNode( exports, yyvsp[0].node );
          ;
    break;}
case 98:
#line 672 "yacc.yy"
{
            yyval.node = new KSParseNode( exports, yyvsp[-1].node );
            yyval.node->setBranch( 2, yyvsp[0].node );
          ;
    break;}
case 99:
#line 680 "yacc.yy"
{
            yyval.node = yyvsp[-1].node;
          ;
    break;}
case 100:
#line 684 "yacc.yy"
{
            yyval.node = yyvsp[-1].node;
          ;
    break;}
case 101:
#line 688 "yacc.yy"
{
            yyval.node = yyvsp[-1].node;
          ;
    break;}
case 102:
#line 695 "yacc.yy"
{
            yyval.node = new KSParseNode( t_struct_members );
            yyval.node->setIdent( yyvsp[0].ident );
          ;
    break;}
case 103:
#line 700 "yacc.yy"
{
            yyval.node = new KSParseNode( t_struct_members, yyvsp[0].node );
            yyval.node->setIdent( yyvsp[-2].ident );
          ;
    break;}
case 104:
#line 708 "yacc.yy"
{
            yyval.node = new KSParseNode( func_dcl );
            yyval.node->setBranch( 2, yyvsp[0].node );
            yyval.node->setIdent( yyvsp[-4].ident );
          ;
    break;}
case 105:
#line 714 "yacc.yy"
{
            yyval.node = new KSParseNode( func_dcl, yyvsp[-3].node, yyvsp[0].node );
            yyval.node->setIdent( yyvsp[-5].ident );
          ;
    break;}
case 106:
#line 722 "yacc.yy"
{
            yyval.node = new KSParseNode( func_params, yyvsp[0].node );
          ;
    break;}
case 107:
#line 726 "yacc.yy"
{
            yyval.node = new KSParseNode( func_params, yyvsp[-2].node, yyvsp[0].node );
          ;
    break;}
case 108:
#line 733 "yacc.yy"
{
            yyval.node = new KSParseNode( func_param_in );
            yyval.node->setIdent( yyvsp[0].ident );
          ;
    break;}
case 109:
#line 738 "yacc.yy"
{
            yyval.node = new KSParseNode( func_param_in, yyvsp[0].node );
            yyval.node->setIdent( yyvsp[-2].ident );
          ;
    break;}
case 110:
#line 743 "yacc.yy"
{
            yyval.node = new KSParseNode( func_param_out );
            yyval.node->setIdent( yyvsp[0].ident );
          ;
    break;}
case 111:
#line 748 "yacc.yy"
{
            yyval.node = new KSParseNode( func_param_inout );
            yyval.node->setIdent( yyvsp[0].ident );
          ;
    break;}
case 112:
#line 756 "yacc.yy"
{
            yyval.node = NULL;
          ;
    break;}
case 113:
#line 760 "yacc.yy"
{
            yyval.node = yyvsp[0].node;
          ;
    break;}
case 114:
#line 767 "yacc.yy"
{
            yyval.node = new KSParseNode( func_lines, yyvsp[0].node );
          ;
    break;}
case 115:
#line 771 "yacc.yy"
{
            yyval.node = new KSParseNode( func_lines, yyvsp[-1].node );
            yyval.node->setBranch( 2, yyvsp[0].node );
          ;
    break;}
case 116:
#line 779 "yacc.yy"
{
            yyval.node = yyvsp[-1].node;
          ;
    break;}
case 117:
#line 783 "yacc.yy"
{
            yyval.node = new KSParseNode( t_raise, yyvsp[-3].node, yyvsp[-1].node );
          ;
    break;}
case 118:
#line 787 "yacc.yy"
{
            yyval.node = new KSParseNode( t_return );
          ;
    break;}
case 119:
#line 791 "yacc.yy"
{
            yyval.node = new KSParseNode( t_return, yyvsp[-1].node );
          ;
    break;}
case 120:
#line 795 "yacc.yy"
{
            yyval.node = yyvsp[0].node;
          ;
    break;}
case 121:
#line 799 "yacc.yy"
{
            yyval.node = new KSParseNode( t_try, yyvsp[-2].node, yyvsp[0].node );
          ;
    break;}
case 122:
#line 803 "yacc.yy"
{
            yyval.node = new KSParseNode( t_scope, yyvsp[-1].node );
          ;
    break;}
case 123:
#line 810 "yacc.yy"
{
            yyval.node = yyvsp[0].node;
          ;
    break;}
case 124:
#line 814 "yacc.yy"
{
            yyval.node = yyvsp[-1].node;
            yyval.node->setBranch( 4, yyvsp[0].node );
          ;
    break;}
case 125:
#line 823 "yacc.yy"
{
            yyval.node = new KSParseNode( t_catch, yyvsp[-6].node, yyvsp[-1].node );
            yyval.node->setIdent( yyvsp[-4].ident );
          ;
    break;}
case 126:
#line 829 "yacc.yy"
{
            KSParseNode* x = new KSParseNode( scoped_name );
            x->setIdent( yyvsp[-6].ident );
            yyval.node = new KSParseNode( t_catch_default, x, yyvsp[-1].node );
            yyval.node->setIdent( yyvsp[-4].ident );
          ;
    break;}
case 127:
#line 839 "yacc.yy"
{
            yyval.node = new KSParseNode( const_dcl, yyvsp[0].node );
            yyval.node->setIdent( yyvsp[-2].ident );
          ;
    break;}
case 128:
#line 847 "yacc.yy"
{
            yyval.node = new KSParseNode( t_while, yyvsp[-1].node, yyvsp[0].node );
          ;
    break;}
case 129:
#line 851 "yacc.yy"
{
            yyval.node = new KSParseNode( t_for, yyvsp[-6].node, yyvsp[-4].node, yyvsp[-2].node, yyvsp[0].node );
          ;
    break;}
case 130:
#line 855 "yacc.yy"
{
            yyval.node = new KSParseNode( t_do, yyvsp[-2].node, yyvsp[-1].node );
          ;
    break;}
case 131:
#line 859 "yacc.yy"
{
            yyval.node = new KSParseNode( t_if, yyvsp[-3].node, yyvsp[-1].node, yyvsp[0].node );
          ;
    break;}
case 132:
#line 863 "yacc.yy"
{
            yyval.node = new KSParseNode( t_foreach, yyvsp[-2].node, yyvsp[0].node );
            yyval.node->setIdent( yyvsp[-4].ident );
          ;
    break;}
case 133:
#line 868 "yacc.yy"
{
            /* We set $9 twice to indicate thet this is the foreach for maps */
            yyval.node = new KSParseNode( t_foreach, yyvsp[-2].node, yyvsp[0].node, yyvsp[0].node );
            yyval.node->setIdent( yyvsp[-6].ident );
            yyval.node->setStringLiteral( yyvsp[-4].ident );
          ;
    break;}
case 134:
#line 878 "yacc.yy"
{
            yyval.node = NULL;
          ;
    break;}
case 135:
#line 882 "yacc.yy"
{
            yyval.node = new KSParseNode( t_if, yyvsp[-3].node, yyvsp[-1].node, yyvsp[0].node );
          ;
    break;}
case 136:
#line 886 "yacc.yy"
{
            yyval.node = yyvsp[0].node;
          ;
    break;}
case 137:
#line 893 "yacc.yy"
{
            yyval.node = yyvsp[-1].node;
          ;
    break;}
case 138:
#line 900 "yacc.yy"
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

  yyerrstatus = 3;              /* Each real token shifted decrements this */

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
#line 904 "yacc.yy"


void kscriptParse( const char *_code, int extension )
{
    kscriptInitFlex( _code, extension );
    yyparse();
}
