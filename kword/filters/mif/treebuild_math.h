/* $Id$
 *
 * This file is part of MIFParse, a MIF parser for Unix.
 *
 * Copyright (C) 1998 by Matthias Kalle Dalheimer <kalle@dalheimer.de>
 */

#ifndef _TREEBUILD_MATH_H
#define _TREEBUILD_MATH_H

#include <list>
#include <string>

class Unique;
class BRect;

class MathFullForm
{
public:
	MathFullForm( const char* value );

private:
	string _value;
};


class MathLineBreak
{
public:
	MathLineBreak( double value, const char* unit );

private:
	double _value;
};


class MathOrigin
{
public:
	MathOrigin( double x, const char* unitx, 
				double y, const char* unity );

private:
	double _x;
	double _y;
};


class MathAlignment
{
public:
	MathAlignment( const char* value );

private:
	string _value;
};


class MathSize
{
public:
	MathSize( const char* value );

private:
	string _value;
};


class MathElement
{
public:
	enum MathElementType { T_MathFullForm, T_MathLineBreak, T_MathOrigin,
						   T_MathAlignment, T_MathSize, T_Unique,
						   T_BRect };

	MathElement( MathFullForm* element );
	MathElement( MathLineBreak* element );
	MathElement( MathOrigin* element );
	MathElement( MathAlignment* element );
	MathElement( MathSize* element );
	MathElement( Unique* element );
	MathElement( BRect* element );

	MathElementType type() const { return _type; }

private:
	MathElementType _type;

	union {
		MathFullForm* _mathfullform;
		MathLineBreak* _mathlinebreak;
		MathOrigin* _mathorigin;
		MathAlignment* _mathalignment;
		MathSize* _mathsize;
		Unique* _unique;
		BRect* _brect;
	};
};



typedef list<MathElement*> MathElementList;

class Math
{
public:
	Math( MathElementList* elements );

	MathElementList* elements() const { return _elements; }

private:
	MathElementList* _elements;
};

typedef list<Math*> MathList;



#endif
