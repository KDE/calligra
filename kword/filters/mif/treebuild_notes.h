/* $Id$
 *
 * This file is part of MIFParse, a MIF parser for Unix.
 *
 * Copyright (C) 1998 by Matthias Kalle Dalheimer <kalle@dalheimer.de>
 */

#ifndef _TREEBUILD_NOTES
#define _TREEBUILD_NOTES

#include <qlist.h>

class FNote;

class NotesElement
{
public:
	enum NotesElementType { T_FNote };

	NotesElement( FNote* );

private:
	NotesElementType _type;

	union {
		FNote* _fnote;
	};
};

typedef QList<NotesElement> NotesElementList;

class Notes
{
public:
	Notes( NotesElementList* elements );

private:
	NotesElementList* _elements;
};

#endif
