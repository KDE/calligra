/* This file is part of the KDE project
   Copyright (C) 2004 Lucijan Busch <lucijan@kde.org>
   Copyright (C) 2004 Jaroslaw Staniek <js@iidea.pl>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

%{
#include <field.h>
#include <expression.h>

#include "sqlparser.h"
#include "sqltypes.h"
#include <iostream>
#include <kdebug.h>

#define YY_NO_UNPUT
#define ECOUNT current += yyleng; ctoken = yytext
%}

%option case-insensitive
%option noyywrap
%option never-interactive

whitespace	[ \t\n]
digit				[0-9]
identifier	[a-zA-Z_][a-zA-Z_0-9]*
integer			{digit}+
decimal			(({digit}*\.{digit}+)|({digit}+\.{digit}*))
real				((({digit}*\.{digit}+)|({digit}+\.{digit}*)|({digit}+))([Ee][-+]?{digit}+))
/* todo: support for real numbers */


/*
"@" return AT;
";" return SEMICOLON;
"*" {
	ECOUNT;
	return ASTERISK;
}

"+" {
	ECOUNT;
	return ARITHMETIC_PLUS;
}

"-" {
	ECOUNT;
	return ARITHMETIC_MINUS;
}

"/" {
	ECOUNT;
	return SLASH;
}

"(" {
	ECOUNT;
	return LEFTPAREN;
}

")" {
	ECOUNT;
	return RIGHTPAREN;
}

"." {
	ECOUNT;
	return DOT;
}

"$" {
	ECOUNT;
	return DOLLAR_SIGN;
}

"%" {
	ECOUNT;
	return PERCENT;
}

"=" {
	ECOUNT;
	return EQUAL;
}

"<" {
	ECOUNT;
	return LESS_THAN;
}

">" {
	ECOUNT;
	return GREATER_THAN;
}

"'" {
	ECOUNT;
	return SINGLE_QUOTE;
}

"?" {
	ECOUNT;
	return QUESTION_MARK;
}

":" {
	ECOUNT;
	return COLON;
}

"," {
	ECOUNT;
	return COMMA;
}
*/

%%

("<>"|"!=") {
	ECOUNT;
	return NOT_EQUAL;
}

"<=" {
	ECOUNT;
	return LESS_OR_EQUAL;
}

">=" {
	ECOUNT;
	return GREATER_OR_EQUAL;
}

"IN" {
	ECOUNT;
	return SQL_IN;
}

{integer} {
	yylval.integerValue = atoi(yytext);
	ECOUNT;
	return INTEGER_CONST;
}

{decimal} {
	char *p = yytext;
	if (yytext[0]=='.') { /* no integer part */
		yylval.realValue.integer = 0;
	}
	else {
		yylval.realValue.integer = atoi(p);
		int i=0;
		while (p && i < yyleng && *p != '.') {
			i++;
			p++;
		}
		if (i==0 || !p || *p!='.') {
			yylval.realValue.fractional = 0;
			return REAL_CONST;
		}
	}
	/* fractional part */
	p++;
	yylval.realValue.fractional = atoi(p);
	return REAL_CONST;
}

"AND" {
	ECOUNT;
	return AND;
}

"AS" {
	ECOUNT;
	return AS;
}

"CREATE" {
	ECOUNT;
	return CREATE;
}

"FROM" {
	ECOUNT;
	return FROM;
}

"INTEGER" {
	ECOUNT;
	return SQL_TYPE;
}

"JOIN" {
	ECOUNT;
	return JOIN;
}

"LEFT" {
	ECOUNT;
	return LEFT;
}

"LIKE" {
	ECOUNT;
	return LIKE;
}

"NOT" {
	ECOUNT;
	return NOT;
}

"NULL" {
	ECOUNT;
	return SQL_NULL;
}

"ON" {
	ECOUNT;
	return SQL_ON;
}

"OR" {
	ECOUNT;
	return OR;
}

"RIGHT" {
	ECOUNT;
	return RIGHT;
}

"SELECT" {
	ECOUNT;
	return SELECT;
}

"TABLE" {
	ECOUNT;
	return TABLE;
}

"WHERE" {
	ECOUNT;
	return WHERE;
}


['][^']*['] {
	ECOUNT;
	strncpy(yylval.stringValue, yytext, yyleng + 1);
	return CHARACTER_STRING_LITERAL;
}

{identifier}"\.\*" {
	kdDebug() << "yytext: '" << yytext << "' (" << yyleng << ")" << endl;
	strncpy(yylval.stringValue, yytext, yyleng + 1);
	ECOUNT;
	return USER_DEFINED_NAME_DOT_ASTERISK;
}

{identifier} {
	kdDebug() << "yytext: '" << yytext << "' (" << yyleng << ")" << endl;
	strncpy(yylval.stringValue, yytext, yyleng + 1);
	ECOUNT;
	return USER_DEFINED_NAME;
}

{whitespace}+ {
	ECOUNT;
}

[\~\!\@\#\^\&\|\`\?,()\[\]\.;\:\+\-\*\/\%\^\<\>\=] {
	kdDebug() << "char: '" << yytext[0] << "'" << endl;
	ECOUNT;
	return yytext[0];
}

%%

void tokenize(const char *data)
{
	yy_switch_to_buffer(yy_scan_string(data));
	ctoken = "";
	current = 0;
}

