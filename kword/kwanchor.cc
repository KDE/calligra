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

KWAnchor::KWAnchor( KWTextDocument *textdoc, KWFrame * frame )
    : QTextCustomItem( textdoc ), m_frame( frame )
{
    QSize s = size();
    width = s.width();
    height = s.height();
}

void KWAnchor::draw( QPainter* p, int x, int y, int cx, int cy, int cw, int ch, const QColorGroup& cg )
{
    if ( placement() != PlaceInline ) {
        x = xpos;
        y = ypos;
    }

    KWDocument * doc = m_frame->getFrameSet()->kWordDocument();
    // Move the frame to position x,y.
    m_frame->moveTopLeft( QPoint( x / doc->zoomedResolutionX(), y / doc->zoomedResolutionY() ) );

    QColorGroup cg2( cg );
    m_frame->getFrameSet()->drawContents( p, QRect( cx, cy, cw, ch ), cg2, false /*?*/, false /*?*/ );
}

QSize KWAnchor::size() const
{
    KWDocument * doc = m_frame->getFrameSet()->kWordDocument();
    return QSize( doc->zoomItX( m_frame->width() ), doc->zoomItY( m_frame->height() ) );
}
