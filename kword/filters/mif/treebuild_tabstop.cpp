/* $Id$
 *
 * This file is part of MIFParse, a MIF parser for Unix.
 *
 * Copyright (C) 1998 by Matthias Kalle Dalheimer <kalle@dalheimer.de>
 */

#include "treebuild_tabstop.h"
#include "unitconv.h"

TabStop::TabStop(TabStopElementList * elements)
{
	_elements = elements;
}


TabStopElement::TabStopElement(TSX_ * element)
{
	_type = T_TSX;
	_tsx = element;
}


TabStopElement::TabStopElement(TSType * element)
{
	_type = T_TSType;
	_tstype = element;
}


TabStopElement::TabStopElement(TSLeaderStr * element)
{
	_type = T_TSLeaderStr;
	_tsleaderstr = element;
}


TabStopElement::TabStopElement(TSDecimalChar * element)
{
	_type = T_TSDecimalChar;
	_tsdecimalchar = element;
}


TSX_::TSX_(double value, const char* unit)
{
	_value = unitconversion( value, unit );
}


TSType::TSType(char const * value)
{
	_value = value;
}


TSLeaderStr::TSLeaderStr(char const * value)
{
	_value = value;
}


TSDecimalChar::TSDecimalChar(int value)
{
	_value = value;
}



