%{

#include "yacc.cc.h"

%}

%option noyywrap

%%

"%%Creator[^\n]+\n"		{
							ECHO;
							return T_HEAD_COMMENT;
						}
.|\n					/* eat up unmatched stuff */

%%

void karbonInitFlex( const char* in )
{
	yy_switch_to_buffer( yy_scan_string( in ) );
}
