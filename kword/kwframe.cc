/* This file is part of the KDE project
   Copyright (C) 1998, 1999, 2000 Reginald Stadlbauer <reggie@kde.org>

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

#include "kwdoc.h"
#include "kwview.h"
#include "kwcanvas.h"
#include "kwframe.h"
#include "defs.h"
#include "kwutils.h"
#include "kwtextframeset.h"
#include "kwanchor.h"
#include "resizehandles.h"

#include <kformulacontainer.h>
#include <kformuladocument.h>
#include <kformulaview.h>

#include <kdebug.h>

/******************************************************************/
/* Class: KWFrame                                                 */
/******************************************************************/

/*================================================================*/
KWFrame::KWFrame(KWFrameSet *fs, double left, double top, double width, double height, RunAround _ra, double _gap )
    : KoRect( left, top, width, height ),
      // Initialize member vars here. This ensures they are all initialized, since it's
      // easier to compare this list with the member vars list (compiler ensures order).
      sheetSide( AnySide ),
      runAround( _ra ),
      frameBehaviour( AutoCreateNewFrame ),
      newFrameBehaviour( Reconnect ),
      runAroundGap( _gap ),
      selected( false ),
      mostRight( false ),
      m_pageNum( 0 ),
      brd_left( Qt::white, Border::SOLID, 0 ),
      brd_right( Qt::white, Border::SOLID, 0 ),
      brd_top( Qt::white, Border::SOLID, 0 ),
      brd_bottom( Qt::white, Border::SOLID, 0 ),
      backgroundColor( Qt::white ),
      bleft( 0 ),
      bright( 0 ),
      btop( 0 ),
      bbottom( 0 ),
      frameSet( fs ),
      m_anchor( 0 )
{
    //kdDebug() << "KWFrame::KWFrame " << this << " left=" << left << " top=" << top << endl;
    m_pageNum = fs ? fs->kWordDocument()->getPageOfRect( *this ) : 0;
    handles.setAutoDelete(true);
    //intersections.setAutoDelete( true );
    //emptyRegionDirty = TRUE;
}

/*================================================================*/
KWFrame::~KWFrame()
{
    //kdDebug() << "KWFrame::~KWFrame " << this << endl;
    if (selected)
        removeResizeHandles();
}

#if 0
/*================================================================*/
void KWFrame::addIntersect( QRect &_r )
{
    emptyRegionDirty = TRUE;

    intersections.append( new QRect( _r.x(), _r.y(), _r.width(), _r.height() ) );
}

/*================================================================*/
int KWFrame::getLeftIndent( int y, int h )
{
    if ( runAround == RA_NO || intersections.isEmpty() )
        return 0;

    if ( emptyRegionDirty )
        getEmptyRegion();

    int left = 0;
    QRect line( x(), y, width(), h );
    QRegion reg = emptyRegion.intersect( line );
    left = reg.boundingRect().left() - x();
    for ( unsigned int i = 0; i < reg.rects().size(); ++i )
        left = QMAX( left, reg.rects()[ i ].left() - x() );
    if ( left > 0 )
        left += static_cast<int>(runAroundGap);
    if ( left > 0 && runAround == RA_SKIP )
        left = width();

    return QMAX( 0, left );
}

/*================================================================*/
int KWFrame::getRightIndent( int _y, int _h )
{
    if ( runAround == RA_NO || intersections.isEmpty() )
        return 0;

    if ( emptyRegionDirty )
        getEmptyRegion();

    int _right = 0;
    QRegion line( x(), _y, width(), _h );
    QRegion reg = line.subtract( emptyRegion );
    _right = 0;
    for ( unsigned int i = 0; i < reg.rects().size(); ++i ) {
        if ( reg.rects()[ i ].right() == right() ) {
            if ( reg.rects()[ i ].width() == width() && reg.rects()[ i ].x() == x() &&
                 (int)(y() + height() + getBTop().pt() + getBBottom().pt() ) == reg.rects()[ i ].y() )
                continue;
            _right = QMAX( _right, reg.rects()[ i ].width() );
        }
    }
    if ( _right > 0 )
        _right += static_cast<int>(runAroundGap);
    if ( _right > 0 && runAround == RA_SKIP )
        _right = width();

    return QMIN( width(), _right );
}

/*================================================================*/
unsigned int KWFrame::getNextFreeYPos( unsigned int _y, unsigned int _h )
{
    QRect rect;
    unsigned int __y = _y;

    for ( unsigned int i = 0; i < intersections.count(); i++ ) {
        rect = *intersections.at( i );

        if ( rect.intersects( QRect( 0, _y, INT_MAX, _h ) ) )
            __y = __y == _y ? rect.bottom() : QMIN( static_cast<int>( __y ), rect.bottom() );
    }

    return __y;
}

/*================================================================*/
QRegion KWFrame::getEmptyRegion( bool useCached )
{
    if ( !emptyRegionDirty && useCached )
        return emptyRegion;

    emptyRegion = QRegion( x(), y(), width(), height() );
    QRect rect;

    for ( unsigned int i = 0; i < intersections.count(); i++ ) {
        rect = *intersections.at( i );
        emptyRegion = emptyRegion.subtract( QRect( rect.x() - 1, rect.y() - 1,
                                                   rect.width() + 2, rect.height() + 2 ) );
    }

    emptyRegionDirty = FALSE;

    return emptyRegion;
}
#endif

/*================================================================*/
QCursor KWFrame::getMouseCursor( double mx, double my, bool table )
{
    if ( !table ) {
        if ( mx >= x() && my >= y() && mx <= x() + 6 && my <= y() + 6 )
            return Qt::sizeFDiagCursor;
        if ( mx >= x() && my >= y() + height() / 2 - 3 && mx <= x() + 6 && my <= y() + height() / 2 + 3 )
            return Qt::sizeHorCursor;
        if ( mx >= x() && my >= y() + height() - 6 && mx <= x() + 6 && my <= y() + height() )
            return Qt::sizeBDiagCursor;
        if ( mx >= x() + width() / 2 - 3 && my >= y() && mx <= x() + width() / 2 + 3 && my <= y() + 6 )
            return Qt::sizeVerCursor;
        if ( mx >= x() + width() / 2 - 3 && my >= y() + height() - 6 && mx <= x() + width() / 2 + 3 &&
             my <= y() + height() )
            return Qt::sizeVerCursor;
        if ( mx >= x() + width() - 6 && my >= y() && mx <= x() + width() && my <= y() + 6 )
            return Qt::sizeBDiagCursor;
        if ( mx >= x() + width() - 6 && my >= y() + height() / 2 - 3 && mx <= x() + width() &&
             my <= y() + height() / 2 + 3 )
            return Qt::sizeHorCursor;
        if ( mx >= x() + width() - 6 && my >= y() + height() - 6 && mx <= x() + width() && my <= y() + height() )
            return Qt::sizeFDiagCursor;

        if ( selected )
            return Qt::sizeAllCursor;
    } else { // Tables
        if ( mx >= x() + width() - 6 && my >= y() && mx <= x() + width() && my <= y() + height() )
            return Qt::sizeHorCursor;
        if ( mx >= x() && my >= y() + height() - 6 && mx <= x() + width() && my <= y() + height() )
            return Qt::sizeVerCursor;
        return Qt::sizeAllCursor;
    }

    return Qt::arrowCursor;
}

