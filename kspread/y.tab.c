#ifndef lint
/*static char yysccsid[] = "from: @(#)yaccpar	1.9 (Berkeley) 02/21/93";*/
static char yyrcsid[] = "$Id$";
#endif
#define YYBYACC 1
#define YYMAJOR 1
#define YYMINOR 9
#define yyclearin (yychar=(-1))
#define yyerrok (yyerrflag=0)
#define YYRECOVERING (yyerrflag!=0)
#define YYPREFIX "yy"
#line 2 "yacc.y"
#include <math.h>
#include "kspread_calcerr.h"
#include <stdio.h>

extern void setResult( double _res );
extern void setResultBool( char _res );
extern void setError( int _errno, const char *_txt );

extern int funcDbl( const char *_name, void* _args, double* _res );
extern int funcBool( const char *_name, void* _args, char* _res );
extern void* newArgList();
extern void addDbl( void* _args, double _v );
extern void addBool( void* _args, char _v );
extern void addRange( void* _args, void* _range );

extern void initFlex( const char *_code );

int yyparse();
extern int yylex();

#line 24 "yacc.y"
typedef union
{
     char b;
     double dbl;
     char* name;
     void* range;
     void* param;
} YYSTYPE;
#line 43 "y.tab.c"
#define NUM 257
#define RANGE 258
#define BOOL 259
#define ID 260
#define WENN 261
#define LEQ 262
#define GEQ 263
#define NEQ 264
#define NOT 265
#define OR 266
#define AND 267
#define YYERRCODE 256
short yylhs[] = {                                        -1,
    0,    0,    0,    1,    1,    1,    2,    2,    2,    3,
    3,    4,    4,    5,    5,    5,    5,    5,   11,   11,
   11,   11,   11,   11,    6,    6,    7,    7,    8,    8,
    9,    9,    9,    9,    9,    9,    9,    9,    9,    9,
    9,   10,   10,   10,   10,   10,
};
short yylen[] = {                                         2,
    0,    1,    1,    3,    3,    1,    3,    3,    1,    2,
    1,    3,    1,    3,    8,    4,    3,    1,    3,    3,
    3,    1,    1,    1,    3,    1,    3,    1,    2,    1,
    1,    3,    3,    3,    3,    3,    3,    3,    3,    3,
    3,    1,    3,    8,    4,    3,
};
short yydefred[] = {                                      0,
   18,   42,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    9,    0,   13,    0,    0,   28,   30,    0,    0,
    0,    0,    0,    0,   29,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,   24,   17,    0,
    0,    0,   46,    0,    0,    0,   14,   43,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    7,    8,   12,
    0,   27,    0,    0,   41,   39,   40,   16,    0,   45,
    0,    0,   21,    0,    0,    0,    0,    0,    0,    0,
    0,   15,   44,
};
short yydgoto[] = {                                       9,
   24,   11,   12,   13,   14,   51,   16,   17,   18,   19,
   52,
};
short yysindex[] = {                                    113,
    0,    0,  -18,   69,  -36,  -13,   -7,  113,    0,  -32,
  -30,    0,  -75,    0, -242, -232,    0,    0,  -35,  -20,
   72,  113,  113,  -32,    0,   25,   36,  -75,  130, -111,
   -7,   -7,   -7,   -7,   -7,   -7,   -7,   -7,   -7,   -7,
   -7,  -13,  113,  113,  -92,  -92,  -92,    0,    0,  -32,
 -242,  -25,    0,   22,  -57,  -54,    0,    0,  109,  109,
  109,  -30,  -30,  109,  109,  109,  109,    0,    0,    0,
 -232,    0,  -83,  -82,    0,    0,    0,    0,   83,    0,
   -7,  113,    0,  -32, -242,   65,  -53,   -7,  113,  133,
 -107,    0,    0,
};
short yyrindex[] = {                                    129,
    0,    0,    0,    0,    0,    0,    0,    0,    0,  136,
   23,    0,    1,    0,  137,  101,    0,    0,   15,    0,
    0,    0,    0,    0,    0,    0,    0,   10,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,   63,
   66,    0,    0,    0,    0,    0,    0,    0,   37,   39,
   47,   32,   54,   59,   79,   86,   90,    0,    0,    0,
  102,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,   80,   91,    0,    0,    0,    0,    0,
    0,    0,    0,
};
short yygindex[] = {                                      0,
  355,  -27,    9,  124,   99,  111,  103,  100,  146,  135,
  142,
};
#define YYTABLESIZE 443
short yytable[] = {                                      47,
   11,   81,   37,    7,   82,   89,   62,   63,    6,   10,
   34,   40,   35,   58,   31,   78,   41,   93,   42,    7,
   49,   20,    6,   43,    6,   46,    7,   38,   36,   39,
    8,    4,    7,   79,   44,   11,   37,    6,   38,   21,
   23,   11,   11,   11,   10,   11,   34,   11,   68,   69,
   10,   10,   10,    5,   10,   31,   10,    6,   32,   11,
   11,   11,   11,    6,   20,    6,    4,    6,   10,   10,
   10,   10,    4,   31,    4,   22,    4,   37,   33,   38,
   79,    6,    6,    6,    6,   35,    8,   34,    5,   36,
    4,    4,    4,    4,    5,   37,    5,   38,    5,   32,
   26,   25,    8,   22,   21,   34,   23,   34,   22,   35,
   15,    7,    5,    5,    5,    5,    6,   32,   30,   33,
   19,   22,    7,   88,   23,   11,   35,    6,    1,   28,
   36,   20,   55,   56,   10,    2,    3,   33,   19,   31,
   70,   26,   25,   72,   35,   71,   80,    6,   36,   20,
   25,   34,    7,   35,   43,    0,    4,    6,   43,   26,
   25,   37,   54,   38,    0,    0,    2,   73,   74,    0,
   57,   34,   34,   92,   35,   34,    0,   35,    5,   75,
   76,   77,    0,   32,    0,    0,    0,   22,    0,   85,
   23,   23,   87,    0,    8,    0,   53,    0,    0,   91,
    0,    0,    0,   33,   19,    8,    0,    0,   43,    0,
   35,   43,   43,    0,   36,   20,    0,    0,    0,    0,
    1,    0,    2,    3,    4,   26,   25,    0,   45,   31,
   32,   33,    0,    0,    0,    8,    1,   48,    2,    3,
    4,    0,    0,    1,    5,    0,   26,   27,    0,    1,
    0,    0,   26,   27,    0,    0,    0,    0,    0,    0,
    0,    0,   11,   11,   11,    0,   11,   11,    0,    0,
    0,   10,   10,   10,    0,   10,   10,    0,    0,    0,
   31,   31,    0,    0,    6,    6,    6,    0,    6,    6,
    0,    0,    0,    4,    4,    4,    0,    4,    4,    0,
    0,    0,   37,   37,   38,   38,    0,    0,    0,    0,
    0,    0,   34,   34,    0,    5,    5,    5,    0,    5,
    5,    0,    0,    0,   32,   32,    0,    0,    1,   48,
    2,    3,    4,    0,    0,    0,    5,    0,    0,    1,
   83,    2,    3,    4,   33,   33,    0,    5,    0,    0,
    0,   35,   35,    0,   10,   36,   36,    0,    0,    0,
    0,   29,    0,    0,    0,    0,   26,   25,    0,    1,
    0,    2,    3,    4,   50,   50,    0,    5,    0,    0,
    0,    0,    0,    0,    0,   59,   60,   61,    0,    0,
   64,   65,   66,   67,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,   84,    0,   86,    0,    0,    0,    0,
    0,    0,   90,
};
short yycheck[] = {                                      35,
    0,   59,   35,   40,   59,   59,   34,   35,   45,    0,
   43,   42,   45,  125,    0,   41,   47,  125,   94,   40,
   41,   40,    0,  266,   45,   61,   40,   60,   61,   62,
  123,    0,   40,   59,  267,   35,    0,   45,    0,  123,
  123,   41,   42,   43,   35,   45,    0,   47,   40,   41,
   41,   42,   43,    0,   45,   41,   47,   35,    0,   59,
   60,   61,   62,   41,   40,   43,   35,   45,   59,   60,
   61,   62,   41,   59,   43,   40,   45,   41,    0,   41,
   59,   59,   60,   61,   62,    0,  123,   41,   35,    0,
   59,   60,   61,   62,   41,   59,   43,   59,   45,   41,
    0,    0,  123,   41,  123,   59,   41,   43,   40,   45,
    0,   40,   59,   60,   61,   62,   45,   59,    8,   41,
   41,   59,   40,   59,   59,  125,   41,   45,    0,    6,
   41,   41,   22,   23,  125,    0,    0,   59,   59,  125,
   42,   41,   41,   44,   59,   43,  125,  125,   59,   59,
    5,   43,   40,   45,  266,   -1,  125,   45,  266,   59,
   59,  125,   21,  125,   -1,   -1,  259,  260,  261,   -1,
   41,  125,   43,   41,   45,   43,   -1,   45,  125,   45,
   46,   47,   -1,  125,   -1,   -1,   -1,  125,   -1,   79,
  125,  123,   82,   -1,  123,   -1,  125,   -1,   -1,   89,
   -1,   -1,   -1,  125,  125,  123,   -1,   -1,  266,   -1,
  125,  266,  266,   -1,  125,  125,   -1,   -1,   -1,   -1,
  257,   -1,  259,  260,  261,  125,  125,   -1,  264,  262,
  263,  264,   -1,   -1,   -1,  123,  257,  258,  259,  260,
  261,   -1,   -1,  257,  265,   -1,  260,  261,   -1,  257,
   -1,   -1,  260,  261,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,  262,  263,  264,   -1,  266,  267,   -1,   -1,
   -1,  262,  263,  264,   -1,  266,  267,   -1,   -1,   -1,
  266,  267,   -1,   -1,  262,  263,  264,   -1,  266,  267,
   -1,   -1,   -1,  262,  263,  264,   -1,  266,  267,   -1,
   -1,   -1,  266,  267,  266,  267,   -1,   -1,   -1,   -1,
   -1,   -1,  266,  267,   -1,  262,  263,  264,   -1,  266,
  267,   -1,   -1,   -1,  266,  267,   -1,   -1,  257,  258,
  259,  260,  261,   -1,   -1,   -1,  265,   -1,   -1,  257,
  258,  259,  260,  261,  266,  267,   -1,  265,   -1,   -1,
   -1,  266,  267,   -1,    0,  266,  267,   -1,   -1,   -1,
   -1,    7,   -1,   -1,   -1,   -1,  266,  266,   -1,  257,
   -1,  259,  260,  261,   20,   21,   -1,  265,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   31,   32,   33,   -1,   -1,
   36,   37,   38,   39,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   79,   -1,   81,   -1,   -1,   -1,   -1,
   -1,   -1,   88,
};
#define YYFINAL 9
#ifndef YYDEBUG
#define YYDEBUG 0
#endif
#define YYMAXTOKEN 267
#if YYDEBUG
char *yyname[] = {
"end-of-file",0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,"'#'",0,0,0,0,"'('","')'","'*'","'+'",0,"'-'",0,"'/'",0,0,0,0,0,0,0,0,0,0,0,
"';'","'<'","'='","'>'",0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,"'^'",0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,"'{'",0,
"'}'",0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,"NUM","RANGE","BOOL","ID","WENN","LEQ","GEQ","NEQ",
"NOT","OR","AND",
};
char *yyrule[] = {
"$accept : input",
"input :",
"input : exp",
"input : bool_exp",
"exp : exp '+' mul",
"exp : exp '-' mul",
"exp : mul",
"mul : mul '*' unary",
"mul : mul '/' unary",
"mul : unary",
"unary : '-' power",
"unary : power",
"power : power '^' atom",
"power : atom",
"atom : '(' exp ')'",
"atom : WENN '(' bool_exp ';' exp ';' exp ')'",
"atom : ID '(' args ')'",
"atom : ID '(' ')'",
"atom : NUM",
"args : args ';' exp",
"args : args ';' bool_exp",
"args : args ';' RANGE",
"args : exp",
"args : bool_exp",
"args : RANGE",
"bool_exp : bool_exp OR bool_and",
"bool_exp : bool_and",
"bool_and : bool_and AND bool_unary",
"bool_and : bool_unary",
"bool_unary : NOT bool_simple",
"bool_unary : bool_simple",
"bool_simple : bool_atom",
"bool_simple : exp '=' exp",
"bool_simple : exp '#' exp",
"bool_simple : exp NEQ exp",
"bool_simple : exp '<' exp",
"bool_simple : exp '>' exp",
"bool_simple : exp LEQ exp",
"bool_simple : exp GEQ exp",
"bool_simple : bool_atom '=' bool_atom",
"bool_simple : bool_atom '#' bool_atom",
"bool_simple : bool_atom NEQ bool_atom",
"bool_atom : BOOL",
"bool_atom : '{' bool_exp '}'",
"bool_atom : WENN '{' bool_exp ';' bool_exp ';' bool_exp '}'",
"bool_atom : ID '{' args '}'",
"bool_atom : ID '{' '}'",
};
#endif
#ifdef YYSTACKSIZE
#undef YYMAXDEPTH
#define YYMAXDEPTH YYSTACKSIZE
#else
#ifdef YYMAXDEPTH
#define YYSTACKSIZE YYMAXDEPTH
#else
#define YYSTACKSIZE 500
#define YYMAXDEPTH 500
#endif
#endif
int yydebug;
int yynerrs;
int yyerrflag;
int yychar;
short *yyssp;
YYSTYPE *yyvsp;
YYSTYPE yyval;
YYSTYPE yylval;
short yyss[YYSTACKSIZE];
YYSTYPE yyvs[YYSTACKSIZE];
#define yystacksize YYSTACKSIZE
#line 129 "yacc.y"

