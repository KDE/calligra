%{
#define yywrap kiotraderwrap

#include "yacc.h"
#include <string.h>
#include <stdlib.h>
#define YY_NO_UNPUT

char* KTraderParse_putSymbol( char *_name );
char *KTraderParse_putSymbolInBrackets( char *_name );
char* KTraderParse_putString( char *_name );
double KTraderParse_putSimplePositiveFloat( char *_name );
int yywrap();
int kiotraderlex(YYSTYPE * yylval, yyscan_t scanner);
void KTraderParse_initFlex( const char *_code, yyscan_t _scanner );

%}
%option nomain
%option never-interactive
%option noalways-interactive
%option nostack
%option reentrant
%option bison-bridge

DIGIT    [0-9]

%%

"==" { return EQ; }
"=~" { return EQI; }
"!=" { return NEQ; }
"!~" { return NEQI; }
"<" { return LE; }
">" { return GR; }
"<=" { return LEQ; }
">=" { return GEQ; }
"~~" { return MATCH_INSENSITIVE; }
"not" { return NOT; }
"and" { return AND; }
"or" { return OR; }
"in" { return TOKEN_IN; }
"~in" { return TOKEN_IN_INSENSITIVE; }
"subin" { return TOKEN_IN_SUBSTRING; }
"~subin" { return TOKEN_IN_SUBSTRING_INSENSITIVE; }
"exist" { return EXIST; }
"max" { return MAX; }
"min" { return MIN; }

"~"|"/"|"+"|"-"|"="|"*"|"("|")"|","  { yylval->name = 0L; return (int)(*yytext); }

"TRUE" { yylval->valb = 1; return VAL_BOOL; }
"FALSE" { yylval->valb = 0; return VAL_BOOL; }

"'"[^']*"'" { yylval->name = KTraderParse_putString( yytext ); return VAL_STRING; }

"-"{DIGIT}+  { yylval->vali = atoi( yytext ); return VAL_NUM; }
{DIGIT}+  { yylval->vali = atoi( yytext ); return VAL_NUM; }

{DIGIT}*"\."{DIGIT}+ { yylval->vald = KTraderParse_putSimplePositiveFloat( yytext ); return VAL_FLOAT; }

\[[a-zA-Z][a-zA-Z0-9\-]*\] { yylval->name = KTraderParse_putSymbolInBrackets( yytext ); return VAL_ID; }

[a-zA-Z][a-zA-Z0-9]* { yylval->name = KTraderParse_putSymbol( yytext ); return VAL_ID; }

[ \t\n]+          /* eat up whitespace */

. { printf( "Unrecognized character: %s\n", yytext ); }

%%

char* KTraderParse_putSymbolInBrackets( char *_name )
{
  int l = strlen( _name )-1;
  char *p = (char *)malloc( l );
  if (p != NULL)
  {
    strncpy( p, _name+1, l-1 );
    p[l-1] = 0;
  }

  return p;
}

char *KTraderParse_putSymbol( char *_name )
{
  char *p = (char*)malloc( strlen( _name ) + 1 );
  if (p != NULL)
  {
      strcpy( p, _name );
  }
  return p;
}

char* KTraderParse_putString( char *_str )
{
  int l = strlen( _str );
  char *p = (char*)malloc( l );
  char *s = _str + 1;
  char *d = p;

  if (p == NULL)
    return NULL;

  while ( s != _str + l - 1 )
  {
     if ( *s != '\\' )
        *d++ = *s++;
     else
     {
        s++;
        if ( s != _str + l - 1 )
        {
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
  }
  *d = 0;
  return p;
}

/* Sufficient replacement for atof() to parse [0-9]*\.[0-9]+
   because atof() consults locale for decimal separator,
   while we need it always to be the dot. */
double KTraderParse_putSimplePositiveFloat( char *_str )
{
  char *p;
  int f, g, d;

  f = 0;
  p = _str;
  while ( *p >= '0' && *p <= '9' )
  {
    f = f * 10 + ( *p - '0' );
    p++;
  }
  if ( *p != '.' )
    return (double) f;
  p++;
  g = 0;
  d = 1;
  while ( *p >= '0' && *p <= '9' )
  {
    g = g * 10 + ( *p - '0' );
    d *= 10;
    p++;
  }
  return ( (double) ( f * d + g ) ) / d;
}

void KTraderParse_initFlex( const char *_code, yyscan_t _scanner )
{
  yy_switch_to_buffer( yy_scan_string( _code, _scanner ), _scanner );
}

int yywrap( yyscan_t _scanner )
{
  struct yyguts_t * yyg = (struct yyguts_t*)_scanner;
  yy_delete_buffer( YY_CURRENT_BUFFER, _scanner );
  return 1;
}
