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
#include "kwcommand.h"
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
      //mostRight( false ),
      m_pageNum( 0 ),
      backgroundColor( QBrush( QColor() ) ), // valid brush with invalid color ( default )
      brd_left( QColor(), Border::SOLID, 0 ),
      brd_right( QColor(), Border::SOLID, 0 ),
      brd_top( QColor(), Border::SOLID, 0 ),
      brd_bottom( QColor(), Border::SOLID, 0 ),
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

KWFrame::~KWFrame()
{
    //kdDebug() << "KWFrame::~KWFrame " << this << endl;
    if (selected)
        removeResizeHandles();
    if(anchor())
        anchor()->setDeleted(true);
}

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
    frm->setBackgroundColor( getBackgroundColor() );
    frm->setFrameBehaviour(getFrameBehaviour());
    frm->setNewFrameBehaviour(getNewFrameBehaviour());
    frm->setSheetSide(getSheetSide());
    frm->setPageNum(pageNum());
    frm->setLeftBorder(getLeftBorder());
    frm->setRightBorder(getRightBorder());
    frm->setTopBorder(getTopBorder());
    frm->setBottomBorder(getBottomBorder());
    frm->setBLeft(getBLeft());
    frm->setBRight(getBRight());
    frm->setBTop(getBTop());
    frm->setBBottom(getBBottom());
    if(anchor())
        frm->setAnchor(anchor());
    return frm;
}

void KWFrame::deleteAnchor()
{
    delete m_anchor;
    m_anchor = 0L;
}

// Insert all resize handles
void KWFrame::createResizeHandles() {
    removeResizeHandles();
    QList <KWView> pages = getFrameSet()->kWordDocument()->getAllViews();
    for (int i=pages.count() -1; i >= 0; i--)
        createResizeHandlesForPage(pages.at(i)->getGUI()->canvasWidget());
}

// Insert 8 resize handles which will be drawn in param canvas
void KWFrame::createResizeHandlesForPage(KWCanvas *canvas) {
    removeResizeHandlesForPage(canvas);

    for (unsigned int i=0; i < 8; i++) {
        KWResizeHandle * h = new KWResizeHandle( canvas, (KWResizeHandle::Direction)i, this );
        handles.append( h );
    }
}

// remove all the resize handles which will be drawn in param canvas
void KWFrame::removeResizeHandlesForPage(KWCanvas *canvas) {
    for( unsigned int i=0; i < handles.count(); i++) {
        if(handles.at ( i )->getCanvas() == canvas) {
            handles.remove(i--);
        }
    }
}

// remove all resizeHandles
void KWFrame::removeResizeHandles() {
    handles.clear();
}

// move the resizehandles to current location of frame
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
    outerRect.rLeft() -= Border::zoomWidthX( brd_left.ptWidth, doc, 1 );
    outerRect.rTop() -= Border::zoomWidthY( brd_top.ptWidth, doc, 1 );
    outerRect.rRight() += Border::zoomWidthX( brd_right.ptWidth, doc, 1 );
    outerRect.rBottom() += Border::zoomWidthY( brd_bottom.ptWidth, doc, 1 );
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
    m_current = 0;
    grpMgr = 0L;
}

KWFrameSet::~KWFrameSet()
{
}

void KWFrameSet::addFrame( KWFrame *_frame, bool recalc )
{
    if ( frames.findRef( _frame ) != -1 )
        return;

    frames.append( _frame );
    _frame->setFrameSet(this);
    if(recalc)
        updateFrames();
    /*if ( isFloating() )
    {
        findFirstAnchor();
        updateAnchors();
    }*/
}

void KWFrameSet::delFrame( unsigned int _num )
{
    KWFrame *frm = frames.at( _num );
    ASSERT( frm );
    delFrame(frm,true);
}

void KWFrameSet::delFrame( KWFrame *frm, bool remove )
{
    int _num = frames.findRef( frm );
    ASSERT( _num != -1 );
    if ( _num == -1 )
        return;

    // It's the anchor that deletes the frame, not the other way round.
    //if ( isFloating() )
    //    deleteAnchors();

    frm->setFrameSet(0L);
    if ( !remove )
    //if ( !del || !remove )
        frames.take( _num );
    else
        frames.remove( _num );

    updateFrames();

    //if ( isFloating() )
    //    updateAnchors();
}