KWFrame *KWFrame::getCopy() {
    /* returns a deep copy of self */
    KWFrame *frm = new KWFrame(getFrameSet(), x(), y(), width(), height(), getRunAround(), getRunAroundGap() );
    frm->setBackgroundColor( QBrush( getBackgroundColor() ) );
    frm->setFrameBehaviour(getFrameBehaviour());
    frm->setNewFrameBehaviour(getNewFrameBehaviour());
    frm->setSheetSide(getSheetSide());
    frm->setPageNum(pageNum());
    return frm;
}

void KWFrame::deleteAnchor()
{
    delete m_anchor;
    m_anchor = 0L;
}

/*================================================================*/
/* Insert all resize handles                                      */
/*================================================================*/
void KWFrame::createResizeHandles() {
    removeResizeHandles();
    QList <KWView> pages = getFrameSet()->kWordDocument()->getAllViews();
    for (int i=pages.count() -1; i >= 0; i--)
        createResizeHandlesForPage(pages.at(i)->getGUI()->canvasWidget());
}

/*================================================================*/
/* Insert 8 resize handles which will be drawn in param canvas    */
/*================================================================*/
void KWFrame::createResizeHandlesForPage(KWCanvas *canvas) {
    removeResizeHandlesForPage(canvas);

    for (unsigned int i=0; i < 8; i++) {
        KWResizeHandle * h = new KWResizeHandle( canvas, (KWResizeHandle::Direction)i, this );
        handles.append( h );
    }
}

/*===================================================================*/
/* remove all the resize handles which will be drawn in param canvas */
/*===================================================================*/
void KWFrame::removeResizeHandlesForPage(KWCanvas *canvas) {
    for( unsigned int i=0; i < handles.count(); i++) {
        if(handles.at ( i )->getCanvas() == canvas) {
            handles.remove(i--);
        }
    }
}

/*================================================================*/
/* remove all resizeHandles                                       */
/*================================================================*/
void KWFrame::removeResizeHandles() {
    handles.clear();
}

/*================================================================*/
/* move the resizehandles to current location of frame            */
/*================================================================*/
void KWFrame::updateResizeHandles() {
    for (unsigned int i=0; i< handles.count(); i++) {
        handles.at(i)->updateGeometry();
    }
}

void KWFrame::setSelected( bool _selected )
{
    bool s = selected;
    selected = _selected;
    if ( selected )
        createResizeHandles();
    else if ( s )
        removeResizeHandles();
}

QRect KWFrame::outerRect() const
{
    KWDocument *doc = getFrameSet()->kWordDocument();
    QRect outerRect( doc->zoomRect( *this ) );
    outerRect.rLeft() -= (int)QMAX( 1, doc->zoomItX( getLeftBorder().ptWidth ) + 0.5 );
    outerRect.rTop() -= (int)QMAX( 1, doc->zoomItY( getTopBorder().ptWidth ) + 0.5 );
    outerRect.rRight() += (int)QMAX( 1, doc->zoomItX( getRightBorder().ptWidth ) + 0.5 );
    outerRect.rBottom() += (int)QMAX( 1, doc->zoomItY( getBottomBorder().ptWidth ) + 0.5 );
    return outerRect;
}

/******************************************************************/
/* Class: KWFrameSet                                              */
/******************************************************************/

KWFrameSet::KWFrameSet( KWDocument *doc )
    : frames(), removeableHeader( false ), visible( true )
{
    m_doc = doc;
    // Send our "repaintChanged" signals to the document.
    connect( this, SIGNAL( repaintChanged( KWFrameSet * ) ),
             doc, SLOT( slotRepaintChanged( KWFrameSet * ) ) );
    frames.setAutoDelete( true );
    frameInfo = FI_BODY;
    current = 0;
    grpMgr = 0L;
    m_anchorTextFs = 0L;
    m_anchorParag = 0L;
    m_anchorIndex = 0;
}

KWFrameSet::~KWFrameSet()
{
    if ( isFloating() )
        deleteAnchors();
}

void KWFrameSet::addFrame( KWFrame *_frame, bool recalc )
{
    if ( frames.findRef( _frame ) != -1 )
        return;

    frames.append( _frame );
    _frame->setFrameSet(this);
    if(recalc)
        updateFrames();
    if ( isFloating() )
    {
        findFirstAnchor();
        updateAnchors();
    }
}

void KWFrameSet::delFrame( unsigned int _num )
{
    if ( isFloating() )
        deleteAnchors();
    KWFrame *frm = frames.at( _num );
    delFrame(frm,true);
    if ( isFloating() )
        updateAnchors();
}

/*================================================================*/
void KWFrameSet::delFrame( KWFrame *frm, bool remove )
{
    int _num = frames.findRef( frm );
    if ( _num == -1 )
        return;

    if ( isFloating() )
        deleteAnchors();

    frm->setFrameSet(0L);
    if ( !remove )
    //if ( !del || !remove )
        frames.take( _num );
    else
        frames.remove( _num );

    updateFrames();

    if ( isFloating() )
        updateAnchors();
}

