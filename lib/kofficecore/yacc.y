%{
#include <stdlib.h>
#include "koTraderParse.h"

void yyerror(char *s);
int yylex();
void initFlex( const char *s );

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
%token NEQ
%token LEQ
%token GEQ
%token LE
%token GR
%token OR
%token AND
%token IN
%token EXIST
%token MAX
%token MIN

%token <valb> BOOL
%token <name> STRING
%token <name> ID
%token <vali> NUM
%token <vald> FLOAT

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

/* Grammar follows */

%%

constraint: /* empty */ { setParseTree( 0L ); }
          | bool { setParseTree( $<ptr>1 ); }
;

bool: bool_or { $$ = $<ptr>1; }
;

bool_or: bool_and OR bool_and { $$ = newOR( $<ptr>1, $<ptr>3 ); }
       | bool_and { $$ = $<ptr>1; }
;

bool_and: bool_compare AND bool_compare { $$ = newAND( $<ptr>1, $<ptr>3 ); }
        | bool_compare { $$ = $<ptr>1; }
;

bool_compare: expr_in EQ expr_in { $$ = newCMP( $<ptr>1, $<ptr>3, 1 ); }
            | expr_in NEQ expr_in { $$ = newCMP( $<ptr>1, $<ptr>3, 2 ); }
            | expr_in GEQ expr_in { $$ = newCMP( $<ptr>1, $<ptr>3, 3 ); }
            | expr_in LEQ expr_in { $$ = newCMP( $<ptr>1, $<ptr>3, 4 ); }
            | expr_in LE expr_in { $$ = newCMP( $<ptr>1, $<ptr>3, 5 ); }
            | expr_in GR expr_in { $$ = newCMP( $<ptr>1, $<ptr>3, 6 ); }
            | expr_in { $$ = $<ptr>1; }
;

expr_in: expr_twiddle IN ID { $$ = newIN( $<ptr>1, newID( $<name>3 ) ); }
       | expr_twiddle { $$ = $<ptr>1; }
;

expr_twiddle: expr '~' expr { $$ = newMATCH( $<ptr>1, $<ptr>3 ); }
            | expr { $$ = $<ptr>1; }
;

expr: expr '+' term { $$ = newCALC( $<ptr>1, $<ptr>3, 1 ); }
    | expr '-' term { $$ = newCALC( $<ptr>1, $<ptr>3, 2 ); }
    | term { $$ = $<ptr>1; }
;

term: term '*' factor_non { $$ = newCALC( $<ptr>1, $<ptr>3, 3 ); }
    | term '/' factor_non { $$ = newCALC( $<ptr>1, $<ptr>3, 4 ); }
    | factor_non { $$ = $<ptr>1; }
;

factor_non: NOT factor { $$ = newNOT( $<ptr>2 ); }
          | factor { $$ = $<ptr>1; }
;

factor: '(' bool_or ')' { $$ = newBRACKETS( $<ptr>2 ); }
      | EXIST ID { $$ = newEXIST( $<name>2 ); }
      | ID { $$ = newID( $<name>1 ); }
      | NUM { $$ = newNUM( $<vali>1 ); }
      | FLOAT { $$ = newFLOAT( $<vald>1 ); }
      | STRING { $$ = newSTRING( $<name>1 ); }
      | BOOL { $$ = newBOOL( $<valb>1 ); }
      | MAX ID { $$ = newMAX2( $<name>2 ); }
      | MIN ID { $$ = newMIN2( $<name>2 ); }
;

/* End of grammar */

%%

void yyerror ( char *s )  /* Called by yyparse on error */
{
    printf ("ERROR: %s\n", s);
}

void mainParse( const char *_code )
{
  initFlex( _code );
  yyparse();
}
