%{

#include "yacc.h"
#include <string.h>
#include <stdlib.h>
#define YY_NO_UNPUT

char* putSymbol( char *_name );
char* putString( char *_name );
int yywrap();

%}

DIGIT    [0-9]

%%

"==" { return EQ; }
"!=" { return NEQ; }
"<" { return LE; }
">" { return GR; }
"<=" { return LEQ; }
">=" { return GEQ; }
"not" { return NOT; }
"and" { return AND; }
"or" { return OR; }
"in" { return IN; }
"exist" { return EXIST; }
"max" { return MAX; }
"min" { return MIN; }

"~"|"/"|"+"|"-"|"="|"*"|"("|")"|","  { yylval.name = 0L; return (int)(*yytext); }

"TRUE" { yylval.valb = 1; return BOOL; }
"FALSE" { yylval.valb = 0; return BOOL; }

"'"[^']*"'" { yylval.name = putString( yytext ); return STRING; }

"-"{DIGIT}+  { yylval.vali = atoi( yytext ); return NUM; }
{DIGIT}+  { yylval.vali = atoi( yytext ); return NUM; }

{DIGIT}*"\."{DIGIT}+ { yylval.vald = atof( yytext ); return FLOAT; }

[a-zA-Z][a-zA-Z0-9]* { yylval.name = putSymbol( yytext ); return ID; }

[ \t\n]+          /* eat up whitespace */

. { printf( "Unrecognized character: %s\n", yytext ); }

%%

char* putSymbol( char *_name )
{
  char *p = (char*)malloc( strlen( _name ) + 1 );
  strcpy( p, _name );
  return p;
}

char* putString( char *_str )
{
  int l = strlen( _str );
  char *p = (char*)malloc( l );
  char *s = _str + 1;
  char *d = p;
  while ( s != _str + l - 1 )
  {
     if ( *s != '\\' )
        *d++ = *s++;
     else
     {
        s++;
        if ( *s == '\\' )
          *d++ = '\\';
        else if ( *s == 'n' )
          *d++ = '\n';
        else if ( *s == 'r' )
          *d++ = '\r';
        else if ( *s == 't' )
          *d++ = '\t';
        s++;
     }
  }
  *d = 0;
  return p;
}

void initFlex( const char *_code )
{
  yy_switch_to_buffer( yy_scan_string( _code ) );
}

int yywrap()
{
  yy_delete_buffer( YY_CURRENT_BUFFER );
  return 1;
}