/*================================================================*/
void KWFrameSet::drawBorders( QPainter *painter, const QRect &crect, QRegion &region )
{
    painter->save();

    QListIterator<KWFrame> frameIt = frameIterator();
    for ( ; frameIt.current(); ++frameIt )
    {
        KWFrame *frame = frameIt.current();
        QRect frameRect( m_doc->zoomRect( *frame ) );
        QRect outerRect( frame->outerRect() );
        //kdDebug(32002) << "KWFrameSet::drawBorders frameRect: " << DEBUGRECT( frameRect ) << endl;
        //kdDebug(32002) << "KWFrameSet::drawBorders outerRect: " << DEBUGRECT( outerRect ) << endl;

        if ( !crect.intersects( outerRect ) )
        {
            //kdDebug() << "KWFrameSet::drawBorders no intersection with " << DEBUGRECT(crect) << endl;
            continue;
        }

        region = region.subtract( outerRect );

        // Set the background color from the main frameset (why?)
        if ( isAHeader() || isAFooter() )
            frame = getFrame( 0 );
        painter->setBrush( frame->getBackgroundColor() );

        // Draw default borders using view settings except when printing, or disabled.
        QPen viewSetting( lightGray );
        if ( ( painter->device()->devType() == QInternal::Printer ) ||
            !m_doc->getViewFrameBorders() )
        {
            viewSetting.setColor( frame->getBackgroundColor().color() );
        }

        // Draw borders either as the user defined them, or using the view settings.
        // Borders should be drawn _outside_ of the frame area
        // otherwise the frames will erase the border when painting themselves.
        QPen pen;
        double width;
        int w;

        // Right
        width = frame->getRightBorder().ptWidth;
        if ( width > 0 )
        {
            w = QMAX( 1, (int)(m_doc->zoomItX( width ) + 0.5) );
            pen = Border::borderPen( frame->getRightBorder(), w );
        }
        else
        {
            w = 1;
            pen = viewSetting;
        }
        painter->setPen( pen );
        w = QMAX( w / 2, 1 );
        painter->drawLine( frameRect.right() + w, frameRect.y(),
                           frameRect.right() + w, frameRect.bottom() );

        // Bottom
        width = frame->getBottomBorder().ptWidth;
        if ( width > 0 )
        {
            w = QMAX( 1, (int)(m_doc->zoomItY( width ) + 0.5) );
            pen = Border::borderPen( frame->getBottomBorder(), w );
        }
        else
        {
            w = 1;
            pen = viewSetting;
        }
        painter->setPen( pen );
        w = QMAX( w / 2, 1 );
        painter->drawLine( frameRect.x(),     frameRect.bottom() + w,
                           frameRect.right(), frameRect.bottom() + w );

        // Left
        width = frame->getLeftBorder().ptWidth;
        if ( width > 0 )
        {
            w = QMAX( 1, (int)(m_doc->zoomItX( width ) + 0.5) );
            pen = Border::borderPen( frame->getLeftBorder(), w );
        }
        else
        {
            w = 1;
            pen = viewSetting;
        }
        painter->setPen( pen );
        w = QMAX( w / 2, 1 );
        painter->drawLine( frameRect.x() - w, frameRect.y(),
                        frameRect.x() - w, frameRect.bottom() );

        // Top
        width = frame->getTopBorder().ptWidth;
        if ( width > 0 )
        {
            w = QMAX( 1, (int)(m_doc->zoomItY( width ) + 0.5) );
            pen = Border::borderPen( frame->getTopBorder(), w );
        }
        else
        {
            w = 1;
            pen = viewSetting;
        }
        painter->setPen( pen );
        w = QMAX( w / 2, 1 );
        painter->drawLine( frameRect.x(),     frameRect.y() - w,
                        frameRect.right(), frameRect.y() - w );
    }
    painter->restore();
}

void KWFrameSet::setAnchored( KWTextFrameSet* textfs, KWTextParag* parag, int index )
{
    if ( isFloating() )
        deleteAnchors();
    ASSERT( parag );
    m_anchorTextFs = textfs;
    m_anchorParag = parag;
    m_anchorIndex = index;
    updateAnchors();
}

void KWFrameSet::setFixed()
{
    if ( isFloating() )
        deleteAnchors();
    m_anchorTextFs = 0L;
    m_anchorParag = 0L;
    m_anchorIndex = 0;
}

void KWFrameSet::updateAnchors()
{
    kdDebug() << "KWFrameSet::updateAnchors" << endl;
    int index = m_anchorIndex;
    QListIterator<KWFrame> frameIt = frameIterator();
    for ( ; frameIt.current(); ++frameIt, ++index )
    {
        if ( ! frameIt.current()->anchor() )
        {
            // Anchor this frame, after the previous one
            KWAnchor * anchor = new KWAnchor( m_anchorTextFs->textDocument(), frameIt.current() );
            m_anchorParag->insert( index, QChar('@') /*whatever*/ );
            m_anchorParag->setCustomItem( index, anchor, 0 );
            frameIt.current()->setAnchor( anchor );
        }
    }
    m_anchorParag->setChanged( true );
    //kdDebug() << "KWFrameSet::updateAnchors emit repaintChanged" << endl;
    emit repaintChanged( m_anchorTextFs );
}

void KWFrameSet::deleteAnchors()
{
    kdDebug() << "KWFrameSet::deleteAnchors" << endl;
    findFirstAnchor();
    int index = m_anchorIndex;
    QListIterator<KWFrame> frameIt = frameIterator();
    for ( ; frameIt.current(); ++frameIt )
        if ( frameIt.current()->anchor() )
        {
            // Delete anchor (after removing anchor char)
            m_anchorParag->at( index )->loseCustomItem();
            m_anchorParag->remove( index, 1 );
            frameIt.current()->deleteAnchor();
        }
}

void KWFrameSet::findFirstAnchor()
{
    ASSERT( frames.count() );
    if ( frames.count() > 0 && frames.first()->anchor() )
    {
        KWAnchor * anchor = frames.first()->anchor();
        m_anchorParag = static_cast<KWTextParag *>( anchor->paragraph() );
        m_anchorIndex = m_anchorParag->findCustomItem( anchor );
    } else {
        kdDebug() << "KWFrameSet::findFirstAnchor no anchor !" << endl;
        m_anchorParag = 0L;
    }
}

KWFrame * KWFrameSet::getFrame( double _x, double _y )
{
    QListIterator<KWFrame> frameIt = frameIterator();
    for ( ; frameIt.current(); ++frameIt )
        if ( frameIt.current()->contains( KoPoint( _x, _y ) ) )
            return frameIt.current();
    return 0L;
}

/*================================================================*/
KWFrame *KWFrameSet::getFrame( unsigned int _num )
{
    return frames.at( _num );
}

/*================================================================*/
int KWFrameSet::getFrameFromPtr( KWFrame *frame )
{
    return frames.findRef( frame );
}

/*================================================================*/
void KWFrameSet::updateFrames()
{
    // Iterate over ALL framesets, to find those which have frames on top of us.
    // We'll use this information in various methods (adjust[LR]Margin, drawContents etc.)
    // So we want it cached.
    m_framesOnTop.clear();
    QListIterator<KWFrameSet> framesetIt( m_doc->framesetsIterator() );
    bool foundThis = false;
    for (; framesetIt.current(); ++framesetIt )
    {
        KWFrameSet *frameSet = framesetIt.current();

        if ( frameSet == this )
        {
            foundThis = true;
            continue;
        }

        if ( !foundThis || !frameSet->isVisible() || frameSet->isFloating() )
            continue;

        //kdDebug() << "KWFrameSet::updateFrames considering frameset " << frameSet << endl;
        QListIterator<KWFrame> frameIt( frameSet->frameIterator() );
        for ( ; frameIt.current(); ++frameIt )
        {
            KWFrame *frame = frameIt.current();
            // Is this frame over any of our frames ?
            QListIterator<KWFrame> fIt( frameIterator() );
            for ( ; fIt.current(); ++fIt )
            {
                if ( frame->intersects( *fIt.current() ) )
                {
                    m_framesOnTop.append( frame );
                    break;
                }
            }
        }
    }
    //kdDebug(32002) << "KWTextFrameSet " << this << " updateFrames() : frame on top:"
    //               << m_framesOnTop.count() << endl;

    if ( isFloating() )
    { // The frame[s] might have been resized -> invalidate the parag to recompute widths & heights
        m_anchorParag->invalidate( 0 );
        QListIterator<KWFrame> frameIt = frameIterator();
        for ( ; frameIt.current(); ++frameIt )
        {
            kdDebug() << "KWFrameSet::updateFrames anchor=" << frameIt.current()->anchor() << endl;
            if ( frameIt.current()->anchor() )
                frameIt.current()->anchor()->resize();
        }
    }
}

