/* $Id$
 *
 * This file is part of MIFParse, a MIF parser for Unix.
 *
 * Copyright (C) 1998 by Matthias Kalle Dalheimer <kalle@dalheimer.de>
 */

#ifndef _TREEBUILD_FRAME_H
#define _TREEBUILD_FRAME_H

#include <list>
#include <string>

class FrameID;
class ImportObject;
class ShapeRect;
class FrameType;
class Tag;
class Float;
class NSOffset;
class BLOffset;
class AnchorAlign;
class AnchorBeside;
class Cropped;
class Unique;
class Pen;
class PenWidth;
class Fill;
class Separation;
class ObColor;
class DashedPattern;
class Angle;
class TextRect;
class PolyLine;
class RunAroundGap;
class RunAroundType;
class BRect;
class Math;


class Tag
{
public:
	Tag( const char* value );

private:
	string _value;
};


class FrameType
{
public:
	FrameType( const char* value );

private:
	string _value;
};


class NSOffset
{
public:
	NSOffset( double value, const char* value );

private:
	double _value;
};


class BLOffset
{
public:
	BLOffset( double value, const char* value );

private:
	double _value;
};


class AnchorAlign
{
public:
	AnchorAlign( const char* value );

private:
	string _value;
};


class AnchorBeside
{
public:
	AnchorBeside( const char* value );

private:
	string _value;
};


class Cropped
{
public:
	Cropped( const char* value );

private:
	bool _value;
};


class FrameElement
{
public:
	enum FrameElementType { T_ShapeRect, T_FrameType, T_Tag, T_Float,
							T_NSOffset, T_BLOffset, T_AnchorAlign,
							T_AnchorBeside, T_Cropped, T_FrameID,
							T_Unique, T_Pen, T_PenWidth,
							T_Fill, T_Separation, T_ObColor,
							T_DashedPattern, T_Angle, T_ImportObject, 
							T_TextRect, T_PolyLine,
							T_RunAroundGap, T_RunAroundType,
							T_BRect, T_Math };

	FrameElement( ShapeRect* element );
	FrameElement( FrameType* element );
	FrameElement( Tag* element );
	FrameElement( Float* element );
	FrameElement( NSOffset* element );
	FrameElement( BLOffset* element );
	FrameElement( AnchorAlign* element );
	FrameElement( AnchorBeside* element );
	FrameElement( Cropped* element );
	FrameElement( FrameID* element );
	FrameElement( Unique* element );
	FrameElement( Pen* element );
	FrameElement( PenWidth* element );
	FrameElement( Fill* element );
	FrameElement( Separation* element );
	FrameElement( ObColor* element );
	FrameElement( DashedPattern* element );
	FrameElement( Angle* element );
	FrameElement( ImportObject* element );
	FrameElement( TextRect* element );
	FrameElement( PolyLine* element );
	FrameElement( RunAroundGap* element );
	FrameElement( RunAroundType* element );
	FrameElement( BRect* element );
	FrameElement( Math* element );

	FrameElementType type() const { return _type; }


private:
	FrameElementType _type;
	union {
		ShapeRect* _shaperect;
		FrameType* _frametype;
		Tag* _tag;
		Float* _float;
		NSOffset* _nsoffset;
		BLOffset* _bloffset;
		AnchorAlign* _anchoralign;
		AnchorBeside* _anchorbeside;
		Cropped* _cropped;
		FrameID* _frameid;
		Unique* _unique;
		Pen* _pen;
		PenWidth* _penwidth;
		Fill* _fill;
		Separation* _separation;
		ObColor* _obcolor;
		DashedPattern* _dashedpattern;
		Angle* _angle;
		ImportObject* _importobject;
		TextRect* _textrect;
		PolyLine* _polyline;
		RunAroundGap* _runaroundgap;
		RunAroundType* _runaroundtype;
		BRect* _brect;
		Math* _math;
	};
};

typedef list<FrameElement*> FrameElementList;

class Frame
{
public:
	Frame( FrameElementList* elements );

	FrameElementList* elements() const { return _elements; }

private:
	FrameElementList* _elements;
};

typedef list<Frame*> FrameList;

extern FrameList frames;


#endif