void yyerror ( char *s )  /* Called by yyparse on error */
{
  printf ("ERROR: %s\n", s);
  setError( ERR_SYNTAX, "" );
}

void mainParse( const char *_code )
{
  initFlex( _code );
  yyparse();
}

#line 312 "y.tab.c"
#define YYABORT goto yyabort
#define YYREJECT goto yyabort
#define YYACCEPT goto yyaccept
#define YYERROR goto yyerrlab
int
#if defined(__STDC__)
yyparse(void)
#else
yyparse()
#endif
{
    register int yym, yyn, yystate;
#if YYDEBUG
    register char *yys;
    extern char *getenv();

    if (yys = getenv("YYDEBUG"))
    {
        yyn = *yys;
        if (yyn >= '0' && yyn <= '9')
            yydebug = yyn - '0';
    }
#endif

    yynerrs = 0;
    yyerrflag = 0;
    yychar = (-1);

    yyssp = yyss;
    yyvsp = yyvs;
    *yyssp = yystate = 0;

yyloop:
    if ((yyn = yydefred[yystate]) != 0) goto yyreduce;
    if (yychar < 0)
    {
        if ((yychar = yylex()) < 0) yychar = 0;
#if YYDEBUG
        if (yydebug)
        {
            yys = 0;
            if (yychar <= YYMAXTOKEN) yys = yyname[yychar];
            if (!yys) yys = "illegal-symbol";
            printf("%sdebug: state %d, reading %d (%s)\n",
                    YYPREFIX, yystate, yychar, yys);
        }
#endif
    }
    if ((yyn = yysindex[yystate]) && (yyn += yychar) >= 0 &&
            yyn <= YYTABLESIZE && yycheck[yyn] == yychar)
    {
#if YYDEBUG
        if (yydebug)
            printf("%sdebug: state %d, shifting to state %d\n",
                    YYPREFIX, yystate, yytable[yyn]);
#endif
        if (yyssp >= yyss + yystacksize - 1)
        {
            goto yyoverflow;
        }
        *++yyssp = yystate = yytable[yyn];
        *++yyvsp = yylval;
        yychar = (-1);
        if (yyerrflag > 0)  --yyerrflag;
        goto yyloop;
    }
    if ((yyn = yyrindex[yystate]) && (yyn += yychar) >= 0 &&
            yyn <= YYTABLESIZE && yycheck[yyn] == yychar)
    {
        yyn = yytable[yyn];
        goto yyreduce;
    }
    if (yyerrflag) goto yyinrecovery;
    yyerror("syntax error");
#ifdef lint
    goto yyerrlab;
#endif
yyerrlab:
    ++yynerrs;
yyinrecovery:
    if (yyerrflag < 3)
    {
        yyerrflag = 3;
        for (;;)
        {
            if ((yyn = yysindex[*yyssp]) && (yyn += YYERRCODE) >= 0 &&
                    yyn <= YYTABLESIZE && yycheck[yyn] == YYERRCODE)
            {
#if YYDEBUG
                if (yydebug)
                    printf("%sdebug: state %d, error recovery shifting\
 to state %d\n", YYPREFIX, *yyssp, yytable[yyn]);
#endif
                if (yyssp >= yyss + yystacksize - 1)
                {
                    goto yyoverflow;
                }
                *++yyssp = yystate = yytable[yyn];
                *++yyvsp = yylval;
                goto yyloop;
            }
            else
            {
#if YYDEBUG
                if (yydebug)
                    printf("%sdebug: error recovery discarding state %d\n",
                            YYPREFIX, *yyssp);
#endif
                if (yyssp <= yyss) goto yyabort;
                --yyssp;
                --yyvsp;
            }
        }
    }
    else
    {
        if (yychar == 0) goto yyabort;
#if YYDEBUG
        if (yydebug)
        {
            yys = 0;
            if (yychar <= YYMAXTOKEN) yys = yyname[yychar];
            if (!yys) yys = "illegal-symbol";
            printf("%sdebug: state %d, error recovery discards token %d (%s)\n",
                    YYPREFIX, yystate, yychar, yys);
        }
#endif
        yychar = (-1);
        goto yyloop;
    }
yyreduce:
#if YYDEBUG
    if (yydebug)
        printf("%sdebug: state %d, reducing by rule %d (%s)\n",
                YYPREFIX, yystate, yyn, yyrule[yyn]);
#endif
    yym = yylen[yyn];
    yyval = yyvsp[1-yym];
    switch (yyn)
    {
case 1:
#line 59 "yacc.y"
{ setResult( 0.0 ); }
break;
case 2:
#line 60 "yacc.y"
{ setResult( yyvsp[0].dbl ); }
break;
case 3:
#line 61 "yacc.y"
{ printf("BOOL_input %i\n",(int)yyvsp[0].b); setResultBool( yyvsp[0].b ); }
break;
case 4:
#line 64 "yacc.y"
{ yyval.dbl = yyvsp[-2].dbl + yyvsp[0].dbl; }
break;
case 5:
#line 65 "yacc.y"
{ yyval.dbl = yyvsp[-2].dbl - yyvsp[0].dbl; }
break;
case 6:
#line 66 "yacc.y"
{ yyval.dbl = yyvsp[0].dbl; }
break;
case 7:
#line 69 "yacc.y"
{ yyval.dbl = yyvsp[-2].dbl * yyvsp[0].dbl; }
break;
case 8:
#line 70 "yacc.y"
{ yyval.dbl = yyvsp[-2].dbl / yyvsp[0].dbl; }
break;
case 9:
#line 71 "yacc.y"
{ yyval.dbl = yyvsp[0].dbl; }
break;
case 10:
#line 73 "yacc.y"
{ yyval.dbl = -yyvsp[0].dbl; }
break;
case 11:
#line 74 "yacc.y"
{ yyval.dbl = yyvsp[0].dbl; }
break;
case 12:
#line 77 "yacc.y"
{ yyval.dbl = pow( yyvsp[-2].dbl, yyvsp[0].dbl ); }
break;
case 13:
#line 78 "yacc.y"
{ yyval.dbl = yyvsp[0].dbl; }
break;
case 14:
#line 81 "yacc.y"
{ yyval.dbl = yyvsp[-1].dbl; }
break;
case 15:
#line 82 "yacc.y"
{ if ( yyvsp[-5].b ) yyval.dbl = yyvsp[-3].dbl; else yyval.dbl = yyvsp[-1].dbl; }
break;
case 16:
#line 83 "yacc.y"
{ double res; int e = funcDbl( yyvsp[-3].name, yyvsp[-1].param, &res ); if ( e != 0 ) setError( e, yyvsp[-3].name ); yyval.dbl = res; }
break;
case 17:
#line 84 "yacc.y"
{ double res; int e = funcDbl( yyvsp[-2].name, newArgList(), &res ); if ( e != 0 ) setError( e, yyvsp[-2].name ); yyval.dbl = res; }
break;
case 18:
#line 85 "yacc.y"
{ yyval.dbl = yyvsp[0].dbl; }
break;
case 19:
#line 88 "yacc.y"
{ addDbl( yyvsp[-2].param, yyvsp[0].dbl ); yyval.param = yyvsp[-2].param; }
break;
case 20:
#line 89 "yacc.y"
{ addBool( yyvsp[-2].param, yyvsp[0].b ); yyval.param = yyvsp[-2].param; }
break;
case 21:
#line 90 "yacc.y"
{ addRange( yyvsp[-2].param, yyvsp[0].range ); yyval.param = yyvsp[-2].param; }
break;
case 22:
#line 91 "yacc.y"
{ void* l = newArgList(); addDbl( l, yyvsp[0].dbl ); yyval.param = l; }
break;
case 23:
#line 92 "yacc.y"
{ void* l = newArgList(); addBool( l, yyvsp[0].b ); yyval.param = l; }
break;
case 24:
#line 93 "yacc.y"
{ void* l = newArgList(); addRange( l, yyvsp[0].range ); yyval.param = l; }
break;
case 25:
#line 96 "yacc.y"
{ yyval.b = yyvsp[-2].b || yyvsp[0].b; }
break;
case 26:
#line 97 "yacc.y"
{ printf("BOOL_exp %i\n",(int)yyvsp[0].b); yyval.b = yyvsp[0].b; }
break;
case 27:
#line 100 "yacc.y"
{ yyval.b = yyvsp[-2].b && yyvsp[0].b; }
break;
case 28:
#line 101 "yacc.y"
{ yyval.b = yyvsp[0].b; }
break;
case 29:
#line 104 "yacc.y"
{ yyval.b = !yyvsp[0].b; }
break;
case 30:
#line 105 "yacc.y"
{ yyval.b = yyvsp[0].b; }
break;
case 31:
#line 108 "yacc.y"
{ yyval.b = yyvsp[0].b; }
break;
case 32:
#line 109 "yacc.y"
{ yyval.b = ( yyvsp[-2].dbl == yyvsp[0].dbl ); }
break;
case 33:
#line 110 "yacc.y"
{ yyval.b = ( yyvsp[-2].dbl != yyvsp[0].dbl ); }
break;
case 34:
#line 111 "yacc.y"
{ yyval.b = ( yyvsp[-2].dbl != yyvsp[0].dbl ); }
break;
case 35:
#line 112 "yacc.y"
{ yyval.b = ( yyvsp[-2].dbl < yyvsp[0].dbl ); }
break;
case 36:
#line 113 "yacc.y"
{ yyval.b = ( yyvsp[-2].dbl > yyvsp[0].dbl ); }
break;
case 37:
#line 114 "yacc.y"
{ yyval.b = ( yyvsp[-2].dbl <= yyvsp[0].dbl ); }
break;
case 38:
#line 115 "yacc.y"
{ yyval.b = ( yyvsp[-2].dbl >= yyvsp[0].dbl ); }
break;
case 39:
#line 116 "yacc.y"
{ yyval.b = ( yyvsp[-2].b == yyvsp[0].b ); }
break;
case 40:
#line 117 "yacc.y"
{ yyval.b = ( yyvsp[-2].b != yyvsp[0].b ); }
break;
case 41:
#line 118 "yacc.y"
{ yyval.b = ( yyvsp[-2].b != yyvsp[0].b ); }
break;
case 42:
#line 121 "yacc.y"
{ printf("BOOL %i\n",(int)yyvsp[0].b); yyval.b = yyvsp[0].b; }
break;
case 43:
#line 122 "yacc.y"
{ yyval.b = yyvsp[-1].b; }
break;
case 44:
#line 123 "yacc.y"
{ if ( yyvsp[-5].b ) yyval.b = yyvsp[-3].b; else yyval.b = yyvsp[-1].b; }
break;
case 45:
#line 124 "yacc.y"
{ char res; int e = funcBool( yyvsp[-3].name, yyvsp[-1].param, &res ); if ( e != 0 ) setError( e, yyvsp[-3].name ); yyval.b = res; }
break;
case 46:
#line 125 "yacc.y"
{ char res; int e = funcBool( yyvsp[-2].name, newArgList(), &res ); if ( e != 0 ) setError( e, yyvsp[-2].name ); yyval.b = res; }
break;
#line 637 "y.tab.c"
    }
    yyssp -= yym;
    yystate = *yyssp;
    yyvsp -= yym;
    yym = yylhs[yyn];
    if (yystate == 0 && yym == 0)
    {
#if YYDEBUG
        if (yydebug)
            printf("%sdebug: after reduction, shifting from state 0 to\
 state %d\n", YYPREFIX, YYFINAL);
#endif
        yystate = YYFINAL;
        *++yyssp = YYFINAL;
        *++yyvsp = yyval;
        if (yychar < 0)
        {
            if ((yychar = yylex()) < 0) yychar = 0;
#if YYDEBUG
            if (yydebug)
            {
                yys = 0;
                if (yychar <= YYMAXTOKEN) yys = yyname[yychar];
                if (!yys) yys = "illegal-symbol";
                printf("%sdebug: state %d, reading %d (%s)\n",
                        YYPREFIX, YYFINAL, yychar, yys);
            }
#endif
        }
        if (yychar == 0) goto yyaccept;
        goto yyloop;
    }
    if ((yyn = yygindex[yym]) && (yyn += yystate) >= 0 &&
            yyn <= YYTABLESIZE && yycheck[yyn] == yystate)
        yystate = yytable[yyn];
    else
        yystate = yydgoto[yym];
#if YYDEBUG
    if (yydebug)
        printf("%sdebug: after reduction, shifting from state %d \
to state %d\n", YYPREFIX, *yyssp, yystate);
#endif
    if (yyssp >= yyss + yystacksize - 1)
    {
        goto yyoverflow;
    }
    *++yyssp = yystate;
    *++yyvsp = yyval;
    goto yyloop;
yyoverflow:
    yyerror("yacc stack overflow");
yyabort:
    return (1);
yyaccept:
    return (0);
}