/*================================================================*/
bool KWFrameSet::contains( double mx, double my )
{
    for ( unsigned int i = 0; i < frames.count(); i++ ) {
        if ( frames.at( i )->contains( KoPoint( mx, my ) ) )
            return true;
    }

    return false;
}

/*================================================================*/
/* Select the first frame where the x and y coords fall into
   returns 0 if none was selected, return 1 if selected, return 2
   if the frame was allready selected.
*/
int KWFrameSet::selectFrame( double mx, double my, bool simulate )
{
    for ( unsigned int i = 0; i < frames.count(); i++ ) {
        if ( frames.at( i )->contains( KoPoint( mx, my ) ) ) {
            int r = 1;
            if ( frames.at( i )->isSelected() )
                r = 2;
            if ( !simulate )
                frames.at( i )->setSelected( true );
            return r;
        }
    }
    return 0;
}

/*================================================================*/
void KWFrameSet::deSelectFrame( double mx, double my )
{
    QListIterator<KWFrame> frameIt = frameIterator();
    for ( ; frameIt.current(); ++frameIt )
        if ( frameIt.current()->contains( KoPoint( mx, my ) ) )
            frameIt.current()->setSelected( false );
}

/*================================================================*/
QCursor KWFrameSet::getMouseCursor( double mx, double my )
{
    KWFrame * frame = getFrame( mx, my );

    if ( frame == 0L )
        return Qt::arrowCursor;

    if ( !frame->isSelected() && !grpMgr )
        return Qt::arrowCursor;

    return frame->getMouseCursor( mx, my, grpMgr ? true : false );
}

/*================================================================*/
void KWFrameSet::save( QDomElement &parentElem )
{
    if ( frames.isEmpty() ) // Deleted frameset -> don't save
        return;
    QListIterator<KWFrame> frameIt = frameIterator();
    for ( ; frameIt.current(); ++frameIt )
    {
        KWFrame *frame = frameIt.current();
        QDomElement frameElem = parentElem.ownerDocument().createElement( "FRAME" );
        parentElem.appendChild( frameElem );
#if 0
        if(getGroupManager() && getGroupManager()->isAnchored()) {
            // set the frame coordinates to the offset.
            frameElem.setAttribute( "left", frame->left() );
            frameElem.setAttribute( "top", frame->top() - getGroupManager()->getOrigin().y() );
            frameElem.setAttribute( "right", frame->right() );
            frameElem.setAttribute( "bottom", frame->bottom() - getGroupManager()->getOrigin().y() );
        } else
#endif
        {
            frameElem.setAttribute( "left", frame->left() );
            frameElem.setAttribute( "top", frame->top() );
            frameElem.setAttribute( "right", frame->right() );
            frameElem.setAttribute( "bottom", frame->bottom() );
        }

        if(frame->getRunAround()!=RA_NO)
            frameElem.setAttribute( "runaround", static_cast<int>( frame->getRunAround() ) );

        if(frame->getRunAroundGap()!=0)
            frameElem.setAttribute( "runaroundGap", frame->getRunAroundGap() );

        if(frame->getLeftBorder().ptWidth!=0)
            frameElem.setAttribute( "lWidth", frame->getLeftBorder().ptWidth );

        if(frame->getLeftBorder().color != Qt::white)
        {
            frameElem.setAttribute( "lRed", frame->getLeftBorder().color.red() );
            frameElem.setAttribute( "lGreen", frame->getLeftBorder().color.green() );
            frameElem.setAttribute( "lBlue", frame->getLeftBorder().color.blue() );
        }
        if(frame->getLeftBorder().style != Border::SOLID)
            frameElem.setAttribute( "lStyle", static_cast<int>( frame->getLeftBorder().style ) );

        if(frame->getRightBorder().ptWidth!=0)
            frameElem.setAttribute( "rWidth", frame->getRightBorder().ptWidth );

        if(frame->getRightBorder().color != Qt::white)
        {
            frameElem.setAttribute( "rRed", frame->getRightBorder().color.red() );
            frameElem.setAttribute( "rGreen", frame->getRightBorder().color.green() );
            frameElem.setAttribute( "rBlue", frame->getRightBorder().color.blue() );
        }
        if(frame->getRightBorder().style != Border::SOLID)
            frameElem.setAttribute( "rStyle", static_cast<int>( frame->getRightBorder().style ) );

        if(frame->getTopBorder().ptWidth!=0)
            frameElem.setAttribute( "tWidth", frame->getTopBorder().ptWidth );

        if(frame->getTopBorder().color != Qt::white)
        {
            frameElem.setAttribute( "tRed", frame->getTopBorder().color.red() );
            frameElem.setAttribute( "tGreen", frame->getTopBorder().color.green() );
            frameElem.setAttribute( "tBlue", frame->getTopBorder().color.blue() );
        }
        if(frame->getTopBorder().style != Border::SOLID)
            frameElem.setAttribute( "tStyle", static_cast<int>( frame->getTopBorder().style ) );

        if(frame->getBottomBorder().ptWidth!=0) {
            frameElem.setAttribute( "bWidth", frame->getBottomBorder().ptWidth );
        }
        if(frame->getBottomBorder().color != Qt::white) {
            frameElem.setAttribute( "bRed", frame->getBottomBorder().color.red() );
            frameElem.setAttribute( "bGreen", frame->getBottomBorder().color.green() );
            frameElem.setAttribute( "bBlue", frame->getBottomBorder().color.blue() );
        }
        if(frame->getBottomBorder().style != Border::SOLID)
            frameElem.setAttribute( "bStyle", static_cast<int>( frame->getBottomBorder().style ) );

        if(frame->getBackgroundColor().color() != Qt::white)
        {
            frameElem.setAttribute( "bkRed", frame->getBackgroundColor().color().red() );
            frameElem.setAttribute( "bkGreen", frame->getBackgroundColor().color().green() );
            frameElem.setAttribute( "bkBlue", frame->getBackgroundColor().color().blue() );
        }
        if(frame->getBLeft() != 0)
            frameElem.setAttribute( "bleftpt", frame->getBLeft() );

        if(frame->getBRight()!=0)
            frameElem.setAttribute( "brightpt", frame->getBRight() );

        if(frame->getBTop()!=0)
            frameElem.setAttribute( "btoppt", frame->getBTop() );

        if(frame->getBBottom()!=0)
            frameElem.setAttribute( "bbottompt", frame->getBBottom() );

        if(frame->getFrameBehaviour()!=AutoCreateNewFrame)
            frameElem.setAttribute( "autoCreateNewFrame", static_cast<int>( frame->getFrameBehaviour()) );

        if(frame->getNewFrameBehaviour()!=Reconnect)
            frameElem.setAttribute( "newFrameBehaviour", static_cast<int>( frame->getNewFrameBehaviour()) );

        if(frame->getSheetSide()!= AnySide)
            frameElem.setAttribute( "sheetSide", static_cast<int>( frame->getSheetSide()) );

        if(m_doc->processingType() == KWDocument::WP) {
            if(m_doc->getFrameSet(0) == this) break;
            if(getFrameInfo() == FI_FIRST_HEADER ||
               getFrameInfo() == FI_ODD_HEADER ||
               getFrameInfo() == FI_EVEN_HEADER ||
               getFrameInfo() == FI_FIRST_FOOTER ||
               getFrameInfo() == FI_ODD_FOOTER ||
               getFrameInfo() == FI_EVEN_FOOTER ||
               getFrameInfo() == FI_FOOTNOTE) break;
        }
    }
}

