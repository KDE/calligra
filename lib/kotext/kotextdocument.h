// -*- c++ -*-
// File included by qrichtext_p.h to add our own methods to KoTextDocument
// Method implementations are in kotextdocument.cc

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

// class KoZoomHandler;
// class KoTextFormatCollection;
// class KoParagVisitor;
// class KoTextFormatter;
// class KoTextParag;

public:
    /**
     * Construct a text document, i.e. a set of paragraphs
     *
     * @param zoomHandler The KoZoomHandler instance, to handle the zooming, as the name says :)
     * We need one here because KoTextFormatter needs one for formatting, currently.
     *
     * @param fc a format collection for this document. Ownership is transferred to the document. ###
     * @param formatter a text formatter for this document. If 0L, a KoTextFormatter is created.
     *  If not, ownership of the given one is transferred to the document.
     * @param createInitialParag if true, an initial KoTextParag is created. Set to false if you reimplement createParag,
     *  since the constructor can't call the reimplementation. In that case, make sure to call
     *  clear(true) in your constructor; QRT doesn't support documents without paragraphs.
     */
    KoTextDocument( KoZoomHandler *zoomHandler,
                    KoTextFormatCollection *fc, KoTextFormatter *formatter = 0L,
                    bool createInitialParag = true );

    /** Return the zoom handler associated with this document,
     * used when formatting. Don't use for any other purpose, it might disappear. */
    KoZoomHandler * formattingZoomHandler() const { return m_zoomHandler; }

    /**
     * Return the zoom handler currently used for drawing.
     * (This means, at a particular zoom level).
     * Don't call this in a method that isn't called by drawWYSIWYG, it will be 0L !
     * (a different one than zoomHandler(), in case it disappears one day,
     * to have different zoom levels in different views)
     */
    KoZoomHandler * paintingZoomHandler() const { return m_zoomHandler; }


    /** Visit all the parts of a selection.
     * Returns true, unless canceled. See KoParagVisitor. */
    bool visitSelection( int selectionId, KoParagVisitor *visitor, bool forward = true );

    /** Visit all paragraphs of the document.
     * Returns true, unless canceled. See KoParagVisitor. */
    bool visitDocument( KoParagVisitor *visitor, bool forward = true );

    /** Visit the document between those two point.
     * Returns true, unless canceled. See KoParagVisitor. */
    bool visitFromTo( KoTextParag *firstParag, int firstIndex, KoTextParag* lastParag, int lastIndex, KoParagVisitor* visitor, bool forw = true );

    /**
     * Used by ~KoTextParag to know if it should die quickly
     */
    bool isDestroying() const { return m_bDestroying; }

    /**
     * Flags for drawWYSIWYG and drawParagWYSIWYG
     */
    enum DrawingFlags {
        DrawMisspelledLine = 1,
        DrawFormattingChars = 2,
        DrawSelections = 4
    };
    /** The main drawing method. Equivalent to KoTextDocument::draw, but reimplemented
     * for wysiwyg */
    KoTextParag *drawWYSIWYG( QPainter *p, int cx, int cy, int cw, int ch, const QColorGroup &cg,
                              KoZoomHandler* zoomHandler, bool onlyChanged = FALSE,
                              bool drawCursor = FALSE, KoTextCursor *cursor = 0,
                              bool resetChanged = TRUE, uint drawingFlags = KoTextDocument::DrawSelections );

    /** Draw a single paragraph (used by drawWYSIWYG and by KWTextFrameSet::drawCursor).
     * Equivalent to KoTextDocument::draw, but modified for wysiwyg */
    void drawParagWYSIWYG( QPainter *p, KoTextParag *parag, int cx, int cy, int cw, int ch,
                           QPixmap *&doubleBuffer, const QColorGroup &cg,
                           KoZoomHandler* zoomHandler,
                           bool drawCursor, KoTextCursor *cursor,
                           bool resetChanged = TRUE,
                           uint drawingFlags = KoTextDocument::DrawSelections );

    /** Set by drawParagWYSIWYG, used by KoTextParag::drawParagStringInternal */
    bool drawingShadow() const { return m_bDrawingShadow; }
    /** Set by drawParagWYSIWYG, used by KoTextParag::drawParagString */
    bool drawFormattingChars() const { return (m_drawingFlags & DrawFormattingChars); }
    /** Set by drawParagWYSIWYG, used by KoTextParag::drawParagStringInternal */
    bool drawingMissingSpellLine() const { return (m_drawingFlags & DrawMisspelledLine); }

protected:
    void drawWithoutDoubleBuffer( QPainter *p, const QRect &rect, const QColorGroup &cg,
                                  KoZoomHandler* zoomHandler, const QBrush *paper = 0 );

private:
    // The zoom handler used when formatting
    // (due to the pixelx/pixelww stuff in KoTextFormatter)
    KoZoomHandler * m_zoomHandler;
    bool m_bDestroying;
    bool m_bDrawingShadow;
    uint m_drawingFlags;
