/* $Id$
 *
 * This file is part of MIFParse, a MIF parser for Unix.
 *
 * Copyright (C) 1998 by Matthias Kalle Dalheimer <kalle@dalheimer.de>
 */

#ifndef _TREEBUILD_GRAPHICS_H
#define _TREEBUILD_GRAPHICS_H

class Fill;
class PenWidth;
class Separation;
class ObColor;
class Angle;
class FrameID;
class Point;
class NumPoints;	
class Smoothed;

class Fill
{
public:
	Fill( int value );

private:
	int _value;
};


class PenWidth
{
public:
	PenWidth( double value, const char* unit );

private:
	double _value;
};


class Separation
{
public:
	Separation( int value );

private:
	int _value;
};


class ObColor
{
public:
	ObColor( const char* value );

private:
	string _value;
};


class Angle
{
public:
	Angle( double value );

private:
	double _value;
};


class RunAroundGap
{
public:
	RunAroundGap( double value, const char* unit );

private:
	double _value;
};


class RunAroundType
{
public:
	RunAroundType( const char* value );

private:
	string _value;
};


/****************************************
 * Group
 ***************************************/

class GroupID
{
public:
	GroupID( int value );

private:
	int _value;
};


class GroupElement
{
public:
	enum GroupElementType { T_FrameID, T_RunAroundGap,
							T_RunAroundType, T_Unique, T_GroupID };

	GroupElement( FrameID* element );
	GroupElement( RunAroundType* element );
	GroupElement( RunAroundGap* element );
	GroupElement( Unique* element );
	GroupElement( GroupID* element );

	GroupElementType* type() const { return _type; }

private:
	GroupElementType* _type;
	union {
		FrameID* _id;
		RunAroundGap* _runaroundgap;
		RunAroundType* _runaroundtype;
		Unique* _unique;
		GroupID* _groupid;
	};
};


typedef list<GroupElement*> GroupElementList;

class Group
{
public:
	Group( GroupElementList* elements );

	GroupElementList* elements() const { return _elements; }

private:
	GroupElementList* _elements;
};

typedef list<Group*> GroupList;


/*********************************************
 * DashedPattern
 ********************************************/
class DashedStyle
{
public:
	DashedStyle( const char* value );

private:
	string _value;
};


class NumSegments
{
public:
	NumSegments( int value );

private:
	int _value;
};


class DashSegment
{
public:
	DashSegment( double value, const char* unit );

private:
	double value;
};



class DashedPatternElement
{
public:
	enum DashedPatternElementType { T_DashedStyle, T_NumSegments,
									T_DashSegment };

	DashedPatternElement( DashedStyle* element );
	DashedPatternElement( NumSegments* element );
	DashedPatternElement( DashSegment* element );

	DashedPatternElementType* type() const { return _type; }

private:
	DashedPatternElementType* _type;
	union {
		DashedStyle* _dashedstyle;
		NumSegments* _numsegments;
		DashSegment* _dashsegment;
	};
};


typedef list<DashedPatternElement*> DashedPatternElementList;

class DashedPattern
{
public:
	DashedPattern( DashedPatternElementList* elements );

	DashedPatternElementList* elements() const { return _elements; }

private:
	DashedPatternElementList* _elements;
};

typedef list<DashedPattern*> DashedPatternList;


/********************************************
 * ArrowStyle
 ********************************************/

class TipAngle
{
public:
	TipAngle( int value );

private:
	int _value;
};


class BaseAngle
{
public:
	BaseAngle( int value );

private:
	int _value;
};


class Length
{
public:
	Length( double value, const char* unit );

private:
	double _value;
};


class ScaleFactor
{
public:
	ScaleFactor( double value, const char* unit );

private:
	double _value;
};


class HeadType
{
public:
	HeadType( const char* value );

private:
	string _value;
};


class ScaleHead
{
public:
	ScaleHead( const char* value );

private:
	bool _value;
};



class ArrowStyleElement
{
public:
	enum ArrowStyleElementType { T_TipAngle, T_BaseAngle, T_Length,
								 T_HeadType, T_ScaleHead,
								 T_ScaleFactor };

	ArrowStyleElement( TipAngle* element );
	ArrowStyleElement( BaseAngle* element );
	ArrowStyleElement( Length* element );
	ArrowStyleElement( HeadType* element );
	ArrowStyleElement( ScaleHead* element );
	ArrowStyleElement( ScaleFactor* element );

	ArrowStyleElementType type() const { return _type; }

private:
	ArrowStyleElementType _type;
	union {
		TipAngle* _tipangle;
		BaseAngle* _baseangle;
		Length* _length;
		HeadType* _headtype;
		ScaleHead* _scalehead;
		ScaleFactor* _scalefactor;
	};
};


typedef list<ArrowStyleElement*> ArrowStyleElementList;

class ArrowStyle
{
public:
	ArrowStyle( ArrowStyleElementList* elements );

	ArrowStyleElementList* elements() const { return _elements; }

private:
	ArrowStyleElementList* _elements;
};

typedef list<ArrowStyle*> ArrowStyleList;


/***************************************
 * Ellipse
 **************************************/

class EllipseElement
{
public:
	enum EllipseElementType { T_ShapeRect, T_BRect, T_Unique, T_Pen,
							  T_PenWidth, T_Fill, T_Separation,
							  T_ObColor, T_GroupID };

