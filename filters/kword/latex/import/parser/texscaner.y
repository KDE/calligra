%{
#include <stdlib.h>
#include <stdio.h>
#include <qstring.h>
#include <qpair.h>
#include <qmap.h>
#include "element.h"
#include "comment.h"
#include "env.h"
#include "command.h"
#include "text.h"

extern int yylex();
//extern int yyterminate();
extern void setParseFile(QString);
int yyerror(const char*);
QPtrList<Element>* _root;
%}

%union
{
	Element* yyelement;
	QPair<QString,QString>* yypair;
	char* yystring;
	QPtrList<QPair<QString,QString> >* yyparams;
	QPtrList<Element>* yylistelements;
	QPtrList<QPtrList<Element> >* yylistlistelements;
};

%token COMMA EQUAL LEFTBRACE RIGHTBRACE LEFTBRACKET RIGHTBRACKET LEFTPARENTHESIS RIGHTPARENTHESIS 
%token <yystring>WORD
%token <yystring>COMMAND
%token <yystring>TEXT <yystring>TEXTMATH
%token <yystring>B_ENV <yystring>E_ENV
%token <yystring>COMMENT
%token <yystring>B_MATH <yystring>E_MATH <yystring>MATH
%token EOF

%start document
%%

document:
	Expressions
	{
		printf("document");
		_root = $<yylistelements>1;
	}
	;

Expressions:
	/* Empty */ 
	{
		printf("no exp\n");
		$<yylistelements>$ = NULL;
	}
	| Expressions Expression
	{
		printf("des expressions.\n");
		if($<yylistelements>1 == NULL)
			$<yylistelements>$ = new QPtrList<Element>();
		else
			$<yylistelements>$ = $<yylistelements>1;
		$<yylistelements>$->append($<yyelement>2);
		if($<yyelement>2->getType() == ELT_ENV)
			printf("exp : env %s\n", ((Env*) $<yyelement>2)->getName().latin1()); 
	}
	;
	
Expression:
	Command
	{
		$<yyelement>$ = $<yyelement>1;
	}
	| TEXT
	{
		printf("text: %s\n", $<yystring>1);
		Text* text = new Text($<yystring>1);
		$<yyelement>$ = text;
	}
	| COMMENT
	{
		printf("comment.: %s\n", $<yystring>1);
		Comment* comment = new Comment($<yystring>1);
		$<yyelement>$ = comment;
	}
	| Environment
	{ 
		printf("exp : env %s", ((Env*) $<yyelement>1)->getName().latin1()); 
		$<yyelement>$ = $<yyelement>1;
	}
	/*| Group
	{
	}*/
	| Math_Env
	;

Command:
	COMMAND ListParams LEFTBRACKET Params RIGHTBRACKET Groups
	{
		printf("command : %s\n", $<yystring>1);
		Command* command = new Command($<yystring>1, $<yyparams>3, $<yyparams>6, $<yylistlistelements>8);
		$<yyelement>$ = command;
	}
	| COMMAND ListParams Groups
	{
		printf("command : %s\n", $<yystring>1);
		Command* command = new Command($<yystring>1, $<yyparams>3, $<yylistlistelements>5);
		$<yyelement>$ = command;
	}

	/*| COMMAND LEFTBRACKET Params RIGHTBRACKET Groups
	{
		printf("command : %s\n", $<yystring>1);
		Command* command = new Command($<yystring>1, $<yyparams>3, $<yylistlistelements>5);
		$<yyelement>$ = command;
	}*/
	
	/*| COMMAND Groups
	{
		printf("command : %s\n", $<yystring>1);
		Command* command = new Command($<yystring>1, $<yylistlistelements>2);
		$<yyelement>$ = command;
	}*/
/*	| COMMAND LEFTBRACKET Params RIGHTBRACKET LEFTBRACE WORD RIGHTBRACE
	{
		printf("command : %s %s\n", $<yystring>1, $<yystring>6);
		Command* command = new Command($<yystring>1, $<yyparams>3, $<yystring>6);
		$<yyelement>$ = command;
	}
	| COMMAND LEFTBRACE WORD RIGHTBRACE
	{
		printf("command : %s %s\n", $<yystring>1, $<yystring>3);
		Command* command = new Command($<yystring>1, $<yystring>3);
		$<yyelement>$ = command;
	}*/
	;

ListParams:
	/* Empty */
	{
		$<yyparams>$ = NULL;
	}
	| ListParams LEFTPARENTHESIS Params RIGHTPARENTHESIS
	;

Params:
	/* Empty */
	{
		$<yyparams>$ = NULL;
	}
	| Param
	{
		$<yyparams>$ = new QPtrList<QPair<QString,QString> >();
		$<yyparams>$->append($<yypair>1);
	}
	| Params COMMA Param
	{
		$<yyparams>$ = $<yyparams>1;
		$<yyparams>$->append($<yypair>1);
	}
	;

Param:
		WORD EQUAL WORD
		{
			QPair<QString,QString>* pair = new QPair<QString,QString>(QString($<yystring>1), QString($<yystring>3));
			$<yypair>$ = pair;
		}
		| WORD
		{
			QPair<QString,QString>* pair = new QPair<QString,QString>(QString("SIMPLE_VALUE"), QString($<yystring>1));
			$<yypair>$ = pair;
		}
		;

Groups:
	/* Empty */
	{
		$<yylistlistelements>$ = NULL;
	}
	| Groups Group
	{
		if($<yylistlistelements>1 == NULL)
			$<yylistlistelements>$ = new QPtrList<QPtrList<Element> >();
		else
			$<yylistlistelements>$ = $<yylistlistelements>1;
		$<yylistlistelements>$->append($<yylistelements>2);
	}
	;

Group:
	LEFTBRACE Expressions RIGHTBRACE
	{
	 	$<yylistelements>$ = $<yylistelements>2;
	}
	;

Environment:
	B_ENV Expressions E_ENV
	{
		printf("Environment : %s", $<yystring>1);
		Env* env = new Env($<yystring>1);
		env->setChildren($<yylistelements>2);
		$<yyelement>$ = env;
	}
	;
	
Math_Env:
	MATH MathExpressions MATH
	{
		printf("Math. inline.");
	}
	| B_MATH MathExpressions E_MATH
	{
		printf("Math. env.");
	}
	;
	
MathExpressions:
	/* Empty */
	| MathExpressions MathExpression
	{
		/*QPtrList<Element>* grpList = new QPtrList<Element>(*$<yylistelements>1);
		grpList->append($<yyelement>2);
		$<yylistelements>$ = grpList;*/
	}
	;

MathExpression:
	Command
	{
		/*$<yyelement>$ = $<yyelement>1;*/
	}
	| TEXT
	{
		/*Text* text = new Text($<yystring>1);
		$<yyelement>$ = text;*/
	}
	| COMMENT
	{
		printf("un commentaire : %s\n", $<yystring>1);
		/*Comment* comment = new Comment($<yystring>1);
		$<yyelement>$ = comment;*/
	}
	| Environment
	{
		/*$<yyelement>$ = $<yyelement>1;*/
	}
	;
	
%%

bool texparse(QString filename)
{
	setParseFile(filename);
  yyparse();
	return (yynerrs == 0);
}

/*int yyerror(const char* s)
{
	//yynerrs++;
  printf("%s\n", s);
	return 0;
}*/

/*int main(char* argv[])
{
	texparse(argv[0]);
}*/