void KWFrameSet::drawBorders( QPainter *painter, const QRect &crect, QRegion &region, KWViewMode *viewMode )
{
    // TODO use viewMode
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

        // Header/Footer : set background color and borders from the main frameset (why?)
        if ( isAHeader() || isAFooter() )
            frame = getFrame( 0 );
        QBrush bgBrush( frame->getBackgroundColor() );
	bgBrush.setColor( KWDocument::resolveBgColor( bgBrush.color(), painter ) );
        painter->setBrush( bgBrush );

        // Draw default borders using view settings except when printing, or disabled.
        QPen viewSetting( lightGray ); // TODO use qcolorgroup
        if ( ( painter->device()->devType() == QInternal::Printer ) ||
            !m_doc->getViewFrameBorders() )
        {
            viewSetting.setColor( bgBrush.color() );
        }

        // Draw borders either as the user defined them, or using the view settings.
        // Borders should be drawn _outside_ of the frame area
        // otherwise the frames will erase the border when painting themselves.

        Border::drawBorders( *painter, m_doc, frameRect,
                             frame->getLeftBorder(), frame->getRightBorder(),
                             frame->getTopBorder(), frame->getBottomBorder(),
                             1, viewSetting );
    }
    painter->restore();
}

void KWFrameSet::setFloating()
{
    // Find main text frame
    QListIterator<KWFrameSet> fit = m_doc->framesetsIterator();
    for ( ; fit.current() ; ++fit )
    {
        KWTextFrameSet * frameSet = dynamic_cast<KWTextFrameSet *>( fit.current() );
        if ( !frameSet || frameSet->getFrameInfo() != FI_BODY )
            continue;

        QTextParag* parag = 0L;
        int index = 0;
        QPoint cPoint( qRound( frames.first()->x() ), qRound( frames.first()->y() ) );
        frameSet->findPosition( cPoint, parag, index );
        setAnchored( frameSet, static_cast<KWTextParag *>( parag ), index );
        frameSet->layout();
        frames.first()->updateResizeHandles();
        m_doc->frameChanged(  frames.first() );
        return;
    }
}

void KWFrameSet::setAnchored( KWTextFrameSet* textfs, KWTextParag* parag, int index )
{
    ASSERT( textfs );
    ASSERT( parag );
    KWAnchorPosition pos;
    pos.textfs = textfs;
    pos.parag = parag;
    pos.index = index;
    setAnchored( pos );
}

void KWFrameSet::setAnchored( KWAnchorPosition & pos, bool placeHolderExists /* = false */ )
{
    if ( isFloating() )
        deleteAnchors();
    m_anchorPos = pos;
    updateAnchors( placeHolderExists );
}

void KWFrameSet::setAnchored( KWTextFrameSet* textfs )
{
    m_anchorPos.textfs = textfs;
    findFirstAnchor();
}

void KWFrameSet::setFixed()
{
    if ( isFloating() )
        deleteAnchors(); // is this a problem with undo/redo ?
    m_anchorPos.makeInvalid();
}

void KWFrameSet::updateAnchors( bool placeHolderExists /*= false */ /*only used when loading*/ )
{
    kdDebug() << "KWFrameSet::updateAnchors" << endl;
    int index = m_anchorPos.index;
    QListIterator<KWFrame> frameIt = frameIterator();
    for ( ; frameIt.current(); ++frameIt, ++index )
    {
        if ( ! frameIt.current()->anchor() )
        {
            // Anchor this frame, after the previous one
            KWAnchor * anchor = new KWAnchor( m_anchorPos.textfs->textDocument(), this,
                                              getFrameFromPtr( frameIt.current() ) );
            if ( !placeHolderExists )
                m_anchorPos.parag->insert( index, QChar('@') /*whatever*/ );
            m_anchorPos.parag->setCustomItem( index, anchor, 0 );
            frameIt.current()->setAnchor( anchor );
        }
    }
    m_anchorPos.parag->setChanged( true );
    //kdDebug() << "KWFrameSet::updateAnchors emit repaintChanged" << endl;
    emit repaintChanged( m_anchorPos.textfs );
}

