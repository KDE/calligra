/* $Id$
 *
 * This file is part of MIFParse, a MIF parser for Unix.
 *
 * Copyright (C) 1998 by Matthias Kalle Dalheimer <kalle@dalheimer.de>
 */

#include "treebuild_fnote.h"

FNote::FNote(FNoteElementList * elements)
{
	_elements = elements;
}


FNote::FNote(int value)
{
	_value = value;
}


FNoteElement::FNoteElement(FrameID * element)
{
	_type = T_FrameID;
	_frameid = element;
}


FNoteElement::FNoteElement(Para * element)
{
	_type = T_Para;
	_para = element;
}




