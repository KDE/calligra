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

#include "parse.h"

void yyerror(char *s);
int yylex();
void initFlex( const char *s );

#line 11 "yacc.y"
typedef union
{
     char valb;
     int vali;
     double vald;
     char *name;
     void *ptr;
} YYSTYPE;
#line 30 "y.tab.c"
#define DOCROOT 257
#define PORT 258
#define CGI 259
#define ALIAS 260
#define MIME 261
#define PIXDIR 262
#define NOT 263
#define EQ 264
#define NEQ 265
#define LEQ 266
#define GEQ 267
#define LE 268
#define GR 269
#define OR 270
#define AND 271
#define IN 272
#define EXIST 273
#define FIRST 274
#define RANDOM 275
#define MAX 276
#define MIN 277
#define WITH 278
#define MAX2 279
#define MIN2 280
#define BOOL 281
#define STRING 282
#define ID 283
#define NUM 284
#define FLOAT 285
#define YYERRCODE 256
short yylhs[] = {                                        -1,
    0,    0,    0,   11,   11,   11,   11,   11,    1,    2,
    2,    3,    3,    4,    4,    4,    4,    4,    4,    4,
    5,    5,    6,    6,    7,    7,    7,    8,    8,    8,
    9,    9,   10,   10,   10,   10,   10,   10,   10,   10,
   10,
};
short yylen[] = {                                         2,
    0,    1,    1,    2,    2,    2,    1,    1,    1,    3,
    1,    3,    1,    3,    3,    3,    3,    3,    3,    1,
    3,    1,    3,    1,    3,    3,    1,    3,    3,    1,
    2,    1,    3,    2,    1,    1,    1,    1,    1,    2,
    2,
};
short yydefred[] = {                                      0,
    0,    0,    7,    8,    0,    0,    0,    0,    0,   39,
   38,   35,   36,   37,    0,    0,    2,    9,    0,    0,
    0,    0,    0,    0,   30,   32,    3,   31,   34,    5,
    6,    4,   40,   41,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,   33,
   10,   12,   14,   15,   17,   16,   18,   19,   21,    0,
    0,    0,   28,   29,
};
short yydgoto[] = {                                      16,
   17,   18,   19,   20,   21,   22,   23,   24,   25,   26,
   27,
};
short yysindex[] = {                                    -40,
  -27, -271,    0,    0,   58,   58,   58, -257, -256,    0,
    0,    0,    0,    0,   58,    0,    0,    0, -235, -233,
 -244, -236,  -41,  -39,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,   -2,   58,   58,   58,   58,   58,
   58,   58,   58, -242,   58,   58,   58,   58,   58,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,  -34,
  -39,  -39,    0,    0,
};
short yyrindex[] = {                                     43,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    7,   40,
   49,   47,   28,    1,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,   37,
   10,   19,    0,    0,
};
short yygindex[] = {                                      0,
    9,   30,   14,   15,   -9,    0,   11,  -29,  -43,   53,
    0,
};
#define YYTABLESIZE 343
short yytable[] = {                                      15,
   27,   46,   48,   47,   63,   64,   11,   49,   46,   25,
   47,   29,   15,   30,   31,   32,   61,   62,   26,   38,
   39,   40,   41,   42,   43,   33,   34,   24,   53,   54,
   55,   56,   57,   58,   36,   44,   23,   37,   50,   13,
   59,   27,    1,   27,   35,   27,   22,   11,   20,   51,
   25,   52,   25,   28,   25,   60,    0,    0,    0,   26,
    0,   26,    0,   26,    0,    0,    0,    0,   24,    0,
    0,    0,    0,    0,    0,    0,    0,   23,    0,    0,
   13,    0,    0,    0,   45,    0,    0,   22,    0,   20,
    0,    0,    0,    0,    0,    0,    0,   15,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,   27,    0,    0,    0,
    0,    0,    0,    0,    0,   25,    0,    0,    0,    0,
    0,    0,    0,    0,   26,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    1,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    2,    3,    4,    5,    6,    7,    8,    9,
   10,   11,   12,   13,   14,    2,    0,    0,    0,    0,
    0,    8,    9,   10,   11,   12,   13,   14,    0,    0,
    0,    0,    0,    0,   27,   27,   27,   27,   27,   27,
   27,   27,   27,   25,   25,   25,   25,   25,   25,   25,
   25,   25,   26,   26,   26,   26,   26,   26,   26,   26,
   26,   24,   24,   24,   24,   24,   24,   24,   24,   24,
   23,   23,   23,   23,   23,   23,   23,   23,   23,   13,
   22,   22,   22,   22,   22,   22,   22,   22,   20,   20,
    1,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    2,    0,    0,    0,    0,    0,    8,    9,   10,   11,
   12,   13,   14,
};
short yycheck[] = {                                      40,
    0,   43,   42,   45,   48,   49,    0,   47,   43,    0,
   45,  283,   40,    5,    6,    7,   46,   47,    0,  264,
  265,  266,  267,  268,  269,  283,  283,    0,   38,   39,
   40,   41,   42,   43,  270,  272,    0,  271,   41,    0,
  283,   41,    0,   43,   15,   45,    0,   41,    0,   36,
   41,   37,   43,    1,   45,   45,   -1,   -1,   -1,   41,
   -1,   43,   -1,   45,   -1,   -1,   -1,   -1,   41,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   41,   -1,   -1,
   41,   -1,   -1,   -1,  126,   -1,   -1,   41,   -1,   41,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   40,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,  126,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,  126,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,  126,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,  263,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,  273,  274,  275,  276,  277,  278,  279,  280,
  281,  282,  283,  284,  285,  273,   -1,   -1,   -1,   -1,
   -1,  279,  280,  281,  282,  283,  284,  285,   -1,   -1,
   -1,   -1,   -1,   -1,  264,  265,  266,  267,  268,  269,
  270,  271,  272,  264,  265,  266,  267,  268,  269,  270,
  271,  272,  264,  265,  266,  267,  268,  269,  270,  271,
  272,  264,  265,  266,  267,  268,  269,  270,  271,  272,
  264,  265,  266,  267,  268,  269,  270,  271,  272,  270,
  264,  265,  266,  267,  268,  269,  270,  271,  270,  271,
  263,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
  273,   -1,   -1,   -1,   -1,   -1,  279,  280,  281,  282,
  283,  284,  285,
};
#define YYFINAL 16
#ifndef YYDEBUG
#define YYDEBUG 0
#endif
#define YYMAXTOKEN 285
#if YYDEBUG
char *yyname[] = {
"end-of-file",0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,"'('","')'","'*'","'+'",0,"'-'",0,"'/'",0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,"'~'",0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
"DOCROOT","PORT","CGI","ALIAS","MIME","PIXDIR","NOT","EQ","NEQ","LEQ","GEQ",
"LE","GR","OR","AND","IN","EXIST","FIRST","RANDOM","MAX","MIN","WITH","MAX2",
"MIN2","BOOL","STRING","ID","NUM","FLOAT",
};
char *yyrule[] = {
"$accept : constraint",
"constraint :",
"constraint : bool",
"constraint : preferences",
"preferences : WITH bool",
"preferences : MAX bool",
"preferences : MIN bool",
"preferences : FIRST",
"preferences : RANDOM",
"bool : bool_or",
"bool_or : bool_and OR bool_and",
"bool_or : bool_and",
"bool_and : bool_compare AND bool_compare",
"bool_and : bool_compare",
"bool_compare : expr_in EQ expr_in",
"bool_compare : expr_in NEQ expr_in",
"bool_compare : expr_in GEQ expr_in",
"bool_compare : expr_in LEQ expr_in",
"bool_compare : expr_in LE expr_in",
"bool_compare : expr_in GR expr_in",
"bool_compare : expr_in",
"expr_in : expr_twiddle IN ID",
"expr_in : expr_twiddle",
"expr_twiddle : expr '~' expr",
"expr_twiddle : expr",
"expr : expr '+' term",
"expr : expr '-' term",
"expr : term",
"term : term '*' factor_non",
"term : term '/' factor_non",
"term : factor_non",
"factor_non : NOT factor",
"factor_non : factor",
"factor : '(' bool_or ')'",
"factor : EXIST ID",
"factor : ID",
"factor : NUM",
"factor : FLOAT",
"factor : STRING",
"factor : BOOL",
"factor : MAX2 ID",
"factor : MIN2 ID",
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
#line 135 "yacc.y"

void yyerror ( char *s )  /* Called by yyparse on error */
{
    printf ("ERROR: %s\n", s);
}

void mainParse( const char *_code )
{
  initFlex( _code );
  yyparse();
}
#line 282 "y.tab.c"
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
#line 67 "yacc.y"
{ setConstraintsParseTree( 0L ); }
break;
case 2:
#line 68 "yacc.y"
{ setConstraintsParseTree( yyvsp[0].ptr ); }
break;
case 3:
#line 69 "yacc.y"
{ }
break;
case 4:
#line 72 "yacc.y"
{ setPreferencesParseTree( newWITH( yyvsp[0].ptr ) ); }
break;
case 5:
#line 73 "yacc.y"
{ setPreferencesParseTree( newMAX( yyvsp[0].ptr ) ); }
break;
case 6:
#line 74 "yacc.y"
{ setPreferencesParseTree( newMIN( yyvsp[0].ptr ) ); }
break;
case 7:
#line 75 "yacc.y"
{ setPreferencesParseTree( newFIRST() ); }
break;
case 8:
#line 76 "yacc.y"
{ setPreferencesParseTree( newRANDOM() ); }
break;
case 9:
#line 79 "yacc.y"
{ yyval.ptr = yyvsp[0].ptr; }
break;
case 10:
#line 82 "yacc.y"
{ yyval.ptr = newOR( yyvsp[-2].ptr, yyvsp[0].ptr ); }
break;
case 11:
#line 83 "yacc.y"
{ yyval.ptr = yyvsp[0].ptr; }
break;
case 12:
#line 86 "yacc.y"
{ yyval.ptr = newAND( yyvsp[-2].ptr, yyvsp[0].ptr ); }
break;
case 13:
#line 87 "yacc.y"
{ yyval.ptr = yyvsp[0].ptr; }
break;
case 14:
#line 90 "yacc.y"
{ yyval.ptr = newCMP( yyvsp[-2].ptr, yyvsp[0].ptr, 1 ); }
break;
case 15:
#line 91 "yacc.y"
{ yyval.ptr = newCMP( yyvsp[-2].ptr, yyvsp[0].ptr, 2 ); }
break;
case 16:
#line 92 "yacc.y"
{ yyval.ptr = newCMP( yyvsp[-2].ptr, yyvsp[0].ptr, 3 ); }
break;
case 17:
#line 93 "yacc.y"
{ yyval.ptr = newCMP( yyvsp[-2].ptr, yyvsp[0].ptr, 4 ); }
break;
case 18:
#line 94 "yacc.y"
{ yyval.ptr = newCMP( yyvsp[-2].ptr, yyvsp[0].ptr, 5 ); }
break;
case 19:
#line 95 "yacc.y"
{ yyval.ptr = newCMP( yyvsp[-2].ptr, yyvsp[0].ptr, 6 ); }
break;
case 20:
#line 96 "yacc.y"
{ yyval.ptr = yyvsp[0].ptr; }
break;
case 21:
#line 99 "yacc.y"
{ yyval.ptr = newIN( yyvsp[-2].ptr, newID( yyvsp[0].name ) ); }
break;
case 22:
#line 100 "yacc.y"
{ yyval.ptr = yyvsp[0].ptr; }
break;
case 23:
#line 103 "yacc.y"
{ yyval.ptr = newMATCH( yyvsp[-2].ptr, yyvsp[0].ptr ); }
break;
case 24:
#line 104 "yacc.y"
{ yyval.ptr = yyvsp[0].ptr; }
break;
case 25:
#line 107 "yacc.y"
{ yyval.ptr = newCALC( yyvsp[-2].ptr, yyvsp[0].ptr, 1 ); }
break;
case 26:
#line 108 "yacc.y"
{ yyval.ptr = newCALC( yyvsp[-2].ptr, yyvsp[0].ptr, 2 ); }
break;
case 27:
#line 109 "yacc.y"
{ yyval.ptr = yyvsp[0].ptr; }
break;
case 28:
#line 112 "yacc.y"
{ yyval.ptr = newCALC( yyvsp[-2].ptr, yyvsp[0].ptr, 3 ); }
break;
case 29:
#line 113 "yacc.y"
{ yyval.ptr = newCALC( yyvsp[-2].ptr, yyvsp[0].ptr, 4 ); }
break;
case 30:
#line 114 "yacc.y"
{ yyval.ptr = yyvsp[0].ptr; }
break;
case 31:
#line 117 "yacc.y"
{ yyval.ptr = newNOT( yyvsp[0].ptr ); }
break;
case 32:
#line 118 "yacc.y"
{ yyval.ptr = yyvsp[0].ptr; }
break;
case 33:
#line 121 "yacc.y"
{ yyval.ptr = newBRACKETS( yyvsp[-1].ptr ); }
break;
case 34:
#line 122 "yacc.y"
{ yyval.ptr = newEXIST( yyvsp[0].name ); }
break;
case 35:
#line 123 "yacc.y"
{ yyval.ptr = newID( yyvsp[0].name ); }
break;
case 36:
#line 124 "yacc.y"
{ yyval.ptr = newNUM( yyvsp[0].vali ); }
break;
case 37:
#line 125 "yacc.y"
{ yyval.ptr = newFLOAT( yyvsp[0].vald ); }
break;
case 38:
#line 126 "yacc.y"
{ yyval.ptr = newSTRING( yyvsp[0].name ); }
break;
case 39:
#line 127 "yacc.y"
{ yyval.ptr = newBOOL( yyvsp[0].valb ); }
break;
case 40:
#line 128 "yacc.y"
{ yyval.ptr = newMAX2( yyvsp[0].name ); }
break;
case 41:
#line 129 "yacc.y"
{ yyval.ptr = newMIN2( yyvsp[0].name ); }
break;
#line 587 "y.tab.c"
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
