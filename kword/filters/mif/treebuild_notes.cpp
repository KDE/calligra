/* $Id$
 *
 * This file is part of MIFParse, a MIF parser for Unix.
 *
 * Copyright (C) 1998 by Matthias Kalle Dalheimer <kalle@dalheimer.de>
 */

#include "treebuild_notes.h"

Notes::Notes(NotesElementList * elements)
{
	_elements = elements;
}


NotesElement::NotesElement(FNote * element)
{
	_type = T_FNote;
	_fnote = element;
}



