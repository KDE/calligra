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
#include "kwgroupmanager.h"
#include "kwtextparag.h"
#include "defs.h"
#include "kwutils.h"
#include "resizehandles.h"

#include <koPageLayoutDia.h>

#include <fstream>
#include <unistd.h>
#include <limits.h>

#include <qpicture.h>
#include <qvaluelist.h>
#include <qwidget.h>
#include <qpixmap.h>
#include <qfile.h>
#include <qscrollview.h>
#include <qarray.h>
#include <qfile.h>
#include <qpalette.h>

#include <koFrame.h>
#include <assert.h>

#include <kdebug.h>

#undef getPointBasedAttribute
#define getPointBasedAttribute(attribute, element, attributeName, defaultValue) \
do \
{ \
    attribute##pt = KWDocument::getAttribute( element, attributeName, defaultValue ); \
    attribute##mm = POINT_TO_MM( attribute##pt ); \
    attribute##inch = POINT_TO_INCH( attribute##pt ); \
} while (0)

/******************************************************************/
/* Class: KWFrame                                                 */
/******************************************************************/

/*================================================================*/
KWFrame::KWFrame(KWFrameSet *fs, int left, int top, int width, int height, RunAround _ra, KWUnit _gap )
    : QRect( left, top, width, height )
{
    //kdDebug() << "KWFrame::KWFrame " << this << " left=" << left << " top=" << top << endl;
    frameSet = fs;
    runAround = _ra;
    runAroundGap = _gap;
    pageNum = fs ? fs->kWordDocument()->getPageOfRect( *this ) : 0;
    handles.setAutoDelete(true);
    intersections.setAutoDelete( true );
    selected = false;
    mostRight = false;
    emptyRegionDirty = TRUE;

    backgroundColor = QBrush( Qt::white );
    brd_left.color = getBackgroundColor().color();
    brd_left.style = Border::SOLID;
    brd_left.ptWidth = 0;
    brd_right.color = getBackgroundColor().color();
    brd_right.style = Border::SOLID;
    brd_right.ptWidth = 0;
    brd_top.color = getBackgroundColor().color();
    brd_top.style = Border::SOLID;
    brd_top.ptWidth = 0;
    brd_bottom.color = getBackgroundColor().color();
    brd_bottom.style = Border::SOLID;
    brd_bottom.ptWidth = 0;

    frameBehaviour = AutoCreateNewFrame;
    newFrameBehaviour = Reconnect;
    sheetSide = AnySide;
}

/*================================================================*/
KWFrame::~KWFrame()
{
    //kdDebug() << "KWFrame::~KWFrame " << this << endl;
    if (selected)
        removeResizeHandles();
}

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
        left += static_cast<int>(runAroundGap.pt());
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
        _right += static_cast<int>(runAroundGap.pt());
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

/*================================================================*/
QCursor KWFrame::getMouseCursor( int mx, int my, bool table )
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

/*================================================================*/
FrameInfo KWFrame::getFrameInfo()
{
    if(frameSet)
        return frameSet->getFrameInfo();
    return (FrameInfo) -1;
}

/*================================================================*/
FrameType KWFrame::getFrameType()
{
    if(frameSet)
        return frameSet->getFrameType();
    return (FrameType)  -1;
}

