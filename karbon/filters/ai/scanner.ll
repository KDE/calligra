%{

#include <qstring.h>
#include "yacc.cc.h"


%}

%option noyywrap

Digits				[0-9]+
Oct_Digit			[0-7]
Hex_Digit			[a-fA-F0-9]
Int_Literal			[1-9][0-9]*
Oct_Literal			0{Oct_Digit}*
Hex_Literal			(0x|0X){Hex_Digit}*
Esc_Sequence1		"\\"[ntvbrfa\\\?\'\"]
Esc_Sequence2		"\\"{Oct_Digit}{1,3}
Esc_Sequence3		"\\"(x|X){Hex_Digit}{1,2}
Esc_Sequence		({Esc_Sequence1}|{Esc_Sequence2}|{Esc_Sequence3})
Char				([^\n\t\"\'\\]|{Esc_Sequence})
Char_Literal		"'"({Char}|\")"'"
String_Literal		\"({Char}|"'")*\"
Float_Literal1		{Digits}"."{Digits}(e|E)("+"|"-")?{Digits}
Float_Literal2		{Digits}(e|E)("+"|"-")?{Digits}
Float_Literal3		{Digits}"."{Digits}
Float_Literal4		"."{Digits}
Float_Literal5		"."{Digits}(e|E)("+"|"-")?{Digits}


%x header
%x body

%%

"%!PS-Adobe-"			BEGIN(header);
<header>
{
"%%Creator:"[^\n]+		{
							QString tmp( yytext );
							tmp.remove( 0, 10 );
							yylval.qstr = new QString( tmp.stripWhiteSpace() );
							return HEADER_CREATOR;
						}
"%%For:"[^\n]+			{
							QString tmp( yytext );
							tmp.remove( 0, 6 );
							yylval.qstr = new QString( tmp.stripWhiteSpace() );
							return HEADER_AUTHOR;
						}
"%%EndProlog"			BEGIN(body);
.|\n					/* eat up unmatched stuff */
}


<body>
{
"%%EOF"					BEGIN(INITIAL);
.|\n					/* eat up unmatched stuff */
}


.|\n					/* eat up unmatched stuff */


%%

void karbonInitFlex( const char* in )
{
	yy_switch_to_buffer( yy_scan_string( in ) );
}
