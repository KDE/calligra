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
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#ifndef kwanchor_h
#define kwanchor_h

#include "KWTextDocument.h"
class KCommand;
class KWTextFrameSet;
class KWFrame;
class KWFrameSet;

/**
 * An anchor is a special character, or 'custom item'.
 * It never appears as such. It is as big as the frame it is related to,
 * so that the frame is effectively inline in the text.
 */
class KWAnchor : public KoTextCustomItem
{
public:
    /**
     * Constructor.
     * @param textdoc the document this ancher will be one character of.
     * @param frameset The frameset that is anchored. This frameset contains the content we are
              displaying.
     * @param frameNum  Which frame of the frameset (previos argument) is used for displaying.
     */
    KWAnchor( KoTextDocument *textdoc, KWFrameSet * frameset, int frameNum );
    ~KWAnchor();
    bool ownLine() const;
    virtual void setFormat( KoTextFormat* );

    /** The frameset that will provide the content to display in this anchor object */
    KWFrameSet * frameSet() const { return m_frameset; }
    /** The index the frameset needs to access which content is displayed in the anchor object */
    int frameNum() const { return m_frameNum; }

    /** Return the size of the item, i.e. the size of the frame (zoomed) */
    QSize size() const;

    /* overloaded methods, see lib/kotext/kotextcustomitem.h for docu*/
    virtual void resize();
    virtual void finalize();

    /* overloaded methods, see korichtext.h for docu*/
    virtual Placement placement() const { return PlaceInline; }
    virtual int widthHint() const { return size().width(); }
    virtual int minimumWidth() const { return size().width(); }
    virtual int ascent() const;

    virtual void draw( QPainter* p, int x, int y, int cx, int cy, int cw, int ch, const QColorGroup& cg, bool selected );
    /** Never called since we reimplement draw */
    virtual void drawCustomItem(QPainter*, int, int, int, int, int, int, int, int, int, const QColorGroup&, bool, int, bool) { }

    virtual KCommand * createCommand();
    virtual KCommand * deleteCommand();
    virtual void setDeleted( bool b );
    virtual void save( QDomElement &formatElem );
    virtual void saveOasis( KoXmlWriter& writer, KoSavingContext& context ) const;

    virtual int typeId() const { return 6; }

private:
    KWFrameSet * m_frameset; // the frameset that implements the content for this special char.
    int m_frameNum;          // the reference the frameset needs to identify the content in its
                             // set of frames
};

#endif
