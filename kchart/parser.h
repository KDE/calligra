/****************************************************************************
** Copyright (C) 1992-1998 Troll Tech AS.  All rights reserved.
**
** This file is part of an example program for Qt.  This example
** program may be used, distributed and modified without limitation.
**
*****************************************************************************/


#ifndef PARSER_H
#define PARSER_H


#include <qstring.h>


class ParsedArray
{
public:

    ParsedArray(int numRows, int numCols);
    ~ParsedArray();

    enum Type { Unknown, Error, String, Number };
    Type     type( int row, int col );
    
    QString  stringVal( int row, int col );
    double   doubleVal( int row, int col );
    int      intVal( int row, int col ) { return (int) doubleVal(row, col); }

    QString  calc( int row, int col, bool *ok=0);
    bool     recalc( int row, int col );


    QString  rawText( int row, int col );
    void     setText( int row, int col, QString ); //shallow copy

private:
    struct Data
    {
	QString  text;
	Type     type;
	double   val;
    };
    Data  *data;
    int    numCols;
    int    numRows;

    Data  *current;
    short  index; // opt: later const char *

    // Support functions for the parser.
    QChar  get_c();
    void   putback();

    Type   parseExpr( double & );
    Type   parseTerm( double & );
    Type   parseFactor( double & );
    Type   parseRef( double & );
    Type   parseNumber( double & );
    Type   parseInt( int & );

    int    idx( int r, int c )     const     { return c + r * numCols; }
    bool   isValid( int r, int c ) const
    { return ((uint) r < (uint) numRows 
	      && (uint) c < (uint) numCols);
    } //Ugly
};


#endif
