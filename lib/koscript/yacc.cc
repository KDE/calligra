/* A Bison parser, made from yacc.yy
   by GNU bison 1.35.  */

#define YYBISON 1  /* Identify Bison output.  */

# define	T_MATCH_LINE	257
# define	T_LINE	258
# define	T_INPUT	259
# define	T_AMPERSAND	260
# define	T_ASTERISK	261
# define	T_CASE	262
# define	T_CHARACTER_LITERAL	263
# define	T_CIRCUMFLEX	264
# define	T_COLON	265
# define	T_COMMA	266
# define	T_CONST	267
# define	T_DEFAULT	268
# define	T_ENUM	269
# define	T_EQUAL	270
# define	T_FALSE	271
# define	T_FLOATING_PT_LITERAL	272
# define	T_GREATER_THAN_SIGN	273
# define	T_IDENTIFIER	274
# define	T_IN	275
# define	T_INOUT	276
# define	T_INTEGER_LITERAL	277
# define	T_LEFT_CURLY_BRACKET	278
# define	T_LEFT_PARANTHESIS	279
# define	T_LEFT_SQUARE_BRACKET	280
# define	T_LESS_THAN_SIGN	281
# define	T_MINUS_SIGN	282
# define	T_OUT	283
# define	T_PERCENT_SIGN	284
# define	T_PLUS_SIGN	285
# define	T_RIGHT_CURLY_BRACKET	286
# define	T_RIGHT_PARANTHESIS	287
# define	T_RIGHT_SQUARE_BRACKET	288
# define	T_SCOPE	289
# define	T_SEMICOLON	290
# define	T_SHIFTLEFT	291
# define	T_SHIFTRIGHT	292
# define	T_SOLIDUS	293
# define	T_STRING_LITERAL	294
# define	T_STRUCT	295
# define	T_SWITCH	296
# define	T_TILDE	297
# define	T_TRUE	298
# define	T_VERTICAL_LINE	299
# define	T_LESS_OR_EQUAL	300
# define	T_GREATER_OR_EQUAL	301
# define	T_ASSIGN	302
# define	T_NOTEQUAL	303
# define	T_MEMBER	304
# define	T_WHILE	305
# define	T_IF	306
# define	T_ELSE	307
# define	T_FOR	308
# define	T_DO	309
# define	T_INCR	310
# define	T_DECR	311
# define	T_MAIN	312
# define	T_FOREACH	313
# define	T_SUBST	314
# define	T_MATCH	315
# define	T_NOT	316
# define	T_RETURN	317
# define	T_IMPORT	318
# define	T_VAR	319
# define	T_CATCH	320
# define	T_TRY	321
# define	T_RAISE	322
# define	T_RANGE	323
# define	T_CELL	324
# define	T_FROM	325
# define	T_PLUS_ASSIGN	326
# define	T_MINUS_ASSIGN	327
# define	T_AND	328
# define	T_OR	329
# define	T_DOLLAR	330
# define	T_UNKNOWN	331

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
#ifndef YYSTYPE
typedef union
{
  QString        *ident;
  KSParseNode    *node;
  KScript::Long   _int;
  QString        *_str;
  ushort          _char;
  KScript::Double _float;
} yystype;
# define YYSTYPE yystype
# define YYSTYPE_IS_TRIVIAL 1
#endif
#ifndef YYDEBUG
# define YYDEBUG 1
#endif



#define	YYFINAL		303
#define	YYFLAG		-32768
#define	YYNTBASE	78

/* YYTRANSLATE(YYLEX) -- Bison token number corresponding to YYLEX. */
#define YYTRANSLATE(x) ((unsigned)(x) <= 331 ? yytranslate[x] : 122)

/* YYTRANSLATE[YYLEX] -- Bison token number corresponding to YYLEX. */
static const char yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
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
       2,     2,     2,     2,     2,     2,     1,     3,     4,     5,
       6,     7,     8,     9,    10,    11,    12,    13,    14,    15,
      16,    17,    18,    19,    20,    21,    22,    23,    24,    25,
      26,    27,    28,    29,    30,    31,    32,    33,    34,    35,
      36,    37,    38,    39,    40,    41,    42,    43,    44,    45,
      46,    47,    48,    49,    50,    51,    52,    53,    54,    55,
      56,    57,    58,    59,    60,    61,    62,    63,    64,    65,
      66,    67,    68,    69,    70,    71,    72,    73,    74,    75,
      76,    77
};

#if YYDEBUG
static const short yyprhs[] =
{
       0,     0,     1,     2,     6,     8,    10,    13,    16,    19,
      22,    26,    32,    38,    40,    42,    46,    51,    59,    61,
      64,    68,    72,    76,    78,    82,    84,    88,    90,    94,
      98,   102,   106,   110,   114,   116,   118,   122,   124,   128,
     130,   134,   136,   140,   144,   146,   150,   154,   156,   160,
     164,   168,   171,   174,   177,   180,   182,   187,   192,   196,
     201,   203,   207,   209,   213,   215,   218,   221,   224,   227,
     229,   232,   235,   237,   239,   241,   243,   245,   249,   253,
     255,   257,   259,   261,   263,   265,   267,   269,   272,   276,
     279,   283,   285,   288,   292,   294,   302,   308,   314,   316,
     319,   322,   326,   329,   331,   335,   341,   348,   350,   354,
     357,   362,   365,   368,   369,   371,   373,   376,   379,   385,
     388,   392,   394,   400,   404,   406,   409,   419,   429,   434,
     437,   447,   452,   459,   467,   477,   478,   486,   489,   494
};
static const short yyrhs[] =
{
      -1,     0,    48,    79,    88,     0,    80,     0,    81,     0,
      81,    80,     0,   117,    36,     0,   105,    36,     0,   109,
      32,     0,    64,    20,    36,     0,    71,    20,    64,     7,
      36,     0,    71,    20,    64,    82,    36,     0,    83,     0,
      20,     0,    20,    12,    82,     0,    58,    24,   112,    32,
       0,    58,    25,   110,    33,    24,   112,    32,     0,    20,
       0,    35,    20,     0,    86,    48,    85,     0,    86,    72,
      85,     0,    86,    73,    85,     0,    86,     0,    87,    75,
      86,     0,    87,     0,    88,    74,    87,     0,    88,     0,
      89,    16,    88,     0,    89,    49,    88,     0,    89,    46,
      88,     0,    89,    47,    88,     0,    89,    27,    88,     0,
      89,    19,    88,     0,    89,     0,    90,     0,    89,    45,
      90,     0,    91,     0,    90,    10,    91,     0,    92,     0,
      91,     6,    92,     0,    93,     0,    92,    38,    93,     0,
      92,    37,    93,     0,    94,     0,    93,    31,    94,     0,
      93,    28,    94,     0,    95,     0,    94,     7,    95,     0,
      94,    39,    95,     0,    94,    30,    95,     0,    28,    96,
       0,    31,    96,     0,    43,    96,     0,    62,    96,     0,
      96,     0,    96,    26,    85,    34,     0,    96,    24,    85,
      32,     0,    96,    25,    33,     0,    96,    25,    97,    33,
       0,    98,     0,    85,    12,    97,     0,    85,     0,    96,
      50,    20,     0,    99,     0,    56,   100,     0,    57,   100,
       0,   100,    56,     0,   100,    57,     0,   100,     0,   101,
      61,     0,   101,    60,     0,   101,     0,    84,     0,   102,
       0,     5,     0,     3,     0,    27,    89,    19,     0,    25,
      85,    33,     0,    23,     0,    70,     0,    69,     0,    40,
       0,     9,     0,    18,     0,    44,     0,    17,     0,    26,
      34,     0,    26,   103,    34,     0,    24,    32,     0,    24,
     104,    32,     0,     4,     0,    76,    23,     0,    85,    12,
     103,     0,    85,     0,    25,    85,    12,    88,    33,    12,
     104,     0,    25,    85,    12,    88,    33,     0,    41,    20,
      24,   106,    32,     0,   107,     0,   107,   106,     0,   109,
      32,     0,    65,   108,    36,     0,   117,    36,     0,    20,
       0,    20,    12,   108,     0,    20,    25,    33,    24,   112,
       0,    20,    25,   110,    33,    24,   112,     0,   111,     0,
     111,    12,   110,     0,    21,    20,     0,    21,    20,    48,
      85,     0,    29,    20,     0,    22,    20,     0,     0,   113,
       0,   114,     0,   114,   113,     0,    85,    36,     0,    68,
      85,    12,    85,    36,     0,    63,    36,     0,    63,    85,
      36,     0,   118,     0,    67,    24,   113,    32,   115,     0,
      24,   113,    32,     0,   116,     0,   116,   115,     0,    66,
      25,    85,    12,    20,    33,    24,   113,    32,     0,    14,
      25,    20,    12,    20,    33,    24,   113,    32,     0,    13,
      20,    48,    88,     0,   120,   121,     0,    54,    25,    85,
      36,    85,    36,    85,    33,   121,     0,    55,   121,   120,
      36,     0,    52,    25,    85,    33,   121,   119,     0,    59,
      25,    20,    12,    85,    33,   121,     0,    59,    25,    20,
      12,    20,    12,    85,    33,   121,     0,     0,    53,    52,
      25,    85,    33,   121,   119,     0,    53,   121,     0,    51,
      25,    85,    33,     0,    24,   112,    32,     0
};

