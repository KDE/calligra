/* This file is part of the KDE project
   Copyright (C) 2004 Lucijan Busch <lucijan@kde.org>

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
#include "sqlparser.h"
#include "sqltypes.h"
#include <iostream>
#include <kdebug.h>

#include <field.h>

#define YY_NO_UNPUT
#define ECOUNT current += yyleng; ctoken = yytext;
%}

%option case-insensitive
%option noyywrap
%option never-interactive

whitespace [ \t\n]
%%
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

"<>" {
	ECOUNT;
	return NOT_EQUAL;
}

"<" {
	ECOUNT;
	return LESS_THAN;
}

"<=" {
	ECOUNT;
	return LESS_OR_EQUAL;
}

">" {
	ECOUNT;
	return GREATER_THAN;
}

">=" {
	ECOUNT;
	return GREATER_OR_EQUAL;
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

[-][0-9]+ {
	yylval.integerValue = atoi(yytext);
	ECOUNT;
	return SIGNED_INTEGER;
}

[0-9]+ {
	yylval.integerValue = atoi(yytext);
	ECOUNT;
	return UNSIGNED_INTEGER;
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

[a-zA-Z]* {
	kdDebug() << "yytext: '" << yytext << "' (" << yyleng << ")" << endl;
	strncpy(yylval.stringValue, yytext, yyleng + 1);
	ECOUNT;
	return USER_DEFINED_NAME;
}

{whitespace}+ {
	ECOUNT;
}

%%

void tokenize(const char *data)
{
	yy_switch_to_buffer(yy_scan_string(data));
	ctoken = "";
	current = 0;
}

