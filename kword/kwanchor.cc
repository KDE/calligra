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

#include "kwanchor.h"
#include "kwtextdocument.h"
#include "kwtextframeset.h"
#include "kwdoc.h"
#include <kdebug.h>

KWAnchor::KWAnchor( KWTextDocument *textdoc, KWFrame * frame )
    : QTextCustomItem( textdoc ), m_frame( frame )
{
    resize();
}

void KWAnchor::draw( QPainter* p, int x, int y, int cx, int cy, int cw, int ch, const QColorGroup& cg )
{
    if ( placement() != PlaceInline ) {
        x = xpos;
        y = ypos;
    }

    int paragy = paragraph()->rect().y();
    kdDebug() << "KWAnchor::draw " << x << "," << y << "  paragy=" << paragy << "  " << DEBUGRECT( QRect( cx,cy,cw,ch ) ) << endl;
    KWDocument * doc = m_frame->getFrameSet()->kWordDocument();
    KWTextFrameSet * fs = textDocument()->textFrameSet();
    QPoint cPoint;
    if ( fs->internalToContents( QPoint( x, y+paragy ), cPoint ) )
    {
        kdDebug() << "KWAnchor::draw moving frame to [zoomed pos] " << cPoint.x() << "," << cPoint.y() << endl;
        // Move the frame to position x,y.
        m_frame->moveTopLeft( QPoint( cPoint.x() / doc->zoomedResolutionX(), cPoint.y() / doc->zoomedResolutionY() ) );
    }
    QColorGroup cg2( cg );
    // Determine crect in contents coords
    QRect crect( cx > 0 ? cx : 0, cy+paragy, cw, ch );
    if ( fs->internalToContents( crect.topLeft(), cPoint ) )
        crect.moveTopLeft( cPoint );
    // Draw the frame
    p->translate( 0, -paragy ); // undo what the caller did
    m_frame->getFrameSet()->drawContents( p, crect, cg2, false /*?*/, false /*?*/ );
    p->translate( 0, paragy );
}

QSize KWAnchor::size() const
{
    KWDocument * doc = m_frame->getFrameSet()->kWordDocument();
    return QSize( doc->zoomItX( m_frame->width() ), doc->zoomItY( m_frame->height() ) );
}

void KWAnchor::resize()
{
    QSize s = size();
    width = s.width();
    height = s.height();
    kdDebug() << "KWAnchor::resize " << width << "x" << height << endl;
}

KWTextDocument * KWAnchor::textDocument() const
{
    return static_cast<KWTextDocument *>( parent );
}

