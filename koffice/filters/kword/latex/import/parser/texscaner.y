%{
#include <stdlib.h>
#include <stdio.h>
#include <qstring.h>
/*#include <qpair.h>
#include <qmap.h>*/
#include "param.h"
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
	Param* yyparam;
	char* yystring;
	QPtrList<Param>* yyparams;
	QPtrList<QPtrList<Param> >* yylistparams;
	QPtrList<Element>* yylistelements;
	QPtrList<QPtrList<Element> >* yylistlistelements;
};

%token COMMA EQUAL LEFTBRACE RIGHTBRACE LEFTBRACKET RIGHTBRACKET LEFTPARENTHESIS RIGHTPARENTHESIS SPACES
%token <yystring>WORD
%token <yystring>COMMAND
%token <yystring>TEXT <yystring>TEXTMATH
%token <yystring>B_ENV <yystring>E_ENV
%token <yystring>COMMENT
%token <yystring>B_MATH <yystring>E_MATH <yystring>MATH
//%token EOF

%start document
%%

document:
	Expressions
	{
		_root = $<yylistelements>1;
	}
	;

Expressions:
	/* Empty */ 
	{
		$<yylistelements>$ = NULL;
	}
	| Expressions Expression
	{
		printf("des expressions.\n");
		if($<yylistelements>1 == NULL)
		{
			$<yylistelements>$ = new QPtrList<Element>();
			$<yylistelements>$->setAutoDelete(true);
		}
		else
			$<yylistelements>$ = $<yylistelements>1;
		$<yylistelements>$->append($<yyelement>2);
	}
	;
	
Expression:
	Command
	{
		$<yyelement>$ = $<yyelement>1;
	}
	| Text
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
	COMMAND ListParams LEFTBRACKET ListParams2 RIGHTBRACKET Groups
	{
		printf("command : %s\n", $<yystring>1);
		Command* command = new Command($<yystring>1, $<yylistparams>2, $<yyparams>4,
									$<yylistlistelements>6);
		$<yyelement>$ = command;
	}
	| COMMAND ListParams Groups
	{
		printf("command : %s\n", $<yystring>1);
		//printf("param 1 : %s\n", $<yyparams>2->first()->getKey());
		Command* command = new Command($<yystring>1, $<yylistparams>2, $<yylistlistelements>3);
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
		$<yylistparams>$ = NULL;
	}
	| ListParams LEFTPARENTHESIS ListParams2 RIGHTPARENTHESIS
	{
		if($<yylistparams>1 == NULL)
		{
			$<yylistparams>$ = new QPtrList<QPtrList<Param> >();
			$<yylistparams>$->setAutoDelete(true);
		}
		else
			$<yylistparams>$ = $<yylistparams>1;
		$<yylistparams>$->append($<yyparams>3);
	};

ListParams2:
	/* Empty */
	{
		$<yyparams>$ = NULL;
	}
	| Params
	{
		$<yyparams>$ = $<yyparams>1;
	};

Params:
	Params COMMA Param
	{
		$<yyparams>$ = $<yyparams>1;
		$<yyparams>$->append($<yyparam>3);
	}
	| Param
	{
		$<yyparams>$ = new QPtrList<Param>();
		$<yyparams>$->setAutoDelete(true);
		$<yyparams>$->append($<yyparam>1);
	}
	;

Param:
		TEXT EQUAL TEXT
		{
			//QPair<QString,QString>* pair = new QPair<QString,QString>(QString($<yystring>1), QString($<yystring>3));
			Param* param = new Param(QString($<yystring>1), QString($<yystring>3));
			$<yyparam>$ = param;
		}
		| TEXT
		{
			//QPair<QString,QString>* pair = new QPair<QString,QString>(QString("SIMPLE_VALUE"), QString($<yystring>1));
			Param* param = new Param($<yystring>1, "");
			$<yyparam>$ = param;
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
		{
			$<yylistlistelements>$ = new QPtrList<QPtrList<Element> >();
			$<yylistlistelements>$->setAutoDelete(true);
		}
		else
			$<yylistlistelements>$ = $<yylistlistelements>1;
		$<yylistlistelements>$->append($<yylistelements>2);
	};

Group:
	LEFTBRACE Expressions RIGHTBRACE
	{
		printf("Group\n");
	 	$<yylistelements>$ = $<yylistelements>2;
	};

Environment:
	B_ENV Expressions E_ENV
	{
		printf("Environment : %s", $<yystring>1);
		Env* env = new Env($<yystring>1);
		env->setChildren($<yylistelements>2);
		$<yyelement>$ = env;
	};
	
Math_Env:
	MATH MathExpressions MATH
	{
		printf("Math. inline.");
	}
	| B_MATH MathExpressions E_MATH
	{
		printf("Math. env.");
	};
	
MathExpressions:
	/* Empty */
	| MathExpressions MathExpression
	{
		/*QPtrList<Element>* grpList = new QPtrList<Element>(*$<yylistelements>1);
		grpList->append($<yyelement>2);
		$<yylistelements>$ = grpList;*/
	};

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
	};

Text:
		TEXT
		{
			$<yystring>$ = $<yystring>1;
		}
		| Text TEXT
		{
		 	printf("%s", $<yystring>2);
		 	$<yystring>$ = strcat($<yystring>1, $<yystring>2);

		}
		| Text COMMA
		{
		 	printf(",");
		 	$<yystring>$ = strcat($<yystring>1, ",");
		}
		| Text SPACES
		{
			 printf(" ");
		 	$<yystring>$ = strcat($<yystring>1, " ");
		};
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
