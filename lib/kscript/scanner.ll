%{

#define YY_NO_UNPUT
#include <stdlib.h>
#include <ctype.h>

#include "kscript_parsenode.h"
#include "kscript_types.h"

#ifndef KDE_USE_FINAL
#include "yacc.cc.h"
#endif

extern int idl_line_no;

#include <qstring.h>

static KScript::Long ascii_to_longlong( long base, const char *s )
{
  KScript::Long ll = 0;
  while( *s != '\0' ) {
    char c = *s++;
    if( c >= 'a' )
      c -= 'a' - 'A';
    c -= '0';
    if( c > 9 )
      c -= 'A' - '0' - 10;
    ll = ll * base + c;
  }
  return ll;
}

static KScript::Double ascii_to_longdouble (const char *s)
{
  KScript::Double d;
#ifdef HAVE_SCANF_LF
  sscanf (s, "%Lf", &d);
#else
  /*
   * this is only an approximation and will probably break fixed<>
   * parameter calculation on systems where
   * sizeof(double) < sizeof(long double). but fortunately all
   * systems where scanf("%Lf") is known to be broken (Linux/Alpha
   * and HPUX) have sizeof(double) == sizeof(long double).
   */
  d = strtod (s, NULL);
#endif
  return d;
}

static char translate_char( const char *s )
{
  char c = *s++;
  
  if( c != '\\' )
    return c;
  c = *s++;
  switch( c ) {
  case 'n':
    return '\n';
  case 't':
    return '\t';
  case 'v':
    return '\v';
  case 'b':
    return '\b';
  case 'r':
    return '\r';
  case 'f':
    return '\f';
  case 'a':
    return '\a';
  case '\\':
    return '\\';
  case '?':
    return '\?';
  case '\'':
    return '\'';
  case '"':
    return '"';
  case 'x':
  case 'X':
    return (char) ascii_to_longlong( 16, s );
  default:
    // Gotta be an octal
    return (char) ascii_to_longlong( 8, s );
  }
}


%}

%option noyywrap

/*--------------------------------------------------------------------------*/