#endif

#if YYDEBUG
/* YYRLINE[YYN] -- source line where rule number YYN was defined. */
static const short yyrline[] =
{
       0,   181,   185,   185,   189,   196,   203,   227,   231,   235,
     239,   244,   250,   256,   263,   267,   276,   282,   291,   296,
     308,   312,   316,   320,   327,   331,   338,   342,   349,   353,
     357,   361,   365,   369,   373,   381,   385,   393,   397,   405,
     409,   417,   421,   425,   433,   437,   441,   449,   453,   457,
     461,   469,   473,   477,   481,   485,   492,   496,   500,   504,
     508,   515,   520,   527,   532,   539,   543,   547,   552,   557,
     564,   569,   574,   582,   586,   590,   594,   599,   600,   609,
     614,   619,   624,   629,   634,   639,   644,   649,   653,   657,
     661,   665,   669,   677,   681,   688,   692,   702,   711,   718,
     741,   745,   749,   756,   761,   769,   775,   783,   787,   794,
     799,   804,   809,   817,   821,   828,   832,   840,   844,   848,
     852,   856,   860,   864,   871,   875,   883,   889,   900,   908,
     912,   916,   920,   924,   929,   939,   943,   947,   954,   961
};
#endif


#if (YYDEBUG) || defined YYERROR_VERBOSE

