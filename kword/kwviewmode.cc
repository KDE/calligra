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

#include "kwviewmode.h"
#include "kwcanvas.h"
#include "kwdoc.h"
#include <qpainter.h>

void KWViewModeNormal::drawPageBorders( QPainter * painter, const QRect & crect, const QRegion & emptySpaceRegion )
{
    KWDocument * doc = m_canvas->kWordDocument();
    painter->save();
    painter->setPen( QApplication::palette().active().color( QColorGroup::Dark ) );
    painter->setBrush( Qt::NoBrush );
    const int shadowOffset = 3;
    for ( int page = 0; page < doc->getPages(); page++ )
    {
        // using paperHeight() leads to rounding problems ( one pixel between two pages, belonging to none of them )
        int pagetop = m_canvas->pageTop( page );
        int pagewidth = doc->paperWidth();
        int pageheight = m_canvas->pageTop( page+1 ) - pagetop;
        QRect pageRect( 0, pagetop, pagewidth, pageheight );
        if ( crect.intersects( pageRect ) )
        {
            //kdDebug() << "KWDocument::drawBorders drawing page rect " << DEBUGRECT( pageRect ) << endl;
            painter->drawRect( pageRect );
            // Exclude page border line, to get the page contents rect (avoids flicker)
            pageRect.rLeft() += 1;
            pageRect.rTop() += 1;
            pageRect.rRight() -= 1;
            pageRect.rBottom() -= 1;
            // The empty space to clear up inside this page
            QRegion pageEmptyRegion = emptySpaceRegion.intersect( pageRect );
            if ( !pageEmptyRegion.isEmpty() )
                doc->eraseEmptySpace( painter, pageEmptyRegion );
        }
        if ( crect.right() > pagewidth )
        {
            // The area on the right of the page
            QRect rightArea( pagewidth, pagetop, crect.right() - pagewidth + 1, pageheight );
            QRect repaintRect = rightArea.intersect( crect );
            if ( !repaintRect.isEmpty() )
            {
                painter->fillRect( repaintRect,
                                   QApplication::palette().active().brush( QColorGroup::Mid ) );
                // Draw a shadow
                int topOffset = ( page==0 ) ? shadowOffset : 0; // leave a few pixels on top, only for first page
                QRect shadowRect( rightArea.left(), rightArea.top() + topOffset, shadowOffset, pageheight - topOffset );
                shadowRect &= repaintRect; // intersect
                if ( !shadowRect.isEmpty() )
                {
                    painter->fillRect( shadowRect,
                                       QApplication::palette().active().brush( QColorGroup::Shadow ) );
                }
            }
        }
    }
    // Take care of the area at the bottom of the last page
    int lastBottom = m_canvas->pageTop( doc->getPages() );
    if ( crect.bottom() > lastBottom )
    {
        QRect bottomArea( 0, lastBottom, crect.width(), crect.bottom() - lastBottom + 1 );
        QRect repaintRect = bottomArea.intersect( crect );
        if ( !repaintRect.isEmpty() )
        {
            painter->fillRect( repaintRect,
                               QApplication::palette().active().brush( QColorGroup::Mid ) );
            // Draw a shadow
            int leftOffset = shadowOffset; // leave a few pixels on the left
            QRect shadowRect( leftOffset, bottomArea.top(), doc->paperWidth(), shadowOffset );
            shadowRect &= repaintRect; // intersect
            if ( !shadowRect.isEmpty() )
                painter->fillRect( shadowRect,
                                   QApplication::palette().active().brush( QColorGroup::Shadow ) );
        }
    }
}

//////////////////////// Preview mode ////////////////////////////////

KoPoint KWViewModePreview::normalToView( const KoPoint & nPoint )
{
    double paperWidth = m_canvas->kWordDocument()->ptPaperWidth();
    double paperHeight = m_canvas->kWordDocument()->ptPaperHeight();
    int page = static_cast<int>( nPoint.y() / paperHeight );
    double yInPage = nPoint.y() - page * paperHeight;
    int row = page / m_pagesPerRow;
    int col = page % m_pagesPerRow;
    return KoPoint( col * ( paperWidth + m_spacing ) + nPoint.x(),
                    row * ( paperHeight + m_spacing ) + yInPage );
}

KoPoint KWViewModePreview::viewToNormal( const KoPoint & vPoint )
{
    // Well, just the opposite of the above.... hmm.... headache....
    double paperWidth = m_canvas->kWordDocument()->ptPaperWidth();
    double paperHeight = m_canvas->kWordDocument()->ptPaperHeight();
    int col = static_cast<int>( vPoint.x() / ( paperWidth + m_spacing ) );
    double xInPage = vPoint.x() - col * ( paperWidth + m_spacing );
    int row = static_cast<int>( vPoint.y() / ( paperHeight + m_spacing ) );
    double yInPage = vPoint.y() - row * ( paperHeight + m_spacing );
    int page = row * m_pagesPerRow + col;
    return KoPoint( xInPage, yInPage + page * paperHeight );
}

void KWViewModePreview::drawPageBorders( QPainter * painter, const QRect & crect, const QRegion & emptySpaceRegion )
{
    // TODO
}
