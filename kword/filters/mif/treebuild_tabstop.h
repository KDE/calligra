/* $Id$
 *
 * This file is part of MIFParse, a MIF parser for Unix.
 *
 * Copyright (C) 1998 by Matthias Kalle Dalheimer <kalle@dalheimer.de>
 */

#ifndef _TREEBUILD_TABSTOP
#define _TREEBUILD_TABSTOP

#include <list>
#include <string>

class TSX_
{
public:
	TSX_( double, const char* unit );

private:
	double _value;
};

class TSType
{
public:
	TSType( const char* );

private:
	string _value;
};

class TSLeaderStr
{
public:
	TSLeaderStr( const char* );

private:
	string _value;
};

class TSDecimalChar
{
public:
	TSDecimalChar( int );

private:
	int _value;
};

class TabStopElement
{
public:
	enum TabStopElementType { T_TSX, T_TSType, T_TSDecimalChar, 
							  T_TSLeaderStr };

	TabStopElement( TSX_* element );
	TabStopElement( TSType* element );
	TabStopElement( TSDecimalChar* element );
	TabStopElement( TSLeaderStr* element );

private:
	TabStopElementType _type;
	union {
		TSX_* _tsx;
		TSType* _tstype;
		TSDecimalChar* _tsdecimalchar;
		TSLeaderStr* _tsleaderstr;
	};
};

typedef list<TabStopElement*> TabStopElementList;

class TabStop
{
public:
	TabStop( TabStopElementList* elements );

private:
	TabStopElementList* _elements;
};


#endif
