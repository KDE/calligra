/* $Id$
 *
 * This file is part of MIFParse, a MIF parser for Unix.
 *
 * Copyright (C) 1998 by Matthias Kalle Dalheimer <kalle@dalheimer.de>
 */

#include "treebuild_graphics.h"

#include "unitconv.h"

Fill::Fill(int value)
{
	_value = value;
}

PenWidth::PenWidth(double value, char const* unit)
{
	_value = unitconversion( value, unit );
}

Separation::Separation(int value)
{
	_value = value;
}

ObColor::ObColor(char const * value)
{
	_value = value;
}

DashedPattern::DashedPattern( DashedPatternElementList* elements)
{
	_elements = elements;
}


DashedPatternElement::DashedPatternElement(DashedStyle * element)
{
	_type = T_DashedStyle;
	_dashedstyle = element;
}


DashedPatternElement::DashedPatternElement(NumSegments * element)
{
	_type = T_NumSegments;
	_numsegments = element;
}


DashedPatternElement::DashedPatternElement(DashSegment * element)
{
	_type = T_DashSegment;
	_dashsegment = element;
}


DashedStyle::DashedStyle(char const * value)
{
	_value = value;
}

NumSegments::NumSegments(int value)
{
	_value = value;
}

DashSegment::DashSegment(double value, char const* unit)
{
	_value = unitconversion( value, unit );
}

Angle::Angle(double value)
{
	_value = value;
}

RunAroundGap::RunAroundGap(double value, char const* unit)
{
	_value = unitconversion( value, unit );
}

RunAroundType::RunAroundType(char const * value)
{
	_value = value;
}

PolyLine::PolyLine(PolyLineElementList * elements)
{
	_elements = elements;
}


PolyLineElement::PolyLineElement(Unique * element)
{
	_type = T_Unique;
	_unique = element;
}


PolyLineElement::PolyLineElement(Pen * element)
{
	_type = T_Pen;
	_pen = element;
}


PolyLineElement::PolyLineElement(PenWidth * element)
{
	_type = T_PenWidth;
	_penwidth = element;
}


PolyLineElement::PolyLineElement(HeadCap * element)
{
	_type = T_HeadCap;
	_headcap = element;
}


PolyLineElement::PolyLineElement(TailCap * element)
{
	_type = T_TailCap;
	_tailcap = element;
}


PolyLineElement::PolyLineElement(ArrowStyle * element)
{
	_type = T_ArrowStyle;
	_arrowstyle = element;
}


PolyLineElement::PolyLineElement(NumPoints * element)
{
	_type = T_NumPoints;
	_numpoints = element;
}


PolyLineElement::PolyLineElement(Point * element)
{
	_type = T_Point;
	_point = element;
}


PolyLineElement::PolyLineElement(GroupID * element)
{
	_type = T_GroupID;
	_groupid = element;
}


PolyLineElement::PolyLineElement(RunAroundGap * element)
{
	_type = T_RunAroundGap;
	_runaroundgap = element;
}


PolyLineElement::PolyLineElement(RunAroundType * element)
{
	_type = T_RunAroundType;
	_runaroundtype = element;
}


PolyLineElement::PolyLineElement(Fill * element)
{
	_type = T_Fill;
	_fill = element;
}


PolyLineElement::PolyLineElement(Separation * element)
{
	_type = T_Separation;
	_separation = element;
}


PolyLineElement::PolyLineElement(ObColor * element)
{
	_type = T_ObColor;
	_obcolor = element;
}


HeadCap::HeadCap(char const * value)
{
	_value = value;
}

TailCap::TailCap(char const * value)
{
	_value = value;
}

ArrowStyle::ArrowStyle(ArrowStyleElementList * elements)
{
	_elements = elements;
}


ArrowStyleElement::ArrowStyleElement(TipAngle * element)
{
	_type = T_TipAngle;
	_tipangle = element;
}


ArrowStyleElement::ArrowStyleElement(BaseAngle * element)
{
	_type = T_BaseAngle;
	_baseangle = element;
}


ArrowStyleElement::ArrowStyleElement(Length * element)
{
	_type = T_Length;
	_length = element;
}


ArrowStyleElement::ArrowStyleElement(HeadType * element)
{
	_type = T_HeadType;
	_headtype = element;
}


