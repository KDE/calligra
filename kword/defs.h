/* This file is part of the KDE project
   Copyright (C) 1998, 1999 Reginald Stadlbauer <reggie@kde.org>, Torben Weis <weis@kde.org>

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
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#ifndef defs_h
#define defs_h

#include <qtextstream.h>
#include <qstring.h>
#include <koGlobal.h>
#include "kwfactory.h"

#define MIME_TYPE "application/x-kword"

enum MouseMode {
    MM_EDIT = 0,
    //MM_EDIT_FRAME = 1,
    MM_CREATE_TEXT = 2,
    MM_CREATE_PIX = 3,
    MM_CREATE_TABLE = 5,
    MM_CREATE_FORMULA = 6,
    MM_CREATE_PART = 7
};

enum KWTblCellSize {
    TblAuto = 0,
    TblManual
};

enum TypeStructDocItem {Arrangement, Tables, Pictures, Cliparts, TextFrames, Embedded, FormulaFrames};

#define KWBarIcon( x ) BarIcon( x, KWFactory::global() )

#define DEBUGRECT(rc) (rc).x() << "," << (rc).y() << " " << (rc).width() << "x" << (rc).height()
#define DEBUGREGION(reg) { QArray<QRect>rs=reg.rects(); for (int i=0;i<rs.size();++i) \
                           kdDebug()<<"  "<<DEBUGRECT(rs[i] )<<endl; }

const unsigned int minFrameWidth=18;
const unsigned int minFrameHeight=20;
const unsigned int tableCellSpacing=3;

/** Runaround types
* RA_NO = No run around, all text is just printed.
* RA_BOUNDINGRECT = run around the square of this frame.
* RA_SKIP = stop running text on the whole horizontal space this frame occupies.
*/
enum RunAround { RA_NO = 0, RA_BOUNDINGRECT = 1, RA_SKIP = 2 };

/** what should happen when the frame is full */
enum FrameBehaviour { AutoExtendFrame=0 , AutoCreateNewFrame=1, Ignore=2 };

/** types of behaviours for creating a followup frame on new page */
enum NewFrameBehaviour { Reconnect=0, NoFollowup=1, Copy=2 };

/** This frame will only be copied to:
*   AnySide, OddSide or EvenSide
*/
enum SheetSide { AnySide=0, OddSide=1, EvenSide=2};

/** The different types of framesets
* FT_BASE = unused <br>
* FT_TEXT = text only, this is the only frameset that can have multiple frames. <br>
* FT_PICTURE = One frame with a picture<br>
* FT_PART = one frame with an embedded part, can be a spreadsheet to a kword doc.<br>
* FT_FORMULA = one frame with an embedded formula frame. This is semi-native
* FT_TABLE = Frameset which contains table cells.
*/
enum FrameType { FT_BASE = 0, FT_TEXT = 1, FT_PICTURE = 2, FT_PART = 3, FT_FORMULA = 4, FT_TABLE };

/** The different types of textFramesets (that TEXT is important here!)
* FI_BODY = normal text frames.<br>
* FI_FIRST_HEADER = Header on page 1<br>
* FI_ODD_HEADER = header on any odd page (can be including page 1)<br>
* FI_EVEN_HEADER = header on any even page<br>
* FI_FIRST_FOOTER = footer on page 1<br>
* FI_ODD_FOOTER = footer on any odd page (can be including page 1)<br>
* FI_EVEN_FOOTER = footer on any even page<br>
* FI_FOOTNOTE = a footnote frame.
*/
enum FrameInfo { FI_BODY = 0, FI_FIRST_HEADER = 1, FI_ODD_HEADER = 2, FI_EVEN_HEADER = 3,
                 FI_FIRST_FOOTER = 4, FI_ODD_FOOTER = 5, FI_EVEN_FOOTER = 6,
                 FI_FOOTNOTE = 7 };


enum TypeOfCase { UpperCase =0, LowerCase=1, TitleCase=2, ToggleCase=3};

#endif