void KWFrameSet::deleteAnchors()
{
    kdDebug() << "KWFrameSet::deleteAnchors" << endl;
    findFirstAnchor();
    int index = m_anchorPos.index;
    QListIterator<KWFrame> frameIt = frameIterator();
    for ( ; frameIt.current(); ++frameIt )
        if ( frameIt.current()->anchor() )
        {
            // Delete anchor (after removing anchor char)
            m_anchorPos.parag->removeCustomItem( index );
            frameIt.current()->deleteAnchor();
        }
}

void KWFrameSet::findFirstAnchor()
{
    ASSERT( frames.count() );
    if ( frames.count() > 0 && frames.first()->anchor() )
    {
        KWAnchor * anchor = frames.first()->anchor();
        m_anchorPos.parag = static_cast<KWTextParag *>( anchor->paragraph() );
        m_anchorPos.index = m_anchorPos.parag->findCustomItem( anchor );
    } else {
        kdDebug() << "KWFrameSet::findFirstAnchor no anchor !" << endl;
        m_anchorPos.parag = 0L;
    }
}

void KWFrameSet::moveFloatingFrame( int frameNum, const KoPoint &position )
{
    KWFrame * frame = frames.at( frameNum );
    ASSERT( frame );
    if ( frame->topLeft() != position )
    {
        kdDebug() << "KWFrameSet::moveFloatingFrame " << position.x() << "," << position.y() << endl;
        frame->moveTopLeft( position );
        kWordDocument()->updateAllFrames();
    }
}

KoPoint KWFrameSet::floatingFrameSize( int frameNum )
{
    KWFrame * frame = frames.at( frameNum );
    ASSERT( frame );
    return KoPoint( frame->width(), frame->height() ); // well maybe we should have a KoSize after all :)
}

KCommand * KWFrameSet::anchoredObjectCreateCommand( int frameNum )
{
    KWFrame * frame = frames.at( frameNum );
    ASSERT( frame );
    return new KWCreateFrameCommand( QString::null, kWordDocument(), frame );
}

KCommand * KWFrameSet::anchoredObjectDeleteCommand( int frameNum )
{
    KWFrame * frame = frames.at( frameNum );
    ASSERT( frame );
    return new KWDeleteFrameCommand( QString::null, kWordDocument(), frame );
}

KWFrame * KWFrameSet::getFrame( double _x, double _y )
{
    QListIterator<KWFrame> frameIt = frameIterator();
    for ( ; frameIt.current(); ++frameIt )
        if ( frameIt.current()->contains( KoPoint( _x, _y ) ) )
            return frameIt.current();
    return 0L;
}

KWFrame *KWFrameSet::getFrame( unsigned int _num )
{
    return frames.at( _num );
}

int KWFrameSet::getFrameFromPtr( KWFrame *frame )
{
    return frames.findRef( frame );
}

void KWFrameSet::updateFrames()
{
    m_framesOnTop.clear();

    // hack: table cells are not handled here, since they're not in the doc's frameset list.
    // ( so 'this' will never be found, and the whole method is useless )
    // TODO: hmm, well, store the "parent of this frameset", whether doc or frameset,
    // and look for the frameset list there. Hmm.
    if ( grpMgr )
        return;

    // Not visible ? Don't bother then.
    if ( !isVisible() )
        return;

    //kdDebug() << "KWFrameSet::updateFrames " << this << " " << getName() << endl;
    // Iterate over ALL framesets, to find those which have frames on top of us.
    // We'll use this information in various methods (adjust[LR]Margin, drawContents etc.)
    // So we want it cached.
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

        if ( !foundThis || !frameSet->isVisible() )
            continue;

        // Floating frames are not "on top", they are "inside".
        if ( frameSet->isFloating() )
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
        m_anchorPos.parag->invalidate( 0 );
        QListIterator<KWFrame> frameIt = frameIterator();
        for ( ; frameIt.current(); ++frameIt )
        {
            //kdDebug() << "KWFrameSet::updateFrames anchor=" << frameIt.current()->anchor() << endl;
            if ( frameIt.current()->anchor() )
                frameIt.current()->anchor()->resize();
        }
    }
}

