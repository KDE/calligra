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
#include <kdebug.h>
#include <qpainter.h>
#include <kwtextframeset.h>

QSize KWViewModeNormal::contentsSize()
{
    KWDocument * doc = m_canvas->kWordDocument();
    return QSize( doc->paperWidth(), doc->pageTop( doc->getPages() ) /*i.e. bottom of last one*/ );
}

void KWViewMode::drawOnePageBorder( QPainter * painter, const QRect & crect, const QRect & _pageRect,
                                    const QRegion & emptySpaceRegion )
{
    if ( !crect.intersects( _pageRect ) )
        return;

    QRect pageRect( _pageRect );
    //kdDebug() << "KWViewMode::drawOnePageBorder drawing page rect " << DEBUGRECT( pageRect ) << endl;
    painter->drawRect( pageRect );
    // Exclude page border line, to get the page contents rect (avoids flicker)
    pageRect.rLeft() += 1;
    pageRect.rTop() += 1;
    pageRect.rRight() -= 1;
    pageRect.rBottom() -= 1;
    // The empty space to clear up inside this page
    QRect pagecrect = pageRect.intersect( crect );
    if ( !pagecrect.isEmpty() )
    {
        //kdDebug() << "KWViewMode::drawOnePageBorder : emptySpaceRegion: " << endl; DEBUGREGION( emptySpaceRegion );
        //kdDebug() << "KWViewMode::drawOnePageBorder pagecrect=" << DEBUGRECT( pagecrect ) << endl;

        QRegion pageEmptyRegion = emptySpaceRegion.intersect( pagecrect );
        //kdDebug() << "RESULT: pageEmptyRegion: " << endl; DEBUGREGION( pageEmptyRegion );
        if ( !pageEmptyRegion.isEmpty() )
            m_canvas->kWordDocument()->eraseEmptySpace( painter, pageEmptyRegion, QApplication::palette().active().brush( QColorGroup::Base ) );
    }
}

QRect KWViewMode::drawRightShadow( QPainter * painter, const QRect & crect, const QRect & pageRect, int topOffset )
{
    QRect shadowRect( pageRect.right() + 1, pageRect.top() + topOffset, s_shadowOffset, pageRect.height() - topOffset );
    shadowRect &= crect; // intersect
    if ( !shadowRect.isEmpty() )
    {
        painter->fillRect( shadowRect,
                           QApplication::palette().active().brush( QColorGroup::Shadow ) );
    }
    return shadowRect;
}

QRect KWViewMode::drawBottomShadow( QPainter * painter, const QRect & crect, const QRect & pageRect, int leftOffset )
{
    QRect shadowRect( pageRect.left() + leftOffset, pageRect.bottom() + 1, pageRect.width(), s_shadowOffset );
    shadowRect &= crect; // intersect
    if ( !shadowRect.isEmpty() )
        painter->fillRect( shadowRect,
                           QApplication::palette().active().brush( QColorGroup::Shadow ) );
    return shadowRect;
}

void KWViewModeNormal::drawPageBorders( QPainter * painter, const QRect & crect, const QRegion & emptySpaceRegion )
{
    KWDocument * doc = m_canvas->kWordDocument();
    painter->save();
    painter->setPen( QApplication::palette().active().color( QColorGroup::Dark ) );
    painter->setBrush( Qt::NoBrush );
    QRect pageRect;
    for ( int page = 0; page < doc->getPages(); page++ )
    {
        // using paperHeight() leads to rounding problems ( one pixel between two pages, belonging to none of them )
        int pagetop = doc->pageTop( page );
        int pagewidth = doc->paperWidth();
        int pageheight = doc->pageTop( page+1 ) - pagetop;
        pageRect = QRect( 0, pagetop, pagewidth, pageheight );

        drawOnePageBorder( painter, crect, pageRect, emptySpaceRegion );

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
                int topOffset = ( page==0 ) ? s_shadowOffset : 0; // leave a few pixels on top, only for first page
                drawRightShadow( painter, crect, pageRect, topOffset );
            }
        }
    }
    // Take care of the area at the bottom of the last page
    int lastBottom = doc->pageTop( doc->getPages() );
    if ( crect.bottom() > lastBottom )
    {
        QRect bottomArea( 0, lastBottom, crect.width(), crect.bottom() - lastBottom + 1 );
        QRect repaintRect = bottomArea.intersect( crect );
        if ( !repaintRect.isEmpty() )
        {
            painter->fillRect( repaintRect,
                               QApplication::palette().active().brush( QColorGroup::Mid ) );
            // Draw a shadow
            drawBottomShadow( painter, crect, pageRect, s_shadowOffset );
        }
    }
    painter->restore();
}

