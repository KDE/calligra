/****************************************************************************
** $Id$
**
** Copyright (C) 1992-1998 Troll Tech AS.  All rights reserved.
**
** This file is part of an example program for Qt.  This example
** program may be used, distributed and modified without limitation.
**
*****************************************************************************/

#include <ctype.h>

#include "parser.h"

ParsedArray::ParsedArray( int row, int col )
{
    data = new Data[col*row];
    numCols = col;
    numRows = row;
    current = 0;
}
    
ParsedArray::~ParsedArray()
{
    delete[] data;
}

QString ParsedArray::rawText(int row, int col)
{
    return data[idx(row,col)].text;
}
void ParsedArray::setText( int row, int col, QString s ) //shallow copy (!?)
{
    data[idx(row,col)].text = s;
}

#if 0
int ParsedArray::toInt( bool *ok )	const
{
    return (int) toDouble( ok ); //###
}
#endif


ParsedArray::Type ParsedArray::type( int row, int col )
{
    if ( !isValid(row,col) )
	return Error;
    if ( data[idx(row,col)].text.isNull() )
	return Unknown;
    return data[idx(row,col)].type;
}

double ParsedArray::doubleVal( int row, int col )
{
    return data[idx(row,col)].val;
}


bool ParsedArray::recalc( int row, int col )
{
    index = 0;
    current = &data[idx(row,col)];
    current->type = parseExpr( current->val );

    return (current->type == Number) && !getc();
}


QString ParsedArray::stringVal( int row, int col )
{
    Data &d = data[idx(row,col)];

    if ( d.type == Number ) {
	QString str;
	str.setNum(d.val);
	//str.detach();
	return str;
    } else
	return d.text;
}


QString ParsedArray::calc( int row, int col, bool *ok )
{

    index = 0;
    current = &data[idx(row,col)];
    current->type = parseExpr( current->val );

    bool done = (current->type == Number) && !getc();

    if ( ok )
	*ok = done;
    if ( done ) {
	QString str;
	str.setNum(current->val);
	//str.detach();
	return str;
    } else
	return current->text;
}


char ParsedArray::getc() 
{ 
    while ( index<(int)current->text.length() && 
	    isspace( current->text[index] ) ) 
	index++;

    if ( index<(int)current->text.length() ) {
	//debug ("getc %d,%c,%d", index, contents[index], contents[index]);
	return current->text[index++];
    } else {
	//debug("getc EOS %d",index);
	return 0;
    }
}

void ParsedArray::putback() 
{ 
    if (index>0) index--; 
}




ParsedArray::Type ParsedArray::parseExpr( double &res )
{
    Type t = parseTerm(res);
    if ( t != Number )
	return t;

    char ch;
    while (( ch = getc() )){
	if ( ch != '+' && ch != '-' ) {
	    putback();
	    return Number;
	}
	double x;
	t = parseTerm(x);
	if ( t != Number )
	    return String;
	if ( ch == '+' )
	    res += x;
	else
	    res -= x;
    }
    return Number;
}

ParsedArray::Type ParsedArray::parseTerm( double &res )
{
    Type t = parseFactor(res);
    if ( t != Number )
	return t;

    char ch;
    while (( ch = getc() )){
	if ( ch != '*' && ch != '/' ) {
	    putback();
	    return Number;
	}
	double x;
	t = parseFactor(x);
	if ( t != Number )
	    return String;
	if ( ch == '*' )
	    res *= x;
	else
	    if ( x != 0 )
		res /= x;
	    else
		return Error; 
    }
    return Number;
}

ParsedArray::Type ParsedArray::parseFactor( double &res )
{
    char ch = getc();
    if ( ch == '(' ) {
	Type t = parseExpr( res );
	if ( t != Number )
	    return String;
	if ( getc() == ')' )
	    return Number;
	else
	    return String;
    } else if ( ch == '-' ) {
	Type t = parseExpr(res);
	res = -res;
	return t;
    } else if ( isalpha(ch) ) {
	putback();
	return parseRef(res);
    } else {
	putback();
	return parseNumber(res);
    }
}

ParsedArray::Type ParsedArray::parseNumber( double &res )
{
    int x;
    Type t = parseInt(x); //shortcut
    res = x;
    return t;
}

ParsedArray::Type ParsedArray::parseInt( int &res )
{
    res = 0;
    char ch = getc();
    if ( !isdigit(ch) )
	return String;
    do { 
	res = res*10 + ch - '0';
    } while (isdigit(ch=getc()));
    if (ch)
	putback();
    return Number;
}
   

ParsedArray::Type ParsedArray::parseRef( double &res )
{
    res = 0;
    char ch = getc();
    if ( !isalpha(ch) )
	return String;
    int col = toupper(ch) - 'A'; // ### could be more general
    int row = 0;
    if ( parseInt(row) != Number)
	return String;
    res = doubleVal(row, col);
    return type(row,col);	
}