void KWFrameSet::drawContents( QPainter *p, const QRect & crect, QColorGroup &cg,
                               bool onlyChanged, bool resetChanged,
                               KWFrameSetEdit *edit, KWViewMode *viewMode /*TODO*/)
{
    //kdDebug(32002) << "KWFrameSet::drawContents " << this << " " << getName()
    //               << " onlyChanged=" << onlyChanged << " resetChanged=" << resetChanged
    //               << endl;

    QListIterator<KWFrame> frameIt( frameIterator() );
    KWFrame * copyFrame = 0L;
    int copyFrameTop = 0;
    int totalHeight = 0;
    for ( ; frameIt.current(); ++frameIt )
    {
        KWFrame *frame = frameIt.current();
        if ( !frame->isValid() )
        {
            kdDebug(32002) << "KWFrameSet::drawContents invalid frame " << frame << endl;
            continue;
        }

        QRect r(crect);
        QRect frameRect( m_doc->zoomRect( *frame ) );
        //kdDebug(32002) << "KWTFS::drawContents frame=" << frame << " cr=" << DEBUGRECT(r) << endl;
        r = r.intersect( frameRect );
        //kdDebug(32002) << "                    framerect=" << DEBUGRECT(*frame) << " intersec=" << DEBUGRECT(r) << " todraw=" << !r.isEmpty() << endl;
        if ( !r.isEmpty() )
        {
            // This translates the coordinates in the document contents
            // ( frame and r are up to here in this system )
            // into the QTextDocument's coordinate system
            // (which doesn't have frames, borders, etc.)
            int offsetX = frameRect.left();
            int offsetY = frameRect.top() - ( copyFrame ? copyFrameTop : totalHeight );

            r.moveBy( -offsetX, -offsetY );   // portion of the frame to be drawn, in qrt coords

            QRegion reg = frameClipRegion( p, frame, crect );
            if ( !reg.isEmpty() )
            {
                p->save();
                p->setClipRegion( reg );
                p->translate( offsetX, offsetY );

                QBrush bgBrush( frame->getBackgroundColor() );
                bgBrush.setColor( KWDocument::resolveBgColor( bgBrush.color(), p ) );
                cg.setBrush( QColorGroup::Base, bgBrush );

                drawFrame( frame, p, r, cg, onlyChanged, resetChanged, edit );

                p->restore();
            }
        }
        if ( frame->getNewFrameBehaviour() != Copy )
            copyFrame = 0L;
        else if ( !copyFrame )
        {
            copyFrame = frame;
            copyFrameTop = totalHeight;
        }
        totalHeight += frameRect.height();
    }
}

bool KWFrameSet::contains( double mx, double my )
{
    QListIterator<KWFrame> frameIt = frameIterator();
    for ( ; frameIt.current(); ++frameIt )
        if ( frameIt.current()->contains( KoPoint( mx, my ) ) )
            return true;

    return false;
}

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

void KWFrameSet::deSelectFrame( double mx, double my )
{
    QListIterator<KWFrame> frameIt = frameIterator();
    for ( ; frameIt.current(); ++frameIt )
        if ( frameIt.current()->contains( KoPoint( mx, my ) ) )
            frameIt.current()->setSelected( false );
}

