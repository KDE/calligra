/* $Id$
 *
 * This file is part of MIFParse, a MIF parser for Unix.
 *
 * Copyright (C) 1998 by Matthias Kalle Dalheimer <kalle@dalheimer.de>
 */

#include "treebuild_textline.h"
#include "unitconv.h"

TextLine::TextLine(TextLineElementList * elements)
{
	_elements = elements;
}


TextLineElement::TextLineElement(Unique * element)
{
	_type = T_Unique;
	_unique = element;
}


TextLineElement::TextLineElement(Separation * element)
{
	_type = T_Separation;
	_separation = element;
}


TextLineElement::TextLineElement(ObColor * element)
{
	_type = T_ObColor;
	_obcolor = element;
}


TextLineElement::TextLineElement(DashedPattern * element)
{
	_type = T_DashedPattern;
	_dashedpattern = element;
}


TextLineElement::TextLineElement(TLOrigin * element)
{
	_type = T_TLOrigin;
	_tlorigin = element;
}


TextLineElement::TextLineElement(TLAlignment * element)
{
	_type = T_TLAlignment;
	_tlalignment = element;
}


TextLineElement::TextLineElement(Font * element)
{
	_type = T_Font;
	_font = element;
}


TextLineElement::TextLineElement(String * element)
{
	_type = T_String;
	_string = element;
}


TextLineElement::TextLineElement(RunAroundGap * element)
{
	_type = T_RunAroundGap;
	_runaroundgap = element;
}


TextLineElement::TextLineElement(RunAroundType * element)
{
	_type = T_RunAroundType;
	_runaroundtype = element;
}


TextLineElement::TextLineElement(GroupID * element)
{
	_type = T_GroupID;
	_groupid = element;
}


TLOrigin::TLOrigin(double x, char const * unitx, 
				   double y, char const * unity)
{
	_x = unitconversion( x, unitx );
	_y = unitconversion( y, unity );
}

TLAlignment::TLAlignment(char const * value)
{
	_value = value;
}


