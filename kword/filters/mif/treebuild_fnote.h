/* $Id$
 *
 * This file is part of MIFParse, a MIF parser for Unix.
 *
 * Copyright (C) 1998 by Matthias Kalle Dalheimer <kalle@dalheimer.de>
 */

#ifndef _TREEBUILD_FNOTE_H
#define _TREEBUILD_FNOTE_H

#include <qlist.h>

class FrameID;
class Para;

class FNoteElement
{
public:
	enum FNoteElementType { T_FrameID, T_Para };

	FNoteElement( FrameID* element );
	FNoteElement( Para* element );

private:
	FNoteElementType _type;

	union {
		FrameID* _frameid;
		Para* _para;
	};
};

typedef QList<FNoteElement> FNoteElementList;

class FNote
{
public:
	FNote( FNoteElementList* elements );
	FNote( int );

private:
	int _value;
	FNoteElementList* _elements;
};


#endif