/*================================================================*/
//
// This function is intended as a helper for all the derived classes. It reads
// in all the attributes common to all framesets and loads all frames.
//
void KWFrameSet::load( QDomElement &attributes )
{
    // <FRAME>
    QDomElement frameElem = attributes.firstChild().toElement();
    for ( ; !frameElem.isNull() ; frameElem = frameElem.nextSibling().toElement() )
    {
        if ( frameElem.tagName() == "FRAME" )
        {
            SheetSide sheetSide = AnySide;
            Border l, r, t, b;
            l.color = Qt::white;
            l.style = Border::SOLID;
            l.ptWidth = 0;
            r.color = Qt::white;
            r.style = Border::SOLID;
            r.ptWidth = 0;
            t.color = Qt::white;
            t.style = Border::SOLID;
            t.ptWidth = 0;
            b.color = Qt::white;
            b.style = Border::SOLID;
            b.ptWidth = 0;

            KoRect rect;
            rect.setLeft( KWDocument::getAttribute( frameElem, "left", 0.0 ) );
            rect.setTop( KWDocument::getAttribute( frameElem, "top", 0.0 ) );
            rect.setRight( KWDocument::getAttribute( frameElem, "right", 0.0 ) );
            rect.setBottom( KWDocument::getAttribute( frameElem, "bottom", 0.0 ) );
            RunAround runaround = static_cast<RunAround>( KWDocument::getAttribute( frameElem, "runaround", 0 ) );
            double runAroundGap = ( frameElem.hasAttribute( "runaroundGap" ) )
                                  ? frameElem.attribute( "runaroundGap" ).toDouble()
                                  : frameElem.attribute( "runaGapPT" ).toDouble();
            l.ptWidth = KWDocument::getAttribute( frameElem, "lWidth", 0.0 );
            r.ptWidth = KWDocument::getAttribute( frameElem, "rWidth", 0.0 );
            t.ptWidth = KWDocument::getAttribute( frameElem, "tWidth", 0.0 );
            b.ptWidth = KWDocument::getAttribute( frameElem, "bWidth", 0.0 );
            l.color.setRgb(
                KWDocument::getAttribute( frameElem, "lRed", 0xff ),
                KWDocument::getAttribute( frameElem, "lGreen", 0xff ),
                KWDocument::getAttribute( frameElem, "lBlue", 0xff ) );
            r.color.setRgb(
                KWDocument::getAttribute( frameElem, "rRed", 0xff ),
                KWDocument::getAttribute( frameElem, "rGreen", 0xff ),
                KWDocument::getAttribute( frameElem, "rBlue", 0xff ) );
            t.color.setRgb(
                KWDocument::getAttribute( frameElem, "tRed", 0xff ),
                KWDocument::getAttribute( frameElem, "tGreen", 0xff ),
                KWDocument::getAttribute( frameElem, "tBlue", 0xff ) );
            b.color.setRgb(
                KWDocument::getAttribute( frameElem, "bRed", 0xff ),
                KWDocument::getAttribute( frameElem, "bGreen", 0xff ),
                KWDocument::getAttribute( frameElem, "bBlue", 0xff ) );
            l.style = static_cast<Border::BorderStyle>( KWDocument::getAttribute( frameElem, "lStyle", Border::SOLID ) );
            r.style = static_cast<Border::BorderStyle>( KWDocument::getAttribute( frameElem, "rStyle", Border::SOLID ) );
            t.style = static_cast<Border::BorderStyle>( KWDocument::getAttribute( frameElem, "tStyle", Border::SOLID ) );
            b.style = static_cast<Border::BorderStyle>( KWDocument::getAttribute( frameElem, "bStyle", Border::SOLID ) );
            QColor c;
            c.setRgb(
                KWDocument::getAttribute( frameElem, "bkRed", 0xff ),
                KWDocument::getAttribute( frameElem, "bkGreen", 0xff ),
                KWDocument::getAttribute( frameElem, "bkBlue", 0xff ) );

            double lpt = frameElem.attribute( "bleftpt" ).toDouble();
            double rpt = frameElem.attribute( "brightpt" ).toDouble();
            double tpt = frameElem.attribute( "btoppt" ).toDouble();
            double bpt = frameElem.attribute( "bbottompt" ).toDouble();
            FrameBehaviour autoCreateNewValue = static_cast<FrameBehaviour>( KWDocument::getAttribute( frameElem, "autoCreateNewFrame", AutoCreateNewFrame ) );
            NewFrameBehaviour newFrameBehaviour = static_cast<NewFrameBehaviour>( KWDocument::getAttribute( frameElem, "newFrameBehaviour", Reconnect ) );
            sheetSide = static_cast<SheetSide>( KWDocument::getAttribute( frameElem, "sheetSide", AnySide ) );

            KWFrame * frame = new KWFrame(this, rect.x(), rect.y(), rect.width(), rect.height(), runaround, runAroundGap );
//            if(c==l.color && l.ptWidth==1 && l.style==0 )
//                l.ptWidth=0;
            frame->setLeftBorder( l );
//            if(c==r.color  && r.ptWidth==1 && r.style==0)
//                r.ptWidth=0;
            frame->setRightBorder( r );
//            if(c==t.color && t.ptWidth==1 && t.style==0 )
//                t.ptWidth=0;
            frame->setTopBorder( t );
//            if(c==b.color && b.ptWidth==1 && b.style==0 )
//                b.ptWidth=0;
            frame->setBottomBorder( b );
            frame->setBackgroundColor( QBrush( c ) );
            frame->setBLeft( lpt );
            frame->setBRight( rpt );
            frame->setBTop( tpt );
            frame->setBBottom( bpt );
            frame->setFrameBehaviour( autoCreateNewValue );
            frame->setSheetSide( sheetSide );
            frame->setNewFrameBehaviour( newFrameBehaviour);
            addFrame( frame );
            m_doc->progressItemLoaded();
        }
    }
}

