%{

#include "dep.tab.h"

extern void makeCellDepend( const char *_str );
extern void makeCellDepend2( const char *_str );
extern void makeRangeDepend( const char *_str );
extern void makeRangeDepend2( const char *_str );

int depwrap();

%}

DIGIT    [0-9]

%%

"{"|"}"|"<"|">"|"#"|"="|"^"|"/"|"+"|"-"|"*"|"("|")"|";"  { return (int)(*yytext); }

"TRUE" { return BOOL; }
"FALSE" {return BOOL; }
"true" { return BOOL; }
"false" {return BOOL; }
"IF" { return WENN; }
"if" { return WENN; }
"<=" { return LEQ; }
">=" { return GEQ; }
"!=" { return NEQ; }
"PI" { return NUM; }
"pi" { return NUM; }
"or" { return OR; }
"OR" { return OR; }
"and" { return AND; }
"AND" { return AND; }
"not" { return NOT; }
"NOT" { return NOT; }

{DIGIT}+  { return NUM; }

{DIGIT}*"\."{DIGIT}+ { return NUM; }

"$"?[A-Z]+"$"?{DIGIT}+ { makeCellDepend( yytext ); return NUM; }
[A-Za-z0-9]+"!""$"?[A-Z]+"$"?{DIGIT}+ { makeCellDepend2( yytext ); return NUM; }
[A-Za-z0-9]+"!""$"?[A-Z]+"$"?{DIGIT}+":""$"?[A-Z]+"$"?{DIGIT}+ { makeRangeDepend2( yytext ); return RANGE; }
"$"?[A-Z]+"$"?{DIGIT}+":""$"?[A-Z]+"$"?{DIGIT}+ { makeRangeDepend( yytext ); return RANGE; }

[a-zA-Z]+ { return ID; }

[ \t\n]+          /* eat up whitespace */

. { printf( "Unrecognized character: %s\n", yytext ); }

%%

void initDependFlex( const char *_code )
{
  dep_switch_to_buffer( yy_scan_string( _code ) );
}

int depwrap()
{
  dep_delete_buffer( YY_CURRENT_BUFFER );
  return 1;
}

