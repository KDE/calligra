%{

#define YYERROR_VERBOSE

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

#include <qstring.h>

extern int yylex();

void yyerror( const char* s )
{
	qDebug( "%s", s );
		exit(1);
}

%}


%token NUM

%%

exp:      NUM             { $$ = $1;         }
;

%%

void parseAI( const char* in )
{
qDebug("inside");
	yyparse();
}