//////////////////////// Preview mode ////////////////////////////////


QSize KWViewModePreview::contentsSize()
{
    KWDocument * doc = m_canvas->kWordDocument();
    int pages = doc->getPages();
    int rows = (pages-1) / m_pagesPerRow + 1;
    int hPages = rows > 1 ? m_pagesPerRow : pages;
    return QSize( m_spacing + hPages * ( doc->paperWidth() + m_spacing ),
                  m_spacing + rows * ( doc->paperHeight() + m_spacing ) /* bottom of last row */ );
}

QPoint KWViewModePreview::normalToView( const QPoint & nPoint )
{
    KWDocument * doc = m_canvas->kWordDocument();
    // Can't use nPoint.y() / doc->paperHeight() since this would be a rounding problem
    double unzoomedY = doc->unzoomItY( nPoint.y() );
    int page = static_cast<int>( unzoomedY / doc->ptPaperHeight() ); // quotient
    double yInPagePt = unzoomedY - page * doc->ptPaperHeight();      // and rest
    int row = page / m_pagesPerRow;
    int col = page % m_pagesPerRow;
    /*kdDebug() << "KWViewModePreview::normalToView nPoint=" << nPoint.x() << "," << nPoint.y()
                << " unzoomedY=" << unzoomedY
                << " ptPaperHeight=" << doc->ptPaperHeight()
                << " page=" << page << " row=" << row << " col=" << col
                << " yInPagePt=" << yInPagePt << endl;*/
    return QPoint( m_spacing + col * ( doc->paperWidth() + m_spacing ) + nPoint.x(),
                   m_spacing + row * ( doc->paperHeight() + m_spacing ) + doc->zoomItY( yInPagePt ) );
}

QPoint KWViewModePreview::viewToNormal( const QPoint & vPoint )
{
    // Well, just the opposite of the above.... hmm.... headache....
    KWDocument * doc = m_canvas->kWordDocument();
    int paperWidth = doc->paperWidth();
    int paperHeight = doc->paperHeight();
    QPoint p( vPoint.x() - m_spacing, vPoint.y() - m_spacing );
    int col = static_cast<int>( p.x() / ( paperWidth + m_spacing ) );
    int xInPage = p.x() - col * ( paperWidth + m_spacing );
    int row = static_cast<int>( p.y() / ( paperHeight + m_spacing ) );
    int yInPage = p.y() - row * ( paperHeight + m_spacing );
    int page = row * m_pagesPerRow + col;
    if ( page > doc->getPages() - 1 ) // [this happens when moving frames around and going out of the pages]
        return QPoint( paperWidth, doc->pageTop( doc->getPages() ) );
    else // normal case
        return QPoint( xInPage, yInPage + doc->pageTop( page ) );
}

void KWViewModePreview::drawPageBorders( QPainter * painter, const QRect & crect, const QRegion & emptySpaceRegion )
{
    KWDocument * doc = m_canvas->kWordDocument();
    painter->save();
    painter->setPen( QApplication::palette().active().color( QColorGroup::Dark ) );
    painter->setBrush( Qt::NoBrush );
    int paperWidth = doc->paperWidth();
    int paperHeight = doc->paperHeight();
    //kdDebug() << "KWViewModePreview::drawPageBorders crect=" << DEBUGRECT( crect ) << endl;
    QRegion grayRegion( crect );
    for ( int page = 0; page < doc->getPages(); page++ )
    {
        int row = page / m_pagesPerRow;
        int col = page % m_pagesPerRow;
        QRect pageRect( m_spacing + col * ( paperWidth + m_spacing ),
                        m_spacing + row * ( paperHeight + m_spacing ),
                        paperWidth, paperHeight );
        drawOnePageBorder( painter, crect, pageRect, emptySpaceRegion );
        if ( pageRect.intersects( crect ) )
            grayRegion -= pageRect;
        QRect rightShadow = drawRightShadow( painter, crect, pageRect, s_shadowOffset );
        if ( !rightShadow.isEmpty() )
            grayRegion -= rightShadow;
        QRect bottomShadow = drawBottomShadow( painter, crect, pageRect, s_shadowOffset );
        if ( !bottomShadow.isEmpty() )
            grayRegion -= bottomShadow;

        //kdDebug() << "KWViewModePreview::drawPageBorders grayRegion is now : " << endl;
        //DEBUGREGION( grayRegion );
    }
    if ( !grayRegion.isEmpty() )
    {
        //kdDebug() << "KWViewModePreview::drawPageBorders grayRegion's bounding Rect = " << DEBUGRECT( grayRegion.boundingRect() ) << endl;
        m_canvas->kWordDocument()->eraseEmptySpace( painter, grayRegion, QApplication::palette().active().brush( QColorGroup::Mid ) );
    }
    painter->restore();
}

