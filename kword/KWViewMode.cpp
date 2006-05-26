/* This file is part of the KDE project
   Copyright (C) 2001-2005 David Faure <faure@kde.org>
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

#include "KWViewMode.h"
#include "KWCanvas.h"
#include "KWView.h"
#include "KWDocument.h"
#include "KWTextFrameSet.h"
#include "KWFrameSet.h"
#include "KWTableFrameSet.h"
#include "KWPageManager.h"
#include "KWPage.h"
#include "KWFrameViewManager.h"
#include "KWFrameView.h"
#include <QApplication>
#include <kdebug.h>

const unsigned short KWViewMode::s_shadowOffset = 3;

QSize KWViewMode::availableSizeForText( KWTextFrameSet* textfs )
{
    KWFrame* frame = textfs->frameIterator().getLast();
    return m_doc->zoomSizeOld( KoSize( frame->innerWidth(), frame->internalY() + frame->innerHeight() ) );

}

void KWViewMode::drawOnePageBorder( QPainter * painter, const QRect & crect, const QRect & _pageRect,
                                    const QRegion & emptySpaceRegion )
{
    if ( !crect.intersects( _pageRect ) )
        return;

    QRect pageRect( _pageRect );
    //kDebug() << "KWViewMode::drawOnePageBorder drawing page rect " << pageRect << endl;
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
        //kDebug() << "KWViewMode::drawOnePageBorder : pagecrect=" << pagecrect << " emptySpaceRegion: " << emptySpaceRegion << endl;
        QRegion pageEmptyRegion = emptySpaceRegion.intersect( pagecrect );
        //kDebug() << "RESULT: pageEmptyRegion: " << pageEmptyRegion << endl;
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

QPoint KWViewMode::pageCorner()
{
    // Same code as KWView::slotUpdateRuler
    KWFrame * frame = 0L;
    // Use the currently edited (fallback: the first selected) frame
    if( m_canvas->currentFrameSetEdit() && m_canvas->currentFrameSetEdit()->currentFrame() )
        frame = m_canvas->currentFrameSetEdit()->currentFrame();
    else {
        KWFrameView *view = m_canvas->frameViewManager()->selectedFrame();
        frame = view == 0 ? 0 : view->frame();
    }

    int pageNum = 0;
    if ( frame )
        pageNum = frame->pageNumber();
    QPoint nPoint( 0, m_doc->pageTop(pageNum) + 1 );
    QPoint cPoint( normalToView( nPoint ) );
    /*kDebug() << "KWViewMode::pageCorner frame=" << frame << " pagenum=" << pageNum
              << " nPoint=" << nPoint.x() << "," << nPoint.y()
              << " cPoint=" << cPoint.x() << "," << cPoint.y() << endl;*/
    return cPoint;
}

