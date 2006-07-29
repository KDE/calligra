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
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#ifndef defs_h
#define defs_h

#include <qtextstream.h>
#include <qstring.h>
#include <KoGlobal.h>
#include "KWFactory.h"

// Try to add new defines and enums in the most appropriate class
// instead of in this file. Only very global things should be here.

#define KWBarIcon( x ) BarIcon( x, KWFactory::instance() )

#define DEBUGRECT(rc) (rc).x() << "," << (rc).y() << " " << (rc).width() << "x" << (rc).height()
#define DEBUGREGION(reg) { QMemArray<QRect>rs=reg.rects(); for (int i=0;i<rs.size();++i) \
                           kdDebug()<<"  "<<DEBUGRECT(rs[i] )<<endl; }

const unsigned int s_minFrameWidth=4;
const unsigned int s_minFrameHeight=11;

/** The different types of framesets
 * FT_BASE = unused <br>
 * FT_TEXT = text only, this is the only frameset that can have multiple frames. <br>
 * FT_PICTURE = One frame with an picture (only image, if for KWord 1.1.x compactibility)<br>
 * FT_PART = one frame with an embedded part, can be a spreadsheet to a kword doc.<br>
 * FT_FORMULA = one frame with an embedded formula frame. This is semi-native
 * FT_CLIPART = One frame with a clipart (deprecated, only for KWord 1.1.x compactibility)<br>
 * FT_HORZLINE = reserved
 * FT_TABLE = Frameset which contains table cells.
 * Table is 10 so that in the XML file we can use 7, 8, 9 in the future.
 */
enum FrameSetType { FT_BASE = 0, FT_TEXT = 1, FT_PICTURE = 2, FT_PART = 3,
                    FT_FORMULA = 4, FT_CLIPART = 5,
                    FT_TABLE = 10 };
// This has to remain here because of KWDocument::refreshDocStructure()

// This one has better remain here, otherwise kwdoc.cc needs docstruct.h
enum TypeStructDocItem {Arrangement=1, Tables=2, Pictures=4, TextFrames=16, Embedded=32, FormulaFrames=64};

// This one has better remain here, otherwise kwtexframeset.h needs footnote.h
// Yes, if gcc was faster we would choose more modular code over compilation speed...
enum NoteType { FootNote, EndNote };

enum SeparatorLinePos { SLP_LEFT = 0, SLP_CENTERED = 1, SLP_RIGHT = 2};
enum SeparatorLineLineType { SLT_SOLID = 0, SLT_DASH = 1, SLT_DOT = 2, SLT_DASH_DOT = 3, SLT_DASH_DOT_DOT = 4};

// Shared between sortdia.h and kwtextframeset.h
enum SortType { KW_SORTINCREASE, KW_SORTDECREASE };

/**
   Relative position of the mouse compared to a frame, or "meaning of a click right there".
   Can be: nothing ; inside a frame ; or on a border.
   When clicking on a frame's border, this can be for moving it, resizing it, or simply select it. */
enum MouseMeaning { MEANING_NONE = 0, MEANING_MOUSE_INSIDE, MEANING_MOUSE_INSIDE_TEXT,
                    MEANING_MOUSE_OVER_LINK, MEANING_MOUSE_OVER_FOOTNOTE,
                    MEANING_MOUSE_MOVE, MEANING_MOUSE_SELECT, MEANING_ACTIVATE_PART,
                    MEANING_TOPLEFT, MEANING_TOP, MEANING_TOPRIGHT, MEANING_RIGHT,
                    MEANING_BOTTOMRIGHT, MEANING_BOTTOM, MEANING_BOTTOMLEFT, MEANING_LEFT,
                    MEANING_RESIZE_COLUMN, MEANING_RESIZE_ROW, MEANING_SELECT_RANGE,
                    MEANING_SELECT_COLUMN, MEANING_SELECT_ROW, MEANING_FORBIDDEN };

enum InsertPagePos { KW_INSERTPAGEAFTER=0 , KW_INSERTPAGEBEFORE=1 };


#endif
