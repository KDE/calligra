%{

#define YYERROR_VERBOSE
#define YYDEBUG 1

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

#include <qstring.h>

#include "aidocument.h"

extern int yylex();

void karbonInitFlex( const char* in );

void yyerror( const char* s )
{
	qDebug( "%s", s );
		exit(1);
}

%}


%union
{
	QString* qstr;
	double nmbr;
	char* node;
}

%token <qstr> HEADER_CREATOR
%token <qstr> HEADER_AUTHOR

%type <node>	head


%%


input:
		| input head
;

head:		HEADER_CREATOR		{ aiDocument->headerCreator( $1 ); }
		| 	HEADER_AUTHOR		{ aiDocument->headerAuthor( $1 ); }
;

%%

void parseAI( const char* in )
{
	karbonInitFlex( in );
	yyparse();
}