/* YYTNAME[TOKEN_NUM] -- String name of the token TOKEN_NUM. */
static const char *const yytname[] =
{
  "$", "error", "$undefined.", "T_MATCH_LINE", "T_LINE", "T_INPUT", 
  "T_AMPERSAND", "T_ASTERISK", "T_CASE", "T_CHARACTER_LITERAL", 
  "T_CIRCUMFLEX", "T_COLON", "T_COMMA", "T_CONST", "T_DEFAULT", "T_ENUM", 
  "T_EQUAL", "T_FALSE", "T_FLOATING_PT_LITERAL", "T_GREATER_THAN_SIGN", 
  "T_IDENTIFIER", "T_IN", "T_INOUT", "T_INTEGER_LITERAL", 
  "T_LEFT_CURLY_BRACKET", "T_LEFT_PARANTHESIS", "T_LEFT_SQUARE_BRACKET", 
  "T_LESS_THAN_SIGN", "T_MINUS_SIGN", "T_OUT", "T_PERCENT_SIGN", 
  "T_PLUS_SIGN", "T_RIGHT_CURLY_BRACKET", "T_RIGHT_PARANTHESIS", 
  "T_RIGHT_SQUARE_BRACKET", "T_SCOPE", "T_SEMICOLON", "T_SHIFTLEFT", 
  "T_SHIFTRIGHT", "T_SOLIDUS", "T_STRING_LITERAL", "T_STRUCT", "T_SWITCH", 
  "T_TILDE", "T_TRUE", "T_VERTICAL_LINE", "T_LESS_OR_EQUAL", 
  "T_GREATER_OR_EQUAL", "T_ASSIGN", "T_NOTEQUAL", "T_MEMBER", "T_WHILE", 
  "T_IF", "T_ELSE", "T_FOR", "T_DO", "T_INCR", "T_DECR", "T_MAIN", 
  "T_FOREACH", "T_SUBST", "T_MATCH", "T_NOT", "T_RETURN", "T_IMPORT", 
  "T_VAR", "T_CATCH", "T_TRY", "T_RAISE", "T_RANGE", "T_CELL", "T_FROM", 
  "T_PLUS_ASSIGN", "T_MINUS_ASSIGN", "T_AND", "T_OR", "T_DOLLAR", 
  "T_UNKNOWN", "specification", "@1", "definitions", "definition", 
  "import_list", "main", "scoped_name", "assign_expr", "bool_or", 
  "bool_and", "equal_expr", "or_expr", "xor_expr", "and_expr", 
  "shift_expr", "add_expr", "mult_expr", "unary_expr", "index_expr", 
  "func_call_params", "member_expr", "incr_expr", "match_expr", 
  "primary_expr", "literal", "array_elements", "dict_elements", 
  "struct_dcl", "struct_exports", "struct_export", "struct_members", 
  "func_dcl", "func_params", "func_param", "func_body", "func_lines", 
  "func_line", "catches", "single_catch", "const_dcl", "loops", "else", 
  "while", "loop_body", 0
};
#endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives. */
static const short yyr1[] =
{
       0,    78,    79,    78,    78,    80,    80,    81,    81,    81,
      81,    81,    81,    81,    82,    82,    83,    83,    84,    84,
      85,    85,    85,    85,    86,    86,    87,    87,    88,    88,
      88,    88,    88,    88,    88,    89,    89,    90,    90,    91,
      91,    92,    92,    92,    93,    93,    93,    94,    94,    94,
      94,    95,    95,    95,    95,    95,    96,    96,    96,    96,
      96,    97,    97,    98,    98,    99,    99,    99,    99,    99,
     100,   100,   100,   101,   101,   101,   101,   101,   101,   102,
     102,   102,   102,   102,   102,   102,   102,   102,   102,   102,
     102,   102,   102,   103,   103,   104,   104,   105,   106,   106,
     107,   107,   107,   108,   108,   109,   109,   110,   110,   111,
     111,   111,   111,   112,   112,   113,   113,   114,   114,   114,
     114,   114,   114,   114,   115,   115,   116,   116,   117,   118,
     118,   118,   118,   118,   118,   119,   119,   119,   120,   121
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN. */
static const short yyr2[] =
{
       0,     0,     0,     3,     1,     1,     2,     2,     2,     2,
       3,     5,     5,     1,     1,     3,     4,     7,     1,     2,
       3,     3,     3,     1,     3,     1,     3,     1,     3,     3,
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

/* YYDEFACT[S] -- default rule to reduce with in state S when YYTABLE
   doesn't specify something else to do.  Zero means the default is an
   error. */
static const short yydefact[] =
{
       1,     0,     0,     0,     2,     0,     0,     0,     4,     5,
      13,     0,     0,     0,     0,     0,     0,     0,   113,     0,
       0,     0,     6,     8,     9,     7,     0,     0,     0,     0,
       0,     0,   107,     0,    76,    91,    75,    83,    86,    84,
      18,    79,     0,     0,     0,     0,     0,     0,     0,    82,
       0,    85,     0,     0,     0,    81,    80,     0,    73,     3,
      34,    35,    37,    39,    41,    44,    47,    55,    60,    64,
      69,    72,    74,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    23,    25,    27,     0,   114,   115,   121,
       0,     0,    10,     0,   128,   109,   112,   111,   113,     0,
       0,     0,     0,    98,     0,     0,     0,    89,     0,     0,
      87,    94,     0,     0,    51,    52,    19,    53,    65,    66,
      54,    92,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    67,    68,    71,    70,     0,     0,     0,     0,
       0,   113,     0,     0,   119,     0,     0,     0,   117,     0,
       0,     0,     0,     0,    16,   116,   129,     0,     0,    14,
       0,     0,   105,   113,   108,   103,     0,    97,    99,   100,
     102,     0,    90,    78,     0,    88,    77,    28,    33,    32,
      36,    30,    31,    29,    38,    40,    43,    42,    46,    45,
      48,    50,    49,     0,    58,    62,     0,     0,    63,     0,
     123,     0,     0,     0,     0,     0,     0,   120,     0,     0,
      20,    21,    22,    24,    26,   113,    11,     0,    12,   110,
     106,     0,   101,     0,    93,    57,     0,    59,    56,   138,
       0,     0,   139,   131,     0,     0,     0,     0,    15,   104,
       0,    61,   135,     0,    18,     0,     0,     0,   122,   124,
     118,    17,    96,     0,   132,     0,     0,     0,     0,     0,
     125,     0,     0,   137,     0,     0,   133,     0,     0,    95,
       0,     0,     0,     0,     0,     0,   130,   134,     0,     0,
       0,     0,     0,   135,     0,     0,   136,     0,     0,   127,
     126,     0,     0,     0
};

static const short yydefgoto[] =
{
     301,    17,     8,     9,   170,    10,    58,    82,    83,    84,
      85,    60,    61,    62,    63,    64,    65,    66,    67,   206,
      68,    69,    70,    71,    72,   112,   108,    11,   102,   103,
     176,    12,    31,    32,    86,    87,    88,   258,   259,    13,
      89,   264,    90,   152
};

static const short yypact[] =
{
      -4,    21,    -5,    39,-32768,    47,    41,    65,-32768,    10,
  -32768,    -3,    55,    53,    46,   141,    78,   470,   292,   103,
      68,    44,-32768,-32768,-32768,-32768,   470,    92,   102,   111,
     109,   116,   123,    -1,-32768,-32768,-32768,-32768,-32768,-32768,
  -32768,-32768,   -10,   470,    22,   470,   133,   133,   132,-32768,
     133,-32768,   580,   580,   133,-32768,-32768,   131,-32768,-32768,
      74,   151,   169,    -9,    42,     4,-32768,   119,-32768,-32768,
      26,    79,-32768,   224,   154,   155,   156,   159,   160,   360,
     162,   470,   148,    27,   112,   117,   163,-32768,   292,-32768,
     159,   161,-32768,     1,-32768,   140,-32768,-32768,   292,   168,
     103,   179,   172,    -1,   174,   164,   470,-32768,   175,   177,
  -32768,   196,   178,    13,   119,   119,-32768,   119,-32768,-32768,
     119,-32768,   470,   470,   470,   470,   470,   470,   470,   470,
     470,   470,   470,   470,   470,   470,   470,   470,   470,   415,
     470,   191,-32768,-32768,-32768,-32768,   470,   181,   470,   470,
     470,   292,   165,   194,-32768,   182,   292,   203,-32768,   470,
     470,   470,   470,   470,-32768,-32768,-32768,   193,   183,   209,
     195,   470,-32768,   292,-32768,   218,   198,-32768,-32768,-32768,
  -32768,   220,-32768,-32768,   470,-32768,-32768,-32768,-32768,-32768,
     151,-32768,-32768,-32768,   169,    -9,    42,    42,     4,     4,
  -32768,-32768,-32768,   204,-32768,   223,   205,   206,-32768,     2,
  -32768,   210,   212,   217,   207,   221,   234,-32768,   222,   470,
  -32768,-32768,-32768,-32768,-32768,   292,-32768,   238,-32768,-32768,
  -32768,   179,-32768,   470,-32768,-32768,   470,-32768,-32768,-32768,
     159,   470,-32768,-32768,   525,     3,   225,   228,-32768,-32768,
     229,-32768,   213,   227,   253,   236,   245,   246,-32768,     3,
  -32768,-32768,   260,     0,-32768,   470,   470,   159,   254,   470,
  -32768,   248,   252,-32768,   249,   251,-32768,   273,   276,-32768,
     470,   159,   159,   269,   270,   265,-32768,-32768,   266,   271,
     159,   278,   279,   213,   292,   292,-32768,   274,   275,-32768,
  -32768,   305,   308,-32768
};

static const short yypgoto[] =
{
  -32768,-32768,   302,-32768,    86,-32768,-32768,   -43,   152,   158,
     -13,   277,   199,   197,   200,    33,    38,    11,    80,    89,
  -32768,-32768,   114,-32768,-32768,   144,    58,-32768,   230,-32768,
     100,   -27,   -12,-32768,   -96,   -70,-32768,    75,-32768,   -23,
  -32768,    45,   185,   -85
};


#define	YYLAST		656


static const short yytable[] =
{
     109,   111,   172,   147,    59,   166,   104,    91,   168,     1,
     105,   135,     1,    94,   233,   106,     2,   256,   165,     2,
      15,   169,   107,     1,   151,    34,    35,    36,   131,   132,
       2,    37,   186,    23,   136,   183,   155,     3,   157,    38,
      39,    14,    40,   137,     4,    41,    42,    43,    44,    45,
      46,     3,   272,    47,     5,   214,   110,    48,   125,    16,
       6,    20,    49,   181,   101,    50,    51,     7,     5,   257,
     133,    18,    19,   134,     6,   159,   104,   230,    52,    53,
     105,     7,   142,   143,    54,    21,   218,    24,   174,    25,
     122,    55,    56,   123,    26,   203,   205,   207,    57,   160,
     161,   124,    33,   209,    92,   211,   212,   213,    93,   187,
     188,   189,    95,   191,   192,   193,   220,   221,   222,   125,
     126,   127,    96,   128,    27,    28,   114,   115,   229,   247,
     117,    97,    29,    98,   120,   100,    34,    35,    36,   144,
     145,   111,    37,   138,   139,   140,   200,   201,   202,    99,
      38,    39,   116,    40,   121,   252,    41,    42,    43,    44,
      45,   129,    27,    28,   196,   197,   118,   119,    48,   141,
      29,   198,   199,    49,    30,   130,   246,    51,   273,   148,
     149,   150,   276,   151,   158,   153,   156,   162,   171,    52,
      53,   163,   173,   205,   167,   164,   286,   287,   253,   175,
     180,   255,    55,    56,   177,   293,   179,   182,   184,    57,
     183,   208,   185,   210,   216,   219,    74,   225,   217,   226,
     250,   227,   274,   275,   297,   298,   278,    34,    35,    36,
     231,   228,   233,    37,   232,   236,   235,   285,   237,   242,
     238,    38,    39,   239,    40,   240,   244,    41,    73,   146,
      44,    45,    46,   241,   245,    47,   107,   243,   169,    48,
     261,   260,   262,   265,    49,   266,   263,    50,    51,   267,
     268,   269,   271,   106,   277,    74,    75,   280,    76,    77,
      52,    53,   281,    78,   282,   283,    54,    79,   284,   288,
     289,    80,    81,    55,    56,    34,    35,    36,   290,   291,
      57,    37,   294,   295,   292,   302,   299,   300,   303,    38,
      39,    22,    40,   248,   223,    41,    73,    43,    44,    45,
      46,   224,   113,    47,   190,   251,   194,    48,   234,   279,
     195,   249,    49,   178,   270,    50,    51,   215,   296,     0,
       0,     0,     0,    74,    75,     0,    76,    77,    52,    53,
       0,    78,     0,     0,    54,    79,     0,     0,     0,    80,
      81,    55,    56,    34,    35,    36,     0,     0,    57,    37,
       0,     0,     0,     0,     0,     0,     0,    38,    39,     0,
      40,     0,     0,    41,    42,    43,    44,    45,    46,     0,
       0,    47,     0,     0,     0,    48,   154,     0,     0,     0,
      49,     0,     0,    50,    51,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    52,    53,    34,    35,
      36,     0,    54,     0,    37,     0,     0,     0,     0,    55,
      56,     0,    38,    39,     0,    40,    57,     0,    41,    42,
      43,    44,    45,    46,     0,     0,    47,     0,   204,     0,
      48,     0,     0,     0,     0,    49,     0,     0,    50,    51,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    52,    53,    34,    35,    36,     0,    54,     0,    37,
       0,     0,     0,     0,    55,    56,     0,    38,    39,     0,
      40,    57,     0,    41,    42,    43,    44,    45,    46,     0,
       0,    47,     0,     0,     0,    48,     0,     0,     0,     0,
      49,     0,     0,    50,    51,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    52,    53,    34,    35,
      36,     0,    54,     0,    37,     0,     0,     0,     0,    55,
      56,     0,    38,    39,     0,   254,    57,     0,    41,    42,
      43,    44,    45,    46,     0,     0,    47,     0,     0,     0,
      48,     0,     0,     0,     0,    49,     0,     0,    50,    51,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    52,    53,    34,    35,    36,     0,    54,     0,    37,
       0,     0,     0,     0,    55,    56,     0,    38,    39,     0,
      40,    57,     0,    41,    42,    43,    44,    45,     0,     0,
       0,     0,     0,     0,     0,    48,     0,     0,     0,     0,
      49,     0,     0,     0,    51,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,    55,
      56,     0,     0,     0,     0,     0,    57
};

static const short yycheck[] =
{
      43,    44,    98,    73,    17,    90,    33,    19,     7,    13,
      33,     7,    13,    26,    12,    25,    20,    14,    88,    20,
      25,    20,    32,    13,    24,     3,     4,     5,    37,    38,
      20,     9,    19,    36,    30,    33,    79,    41,    81,    17,
      18,    20,    20,    39,    48,    23,    24,    25,    26,    27,
      28,    41,    52,    31,    58,   151,    34,    35,    45,    20,
      64,    20,    40,   106,    65,    43,    44,    71,    58,    66,
      28,    24,    25,    31,    64,    48,   103,   173,    56,    57,
     103,    71,    56,    57,    62,    20,   156,    32,   100,    36,
      16,    69,    70,    19,    48,   138,   139,   140,    76,    72,
      73,    27,    24,   146,    36,   148,   149,   150,    64,   122,
     123,   124,    20,   126,   127,   128,   159,   160,   161,    45,
      46,    47,    20,    49,    21,    22,    46,    47,   171,   225,
      50,    20,    29,    24,    54,    12,     3,     4,     5,    60,
      61,   184,     9,    24,    25,    26,   135,   136,   137,    33,
      17,    18,    20,    20,    23,   240,    23,    24,    25,    26,
      27,    10,    21,    22,   131,   132,    52,    53,    35,    50,
      29,   133,   134,    40,    33,     6,   219,    44,   263,    25,
      25,    25,   267,    24,    36,    25,    24,    75,    48,    56,
      57,    74,    24,   236,    33,    32,   281,   282,   241,    20,
      36,   244,    69,    70,    32,   290,    32,    32,    12,    76,
      33,    20,    34,    32,    20,    12,    51,    24,    36,    36,
     233,    12,   265,   266,   294,   295,   269,     3,     4,     5,
      12,    36,    12,     9,    36,    12,    32,   280,    33,    32,
      34,    17,    18,    33,    20,    33,    12,    23,    24,    25,
      26,    27,    28,    36,    32,    31,    32,    36,    20,    35,
      32,    36,    33,    36,    40,    12,    53,    43,    44,    33,
      25,    25,    12,    25,    20,    51,    52,    25,    54,    55,
      56,    57,    33,    59,    33,    12,    62,    63,    12,    20,
      20,    67,    68,    69,    70,     3,     4,     5,    33,    33,
      76,     9,    24,    24,    33,     0,    32,    32,     0,    17,
      18,     9,    20,   227,   162,    23,    24,    25,    26,    27,
      28,   163,    45,    31,   125,   236,   129,    35,   184,   271,
     130,   231,    40,   103,   259,    43,    44,   152,   293,    -1,
      -1,    -1,    -1,    51,    52,    -1,    54,    55,    56,    57,
      -1,    59,    -1,    -1,    62,    63,    -1,    -1,    -1,    67,
      68,    69,    70,     3,     4,     5,    -1,    -1,    76,     9,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    17,    18,    -1,
      20,    -1,    -1,    23,    24,    25,    26,    27,    28,    -1,
      -1,    31,    -1,    -1,    -1,    35,    36,    -1,    -1,    -1,
      40,    -1,    -1,    43,    44,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    56,    57,     3,     4,
       5,    -1,    62,    -1,     9,    -1,    -1,    -1,    -1,    69,
      70,    -1,    17,    18,    -1,    20,    76,    -1,    23,    24,
      25,    26,    27,    28,    -1,    -1,    31,    -1,    33,    -1,
      35,    -1,    -1,    -1,    -1,    40,    -1,    -1,    43,    44,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    56,    57,     3,     4,     5,    -1,    62,    -1,     9,
      -1,    -1,    -1,    -1,    69,    70,    -1,    17,    18,    -1,
      20,    76,    -1,    23,    24,    25,    26,    27,    28,    -1,
      -1,    31,    -1,    -1,    -1,    35,    -1,    -1,    -1,    -1,
      40,    -1,    -1,    43,    44,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    56,    57,     3,     4,
       5,    -1,    62,    -1,     9,    -1,    -1,    -1,    -1,    69,
      70,    -1,    17,    18,    -1,    20,    76,    -1,    23,    24,
      25,    26,    27,    28,    -1,    -1,    31,    -1,    -1,    -1,
      35,    -1,    -1,    -1,    -1,    40,    -1,    -1,    43,    44,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    56,    57,     3,     4,     5,    -1,    62,    -1,     9,
      -1,    -1,    -1,    -1,    69,    70,    -1,    17,    18,    -1,
      20,    76,    -1,    23,    24,    25,    26,    27,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    35,    -1,    -1,    -1,    -1,
      40,    -1,    -1,    -1,    44,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    69,
      70,    -1,    -1,    -1,    -1,    -1,    76
};
/* -*-C-*-  Note some compilers choke on comments on `#line' lines.  */
#line 3 "/usr/share/bison/bison.simple"

/* Skeleton output parser for bison,

   Copyright (C) 1984, 1989, 1990, 2000, 2001, 2002 Free Software
   Foundation, Inc.

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
   Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.  */

/* As a special exception, when this file is copied by Bison into a
   Bison output file, you may use that output file without restriction.
   This special exception was added by the Free Software Foundation
   in version 1.24 of Bison.  */

/* This is the parser code that is written into each bison parser when
   the %semantic_parser declaration is not specified in the grammar.
   It was written by Richard Stallman by simplifying the hairy parser
   used when %semantic_parser is specified.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

#if ! defined (yyoverflow) || defined (YYERROR_VERBOSE)

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# if YYSTACK_USE_ALLOCA
#  define YYSTACK_ALLOC alloca
# else
#  ifndef YYSTACK_USE_ALLOCA
#   if defined (alloca) || defined (_ALLOCA_H)
#    define YYSTACK_ALLOC alloca
#   else
#    ifdef __GNUC__
#     define YYSTACK_ALLOC __builtin_alloca
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's `empty if-body' warning. */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (0)
# else
#  if defined (__STDC__) || defined (__cplusplus)
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   define YYSIZE_T size_t
#  endif
#  define YYSTACK_ALLOC malloc
#  define YYSTACK_FREE free
# endif
#endif /* ! defined (yyoverflow) || defined (YYERROR_VERBOSE) */


#if (! defined (yyoverflow) \
     && (! defined (__cplusplus) \
	 || (YYLTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  short yyss;
  YYSTYPE yyvs;
# if YYLSP_NEEDED
  YYLTYPE yyls;
# endif
};

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAX (sizeof (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# if YYLSP_NEEDED
#  define YYSTACK_BYTES(N) \
     ((N) * (sizeof (short) + sizeof (YYSTYPE) + sizeof (YYLTYPE))	\
      + 2 * YYSTACK_GAP_MAX)
# else
#  define YYSTACK_BYTES(N) \
     ((N) * (sizeof (short) + sizeof (YYSTYPE))				\
      + YYSTACK_GAP_MAX)
# endif

/* Copy COUNT objects from FROM to TO.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if 1 < __GNUC__
#   define YYCOPY(To, From, Count) \
      __builtin_memcpy (To, From, (Count) * sizeof (*(From)))
#  else
#   define YYCOPY(To, From, Count)		\
      do					\
	{					\
	  register YYSIZE_T yyi;		\
	  for (yyi = 0; yyi < (Count); yyi++)	\
	    (To)[yyi] = (From)[yyi];		\
	}					\
      while (0)
#  endif
# endif

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack)					\
    do									\
      {									\
	YYSIZE_T yynewbytes;						\
	YYCOPY (&yyptr->Stack, Stack, yysize);				\
	Stack = &yyptr->Stack;						\
	yynewbytes = yystacksize * sizeof (*Stack) + YYSTACK_GAP_MAX;	\
	yyptr += yynewbytes / sizeof (*yyptr);				\
      }									\
    while (0)

#endif


#if ! defined (YYSIZE_T) && defined (__SIZE_TYPE__)
# define YYSIZE_T __SIZE_TYPE__
#endif
#if ! defined (YYSIZE_T) && defined (size_t)
# define YYSIZE_T size_t
#endif
#if ! defined (YYSIZE_T)
# if defined (__STDC__) || defined (__cplusplus)
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# endif
#endif
#if ! defined (YYSIZE_T)
# define YYSIZE_T unsigned int
#endif

#define yyerrok		(yyerrstatus = 0)
#define yyclearin	(yychar = YYEMPTY)
#define YYEMPTY		-2
#define YYEOF		0
#define YYACCEPT	goto yyacceptlab
#define YYABORT 	goto yyabortlab
#define YYERROR		goto yyerrlab1
/* Like YYERROR except do call yyerror.  This remains here temporarily
   to ease the transition to the new meaning of YYERROR, for GCC.
   Once GCC version 2 has supplanted version 1, this can go.  */
#define YYFAIL		goto yyerrlab
#define YYRECOVERING()  (!!yyerrstatus)
#define YYBACKUP(Token, Value)					\
do								\
  if (yychar == YYEMPTY && yylen == 1)				\
    {								\
      yychar = (Token);						\
      yylval = (Value);						\
      yychar1 = YYTRANSLATE (yychar);				\
      YYPOPSTACK;						\
      goto yybackup;						\
    }								\
  else								\
    { 								\
      yyerror ("syntax error: cannot back up");			\
      YYERROR;							\
    }								\
while (0)

#define YYTERROR	1
#define YYERRCODE	256


/* YYLLOC_DEFAULT -- Compute the default location (before the actions
   are run).

   When YYLLOC_DEFAULT is run, CURRENT is set the location of the
   first token.  By default, to implement support for ranges, extend
   its range to the last symbol.  */

#ifndef YYLLOC_DEFAULT
# define YYLLOC_DEFAULT(Current, Rhs, N)       	\
   Current.last_line   = Rhs[N].last_line;	\
   Current.last_column = Rhs[N].last_column;
#endif


/* YYLEX -- calling `yylex' with the right arguments.  */

#if YYPURE
# if YYLSP_NEEDED
#  ifdef YYLEX_PARAM
#   define YYLEX		yylex (&yylval, &yylloc, YYLEX_PARAM)
#  else
#   define YYLEX		yylex (&yylval, &yylloc)
#  endif
# else /* !YYLSP_NEEDED */
#  ifdef YYLEX_PARAM
#   define YYLEX		yylex (&yylval, YYLEX_PARAM)
#  else
#   define YYLEX		yylex (&yylval)
#  endif
# endif /* !YYLSP_NEEDED */
#else /* !YYPURE */
# define YYLEX			yylex ()
#endif /* !YYPURE */


/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)			\
do {						\
  if (yydebug)					\
    YYFPRINTF Args;				\
} while (0)
/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args)
#endif /* !YYDEBUG */

/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef	YYINITDEPTH
# define YYINITDEPTH 200
#endif

/* YYMAXDEPTH -- maximum size the stacks can grow to (effective only
   if the built-in stack extension method is used).

   Do not make this value too large; the results are undefined if
   SIZE_MAX < YYSTACK_BYTES (YYMAXDEPTH)
   evaluated with infinite-precision integer arithmetic.  */

#if YYMAXDEPTH == 0
# undef YYMAXDEPTH
#endif

#ifndef YYMAXDEPTH
# define YYMAXDEPTH 10000
#endif

#ifdef YYERROR_VERBOSE

# ifndef yystrlen
#  if defined (__GLIBC__) && defined (_STRING_H)
#   define yystrlen strlen
#  else
/* Return the length of YYSTR.  */
static YYSIZE_T
#   if defined (__STDC__) || defined (__cplusplus)
yystrlen (const char *yystr)
#   else
yystrlen (yystr)
     const char *yystr;
#   endif
{
  register const char *yys = yystr;

  while (*yys++ != '\0')
    continue;

  return yys - yystr - 1;
}
#  endif
# endif

# ifndef yystpcpy
#  if defined (__GLIBC__) && defined (_STRING_H) && defined (_GNU_SOURCE)
#   define yystpcpy stpcpy
#  else
/* Copy YYSRC to YYDEST, returning the address of the terminating '\0' in
   YYDEST.  */
static char *
#   if defined (__STDC__) || defined (__cplusplus)
yystpcpy (char *yydest, const char *yysrc)
#   else
yystpcpy (yydest, yysrc)
     char *yydest;
     const char *yysrc;
#   endif
{
  register char *yyd = yydest;
  register const char *yys = yysrc;

  while ((*yyd++ = *yys++) != '\0')
    continue;

  return yyd - 1;
}
#  endif
# endif
#endif

#line 315 "/usr/share/bison/bison.simple"


/* The user can define YYPARSE_PARAM as the name of an argument to be passed
   into yyparse.  The argument should have type void *.
   It should actually point to an object.
   Grammar actions can access the variable by casting it
   to the proper pointer type.  */

#ifdef YYPARSE_PARAM
# if defined (__STDC__) || defined (__cplusplus)
#  define YYPARSE_PARAM_ARG void *YYPARSE_PARAM
#  define YYPARSE_PARAM_DECL
# else
#  define YYPARSE_PARAM_ARG YYPARSE_PARAM
#  define YYPARSE_PARAM_DECL void *YYPARSE_PARAM;
# endif
#else /* !YYPARSE_PARAM */
# define YYPARSE_PARAM_ARG
# define YYPARSE_PARAM_DECL
#endif /* !YYPARSE_PARAM */

/* Prevent warning if -Wstrict-prototypes.  */
#ifdef __GNUC__
# ifdef YYPARSE_PARAM
int yyparse (void *);
# else
int yyparse (void);
# endif
#endif

/* YY_DECL_VARIABLES -- depending whether we use a pure parser,
   variables are global, or local to YYPARSE.  */

#define YY_DECL_NON_LSP_VARIABLES			\
/* The lookahead symbol.  */				\
int yychar;						\
							\
/* The semantic value of the lookahead symbol. */	\
YYSTYPE yylval;						\
							\
/* Number of parse errors so far.  */			\
int yynerrs;

#if YYLSP_NEEDED
# define YY_DECL_VARIABLES			\
YY_DECL_NON_LSP_VARIABLES			\
						\
/* Location data for the lookahead symbol.  */	\
YYLTYPE yylloc;
#else
# define YY_DECL_VARIABLES			\
YY_DECL_NON_LSP_VARIABLES
#endif


/* If nonreentrant, generate the variables here. */

#if !YYPURE
YY_DECL_VARIABLES
#endif  /* !YYPURE */

int
yyparse (YYPARSE_PARAM_ARG)
     YYPARSE_PARAM_DECL
{
  /* If reentrant, generate the variables here. */
#if YYPURE
  YY_DECL_VARIABLES
#endif  /* !YYPURE */

  register int yystate;
  register int yyn;
  int yyresult;
  /* Number of tokens to shift before error messages enabled.  */
  int yyerrstatus;
  /* Lookahead token as an internal (translated) token number.  */
  int yychar1 = 0;

  /* Three stacks and their tools:
     `yyss': related to states,
     `yyvs': related to semantic values,
     `yyls': related to locations.

     Refer to the stacks thru separate pointers, to allow yyoverflow
     to reallocate them elsewhere.  */

  /* The state stack. */
  short	yyssa[YYINITDEPTH];
  short *yyss = yyssa;
  register short *yyssp;

  /* The semantic value stack.  */
  YYSTYPE yyvsa[YYINITDEPTH];
  YYSTYPE *yyvs = yyvsa;
  register YYSTYPE *yyvsp;

#if YYLSP_NEEDED
  /* The location stack.  */
  YYLTYPE yylsa[YYINITDEPTH];
  YYLTYPE *yyls = yylsa;
  YYLTYPE *yylsp;
#endif

#if YYLSP_NEEDED
# define YYPOPSTACK   (yyvsp--, yyssp--, yylsp--)
#else
# define YYPOPSTACK   (yyvsp--, yyssp--)
#endif

  YYSIZE_T yystacksize = YYINITDEPTH;


  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;
#if YYLSP_NEEDED
  YYLTYPE yyloc;
#endif

  /* When reducing, the number of symbols on the RHS of the reduced
     rule. */
  int yylen;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY;		/* Cause a token to be read.  */

  /* Initialize stack pointers.
     Waste one element of value and location stack
     so that they stay on the same level as the state stack.
     The wasted elements are never initialized.  */

  yyssp = yyss;
  yyvsp = yyvs;
#if YYLSP_NEEDED
  yylsp = yyls;
#endif
  goto yysetstate;

/*------------------------------------------------------------.
| yynewstate -- Push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
 yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed. so pushing a state here evens the stacks.
     */
  yyssp++;

 yysetstate:
  *yyssp = yystate;

  if (yyssp >= yyss + yystacksize - 1)
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYSIZE_T yysize = yyssp - yyss + 1;

#ifdef yyoverflow
      {
	/* Give user a chance to reallocate the stack. Use copies of
	   these so that the &'s don't force the real ones into
	   memory.  */
	YYSTYPE *yyvs1 = yyvs;
	short *yyss1 = yyss;

	/* Each stack pointer address is followed by the size of the
	   data in use in that stack, in bytes.  */
# if YYLSP_NEEDED
	YYLTYPE *yyls1 = yyls;
	/* This used to be a conditional around just the two extra args,
	   but that might be undefined if yyoverflow is a macro.  */
	yyoverflow ("parser stack overflow",
		    &yyss1, yysize * sizeof (*yyssp),
		    &yyvs1, yysize * sizeof (*yyvsp),
		    &yyls1, yysize * sizeof (*yylsp),
		    &yystacksize);
	yyls = yyls1;
# else
	yyoverflow ("parser stack overflow",
		    &yyss1, yysize * sizeof (*yyssp),
		    &yyvs1, yysize * sizeof (*yyvsp),
		    &yystacksize);
# endif
	yyss = yyss1;
	yyvs = yyvs1;
      }
#else /* no yyoverflow */
# ifndef YYSTACK_RELOCATE
      goto yyoverflowlab;
# else
      /* Extend the stack our own way.  */
      if (yystacksize >= YYMAXDEPTH)
	goto yyoverflowlab;
      yystacksize *= 2;
      if (yystacksize > YYMAXDEPTH)
	yystacksize = YYMAXDEPTH;

      {
	short *yyss1 = yyss;
	union yyalloc *yyptr =
	  (union yyalloc *) YYSTACK_ALLOC (YYSTACK_BYTES (yystacksize));
	if (! yyptr)
	  goto yyoverflowlab;
	YYSTACK_RELOCATE (yyss);
	YYSTACK_RELOCATE (yyvs);
# if YYLSP_NEEDED
	YYSTACK_RELOCATE (yyls);
# endif
# undef YYSTACK_RELOCATE
	if (yyss1 != yyssa)
	  YYSTACK_FREE (yyss1);
      }
# endif
#endif /* no yyoverflow */

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;
#if YYLSP_NEEDED
      yylsp = yyls + yysize - 1;
#endif

      YYDPRINTF ((stderr, "Stack size increased to %lu\n",
		  (unsigned long int) yystacksize));

      if (yyssp >= yyss + yystacksize - 1)
	YYABORT;
    }

  YYDPRINTF ((stderr, "Entering state %d\n", yystate));

  goto yybackup;


/*-----------.
| yybackup.  |
`-----------*/
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
      YYDPRINTF ((stderr, "Reading a token: "));
      yychar = YYLEX;
    }

  /* Convert token to internal form (in yychar1) for indexing tables with */

  if (yychar <= 0)		/* This means end of input. */
    {
      yychar1 = 0;
      yychar = YYEOF;		/* Don't call YYLEX any more */

      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else
    {
      yychar1 = YYTRANSLATE (yychar);

#if YYDEBUG
     /* We have to keep this `#if YYDEBUG', since we use variables
	which are defined only if `YYDEBUG' is set.  */
      if (yydebug)
	{
	  YYFPRINTF (stderr, "Next token is %d (%s",
		     yychar, yytname[yychar1]);
	  /* Give the individual parser a way to print the precise
	     meaning of a token, for further debugging info.  */
# ifdef YYPRINT
	  YYPRINT (stderr, yychar, yylval);
# endif
	  YYFPRINTF (stderr, ")\n");
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
  YYDPRINTF ((stderr, "Shifting token %d (%s), ",
	      yychar, yytname[yychar1]));

  /* Discard the token being shifted unless it is eof.  */
  if (yychar != YYEOF)
    yychar = YYEMPTY;

  *++yyvsp = yylval;
#if YYLSP_NEEDED
  *++yylsp = yylloc;
#endif

  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  yystate = yyn;
  goto yynewstate;


/*-----------------------------------------------------------.
| yydefault -- do the default action for the current state.  |
`-----------------------------------------------------------*/
yydefault:
  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;
  goto yyreduce;


/*-----------------------------.
| yyreduce -- Do a reduction.  |
`-----------------------------*/
yyreduce:
  /* yyn is the number of a rule to reduce with.  */
  yylen = yyr2[yyn];

  /* If YYLEN is nonzero, implement the default value of the action:
     `$$ = $1'.

     Otherwise, the following line sets YYVAL to the semantic value of
     the lookahead token.  This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];

#if YYLSP_NEEDED
  /* Similarly for the default location.  Let the user run additional
     commands if for instance locations are ranges.  */
  yyloc = yylsp[1-yylen];
  YYLLOC_DEFAULT (yyloc, (yylsp - yylen), yylen);
#endif

#if YYDEBUG
  /* We have to keep this `#if YYDEBUG', since we use variables which
     are defined only if `YYDEBUG' is set.  */
  if (yydebug)
    {
      int yyi;

      YYFPRINTF (stderr, "Reducing via rule %d (line %d), ",
		 yyn, yyrline[yyn]);

      /* Print the symbols being reduced, and their result.  */
      for (yyi = yyprhs[yyn]; yyrhs[yyi] > 0; yyi++)
	YYFPRINTF (stderr, "%s ", yytname[yyrhs[yyi]]);
      YYFPRINTF (stderr, " -> %s\n", yytname[yyr1[yyn]]);
    }
#endif

  switch (yyn) {

case 1:
#line 182 "yacc.yy"
{
            theParser->setRootNode( NULL );
          ;
    break;}
case 2:
#line 185 "yacc.yy"
{ kspread_mode(); ;
    break;}
case 3:
#line 186 "yacc.yy"
{
            theParser->setRootNode( yyvsp[0].node );
          ;
    break;}
case 4:
#line 190 "yacc.yy"
{
            theParser->setRootNode( new KSParseNode( definitions, yyvsp[0].node ) );
          ;
    break;}
case 5:
#line 197 "yacc.yy"
{
            if ( yyvsp[0].node != 0 )
            {
                yyval.node = new KSParseNode( definitions, yyvsp[0].node );
            }
          ;
    break;}
case 6:
#line 204 "yacc.yy"
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
#line 228 "yacc.yy"
{
            yyval.node = yyvsp[-1].node;
          ;
    break;}
case 8:
#line 232 "yacc.yy"
{
            yyval.node = yyvsp[-1].node;
          ;
    break;}
case 9:
#line 236 "yacc.yy"
{
            yyval.node = yyvsp[-1].node;
          ;
    break;}
case 10:
#line 240 "yacc.yy"
{
            yyval.node = new KSParseNode( import );
            yyval.node->setIdent( yyvsp[-1].ident );
          ;
    break;}
case 11:
#line 245 "yacc.yy"
{
            yyval.node = new KSParseNode( from );
            yyval.node->setIdent( yyvsp[-3].ident );
            yyval.node->setStringLiteral( QString( "" ) );
          ;
    break;}
case 12:
#line 251 "yacc.yy"
{
            yyval.node = new KSParseNode( from );
            yyval.node->setIdent( yyvsp[-3].ident );
            yyval.node->setStringLiteral( yyvsp[-1]._str );
          ;
    break;}
case 13:
#line 257 "yacc.yy"
{
            yyval.node = yyvsp[0].node;
          ;
    break;}
case 14:
#line 264 "yacc.yy"
{
                yyval._str = yyvsp[0].ident;
          ;
    break;}
case 15:
#line 268 "yacc.yy"
{
                (*yyvsp[-2].ident) += "/";
                (*yyvsp[-2].ident) += (*yyvsp[0]._str);
                yyval._str = yyvsp[-2].ident;
          ;
    break;}
case 16:
#line 277 "yacc.yy"
{
            yyval.node = new KSParseNode( func_dcl );
            yyval.node->setBranch( 2, yyvsp[-1].node );
            yyval.node->setIdent( "main" );
          ;
    break;}
case 17:
#line 283 "yacc.yy"
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
            yyval.node = new KSParseNode( t_less_or_equal, yyvsp[-2].node, yyvsp[0].node );
          ;
    break;}
case 31:
#line 362 "yacc.yy"
{
            yyval.node = new KSParseNode( t_greater_or_equal, yyvsp[-2].node, yyvsp[0].node );
          ;
    break;}
case 32:
#line 366 "yacc.yy"
{
            yyval.node = new KSParseNode( t_less, yyvsp[-2].node, yyvsp[0].node );
          ;
    break;}
case 33:
#line 370 "yacc.yy"
{
            yyval.node = new KSParseNode( t_greater, yyvsp[-2].node, yyvsp[0].node );
          ;
    break;}
case 34:
#line 374 "yacc.yy"
{
            yyval.node = yyvsp[0].node;
          ;
    break;}
case 35:
#line 382 "yacc.yy"
{
            yyval.node = yyvsp[0].node;
          ;
    break;}
case 36:
#line 386 "yacc.yy"
{
            yyval.node = new KSParseNode( t_vertical_line, yyvsp[-2].node, yyvsp[0].node );
          ;
    break;}
case 37:
#line 394 "yacc.yy"
{
            yyval.node = yyvsp[0].node;
          ;
    break;}
case 38:
#line 398 "yacc.yy"
{
            yyval.node = new KSParseNode( t_circumflex, yyvsp[-2].node, yyvsp[0].node );
          ;
    break;}
case 39:
#line 406 "yacc.yy"
{
            yyval.node = yyvsp[0].node;
          ;
    break;}
case 40:
#line 410 "yacc.yy"
{
            yyval.node = new KSParseNode( t_ampersand, yyvsp[-2].node, yyvsp[0].node );
          ;
    break;}
case 41:
#line 418 "yacc.yy"
{
            yyval.node = yyvsp[0].node;
          ;
    break;}
case 42:
#line 422 "yacc.yy"
{
            yyval.node = new KSParseNode( t_shiftright, yyvsp[-2].node, yyvsp[0].node );
          ;
    break;}
case 43:
#line 426 "yacc.yy"
{
            yyval.node = new KSParseNode( t_shiftleft, yyvsp[-2].node, yyvsp[0].node );
          ;
    break;}
case 44:
#line 434 "yacc.yy"
{
            yyval.node = yyvsp[0].node;
          ;
    break;}
case 45:
#line 438 "yacc.yy"
{
            yyval.node = new KSParseNode( t_plus_sign, yyvsp[-2].node, yyvsp[0].node );
          ;
    break;}
case 46:
#line 442 "yacc.yy"
{
            yyval.node = new KSParseNode( t_minus_sign, yyvsp[-2].node, yyvsp[0].node );
          ;
    break;}
case 47:
#line 450 "yacc.yy"
{
            yyval.node = yyvsp[0].node;
          ;
    break;}
case 48:
#line 454 "yacc.yy"
{
            yyval.node = new KSParseNode( t_asterik, yyvsp[-2].node, yyvsp[0].node );
          ;
    break;}
case 49:
#line 458 "yacc.yy"
{
            yyval.node = new KSParseNode( t_solidus, yyvsp[-2].node, yyvsp[0].node );
          ;
    break;}
case 50:
#line 462 "yacc.yy"
{
            yyval.node = new KSParseNode( t_percent_sign, yyvsp[-2].node, yyvsp[0].node );
          ;
    break;}
case 51:
#line 470 "yacc.yy"
{
            yyval.node = new KSParseNode( t_minus_sign, yyvsp[0].node );
          ;
    break;}
case 52:
#line 474 "yacc.yy"
{
            yyval.node = new KSParseNode( t_plus_sign, yyvsp[0].node );
          ;
    break;}
case 53:
#line 478 "yacc.yy"
{
            yyval.node = new KSParseNode( t_tilde, yyvsp[0].node );
          ;
    break;}
case 54:
#line 482 "yacc.yy"
{
            yyval.node = new KSParseNode( t_not, yyvsp[0].node );
          ;
    break;}
case 55:
#line 486 "yacc.yy"
{
            yyval.node = yyvsp[0].node;
          ;
    break;}
case 56:
#line 493 "yacc.yy"
{
            yyval.node = new KSParseNode( t_array, yyvsp[-3].node, yyvsp[-1].node );
          ;
    break;}
case 57:
#line 497 "yacc.yy"
{
            yyval.node = new KSParseNode( t_dict, yyvsp[-3].node, yyvsp[-1].node );
          ;
    break;}
case 58:
#line 501 "yacc.yy"
{
            yyval.node = new KSParseNode( t_func_call, yyvsp[-2].node );
          ;
    break;}
case 59:
#line 505 "yacc.yy"
{
            yyval.node = new KSParseNode( t_func_call, yyvsp[-3].node, yyvsp[-1].node );
          ;
    break;}
case 60:
#line 509 "yacc.yy"
{
            yyval.node = yyvsp[0].node;
          ;
    break;}
case 61:
#line 516 "yacc.yy"
{
            yyval.node = new KSParseNode( func_call_params, yyvsp[-2].node, yyvsp[0].node );
          ;
    break;}
case 62:
#line 521 "yacc.yy"
{
            yyval.node = new KSParseNode( func_call_params, yyvsp[0].node );
          ;
    break;}
case 63:
#line 528 "yacc.yy"
{
            yyval.node = new KSParseNode( member_expr, yyvsp[-2].node );
            yyval.node->setIdent( yyvsp[0].ident );
          ;
    break;}
case 64:
#line 533 "yacc.yy"
{
            yyval.node = yyvsp[0].node;
          ;
    break;}
case 65:
#line 540 "yacc.yy"
{
            yyval.node = new KSParseNode( t_incr, yyvsp[0].node );
          ;
    break;}
case 66:
#line 544 "yacc.yy"
{
            yyval.node = new KSParseNode( t_decr, yyvsp[0].node );
          ;
    break;}
case 67:
#line 548 "yacc.yy"
{
            /* Setting $1 twice indicates that this is a postfix operator */
            yyval.node = new KSParseNode( t_incr, yyvsp[-1].node, yyvsp[-1].node );
          ;
    break;}
case 68:
#line 553 "yacc.yy"
{
            /* Setting $1 twice indicates that this is a postfix operator */
            yyval.node = new KSParseNode( t_decr, yyvsp[-1].node, yyvsp[-1].node );
          ;
    break;}
case 69:
#line 558 "yacc.yy"
{
            yyval.node = yyvsp[0].node;
          ;
    break;}
case 70:
#line 565 "yacc.yy"
{
            yyval.node = new KSParseNode( t_match, yyvsp[-1].node );
            yyval.node->setIdent( yyvsp[0]._str );
          ;
    break;}
case 71:
#line 570 "yacc.yy"
{
            yyval.node = new KSParseNode( t_subst, yyvsp[-1].node );
            yyval.node->setIdent( yyvsp[0].ident );
          ;
    break;}
case 72:
#line 575 "yacc.yy"
{
            yyval.node = yyvsp[0].node;
          ;
    break;}
case 73:
#line 583 "yacc.yy"
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
            yyval.node = new KSParseNode( t_input );
          ;
    break;}
case 76:
#line 595 "yacc.yy"
{
            yyval.node = new KSParseNode( t_match_line );
            yyval.node->setIdent( yyvsp[0]._str );
          ;
    break;}
case 77:
#line 599 "yacc.yy"
{ ;
    break;}
case 78:
#line 601 "yacc.yy"
{
            yyval.node = yyvsp[-1].node;
          ;
    break;}
case 79:
#line 610 "yacc.yy"
{
            yyval.node = new KSParseNode( t_integer_literal );
            yyval.node->setIntegerLiteral( yyvsp[0]._int );
          ;
    break;}
case 80:
#line 615 "yacc.yy"
{
            yyval.node = new KSParseNode( t_cell );
            yyval.node->setStringLiteral( yyvsp[0]._str );
          ;
    break;}
case 81:
#line 620 "yacc.yy"
{
            yyval.node = new KSParseNode( t_range );
            yyval.node->setStringLiteral( yyvsp[0]._str );
          ;
    break;}
case 82:
#line 625 "yacc.yy"
{
            yyval.node = new KSParseNode( t_string_literal );
            yyval.node->setStringLiteral( yyvsp[0]._str );
          ;
    break;}
case 83:
#line 630 "yacc.yy"
{
            yyval.node = new KSParseNode( t_character_literal );
            yyval.node->setCharacterLiteral( yyvsp[0]._char );
          ;
    break;}
case 84:
#line 635 "yacc.yy"
{
            yyval.node = new KSParseNode( t_floating_pt_literal );
            yyval.node->setFloatingPtLiteral( yyvsp[0]._float );
          ;
    break;}
case 85:
#line 640 "yacc.yy"
{
            yyval.node = new KSParseNode( t_boolean_literal );
            yyval.node->setBooleanLiteral( true );
          ;
    break;}
case 86:
#line 645 "yacc.yy"
{
            yyval.node = new KSParseNode( t_boolean_literal );
            yyval.node->setBooleanLiteral( false );
          ;
    break;}
case 87:
#line 650 "yacc.yy"
{
            yyval.node = new KSParseNode( t_array_const );
          ;
    break;}
case 88:
#line 654 "yacc.yy"
{
            yyval.node = new KSParseNode( t_array_const, yyvsp[-1].node );
          ;
    break;}
case 89:
#line 658 "yacc.yy"
{
            yyval.node = new KSParseNode( t_dict_const );
          ;
    break;}
case 90:
#line 662 "yacc.yy"
{
            yyval.node = new KSParseNode( t_dict_const, yyvsp[-1].node );
          ;
    break;}
case 91:
#line 666 "yacc.yy"
{
            yyval.node = new KSParseNode( t_line );
          ;
    break;}
case 92:
#line 670 "yacc.yy"
{
            yyval.node = new KSParseNode( t_regexp_group );
            yyval.node->setIntegerLiteral( yyvsp[0]._int );
          ;
    break;}
case 93:
#line 678 "yacc.yy"
{
            yyval.node = new KSParseNode( t_array_element, yyvsp[-2].node, yyvsp[0].node );
          ;
    break;}
case 94:
#line 682 "yacc.yy"
{
            yyval.node = new KSParseNode( t_array_element, yyvsp[0].node  );
          ;
    break;}
case 95:
#line 689 "yacc.yy"
{
            yyval.node = new KSParseNode( t_dict_element, yyvsp[-5].node, yyvsp[-3].node, yyvsp[0].node );
          ;
    break;}
case 96:
#line 693 "yacc.yy"
{
            yyval.node = new KSParseNode( t_dict_element, yyvsp[-3].node, yyvsp[-1].node );
          ;
    break;}
case 97:
#line 703 "yacc.yy"
{
            yyval.node = new KSParseNode( t_struct, yyvsp[-1].node );
            yyval.node->setIdent( yyvsp[-3].ident );
          ;
    break;}
case 98:
#line 712 "yacc.yy"
{
            if ( yyvsp[0].node != 0 )
            {
                yyval.node = new KSParseNode( exports, yyvsp[0].node );
            }
          ;
    break;}
case 99:
#line 719 "yacc.yy"
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
#line 742 "yacc.yy"
{
            yyval.node = yyvsp[-1].node;
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
#line 757 "yacc.yy"
{
            yyval.node = new KSParseNode( t_struct_members );
            yyval.node->setIdent( yyvsp[0].ident );
          ;
    break;}
case 104:
#line 762 "yacc.yy"
{
            yyval.node = new KSParseNode( t_struct_members, yyvsp[0].node );
            yyval.node->setIdent( yyvsp[-2].ident );
          ;
    break;}
case 105:
#line 770 "yacc.yy"
{
            yyval.node = new KSParseNode( func_dcl );
            yyval.node->setBranch( 2, yyvsp[0].node );
            yyval.node->setIdent( yyvsp[-4].ident );
          ;
    break;}
case 106:
#line 776 "yacc.yy"
{
            yyval.node = new KSParseNode( func_dcl, yyvsp[-3].node, yyvsp[0].node );
            yyval.node->setIdent( yyvsp[-5].ident );
          ;
    break;}
case 107:
#line 784 "yacc.yy"
{
            yyval.node = new KSParseNode( func_params, yyvsp[0].node );
          ;
    break;}
case 108:
#line 788 "yacc.yy"
{
            yyval.node = new KSParseNode( func_params, yyvsp[-2].node, yyvsp[0].node );
          ;
    break;}
case 109:
#line 795 "yacc.yy"
{
            yyval.node = new KSParseNode( func_param_in );
            yyval.node->setIdent( yyvsp[0].ident );
          ;
    break;}
case 110:
#line 800 "yacc.yy"
{
            yyval.node = new KSParseNode( func_param_in, yyvsp[0].node );
            yyval.node->setIdent( yyvsp[-2].ident );
          ;
    break;}
case 111:
#line 805 "yacc.yy"
{
            yyval.node = new KSParseNode( func_param_out );
            yyval.node->setIdent( yyvsp[0].ident );
          ;
    break;}
case 112:
#line 810 "yacc.yy"
{
            yyval.node = new KSParseNode( func_param_inout );
            yyval.node->setIdent( yyvsp[0].ident );
          ;
    break;}
case 113:
#line 818 "yacc.yy"
{
            yyval.node = NULL;
          ;
    break;}
case 114:
#line 822 "yacc.yy"
{
            yyval.node = yyvsp[0].node;
          ;
    break;}
case 115:
#line 829 "yacc.yy"
{
            yyval.node = new KSParseNode( func_lines, yyvsp[0].node );
          ;
    break;}
case 116:
#line 833 "yacc.yy"
{
            yyval.node = new KSParseNode( func_lines, yyvsp[-1].node );
            yyval.node->setBranch( 2, yyvsp[0].node );
          ;
    break;}
case 117:
#line 841 "yacc.yy"
{
            yyval.node = yyvsp[-1].node;
          ;
    break;}
case 118:
#line 845 "yacc.yy"
{
            yyval.node = new KSParseNode( t_raise, yyvsp[-3].node, yyvsp[-1].node );
          ;
    break;}
case 119:
#line 849 "yacc.yy"
{
            yyval.node = new KSParseNode( t_return );
          ;
    break;}
case 120:
#line 853 "yacc.yy"
{
            yyval.node = new KSParseNode( t_return, yyvsp[-1].node );
          ;
    break;}
case 121:
#line 857 "yacc.yy"
{
            yyval.node = yyvsp[0].node;
          ;
    break;}
case 122:
#line 861 "yacc.yy"
{
            yyval.node = new KSParseNode( t_try, yyvsp[-2].node, yyvsp[0].node );
          ;
    break;}
case 123:
#line 865 "yacc.yy"
{
            yyval.node = new KSParseNode( t_scope, yyvsp[-1].node );
          ;
    break;}
case 124:
#line 872 "yacc.yy"
{
            yyval.node = yyvsp[0].node;
          ;
    break;}
case 125:
#line 876 "yacc.yy"
{
            yyval.node = yyvsp[-1].node;
            yyval.node->setBranch( 4, yyvsp[0].node );
          ;
    break;}
case 126:
#line 885 "yacc.yy"
{
            yyval.node = new KSParseNode( t_catch, yyvsp[-6].node, yyvsp[-1].node );
            yyval.node->setIdent( yyvsp[-4].ident );
          ;
    break;}
case 127:
#line 891 "yacc.yy"
{
            KSParseNode* x = new KSParseNode( scoped_name );
            x->setIdent( yyvsp[-6].ident );
            yyval.node = new KSParseNode( t_catch_default, x, yyvsp[-1].node );
            yyval.node->setIdent( yyvsp[-4].ident );
          ;
    break;}
case 128:
#line 901 "yacc.yy"
{
            yyval.node = new KSParseNode( const_dcl, yyvsp[0].node );
            yyval.node->setIdent( yyvsp[-2].ident );
          ;
    break;}
case 129:
#line 909 "yacc.yy"
{
            yyval.node = new KSParseNode( t_while, yyvsp[-1].node, yyvsp[0].node );
          ;
    break;}
case 130:
#line 913 "yacc.yy"
{
            yyval.node = new KSParseNode( t_for, yyvsp[-6].node, yyvsp[-4].node, yyvsp[-2].node, yyvsp[0].node );
          ;
    break;}
case 131:
#line 917 "yacc.yy"
{
            yyval.node = new KSParseNode( t_do, yyvsp[-2].node, yyvsp[-1].node );
          ;
    break;}
case 132:
#line 921 "yacc.yy"
{
            yyval.node = new KSParseNode( t_if, yyvsp[-3].node, yyvsp[-1].node, yyvsp[0].node );
          ;
    break;}
case 133:
#line 925 "yacc.yy"
{
            yyval.node = new KSParseNode( t_foreach, yyvsp[-2].node, yyvsp[0].node );
            yyval.node->setIdent( yyvsp[-4].ident );
          ;
    break;}
case 134:
#line 930 "yacc.yy"
{
            /* We set $9 twice to indicate thet this is the foreach for maps */
            yyval.node = new KSParseNode( t_foreach, yyvsp[-2].node, yyvsp[0].node, yyvsp[0].node );
            yyval.node->setIdent( yyvsp[-6].ident );
            yyval.node->setStringLiteral( yyvsp[-4].ident );
          ;
    break;}
case 135:
#line 940 "yacc.yy"
{
            yyval.node = NULL;
          ;
    break;}
case 136:
#line 944 "yacc.yy"
{
            yyval.node = new KSParseNode( t_if, yyvsp[-3].node, yyvsp[-1].node, yyvsp[0].node );
          ;
    break;}
case 137:
#line 948 "yacc.yy"
{
            yyval.node = yyvsp[0].node;
          ;
    break;}
case 138:
#line 955 "yacc.yy"
{
            yyval.node = yyvsp[-1].node;
          ;
    break;}
case 139:
#line 962 "yacc.yy"
{
            yyval.node = new KSParseNode( t_scope, yyvsp[-1].node );
          ;
    break;}
}

#line 705 "/usr/share/bison/bison.simple"


  yyvsp -= yylen;
  yyssp -= yylen;
#if YYLSP_NEEDED
  yylsp -= yylen;
#endif

#if YYDEBUG
  if (yydebug)
    {
      short *yyssp1 = yyss - 1;
      YYFPRINTF (stderr, "state stack now");
      while (yyssp1 != yyssp)
	YYFPRINTF (stderr, " %d", *++yyssp1);
      YYFPRINTF (stderr, "\n");
    }
#endif

  *++yyvsp = yyval;
#if YYLSP_NEEDED
  *++yylsp = yyloc;
#endif

  /* Now `shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */

  yyn = yyr1[yyn];

  yystate = yypgoto[yyn - YYNTBASE] + *yyssp;
  if (yystate >= 0 && yystate <= YYLAST && yycheck[yystate] == *yyssp)
    yystate = yytable[yystate];
  else
    yystate = yydefgoto[yyn - YYNTBASE];

  goto yynewstate;


/*------------------------------------.
| yyerrlab -- here on detecting error |
`------------------------------------*/
yyerrlab:
  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;

#ifdef YYERROR_VERBOSE
      yyn = yypact[yystate];

      if (yyn > YYFLAG && yyn < YYLAST)
	{
	  YYSIZE_T yysize = 0;
	  char *yymsg;
	  int yyx, yycount;

	  yycount = 0;
	  /* Start YYX at -YYN if negative to avoid negative indexes in
	     YYCHECK.  */
	  for (yyx = yyn < 0 ? -yyn : 0;
	       yyx < (int) (sizeof (yytname) / sizeof (char *)); yyx++)
	    if (yycheck[yyx + yyn] == yyx)
	      yysize += yystrlen (yytname[yyx]) + 15, yycount++;
	  yysize += yystrlen ("parse error, unexpected ") + 1;
	  yysize += yystrlen (yytname[YYTRANSLATE (yychar)]);
	  yymsg = (char *) YYSTACK_ALLOC (yysize);
	  if (yymsg != 0)
	    {
	      char *yyp = yystpcpy (yymsg, "parse error, unexpected ");
	      yyp = yystpcpy (yyp, yytname[YYTRANSLATE (yychar)]);

	      if (yycount < 5)
		{
		  yycount = 0;
		  for (yyx = yyn < 0 ? -yyn : 0;
		       yyx < (int) (sizeof (yytname) / sizeof (char *));
		       yyx++)
		    if (yycheck[yyx + yyn] == yyx)
		      {
			const char *yyq = ! yycount ? ", expecting " : " or ";
			yyp = yystpcpy (yyp, yyq);
			yyp = yystpcpy (yyp, yytname[yyx]);
			yycount++;
		      }
		}
	      yyerror (yymsg);
	      YYSTACK_FREE (yymsg);
	    }
	  else
	    yyerror ("parse error; also virtual memory exhausted");
	}
      else
#endif /* defined (YYERROR_VERBOSE) */
	yyerror ("parse error");
    }
  goto yyerrlab1;


/*--------------------------------------------------.
| yyerrlab1 -- error raised explicitly by an action |
`--------------------------------------------------*/
yyerrlab1:
  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse lookahead token after an
	 error, discard it.  */

      /* return failure if at end of input */
      if (yychar == YYEOF)
	YYABORT;
      YYDPRINTF ((stderr, "Discarding token %d (%s).\n",
		  yychar, yytname[yychar1]));
      yychar = YYEMPTY;
    }

  /* Else will try to reuse lookahead token after shifting the error
     token.  */

  yyerrstatus = 3;		/* Each real token shifted decrements this */

  goto yyerrhandle;


/*-------------------------------------------------------------------.
| yyerrdefault -- current state does not do anything special for the |
| error token.                                                       |
`-------------------------------------------------------------------*/
yyerrdefault:
#if 0
  /* This is wrong; only states that explicitly want error tokens
     should shift them.  */

  /* If its default is to accept any token, ok.  Otherwise pop it.  */
  yyn = yydefact[yystate];
  if (yyn)
    goto yydefault;
#endif


/*---------------------------------------------------------------.
| yyerrpop -- pop the current state because it cannot handle the |
| error token                                                    |
`---------------------------------------------------------------*/
yyerrpop:
  if (yyssp == yyss)
    YYABORT;
  yyvsp--;
  yystate = *--yyssp;
#if YYLSP_NEEDED
  yylsp--;
#endif

#if YYDEBUG
  if (yydebug)
    {
      short *yyssp1 = yyss - 1;
      YYFPRINTF (stderr, "Error: state stack now");
      while (yyssp1 != yyssp)
	YYFPRINTF (stderr, " %d", *++yyssp1);
      YYFPRINTF (stderr, "\n");
    }
#endif

/*--------------.
| yyerrhandle.  |
`--------------*/
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

  YYDPRINTF ((stderr, "Shifting error token, "));

  *++yyvsp = yylval;
#if YYLSP_NEEDED
  *++yylsp = yylloc;
#endif

  yystate = yyn;
  goto yynewstate;


/*-------------------------------------.
| yyacceptlab -- YYACCEPT comes here.  |
`-------------------------------------*/
yyacceptlab:
  yyresult = 0;
  goto yyreturn;

/*-----------------------------------.
| yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
yyabortlab:
  yyresult = 1;
  goto yyreturn;

/*---------------------------------------------.
| yyoverflowab -- parser overflow comes here.  |
`---------------------------------------------*/
yyoverflowlab:
  yyerror ("parser stack overflow");
  yyresult = 2;
  /* Fall through.  */

yyreturn:
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif
  return yyresult;
}
#line 966 "yacc.yy"


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
