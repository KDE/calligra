/* $Id$
 *
 * This file is part of MIFParse, a MIF parser for Unix.
 *
 * Copyright (C) 1998 by Matthias Kalle Dalheimer <kalle@dalheimer.de>
 */

#ifndef _TREEBUILD_TEXTLINE_H
#define _TREEBUILD_TEXTLINE_H

#include <qlist.h>
#include <qstring.h>

class String;
class Unique;
class Separation;
class ObColor;
class DashedPattern;
class RunAroundGap;
class RunAroundType;
class Font;
class GroupID;

class TLOrigin
{
public:
	TLOrigin( double x, const char* unitx,
			  double y, const char* unity );

private:
	double _x;
	double _y;
};


class TLAlignment
{
public:
	TLAlignment( const char* value );

private:
	QString _value;
};


class TLLanguage
{
public:
	TLLanguage( const char* value );

private:
	QString _value;
};



class TextLineElement
{
public:
	enum TextLineElementType { T_TLOrigin, T_TLAlignment, T_Unique,
							   T_TLLanguage,
							   T_Separation, T_ObColor,
							   T_DashedPattern,
							   T_RunAroundGap, T_RunAroundType,
							   T_Font, T_String, T_GroupID };

	TextLineElement( TLOrigin* element );
	TextLineElement( TLAlignment* element );
	TextLineElement( TLLanguage* element );
	TextLineElement( Unique* element );
	TextLineElement( Separation* element );
	TextLineElement( ObColor* element );
	TextLineElement( DashedPattern* element );
	TextLineElement( RunAroundGap* element );
	TextLineElement( RunAroundType* element );
	TextLineElement( Font* element );
	TextLineElement( String* element );
	TextLineElement( GroupID* element );

	TextLineElementType type() const { return _type; }

private:
	TextLineElementType _type;
	union {
		Unique* _unique;
		Separation* _separation;
		RunAroundGap* _runaroundgap;
		RunAroundType* _runaroundtype;
		Font* _font;
		ObColor* _obcolor;
		TLOrigin* _tlorigin;
		TLAlignment* _tlalignment;
		TLLanguage* _tllanguage;
		DashedPattern* _dashedpattern;
		String* _string;
		GroupID* _groupid;
	};
};



typedef QList<TextLineElement> TextLineElementList;

class TextLine
{
public:
	TextLine( TextLineElementList* elements );

	TextLineElementList* elements() const { return _elements; }

private:
	TextLineElementList* _elements;
};

typedef QList<TextLine> TextLineList;


#endif