QRect KWViewMode::rulerFrameRect()
{
    // Set the "frame start" in the ruler (tabs are relative to that position)
    KWFrameSetEdit * edit = m_canvas->currentFrameSetEdit();
    KWFrame * frame = 0L;
    // Use the currently edited (fallback: the first selected) frame
    if ( edit && edit->currentFrame() )
        frame = edit->currentFrame();
    else {
        KWFrameView *view = m_canvas->frameViewManager()->selectedFrame();
        frame = view == 0 ? 0 : view->frame();
    }
    if( !frame) {
        KWFrameSet *fs= m_doc->frameSet(0);
        if(fs) frame=fs->frame(0);
    }
    if ( frame )
    {
        QRect r = normalToView( m_doc->zoomRectOld( frame->innerRect() ) );

        // Calculate page corner (see pageCorner above)
        int pageNum = frame->pageNumber();
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
KWViewMode * KWViewMode::create( const QString & viewModeType, KWDocument *doc, KWCanvas* canvas )
{
    Q_ASSERT(doc);
    if(viewModeType=="ModeNormal")
    {
        return new KWViewModeNormal( doc, canvas, doc->viewFrameBorders() );
    }
    else if(viewModeType=="ModeEmbedded")
    {
        return new KWViewModeEmbedded( doc, canvas );
    }
    else if(viewModeType=="ModePreview")
    {
        return new KWViewModePreview( doc, canvas, doc->viewFrameBorders(), doc->nbPagePerRow() );
    }
    else if(viewModeType=="ModeText")
    {
        KWTextFrameSet* fs = KWViewModeText::determineTextFrameSet( doc );
        if ( fs )
            return new KWViewModeText( doc, canvas, fs );
        return new KWViewModeNormal( doc, canvas, doc->viewFrameBorders() );
    }
    else
    {
        kDebug() << viewModeType << " mode type is unknown\n";
        return 0;
    }
}

////

QSize KWViewModeNormal::contentsSize()
{
    return QSize( m_doc->paperWidth(m_doc->startPage()),
                  m_doc->zoomItYOld( m_doc->pageManager()->bottomOfPage(m_doc->lastPage()) ) );
}

QRect KWViewModeNormal::viewPageRect( int pgNum )
{
    KWPage* page = m_doc->pageManager()->page( pgNum );
    QRect r = page->zoomedRect( m_doc );
    r.moveBy( xOffset( page ), 0 );
    return r;
}

QPoint KWViewModeNormal::normalToView( const QPoint & nPoint )
{
    double unzoomedY = m_doc->unzoomItYOld( nPoint.y() );
    KWPage *page = m_doc->pageManager()->page(unzoomedY);   // quotient
    if( !page) {
        kWarning(31001) << "KWViewModeNormal::normalToView request for conversion out of the document! Check your input data.. ("<< nPoint << ")" << endl;
        return QPoint(0,0);
    }
    Q_ASSERT(canvas());
    return QPoint( xOffset(page) + nPoint.x(), nPoint.y() );
}

QPoint KWViewModeNormal::viewToNormal( const QPoint & vPoint )
{
    // Opposite of the above
    // The Y is unchanged by the centering so we can use it to get the page.
    double unzoomedY = m_doc->unzoomItYOld( vPoint.y() );
    KWPage *page = m_doc->pageManager()->page(unzoomedY);   // quotient
    if( !page) {
        kWarning(31001) << "KWViewModeNormal::normalToView request for conversion out of the document! Check your input data.. ("<< vPoint << ")" << endl;
        return QPoint(-1,-1); // yes this is an ugly way to mark this as an excetional state...
    }
    Q_ASSERT(canvas());
    return QPoint( vPoint.x() - xOffset(page), vPoint.y() );
}

int KWViewModeNormal::xOffset(KWPage *page, int canvasWidth /* = -1 */) {
    // Center horizontally
    if(canvasWidth < 0)
        canvasWidth = canvas()->visibleWidth();
    return qMax( 0, ( canvasWidth - m_doc->zoomItXOld( page->width() ) ) / 2 );
}

void KWViewModeNormal::drawPageBorders( QPainter * painter, const QRect & crect, const QRegion & emptySpaceRegion )
{
    painter->save();
    painter->setPen( QApplication::palette().active().color( QColorGroup::Dark ) );
    painter->setBrush( Qt::NoBrush );
    QRect pageRect;

    int lastPage = m_doc->lastPage();
    Q_ASSERT(canvas());
    const int canvasWidth = canvas()->visibleWidth();
    double pagePosPt = 0;
    int topOfPage = 0; // in pixels
    for ( int pageNr = m_doc->startPage(); pageNr <= lastPage; pageNr++ )
    {
        KWPage *page = m_doc->pageManager()->page(pageNr);

        int pageWidth = m_doc->zoomItXOld( page->width() );
        int pageHeight = m_doc->zoomItYOld( pagePosPt + page->height() ) - topOfPage;
        if ( crect.bottom() < topOfPage )
            break;
        // Center horizontally
        int x = xOffset(page, canvasWidth);
        // Draw page border (and erase empty area inside page)
        pageRect = QRect( x, topOfPage, pageWidth, pageHeight );
        drawOnePageBorder( painter, crect, pageRect, emptySpaceRegion );

        // The area on the left of the page
        QRect leftArea( 0, topOfPage, x, pageHeight );
        leftArea &= crect;
        if ( !leftArea.isEmpty() ) {
            painter->fillRect( leftArea,
                               QApplication::palette().active().brush( QColorGroup::Mid ) );
        }

        // The area on the right of the page
        QRect rightArea( x + pageWidth, topOfPage, crect.right() - pageWidth + 1, pageHeight );
        rightArea &= crect;
        if ( !rightArea.isEmpty() )
        {
            painter->fillRect( rightArea,
                               QApplication::palette().active().brush( QColorGroup::Mid ) );

            // Draw a shadow
            int topOffset = ( page==0 ) ? s_shadowOffset : 0; // leave a few pixels on top, only for first page
            drawRightShadow( painter, crect, pageRect, topOffset );
        }
        pagePosPt += page->height(); // for next page already..
        topOfPage += pageHeight;
    }
    // Take care of the area at the bottom of the last page
    if ( crect.bottom() > topOfPage )
    {
        QRect bottomArea( 0, topOfPage, crect.right() + 1, crect.bottom() - topOfPage + 1 );
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

////

QRect KWViewModeEmbedded::viewPageRect( int pgNum )
{
    // Only one page makes sense in embedded mode though
    return m_doc->pageManager()->page( pgNum )->zoomedRect( m_doc );
}

//////////////////////// Preview mode ////////////////////////////////

KWViewModePreview::KWViewModePreview( KWDocument * doc, KWCanvas* canvas,
                                      bool drawFrameBorders, int _nbPagePerRow )
    : KWViewMode( doc, canvas, drawFrameBorders ),
      m_pagesPerRow(_nbPagePerRow),
      m_spacing(10)
{}

int KWViewModePreview::leftSpacing()
{
    if ( canvas() )
    {
        int pagesPerRow;
        if ( m_doc->pageCount() < m_pagesPerRow )
            pagesPerRow = m_doc->pageCount();
        else
            pagesPerRow = m_pagesPerRow;

        int pagesWidth = ( m_spacing + ( m_doc->paperWidth(m_doc->startPage()) + m_spacing ) * pagesPerRow );
        if ( pagesWidth < canvas()->visibleWidth() )
            return ( m_spacing + ( canvas()->visibleWidth() / 2 ) - ( pagesWidth / 2 ) );
    }
    return m_spacing;
}

int KWViewModePreview::topSpacing()
{
    if ( canvas() )
    {
        int pagesHeight = ( m_spacing + ( m_doc->paperHeight(m_doc->startPage()) + m_spacing ) * numRows() );
        if ( pagesHeight < canvas()->visibleHeight() )
            return ( m_spacing + ( canvas()->visibleHeight() / 2 ) - ( pagesHeight / 2 ) );
    }
    return m_spacing;
}

int KWViewModePreview::numRows() const
{
    return ( m_doc->pageCount() ) / m_pagesPerRow + 1;
}

QSize KWViewModePreview::contentsSize()
{
    int pages = m_doc->pageCount();
    int rows = (pages-1) / m_pagesPerRow + 1;
    int hPages = rows > 1 ? m_pagesPerRow : pages;
    return QSize( m_spacing + hPages * ( m_doc->paperWidth(m_doc->startPage()) + m_spacing ),
                  m_spacing + rows * ( m_doc->paperHeight(m_doc->startPage()) + m_spacing ) /* bottom of last row */ );
}

QPoint KWViewModePreview::normalToView( const QPoint & nPoint )
{
    // Can't use nPoint.y() / m_doc->paperHeight() since this would be a rounding problem
    double unzoomedY = m_doc->unzoomItYOld( nPoint.y() );
    KWPage *page = m_doc->pageManager()->page(unzoomedY);   // quotient
    if( !page) {
        kWarning(31001) << "KWViewModePreview::normalToView request for conversion out of the document! Check your input data.. ("<< nPoint << ")" << endl;
        return QPoint(0,0);
    }

    double yInPagePt = unzoomedY - page->offsetInDocument();// and rest
    int row = (page->pageNumber() - m_doc->startPage()) / m_pagesPerRow;
    int col = (page->pageNumber() - m_doc->startPage()) % m_pagesPerRow;
    /*kDebug() << "KWViewModePreview::normalToView nPoint=" << nPoint
                << " unzoomedY=" << unzoomedY
                << " page=" << page->pageNumber() << " row=" << row << " col=" << col
                << " yInPagePt=" << yInPagePt << endl;*/
    return QPoint( leftSpacing() + col * ( m_doc->paperWidth(page->pageNumber()) +
                                           m_spacing ) + nPoint.x(),
                   topSpacing() + row * ( m_doc->paperHeight(page->pageNumber()) +
                                          m_spacing ) + m_doc->zoomItYOld( yInPagePt ) );
}

QRect KWViewModePreview::viewPageRect( int pgNum )
{
    int row = (pgNum - m_doc->startPage()) / m_pagesPerRow;
    int col = (pgNum - m_doc->startPage()) % m_pagesPerRow;
    const int paperWidth = m_doc->paperWidth( pgNum );
    const int paperHeight = m_doc->paperHeight( pgNum );
    return QRect( leftSpacing() + col * ( paperWidth + m_spacing ),
                  topSpacing() + row * ( paperHeight + m_spacing ),
                  paperWidth,
                  paperHeight );
}

QPoint KWViewModePreview::viewToNormal( const QPoint & vPoint )
{
    // Well, just the opposite of the above.... hmm.... headache....
    int paperWidth = m_doc->paperWidth(m_doc->startPage());
    int paperHeight = m_doc->paperHeight(m_doc->startPage());
    QPoint p( vPoint.x() - leftSpacing(), vPoint.y() - topSpacing() );
    int col = static_cast<int>( p.x() / ( paperWidth + m_spacing ) );
    int xInPage = p.x() - col * ( paperWidth + m_spacing );
    int row = static_cast<int>( p.y() / ( paperHeight + m_spacing ) );
    int yInPage = p.y() - row * ( paperHeight + m_spacing );
    int page = row * m_pagesPerRow + col + m_doc->startPage();
    if ( page > m_doc->lastPage() ) // [this happens when moving frames around and going out of the pages]
        return QPoint( paperWidth, m_doc->pageTop( m_doc->lastPage() ) );
    else // normal case
        return QPoint( xInPage, yInPage + m_doc->pageTop( page ) );
}

void KWViewModePreview::drawPageBorders( QPainter * painter, const QRect & crect, const QRegion & emptySpaceRegion )
{
    painter->save();
    painter->setPen( QApplication::palette().active().color( QColorGroup::Dark ) );
    painter->setBrush( Qt::NoBrush );
    //kDebug() << "KWViewModePreview::drawPageBorders crect=" << DEBUGRECT( crect ) << endl;
    QRegion grayRegion( crect );
    int pageCount = m_doc->pageCount();
    for ( int counter = 0; counter < pageCount; counter++ )
    {
        int row = counter / m_pagesPerRow;
        int col = counter % m_pagesPerRow;
        int page = m_doc->startPage() + counter;
        int paperWidth = m_doc->paperWidth(page);
        int paperHeight = m_doc->paperHeight(page);
        QRect pageRect( leftSpacing() + col * ( paperWidth + m_spacing ),
                        topSpacing() + row * ( paperHeight + m_spacing ),
                        paperWidth, paperHeight );
        drawOnePageBorder( painter, crect, pageRect, emptySpaceRegion );
        if( pageRect.top() > crect.bottom())
            break;
        if ( pageRect.intersects( crect ) )
            grayRegion -= pageRect;
        QRect rightShadow = drawRightShadow( painter, crect, pageRect, s_shadowOffset );
        if ( !rightShadow.isEmpty() )
            grayRegion -= rightShadow;
        QRect bottomShadow = drawBottomShadow( painter, crect, pageRect, s_shadowOffset );
        if ( !bottomShadow.isEmpty() )
            grayRegion -= bottomShadow;

        //kDebug() << "KWViewModePreview::drawPageBorders grayRegion is now : " << endl;
        //DEBUGREGION( grayRegion );
    }
    if ( !grayRegion.isEmpty() )
    {
        //kDebug() << "KWViewModePreview::drawPageBorders grayRegion's bounding Rect = " << DEBUGRECT( grayRegion.boundingRect() ) << endl;
        m_doc->eraseEmptySpace( painter, grayRegion, QApplication::palette().active().brush( QColorGroup::Mid ) );
    }
    painter->restore();
}

//////////////////

KWViewModeText::KWViewModeText( KWDocument * doc, KWCanvas* canvas, KWTextFrameSet* fs )
    : KWViewMode( doc, canvas, false )
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
    KWTextFrameSet* fs = 0L;

    if(!doc->getAllViews().empty()) { // try the one that is selected
        KWView *view = doc->getAllViews()[0];
        KWFrameView *fv = view->getGUI()->canvasWidget()->frameViewManager()->selectedFrame();
        KWFrame *f = fv == 0 ? 0 : fv->frame();
        if(f)
            fs = dynamic_cast<KWTextFrameSet *>( f->frameSet() );

        if (!fs) { // try the one I am editing
            KWFrameSetEdit *fse = view->getGUI()->canvasWidget()->currentFrameSetEdit();
            if(fse)
                fs = dynamic_cast<KWTextFrameSet *>( fse->frameSet() );
        }
    }

    if (!fs || fs->isHeaderOrFooter() || fs->isFootEndNote())
        // if not a textFS, or header/footer/footnote: fallback to fs 0
        if ( doc->frameSetCount() > 0 && doc->frameSet( 0 )->isVisible() )
            fs = dynamic_cast<KWTextFrameSet *>( doc->frameSet( 0 ) );

    return fs;
}

QPoint KWViewModeText::normalToView( const QPoint & nPoint )
{
    QPoint r (nPoint);
    r.setX(r.x() + OFFSET);
    return r;
}

QPoint KWViewModeText::viewToNormal( const QPoint & vPoint )
{
    QPoint r (vPoint);
    r.setX(r.x() - OFFSET);
    return r;
}

QSize KWViewModeText::contentsSize()
{
    textFrameSet(); // init.

    if (!m_textFrameSet)
        return QSize();

    // The actual contents only depend on the amount of text.
    // The width is the one from the text, so that the placement of tabs makes a bit of sense, etc.
    // The minimum height is the one of a normal page though.

    int width = m_doc->layoutUnitToPixelX( m_textFrameSet->textDocument()->width() );

    int height = qMax((int)m_doc->paperHeight(m_doc->startPage()),
                      m_doc->layoutUnitToPixelY( m_textFrameSet->textDocument()->height() ) );
    //kDebug() << "KWViewModeText::contentsSize " << width << "x" << height << endl;
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

    const KWFrameSet* parentFrameset = fs->groupmanager() ? fs->groupmanager() : fs;
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
    painter->save();
    QRegion grayRegion( crect );
    //kDebug() << "KWViewModeText::drawPageBorders crect=" << grayRegion << endl;
    painter->setPen( QApplication::palette().active().color( QColorGroup::Dark ) );
    QSize cSize = contentsSize();
    // Draw a line on the right -- ## or a shadow?
    // +1 to be out of the contents, and +1 for QRect
    QRect frameRect( OFFSET, 0, cSize.width() + 2, cSize.height() );
    //kDebug() << "KWViewModeText::drawPageBorders right line: "  << frameRect.topRight() << "   " << frameRect.bottomRight()<< endl;
    painter->drawLine( frameRect.topRight(), frameRect.bottomRight() );
    if ( frameRect.intersects( crect ) )
        grayRegion -= frameRect;

    //kDebug() << "KWViewModeText::drawPageBorders grayRegion is now " << grayRegion << endl;
    if ( crect.bottom() >= cSize.height() )
    {
        // And draw a line at the bottom -- ## or a shadow?
        painter->drawLine( 0, cSize.height(),
                           cSize.width(), cSize.height() );
        grayRegion -= QRect( 0, cSize.height(),
                             cSize.width(), cSize.height() );
    }
    //kDebug() << "KWViewModeText::drawPageBorders erasing grayRegion " << grayRegion << endl;
    if ( !grayRegion.isEmpty() )
        m_doc->eraseEmptySpace( painter, grayRegion, QApplication::palette().active().brush( QColorGroup::Mid ) );
    painter->restore();
}

QRect KWViewModeText::rulerFrameRect()
{
    return QRect( QPoint(OFFSET, 0), contentsSize() );
}

void KWViewModeText::setPageLayout( KoRuler* hRuler, KoRuler* vRuler, const KoPageLayout& /*layout*/ )
{
    // Create a dummy page-layout, as if we had a single page englobing the whole text.
    KoPageLayout layout;
    layout.format = PG_CUSTOM;
    layout.orientation = PG_PORTRAIT;
    QSize cSize = contentsSize();
    layout.ptWidth = m_doc->unzoomItXOld( cSize.width() );
    layout.ptHeight = m_doc->unzoomItYOld( cSize.height() );
    //kDebug() << "KWViewModeText::setPageLayout layout size " << layout.ptWidth << "x" << layout.ptHeight << endl;
    layout.ptLeft = OFFSET;
    layout.ptRight = 0;
    layout.ptTop = 0;
    layout.ptBottom = 0;
    layout.ptBindingSide = 0;
    layout.ptPageEdge = 0;
    hRuler->setPageLayout( layout );
    vRuler->setPageLayout( layout );
}

bool KWViewModeText::isTextModeFrameset(KWFrameSet *fs) const {
    return fs==m_textFrameSet;
}


int KWViewModePrint::xOffset(KWPage *page, int canvasWidth) {
    Q_UNUSED(page);
    Q_UNUSED(canvasWidth);
    return 0;
}
