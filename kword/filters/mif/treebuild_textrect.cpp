/* $Id$
 *
 * This file is part of MIFParse, a MIF parser for Unix.
 *
 * Copyright (C) 1998 by Matthias Kalle Dalheimer <kalle@dalheimer.de>
 */

#include "treebuild_textrect.h"
#include "unitconv.h"
#include "treebuild_general.h"

#include <qlist.h>

TextRect::TextRect(TextRectElementList * elements)
{
	_elements = elements;
}


TextRectElement::TextRectElement(FrameID * element)
{
	_type = T_FrameID;
	_frameid = element;
}


TextRectElement::TextRectElement(Unique * element)
{
	_type = T_Unique;
	_unique = element;
}


TextRectElement::TextRectElement(Pen * element)
{
	_type = T_Pen;
	_pen = element;
}


TextRectElement::TextRectElement(Fill * element)
{
	_type = T_Fill;
	_fill = element;
}


TextRectElement::TextRectElement(PenWidth * element)
{
	_type = T_PenWidth;
	_penwidth = element;
}


TextRectElement::TextRectElement(Separation * element)
{
	_type = T_Separation;
	_separation = element;
}


TextRectElement::TextRectElement(ObColor * element)
{
	_type = T_ObColor;
	_obcolor = element;
}


TextRectElement::TextRectElement(DashedPattern * element)
{
	_type = T_DashedPattern;
	_dashedpattern = element;
}


TextRectElement::TextRectElement(Angle * element)
{
	_type = T_Angle;
	_angle = element;
}


TextRectElement::TextRectElement(ShapeRect * element)
{
	_type = T_ShapeRect;
	_shaperect = element;
}


TextRectElement::TextRectElement(BRect * element)
{
	_type = T_BRect;
	_brect = element;
}


TextRectElement::TextRectElement(TRNumColumns * element)
{
	_type = T_TRNumColumns;
	_trnumcolumns = element;
}


TextRectElement::TextRectElement(TRColumnGap * element)
{
	_type = T_TRColumnGap;
	_trcolumngap = element;
}


TextRectElement::TextRectElement(TRColumnBalance * element)
{
	_type = T_TRColumnBalance;
	_trcolumnbalance = element;
}


TextRectElement::TextRectElement(TRSideheadWidth * element)
{
	_type = T_TRSideheadWidth;
	_trsideheadwidth = element;
}


TextRectElement::TextRectElement(TRSideheadGap * element)
{
	_type = T_TRSideheadGap;
	_trsideheadgap = element;
}


TextRectElement::TextRectElement(TRSideheadPlacement * element)
{
	_type = T_TRSideheadPlacement;
	_trsideheadplacement = element;
}


TextRectElement::TextRectElement(TRNext * element)
{
	_type = T_TRNext;
	_trnext = element;
}


TextRectElement::TextRectElement(RunAroundGap * element)
{
	_type = T_RunAroundGap;
	_runaroundgap = element;
}


TextRectElement::TextRectElement(RunAroundType * element)
{
	_type = T_RunAroundType;
	_runaroundtype = element;
}


TRNumColumns::TRNumColumns(int value)
{
	_value = value;
}

TRColumnGap::TRColumnGap(double value, char const* unit)
{
	_value = unitconversion( value, unit );
}

TRColumnBalance::TRColumnBalance(char const * value)
{
	_value = value;
}

TRSideheadWidth::TRSideheadWidth(double value, char const* unit)
{
	_value = unitconversion( value, unit );
}

TRSideheadGap::TRSideheadGap(double value, char const* unit)
{
	_value = unitconversion( value, unit );
}

TRSideheadPlacement::TRSideheadPlacement(char const * value)
{
	_value = value;
}

TRNext::TRNext(int value)
{
	_value = value;
}


/*!
 * Return the ID of the TextRect object.
 */
int TextRect::id()
{
	QListIterator<TextRectElement> ei( *_elements );
	TextRectElement* el = ei.current();
	while( el ) {
		++ei;
		if( el->type() == TextRectElement::T_FrameID ) {
			FrameID* fid = el->frameID();
			return fid->id();
		}
		el = ei.current();
	}
	return -1;
}


/*!
 * Return the BRect of the TextRect object.
 */
BRect* TextRect::bRect()
{
	QListIterator<TextRectElement> ei( *_elements );
	TextRectElement* el = ei.current();
	while( el ) {
		++ei;
		if( el->type() == TextRectElement::T_BRect ) {
			return el->bRect();
		}
		el = ei.current();
	}
	return 0;
}