/*================================================================*/
bool KWFrameSet::hasSelectedFrame()
{
    for ( unsigned int i = 0; i < frames.count(); i++ ) {
        if ( frames.at( i )->isSelected() )
            return true;
    }

    return false;
}

/*================================================================*/
bool KWFrameSet::isVisible()
{
    return ( visible &&
             !frames.isEmpty() &&
             (!isAHeader() || m_doc->isHeaderVisible()) &&
             (!isAFooter() || m_doc->isFooterVisible()) &&
             !isAWrongHeader( m_doc->getHeaderType() ) &&
             !isAWrongFooter( m_doc->getFooterType() ) );
}

void KWFrameSet::finalize()
{
    //kdDebug() << "KWFrameSet::finalize" << endl;
    updateFrames();
    zoom();
}

// This determines where to clip the painter to draw the contents of a given frame
// It clips to the frame, and clips out any "on top" frame.
QRegion KWFrameSet::frameClipRegion( QPainter * painter, KWFrame *frame, const QRect & crect )
{
    QRect rc = painter->xForm( kWordDocument()->zoomRect( *frame ) );
    rc &= painter->xForm( crect ); // intersect
    //kdDebug() << "KWTextFrameSet::frameClipRegion frame=" << DEBUGRECT(*frame)
    //          << " clip region rect=" << DEBUGRECT(rc)
    //          << " rc.isEmpty()=" << rc.isEmpty() << endl;
    if ( !rc.isEmpty() )
    {
        QRegion reg( rc );
        QListIterator<KWFrame> fIt( m_framesOnTop );
        for ( ; fIt.current() ; ++fIt )
        {
            QRect r = painter->xForm( kWordDocument()->zoomRect( *fIt.current() ) );
            // ### plan for a one-pixel border. Maybe we should use the real border width
            r = QRect( r.x() - 1, r.y() - 1,
                       r.width() + 2, r.height() + 2 );
            //kdDebug() << "frameClipRegion subtract rect "<< DEBUGRECT(r) << endl;
            reg -= r; // subtract
        }
        return reg;
    } else return QRegion();
}

KWFrameSetEdit::KWFrameSetEdit( KWFrameSet * fs, KWCanvas * canvas )
     : m_fs(fs), m_canvas(canvas), m_currentFrame( fs->getFrame(0) )
{
}

/******************************************************************/
/* Class: KWPictureFrameSet                                       */
/******************************************************************/

/*================================================================*/
KWPictureFrameSet::~KWPictureFrameSet() {
}

/*================================================================*/
void KWPictureFrameSet::setFileName( const QString &_filename, const QSize &_imgSize )
{
    KWImageCollection *collection = m_doc->imageCollection();

    m_image = collection->image( _filename );

    m_image = m_image.scale( _imgSize );
}

/*================================================================*/
void KWPictureFrameSet::setSize( QSize _imgSize )
{
    m_image = m_image.scale( _imgSize );
}

/*================================================================*/
void KWPictureFrameSet::save( QDomElement & parentElem )
{
    if ( frames.isEmpty() ) // Deleted frameset -> don't save
        return;
    QDomElement framesetElem = parentElem.ownerDocument().createElement( "FRAMESET" );
    parentElem.appendChild( framesetElem );

    framesetElem.setAttribute( "frameType", static_cast<int>( getFrameType() ) );
    framesetElem.setAttribute( "frameInfo", static_cast<int>( frameInfo ) );

    KWFrameSet::save( framesetElem ); // Save all frames

    QDomElement imageElem = parentElem.ownerDocument().createElement( "IMAGE" );
    framesetElem.appendChild( imageElem );
    QDomElement elem = parentElem.ownerDocument().createElement( "FILENAME" );
    imageElem.appendChild( elem );
    elem.setAttribute( "value", m_image.key() );
}

/*================================================================*/
void KWPictureFrameSet::load( QDomElement &attributes )
{
    KWFrameSet::load( attributes );

    // <IMAGE>
    QDomElement image = attributes.namedItem( "IMAGE" ).toElement();
    if ( !image.isNull() ) {
        // <FILENAME>
        QDomElement filenameElement = image.namedItem( "FILENAME" ).toElement();
        if ( !filenameElement.isNull() )
        {
            QString filename = filenameElement.attribute( "value" );
            m_doc->addImageRequest( filename, this );
        }
        else
        {
            kdError(32001) << "Missing FILENAME tag in IMAGE" << endl;
        }
    } else {
        kdError(32001) << "Missing IMAGE tag in FRAMESET" << endl;
    }
}

void KWPictureFrameSet::drawContents( QPainter *painter, const QRect & crect,
                                      QColorGroup &, bool, bool )
{
    if ( frames.isEmpty() ) // Deleted frameset
        return;
    QRect r = kWordDocument()->zoomRect( *frames.first() );

    if ( r.size() != m_image.image().size() )
        m_image = m_image.scale( r.size() );
    QRegion reg = frameClipRegion( painter, frames.first(), crect );
    if ( !reg.isEmpty() )
    {
        painter->save();
        painter->setClipRegion( reg );
        painter->drawPixmap( r.left(), r.top(), m_image.pixmap() );
        painter->restore();
    }
}

/******************************************************************/
/* Class: KWPartFrameSet                                          */
/******************************************************************/

/*================================================================*/
KWPartFrameSet::KWPartFrameSet( KWDocument *_doc, KWChild *_child )
    : KWFrameSet( _doc )
{
    child = _child;
    m_lock = false;
    kdDebug() << "KWPartFrameSet::KWPartFrameSet" << endl;
    connect( child, SIGNAL( changed( KoChild * ) ),
             this, SLOT( slotChildChanged() ) );
}

/*================================================================*/
KWPartFrameSet::~KWPartFrameSet()
{
}

void KWPartFrameSet::drawContents( QPainter * painter, const QRect & crect,
                                   QColorGroup &, bool onlyChanged, bool )
{
    if (!onlyChanged)
    {
        if ( !child || !child->document() || frames.isEmpty() )
        {
            kdDebug() << "KWPartFrameSet::drawContents " << this << " aborting. child=" << child << " child->document()=" << child->document() << " frames.count()=" << frames.count() << endl;
            return;
        }
        KWFrame *frame = frames.first();
        QRegion reg = frameClipRegion( painter, frame, crect );
        if ( !reg.isEmpty() )
        {
            //kdDebug() << "KWPartFrameSet::drawContents clipregion=" << DEBUGRECT(reg.boundingRect()) << endl;
            painter->save();
            QRect r = painter->viewport();
            painter->setClipRegion( reg );
            painter->setViewport( kWordDocument()->zoomItX( frame->x() ), kWordDocument()->zoomItY( frame->y() ),
                                  r.width(), r.height() );
            // painter->translate( frame->x(), frame->y() ); // messes up the clip regions
            QRect rframe( 0, 0, kWordDocument()->zoomItX( frames.first()->width() ),
                          kWordDocument()->zoomItY( frames.first()->height() ) ); // Not sure if applying the zoom here works
            child->document()->paintEverything( *painter, rframe, true, 0L,
                                                kWordDocument()->zoomedResolutionX(), kWordDocument()->zoomedResolutionY() );
            painter->setViewport( r );
            painter->restore();
        }// else kdDebug() << "KWPartFrameSet::drawContents " << this << " no intersection" << endl;
    }
}