QCursor KWFrameSet::getMouseCursor( double mx, double my )
{
    KWFrame * frame = getFrame( mx, my );

    if ( frame == 0L )
        return Qt::arrowCursor;

    if ( !frame->isSelected() && !grpMgr )
        return Qt::arrowCursor;

    return frame->getMouseCursor( mx, my, grpMgr ? true : false );
}

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

        frameElem.setAttribute( "left", frame->left() );
        frameElem.setAttribute( "top", frame->top() );
        frameElem.setAttribute( "right", frame->right() );
        frameElem.setAttribute( "bottom", frame->bottom() );

        if(frame->getRunAround()!=RA_NO)
            frameElem.setAttribute( "runaround", static_cast<int>( frame->getRunAround() ) );

        if(frame->getRunAroundGap()!=0)
            frameElem.setAttribute( "runaroundGap", frame->getRunAroundGap() );

        if(frame->getLeftBorder().ptWidth!=0)
            frameElem.setAttribute( "lWidth", frame->getLeftBorder().ptWidth );

        if(frame->getLeftBorder().color.isValid())
        {
            frameElem.setAttribute( "lRed", frame->getLeftBorder().color.red() );
            frameElem.setAttribute( "lGreen", frame->getLeftBorder().color.green() );
            frameElem.setAttribute( "lBlue", frame->getLeftBorder().color.blue() );
        }
        if(frame->getLeftBorder().style != Border::SOLID)
            frameElem.setAttribute( "lStyle", static_cast<int>( frame->getLeftBorder().style ) );

        if(frame->getRightBorder().ptWidth!=0)
            frameElem.setAttribute( "rWidth", frame->getRightBorder().ptWidth );

        if(frame->getRightBorder().color.isValid())
        {
            frameElem.setAttribute( "rRed", frame->getRightBorder().color.red() );
            frameElem.setAttribute( "rGreen", frame->getRightBorder().color.green() );
            frameElem.setAttribute( "rBlue", frame->getRightBorder().color.blue() );
        }
        if(frame->getRightBorder().style != Border::SOLID)
            frameElem.setAttribute( "rStyle", static_cast<int>( frame->getRightBorder().style ) );

        if(frame->getTopBorder().ptWidth!=0)
            frameElem.setAttribute( "tWidth", frame->getTopBorder().ptWidth );

        if(frame->getTopBorder().color.isValid())
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
        if(frame->getBottomBorder().color.isValid()) {
            frameElem.setAttribute( "bRed", frame->getBottomBorder().color.red() );
            frameElem.setAttribute( "bGreen", frame->getBottomBorder().color.green() );
            frameElem.setAttribute( "bBlue", frame->getBottomBorder().color.blue() );
        }
        if(frame->getBottomBorder().style != Border::SOLID)
            frameElem.setAttribute( "bStyle", static_cast<int>( frame->getBottomBorder().style ) );

        if(frame->getBackgroundColor().color().isValid())
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

        //if(frame->getNewFrameBehaviour()!=Reconnect) // always save this one, since the default value depends on the type of frame, etc.
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
            l.ptWidth = 0;
            r.ptWidth = 0;
            t.ptWidth = 0;
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
	    if ( frameElem.hasAttribute("lRed") )
                l.color.setRgb(
                    KWDocument::getAttribute( frameElem, "lRed", 0 ),
                    KWDocument::getAttribute( frameElem, "lGreen", 0 ),
                    KWDocument::getAttribute( frameElem, "lBlue", 0 ) );
	    if ( frameElem.hasAttribute("rRed") )
                r.color.setRgb(
                    KWDocument::getAttribute( frameElem, "rRed", 0 ),
                    KWDocument::getAttribute( frameElem, "rGreen", 0 ),
                    KWDocument::getAttribute( frameElem, "rBlue", 0 ) );
	    if ( frameElem.hasAttribute("tRed") )
                t.color.setRgb(
                    KWDocument::getAttribute( frameElem, "tRed", 0 ),
                    KWDocument::getAttribute( frameElem, "tGreen", 0 ),
                    KWDocument::getAttribute( frameElem, "tBlue", 0 ) );
	    if ( frameElem.hasAttribute("bRed") )
                b.color.setRgb(
                    KWDocument::getAttribute( frameElem, "bRed", 0 ),
                    KWDocument::getAttribute( frameElem, "bGreen", 0 ),
                    KWDocument::getAttribute( frameElem, "bBlue", 0 ) );
            l.style = static_cast<Border::BorderStyle>( KWDocument::getAttribute( frameElem, "lStyle", Border::SOLID ) );
            r.style = static_cast<Border::BorderStyle>( KWDocument::getAttribute( frameElem, "rStyle", Border::SOLID ) );
            t.style = static_cast<Border::BorderStyle>( KWDocument::getAttribute( frameElem, "tStyle", Border::SOLID ) );
            b.style = static_cast<Border::BorderStyle>( KWDocument::getAttribute( frameElem, "bStyle", Border::SOLID ) );
            QColor c;
	    if ( frameElem.hasAttribute("bkRed") )
                c.setRgb(
                    KWDocument::getAttribute( frameElem, "bkRed", 0 ),
                    KWDocument::getAttribute( frameElem, "bkGreen", 0 ),
                    KWDocument::getAttribute( frameElem, "bkBlue", 0 ) );

            double lpt = frameElem.attribute( "bleftpt" ).toDouble();
            double rpt = frameElem.attribute( "brightpt" ).toDouble();
            double tpt = frameElem.attribute( "btoppt" ).toDouble();
            double bpt = frameElem.attribute( "bbottompt" ).toDouble();
            FrameBehaviour autoCreateNewValue = static_cast<FrameBehaviour>( KWDocument::getAttribute( frameElem, "autoCreateNewFrame", AutoCreateNewFrame ) );
            // Old documents had no "NewFrameBehaviour" for footers/headers -> default to Copy.
            NewFrameBehaviour defaultValue = ( isAHeader() || isAFooter() ) ? Copy : Reconnect;
            NewFrameBehaviour newFrameBehaviour = static_cast<NewFrameBehaviour>( KWDocument::getAttribute( frameElem, "newFrameBehaviour", defaultValue ) );
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

