/* $Id$
 *
 * This file is part of MIFParse, a MIF parser for Unix.
 *
 * Copyright (C) 1998 by Matthias Kalle Dalheimer <kalle@dalheimer.de>
 */

#ifndef _TREEBUILD_GENERAL
#define _TREEBUILD_GENERAL

#include <list>
#include <string>

class Unique
{
public:
	Unique( int );

	int value() const { return _value; }

private:
	int _value;
};

class FrameID
{
public:
	FrameID( int );

private:
	int _value;
};

class Pen
{
public:
	Pen( int );

private:
	int _value;
};

class String
{
public:
	String( string );

	string value() const { return _value; }

private:
	string _value;
};

class Char
{
public:
	Char( string );

	string value() const { return _value; }

private:
	string _value;
};

class TextRectID
{
public:
	TextRectID( int );

private:
	int _value;
};

class ATbl
{
public:
	ATbl( int );

private:
	int _value;
};

class AFrame
{
public:
	AFrame( int );

private:
	int _value;
};

class MType
{
public:
	MType( int );

private:
	int _value;
};

class MTypeName
{
public:
	MTypeName( const char* );

private:
	string _value;
};

class MText
{
public:
	MText( const char* );

private:
	string _value;
};

class MCurrPage
{
public:
	MCurrPage( const char* );

private:
	string _value;
};

class MarkerElement
{
public:
	enum MarkerElementType { T_Unique, T_MType, T_MTypeName, T_MText,
							 T_MCurrPage };

	MarkerElement( Unique* element );
	MarkerElement( MType* element );
	MarkerElement( MTypeName* element );
	MarkerElement( MText* element );
	MarkerElement( MCurrPage* elemente );

private:
	MarkerElementType _type;

	union {
		Unique* _unique;
		MType* _mtype;
		MTypeName* _mtypename;
		MText* _mtext;
		MCurrPage* _mcurrpage;
	};
};

typedef list<MarkerElement*> MarkerElementList;

class Marker
{
public:
	Marker( MarkerElementList* elements );

private:
	MarkerElementList* _elements;
};


class MarkerTypeCatalog
{
	// PENDING(kalle) Stuff missing here
public:
};

class KumihanCatalog
{
	//PENDING(kalle) Stuff missing here
};

#endif
