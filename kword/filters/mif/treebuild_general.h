/* $Id$
 *
 * This file is part of MIFParse, a MIF parser for Unix.
 *
 * Copyright (C) 1998 by Matthias Kalle Dalheimer <kalle@dalheimer.de>
 */

#ifndef _TREEBUILD_GENERAL
#define _TREEBUILD_GENERAL

#include <qlist.h>
#include <qstring.h>

class ShapeRect;
class BRect;
class GroupID;
class DashedPattern;
class RunAroundGap;
class RunAroundType;
class Separation;
class PenWidth;
class ObColor;
class Fill;
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
	int id() const { return _value; }

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
	String( QString );

	QString value() const { return _value; }

private:
	QString _value;
};

class Char
{
public:
	Char( QString );

	QString value() const { return _value; }

private:
	QString _value;
};

class TextRectID
{
public:
	TextRectID( int );
	int id() const { return _value; }

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
	QString _value;
};

class MText
{
public:
	MText( const char* );

private:
	QString _value;
};

class MCurrPage
{
public:
	MCurrPage( const char* );

private:
	QString _value;
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

typedef QList<MarkerElement> MarkerElementList;

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


class ShapeRect
{
public:
	ShapeRect( double x, const char* unitx,
			   double y, const char* unity,
			   double w, const char* unitw,
			   double h, const char* unith );

private:
	double _x;
	double _y;
	double _w;
	double _h;
};


class BRect
{
public:
	BRect( double x, const char* unitx,
		   double y, const char* unity,
		   double w, const char* unitw,
		   double h, const char* unith );

	double x() const { return _x; }
	double y() const { return _y; }
	double width() const { return _w; }
	double height() const { return _h; }

private:
	double _x;
	double _y;
	double _w;
	double _h;
};


class RectangleElement
{
public:
	enum RectangleElementType { T_Unique, T_Separation, T_ObColor,
								T_ShapeRect, T_BRect, T_Pen,
								T_PenWidth, T_Fill, T_DashedPattern,
								T_RunAroundGap, T_RunAroundType, T_GroupID };

	RectangleElement( Unique* element );
	RectangleElement( Separation* element );
	RectangleElement( ObColor* element );
	RectangleElement( ShapeRect* element );
	RectangleElement( BRect* element );
	RectangleElement( Fill* element );
	RectangleElement( Pen* element );
	RectangleElement( PenWidth* element );
	RectangleElement( DashedPattern* element );
	RectangleElement( RunAroundType* element );
	RectangleElement( RunAroundGap* element );
	RectangleElement( GroupID* element );
	
	RectangleElementType type() const { return _type; }

private:
	RectangleElementType _type;

	union {
		Unique* _unique;
		Separation* _separation;
		ObColor* _obcolor;
		ShapeRect* _shaperect;
		BRect* _brect;
		Fill* _fill;
		Pen* _pen;
		PenWidth* _penwidth;
		DashedPattern* _dashedpattern;
		RunAroundGap* _runaroundgap;
		RunAroundType* _runaroundtype;
		GroupID* _groupid;
	};
};



typedef QList<RectangleElement> RectangleElementList;

class Rectangle
{
public:
	Rectangle( RectangleElementList* elements );

	RectangleElementList* elements() const { return _elements; }

private:
	RectangleElementList* _elements;
};

typedef QList<Rectangle> RectangleList;


#endif
