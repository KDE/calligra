%{

#define YYERROR_VERBOSE

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

#include <qstring.h>
#include <qtextstream.h>

extern int yylex();

void karbonInitFlex( const char* in );

void yyerror( const char* s )
{
	qDebug( "%s", s );
		exit(1);
}

%}

%token T_HEAD_COMMENT

%%

exp:	T_HEAD_COMMENT	{  }
;

%%

void parseAI( QTextStream& s, const char* in )
{
	karbonInitFlex( in );
	yyparse();
}
