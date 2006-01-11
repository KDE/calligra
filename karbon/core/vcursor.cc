/* This file is part of the KDE project
   Copyright (C) 2006 The Karbon Developers

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#include "vcursor.h"
#include <qbitmap.h>

static char *cross_mask[]={
"16 16 2 1",
"# c #000000",
". c #ffffff",
"......###.......",
"......###.......",
"......###.......",
"......###.......",
"......###.......",
"......###.......",
"###############.",
"#######.#######.",
"###############.",
"......###.......",
"......###.......",
"......###.......",
"......###.......",
"......###.......",
"......###.......",
"................"};

static char *cross_bitmap[]={
"16 16 2 1",
"# c #000000",
". c #ffffff",
".......#........",
".......#........",
".......#........",
".......#........",
".......#........",
".......#........",
".......#........",
"#######.#######.",
".......#........",
".......#........",
".......#........",
".......#........",
".......#........",
".......#........",
".......#........",
"................"};

static const char* const cminus[] = {
"16 16 6 1",
"  c Gray0",
". c #939393",
"X c Gray63",
"o c #aeaeae",
"O c None",
"+ c Gray100",
"OOOOo    XXoOOOO",
"OOo  ++++  XoOOO",
"OO ++++++++ XoOO",
"Oo ++++++++ XXoO",
"O ++++++++++ XoO",
"O ++      ++ XoO",
"O ++      ++ XoO",
"O ++++++++++ XoO",
"Oo ++++++++ .oOO",
"OO ++++++++ .oOO",
"OOo  ++++   .oOO",
"OOOOo    O   XoO",
"OOOOOOOOOOO   Xo",
"OOOOOOOOOOOO   X",
"OOOOOOOOOOOOO   ",
"OOOOOOOOOOOOOO  "
};

static const char* const cplus[] = {
"16 16 6 1",
"  c Gray0",
". c #939393",
"X c Gray63",
"o c #aeaeae",
"O c None",
"+ c Gray100",
"OOOo    XXoOOOOO",
"Oo  ++++  XoOOOO",
"O ++++++++ XoOOO",
"o +++  +++ XXoOO",
" ++++  ++++ XoOO",
" ++      ++ XoOO",
" ++      ++ XoOO",
" ++++  ++++ XoOO",
"o +++  +++ .oOOO",
"O ++++++++ .oOOO",
"Oo  ++++   .oOOO",
"OOOo    O   XoOO",
"OOOOOOOOOO   XoO",
"OOOOOOOOOOO   XO",
"OOOOOOOOOOOO   O",
"OOOOOOOOOOOOO  O"
};

QCursor VCursor::createCursor( CursorType type )
{
	switch( type )
	{
		case CrossHair:
			return createCursor( (const char**) cross_bitmap, (const char**) cross_mask, 7, 7 );
		break;
		case ZoomPlus:
			return QCursor( QPixmap( ( const char**) cplus ), -1, -1 );
		break;
		case ZoomMinus:
			return QCursor( QPixmap( ( const char**) cminus ), -1, -1 );
		break;
		default: return QCursor( Qt::arrowCursor );
	}
}

QCursor VCursor::createCursor( const char * bitmap[], const char * mask[], int hotX, int hotY )
{
	// the cursor bitmap and mask
	QBitmap b, m;

	b = QPixmap( (const char**) bitmap );
	m = QPixmap( (const char**) mask );
	
	return QCursor( b, m, hotX, hotY );
}
