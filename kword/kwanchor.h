/* This file is part of the KDE project
   Copyright (C) 2001 David Faure <faure@kde.org>

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

#ifndef kwanchor_h
#define kwanchor_h

#include "kwtextdocument.h"
class KWFrame;
class KWFrameSet;

/**
 * An anchor is a special character, or 'custom item'.
 * It never appears as such. It is as big as the frame it is related to,
 * so that the frame is effectively inline in the text.
 */
class KWAnchor : public KWTextCustomItem
{
public:
    // We store the frame as frameset+framenum for undo/redo purposes
    // and as an abstract way to represent a frame so that it doesn't have
    // to be an actual KWFrame for tables.
    KWAnchor( KWTextDocument *textdoc, KWFrameSet * frameset, int frameNum );
    ~KWAnchor();

    // The frame related to this anchor.
    //KWFrame * frame() const;

    // The text document in which this anchor is
    KWTextDocument * textDocument() const;

    // Return the size of the item, i.e. the size of the frame (zoomed)
    QSize size() const;

    virtual void resize();

    virtual Placement placement() const { return PlaceInline; }
    virtual bool ownLine() const;
    virtual int widthHint() const { return size().width(); }
    virtual int minimumWidth() const { return size().width(); }

    virtual void draw( QPainter* p, int x, int y, int cx, int cy, int cw, int ch, const QColorGroup& cg );

    virtual void addDeleteCommand( KMacroCommand * macroCmd );
    virtual void save( QDomElement &formatElem );

private:
    KWFrameSet * m_frameset;
    int m_frameNum;
};

#endif
