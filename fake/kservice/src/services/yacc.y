%{
#include <stdlib.h>
#include <stdio.h>
#include "ktraderparse_p.h"
#include "yacc.h"

#define YYLTYPE_IS_TRIVIAL 0
#define YYENABLE_NLS 0
#define YYLEX_PARAM _scanner
#define YYPARSE_PARAM _scanner
typedef void* yyscan_t;
void yyerror(const char *s);
int kiotraderlex(YYSTYPE * yylval, yyscan_t scanner);
int kiotraderlex_init (yyscan_t* scanner);
int kiotraderlex_destroy(yyscan_t scanner);

void KTraderParse_initFlex( const char *s, yyscan_t _scanner );

%}

%union
{
     char valb;
     int vali;
     double vald;
     char *name;
     void *ptr;
}

%token NOT
%token EQ
%token EQI
%token NEQ
%token NEQI
%token LEQ
%token GEQ
%token LE
%token GR
%token OR
%token AND
%token TOKEN_IN
%token TOKEN_IN_SUBSTRING
%token MATCH_INSENSITIVE
%token TOKEN_IN_INSENSITIVE
%token TOKEN_IN_SUBSTRING_INSENSITIVE
%token EXIST
%token MAX
%token MIN

%token <valb> VAL_BOOL
%token <name> VAL_STRING
%token <name> VAL_ID
%token <vali> VAL_NUM
%token <vald> VAL_FLOAT

%type <ptr> bool
%type <ptr> bool_or
%type <ptr> bool_and
%type <ptr> bool_compare
%type <ptr> expr_in
%type <ptr> expr_twiddle
%type <ptr> expr
%type <ptr> term
%type <ptr> factor_non
%type <ptr> factor

%destructor { KTraderParse_destroy( $$ ); } bool_or
%destructor { KTraderParse_destroy( $$ ); } bool_and
%destructor { KTraderParse_destroy( $$ ); } bool_compare
%destructor { KTraderParse_destroy( $$ ); } expr_in
%destructor { KTraderParse_destroy( $$ ); } expr_twiddle
%destructor { KTraderParse_destroy( $$ ); } expr
%destructor { KTraderParse_destroy( $$ ); } term
%destructor { KTraderParse_destroy( $$ ); } factor_non
%destructor { KTraderParse_destroy( $$ ); } factor

%pure-parser

/* Grammar follows */

%%

constraint: /* empty */ { KTraderParse_setParseTree( 0L ); }
          | bool { KTraderParse_setParseTree( $<ptr>1 ); }
;

bool: bool_or { $$ = $<ptr>1; }
;

bool_or: bool_and OR bool_or { $$ = KTraderParse_newOR( $<ptr>1, $<ptr>3 ); }
       | bool_and { $$ = $<ptr>1; }
;

bool_and: bool_compare AND bool_and { $$ = KTraderParse_newAND( $<ptr>1, $<ptr>3 ); }
        | bool_compare { $$ = $<ptr>1; }
;

bool_compare: expr_in EQ expr_in { $$ = KTraderParse_newCMP( $<ptr>1, $<ptr>3, 1 ); }
            | expr_in EQI expr_in { $$ = KTraderParse_newCMP( $<ptr>1, $<ptr>3, 7 ); }
            | expr_in NEQ expr_in { $$ = KTraderParse_newCMP( $<ptr>1, $<ptr>3, 2 ); }
            | expr_in NEQI expr_in { $$ = KTraderParse_newCMP( $<ptr>1, $<ptr>3, 8 ); }
            | expr_in GEQ expr_in { $$ = KTraderParse_newCMP( $<ptr>1, $<ptr>3, 3 ); }
            | expr_in LEQ expr_in { $$ = KTraderParse_newCMP( $<ptr>1, $<ptr>3, 4 ); }
            | expr_in LE expr_in { $$ = KTraderParse_newCMP( $<ptr>1, $<ptr>3, 5 ); }
            | expr_in GR expr_in { $$ = KTraderParse_newCMP( $<ptr>1, $<ptr>3, 6 ); }
            | expr_in { $$ = $<ptr>1; }
;

expr_in: expr_twiddle TOKEN_IN VAL_ID { $$ = KTraderParse_newIN( $<ptr>1, KTraderParse_newID( $<name>3 ), 1 ); }
       | expr_twiddle TOKEN_IN_INSENSITIVE VAL_ID { $$ = KTraderParse_newIN( $<ptr>1, KTraderParse_newID( $<name>3 ), 0 ); }
       | expr_twiddle TOKEN_IN_SUBSTRING VAL_ID { $$ = KTraderParse_newSubstringIN( $<ptr>1, KTraderParse_newID( $<name>3 ), 1 ); }
       | expr_twiddle TOKEN_IN_SUBSTRING_INSENSITIVE VAL_ID { $$ = KTraderParse_newSubstringIN( $<ptr>1, KTraderParse_newID( $<name>3 ), 0 ); }
       | expr_twiddle { $$ = $<ptr>1; }
;

expr_twiddle: expr '~' expr { $$ = KTraderParse_newMATCH( $<ptr>1, $<ptr>3, 1 ); }
            | expr_twiddle MATCH_INSENSITIVE expr { $$ = KTraderParse_newMATCH( $<ptr>1, $<ptr>3, 0 ); }
            | expr { $$ = $<ptr>1; }
;

expr: expr '+' term { $$ = KTraderParse_newCALC( $<ptr>1, $<ptr>3, 1 ); }
    | expr '-' term { $$ = KTraderParse_newCALC( $<ptr>1, $<ptr>3, 2 ); }
    | term { $$ = $<ptr>1; }
;

term: term '*' factor_non { $$ = KTraderParse_newCALC( $<ptr>1, $<ptr>3, 3 ); }
    | term '/' factor_non { $$ = KTraderParse_newCALC( $<ptr>1, $<ptr>3, 4 ); }
    | factor_non { $$ = $<ptr>1; }
;

factor_non: NOT factor { $$ = KTraderParse_newNOT( $<ptr>2 ); }
          | factor { $$ = $<ptr>1; }
;

factor: '(' bool_or ')' { $$ = KTraderParse_newBRACKETS( $<ptr>2 ); }
      | EXIST VAL_ID { $$ = KTraderParse_newEXIST( $<name>2 ); }
      | VAL_ID { $$ = KTraderParse_newID( $<name>1 ); }
      | VAL_NUM { $$ = KTraderParse_newNUM( $<vali>1 ); }
      | VAL_FLOAT { $$ = KTraderParse_newFLOAT( $<vald>1 ); }
      | VAL_STRING { $$ = KTraderParse_newSTRING( $<name>1 ); }
      | VAL_BOOL { $$ = KTraderParse_newBOOL( $<valb>1 ); }
      | MAX VAL_ID { $$ = KTraderParse_newMAX2( $<name>2 ); }
      | MIN VAL_ID { $$ = KTraderParse_newMIN2( $<name>2 ); }
;

/* End of grammar */

%%

void yyerror ( const char *s )  /* Called by yyparse on error */
{
    KTraderParse_error( s );
}

void KTraderParse_mainParse( const char *_code )
{
    yyscan_t scanner;
    kiotraderlex_init(&scanner);
    KTraderParse_initFlex(_code, scanner);
    kiotraderparse(scanner);
    kiotraderlex_destroy(scanner);
}