//////////////////

KWTextFrameSet * KWViewModeText::textFrameSet() const
{
    KWDocument * doc = m_canvas->kWordDocument();
    KWFrameSet * fs = doc->getFrameSet( 0 );
    if ( !fs || fs->type() != FT_TEXT )
        return 0L;
    return static_cast<KWTextFrameSet *>(fs);
}

QPoint KWViewModeText::normalToView( const QPoint & nPoint )
{
    KWTextFrameSet * textfs = textFrameSet();
    if (!textfs || !QRect(0,0,textfs->textDocument()->width(), textfs->availableHeight()).contains(nPoint))
        return nPoint;
    else
    {
        QPoint iPoint;
        if ( textfs->normalToInternal( nPoint, iPoint, true ) )
            return iPoint;
        else
        {
            kdWarning() << "KWViewModeText: normalToInternal returned 0L for"
                        << nPoint.x() << "," << nPoint.y() << endl;
            return nPoint;
        }
    }
}

QPoint KWViewModeText::viewToNormal( const QPoint & vPoint )
{
    KWTextFrameSet * textfs = textFrameSet();
    if (!textfs || !QRect(0,0,textfs->textDocument()->width(), textfs->availableHeight()).contains(vPoint))
        return vPoint;
    else
    {
        QPoint nPoint;
        if ( textfs->internalToNormal( vPoint, nPoint ) )
            return nPoint;
        else
        {
            kdWarning() << "KWViewModeText: internalToNormal returned 0L for "
                        << vPoint.x() << "," << vPoint.y() << endl;
            return vPoint;
        }
    }
}

QSize KWViewModeText::contentsSize()
{
    KWTextFrameSet * textfs = textFrameSet();
    if (!textfs)
        return QSize();
    return QSize( textfs->textDocument()->width(), textfs->availableHeight() + 1 /*bottom line*/ );
}

/// ### TODO for this view mode: replace all frameset->isVisible with
// viewMode->isFramesetVisible( KWFrameSet * )
// (except for a few in kwdoc.cc)

void KWViewModeText::drawPageBorders( QPainter * painter, const QRect & crect,
                                      const QRegion & /*emptySpaceRegion*/ )
{
    KWTextFrameSet * textfs = textFrameSet();
    if (!textfs)
        return;
    painter->save();
    KWDocument * doc = m_canvas->kWordDocument();
    QRegion grayRegion( crect );
    QListIterator<KWFrame> it( textfs->frameIterator() );
    painter->setPen( QApplication::palette().active().color( QColorGroup::Dark ) );
    // Draw a line on the right of every frame
    for ( ; it.current() ; ++it )
    {
        QRect frameRect( doc->zoomRect( *it.current() ) );
        painter->drawLine( frameRect.topRight(), frameRect.bottomRight() );
        if ( frameRect.intersects( crect ) )
            grayRegion -= frameRect;
    }
    if ( crect.bottom() >= textfs->availableHeight() )
    {
        // And draw a line at the bottom.
        painter->drawLine( 0, textfs->availableHeight(),
                           textfs->textDocument()->width(), textfs->availableHeight() );
        grayRegion -= QRect( 0, textfs->availableHeight(),
                           textfs->textDocument()->width(), textfs->availableHeight() );
    }
    if ( !grayRegion.isEmpty() )
        doc->eraseEmptySpace( painter, grayRegion, QApplication::palette().active().brush( QColorGroup::Mid ) );
    painter->restore();
}
