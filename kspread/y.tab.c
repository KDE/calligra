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
extern void setError( int _errno, const char *_txt );

extern int funcDbl( const char *_name, void* _args, double* _res );
extern void* newArgList();
extern void addDbl( void* _args, double _v );
extern void addRange( void* _args, void* _range );

extern void initFlex( const char *_code );
#line 17 "yacc.y"
typedef union
{
     double dbl;
     char b;
     char* name;
     void* range;
     void* param;
} YYSTYPE;
#line 36 "y.tab.c"
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
    0,    0,    1,    1,    1,    2,    2,    2,    3,    3,
    4,    4,    5,    5,    5,    5,    5,   10,   10,   10,
   10,    6,    6,    7,    7,    8,    8,    9,    9,    9,
    9,    9,    9,    9,    9,
};
short yylen[] = {                                         2,
    0,    1,    3,    3,    1,    3,    3,    1,    2,    1,
    3,    1,    3,    8,    4,    3,    1,    3,    3,    1,
    1,    3,    1,    3,    1,    2,    1,    1,    3,    3,
    3,    3,    3,    3,    3,
};
short yydefred[] = {                                      0,
   17,    0,    0,    0,    0,    0,    0,    0,    8,    0,
   12,    0,    0,    0,    0,    0,    0,    0,    0,    0,
   21,   16,    0,    0,   28,    0,    0,    0,    0,   25,
   27,   13,    0,    0,    6,    7,   11,   15,    0,   26,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
   19,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,   24,    0,    0,   14,
};
short yydgoto[] = {                                       6,
   27,    8,    9,   10,   11,   28,   29,   30,   31,   24,
};
short yysindex[] = {                                     -7,
    0,  -31,  -26,  -16,   -7,    0,   36,   58,    0,  -59,
    0,  -24,  -40,  -59,   45,   -7,   -7,   -7,   -7,  -16,
    0,    0,   36,  -22,    0,  -33,  -32,  -28, -217,    0,
    0,    0,   58,   58,    0,    0,    0,    0,  -18,    0,
   -7,   -7,   -7,   -7,   -7,   -7,   -7,  -40,   -7,  -40,
    0,   36,   36,   36,   36,   36,   36,   36,   36, -217,
  -25,    0,   -7,   61,    0,
};
short yyrindex[] = {                                     56,
    0,    0,    0,    0,    0,    0,   59,   23,    0,    1,
    0,    0,    0,   10,    0,    0,    0,    0,    0,    0,
    0,    0,    6,    0,    0,    0,    0,    0,   15,    0,
    0,    0,   32,   54,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,   39,  -57,  -55,  -53,  -51,  -44,  -20,  -10,   17,
    0,    0,    0,    0,    0,
};
short yygindex[] = {                                      0,
   96,    9,   22,   74,   67,    0,   55,   57,   83,    0,
};
#define YYTABLESIZE 321
short yytable[] = {                                       5,
   10,   34,   45,   35,    4,   31,    5,   29,   12,    9,
   16,    4,   17,   13,   30,    5,   22,   16,   38,   17,
    4,    5,    5,    5,   33,   34,    4,   46,   44,   47,
   49,    3,    5,   63,   20,   10,   39,    4,   32,   35,
   36,   10,   10,   10,    9,   10,   20,   10,   33,   50,
    9,    9,    9,    4,    9,    1,    9,    5,    2,   10,
   10,   10,   10,    5,   20,    5,    3,    5,    9,    9,
    9,    9,    3,   23,    3,   22,    3,   14,   16,   18,
   17,    5,    5,    5,    5,   32,   37,   16,    4,   17,
    3,    3,    3,    3,    4,    7,    4,   18,    4,   18,
   15,   65,   60,   16,   19,   17,   62,   23,   40,    0,
    0,    0,    4,    4,    4,    4,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,   52,    0,   53,   54,   55,   56,
   57,   58,   59,    0,   61,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,   64,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,   34,   34,
   35,   35,   31,   31,   29,   29,    1,    0,   25,    2,
    3,   30,   30,    1,   26,   25,    2,    3,    0,   41,
   42,   43,    1,   21,    0,    2,    3,   48,    1,   51,
    1,    2,    3,    2,    3,   32,   32,    0,    0,    1,
    0,    0,    2,    3,    0,   33,   33,    0,    0,    0,
    0,    0,   10,   10,   10,    0,   10,   10,    0,    0,
    0,    9,    9,    9,    0,    9,    9,    0,    0,    0,
   23,    0,   22,    0,    5,    5,    5,    0,    5,    5,
    0,    0,    0,    3,    3,    3,    0,    3,    3,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    4,    4,    4,    0,    4,
    4,
};
short yycheck[] = {                                      40,
    0,   59,   35,   59,   45,   59,   40,   59,   40,    0,
   43,   45,   45,   40,   59,   40,   41,   43,   41,   45,
   45,   40,    0,   40,   16,   17,   45,   60,   61,   62,
   59,    0,   40,   59,   94,   35,   59,   45,   59,   18,
   19,   41,   42,   43,   35,   45,   41,   47,   59,  267,
   41,   42,   43,    0,   45,    0,   47,   35,    0,   59,
   60,   61,   62,   41,   59,   43,   35,   45,   59,   60,
   61,   62,   41,   59,   43,   59,   45,    4,   43,   41,
   45,   59,   60,   61,   62,   41,   20,   43,   35,   45,
   59,   60,   61,   62,   41,    0,   43,   59,   45,   42,
    5,   41,   48,   43,   47,   45,   50,   12,   26,   -1,
   -1,   -1,   59,   60,   61,   62,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   39,   -1,   41,   42,   43,   44,
   45,   46,   47,   -1,   49,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   63,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,  266,  267,
  266,  267,  266,  267,  266,  267,  257,   -1,  259,  260,
  261,  266,  267,  257,  265,  259,  260,  261,   -1,  262,
  263,  264,  257,  258,   -1,  260,  261,  266,  257,  258,
  257,  260,  261,  260,  261,  266,  267,   -1,   -1,  257,
   -1,   -1,  260,  261,   -1,  266,  267,   -1,   -1,   -1,
   -1,   -1,  262,  263,  264,   -1,  266,  267,   -1,   -1,
   -1,  262,  263,  264,   -1,  266,  267,   -1,   -1,   -1,
  266,   -1,  266,   -1,  262,  263,  264,   -1,  266,  267,
   -1,   -1,   -1,  262,  263,  264,   -1,  266,  267,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,  262,  263,  264,   -1,  266,
  267,
};
#define YYFINAL 6
#ifndef YYDEBUG
#define YYDEBUG 0
#endif
#define YYMAXTOKEN 267
#if YYDEBUG
char *yyname[] = {
"end-of-file",0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,"'#'",0,0,0,0,"'('","')'","'*'","'+'",0,"'-'",0,"'/'",0,0,0,0,0,0,0,0,0,0,0,
"';'","'<'","'='","'>'",0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,"'^'",0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,"NUM","RANGE","BOOL","ID","WENN","LEQ","GEQ","NEQ","NOT","OR",
"AND",
};
char *yyrule[] = {
"$accept : input",
"input :",
"input : exp",
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
"atom : WENN '(' bool ';' exp ';' exp ')'",
"atom : ID '(' args ')'",
"atom : ID '(' ')'",
"atom : NUM",
"args : args ';' exp",
"args : args ';' RANGE",
"args : exp",
"args : RANGE",
"bool : bool OR bool_and",
"bool : bool_and",
"bool_and : bool_and AND bool_unary",
"bool_and : bool_unary",
"bool_unary : NOT bool_atom",
"bool_unary : bool_atom",
"bool_atom : BOOL",
"bool_atom : exp '=' exp",
"bool_atom : exp '#' exp",
"bool_atom : exp NEQ exp",
"bool_atom : exp '<' exp",
"bool_atom : exp '>' exp",
"bool_atom : exp LEQ exp",
"bool_atom : exp GEQ exp",
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
#line 107 "yacc.y"

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

#line 257 "y.tab.c"
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
#line 51 "yacc.y"
{ setResult( 0.0 ); }
break;
case 2:
#line 52 "yacc.y"
{ setResult( yyvsp[0].dbl ); }
break;
case 3:
#line 55 "yacc.y"
{ yyval.dbl = yyvsp[-2].dbl + yyvsp[0].dbl; }
break;
case 4:
#line 56 "yacc.y"
{ yyval.dbl = yyvsp[-2].dbl - yyvsp[0].dbl; }
break;
case 5:
#line 57 "yacc.y"
{ yyval.dbl = yyvsp[0].dbl; }
break;
case 6:
#line 60 "yacc.y"
{ yyval.dbl = yyvsp[-2].dbl * yyvsp[0].dbl; }
break;
case 7:
#line 61 "yacc.y"
{ yyval.dbl = yyvsp[-2].dbl / yyvsp[0].dbl; }
break;
case 8:
#line 62 "yacc.y"
{ yyval.dbl = yyvsp[0].dbl; }
break;
case 9:
#line 64 "yacc.y"
{ yyval.dbl = -yyvsp[0].dbl; }
break;
case 10:
#line 65 "yacc.y"
{ yyval.dbl = yyvsp[0].dbl; }
break;
case 11:
#line 68 "yacc.y"
{ yyval.dbl = pow( yyvsp[-2].dbl, yyvsp[0].dbl ); }
break;
case 12:
#line 69 "yacc.y"
{ yyval.dbl = yyvsp[0].dbl; }
break;
case 13:
#line 72 "yacc.y"
{ yyval.dbl = yyvsp[-1].dbl; }
break;
case 14:
#line 73 "yacc.y"
{ if ( yyvsp[-5].b ) yyval.dbl = yyvsp[-3].dbl; else yyval.dbl = yyvsp[-1].dbl; }
break;
case 15:
#line 74 "yacc.y"
{ double res; int e = funcDbl( yyvsp[-3].name, yyvsp[-1].param, &res ); if ( e != 0 ) setError( e, yyvsp[-3].name ); yyval.dbl = res; }
break;
case 16:
#line 75 "yacc.y"
{ double res; int e = funcDbl( yyvsp[-2].name, newArgList(), &res ); if ( e != 0 ) setError( e, yyvsp[-2].name ); yyval.dbl = res; }
break;
case 17:
#line 76 "yacc.y"
{ yyval.dbl = yyvsp[0].dbl; }
break;
case 18:
#line 79 "yacc.y"
{ addDbl( yyvsp[-2].param, yyvsp[0].dbl ); yyval.param = yyvsp[-2].param; }
break;
case 19:
#line 80 "yacc.y"
{ addRange( yyvsp[-2].param, yyvsp[0].range ); yyval.param = yyvsp[-2].param; }
break;
case 20:
#line 81 "yacc.y"
{ void* l = newArgList(); addDbl( l, yyvsp[0].dbl ); yyval.param = l; }
break;
case 21:
#line 82 "yacc.y"
{ void* l = newArgList(); addRange( l, yyvsp[0].range ); yyval.param = l; }
break;
case 22:
#line 84 "yacc.y"
{ yyval.b = yyvsp[-2].b || yyvsp[0].b; }
break;
case 23:
#line 85 "yacc.y"
{ yyval.b = yyvsp[0].b; }
break;
case 24:
#line 88 "yacc.y"
{ yyval.b = yyvsp[-2].b && yyvsp[0].b; }
break;
case 25:
#line 89 "yacc.y"
{ yyval.b = yyvsp[0].b; }
break;
case 26:
#line 92 "yacc.y"
{ yyval.b = !yyvsp[0].b; }
break;
case 27:
#line 93 "yacc.y"
{ yyval.b = yyvsp[0].b; }
break;
case 28:
#line 96 "yacc.y"
{ yyval.b = yyvsp[0].b; }
break;
case 29:
#line 97 "yacc.y"
{ yyval.b = ( yyvsp[-2].dbl == yyvsp[0].dbl ); }
break;
case 30:
#line 98 "yacc.y"
{ yyval.b = ( yyvsp[-2].dbl != yyvsp[0].dbl ); }
break;
case 31:
#line 99 "yacc.y"
{ yyval.b = ( yyvsp[-2].dbl != yyvsp[0].dbl ); }
break;
case 32:
#line 100 "yacc.y"
{ yyval.b = ( yyvsp[-2].dbl < yyvsp[0].dbl ); }
break;
case 33:
#line 101 "yacc.y"
{ yyval.b = ( yyvsp[-2].dbl > yyvsp[0].dbl ); }
break;
case 34:
#line 102 "yacc.y"
{ yyval.b = ( yyvsp[-2].dbl <= yyvsp[0].dbl ); }
break;
case 35:
#line 103 "yacc.y"
{ yyval.b = ( yyvsp[-2].dbl >= yyvsp[0].dbl ); }
break;
#line 538 "y.tab.c"
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
