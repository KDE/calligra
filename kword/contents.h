/* This file is part of the KDE project
   Copyright (C) 1998, 1999 Reginald Stadlbauer <reggie@kde.org>

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

#ifndef contents_h
#define contents_h

class KWTextFrameSet;
class KMacroCommand;
class KoStyle;
#include <qrichtext_p.h>
using namespace Qt3;

// This command inserts a TOC at the beginning of a frameset, and is able to undo that.
// The reason we don't use KWTextFrameSet's insert, applyStyle etc. is that it would
// generate many many subcommands (resulting in much memory use).
class KWInsertTOCCommand : public QTextCommand
{
public:
    // The parag is where the TOC should be inserted
    KWInsertTOCCommand( KWTextFrameSet *fs, QTextParag *parag );
    QTextCursor *execute( QTextCursor *c );
    QTextCursor *unexecute( QTextCursor *c );

    // Helper method, public for KWTextFrameSet::insertTOC().
    // Remove a toc based on the parag styles
    static QTextCursor * removeTOC( KWTextFrameSet *fs, QTextCursor *cursor, KMacroCommand *macroCmd );

protected:
    // Find or create a toc style
    static KoStyle * findOrCreateTOCStyle( KWTextFrameSet *fs, int depth );

    int m_paragId;
};



#endif
