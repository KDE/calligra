%{
#include "kspread_calcerr.h"

extern void setError( int _errno, const char *_txt );
extern void initDependFlex( const char* _code );
extern int deperror ( char *s );
%}

%union
{
     char b;
     double dbl;
     char* name;
     void* range;
     void* param;
}

%token NUM
%token RANGE
%token BOOL
%token ID
%token WENN
%token LEQ
%token GEQ
%token NEQ
%token NOT
%token OR
%token AND

%%

input: /* empty string */ { }
     | exp { }
     | bool_exp { }
;

exp: exp '+' mul { }
   | exp '-' mul { }
   | mul { }
;

mul: mul '*' unary { }
   | mul '/' unary { }
   | unary { }

unary: '-' power { }
     | power { }
;

power: power '^' atom { }
     | atom { }
;

atom: '(' exp ')' { }
   | WENN '(' bool_exp ';' exp ';' exp ')' { }
   | ID '(' args ')' { }
   | ID '(' ')' { }
   | NUM { }
;

args: args ';' exp { }
    | args ';' bool_exp { }
    | args ';' RANGE { }
    | exp { }
    | bool_exp { }
    | RANGE { }
;

bool_exp: bool_exp OR bool_and { }
        | bool_and { }
;

bool_and: bool_and AND bool_unary { }
        | bool_unary { }
;

bool_unary: NOT bool_simple { }
          | bool_simple { }
;

bool_simple: bool_atom { }
	   | exp '=' exp { }
           | exp '#' exp { }
           | exp NEQ exp { }
           | exp '<' exp { }
           | exp '>' exp { }
           | exp LEQ exp { }
           | exp GEQ exp { }
           | bool_atom '=' bool_atom { }
           | bool_atom '#' bool_atom { }
           | bool_atom NEQ bool_atom { }
;

bool_atom: BOOL { }
         | '{' bool_exp '}' { }
         | WENN '{' bool_exp ';' bool_exp ';' bool_exp '}' { }
         | ID '{' args '}' { }
         | ID '{' '}' { }
;

%%

int deperror ( char *s )  /* Called by yyparse on error */
{
  printf ("ERROR: %s\n", s);
  setError( ERR_SYNTAX, "" );
  return 0;
}

void dependMainParse( const char *_code )
{
  initDependFlex( _code );
  depparse();
}