ArrowStyleElement::ArrowStyleElement(ScaleHead * element)
{
	_type = T_ScaleHead;
	_scalehead = element;
}


ArrowStyleElement::ArrowStyleElement(ScaleFactor * element)
{
	_type = T_ScaleFactor;
	_scalefactor = element;
}


TipAngle::TipAngle(int value)
{
	_value = value;
}

BaseAngle::BaseAngle(int value)
{
	_value = value;
}

Length::Length(double value, char const* unit)
{
	_value = unitconversion( value, unit );
}

HeadType::HeadType(char const * value)
{
	_value = value;
}

ScaleHead::ScaleHead(char const * value)
{
	_value = value;
}

ScaleFactor::ScaleFactor(double value, char const* unit)
{
	_value = unitconversion( value, unit );
}

NumPoints::NumPoints(int value)
{
	_value = value;
}

Point::Point(double x, char const * unitx, 
			 double y, char const * unity)
{
	_x = unitconversion( x, unitx );
	_y = unitconversion( y, unity );
}

Polygon::Polygon(PolygonElementList * elements)
{
	_elements = elements;
}


PolygonElement::PolygonElement(Unique * element)
{
	_type = T_Unique;
	_unique = element;
}


PolygonElement::PolygonElement(GroupID * element)
{
	_type = T_GroupID;
	_groupid = element;
}


PolygonElement::PolygonElement(Pen * element)
{
	_type = T_Pen;
	_pen = element;
}


PolygonElement::PolygonElement(Fill * element)
{
	_type = T_Fill;
	_fill = element;
}


PolygonElement::PolygonElement(PenWidth * element)
{
	_type = T_PenWidth;
	_penwidth = element;
}


PolygonElement::PolygonElement(Separation * element)
{
	_type = T_Separation;
	_separation = element;
}


PolygonElement::PolygonElement(ObColor * element)
{
	_type = T_ObColor;
	_obcolor = element;
}


PolygonElement::PolygonElement(DashedPattern * element)
{
	_type = T_DashedPattern;
	_dashedpattern = element;
}


PolygonElement::PolygonElement(RunAroundGap * element)
{
	_type = T_RunAroundGap;
	_runaroundgap = element;
}


PolygonElement::PolygonElement(RunAroundType * element)
{
	_type = T_RunAroundType;
	_runaroundtype = element;
}


PolygonElement::PolygonElement(NumPoints * element)
{
	_type = T_NumPoints;
	_numpoints = element;
}


PolygonElement::PolygonElement(Point * element)
{
	_type = T_Point;
	_point = element;
}


Ellipse::Ellipse(EllipseElementList * elements)
{
	_elements = elements;
}


EllipseElement::EllipseElement(Unique * element)
{
	_type = T_Unique;
	_unique = element;
}


EllipseElement::EllipseElement(GroupID * element)
{
	_type = T_GroupID;
	_groupid = element;
}


EllipseElement::EllipseElement(Pen * element)
{
	_type = T_Pen;
	_pen = element;
}


EllipseElement::EllipseElement(Fill * element)
{
	_type = T_Fill;
	_fill = element;
}


EllipseElement::EllipseElement(Separation * element)
{
	_type = T_Separation;
	_separation = element;
}


EllipseElement::EllipseElement(ObColor * element)
{
	_type = T_ObColor;
	_obcolor = element;
}


EllipseElement::EllipseElement(ShapeRect * element)
{
	_type = T_ShapeRect;
	_shaperect = element;
}


EllipseElement::EllipseElement(BRect * element)
{
	_type = T_BRect;
	_brect = element;
}


GroupID::GroupID(int value)
{
	_value = value;
}

Group::Group(GroupElementList* elements)
{
	_elements = elements;
}


GroupElement::GroupElement(FrameID * element)
{
	_type = T_FrameID;
	_frameid = element;
}


GroupElement::GroupElement(Unique * element)
{
	_type = T_Unique;
	_unique = element;
}


GroupElement::GroupElement(RunAroundGap * element)
{
	_type = T_RunAroundGap;
	_runaroundgap = element;
}


GroupElement::GroupElement(RunAroundType * element)
{
	_type = T_RunAroundType;
	_runaroundtype = element;
}


GroupElement::GroupElement(GroupID * element)
{
	_type = T_GroupID;
	_groupid = element;
}