bool KWFrameSet::hasSelectedFrame()
{
    for ( unsigned int i = 0; i < frames.count(); i++ ) {
        if ( frames.at( i )->isSelected() )
            return true;
    }

    return false;
}

void KWFrameSet::setVisible( bool v )
{
    visible = v;
    if ( visible )
        // updateFrames was disabled while we were invisible
        updateFrames();
}

bool KWFrameSet::isVisible() const
{
    return ( visible &&
             !frames.isEmpty() &&
             (!isAHeader() || m_doc->isHeaderVisible()) &&
             (!isAFooter() || m_doc->isFooterVisible()) &&
             !isAWrongHeader( m_doc->getHeaderType() ) &&
             !isAWrongFooter( m_doc->getFooterType() ) );
}

bool KWFrameSet::isAHeader() const
{
    return ( frameInfo == FI_FIRST_HEADER || frameInfo == FI_EVEN_HEADER || frameInfo == FI_ODD_HEADER );
}

bool KWFrameSet::isAFooter() const
{
    return ( frameInfo == FI_FIRST_FOOTER || frameInfo == FI_EVEN_FOOTER || frameInfo == FI_ODD_FOOTER );
}

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

void KWFrameSet::zoom()
{
}

void KWFrameSet::finalize()
{
    //kdDebug() << "KWFrameSet::finalize ( calls updateFrames + zoom ) " << this << endl;
    updateFrames();
    zoom();
}

// This determines where to clip the painter to draw the contents of a given frame
// It clips to the frame, and clips out any "on top" frame.
QRegion KWFrameSet::frameClipRegion( QPainter * painter, KWFrame *frame, const QRect & crect )
{
    QRect rc = painter->xForm( kWordDocument()->zoomRect( *frame ) );
    rc &= painter->xForm( crect ); // intersect
    //kdDebug(32002) << "KWTextFrameSet::frameClipRegion frame=" << DEBUGRECT(*frame)
    //               << " clip region rect=" << DEBUGRECT(rc)
    //               << " rc.isEmpty()=" << rc.isEmpty() << endl;
    if ( !rc.isEmpty() )
    {
        QRegion reg( rc );
        QListIterator<KWFrame> fIt( m_framesOnTop );
        for ( ; fIt.current() ; ++fIt )
        {
            QRect r = painter->xForm( fIt.current()->outerRect() );
            //kdDebug(32002) << "frameClipRegion subtract rect "<< DEBUGRECT(r) << endl;
            reg -= r; // subtract
        }
        return reg;
    } else return QRegion();
}

