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

#ifndef kwviewmode_h
#define kwviewmode_h

#include <qrect.h>
class KWCanvas;
class QPainter;
class QRegion;
class KWTextFrameSet;

/**
 * Abstract base class for KWCanvas's view modes.
 *
 * The 'normal' mode is the WYSIWYG mode ("page mode"), in which pages are one
 * below another. Since frames are in this coordinate system, the view modes
 * other than the normal mode implement a transformation between the normal mode's
 * coordinate system and its own.
 * When drawing, we use normalcoord->viewcoord and when handling mouse clicks
 * we use viewcoord->normalcoord.
 * Note that this all in zoomed (pixel) coordinates (see DESIGN file).
 *
 * The View Mode is obviously also responsible for drawing pages etc.
 */
class KWViewMode
{
protected:
    KWViewMode( KWCanvas * canvas ) : m_canvas( canvas ) {}
public:
    virtual ~KWViewMode() {}

    // Normal coord -> view coord
    virtual QPoint normalToView( const QPoint & nPoint ) = 0;

    QRect normalToView( const QRect & nRect )
    { return QRect( normalToView( nRect.topLeft() ), normalToView( nRect.bottomRight() ) ); }

    // View coord -> normal coord
    virtual QPoint viewToNormal( const QPoint & vPoint ) = 0;

    QRect viewToNormal( const QRect & nRect )
    { return QRect( viewToNormal( nRect.topLeft() ), viewToNormal( nRect.bottomRight() ) ); }

    // Size of the contents area
    virtual QSize contentsSize() = 0;

    virtual void drawPageBorders( QPainter * painter, const QRect & crect, const QRegion & emptySpaceRegion ) = 0;

    virtual void setPagesPerRow(int) {}
    virtual int pagesPerRow() {return 0;}
    virtual bool drawSelections() {return true;}
protected:
    // Draw the border around one page, and clear up the empty space in that page
    void drawOnePageBorder( QPainter * painter, const QRect & crect, const QRect & pageRect,
                            const QRegion & emptySpaceRegion );
    QRect drawRightShadow( QPainter * painter, const QRect & crect, const QRect & pageRect, int topOffset );
    QRect drawBottomShadow( QPainter * painter, const QRect & crect, const QRect & pageRect, int leftOffset );
    static const unsigned short s_shadowOffset = 3;

    KWCanvas * m_canvas;
};

// The 'normal' view mode (pages below one another)
class KWViewModeNormal : public KWViewMode
{
public:
    KWViewModeNormal( KWCanvas * canvas ) : KWViewMode( canvas ) {}
    virtual ~KWViewModeNormal() {}

    // This view mode is very easy to implement ;-P
    virtual QPoint normalToView( const QPoint & nPoint ) { return nPoint; }
    virtual QPoint viewToNormal( const QPoint & vPoint ) { return vPoint; }
    virtual QSize contentsSize();

    virtual void drawPageBorders( QPainter * painter, const QRect & crect, const QRegion & emptySpaceRegion );
};

// The 'embedded' view mode (usually a single page, no selections)
class KWViewModeEmbedded : public KWViewMode
{
public:
     KWViewModeEmbedded ( ) : KWViewMode( 0L ) {}
    virtual ~ KWViewModeEmbedded() {}

    // This view mode is very easy to implement ;-P
    virtual QPoint normalToView( const QPoint & nPoint ) { return nPoint; }
    virtual QPoint viewToNormal( const QPoint & vPoint ) { return vPoint; }
    virtual QSize contentsSize() {return QSize();}

    virtual void drawPageBorders( QPainter *, const QRect &, const QRegion & ){}
    virtual bool drawSelections() { return false; }
};



// A mode for previewing the overall document
// Pages are organized in a grid (mostly useful when zooming out a lot)
class KWViewModePreview : public KWViewMode
{
public:
    KWViewModePreview( KWCanvas * canvas, int _nbPagePerRow=4 ) : KWViewMode( canvas ),
        m_pagesPerRow(_nbPagePerRow), // TODO make configurable somehow
        m_spacing(10)
    {}
    virtual ~KWViewModePreview() {}

    virtual QPoint normalToView( const QPoint & nPoint );
    virtual QPoint viewToNormal( const QPoint & vPoint );
    virtual QSize contentsSize();
    virtual void drawPageBorders( QPainter * painter, const QRect & crect, const QRegion & emptySpaceRegion );

    virtual void setPagesPerRow(int _nb) {m_pagesPerRow=_nb;}
    virtual int pagesPerRow() {return m_pagesPerRow;}

private:
    int m_pagesPerRow;
    int m_spacing;
};

// The 'edit only one text frameset' view mode
class KWViewModeText : public KWViewMode
{
public:
    KWViewModeText( KWCanvas * canvas ) : KWViewMode( canvas ) {}
    virtual ~KWViewModeText() {}

    KWTextFrameSet *textFrameSet() const;

    virtual QPoint normalToView( const QPoint & nPoint );
    virtual QPoint viewToNormal( const QPoint & vPoint );
    virtual QSize contentsSize();

    virtual void drawPageBorders( QPainter * painter, const QRect & crect, const QRegion & emptySpaceRegion );
};

#endif
