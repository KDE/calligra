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

    virtual void drawPageBorders( QPainter * painter, const QRect & crect, const QRegion & emptySpaceRegion ) = 0;

protected:
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

    virtual void drawPageBorders( QPainter * painter, const QRect & crect, const QRegion & emptySpaceRegion );
};

// A mode for previewing the overall document
// Pages are organized in a grid (mostly useful when zooming out a lot)
class KWViewModePreview : public KWViewMode
{
public:
    KWViewModePreview( KWCanvas * canvas ) : KWViewMode( canvas ),
        m_pagesPerRow(4), // TODO make configurable somehow
        m_spacing(10)
    {}
    virtual ~KWViewModePreview() {}

    virtual QPoint normalToView( const QPoint & nPoint );
    virtual QPoint viewToNormal( const QPoint & vPoint );
    virtual void drawPageBorders( QPainter * painter, const QRect & crect, const QRegion & emptySpaceRegion );

private:
    int m_pagesPerRow;
    int m_spacing;
};

#endif
