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

namespace KChart
{

ParsedArray::ParsedArray( int row, int col )
{
    data = new Data[col * row];
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
    return data[idx(row, col)].text;
}


void ParsedArray::setText( int row, int col, QString s ) //shallow copy (!?)
{
    data[idx(row, col)].text = s;
}


ParsedArray::Type ParsedArray::type( int row, int col )
{
    if ( !isValid(row, col) )
	return Error;

    if ( data[idx(row, col)].text.isNull() )
	return Unknown;

    return data[idx(row, col)].type;
}


QString ParsedArray::stringVal( int row, int col )
{
    Data &d = data[idx(row, col)];

    if ( d.type == Number ) {
	QString  str;

	str.setNum(d.val);
	//str.detach();
	return str;
    } else
	return d.text;
}


double ParsedArray::doubleVal( int row, int col )
{
    return data[idx(row, col)].val;
}


QString ParsedArray::calc( int row, int col, bool *ok )
{

    index = 0;
    current = &data[idx(row, col)];
    current->type = parseExpr( current->val );

    bool done = (current->type == Number) && !get_c().unicode();

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


bool ParsedArray::recalc( int row, int col )
{
    index = 0;
    current = &data[idx(row, col)];
    current->type = parseExpr( current->val );

    return (current->type == Number) && !get_c().unicode();
}


// ----------------------------------------------------------------
//                        Expression parser


// Get the next character or EndOfString (EOS).

QChar ParsedArray::get_c()
{
    while ( index < (int) current->text.length() 
	    && QChar(current->text[index]).isSpace() )
	index++;

    if ( index < (int) current->text.length() ) {
	//debug ("get_c %d,%c,%d", index, contents[index], contents[index]);
	return QChar(current->text[index++]).unicode();
    } else {
	//debug("get_c EOS %d",index);
	return 0;
    }
}


// Put the last character back into the string.

void ParsedArray::putback()
{
    if (index > 0) index--;
}


// Parse an expression and return the calculated value of it through
// the 'res' reference.

ParsedArray::Type ParsedArray::parseExpr( double &res )
{
    Type  t = parseTerm(res);

    if ( t != Number )
	return t;

    QChar  ch = get_c();
    while ( ch.unicode() ) {
	if ( ch.unicode() != '+' && ch.unicode() != '-' ) {
	    putback();
	    return Number;
	}

	double x;
	t = parseTerm(x);
	if ( t != Number )
	    return String;

	if ( ch.unicode() == '+' )
	    res += x;
	else
	    res -= x;
        ch = get_c();
    }

    return Number;
}


ParsedArray::Type ParsedArray::parseTerm( double &res )
{
    Type t = parseFactor(res);
    if ( t != Number )
	return t;

    QChar ch=get_c();
    while ( ch.unicode() ){
	if ( ch.unicode() != '*' && ch.unicode() != '/' ) {
	    putback();
	    return Number;
	}
	double x;
	t = parseFactor(x);
	if ( t != Number )
	    return String;
	if ( ch.unicode() == '*' )
	    res *= x;
	else
        {
            if ( x != 0 )
		res /= x;
            else
                return Error;
        }

        ch=get_c();
    }

    return Number;
}


ParsedArray::Type ParsedArray::parseFactor( double &res )
{
    QChar ch = get_c();
    if ( ch.unicode() == '(' ) {
	Type  t = parseExpr( res );

	if ( t != Number )
	    return String;
	if ( get_c().unicode() == ')' )
	    return Number;
	else
	    return String;
    } else if ( ch.unicode() == '-' ) {
	Type t = parseExpr(res);
	res = -res;
	return t;
    } else if ( !isdigit(ch.latin1()) ) {
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
    QChar ch = get_c();
    if ( !isdigit(ch.latin1()) )
	return String;
    do
        {
        res = res*10 + ch.unicode() - '0';
        ch = get_c();
        }
    while (isdigit(ch.latin1()));
    if (ch.unicode())
	putback();
    return Number;
}


ParsedArray::Type ParsedArray::parseRef( double &res )
{
    res = 0;
    QChar ch = get_c();
    if ( !isalpha(ch.latin1()) )
	return String;
    int col = toupper(ch.latin1()) - 'A'; // ### could be more general
    int row = 0;
    if ( parseInt(row) != Number)
	return String;
    res = doubleVal(row, col);
    return type(row,col);
}

}  //namespace KChart
