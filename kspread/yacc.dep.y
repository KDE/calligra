%{
#include "kspread_calcerr.h"

extern void setError( int _errno, const char *_txt );
extern void initDependFlex( const char* _code );
extern int deperror ( char *s );
%}

%union
{
     double dbl;
     char b;
     char* name;
     void* range;
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
   | WENN '(' bool ';' exp ';' exp ')' { }
   | ID '(' args ')' { }
   | ID '(' ')' { }
   | NUM { }
;

args: args ';' exp { }
    | args ';' RANGE { }
    | exp { }
    | RANGE { }

bool: bool OR bool_and { }
    | bool_and { }
;

bool_and: bool_and AND bool_unary { }
        | bool_unary { }
;

bool_unary: NOT bool_atom { }
          | bool_atom { }
;

bool_atom: BOOL { }
         | exp '=' exp { }
         | exp '#' exp { }
         | exp NEQ exp { }
         | exp '<' exp { }
         | exp '>' exp { }
         | exp LEQ exp { }
         | exp GEQ exp { }
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
