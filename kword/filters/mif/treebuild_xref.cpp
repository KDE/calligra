/* $Id$
 *
 * This file is part of MIFParse, a MIF parser for Unix.
 *
 * Copyright (C) 1998 by Matthias Kalle Dalheimer <kalle@dalheimer.de>
 */

#include "treebuild_xref.h"

XRefName::XRefName(char const * value)
{
	_value = value;
}


XRef::XRef(XRefElementList * elements)
{
	_elements = elements;
}


XRefElement::XRefElement(XRefName * element)
{
	_type = T_XRefName;
	_xrefname = element;
}


XRefElement::XRefElement(XRefSrcText * element)
{
	_type = T_XRefSrcText;
	_xrefsrctext = element;
}


XRefElement::XRefElement(XRefSrcFile * element)
{
	_type = T_XRefSrcFile;
	_xrefsrcfile = element;
}


XRefElement::XRefElement(XRefSrcIsElem * element)
{
	_type = T_XRefSrcIsElem;
	_xrefsrciselem = element;
}


XRefElement::XRefElement(XRefLastUpdate * element)
{
	_type = T_XRefLastUpdate;
	_xreflastupdate = element;
}


XRefElement::XRefElement(Unique * element)
{
	_type = T_Unique;
	_unique = element;
}


XRefSrcText::XRefSrcText(char const * value)
{
	_value = value;
}


XRefSrcIsElem::XRefSrcIsElem(char const * value)
{
	_value = value;
}


XRefSrcFile::XRefSrcFile(char const * value)
{
	_value = value;
}


XRefLastUpdate::XRefLastUpdate(int value)
{
	_value = value;
}



