/* $Id$
 *
 * This file is part of MIFParse, a MIF parser for Unix.
 *
 * Copyright (C) 1998 by Matthias Kalle Dalheimer <kalle@dalheimer.de>
 */

#include "treebuild_frame.h"
#include "unitconv.h"

Frame::Frame( FrameElementList * elements)
{
	_elements = elements;
}


FrameElement::FrameElement(FrameID * element)
{
	_type = T_FrameID;
	_frameid = element;
}


FrameElement::FrameElement(Unique * element)
{
	_type = T_Unique;
	_unique = element;
}


FrameElement::FrameElement(Pen * element)
{
	_type = T_Pen;
	_pen = element;
}


FrameElement::FrameElement(PenWidth * element)
{
	_type = T_PenWidth;
	_penwidth = element;
}


FrameElement::FrameElement(Fill * element)
{
	_type = T_Fill;
	_fill = element;
}


FrameElement::FrameElement(Separation * element)
{
	_type = T_Separation;
	_separation = element;
}


FrameElement::FrameElement(ObColor * element)
{
	_type = T_ObColor;
	_obcolor = element;
}


FrameElement::FrameElement(DashedPattern * element)
{
	_type = T_DashedPattern;
	_dashedpattern = element;
}


FrameElement::FrameElement(Angle * element)
{
	_type = T_Angle;
	_angle = element;
}


FrameElement::FrameElement(ShapeRect * element)
{
	_type = T_ShapeRect;
	_shaperect = element;
}


FrameElement::FrameElement(BRect * element)
{
	_type = T_BRect;
	_brect = element;
}


FrameElement::FrameElement(FrameType * element)
{
	_type = T_FrameType;
	_frametype = element;
}


FrameElement::FrameElement(NSOffset * element)
{
	_type = T_NSOffset;
	_nsoffset = element;
}


FrameElement::FrameElement(BLOffset * element)
{
	_type = T_BLOffset;
	_bloffset = element;
}


FrameElement::FrameElement(AnchorAlign * element)
{
	_type = T_AnchorAlign;
	_anchoralign = element;
}


FrameElement::FrameElement(Cropped * element)
{
	_type = T_Cropped;
	_cropped = element;
}


FrameElement::FrameElement(ImportObject * element)
{
	_type = T_ImportObject;
	_importobject = element;
}


FrameElement::FrameElement(TextRect * element)
{
	_type = T_TextRect;
	_textrect = element;
}


FrameElement::FrameElement(Tag * element)
{
	_type = T_Tag;
	_tag = element;
}


FrameElement::FrameElement(PolyLine * element)
{
	_type = T_PolyLine;
	_polyline = element;
}


FrameElement::FrameElement(RunAroundGap * element)
{
	_type = T_RunAroundGap;
	_runaroundgap = element;
}


FrameElement::FrameElement(RunAroundType * element)
{
	_type = T_RunAroundType;
	_runaroundtype = element;
}


FrameElement::FrameElement(Math * element)
{
	_type = T_Math;
	_math = element;
}


Tag::Tag(char const *value)
{
	_value = value;
}

FrameType::FrameType(char const *value)
{
	_value = value;
}

NSOffset::NSOffset(double value, char const * unit)
{
	_value = unitconversion( value, unit );
}

BLOffset::BLOffset(double value, char const * unit)
{
	_value = unitconversion( value, unit );
}

AnchorAlign::AnchorAlign(char const *value)
{
	_value = value;
}

Cropped::Cropped(char const *value)
{
	_value = value;
}