#ifndef NDEBUG
void KWFrameSet::printDebug()
{
    static const char * typeFrameset[] = { "base", "txt", "pic", "part", "formula", "table","ERROR" };
    static const char * infoFrameset[] = { "body", "first header", "odd headers", "even headers",
                                           "first footer", "odd footers", "even footers", "footnote", "ERROR" };
    static const char * frameBh[] = { "AutoExtendFrame", "AutoCreateNewFrame", "Ignore", "ERROR" };
    static const char * newFrameBh[] = { "Reconnect", "NoFollowup", "Copy" };
    static const char * runaround[] = { "No Runaround", "Bounding Rect", "Horizontal Space", "ERROR" };

    kdDebug() << " |  Visible: " << isVisible() << endl;
    kdDebug() << " |  Type: " << typeFrameset[ getFrameType() ] << endl;
    kdDebug() << " |  Info: " << infoFrameset[ getFrameInfo() ] << endl;
    kdDebug() << " |  Number of frames on top: " << m_framesOnTop.count() << endl;

    QListIterator<KWFrame> frameIt = frameIterator();
    for ( unsigned int j = 0; frameIt.current(); ++frameIt, ++j ) {
        KWFrame * frame = frameIt.current();
        kdDebug() << " +-- Frame " << j << " of "<< getNumFrames() << "    (" << frame << ")" << endl;
        printDebug( frame );
        kdDebug() << "     Rectangle : " << frame->x() << "," << frame->y() << " " << frame->width() << "x" << frame->height() << endl;
        kdDebug() << "     RunAround: "<< runaround[ frame->getRunAround() ] << endl;
        kdDebug() << "     FrameBehaviour: "<< frameBh[ frame->getFrameBehaviour() ] << endl;
        kdDebug() << "     NewFrameBehaviour: "<< newFrameBh[ frame->getNewFrameBehaviour() ] << endl;
        QColor col = frame->getBackgroundColor().color();
        kdDebug() << "     BackgroundColor: "<< ( col.isValid() ? col.name().latin1() : "(default)" ) << endl;
        kdDebug() << "     SheetSide "<< frame->getSheetSide() << endl;
        if(frame->isSelected())
            kdDebug() << " *   Page "<< frame->pageNum() << endl;
        else
            kdDebug() << "     Page "<< frame->pageNum() << endl;
    }
}

void KWFrameSet::printDebug( KWFrame * )
{
}

#endif

KWFrameSetEdit::KWFrameSetEdit( KWFrameSet * fs, KWCanvas * canvas )
     : m_fs(fs), m_canvas(canvas), m_currentFrame( fs->getFrame(0) )
{
}

void KWFrameSetEdit::drawContents( QPainter *p, const QRect &crect,
                                   QColorGroup &cg, bool onlyChanged, bool resetChanged,
                                   KWViewMode * viewMode )
{
    frameSet()->drawContents( p, crect, cg, onlyChanged, resetChanged, this, viewMode );
}

/******************************************************************/
/* Class: KWPictureFrameSet                                       */
/******************************************************************/
KWPictureFrameSet::KWPictureFrameSet( KWDocument *_doc, const QString & name )
    : KWFrameSet( _doc )
{
    if ( name.isEmpty() )
        m_name = _doc->generateFramesetName( i18n( "Picture Frameset %1" ) );
    else
        m_name = name;
}

KWPictureFrameSet::~KWPictureFrameSet() {
}

void KWPictureFrameSet::setFileName( const QString &_filename, const QSize &_imgSize )
{
    KWImageCollection *collection = m_doc->imageCollection();

    m_image = collection->image( _filename );

    m_image = m_image.scale( _imgSize );
}

void KWPictureFrameSet::setSize( QSize _imgSize )
{
    m_image = m_image.scale( _imgSize );
}

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

void KWPictureFrameSet::drawFrame( KWFrame *frame, QPainter *painter, const QRect &,
                                   QColorGroup &, bool, bool, KWFrameSetEdit * )
{
    QSize s ( kWordDocument()->zoomItX( frame->width() ), kWordDocument()->zoomItY( frame->height() ) );

    if ( s != m_image.image().size() )
        m_image = m_image.scale( s );
    painter->drawPixmap( 0, 0, m_image.pixmap() );
}

/******************************************************************/
/* Class: KWPartFrameSet                                          */
/******************************************************************/
KWPartFrameSet::KWPartFrameSet( KWDocument *_doc, KWChild *_child, const QString & name )
    : KWFrameSet( _doc )
{
    child = _child;
    m_lock = false;
    kdDebug() << "KWPartFrameSet::KWPartFrameSet" << endl;
    connect( child, SIGNAL( changed( KoChild * ) ),
             this, SLOT( slotChildChanged() ) );
    if ( name.isEmpty() )
        m_name = _doc->generateFramesetName( i18n( "Part Frameset %1" ) );
    else
        m_name = name;
}

KWPartFrameSet::~KWPartFrameSet()
{
}