/*================================================================*/
KWFrame *KWFrame::getCopy() {
    /* returns a deep copy of self */
    KWFrame *frm = new KWFrame(getFrameSet(), x(), y(),width(), height(), getRunAround(), getRunAroundGap() );
    frm->setBackgroundColor( QBrush( getBackgroundColor() ) );
    frm->setFrameBehaviour(getFrameBehaviour());
    frm->setNewFrameBehaviour(getNewFrameBehaviour());
    frm->setSheetSide(getSheetSide());
    frm->setPageNum(getPageNum());

    return frm;
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

/******************************************************************/
/* Class: KWFrameSet                                              */
/******************************************************************/

/*================================================================*/
KWFrameSet::KWFrameSet( KWDocument *_doc )
    : frames(), removeableHeader( false ), visible( true )
{
    doc = _doc;
    frames.setAutoDelete( true );
    frameInfo = FI_BODY;
    current = 0;
    grpMgr = 0L;
}

/*================================================================*/
/*void KWFrameSet::addFrame( KWFrame _frame )
{
    addFrame(_frame.getCopy());
}*/

/*================================================================*/
void KWFrameSet::addFrame( KWFrame *_frame )
{
    if ( frames.findRef( _frame ) != -1 )
        return;

    frames.append( _frame );
    _frame->setFrameSet(this);
    updateFrames();
}

/*================================================================*/
void KWFrameSet::delFrame( unsigned int _num )
{
    KWFrame *frm = frames.at( _num );
    delFrame(frm,true);
}

/*================================================================*/
void KWFrameSet::delFrame( KWFrame *frm, bool remove )
{
    int _num = frames.findRef( frm );
    if ( _num == -1 )
        return;

    //KWFrame *f;

    // This check is totally irrelevant since we allready check
    // for duplicate occurences on addFrame (TZ)
/*
    bool del = true;
    int i = 0;
    for ( f = frames.first(); f != 0; f = frames.next(), i++ ) {
        if ( f == frm && i != _num ) {
            del = false;
            break;
        }
    } */

    frm->setFrameSet(0L);
    if ( !remove )
    //if ( !del || !remove )
        frames.take( _num );
    else
        frames.remove( _num );

    updateFrames();
}

/*================================================================*/
KWFrame * KWFrameSet::getFrame( int _x, int _y )
{
    QListIterator<KWFrame> frameIt = frameIterator();
    for ( ; frameIt.current(); ++frameIt )
        if ( frameIt.current()->contains( QPoint( _x, _y ) ) )
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
    QListIterator<KWFrameSet> framesetIt( doc->framesetsIterator() );
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
    //kdDebug() << "KWTextFrameSet::updateFrames frame on top:" << m_framesOnTop.count() << endl;
}

/*================================================================*/
bool KWFrameSet::contains( unsigned int mx, unsigned int my )
{
    for ( unsigned int i = 0; i < frames.count(); i++ ) {
        if ( frames.at( i )->contains( QPoint( mx, my ) ) ) return true;
    }

    return false;
}

/*================================================================*/
/* Select the first frame where the x and y coords fall into
   returns 0 if none was selected, return 1 if selected, return 2
   if the frame was allready selected.
*/
int KWFrameSet::selectFrame( unsigned int mx, unsigned int my, bool simulate )
{
    for ( unsigned int i = 0; i < frames.count(); i++ ) {
        if ( frames.at( i )->contains( QPoint( mx, my ) ) ) {
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
void KWFrameSet::deSelectFrame( unsigned int mx, unsigned int my )
{
    for ( unsigned int i = 0; i < frames.count(); i++ ) {
        if ( frames.at( i )->contains( QPoint( mx, my ) ) )
            frames.at( i )->setSelected( false );
    }
}

/*================================================================*/
QCursor KWFrameSet::getMouseCursor( unsigned int mx, unsigned int my )
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

        if(frame->getRunAroundGap().mm()!=1)
            frameElem.setAttribute( "runaroundGap", frame->getRunAroundGap().pt() );

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
        if(frame->getBLeft().pt() != 0)
            frameElem.setAttribute( "bleftpt", frame->getBLeft().pt() );

        if(frame->getBRight().pt()!=0)
            frameElem.setAttribute( "brightpt", frame->getBRight().pt() );

        if(frame->getBTop().pt()!=0)
            frameElem.setAttribute( "btoppt", frame->getBTop().pt() );

        if(frame->getBBottom().pt()!=0)
            frameElem.setAttribute( "bbottompt", frame->getBBottom().pt() );

        if(frame->getFrameBehaviour()!=AutoCreateNewFrame)
            frameElem.setAttribute( "autoCreateNewFrame", static_cast<int>( frame->getFrameBehaviour()) );

        if(frame->getNewFrameBehaviour()!=Reconnect)
            frameElem.setAttribute( "newFrameBehaviour", static_cast<int>( frame->getNewFrameBehaviour()) );

        if(frame->getSheetSide()!= AnySide)
            frameElem.setAttribute( "sheetSide", static_cast<int>( frame->getSheetSide()) );

        if(doc->processingType() == KWDocument::WP) {
            if(doc->getFrameSet(0) == this) break;
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
    QDomNodeList listFrames = attributes.elementsByTagName ( "FRAME" );
    for (unsigned int item = 0; item < listFrames.count(); item++)
    {
        QDomElement frameElem = listFrames.item( item ).toElement();
        QRect rect;
        NewFrameBehaviour newFrameBehaviour = Reconnect;
        FrameBehaviour autoCreateNewValue = AutoCreateNewFrame;
        SheetSide sheetSide = AnySide;
        Border l, r, t, b;
        double lmm = 0.0, linch = 0.0, rmm = 0.0, rinch = 0.0, tmm = 0.0, tinch = 0.0, bmm = 0.0, binch = 0.0, ramm = 1.0, rainch = 0.0393701;
        double lpt = 0.0, rpt = 0.0, tpt = 0.0, bpt = 0.0, rapt = 2.0;

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
        QColor c( Qt::white );

        rect.setLeft( KWDocument::getAttribute( frameElem, "left", 0 ) );
        rect.setTop( KWDocument::getAttribute( frameElem, "top", 0 ) );
        rect.setRight( KWDocument::getAttribute( frameElem, "right", 0 ) );
        rect.setBottom( KWDocument::getAttribute( frameElem, "bottom", 0 ) );
        RunAround runaround = static_cast<RunAround>( KWDocument::getAttribute( frameElem, "runaround", 0 ) );
        KWUnit runAroundGap;
        runAroundGap.setPT( KWDocument::getAttribute( frameElem, "runaroundGap", 0.0 ) );
        getPointBasedAttribute( ra, frameElem, "runaGapPT", 0.0 );
        l.ptWidth = KWDocument::getAttribute( frameElem, "lWidth", 0 );
        r.ptWidth = KWDocument::getAttribute( frameElem, "rWidth", 0 );
        t.ptWidth = KWDocument::getAttribute( frameElem, "tWidth", 0 );
        b.ptWidth = KWDocument::getAttribute( frameElem, "bWidth", 0 );
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
        c.setRgb(
            KWDocument::getAttribute( frameElem, "bkRed", 0xff ),
            KWDocument::getAttribute( frameElem, "bkGreen", 0xff ),
            KWDocument::getAttribute( frameElem, "bkBlue", 0xff ) );
        getPointBasedAttribute( l, frameElem, "bleftpt", 0.0 );
        getPointBasedAttribute( r, frameElem, "brightpt", 0.0 );
        getPointBasedAttribute( t, frameElem, "btoppt", 0.0 );
        getPointBasedAttribute( b, frameElem, "bbottompt", 0.0 );
        autoCreateNewValue = static_cast<FrameBehaviour>( KWDocument::getAttribute( frameElem, "autoCreateNewFrame", AutoCreateNewFrame ) );
        newFrameBehaviour = static_cast<NewFrameBehaviour>( KWDocument::getAttribute( frameElem, "newFrameBehaviour", Reconnect ) );
        sheetSide = static_cast<SheetSide>( KWDocument::getAttribute( frameElem, "sheetSide", AnySide ) );

        KWFrame * frame = new KWFrame(this, rect.x(), rect.y(), rect.width(), rect.height(), runaround, rainch == -1 ? runAroundGap : KWUnit( rapt, ramm, rainch ) );
        if(c==l.color && l.ptWidth==1 && l.style==0 )
	  l.ptWidth=0;
	frame->setLeftBorder( l );
	if(c==r.color  && r.ptWidth==1 && r.style==0)
	  r.ptWidth=0;
        frame->setRightBorder( r );
	if(c==t.color && t.ptWidth==1 && t.style==0 )
	  t.ptWidth=0;
        frame->setTopBorder( t );
	if(c==b.color && b.ptWidth==1 && b.style==0 )
	  b.ptWidth=0;
        frame->setBottomBorder( b );
        frame->setBackgroundColor( QBrush( c ) );
        frame->setBLeft( KWUnit( lpt, lmm, linch ) );
        frame->setBRight( KWUnit( rpt, rmm, rinch ) );
        frame->setBTop( KWUnit( tpt, tmm, tinch ) );
        frame->setBBottom( KWUnit( bpt, bmm, binch ) );
        frame->setFrameBehaviour( autoCreateNewValue );
        frame->setSheetSide( sheetSide );
        frame->setNewFrameBehaviour( newFrameBehaviour);
        addFrame( frame ); // this will call updateFrames, and will update m_availableHeight
        doc->progressItemLoaded();
    }
}

/*================================================================*/
int KWFrameSet::getNext( QRect _rect )
{
    for ( unsigned int i = 0; i < frames.count(); i++ ) {
        if ( frames.at( i )->intersects( _rect ) )
            return i;
    }

    return -1;
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
bool KWFrameSet::isVisible() const
{
    return ( visible &&
             (!isAHeader( frameInfo ) || doc->isHeaderVisible()) &&
             (!isAFooter( frameInfo ) || doc->isFooterVisible()) &&
             !isAWrongHeader( frameInfo, doc->getHeaderType() ) &&
             !isAWrongFooter( frameInfo, doc->getFooterType() ) );
}


// This determines where to clip the painter to draw the contents of a given frame
// It clips to the frame, and clips out any "on top" frame.
QRegion KWFrameSet::frameClipRegion( QPainter * painter, KWFrame *frame, const QRect & crect )
{
    QRect rc = painter->xForm( *frame );
    rc &= painter->xForm( crect ); // intersect
    //kdDebug() << "KWTextFrameSet::frameClipRegion frame=" << DEBUGRECT(*frame) << " clip region rect=" << DEBUGRECT(rc) << " rc.isEmpty()=" << rc.isEmpty() << endl;
    if ( !rc.isEmpty() )
    {
        QRegion reg( rc );
        QListIterator<KWFrame> fIt( m_framesOnTop );
        for ( ; fIt.current() ; ++fIt )
        {
            QRect r = painter->xForm( *fIt.current() );
            r = QRect( r.x() - 1, r.y() - 1,  // ### plan for a one-pixel border. Maybe we should use the real border width.
                       r.width() + 2, r.height() + 2 );
            //kdDebug() << "frameClipRegion subtract rect "<< DEBUGRECT(r) << endl;
            reg -= r; // subtract
        }
        return reg;
    } else return QRegion();
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
    KWImageCollection *collection = doc->imageCollection();

    m_image = collection->findImage( _filename );
    if ( !m_image.isNull() )
    {
        QImage img( _filename );
        if ( !img.isNull() )
            m_image = collection->insertImage( _filename, img );
    }

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
    QDomElement framesetElem = parentElem.ownerDocument().createElement( "FRAMESET" );
    parentElem.appendChild( framesetElem );

    framesetElem.setAttribute( "frameType", static_cast<int>( getFrameType() ) );
    framesetElem.setAttribute( "frameInfo", static_cast<int>( frameInfo ) );

    KWFrameSet::save( framesetElem ); // Save all frames

    QDomElement imageElem = parentElem.ownerDocument().createElement( "IMAGE" );
    framesetElem.appendChild( imageElem );
    QDomElement elem = parentElem.ownerDocument().createElement( "FILENAME" );
    imageElem.appendChild( elem );
    elem.setAttribute( "value", correctQString( m_image.key() ) );
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
            QString filename = correctQString( filenameElement.attribute( "value" ) );
            doc->addImageRequest( filename, this );
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
                                      QColorGroup &, bool /*onlyChanged*/ )
{
    QRect r = *frames.first();

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
    //_enableDrawing = true;
    frame = 0L;
}

/*================================================================*/
KWPartFrameSet::~KWPartFrameSet()
{
}

/*================================================================*/
QPicture *KWPartFrameSet::getPicture()
{
    QPainter p( &pic );
    //child->transform( p );

    if ( child && child->document() && !frames.isEmpty() )
        child->document()->paintEverything( p, QRect( 0, 0,
                                            frames.first()->width(), frames.first()->height() ),
                                            TRUE, 0 );

    return &pic;
}

void KWPartFrameSet::drawContents( QPainter * painter, const QRect & crect,
                                   QColorGroup &, bool onlyChanged )
{
    if (!onlyChanged)
    {
        if ( !child || !child->document() || frames.isEmpty() )
        {
            //kdDebug() << "KWPartFrameSet::drawContents " << this << " aborting. child=" << child << " child->document()=" << child->document() << " frames.count()=" << frames.count() << endl;
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
            painter->setViewport( frame->x(), frame->y(), r.width(), r.height() );
            // painter->translate( frame->x(), frame->y() ); // messes up the clip regions
            QRect rframe( 0, 0, frames.first()->width(), frames.first()->height() );
            child->document()->paintEverything( *painter, rframe, true, 0 );
            painter->setViewport( r );
            painter->restore();
        } //else kdDebug() << "KWPartFrameSet::drawContents " << this << " no intersection" << endl;
    }
}

/*================================================================*/
void KWPartFrameSet::activate( QWidget *_widget )
{
    updateFrames();
    KWView *view = (KWView*)_widget;
    KoDocument* part = child->document();
    if ( !part )
        return;
    //kdDebug() << "Child activated. part="<<part<<" child="<<child<<endl;
    view->partManager()->addPart( part, false );
    view->partManager()->setActivePart(  part, view );
}

/*================================================================*/
void KWPartFrameSet::deactivate()
{
}

/*================================================================*/
void KWPartFrameSet::updateFrames()
{
    child->setGeometry( QRect( frames.at( 0 )->x(), frames.at( 0 )->y(),
                               frames.at( 0 )->width(), frames.at( 0 )->height() ) );
    KWFrameSet::updateFrames();
}

/*================================================================*/
void KWPartFrameSet::save( QDomElement &parentElem )
{
    KWFrameSet::save( parentElem );
}

/*================================================================*/
void KWPartFrameSet::load( QDomElement &attributes )
{
    KWFrameSet::load( attributes );
}

KWFrameSetEdit * KWPartFrameSet::createFrameSetEdit( KWCanvas * canvas )
{
    return new KWPartFrameSetEdit( this, canvas );
}

KWPartFrameSetEdit::~KWPartFrameSetEdit()
{
    partFrameSet()->deactivate();
}

void KWPartFrameSetEdit::mousePressEvent( QMouseEvent * )
{
    partFrameSet()->activate( m_canvas->gui()->getView() );
}

void KWPartFrameSetEdit::mouseDoubleClickEvent( QMouseEvent * )
{
    /// ## Pretty useless since single-click does it now...
    partFrameSet()->activate( m_canvas->gui()->getView() );
}

#include <kformulaedit.h>

/******************************************************************/
/* Class: KWFormulaFrameSet                                       */
/******************************************************************/

/*================================================================*/
KWFormulaFrameSet::KWFormulaFrameSet( KWDocument *_doc, QWidget *parent )
    : KWFrameSet( _doc ), pic( 0 ), font( "times", 12 ), color( Qt::black )

{
    formulaEdit = new KFormulaEdit( ( (QScrollView*)parent )->viewport() );
    ( (QScrollView*)parent )->addChild( formulaEdit );
    formulaEdit->setFont( font );
    if ( pic )
        delete pic;
    pic = new QPicture;
    QPainter p;
    p.begin( pic );
    formulaEdit->getFormula()->setFont( font );
    formulaEdit->getFormula()->setBackColor( Qt::white );
    formulaEdit->getFormula()->setForeColor( color );
    formulaEdit->getFormula()->redraw( p );
    p.end();
    formulaEdit->hide();
}

/*================================================================*/
KWFormulaFrameSet::KWFormulaFrameSet( KWDocument *_doc )
    : KWFrameSet( _doc ), formulaEdit( 0 ), pic( 0 )
{
}

/*================================================================*/
KWFormulaFrameSet::~KWFormulaFrameSet()
{
    if ( pic )
        delete pic;
    if ( formulaEdit )
        delete formulaEdit;
}

/*================================================================*/
QPicture *KWFormulaFrameSet::getPicture()
{
    return pic;
}

/*================================================================*/
void KWFormulaFrameSet::setFormat( const QFont &f, const QColor &c )
{
    font = f;
    color = c;
    if ( formulaEdit && formulaEdit->isVisible() ) {
        formulaEdit->getFormula()->setFont( font );
        formulaEdit->getFormula()->setBackColor( frames.at( 0 )->getBackgroundColor().color() );
        formulaEdit->getFormula()->setForeColor( color );
        formulaEdit->getFormula()->makeDirty();
        formulaEdit->redraw( TRUE );
    }
    updateFrames();
}

/*================================================================*/
void KWFormulaFrameSet::activate( QWidget *_widget )
{
    if ( formulaEdit->parent() != ( (QScrollView*)_widget )->viewport() )
        formulaEdit->reparent( ( (QScrollView*)_widget )->viewport(), 0, QPoint( 0, 0 ), FALSE );

    formulaEdit->getFormula()->setBackColor( frames.at( 0 )->getBackgroundColor().color() );
    formulaEdit->redraw( TRUE );
    formulaEdit->setBackgroundColor( frames.at( 0 )->getBackgroundColor().color() );
    formulaEdit->resize( frames.at( 0 )->width(), frames.at( 0 )->height() );
    ( (QScrollView*)_widget )->moveChild( formulaEdit, frames.at( 0 )->x(), frames.at( 0 )->y() );
    formulaEdit->show();
    ( (QScrollView*)_widget )->viewport()->setFocusProxy( formulaEdit );
    _widget->setFocusProxy( formulaEdit );
    formulaEdit->setFocus();
}

/*================================================================*/
void KWFormulaFrameSet::deactivate()
{
    formulaEdit->hide();

    if ( pic )
        delete pic;
    pic = new QPicture;
    QPainter p;
    p.begin( pic );
    formulaEdit->getFormula()->redraw( p );
    p.end();
}

/*================================================================*/
void KWFormulaFrameSet::insertChar( int c )
{
    if ( formulaEdit )
        formulaEdit->insertChar( c );
}

/*================================================================*/
void KWFormulaFrameSet::create( QWidget *parent )
{
    if ( formulaEdit ) {
        updateFrames();
        return;
    }

    formulaEdit = new KFormulaEdit( ( (QScrollView*)parent )->viewport() );
    ( (QScrollView*)parent )->addChild( formulaEdit );
    formulaEdit->getFormula()->setFont( font );
    formulaEdit->getFormula()->setBackColor( frames.at( 0 )->getBackgroundColor().color() );
    formulaEdit->getFormula()->setForeColor( color );
    formulaEdit->hide();
    formulaEdit->setText( text );
    updateFrames();
}

/*================================================================*/
void KWFormulaFrameSet::updateFrames()
{
    KWFrameSet::updateFrames();
    if ( !formulaEdit )
        return;
    formulaEdit->setFont( font );
    formulaEdit->resize( frames.at( 0 )->width(), frames.at( 0 )->height() );
    formulaEdit->getFormula()->setPos( formulaEdit->width() / 2, formulaEdit->height() / 2 );
    if ( pic )
        delete pic;
    pic = new QPicture;
    QPainter p;
    p.begin( pic );
    formulaEdit->getFormula()->redraw( p );
    p.end();
}

/*================================================================*/
void KWFormulaFrameSet::save( QDomElement& /*parentElem*/ )
{
#if 0
    out << otag << "<FRAMESET frameType=\"" << static_cast<int>( getFrameType() ) << "\" frameInfo=\""
        << static_cast<int>( frameInfo ) << "\">" << endl;

    KWFrameSet::save( out );

    out << otag << "<FORMULA>" << endl;
    out << formulaEdit->text() << endl;
    out << etag << "</FORMULA>" << endl;

    out << otag << "<FORMAT>" << endl;
    getFormat()->save( out );
    out << etag << "</FORMAT>" << endl;

    out << etag << "</FRAMESET>" << endl;
#endif
}

/*================================================================*/
void KWFormulaFrameSet::load( QDomElement &/*attributes*/ )
{
#if 0
    KWFrameSet::load( attributes );

    QString tag;
    QString name;
    //QString tmp;

    while ( parser.open( QString::null, tag ) ) {
        parser.parseTag( tag, name, lst );

        if ( name == "FORMULA" ) {
            parser.parseTag( tag, name, lst );
            parser.readText( text );
            text = text.stripWhiteSpace();
        } if ( name == "FORMAT" ) {
            KWFormat f( doc );
            f.load( parser, lst, doc );
            font = QFont( f.getUserFont()->getFontName() );
            font.setPointSize( f.ptFontSize() );
            font.setWeight( f.getWeight() );
            font.setUnderline( f.getUnderline() );
            font.setItalic( f.getItalic() );
            color = f.getColor();
        }
        } else
            kdError(32001) << "Unknown tag '" << name << "' in FRAMESET" << endl;

        if ( !parser.close( tag ) ) {
            kdError(32001) << "Closing " << tag << endl;
            return;
        }
    }
#endif
}



/*================================================================*/
bool isAHeader( FrameInfo fi )
{
    return ( fi == FI_FIRST_HEADER || fi == FI_EVEN_HEADER || fi == FI_ODD_HEADER );
}

/*================================================================*/
bool isAFooter( FrameInfo fi )
{
    return ( fi == FI_FIRST_FOOTER || fi == FI_EVEN_FOOTER || fi == FI_ODD_FOOTER );
}

/*================================================================*/
bool isAWrongHeader( FrameInfo fi, KoHFType t )
{
    switch ( fi ) {
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

    return false;
}

/*================================================================*/
bool isAWrongFooter( FrameInfo fi, KoHFType t )
{
    switch ( fi ) {
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

    return false;
}

#include "kwframe.moc"
