%{

#include "y.tab.h"
#include <stdlib.h>
#include <math.h>

#ifndef PI
#define PI M_PI
#endif

char* putSymbol( char *_name );
double getCell( const char *_str );
double getCell2( const char *_str );

extern void parseCell( const char *_str, int* _x, int* _y, void **_table );
extern void parseCell2( const char *_str, int* _x, int* _y, void **_table );
extern void* parseRange( const char *_str );
extern void* parseRange2( const char *_str );
extern double cellValue( void* _table, int _x, int _y );

int yywrap();

%}

DIGIT    [0-9]

%%

"<"|">"|"#"|"="|"^"|"/"|"+"|"-"|"*"|"("|")"|";"  { return (int)(*yytext); }

"TRUE" { yylval.b = 1; return BOOL; }
"FALSE" { yylval.b = 0; return BOOL; }
"IF" { return WENN; }
"<=" { return LEQ; }
">=" { return GEQ; }
"!=" { return NEQ; }
"PI" { yylval.dbl = PI; return NUM; }
"pi" { yylval.dbl = PI; return NUM; }
"or" { return OR; }
"OR" { return OR; }
"and" { return AND; }
"AND" { return AND; }
"not" { return NOT; }
"NOT" { return NOT; }

{DIGIT}+  { yylval.dbl = (double)atoi( yytext ); return NUM; }

{DIGIT}*"\."{DIGIT}+ { yylval.dbl = atof( yytext ); return NUM; }

"$"?[A-Z]+"$"?{DIGIT}+ { yylval.dbl = getCell( yytext ); return NUM; }
[A-Za-z0-9]+"!""$"?[A-Z]+"$"?{DIGIT}+ { yylval.dbl = getCell2( yytext ); return NUM; }
[A-Za-z0-9]+"!""$"?[A-Z]+"$"?{DIGIT}+":""$"?[A-Z]+"$"?{DIGIT}+ { yylval.range = parseRange2( yytext ); return RANGE; }
"$"?[A-Z]+"$"?{DIGIT}+":""$"?[A-Z]+"$"?{DIGIT}+ { yylval.range = parseRange( yytext ); return RANGE; }

[a-zA-Z]+ { yylval.name = putSymbol( yytext ); return ID; }

[ \t\n]+          /* eat up whitespace */

. { printf( "Unrecognized character: %s\n", yytext ); }

%%

double getCell( const char *_str )
{
   int x,y;
   void *table;
   parseCell( _str, &x, &y, &table );
   return cellValue( table, x, y );
}

double getCell2( const char *_str )
{
   int x,y;
   void *table;
   parseCell2( _str, &x, &y, &table );
   return cellValue( table, x, y );
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

char* putSymbol( char *_name )
{
  char *p = (char*)malloc( strlen( _name ) + 1 );
  strcpy( p, _name );
  return p;
}

