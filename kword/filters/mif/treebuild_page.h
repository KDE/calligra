/* $Id$
 *
 * This file is part of MIFParse, a MIF parser for Unix.
 *
 * Copyright (C) 1998 by Matthias Kalle Dalheimer <kalle@dalheimer.de>
 */

#ifndef _TREEBUILD_PAGE_H
#define _TREEBUILD_PAGE_H

#include <string>
#include <qlist.h>

class Polygon;
class Unique;
class TextRect;
class TextLine;
class PolyLine;
class Frame;
class Rectangle;
class RunAroundType;
class RunAroundGap;
class PolyLine;
class Group;
class Ellipse;

class PageType
{
public:
	PageType( const char* );

private:
	string _value;
};


class PageTag
{
public:
	PageTag( const char* );

private:
	string _value;
};


class PageSize
{
public:
	PageSize( double width, const char* unitw, 
			  double height, const char* unith );

private:
	double _width;
	double _height;
};


class PageOrientation
{
public:
	PageOrientation( const char* );

private:
	string _value;
};


class PageAngle
{
public:
	PageAngle( double );

private:
	double _value;
};


class PageNum
{
public:
	PageNum( const char* );

private:
	string _value;
};


class PageBackground
{
public:
	PageBackground( const char* );

private:
	string _value;
};



class PageElement
{
public:
	enum PageElementType { T_PageType, T_PageTag, T_PageSize,
						   T_PageOrientation, T_PageAngle,
						   T_PageNum, T_PageBackground,
						   T_Unique, T_TextRect, T_TextLine,
						   T_PolyLine, T_Frame, T_Rectangle,
						   T_RunAroundGap, T_RunAroundType, T_Polygon,
						   T_Group, T_Ellipse };

	PageElement( PageType* element );
	PageElement( PageTag* element );
	PageElement( PageSize* element );
	PageElement( PageOrientation* element );
	PageElement( PageAngle* element );
	PageElement( PageNum* element );
	PageElement( PageBackground* element );
	PageElement( Unique* element );
	PageElement( TextRect* element );
	PageElement( TextLine* element );
	PageElement( PolyLine* element );
	PageElement( Frame* element );
	PageElement( Rectangle* element );
	PageElement( RunAroundGap* element );
	PageElement( RunAroundType* element );
	PageElement( Polygon* element );
	PageElement( Group* element );
	PageElement( Ellipse* element );

	PageElementType type() const { return _type; }

	TextRect* textRect() const { return _type == T_TextRect ?
									 _textrect : 0; }

private:
	PageElementType _type;
	union {
		PageTag* _pagetag;
		PageSize* _pagesize;
		PageType* _pagetype;
		PageOrientation* _pageorientation;
		PageAngle* _pageangle;
		PageNum* _pagenum;
		PageBackground* _pagebackground;
		Unique* _unique;
		TextRect* _textrect;
		TextLine* _textline;
		PolyLine* _polyline;
		Frame* _frame;
		Rectangle* _rectangle;
		RunAroundGap* _runaroundgap;
		RunAroundType* _runaroundtype;
		Polygon* _polygon;
		Group* _group;
		Ellipse* _ellipse;
	};
};

typedef QList<PageElement> PageElementList;

class Page
{
public:
	Page( PageElementList* elements );

	PageElementList* elements() const { return _elements; }

private:
	PageElementList* _elements;
};

typedef QList<Page> PageList;

extern PageList pages;


#endif