Digits                  [0-9]+
Oct_Digit               [0-7]
Hex_Digit               [a-fA-F0-9]
Int_Literal		[1-9][0-9]*
Oct_Literal		0{Oct_Digit}*
Hex_Literal		(0x|0X){Hex_Digit}*
Esc_Sequence1           "\\"[ntvbrfa\\\?\'\"]
Esc_Sequence2           "\\"{Oct_Digit}{1,3}
Esc_Sequence3           "\\"(x|X){Hex_Digit}{1,2}
Esc_Sequence            ({Esc_Sequence1}|{Esc_Sequence2}|{Esc_Sequence3})
Char                    ([^\n\t\"\'\\]|{Esc_Sequence})
Char_Literal            "'"({Char}|\")"'"
String_Literal		\"({Char}|"'")*\"
Float_Literal1		{Digits}"."{Digits}(e|E)("+"|"-")?{Digits}  
Float_Literal2		{Digits}(e|E)("+"|"-")?{Digits}
Float_Literal3          {Digits}"."{Digits}
Float_Literal4		"."{Digits} 
Float_Literal5		"."{Digits}(e|E)("+"|"-")?{Digits}  

/*--------------------------------------------------------------------------*/

KScript_Identifier	[_a-zA-Z][a-zA-Z0-9_]*

/*--------------------------------------------------------------------------*/



%%

[ \t]			;
[\n]			idl_line_no++;
"//"[^\n]*		;
"#pragma"[^\n]*\n       {
			  yylval._str = new QString( yytext );
                          idl_line_no++;
                          return T_PRAGMA;
                        }
"#"[^\n]*\n             ;

"=~"" "*"s/"(\\.|[^\\/])*"/"(\\.|[^\\/])*"/""g"?	{
						  const char *c = yytext + 2;
						  while( isspace( *c ) ) ++c;
			 			  yylval.ident = new QString( c );
						  return T_SUBST;
						}
"=~"" "*"/"(\\.|[^\\/])*"/"			{
						  const char *c = yytext + 2;
						  while( isspace( *c ) ) ++c;
			 			  yylval.ident = new QString( c );
						  return T_MATCH;
						}

"{"			return T_LEFT_CURLY_BRACKET;
"}"			return T_RIGHT_CURLY_BRACKET;
"["		 	return T_LEFT_SQUARE_BRACKET;
"]"			return T_RIGHT_SQUARE_BRACKET;
"("			return T_LEFT_PARANTHESIS;
")"			return T_RIGHT_PARANTHESIS;
":"			return T_COLON;
","			return T_COMMA;
";"			return T_SEMICOLON;
"=="			return T_EQUAL;
"!="			return T_NOTEQUAL;
"!"			return T_NOT;
"="			return T_ASSIGN;
">>"			return T_SHIFTRIGHT;
"<<"			return T_SHIFTLEFT;
"+"			return T_PLUS_SIGN;
"-"			return T_MINUS_SIGN;
"*"			return T_ASTERISK;
"/"			return T_SOLIDUS;
"%"			return T_PERCENT_SIGN;
"~"			return T_TILDE;
"|"			return T_VERTICAL_LINE;
"^"			return T_CIRCUMFLEX;
"&"			return T_AMPERSAND;
"<="			return T_LESS_OR_EQUAL;
">="			return T_GREATER_OR_EQUAL;
"<"			return T_LESS_THAN_SIGN;
">"			return T_GREATER_THAN_SIGN;
"."			return T_MEMBER;

const			return T_CONST;
FALSE			return T_FALSE;
TRUE			return T_TRUE;
struct			return T_STRUCT;
switch			return T_SWITCH;
case			return T_CASE;
default			return T_DEFAULT;
enum			return T_ENUM;
in			return T_IN;
out			return T_OUT;
interface		return T_INTERFACE;
class			return T_CLASS;
immortal		return T_NEW;
delete			return T_DELETE;
while			return T_WHILE;
do			return T_DO;
for			return T_FOR;
if			return T_IF;
else			return T_ELSE;
main			return T_MAIN;
foreach			return T_FOREACH;
return			return T_RETURN;
signal			return T_SIGNAL;
emit			return T_EMIT;
import			return T_IMPORT;
var			return T_VAR;
readonly		return T_READONLY;
attribute		return T_ATTRIBUTE;
inout			return T_INOUT;
raises			return T_RAISES;
try			return T_TRY;
catch			return T_CATCH;
raise			return T_RAISE;

"++"			return T_INCR;
"--"			return T_DECR;
"::"			return T_SCOPE; 



{KScript_Identifier}	{
			  yylval.ident = new QString( yytext );
			  return T_IDENTIFIER;
			}
{Float_Literal1}	|
{Float_Literal2}	|
{Float_Literal3}	|
{Float_Literal4}	|
{Float_Literal5}	{
			  yylval._float = ascii_to_longdouble( yytext );
			  return T_FLOATING_PT_LITERAL;
			}
{Int_Literal}		{ 
			  yylval._int = ascii_to_longlong( 10, yytext );
			  return T_INTEGER_LITERAL;
			}
{Oct_Literal}		{
			  yylval._int = ascii_to_longlong( 8, yytext );
			  return T_INTEGER_LITERAL;
			}
{Hex_Literal}		{
			  yylval._int = ascii_to_longlong( 16, yytext + 2 );
			  return T_INTEGER_LITERAL;
			}
{Char_Literal}		{
                          QCString s( yytext );
	                  s = s.mid( 1, s.length() - 2 );
			  yylval._char = translate_char( s );
			  return T_CHARACTER_LITERAL;
			}
{String_Literal}	{
                          QString s( yytext );
                          yylval._str = new QString( s.mid( 1, s.length() - 2 ) );
			  return T_STRING_LITERAL;
			}
.                       {
                          return T_UNKNOWN;
                        }

%%

