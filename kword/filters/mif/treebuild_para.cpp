/* $Id$
 *
 * This file is part of MIFParse, a MIF parser for Unix.
 *
 * Copyright (C) 1998 by Matthias Kalle Dalheimer <kalle@dalheimer.de>
 */

#include "treebuild_para.h"

Para::Para(ParaElementList * elements)
{
	_elements = elements;
}


ParaElement::ParaElement(Unique * element)
{
	_type = T_Unique;
	_unique = element;
}


ParaElement::ParaElement(Pgf * element)
{
	_type = T_Pgf;
	_pgf = element;
}


ParaElement::ParaElement(PgfTag * element)
{
	_type = T_PgfTag;
	_pgftag = element;
}


ParaElement::ParaElement(PgfNumString * element)
{
	_type = T_PgfNumString;
	_pgfnumstring = element;
}


ParaElement::ParaElement(ParaLine * element)
{
	_type = T_ParaLine;
	_paraline = element;
}


ParaLine::ParaLine(ParaLineElementList * elements )
{
	_elements = elements;
}


ParaLineElement::ParaLineElement()
{
	_type = T_NIL;
}


ParaLineElement::ParaLineElement(String * element)
{
	_type = T_String;
	_string = element;
}


ParaLineElement::ParaLineElement(Char * element)
{
	_type = T_Char;
	_char = element;
}


ParaLineElement::ParaLineElement(TextRectID * element)
{
	_type = T_TextRectID;
	_textrectid = element;
}


ParaLineElement::ParaLineElement(Variable * element)
{
	_type = T_Variable;
	_variable = element;
}


ParaLineElement::ParaLineElement(ATbl * element)
{
	_type = T_ATbl;
	_atbl = element;
}


ParaLineElement::ParaLineElement(AFrame * element)
{
	_type = T_AFrame;
	_aframe = element;
}


ParaLineElement::ParaLineElement(Font * element)
{
	_type = T_Font;
	_font = element;
}


ParaLineElement::ParaLineElement(Marker * element)
{
	_type = T_Marker;
	_marker = element;
}


ParaLineElement::ParaLineElement(XRef * element)
{
	_type = T_XRef;
	_xref = element;
}


ParaLineElement::ParaLineElement(XRefEnd * element)
{
	_type = T_XRefEnd;
	_xrefend = element;
}


ParaLineElement::ParaLineElement(FNote * element)
{
	_type = T_FNote;
	_fnote = element;
}



