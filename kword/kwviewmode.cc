/* This file is part of the KDE project
   Copyright (C) 2001 David Faure <faure@kde.org>
   Copyright (C) 2005 Thomas Zander <zander@kde.org>

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

#include "kwviewmode.h"
#include "kwcanvas.h"
#include "kwview.h"
#include "kwdoc.h"
#include "kwtextframeset.h"
#include "kwtableframeset.h"
#include "KWPageManager.h"
#include "KWPage.h"
#include <kdebug.h>

const unsigned short KWViewMode::s_shadowOffset = 3;

QSize KWViewModeNormal::contentsSize()
{
    return QSize( m_doc->paperWidth(1), m_doc->pageTop( m_doc->pageCount() ) /*i.e. bottom of last one*/ );
}

QSize KWViewMode::availableSizeForText( KWTextFrameSet* textfs )
{
    KWFrame* frame = textfs->frameIterator().getLast();
    return m_doc->zoomSize( KoSize( frame->innerWidth(), frame->internalY() + frame->innerHeight() ) );

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

// static
KWViewMode * KWViewMode::create( const QString & viewModeType, KWDocument *doc )
{
    Q_ASSERT(doc);
    if(viewModeType=="ModeNormal")
    {
        return new KWViewModeNormal( doc, doc->viewFrameBorders() );
    }
    else if(viewModeType=="ModeEmbedded")
    {
        return new KWViewModeEmbedded( doc );
    }
    else if(viewModeType=="ModePreview")
    {
        return new KWViewModePreview( doc, doc->viewFrameBorders(), doc->nbPagePerRow() );
    }
    else if(viewModeType=="ModeText")
    {
        KWTextFrameSet* fs = KWViewModeText::determineTextFrameSet( doc );
        if ( fs )
            return new KWViewModeText( doc, fs );
        return new KWViewModeNormal( doc, doc->viewFrameBorders() );
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
    for ( int page = 0; page < m_doc->numPages(); page++ )
    {
        // using paperHeight() leads to rounding problems ( one pixel between two pages, belonging to none of them )
        int pagetop = m_doc->pageTop( page );
        int pagewidth = m_doc->paperWidth(1);
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
    int lastBottom = m_doc->pageTop( m_doc->numPages() );
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

int KWViewModePreview::leftSpacing()
{
    if ( canvas() )
    {
        int pagesPerRow;
        if ( m_doc->numPages() < m_pagesPerRow )
            pagesPerRow = m_doc->numPages();
        else
            pagesPerRow = m_pagesPerRow;

        int pagesWidth = ( m_spacing + ( m_doc->paperWidth(1) + m_spacing ) * pagesPerRow );
        if ( pagesWidth < canvas()->visibleWidth() )
            return ( m_spacing + ( canvas()->visibleWidth() / 2 ) - ( pagesWidth / 2 ) );
    }
    return m_spacing;
}

int KWViewModePreview::topSpacing()
{
    if ( canvas() )
    {
        int pagesHeight = ( m_spacing + ( m_doc->paperHeight(1) + m_spacing ) * numRows() );
        if ( pagesHeight < canvas()->visibleHeight() )
            return ( m_spacing + ( canvas()->visibleHeight() / 2 ) - ( pagesHeight / 2 ) );
    }
    return m_spacing;
}

int KWViewModePreview::numRows() const
{
    return ( m_doc->numPages() - 1 ) / m_pagesPerRow + 1;
}

QSize KWViewModePreview::contentsSize()
{
    int pages = m_doc->numPages();
    int rows = (pages-1) / m_pagesPerRow + 1;
    int hPages = rows > 1 ? m_pagesPerRow : pages;
    return QSize( m_spacing + hPages * ( m_doc->paperWidth(1) + m_spacing ),
                  m_spacing + rows * ( m_doc->paperHeight(1) + m_spacing ) /* bottom of last row */ );
}

QPoint KWViewModePreview::normalToView( const QPoint & nPoint )
{
    // Can't use nPoint.y() / m_doc->paperHeight() since this would be a rounding problem
    double unzoomedY = m_doc->unzoomItY( nPoint.y() );
    KWPage *page = m_doc->pageManager()->page(unzoomedY);   // quotient
    double yInPagePt = unzoomedY - page->offsetInDocument();// and rest
    int row = page->pageNumber() / m_pagesPerRow;
    int col = page->pageNumber() % m_pagesPerRow;
    /*kdDebug() << "KWViewModePreview::normalToView nPoint=" << nPoint.x() << "," << nPoint.y()
                << " unzoomedY=" << unzoomedY
                << " ptPaperHeight=" << m_doc->ptPaperHeight()
                << " page=" << page->pageNumber() << " row=" << row << " col=" << col
                << " yInPagePt=" << yInPagePt << endl;*/
    return QPoint( leftSpacing() + col * ( m_doc->paperWidth(1) + m_spacing ) + nPoint.x(),
                   topSpacing() + row * ( m_doc->paperHeight(1) + m_spacing ) + m_doc->zoomItY( yInPagePt ) );
}

QPoint KWViewModePreview::viewToNormal( const QPoint & vPoint )
{
    // Well, just the opposite of the above.... hmm.... headache....
    int paperWidth = m_doc->paperWidth(1);
    int paperHeight = m_doc->paperHeight(1);
    QPoint p( vPoint.x() - leftSpacing(), vPoint.y() - topSpacing() );
    int col = static_cast<int>( p.x() / ( paperWidth + m_spacing ) );
    int xInPage = p.x() - col * ( paperWidth + m_spacing );
    int row = static_cast<int>( p.y() / ( paperHeight + m_spacing ) );
    int yInPage = p.y() - row * ( paperHeight + m_spacing );
    int page = row * m_pagesPerRow + col;
    if ( page > m_doc->numPages() - 1 ) // [this happens when moving frames around and going out of the pages]
        return QPoint( paperWidth, m_doc->pageTop( m_doc->numPages() ) );
    else // normal case
        return QPoint( xInPage, yInPage + m_doc->pageTop( page ) );
}

void KWViewModePreview::drawPageBorders( QPainter * painter, const QRect & crect, const QRegion & emptySpaceRegion )
{
    painter->save();
    painter->setPen( QApplication::palette().active().color( QColorGroup::Dark ) );
    painter->setBrush( Qt::NoBrush );
    int paperWidth = m_doc->paperWidth(1);
    int paperHeight = m_doc->paperHeight(1);
    //kdDebug() << "KWViewModePreview::drawPageBorders crect=" << DEBUGRECT( crect ) << endl;
    QRegion grayRegion( crect );
    for ( int page = 0; page < m_doc->numPages(); page++ )
    {
        int row = page / m_pagesPerRow;
        int col = page % m_pagesPerRow;
        QRect pageRect( leftSpacing() + col * ( paperWidth + m_spacing ),
                        topSpacing() + row * ( paperHeight + m_spacing ),
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
KWViewModeText::KWViewModeText( KWDocument * doc, KWTextFrameSet* fs ) : KWViewMode( doc, false )
{
    Q_ASSERT( fs );
    m_textFrameSet = fs;
}

KWTextFrameSet * KWViewModeText::textFrameSet() const
{
    return m_textFrameSet;
}

KWTextFrameSet * KWViewModeText::determineTextFrameSet( KWDocument* doc ) // static
{
    KWFrameSet * fs = 0L;

    KWFrame *f = doc->getFirstSelectedFrame();
    if(f) fs=f->frameSet();

    if (!fs || fs->type() != FT_TEXT) { // no suitable frameset found
        if(doc->getAllViews().count() > 0) {
            KWView *view = doc->getAllViews().first(); // try the one I am editing..
            KWFrameSetEdit *fse = view->getGUI()->canvasWidget()->currentFrameSetEdit();
            if(fse) fs = fse->frameSet(); // selected frame
        }
    }

    if (!fs || fs->type() != FT_TEXT || fs->isHeaderOrFooter() || fs->isFootEndNote())
        if ( doc->numFrameSets() > 0 && doc->frameSet( 0 )->isVisible() )
            fs = doc->frameSet( 0 );  // if not a textFS, or header/footer/footnote: fallback to fs 0

    return dynamic_cast<KWTextFrameSet *>(fs);
}

QPoint KWViewModeText::normalToView( const QPoint & nPoint )
{
    return nPoint;
}

QPoint KWViewModeText::viewToNormal( const QPoint & vPoint )
{
    return vPoint;
}

QSize KWViewModeText::contentsSize()
{
    textFrameSet(); // init.

    if (!m_textFrameSet)
        return QSize();

    // The actual contents only depend on the amount of text.
    // The width is the one from the text, so that the placement of tabs makes a bit of sense, etc.
    // The minimum height is the one of a normal page though.

    int width = /*m_doc->paperWidth()*/
        m_doc->layoutUnitToPixelX( m_textFrameSet->textDocument()->width() );

    int height = QMAX((int)m_doc->paperHeight(1),
                      m_doc->layoutUnitToPixelY( m_textFrameSet->textDocument()->height() ) );
    //kdDebug() << "KWViewModeText::contentsSize " << width << "x" << height << endl;
    return QSize( width, height );
}

QSize KWViewModeText::availableSizeForText( KWTextFrameSet* /*textfs*/ )
{
    return contentsSize();
}

bool KWViewModeText::isFrameSetVisible( const KWFrameSet *fs )
{
    if(fs==NULL) return false; // assertion
    if(fs==textFrameSet()) return true;

    const KWFrameSet* parentFrameset = fs->getGroupManager() ? fs->getGroupManager() : fs;
    while ( parentFrameset->isFloating() ) {
        parentFrameset = parentFrameset->anchorFrameset();
        if ( parentFrameset == m_textFrameSet )
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
    //kdDebug() << "KWViewModeText::drawPageBorders crect=" << grayRegion << endl;
    QPtrListIterator<KWFrame> it( textfs->frameIterator() );
    painter->setPen( QApplication::palette().active().color( QColorGroup::Dark ) );
    QSize cSize = contentsSize();
    // Draw a line on the right -- ## or a shadow?
    // +1 to be out of the contents, and +1 for QRect
    QRect frameRect( 0, 0, cSize.width() + 2, cSize.height() );
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
    //kdDebug() << "KWViewModeText::setPageLayout layout size " << layout.ptWidth << "x" << layout.ptHeight << endl;
    layout.ptLeft = 0;
    layout.ptRight = 0;
    layout.ptTop = 0;
    layout.ptBottom = 0;
    hRuler->setPageLayout( layout );
    vRuler->setPageLayout( layout );
}

bool KWViewModeText::isTextModeFrameset(KWFrameSet *fs) const {
    return fs==m_textFrameSet;
}
