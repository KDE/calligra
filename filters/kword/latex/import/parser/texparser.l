/*
 * Ajouter une pile contenant tout les environnements.
 *
 * To compil :
 * flex -+ -otexparser.cc texparser.lex
 * g++ -o texparser texparser.cc -lfl -lm
 *
 */

%{

#define YY_NO_UNPUT

#include <stdio.h>
#include <string.h>
#include <sys/param.h>
#include <QString>
#include <qpair.h>
#include <QMap>

#include "element.h"
#include "env.h"
#include "command.h"
#include "comment.h"
#include "text.h"

#include "texscaner.hh"

int yyerror(const char*);
//extern int idl_line_no;
//%option yylineno
%}
%option noyywrap

%x MATHMODE ENV

b_math \\\(
e_math \\\)
math \$

b_env \\begin\{{word}\}
e_env \\end\{{word}\}

command \\[a-zA-Z]+

display \$\$

comments \%.*\n

ws [ \n\t]
space ({ws}|\~|\\space)

text [^\\\n\$\%\{\}\[\]\(\)\,]+
word ({alphanum}|\*)+

symbol ("$"("\\"{atoz}+|.)"$"|"\\#"|"\\$"|"\\%"|"\\ref")
letter [A-Za-z]
alphanum [A-Za-z0-9]

%%
<*>"{" return(LEFTBRACE);
<*>"}" return(RIGHTBRACE);
<*>"(" return(LEFTPARENTHESIS);
<*>")" return(RIGHTPARENTHESIS);
<*>"[" return(LEFTBRACKET);
<*>"]" { printf("]"); return(RIGHTBRACKET); }
<*>"=" return(EQUAL);
<*>"," { printf(","); return(COMMA); }

<*>{command} {
	printf("COMMAND ");

	//yylval.yyelement = new Command(yytext);
	yylval.yystring = strdup(yytext);
	return COMMAND;
}

<INITIAL,ENV>{b_env} {
	printf("BEGIN ENV ");
	yylval.yystring = strdup(yytext);
	BEGIN(ENV);
	return B_ENV;
}

<ENV>{e_env} {
	printf("END ENV ");
	yylval.yystring = strdup(yytext);

	BEGIN(INITIAL);
	return E_ENV;
}

<*>"\n" {
	printf("NL\n");
	//yylval.yystring = strdup(yytext);
}

<MATHMODE>{math} {
	printf("END MATH ENV ");
	yylval.yystring = strdup(yytext);
	BEGIN(INITIAL);
	return MATH;
}

<INITIAL,ENV>{math} {
	printf("BEGIN MATH ENV ");
	yylval.yystring = strdup(yytext);
	BEGIN(MATH);
	return MATH;
}


<*>{comments} {
	printf("COMMENT ");
	//yylval.yyelement = new Comment(yytext);
	yylval.yystring = strdup(yytext);
	return COMMENT;
}

<*>{text} {
	printf("TEXT (%s)", yytext);
	yylval.yystring = strdup(yytext);
	return TEXT;
}
%%
/**/

/*
<*>{space} {
	printf(" ");
	yylval.yystring = strdup(yytext);
	return SPACES;
}


<*>{word} {
	printf("WORD (%s)", yytext);
	yylval.yystring = strdup(yytext);
	return WORD;
}

<MATHMODE>{texte} {
	printf("MATH TEXT");
	yylval.yystring = strdup(yytext);
	return TEXTMATH;
}*/
void setParseFile(QString filename)
{
	const char* file;
	if( filename.isNull())
		file = "";
	else
		file = filename.latin1();
	yyin = fopen(file, "r");
	if(!yyin)
		yyerror("Can't open file");
	yy_switch_to_buffer(yy_create_buffer(yyin, YY_BUF_SIZE));
}

int yyerror(const char* s)
{
	//yynerrs++;
  printf("\nline  %s\n",  s);
	//return 0;
}