void KWPartFrameSet::drawFrame( KWFrame* frame, QPainter * painter, const QRect & crect,
                                QColorGroup &, bool onlyChanged, bool, KWFrameSetEdit * )
{
    if (!onlyChanged)
    {
        if ( !child || !child->document() )
        {
            kdDebug() << "KWPartFrameSet::drawFrame " << this << " aborting. child=" << child << " child->document()=" << child->document() << endl;
            return;
        }

        // We have to define better what the rect that we pass, means. Does it include zooming ? (yes I think)
        // Does it define the area to be repainted only ? (here it doesn't, really, but it should)
        QRect rframe( 0, 0, kWordDocument()->zoomItX( frame->width() ),
                      kWordDocument()->zoomItY( frame->height() ) );
        //kdDebug() << "rframe=" << DEBUGRECT( rframe ) << endl;

        child->document()->paintEverything( *painter, rframe, true, 0L,
                                            kWordDocument()->zoomedResolutionX(), kWordDocument()->zoomedResolutionY() );

    } //else kdDebug() << "KWPartFrameSet::drawFrame " << this << " onlychanged=true!" << endl;
}

void KWPartFrameSet::updateFrames()
{
    if( frames.isEmpty() ) // Deleted frameset -> don't refresh
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

void KWPartFrameSet::save( QDomElement &parentElem )
{
    if ( frames.isEmpty() ) // Deleted frameset -> don't save
        return;
    KWFrameSet::save( parentElem );
}

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

/******************************************************************/
/* Class: KWFormulaFrameSet                                       */
/******************************************************************/
KWFormulaFrameSet::KWFormulaFrameSet( KWDocument *_doc, const QString & name )
    : KWFrameSet( _doc ), m_changed( false )
{
    formula = _doc->getFormulaDocument()->createFormula();
    // With the new drawing scheme (drawFrame being called with translated painter)
    // there is no need to move the KFormulaContainer anymore, it remains at (0,0).
    formula->moveTo( 0, 0 );

    connect(formula, SIGNAL(formulaChanged(int, int)),
            this, SLOT(slotFormulaChanged(int, int)));
    if ( name.isEmpty() )
        m_name = _doc->generateFramesetName( i18n( "Formula Frameset %1" ) );
    else
        m_name = name;
}

KWFormulaFrameSet::~KWFormulaFrameSet()
{
    delete formula;
}

KWFrameSetEdit* KWFormulaFrameSet::createFrameSetEdit(KWCanvas* canvas)
{
    return new KWFormulaFrameSetEdit(this, canvas);
}

void KWFormulaFrameSet::drawFrame( KWFrame* frame, QPainter* painter, const QRect& crect,
                                   QColorGroup& cg, bool onlyChanged, bool resetChanged,
                                   KWFrameSetEdit *edit )
{
    if ( m_changed || !onlyChanged )
    {
        if ( resetChanged )
            m_changed = false;

        if ( edit )
        {
            KWFormulaFrameSetEdit * formulaEdit = static_cast<KWFormulaFrameSetEdit *>(edit);
            formulaEdit->getFormulaView()->draw( *painter, crect, cg );
        }
        else
        {
            formula->draw( *painter, crect, cg );
        }
    }
}

void KWFormulaFrameSet::slotFormulaChanged(int width, int height)
{
    if ( frames.isEmpty() )
        return;
    // Did I tell you that assignment to parameters is evil?
    width = static_cast<int>( width / kWordDocument()->zoomedResolutionX() ) + 5;
    height = static_cast<int>( height / kWordDocument()->zoomedResolutionY() ) + 5;

    double oldWidth = frames.first()->width();
    double oldHeight = frames.first()->height();

    frames.first()->setWidth( width );
    frames.first()->setHeight( height );

    if ( ( oldWidth != width ) || ( oldHeight != height ) ) {
        kWordDocument()->repaintAllViews( false );
    }

    updateFrames();
    kWordDocument()->layout();
    m_changed = true;
    emit repaintChanged( this );
}

void KWFormulaFrameSet::updateFrames()
{
    KWFrameSet::updateFrames();
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
    if ( !frames.isEmpty() )
    {
        formula->recalc();
        KWFrameSet::zoom();
    }
}


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

#include "kwframe.moc"
