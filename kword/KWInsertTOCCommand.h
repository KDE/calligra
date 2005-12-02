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
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#ifndef contents_h
#define contents_h

class KWTextFrameSet;
class KMacroCommand;
class KoParagStyle;
#include <KoRichText.h>

// This command inserts a TOC at the beginning of a frameset, and is able to undo that.
// The reason we don't use KWTextFrameSet's insert, applyStyle etc. is that it would
// generate many many subcommands (resulting in much memory use).
class KWInsertTOCCommand : public KoTextDocCommand
{
public:
    // The parag is where the TOC should be inserted
    KWInsertTOCCommand( KWTextFrameSet *fs, KoTextParag *parag );
    KoTextCursor *execute( KoTextCursor *c );
    KoTextCursor *unexecute( KoTextCursor *c );

    // Helper method, public for KWTextFrameSet::insertTOC().
    // Remove a toc based on the parag styles
    static KoTextCursor * removeTOC( KWTextFrameSet *fs, KoTextCursor *cursor, KMacroCommand *macroCmd );

protected:
    // Find or create a toc style
    static KoParagStyle * findOrCreateTOCStyle( KWTextFrameSet *fs, int depth );

    int m_paragId;
};



#endif