/*================================================================*/
void KWPartFrameSet::updateFrames()
{
    if(frames.isEmpty() ) // Deleted frameset -> don't refresh
        return;

    if ( !m_lock )
    {
        m_lock = true; // setGeometry emits changed() !
        KoRect frect = *frames.first();
        //kdDebug() << "KWPartFrameSet::updateFrames frames.first()=" << DEBUGRECT(frect)
        //          << " child set to " << DEBUGRECT( kWordDocument()->zoomRect( frect ) ) << endl;
        child->setGeometry( kWordDocument()->zoomRect( frect ) );
        m_lock = false;
    }
    KWFrameSet::updateFrames();
}

/*================================================================*/
void KWPartFrameSet::save( QDomElement &parentElem )
{
    if ( frames.isEmpty() ) // Deleted frameset -> don't save
        return;
    KWFrameSet::save( parentElem );
}

/*================================================================*/
void KWPartFrameSet::load( QDomElement &attributes )
{
    KWFrameSet::load( attributes );
}


void KWPartFrameSet::slotChildChanged()
{
    KWFrame *frame = frames.first();
    if ( frame && !m_lock )
    {
        QRect r = getChild()->geometry();
        // Make "frame" follow the child's geometry (but frame is unzoomed)
        //kdDebug() << "KWPartFrameSet::slotChildChanged child's geometry " << DEBUGRECT( r ) << endl;
        frame->setCoords( r.left() / kWordDocument()->zoomedResolutionX(), r.top() / kWordDocument()->zoomedResolutionY(),
                          r.right() / kWordDocument()->zoomedResolutionX(), r.bottom() / kWordDocument()->zoomedResolutionY() );
        //kdDebug() << "KWPartFrameSet::slotChildChanged frame set to " << DEBUGRECT( *frame ) << endl;
        m_lock = true;
        kWordDocument()->frameChanged( frame ); // this triggers updateFrames
        m_lock = false;
    }
}

KWFrameSetEdit * KWPartFrameSet::createFrameSetEdit( KWCanvas * canvas )
{
    return new KWPartFrameSetEdit( this, canvas );
}

KWPartFrameSetEdit::~KWPartFrameSetEdit()
{
    kdDebug() << "KWPartFrameSetEdit::~KWPartFrameSetEdit" << endl;
}

void KWPartFrameSetEdit::mousePressEvent( QMouseEvent * )
{
    // activate child part
    partFrameSet()->updateFrames();
    KoDocument* part = partFrameSet()->getChild()->document();
    if ( !part )
        return;
    KWView * view = m_canvas->gui()->getView();
    //kdDebug() << "Child activated. part="<<part<<" child="<<child<<endl;
    view->partManager()->addPart( part, false );
    view->partManager()->setActivePart( part, view );
}

void KWPartFrameSetEdit::mouseDoubleClickEvent( QMouseEvent * )
{
    /// ## Pretty useless since single-click does it now...
    //partFrameSet()->activate( m_canvas->gui()->getView() );
}

void KWPartFrameSetEdit::drawContents( QPainter *p, const QRect &r, QColorGroup &cg, bool oc, bool rc)
{
    // Nothing to be painted by ourselves while editing a part
    // But when deactivating, kwcanvas doesn't get the click, so
    // we still have the edit object created...
    partFrameSet()->drawContents( p, r, cg, oc, rc );
}

/******************************************************************/
/* Class: KWFormulaFrameSet                                       */
/******************************************************************/


KWFormulaFrameSet::KWFormulaFrameSet( KWDocument *_doc, KFormulaContainer *_f )
    : KWFrameSet( _doc ), formula(_f), m_changed( false )
{
    if ( formula )
        connect(formula, SIGNAL(formulaChanged(int, int)),
                this, SLOT(slotFormulaChanged(int, int)));
}

KWFormulaFrameSet::~KWFormulaFrameSet()
{
    delete formula;
}

KWFrameSetEdit* KWFormulaFrameSet::createFrameSetEdit(KWCanvas* canvas)
{
    return new KWFormulaFrameSetEdit(this, canvas);
}

void KWFormulaFrameSet::drawContents( QPainter* painter, const QRect& crect,
                                      QColorGroup& cg, bool onlyChanged, bool resetChanged )
{
    if ( m_changed || !onlyChanged )
    {
        if ( resetChanged )
            m_changed = false;

        if ( !frames.isEmpty() )
        {
            //kdDebug(32001) << "KWFormulaFrameSet::drawContents1" << endl;
            QRegion reg = frameClipRegion( painter, frames.first(), crect );
            if ( !reg.isEmpty() ) {
                kdDebug() << "KWFormulaFrameSet::drawContents" << endl;
                painter->save();
                painter->setClipRegion( reg );
                cg.setBrush(QColorGroup::Base,frames.first()->getBackgroundColor());
                formula->draw( *painter, crect, cg );
                painter->restore();
            }
        }
    }
}

void KWFormulaFrameSet::drawContents( QPainter* painter, const QRect& crect,
                                      QColorGroup& cg, bool onlyChanged, bool resetChanged,
                                      KFormulaView* formulaView )
{
    if ( m_changed || !onlyChanged )
    {
        if ( resetChanged )
            m_changed = false;

        if ( !frames.isEmpty() )
        {
            //kdDebug(32001) << "KWFormulaFrameSet::drawContents2" << endl;
            QRegion reg = frameClipRegion( painter, frames.first(), crect );
            if ( !reg.isEmpty() ) {
                painter->save();
                painter->setClipRegion( reg );
                cg.setBrush(QColorGroup::Base,frames.first()->getBackgroundColor());
                formulaView->draw( *painter, crect, cg );
                painter->restore();
            }
        }
    }
}


// ## can this be done in the constructor instead (DF) ?
void KWFormulaFrameSet::create()
{
    if ( formula != 0 ) {
        updateFrames();
        return;
    }

    formula = m_doc->getFormulaDocument()->createFormula();
    connect(formula, SIGNAL(formulaChanged(int, int)),
            this, SLOT(slotFormulaChanged(int, int)));
    updateFrames();
}

void KWFormulaFrameSet::slotFormulaChanged(int width, int height)
{
    // Did I tell you that assignment to parameters is evil?
    width = static_cast<int>( width / kWordDocument()->zoomedResolutionX() ) + 5;
    height = static_cast<int>( height / kWordDocument()->zoomedResolutionY() ) + 5;

    double oldWidth = frames.first()->width();
    double oldHeight = frames.first()->height();

    frames.first()->setWidth( width );
    frames.first()->setHeight( height );

    if ( ( oldWidth > width ) || ( oldHeight > height ) ) {
        kWordDocument()->repaintAllViews( true ); // ## is "true" necessary ? Try false (DF)
    }

    updateFrames();
    kWordDocument()->layout();
    m_changed = true;
    emit repaintChanged( this );
}

