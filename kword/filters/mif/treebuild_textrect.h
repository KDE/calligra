/* $Id$
 *
 * This file is part of MIFParse, a MIF parser for Unix.
 *
 * Copyright (C) 1998 by Matthias Kalle Dalheimer <kalle@dalheimer.de>
 */

#ifndef _TREEBUILD_TEXTRECT_H
#define _TREEBUILD_TEXTRECT_H

#include <string>
#include <qlist.h>

class ShapeRect;
class BRect;
class Angle;
class DashedPattern;
class FrameID;
class Unique;
class Pen;
class PenWidth;
class Fill;
class Separation;
class ObColor;
class RunAroundType;
class RunAroundGap;

class TRNumColumns
{
public:
	TRNumColumns( int value );

private:
	int _value;
};


class TRColumnGap
{
public:
	TRColumnGap( double value, const char* unit );

private:
	double _value;
};



class TRColumnBalance
{
public:
	TRColumnBalance( const char* value );

private:
	bool _value;
};



class TRSideheadWidth
{
public:
	TRSideheadWidth( double value, const char* unit );

private:
	double _value;
};


class TRSideheadGap
{
public:
	TRSideheadGap( double value, const char* unit );

private:
	double _value;
};


class TRSideheadPlacement
{
public:
	TRSideheadPlacement( const char* value );

private:
	string _value;
};



class TRNext
{
public:
	TRNext( int value );

private:
	int _value;
};





class TextRectElement
{
public:
	enum TextRectElementType { T_ShapeRect, T_BRect, T_TRNext, T_TRNumColumns,
							   T_TRColumnGap, T_TRColumnBalance, 
							   T_TRSideheadWidth, T_TRSideheadGap,
							   T_TRSideheadPlacement, T_FrameID, T_Unique,
							   T_Pen, T_PenWidth, T_Separation, T_ObColor,
							   T_Fill, T_DashedPattern, T_Angle,
							   T_RunAroundGap, T_RunAroundType };

	TextRectElement( ShapeRect* element );
	TextRectElement( BRect* element );
	TextRectElement( TRNext* element );
	TextRectElement( TRNumColumns* element );
	TextRectElement( TRColumnGap* element );
	TextRectElement( TRColumnBalance* element );
	TextRectElement( TRSideheadWidth* element );
	TextRectElement( TRSideheadGap* element );
	TextRectElement( TRSideheadPlacement* element );
	TextRectElement( FrameID* element );
	TextRectElement( Unique* element );
	TextRectElement( Fill* element );
	TextRectElement( Pen* element );
	TextRectElement( PenWidth* element );
	TextRectElement( Separation* element );
	TextRectElement( ObColor* element );
	TextRectElement( DashedPattern* element );
	TextRectElement( Angle* element );
	TextRectElement( RunAroundType* element );
	TextRectElement( RunAroundGap* element );

	TextRectElementType type() const { return _type; }

	FrameID* frameID() const { return _type == T_FrameID ? 
								   _frameid : 0; } 
	BRect* bRect() const { return _type == T_BRect ? _brect : 0; }

private:
	TextRectElementType _type;
	union {
		ShapeRect* _shaperect;
		BRect* _brect;
		TRNext* _trnext;
		TRNumColumns* _trnumcolumns;
		TRColumnGap* _trcolumngap;
		TRColumnBalance* _trcolumnbalance;
		TRSideheadWidth* _trsideheadwidth;
		TRSideheadGap* _trsideheadgap;
		TRSideheadPlacement* _trsideheadplacement;
		FrameID* _frameid;
		Unique* _unique;
		Fill* _fill;
		Pen* _pen;
		PenWidth* _penwidth;
		Separation* _separation;
		ObColor* _obcolor;
		DashedPattern* _dashedpattern;
		Angle* _angle;
		RunAroundGap* _runaroundgap;
		RunAroundType* _runaroundtype;
	};
};


typedef QList<TextRectElement> TextRectElementList;

class TextRect
{
public:
	TextRect( TextRectElementList* elements );

	TextRectElementList* elements() const { return _elements; }
	int id();
	BRect* bRect();

private:
	TextRectElementList* _elements;
};

typedef QList<TextRect> TextRectList;


#endif
