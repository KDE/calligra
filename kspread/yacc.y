%{
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
%}

%union
{
     double dbl;
     char b;
     char* name;
     void* range;
     void* param;
}

%token <dbl> NUM
%token <range> RANGE
%token <b> BOOL
%token <name> ID
%token WENN
%token LEQ
%token GEQ
%token NEQ
%token NOT
%token OR
%token AND

%type <dbl> exp
%type <dbl> mul
%type <dbl> unary
%type <dbl> power
%type <dbl> atom
%type <b> bool
%type <b> bool_and
%type <b> bool_unary
%type <b> bool_atom
%type <param> args

%%

input: /* empty string */ { setResult( 0.0 ); }
     | exp { setResult( $<dbl>1 ); }
;

exp: exp '+' mul { $$ = $<dbl>1 + $<dbl>3; }
   | exp '-' mul { $$ = $<dbl>1 - $<dbl>3; }
   | mul { $$ = $<dbl>1; }
;

mul: mul '*' unary { $$ = $<dbl>1 * $<dbl>3; }
   | mul '/' unary { $$ = $<dbl>1 / $<dbl>3; }
   | unary { $$ = $<dbl>1; }

unary: '-' power { $$ = -$<dbl>2; }
     | power { $$ = $<dbl>1; }
;

power: power '^' atom { $$ = pow( $<dbl>1, $<dbl>3 ); }
     | atom { $$ = $<dbl>1; }
;

atom: '(' exp ')' { $$ = $<dbl>2; }
   | WENN '(' bool ';' exp ';' exp ')' { if ( $<b>3 ) $$ = $<dbl>5; else $$ = $<dbl>7; }
   | ID '(' args ')' { double res; int e = funcDbl( $<name>1, $<param>3, &res ); if ( e != 0 ) setError( e, $<name>1 ); $$ = res; }
   | ID '(' ')' { double res; int e = funcDbl( $<name>1, newArgList(), &res ); if ( e != 0 ) setError( e, $<name>1 ); $$ = res; }
   | NUM { $$ = $<dbl>1; }
;

args: args ';' exp { addDbl( $<param>1, $<dbl>3 ); $$ = $<param>1; }
    | args ';' RANGE { addRange( $<param>1, $<range>3 ); $$ = $<param>1; }
    | exp { void* l = newArgList(); addDbl( l, $<dbl>1 ); $$ = l; }
    | RANGE { void* l = newArgList(); addRange( l, $<range>1 ); $$ = l; }

bool: bool OR bool_and { $$ = $<b>1 || $<b>3; }
    | bool_and { $$ = $<b>1; }
;

bool_and: bool_and AND bool_unary { $$ = $<b>1 && $<b>3; }
        | bool_unary { $$ = $<b>1; }
;

bool_unary: NOT bool_atom { $$ = !$<b>2; }
          | bool_atom { $$ = $<b>1; }
;

bool_atom: BOOL { $$ = $<b>1; }
         | exp '=' exp { $$ = ( $<dbl>1 == $<dbl>3 ); }
         | exp '#' exp { $$ = ( $<dbl>1 != $<dbl>3 ); }
         | exp NEQ exp { $$ = ( $<dbl>1 != $<dbl>3 ); }
         | exp '<' exp { $$ = ( $<dbl>1 < $<dbl>3 ); }
         | exp '>' exp { $$ = ( $<dbl>1 > $<dbl>3 ); }
         | exp LEQ exp { $$ = ( $<dbl>1 <= $<dbl>3 ); }
         | exp GEQ exp { $$ = ( $<dbl>1 >= $<dbl>3 ); }
;

%%

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