	EllipseElement( ShapeRect* element );
	EllipseElement( BRect* element );
	EllipseElement( Unique* element );
	EllipseElement( Pen* element );
	EllipseElement( PenWidth* element );
	EllipseElement( Fill* element );
	EllipseElement( Separation* element );
	EllipseElement( ObColor* element );
	EllipseElement( GroupID* element );

	EllipseElementType type() const { return _type; }

private:
	EllipseElementType _type;
	union {
		ShapeRect* _shaperect;
		BRect* _brect;
		Unique* _unique;
		Pen* _pen;
		PenWidth* _penwidth;
		Fill* _fill;
		Separation* _separation;
		ObColor* _obcolor;
		GroupID* _groupid;
	};
};

typedef list<EllipseElement*> EllipseElementList;

class Ellipse
{
public:
	Ellipse( EllipseElementList* elements );

	EllipseElementList* elements() const { return _elements; }

private:
	EllipseElementList* _elements;
};

typedef list<Ellipse*> EllipseList;


/****************************************
 * Polygon
 ***************************************/

class PolygonElement
{
public:
	enum PolygonElementType { T_Smoothed, T_NumPoints, T_Point,
							  T_Unique, T_Fill, T_Pen, T_PenWidth,
							  T_Separation, T_ObColor,
							  T_DashedPattern, T_RunAroundGap,
							  T_RunAroundType, T_GroupID };

	PolygonElement( Smoothed* element );
	PolygonElement( NumPoints* element );
	PolygonElement( Point* element );
	PolygonElement( Unique* element );
	PolygonElement( Fill* element );
	PolygonElement( Pen* element );
	PolygonElement( PenWidth* element );
	PolygonElement( Separation* element );
	PolygonElement( ObColor* element );
	PolygonElement( DashedPattern* element );
	PolygonElement( RunAroundGap* element );
	PolygonElement( RunAroundType* element );
	PolygonElement( GroupID* element );

	PolygonElementType* type() const { return _type; }

private:
	PolygonElementType* _type;
	union {
		Smoothed* _smoothed;
		NumPoints* _numpoints;
		Point* _point;
		Unique* _unique;
		Fill* _fill;
		Pen* _pen;
		PenWidth* _penwidth;
		Separation* _separation;
		ObColor* _obcolor;
		DashedPattern* _dashedpattern;
		RunAroundGap* _runaroundgap;
		RunAroundType* _runaroundtype;
		GroupID* _groupid;
	};
};


typedef list<PolygonElement*> PolygonElementList;

class Polygon
{
public:
	Polygon( PolygonElementList* elements );

	PolygonElementList* elements() const { return _elements; }

private:
	PolygonElementList* _elements;
};

typedef list<Polygon*> PolygonList;





/****************************************
 * PolyLine
 ***************************************/

class HeadCap
{
public:
	HeadCap( const char* value );

private:
	string _value;
};


class TailCap
{
public:
	TailCap( const char* value );

private:
	string _value;
};


class Smoothed
{
public:
	Smoothed( const char* value );

private:
	bool _value;
};



class NumPoints
{
public:
	NumPoints( int value );

private:
	int _value;
};


class Point
{
public:
	Point( double x, const char* unitx,
		   double y, const char* unity );

private:
	double _x;
	double _y;
};


class PolyLineElement
{
public:
	enum PolyLineElementType { T_HeadCap, T_TailCap, T_ArrowStyle,
							   T_Smoothed, T_NumPoints, T_Point,
							   T_Unique, T_Pen, T_PenWidth,
							   T_RunAroundGap, T_RunAroundType,
							   T_Fill, T_Separation, T_ObColor,
							   T_GroupID };

	PolyLineElement( HeadCap* element );
	PolyLineElement( TailCap* element );
	PolyLineElement( ArrowStyle* element );
	PolyLineElement( Smoothed* element );
	PolyLineElement( NumPoints* element );
	PolyLineElement( Point* element );
	PolyLineElement( Unique* element );
	PolyLineElement( Pen* element );
	PolyLineElement( PenWidth* element );
	PolyLineElement( RunAroundType* element );
	PolyLineElement( RunAroundGap* element );
	PolyLineElement( Fill* element );
	PolyLineElement( Separation* element );
	PolyLineElement( ObColor* element );
	PolyLineElement( GroupID* element );

	PolyLineElementType type() const { return _type; }

private:
	PolyLineElementType _type;
	union {
		HeadCap* _headcap;
		TailCap* _tailcap;
		ArrowStyle* _arrowstyle;
		Smoothed* _smoothed;
		NumPoints* _numpoints;
		Point* _points;
		Unique* _unique;
		Pen* _pen;
		PenWidth* _penwidth;
		RunAroundGap* _runaroundgap;
		RunAroundType* _runaroundtype;
		Fill* _fill;
		Separation* _separation;
		ObColor* _obcolor;
		GroupID* _groupid;
	};
};



typedef list<PolyLineElement*> PolyLineElementList;

class PolyLine
{
public:
	PolyLine( PolyLineElementList* elements );

	PolyLineElementList* elements() const { return _elements; }

private:
	PolyLineElementList* _elements;
};

typedef list<PolyLine*> PolyLineList;




#endif