void KWFormulaFrameSet::updateFrames()
{
    KWFrameSet::updateFrames();
    if ( !formula || frames.isEmpty() )
        return;

    formula->moveTo( kWordDocument()->zoomItX( frames.at(0)->x() ),
                     kWordDocument()->zoomItY( frames.at(0)->y() ) );

}

void KWFormulaFrameSet::save(QDomElement& parentElem)
{
    if ( frames.isEmpty() ) // Deleted frameset -> don't save
        return;
    QDomElement framesetElem = parentElem.ownerDocument().createElement("FRAMESET");
    parentElem.appendChild(framesetElem);

    framesetElem.setAttribute("frameType", static_cast<int>(getFrameType()));
    framesetElem.setAttribute("frameInfo", static_cast<int>(frameInfo));

    KWFrameSet::save(framesetElem); // Save all frames

    QDomElement formulaElem = parentElem.ownerDocument().createElement("FORMULA");
    framesetElem.appendChild(formulaElem);
    formula->save(formulaElem);
}

void KWFormulaFrameSet::load(QDomElement& attributes)
{
    KWFrameSet::load(attributes);
    QDomElement formulaElem = attributes.namedItem("FORMULA").toElement();
    if (!formulaElem.isNull()) {
        if (formula == 0) {
            formula = m_doc->getFormulaDocument()->createFormula();
            connect(formula, SIGNAL(formulaChanged(int, int)),
                    this, SLOT(slotFormulaChanged(int, int)));
        }
        if (!formula->load(formulaElem)) {
            kdError(32001) << "Error loading formula" << endl;
        }
    }
    else {
        kdError(32001) << "Missing FORMULA tag in FRAMESET" << endl;
    }
}

void KWFormulaFrameSet::zoom()
{
    formula->recalc();
}


/*================================================================*/

KWFormulaFrameSetEdit::KWFormulaFrameSetEdit(KWFormulaFrameSet* fs, KWCanvas* canvas)
        : KWFrameSetEdit(fs, canvas)
{
    kdDebug(32001) << "KWFormulaFrameSetEdit::KWFormulaFrameSetEdit" << endl;
    formulaView = new KFormulaView(fs->getFormula());
    //formulaView->setSmallCursor(true);

    connect( formulaView, SIGNAL( cursorChanged( bool, bool ) ),
             this, SLOT( cursorChanged( bool, bool ) ) );

    m_canvas->gui()->getView()->showFormulaToolbar(true);
    focusInEvent();
}

KWFormulaFrameSetEdit::~KWFormulaFrameSetEdit()
{
    kdDebug(32001) << "KWFormulaFrameSetEdit::~KWFormulaFrameSetEdit" << endl;
    focusOutEvent();
    m_canvas->gui()->getView()->showFormulaToolbar(false);
    delete formulaView;
}

/**
 * Paint this frameset in "has focus" mode (e.g. with a cursor)
 */
void KWFormulaFrameSetEdit::drawContents(QPainter* painter, const QRect& rect,
                                         QColorGroup& gc, bool onlyChanged, bool resetChanged)
{
    //kdDebug(32001) << "KWFormulaFrameSetEdit::drawContents" << endl;
    formulaFrameSet()->drawContents(painter, rect, gc, onlyChanged, resetChanged, formulaView);
}

void KWFormulaFrameSetEdit::keyPressEvent(QKeyEvent* event)
{
    //kdDebug(32001) << "KWFormulaFrameSetEdit::keyPressEvent" << endl;
    formulaView->keyPressEvent(event);
}

void KWFormulaFrameSetEdit::mousePressEvent(QMouseEvent* event)
{
    formulaView->mousePressEvent(event);
}

void KWFormulaFrameSetEdit::mouseMoveEvent(QMouseEvent* event)
{
    formulaView->mouseMoveEvent(event);
}

void KWFormulaFrameSetEdit::mouseReleaseEvent(QMouseEvent* event)
{
    formulaView->mouseReleaseEvent(event);
}

void KWFormulaFrameSetEdit::focusInEvent()
{
    //kdDebug(32001) << "KWFormulaFrameSetEdit::focusInEvent" << endl;
    formulaView->focusInEvent(0);
}

void KWFormulaFrameSetEdit::focusOutEvent()
{
    //kdDebug(32001) << "KWFormulaFrameSetEdit::focusOutEvent" << endl;
    formulaView->focusOutEvent(0);
}

void KWFormulaFrameSetEdit::doAutoScroll(QPoint)
{
}

void KWFormulaFrameSetEdit::copy()
{
    formulaView->getDocument()->copy();
}

void KWFormulaFrameSetEdit::cut()
{
    formulaView->getDocument()->cut();
}

void KWFormulaFrameSetEdit::paste()
{
    formulaView->getDocument()->paste();
}

void KWFormulaFrameSetEdit::selectAll()
{
    formulaView->slotSelectAll();
}

void KWFormulaFrameSetEdit::cursorChanged( bool visible, bool /*selecting*/ )
{
    if ( visible ) {
        int x = formulaView->getCursorPoint().x();
        int y = formulaView->getCursorPoint().y();
        m_canvas->ensureVisible( x, y );
    }
    formulaFrameSet()->setChanged();
    m_canvas->repaintChanged( formulaFrameSet(), true );
}


/*================================================================*/
bool KWFrameSet::isAHeader() const
{
    return ( frameInfo == FI_FIRST_HEADER || frameInfo == FI_EVEN_HEADER || frameInfo == FI_ODD_HEADER );
}

/*================================================================*/
bool KWFrameSet::isAFooter() const
{
    return ( frameInfo == FI_FIRST_FOOTER || frameInfo == FI_EVEN_FOOTER || frameInfo == FI_ODD_FOOTER );
}

/*================================================================*/
bool KWFrameSet::isAWrongHeader( KoHFType t ) const
{
    switch ( frameInfo ) {
    case FI_FIRST_HEADER: {
        if ( t == HF_FIRST_DIFF ) return false;
        return true;
    } break;
    case FI_EVEN_HEADER: {
        return false;
    } break;
    case FI_ODD_HEADER: {
        if ( t == HF_EO_DIFF ) return false;
        return true;
    } break;
    default: return false;
    }
}

/*================================================================*/
bool KWFrameSet::isAWrongFooter( KoHFType t ) const
{
    switch ( frameInfo ) {
    case FI_FIRST_FOOTER: {
        if ( t == HF_FIRST_DIFF ) return false;
        return true;
    } break;
    case FI_EVEN_FOOTER: {
        return false;
    } break;
    case FI_ODD_FOOTER: {
        if ( t == HF_EO_DIFF ) return false;
        return true;
    } break;
    default: return false;
    }
}

#include "kwframe.moc"
