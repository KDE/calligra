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
#include "kwtextframeset.h"

#include <kdebug.h>
#include <kdebugclasses.h>

const unsigned short KWViewMode::s_shadowOffset = 3;

QSize KWViewModeNormal::contentsSize()
{
    return QSize( m_doc->paperWidth(), m_doc->pageTop( m_doc->getPages() ) /*i.e. bottom of last one*/ );
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
            m_doc->eraseEmptySpace( painter, pageEmptyRegion, QApplication::palette().active().brush( QColorGroup::Base ) );
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

QPoint KWViewMode::pageCorner( KWCanvas* canvas )
{
    // Same code as KWView::slotUpdateRuler
    KWFrame * frame = 0L;
    // Use the currently edited (fallback: the first selected) frame
    if( canvas->currentFrameSetEdit() && canvas->currentFrameSetEdit()->currentFrame() )
        frame = canvas->currentFrameSetEdit()->currentFrame();
    else
        frame = m_doc->getFirstSelectedFrame();

    int pageNum = 0;
    if ( frame )
        pageNum = frame->pageNum();
    QPoint nPoint( 0, m_doc->pageTop(pageNum) + 1 );
    QPoint cPoint( normalToView( nPoint ) );
    /*kdDebug() << "KWViewMode::pageCorner frame=" << frame << " pagenum=" << pageNum
              << " nPoint=" << nPoint.x() << "," << nPoint.y()
              << " cPoint=" << cPoint.x() << "," << cPoint.y() << endl;*/
    return cPoint;
}

QRect KWViewMode::rulerFrameRect( KWCanvas* canvas )
{
    // Set the "frame start" in the ruler (tabs are relative to that position)
    KWFrameSetEdit * edit = canvas->currentFrameSetEdit();
    KWFrame * frame = 0L;
    // Use the currently edited (fallback: the first selected) frame
    if ( edit && edit->currentFrame() )
        frame = edit->currentFrame();
    else
        frame = m_doc->getFirstSelectedFrame();
    if( !frame) {
        KWFrameSet *fs= m_doc->frameSet(0);
        if(fs) frame=fs->frame(0);
    }
    if ( frame )
    {
        QRect r = m_doc->zoomRect( frame->innerRect() );
        r = canvas->viewMode()->normalToView( r );

        // Calculate page corner (see pageCorner above)
        int pageNum = frame->pageNum();
        QPoint nPoint( 0, m_doc->pageTop(pageNum) + 1 );
        QPoint cPoint( normalToView( nPoint ) );

        // Frame start/end is relative to page corner.
        r.moveBy( -cPoint.x(), -cPoint.y() );
        return r;
    }
    return QRect();
}

void KWViewMode::setPageLayout( KoRuler* hRuler, KoRuler* vRuler, const KoPageLayout& layout )
{
    hRuler->setPageLayout( layout );
    vRuler->setPageLayout( layout );
}

KWViewMode * KWViewMode::create( const QString & viewModeType, KWDocument *doc )
{
    Q_ASSERT(doc);
    if(viewModeType=="ModeNormal")
    {
        return new KWViewModeNormal( doc );
    }
    else if(viewModeType=="ModeEmbedded")
    {
        return new KWViewModeEmbedded ( doc );
    }
    else if(viewModeType=="ModePreview")
    {
        return new KWViewModePreview( doc, doc->nbPagePerRow() );
    }
    else if(viewModeType=="ModeText")
    {
        return new KWViewModeText( doc );
    }
    else
    {
        kdDebug()<<viewModeType <<" mode type is unknown\n";
        return 0L;
    }
}


void KWViewModeNormal::drawPageBorders( QPainter * painter, const QRect & crect, const QRegion & emptySpaceRegion )
{
    painter->save();
    painter->setPen( QApplication::palette().active().color( QColorGroup::Dark ) );
    painter->setBrush( Qt::NoBrush );
    QRect pageRect;
    for ( int page = 0; page < m_doc->getPages(); page++ )
    {
        // using paperHeight() leads to rounding problems ( one pixel between two pages, belonging to none of them )
        int pagetop = m_doc->pageTop( page );
        int pagewidth = m_doc->paperWidth();
        int pageheight = m_doc->pageTop( page+1 ) - pagetop;
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
    int lastBottom = m_doc->pageTop( m_doc->getPages() );
    if ( crect.bottom() > lastBottom )
    {
        QRect bottomArea( 0, lastBottom, crect.right() + 1, crect.bottom() - lastBottom + 1 );
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
    int pages = m_doc->getPages();
    int rows = (pages-1) / m_pagesPerRow + 1;
    int hPages = rows > 1 ? m_pagesPerRow : pages;
    return QSize( m_spacing + hPages * ( m_doc->paperWidth() + m_spacing ),
                  m_spacing + rows * ( m_doc->paperHeight() + m_spacing ) /* bottom of last row */ );
}

QPoint KWViewModePreview::normalToView( const QPoint & nPoint )
{
    // Can't use nPoint.y() / m_doc->paperHeight() since this would be a rounding problem
    double unzoomedY = m_doc->unzoomItY( nPoint.y() );
    int page = static_cast<int>( unzoomedY / m_doc->ptPaperHeight() ); // quotient
    double yInPagePt = unzoomedY - page * m_doc->ptPaperHeight();      // and rest
    int row = page / m_pagesPerRow;
    int col = page % m_pagesPerRow;
    /*kdDebug() << "KWViewModePreview::normalToView nPoint=" << nPoint.x() << "," << nPoint.y()
                << " unzoomedY=" << unzoomedY
                << " ptPaperHeight=" << m_doc->ptPaperHeight()
                << " page=" << page << " row=" << row << " col=" << col
                << " yInPagePt=" << yInPagePt << endl;*/
    return QPoint( m_spacing + col * ( m_doc->paperWidth() + m_spacing ) + nPoint.x(),
                   m_spacing + row * ( m_doc->paperHeight() + m_spacing ) + m_doc->zoomItY( yInPagePt ) );
}

QPoint KWViewModePreview::viewToNormal( const QPoint & vPoint )
{
    // Well, just the opposite of the above.... hmm.... headache....
    int paperWidth = m_doc->paperWidth();
    int paperHeight = m_doc->paperHeight();
    QPoint p( vPoint.x() - m_spacing, vPoint.y() - m_spacing );
    int col = static_cast<int>( p.x() / ( paperWidth + m_spacing ) );
    int xInPage = p.x() - col * ( paperWidth + m_spacing );
    int row = static_cast<int>( p.y() / ( paperHeight + m_spacing ) );
    int yInPage = p.y() - row * ( paperHeight + m_spacing );
    int page = row * m_pagesPerRow + col;
    if ( page > m_doc->getPages() - 1 ) // [this happens when moving frames around and going out of the pages]
        return QPoint( paperWidth, m_doc->pageTop( m_doc->getPages() ) );
    else // normal case
        return QPoint( xInPage, yInPage + m_doc->pageTop( page ) );
}

void KWViewModePreview::drawPageBorders( QPainter * painter, const QRect & crect, const QRegion & emptySpaceRegion )
{
    painter->save();
    painter->setPen( QApplication::palette().active().color( QColorGroup::Dark ) );
    painter->setBrush( Qt::NoBrush );
    int paperWidth = m_doc->paperWidth();
    int paperHeight = m_doc->paperHeight();
    //kdDebug() << "KWViewModePreview::drawPageBorders crect=" << DEBUGRECT( crect ) << endl;
    QRegion grayRegion( crect );
    for ( int page = 0; page < m_doc->getPages(); page++ )
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
        m_doc->eraseEmptySpace( painter, grayRegion, QApplication::palette().active().brush( QColorGroup::Mid ) );
    }
    painter->restore();
}

//////////////////

KWTextFrameSet * KWViewModeText::textFrameSet() const
{
    KWFrameSet * fs = m_doc->frameSet( 0 );
    Q_ASSERT( fs );
    Q_ASSERT( fs->type() == FT_TEXT );
    if ( !fs || fs->type() != FT_TEXT )
        return 0L;
    return static_cast<KWTextFrameSet *>(fs);
}

QPoint KWViewModeText::normalToView( const QPoint & nPoint )
{
    KWTextFrameSet * textfs = textFrameSet();
    if ( !textfs )
        return nPoint;
    else
    {
        QPoint iPoint;
        // We use documentToInternalMouseSelection to benefit from a bit of tolerance.
        // This is helpful against rounding errors (e.g. x in pt = 28.0, zoomed = 29, unzooming = 27.8)
        KWTextFrameSet::RelativePosition relPos;
        if ( textfs->documentToInternalMouseSelection( m_doc->unzoomPoint( nPoint ), iPoint, relPos ) )
            return m_doc->layoutUnitToPixel( iPoint );
        else
        {
            kdWarning() << "KWViewModeText: documentToInternal returned 0L for "
                        << nPoint.x() << "," << nPoint.y() << endl;
            return nPoint;
        }
    }
}

QPoint KWViewModeText::viewToNormal( const QPoint & vPoint )
{
    KWTextFrameSet * textfs = textFrameSet();
    QRect contentsRect( QPoint(0,0), contentsSize() );
    if ( !textfs || !contentsRect.contains(vPoint) )
        return vPoint;
    else
    {
        QPoint iPoint = m_doc->pixelToLayoutUnit( vPoint );
        KoPoint dPoint;
        if ( textfs->internalToDocument( iPoint, dPoint ) )
            return textfs->kWordDocument()->zoomPoint( dPoint );
        else
        {
            kdWarning() << "KWViewModeText: internalToDocument returned 0L for iPoint "
                        << iPoint.x() << "," << iPoint.y()
                        << " (vPoint: " << vPoint << ")" << endl;
            return vPoint;
        }
    }
}

QSize KWViewModeText::contentsSize()
{
    KWTextFrameSet * textfs = textFrameSet();
    if (!textfs)
        return QSize();
    // Hmm, availableHeight relies on the formatting being done - problem?
    //QSize luSize( textfs->textDocument()->width(), textfs->availableHeight() + 1 /*bottom line*/ );

    // The actual contents only depend on the amount of text.
    QSize luSize( textfs->textDocument()->width(), textfs->textDocument()->height() /* + 1 bottom line*/ );

    // But we want to show at least a page if the doc is empty, IMHO.
    int pageHeight = m_doc->zoomItY( textfs->frame(0)->height() );

    QSize cSize( m_doc->layoutUnitToPixelX( luSize.width() ),
                 QMAX( m_doc->layoutUnitToPixelY( luSize.height() ), pageHeight ) );
    //kdDebug() << "KWViewModeText::contentsSize " << cSize << endl;
    return cSize;
}

bool KWViewModeText::isFrameSetVisible( const KWFrameSet *fs )
{
    KWTextFrameSet * textfs = textFrameSet();
    if ( !textfs || !fs )
        return false;
    if ( fs == textfs )
        return true;
    const KWFrameSet* parentFrameset = fs;
    while ( parentFrameset->isFloating() ) {
        parentFrameset = parentFrameset->anchorFrameset();
        if ( parentFrameset == textfs )
            return true;
    }
    return false;
}

void KWViewModeText::drawPageBorders( QPainter * painter, const QRect & crect,
                                      const QRegion & /*emptySpaceRegion*/ )
{
    KWTextFrameSet * textfs = textFrameSet();
    if (!textfs)
        return;
    painter->save();
    QRegion grayRegion( crect );
    //kdDebug() << "\nKWViewModeText::drawPageBorders crect=" << grayRegion << endl;
    QPtrListIterator<KWFrame> it( textfs->frameIterator() );
    painter->setPen( QApplication::palette().active().color( QColorGroup::Dark ) );
    QSize cSize = contentsSize();
    // Draw a line on the right -- ## or a shadow?
    QRect frameRect( 0, 0, cSize.width() + 1, cSize.height() );
    //kdDebug() << "KWViewModeText::drawPageBorders right line: "  << frameRect.topRight() << "   " << frameRect.bottomRight()<< endl;
    painter->drawLine( frameRect.topRight(), frameRect.bottomRight() );
    if ( frameRect.intersects( crect ) )
        grayRegion -= frameRect;

    //kdDebug() << "KWViewModeText::drawPageBorders grayRegion is now " << grayRegion << endl;
    if ( crect.bottom() >= cSize.height() )
    {
        // And draw a line at the bottom -- ## or a shadow?
        painter->drawLine( 0, cSize.height(),
                           cSize.width(), cSize.height() );
        grayRegion -= QRect( 0, cSize.height(),
                             cSize.width(), cSize.height() );
    }
    //kdDebug() << "KWViewModeText::drawPageBorders erasing grayRegion " << grayRegion << endl;
    if ( !grayRegion.isEmpty() )
        m_doc->eraseEmptySpace( painter, grayRegion, QApplication::palette().active().brush( QColorGroup::Mid ) );
    painter->restore();
}

QRect KWViewModeText::rulerFrameRect( KWCanvas* )
{
    return QRect( QPoint(0,0), contentsSize() );
}

void KWViewModeText::setPageLayout( KoRuler* hRuler, KoRuler* vRuler, const KoPageLayout& /*layout*/ )
{
    // Create a dummy page-layout, as if we had a single page englobing the whole text.
    KoPageLayout layout;
    layout.format = PG_CUSTOM;
    layout.orientation = PG_PORTRAIT;
    QSize cSize = contentsSize();
    layout.ptWidth = m_doc->unzoomItX( cSize.width() );
    layout.ptHeight = m_doc->unzoomItY( cSize.height() );
    kdDebug() << "KWViewModeText::setPageLayout layout size " << layout.ptWidth << "x" << layout.ptHeight << endl;
    layout.ptLeft = 0;
    layout.ptRight = 0;
    layout.ptTop = 0;
    layout.ptBottom = 0;
    hRuler->setPageLayout( layout );
    vRuler->setPageLayout( layout );
}
