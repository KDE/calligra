/* This file is part of the KDE project
   Copyright (C) 1998, 1999 Reginald Stadlbauer <reggie@kde.org>

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

#include "kword_doc.h"
#include "kword_view.h"
#include "kword_page.h"
#include "kword_frame.h"
#include "image.h"
#include "parag.h"
#include "defs.h"
#include "kword_utils.h"
#include "format.h"
#include "font.h"

#include <kdebug.h>
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
#include <kdebug.h>

#include <koFrame.h>

/******************************************************************/
/* Class: KWFrame                                                 */
/******************************************************************/

/*================================================================*/
KWFrame::KWFrame()
    : QRect(), runAroundGap( 1.0 ), intersections(), bleft(), bright(), btop(), bbottom(), handles()
{
    runAround = RA_NO;
    intersections.setAutoDelete( true );
    handles.setAutoDelete( true );
    selected = false;
    runAroundGap = 1;
    mostRight = false;
    emptyRegionDirty = TRUE;

    backgroundColor = QBrush( Qt::white );
    brd_left.color = getBackgroundColor().color();
    brd_left.style = KWParagLayout::SOLID;
    brd_left.ptWidth = 1;
    brd_right.color = getBackgroundColor().color();
    brd_right.style = KWParagLayout::SOLID;
    brd_right.ptWidth = 1;
    brd_top.color = getBackgroundColor().color();
    brd_top.style = KWParagLayout::SOLID;
    brd_top.ptWidth = 1;
    brd_bottom.color = getBackgroundColor().color();
    brd_bottom.style = KWParagLayout::SOLID;
    brd_bottom.ptWidth = 1;

    frameBehaviour= AutoExtendFrame;
    newFrameBehaviour = Reconnect;
    sheetSide = AnySide;
}

/*================================================================*/
KWFrame::KWFrame(KWFrameSet *fs, const QPoint &topleft, const QPoint &bottomright )
    : QRect( topleft, bottomright ), runAroundGap( 1.0 ), intersections(), bleft(), bright(), btop(), bbottom(), handles()
{
    frameSet = fs;

    runAround = RA_NO;
    intersections.setAutoDelete( true );
    handles.setAutoDelete( true );
    selected = false;
    mostRight = false;
    emptyRegionDirty = TRUE;

    backgroundColor = QBrush( Qt::white );
    brd_left.color = getBackgroundColor().color();
    brd_left.style = KWParagLayout::SOLID;
    brd_left.ptWidth = 1;
    brd_right.color = getBackgroundColor().color();
    brd_right.style = KWParagLayout::SOLID;
    brd_right.ptWidth = 1;
    brd_top.color = getBackgroundColor().color();
    brd_top.style = KWParagLayout::SOLID;
    brd_top.ptWidth = 1;
    brd_bottom.color = getBackgroundColor().color();
    brd_bottom.style = KWParagLayout::SOLID;
    brd_bottom.ptWidth = 1;

    frameBehaviour=AutoCreateNewFrame;
    newFrameBehaviour = Reconnect;
    sheetSide = AnySide;
}

/*================================================================*/
KWFrame::KWFrame( KWFrameSet *fs,const QPoint &topleft, const QSize &size )
    : QRect( topleft, size ), runAroundGap( 1.0 ), intersections(), bleft(), bright(), btop(), bbottom(), handles()
{
    frameSet = fs;

    runAround = RA_NO;
    intersections.setAutoDelete( true );
    handles.setAutoDelete( true );
    selected = false;
    mostRight = false;
    emptyRegionDirty = TRUE;

    backgroundColor = QBrush( Qt::white );
    brd_left.color = getBackgroundColor().color();
    brd_left.style = KWParagLayout::SOLID;
    brd_left.ptWidth = 1;
    brd_right.color = getBackgroundColor().color();
    brd_right.style = KWParagLayout::SOLID;
    brd_right.ptWidth = 1;
    brd_top.color = getBackgroundColor().color();
    brd_top.style = KWParagLayout::SOLID;
    brd_top.ptWidth = 1;
    brd_bottom.color = getBackgroundColor().color();
    brd_bottom.style = KWParagLayout::SOLID;
    brd_bottom.ptWidth = 1;

    frameBehaviour=AutoCreateNewFrame;
    newFrameBehaviour = Reconnect;
    sheetSide = AnySide;
}

/*================================================================*/
KWFrame::KWFrame(KWFrameSet *fs, int left, int top, int width, int height )
    : QRect( left, top, width, height ), runAroundGap( 1.0 ), intersections(), bleft(), bright(), btop(), bbottom(), handles()
{
    frameSet = fs;

    runAround = RA_NO;
    intersections.setAutoDelete( true );
    handles.setAutoDelete( true );
    selected = false;
    mostRight = false;
    emptyRegionDirty = TRUE;

    backgroundColor = QBrush( Qt::white );
    brd_left.color = getBackgroundColor().color();
    brd_left.style = KWParagLayout::SOLID;
    brd_left.ptWidth = 1;
    brd_right.color = getBackgroundColor().color();
    brd_right.style = KWParagLayout::SOLID;
    brd_right.ptWidth = 1;
    brd_top.color = getBackgroundColor().color();
    brd_top.style = KWParagLayout::SOLID;
    brd_top.ptWidth = 1;
    brd_bottom.color = getBackgroundColor().color();
    brd_bottom.style = KWParagLayout::SOLID;
    brd_bottom.ptWidth = 1;

    newFrameBehaviour = Reconnect;
    frameBehaviour=AutoCreateNewFrame;
    sheetSide = AnySide;
}

/*================================================================*/
KWFrame::KWFrame(KWFrameSet *fs, int left, int top, int width, int height, RunAround _ra, KWUnit _gap )
    : QRect( left, top, width, height ), runAroundGap( _gap ), intersections(), bleft(), bright(), btop(), bbottom(), handles()
{
    frameSet = fs;

    runAround = _ra;
    intersections.setAutoDelete( true );
    handles.setAutoDelete( true );
    selected = false;
    mostRight = false;
    emptyRegionDirty = TRUE;

    backgroundColor = QBrush( Qt::white );
    brd_left.color = getBackgroundColor().color();
    brd_left.style = KWParagLayout::SOLID;
    brd_left.ptWidth = 1;
    brd_right.color = getBackgroundColor().color();
    brd_right.style = KWParagLayout::SOLID;
    brd_right.ptWidth = 1;
    brd_top.color = getBackgroundColor().color();
    brd_top.style = KWParagLayout::SOLID;
    brd_top.ptWidth = 1;
    brd_bottom.color = getBackgroundColor().color();
    brd_bottom.style = KWParagLayout::SOLID;
    brd_bottom.ptWidth = 1;

    frameBehaviour=AutoCreateNewFrame;
    newFrameBehaviour = Reconnect;
    sheetSide = AnySide;
}

/*================================================================*/
KWFrame::KWFrame(KWFrameSet *fs, const QRect &_rect )
    : QRect( _rect ), runAroundGap( 1.0 ), intersections(), bleft(), bright(), btop(), bbottom(), handles()
{
    frameSet = fs;

    runAround = RA_NO;
    intersections.setAutoDelete( true );
    handles.setAutoDelete( true );
    selected = false;
    mostRight = false;
    emptyRegionDirty = TRUE;

    backgroundColor = QBrush( Qt::white );
    brd_left.color = getBackgroundColor().color();
    brd_left.style = KWParagLayout::SOLID;
    brd_left.ptWidth = 1;
    brd_right.color = getBackgroundColor().color();
    brd_right.style = KWParagLayout::SOLID;
    brd_right.ptWidth = 1;
    brd_top.color = getBackgroundColor().color();
    brd_top.style = KWParagLayout::SOLID;
    brd_top.ptWidth = 1;
    brd_bottom.color = getBackgroundColor().color();
    brd_bottom.style = KWParagLayout::SOLID;
    brd_bottom.ptWidth = 1;

    frameBehaviour=AutoCreateNewFrame;
    newFrameBehaviour = Reconnect;
    sheetSide = AnySide;
}

/*================================================================*/
KWFrame::~KWFrame()
{
}

/*================================================================*/
void KWFrame::addIntersect( QRect &_r )
{
    emptyRegionDirty = TRUE;

    intersections.append( new QRect( _r.x(), _r.y(), _r.width(), _r.height() ) );
}

/*================================================================*/
int KWFrame::getLeftIndent( int _y, int _h )
{
    if ( runAround == RA_NO || intersections.isEmpty() )
        return 0;

    if ( emptyRegionDirty )
        getEmptyRegion();

    int _left = 0;
    QRect line( x(), _y, width(), _h );
    QRegion reg = emptyRegion.intersect( line );
    _left = reg.boundingRect().left() - x();
    for ( unsigned int i = 0; i < reg.rects().size(); ++i )
        _left = QMAX( _left, reg.rects()[ i ].left() - x() );
    if ( _left > 0 )
        _left += static_cast<int>(runAroundGap.pt());
    if ( _left > 0 && runAround == RA_SKIP )
        _left = width();

    return QMAX( 0, _left );
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
    } else {
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
/* Insert 8 resize handles which will be drawn in param page      */
/*================================================================*/
void KWFrame::createResizeHandlesForPage(KWPage *page) {
    removeResizeHandlesForPage(page);

    for (unsigned int i=0; i < 8; i++) {
        handles.append(new KWResizeHandle( page, (KWResizeHandle::Direction)i, this ));
    }
}

/*================================================================*/
/* remove all the resize handles which will be drawn in param page*/
/*================================================================*/
void KWFrame::removeResizeHandlesForPage(KWPage *page) {
    for( unsigned int i=0; i < handles.count(); i++) {
        if(handles.at ( i )->getPage() == page) {
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

/******************************************************************/
/* Class: KWFrameSet                                              */
/******************************************************************/

/*================================================================*/
KWFrameSet::KWFrameSet( KWordDocument *_doc )
    : frames(), removeableHeader( false ), visible( true )
{
    doc = _doc;
    frames.setAutoDelete( true );
    frameInfo = FI_BODY;
    current = 0;
    grpMgr = 0L;
}


/*================================================================*/
void KWFrameSet::addFrame( KWFrame _frame )
{
    addFrame(_frame.getCopy());
}

/*================================================================*/
void KWFrameSet::addFrame( KWFrame *_frame )
{
    if ( frames.findRef( _frame ) != -1 )
        return;

    frames.append( _frame );
    _frame->setFrameSet(this);
    if ( frames.count() == 1 ) init();
    update();
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

    KWFrame *f;

    // This check is totally irrelevant since we allready check
    // for duplicate occurences on addFrame (TZ)
    bool del = true;
    int i = 0;
    for ( f = frames.first(); f != 0; f = frames.next(), i++ ) {
        if ( f == frm && i != _num ) {
            del = false;
            break;
        }
    }

    frm->setFrameSet(0L);
    if ( !del || !remove )
        frames.take( _num );
    else
        frames.remove( _num );

    update();
}

/*================================================================*/
int KWFrameSet::getFrame( int _x, int _y )
{
    for ( unsigned int i = 0; i < getNumFrames(); i++ ) {
        if ( getFrame( i )->contains( QPoint( _x, _y ) ) ) return i;
    }
    return -1;
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
    int frame = getFrame( mx, my );

    if ( frame == -1 )
        return Qt::arrowCursor;

    if ( !getFrame( frame )->isSelected() && !grpMgr )
        return Qt::arrowCursor;

    return getFrame( frame )->getMouseCursor( mx, my, grpMgr ? true : false );
}

/*================================================================*/
void KWFrameSet::save( QTextStream&out )
{
    KWFrame *frame;

    for ( unsigned int i = 0; i < frames.count(); i++ ) {
        frame = getFrame( i );
        if(getGroupManager() && getGroupManager()->isAnchored()) {
            // set the frame cooridinates to the offset.
            out << indent << "<FRAME left=\"" << frame->left() 
                << "\" top=\"" << frame->top() - getGroupManager()->getOrigin().y()
                << "\" right=\"" << frame->right()
                << "\" bottom=\"" << frame->bottom() - getGroupManager()->getOrigin().y() << "\" ";
        } else {
            out << indent << "<FRAME left=\"" << frame->left() << "\" top=\"" << frame->top()
                << "\" right=\"" << frame->right() << "\" bottom=\"" << frame->bottom() << "\" ";
        }
        if(frame->getRunAround()!=RA_NO) {
            out << "runaround=\"" << static_cast<int>( frame->getRunAround() ) << "\" ";
        }
        if(frame->getRunAroundGap().mm()!=1) {
            out << "runaGapPT=\"" << frame->getRunAroundGap().pt()
              << "\" runaGapMM=\"" << frame->getRunAroundGap().mm()
              << "\" runaGapINCH=\"" << frame->getRunAroundGap().inch() << "\" ";
        }
        if(frame->getLeftBorder().ptWidth!=1) {
            out << "lWidth=\"" << frame->getLeftBorder().ptWidth << "\" ";
        }
        if(frame->getLeftBorder().color != Qt::white) {
            out << "lRed=\"" << frame->getLeftBorder().color.red() << "\" lGreen=\"" 
                << frame->getLeftBorder().color.green() << "\" lBlue=\"" 
                << frame->getLeftBorder().color.blue() << "\" ";
        }
        if(frame->getLeftBorder().style != KWParagLayout::SOLID) {
            out << "lStyle=\"" <<  static_cast<int>( frame->getLeftBorder().style ) << "\" ";
        }

        if(frame->getRightBorder().ptWidth!=1) {
            out << "rWidth=\"" << frame->getRightBorder().ptWidth << "\" ";
        }
        if(frame->getRightBorder().color != Qt::white) {
            out << "rRed=\"" << frame->getRightBorder().color.red() << "\" rGreen=\"" 
                << frame->getRightBorder().color.green() << "\" rBlue=\"" 
                << frame->getRightBorder().color.blue() << "\" ";
        }
        if(frame->getRightBorder().style != KWParagLayout::SOLID) {
            out << "rStyle=\"" <<  static_cast<int>( frame->getRightBorder().style ) << "\" ";
        }

        if(frame->getTopBorder().ptWidth!=1) {
            out << "tWidth=\"" << frame->getTopBorder().ptWidth << "\" ";
        }
        if(frame->getTopBorder().color != Qt::white) {
            out << "tRed=\"" << frame->getTopBorder().color.red() << "\" tGreen=\"" 
                << frame->getTopBorder().color.green() << "\" tBlue=\"" 
                << frame->getTopBorder().color.blue() << "\" ";
        }
        if(frame->getTopBorder().style != KWParagLayout::SOLID) {
            out << "tStyle=\"" <<  static_cast<int>( frame->getTopBorder().style ) << "\" ";
        }

        if(frame->getBottomBorder().ptWidth!=1) {
            out << "bWidth=\"" << frame->getBottomBorder().ptWidth << "\" ";
        }
        if(frame->getBottomBorder().color != Qt::white) {
            out << "bRed=\"" << frame->getBottomBorder().color.red() << "\" bGreen=\"" 
                << frame->getBottomBorder().color.green() << "\" bBlue=\"" 
                << frame->getBottomBorder().color.blue() << "\" ";
        }
        if(frame->getBottomBorder().style != KWParagLayout::SOLID) {
            out << "bStyle=\"" <<  static_cast<int>( frame->getBottomBorder().style ) << "\" ";
        }
        if(frame->getBackgroundColor().color() != Qt::white) {
            out << "bkRed=\"" << frame->getBackgroundColor().color().red()
                << "\" bkGreen=\"" << frame->getBackgroundColor().color().green()
                << "\" bkBlue=\"" << frame->getBackgroundColor().color().blue() << "\" ";
        }
        if(frame->getBLeft().pt() != 0) {
            out << "bleftpt=\"" << frame->getBLeft().pt() << "\" bleftmm=\"" << frame->getBLeft().mm()
                << "\" bleftinch=\"" << frame->getBLeft().inch() << "\" ";
        }
        if(frame->getBRight().pt()!=0) {
            out << "brightpt=\"" << frame->getBRight().pt() << "\" brightmm=\"" << frame->getBRight().mm()
                << "\" brightinch=\"" << frame->getBRight().inch() << "\" ";
        }
        if(frame->getBTop().pt()!=0) {
            out << "btoppt=\"" << frame->getBTop().pt() << "\" btopmm=\"" << frame->getBTop().mm()
                << "\" btopinch=\"" << frame->getBTop().inch() << "\" ";
        }
        if(frame->getBBottom().pt()!=0) {
            out << "bbottompt=\"" << frame->getBBottom().pt() << "\" bbottommm=\"" << frame->getBBottom().mm()
                << "\" bbottominch=\"" << frame->getBBottom().inch() << "\" ";
        }
        if(frame->getFrameBehaviour()!=AutoCreateNewFrame) {
            out << "autoCreateNewFrame=\"" << static_cast<int>( frame->getFrameBehaviour()) << "\" ";
        }
        if(frame->getNewFrameBehaviour()!=Reconnect) {
            out << "newFrameBehaviour=\"" << static_cast<int>( frame->getNewFrameBehaviour()) << "\" ";
        }
        if(frame->getSheetSide()!= AnySide) {
            out << "sheetSide=\"" << static_cast<int>( frame->getSheetSide()) << "\" ";
        }
        out << "/>" << endl;
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


/******************************************************************/
/* Class: KWTextFrameSet                                          */
/******************************************************************/

/*================================================================*/
void KWTextFrameSet::init()
{
    parags = 0L;

    parags = new KWParag( this, doc, 0L, 0L, doc->getDefaultParagLayout() );
    format = new KWFormat( doc );
    format->setDefaults( doc );

    updateCounters();
}

/*================================================================*/
void KWTextFrameSet::assign( KWTextFrameSet *fs )
{
    if ( parags )
        delete parags;

    //parags = fs->getFirstParag();

    parags = new KWParag( *fs->getFirstParag() );
    parags->setFrameSet( this );
    parags->setPrev( 0L );
    parags->setDocument( doc );

    KWParag *p1 = fs->getFirstParag()->getNext(), *p2 = parags, *tmp;
    while ( p1 ) {
        tmp = p2;
        p2 = new KWParag( *p1 );
        //*p2 = *p1;
        tmp->setNext( p2 );
        p2->setPrev( tmp );
        p2->setFrameSet( this );
        p2->setDocument( doc );
        tmp = p2;

        p1 = p1->getNext();
    }

    p2->setNext( 0L );
    getFrame( 0 )->setBackgroundColor( fs->getFrame( 0 )->getBackgroundColor() );
    getFrame( 0 )->setLeftBorder( fs->getFrame( 0 )->getLeftBorder2() );
    getFrame( 0 )->setRightBorder( fs->getFrame( 0 )->getRightBorder2() );
    getFrame( 0 )->setTopBorder( fs->getFrame( 0 )->getTopBorder2() );
    getFrame( 0 )->setBottomBorder( fs->getFrame( 0 )->getBottomBorder2() );
    getFrame( 0 )->setBLeft( fs->getFrame( 0 )->getBLeft() );
    getFrame( 0 )->setBRight( fs->getFrame( 0 )->getBRight() );
    getFrame( 0 )->setBTop( fs->getFrame( 0 )->getBTop() );
    getFrame( 0 )->setBBottom( fs->getFrame( 0 )->getBBottom() );
}

/*================================================================*/
KWTextFrameSet::~KWTextFrameSet()
{
    KWParag *p = getLastParag();

    while ( p != parags )
    {
        p = p->getPrev();
        delete p->getNext();
        p->setNext( 0L );
    }

    delete parags;
    parags = 0L;

    delete format;
    format = 0L;

    // don't let us delete ourselves
    if(doc) doc->delFrameSet(this, false);
    doc=0L;
}

/*================================================================*/
void KWTextFrameSet::update()
{
    typedef QList<KWFrame> FrameList;
    QList<FrameList> frameList;
    frameList.setAutoDelete( true );
    frames.setAutoDelete( false );

    // sort frames of this frameset into l2 on (page, y coord, x coord)
    QRect pageRect;
    for ( unsigned int i = 0; i < static_cast<unsigned int>( doc->getPages() + 1 ); i++ ) {
        pageRect = QRect( 0, i * doc->getPTPaperHeight(), doc->getPTPaperWidth(), doc->getPTPaperHeight() );
        FrameList *l = new FrameList();
        l->setAutoDelete( false );
        for ( unsigned int j = 0; j < frames.count(); j++ ) {
            if ( frames.at( j )->intersects( pageRect ) ) {
                frames.at( j )->setPageNum( i );
                l->append( frames.at( j ) );
                frames.remove(j--);
            }
        }


        if ( !l->isEmpty() ) {
            FrameList *ll = new FrameList();
            ll->setAutoDelete( false );
            ll->append( l->first() );
            unsigned int k = 0, m = 0;
            for ( k = 1; k < l->count(); k++ ) {
                bool inserted = false;
                for ( m = 0; m < ll->count(); m++ ) {
                    if ( l->at( k )->y() < ll->at( m )->y() ) {
                        inserted = true;
                        ll->insert( m, l->at( k ) );
                        break;
                    }
                }
                if ( !inserted ) ll->append( l->at( k ) );
            }
            FrameList *l2 = new FrameList();
            l2->setAutoDelete( false );
            l2->append( ll->first() );
            for ( k = 1; k < ll->count(); k++ ) {
                bool inserted = false;
                for ( m = 0; m < l2->count(); m++ ) {
                    if ( ll->at( k )->x() < l2->at( m )->x() ) {
                        inserted = true;
                        l2->insert( m, ll->at( k ) );
                        break;
                    }
                }
                if ( !inserted ) l2->append( ll->at( k ) );
            }

            delete ll;
            delete l;
            l = l2;
        }

        if(! l->isEmpty())
            frameList.append( l );
        else
            delete l;
    }

    int rm = 0;
    for ( unsigned int n = 0; n < frameList.count(); n++ ) {
        for ( unsigned int o = 0; o < frameList.at( n )->count(); o++ ) {
            frames.append( frameList.at( n )->at( o ) );
            frames.at( frames.count() - 1 )->setMostRight( false );
            if ( frames.count() > 1 ) {
                if ( frames.at( frames.count() - 2 )->right() > frames.at( frames.count() - 1 )->right() ) {
                    frames.at( frames.count() - 2 )->setMostRight( true );
                    rm++;
                }
            }
        }
    }
    frames.setAutoDelete( true );
}

/*================================================================*/
void KWTextFrameSet::setFirstParag( KWParag *_parag )
{
    parags = _parag;
}

/*================================================================*/
KWParag* KWTextFrameSet::getFirstParag()
{
    return parags;
}

/*================================================================*/
bool KWTextFrameSet::isPTYInFrame( unsigned int _frame, unsigned int _ypos )
{
    KWFrame *frame = getFrame( _frame );
    return ( static_cast<int>( _ypos ) >= static_cast<int>( frame->top() + frame->getBTop().pt() ) &&
             static_cast<int>( _ypos ) <= static_cast<int>( frame->bottom() - frame->getBBottom().pt() ) );
}

/*================================================================*/
void KWTextFrameSet::deleteParag( KWParag *_parag )
{
    if ( _parag->getInfo() == KWParag::PI_FOOTNOTE )
        return;

    KWParag *p, *p2;

    if ( !getFirstParag()->getPrev() && !getFirstParag()->getNext() )
        return;

    if ( !_parag->getPrev() ) {
        p = _parag->getNext();
        p->setPrev( 0L );
        setFirstParag( p );
        delete _parag;
    } else {
        p = _parag->getNext();
        p2 = _parag->getPrev();
        if ( p ) p->setPrev( p2 );
        p2->setNext( p );
        delete _parag;
    }

    updateCounters();
}

/*================================================================*/
void KWTextFrameSet::joinParag( KWParag *_parag1, KWParag *_parag2 )
{
    if ( !_parag1 || !_parag2 ) return;

    if ( _parag2->getNext() ) _parag2->getNext()->setPrev( _parag1 );
    _parag1->setNext( _parag2->getNext() );

    _parag1->appendText( _parag2->getText(), _parag2->getTextLen() );

    delete _parag2;

    updateCounters();
}

/*================================================================*/
void KWTextFrameSet::insertParag( KWParag *_parag, InsertPos _pos )
{
    KWParag *_new = 0L, *_prev = 0L, *_next = 0L;

    if ( _parag ) {
        _prev = _parag->getPrev();
        _next = _parag->getNext();
    }

    switch ( _pos ) {
    case I_AFTER: {
        _new = new KWParag( this, doc, _parag, _next,
                            doc->findParagLayout( _parag->getParagLayout()->getFollowingParagLayout() ) );
        if ( _new->getParagLayout()->getName() == _parag->getParagLayout()->getName() )
            _new->setParagLayout( _parag->getParagLayout() );
        if ( _next ) _next->setPrev( _new );
    } break;
    case I_BEFORE: {
        _new = new KWParag( this, doc, _prev, _parag, _parag->getParagLayout() );
        if ( _parag ) _parag->setPrev( _new );
        if ( !_prev ) setFirstParag( _new );
    } break;
    }

    updateCounters();
}

/*================================================================*/
void KWTextFrameSet::splitParag( KWParag *_parag, unsigned int _pos )
{
    KWParag *_new = 0, *_next = 0;

    unsigned int len = _parag->getTextLen() - _pos;
    KWChar* _string = _parag->getKWString()->split( _pos );
    if ( _parag )
        _next = _parag->getNext();

    _new = new KWParag( this, doc, _parag, _next, _parag->getParagLayout() );
    if ( _next ) {
        _next->setPrev( _new );
        _new->setNext( _next );
    } else {
        _new->setNext( 0 );
    }

    _new->appendText( _string, len );
    for (unsigned int i = 0; i < len; i++)
        if (_string[i].attrib) delete _string[i].attrib;

    free (_string);

    updateCounters();
}

/*================================================================*/
void KWTextFrameSet::save( QTextStream&out )
{
    QString grp = "";
    if ( grpMgr ) {
        grp = "\" grpMgr=\"";
        grp += correctQString( grpMgr->getName() );

        unsigned int _row = 0, _col = 0;
        _row=grpMgr->getCell( this )->row;
        _col=grpMgr->getCell( this )->col;
        KWGroupManager::Cell *cell = grpMgr->getCell( _row, _col );
        QString tmp = "";
        tmp.sprintf( "\" row=\"%d\" col=\"%d\" rows=\"%d\" cols=\"%d", _row, _col, cell->rows, cell->cols  );
        grp += tmp.copy();
    }

    out << otag << "<FRAMESET frameType=\"" << static_cast<int>( getFrameType() )
        << "\" frameInfo=\""
        << static_cast<int>( frameInfo ) << correctQString( grp ) << "\" removable=\""
        << static_cast<int>( removeableHeader )
        << "\" visible=\"" << static_cast<int>( visible ) << "\" name=\"" << correctQString( name )
        << "\">" << endl;

    KWFrameSet::save( out );

    KWParag *parag = getFirstParag();
    while ( parag ) {
        out << otag << "<PARAGRAPH>" << endl;
        parag->save( out );
        parag = parag->getNext();
        out << etag << "</PARAGRAPH>" << endl;
    }

    out << etag << "</FRAMESET>" << endl;
}

/*================================================================*/
void KWTextFrameSet::load( KOMLParser& parser, QValueList<KOMLAttrib>& lst )
{
    init();

    QString tag;
    QString name;
    KWParag *last = 0L;

    while ( parser.open( QString::null, tag ) ) {
        parser.parseTag( tag, name, lst );

        // paragraph
        if ( name == "PARAGRAPH" ) {
            parser.parseTag( tag, name, lst );
            //QValueList<KOMLAttrib>::ConstIterator it = lst.begin();
            //for( ; it != lst.end(); ++it ) {
            //}

            if ( !last ) {
                delete parags;
                parags = new KWParag( this, doc, 0L, 0L, doc->getDefaultParagLayout() );
                if ( doc->getNumFrameSets() == 0 ) {
                    if(format)
                        delete format;
                    format = new KWFormat( doc );
                    format->setDefaults( doc );
                    parags->setFormat( 0, 1, *format );
                }
                parags->load( parser, lst );
                last = parags;
            } else {
                last = new KWParag( this, doc, last, 0L, doc->getDefaultParagLayout() );
                last->load( parser, lst );
            }
        } else if ( name == "FRAME" ) {
            KWFrame rect;
            NewFrameBehaviour newFrameBehaviour = Reconnect;
            FrameBehaviour autoCreateNewValue = AutoCreateNewFrame;
            SheetSide sheetSide = AnySide;
            KWParagLayout::Border l, r, t, b;
            double lmm = 0.0, linch = 0.0, rmm = 0.0, rinch = 0.0, tmm = 0.0, tinch = 0.0, bmm = 0.0, binch = 0.0, ramm = 1.0, rainch = 0.0393701;
            double lpt = 0.0, rpt = 0.0, tpt = 0.0, bpt = 0.0, rapt = 2.0;

            l.color = Qt::white;
            l.style = KWParagLayout::SOLID;
            l.ptWidth = 1;
            r.color = Qt::white;
            r.style = KWParagLayout::SOLID;
            r.ptWidth = 1;
            t.color = Qt::white;
            t.style = KWParagLayout::SOLID;
            t.ptWidth = 1;
            b.color = Qt::white;
            b.style = KWParagLayout::SOLID;
            b.ptWidth = 1;
            QColor c( Qt::white );

            parser.parseTag( tag, name, lst );
            QValueList<KOMLAttrib>::ConstIterator it = lst.begin();
            for( ; it != lst.end(); ++it ) {
                if ( ( *it ).m_strName == "left" )
                    rect.setLeft( ( *it ).m_strValue.toInt() );
                else if ( ( *it ).m_strName == "top" )
                    rect.setTop( ( *it ).m_strValue.toInt() );
                else if ( ( *it ).m_strName == "right" )
                    rect.setRight( ( *it ).m_strValue.toInt() );
                else if ( ( *it ).m_strName == "bottom" )
                    rect.setBottom( ( *it ).m_strValue.toInt() );
                else if ( ( *it ).m_strName == "runaround" )
                    rect.setRunAround( static_cast<RunAround>( ( *it ).m_strValue.toInt() ) );
                else if ( ( *it ).m_strName == "runaroundGap" )
                    rect.setRunAroundGap( KWUnit( ( *it ).m_strValue.toDouble() ) );
                else if ( ( *it ).m_strName == "runaGapPT" )
                    rapt = ( *it ).m_strValue.toDouble();
                else if ( ( *it ).m_strName == "runaGapMM" )
                    ramm = ( *it ).m_strValue.toDouble();
                else if ( ( *it ).m_strName == "runaGapINCH" )
                    rainch = ( *it ).m_strValue.toDouble();
                else if ( ( *it ).m_strName == "lWidth" )
                    l.ptWidth = ( *it ).m_strValue.toInt();
                else if ( ( *it ).m_strName == "rWidth" )
                    r.ptWidth = ( *it ).m_strValue.toInt();
                else if ( ( *it ).m_strName == "tWidth" )
                    t.ptWidth = ( *it ).m_strValue.toInt();
                else if ( ( *it ).m_strName == "bWidth" )
                    b.ptWidth = ( *it ).m_strValue.toInt();
                else if ( ( *it ).m_strName == "lRed" )
                    l.color.setRgb( ( *it ).m_strValue.toInt(), l.color.green(), l.color.blue() );
                else if ( ( *it ).m_strName == "rRed" )
                    r.color.setRgb( ( *it ).m_strValue.toInt(), r.color.green(), r.color.blue() );
                else if ( ( *it ).m_strName == "tRed" )
                    t.color.setRgb( ( *it ).m_strValue.toInt(), t.color.green(), t.color.blue() );
                else if ( ( *it ).m_strName == "bRed" )
                    b.color.setRgb( ( *it ).m_strValue.toInt(), b.color.green(), b.color.blue() );
                else if ( ( *it ).m_strName == "lGreen" )
                    l.color.setRgb( l.color.red(), ( *it ).m_strValue.toInt(), l.color.blue() );
                else if ( ( *it ).m_strName == "rGreen" )
                    r.color.setRgb( r.color.red(), ( *it ).m_strValue.toInt(), r.color.blue() );
                else if ( ( *it ).m_strName == "tGreen" )
                    t.color.setRgb( t.color.red(), ( *it ).m_strValue.toInt(), t.color.blue() );
                else if ( ( *it ).m_strName == "bGreen" )
                    b.color.setRgb( b.color.red(), ( *it ).m_strValue.toInt(), b.color.blue() );
                else if ( ( *it ).m_strName == "lBlue" )
                    l.color.setRgb( l.color.red(), l.color.green(), ( *it ).m_strValue.toInt() );
                else if ( ( *it ).m_strName == "rBlue" )
                    r.color.setRgb( r.color.red(), r.color.green(), ( *it ).m_strValue.toInt() );
                else if ( ( *it ).m_strName == "tBlue" )
                    t.color.setRgb( t.color.red(), t.color.green(), ( *it ).m_strValue.toInt() );
                else if ( ( *it ).m_strName == "bBlue" )
                    b.color.setRgb( b.color.red(), b.color.green(), ( *it ).m_strValue.toInt() );
                else if ( ( *it ).m_strName == "lStyle" )
                    l.style = static_cast<KWParagLayout::BorderStyle>( ( *it ).m_strValue.toInt() );
                else if ( ( *it ).m_strName == "rStyle" )
                    r.style = static_cast<KWParagLayout::BorderStyle>( ( *it ).m_strValue.toInt() );
                else if ( ( *it ).m_strName == "tStyle" )
                    t.style = static_cast<KWParagLayout::BorderStyle>( ( *it ).m_strValue.toInt() );
                else if ( ( *it ).m_strName == "bStyle" )
                    b.style = static_cast<KWParagLayout::BorderStyle>( ( *it ).m_strValue.toInt() );
                else if ( ( *it ).m_strName == "bkRed" )
                    c.setRgb( ( *it ).m_strValue.toInt(), c.green(), c.blue() );
                else if ( ( *it ).m_strName == "bkGreen" )
                    c.setRgb( c.red(), ( *it ).m_strValue.toInt(), c.blue() );
                else if ( ( *it ).m_strName == "bkBlue" )
                    c.setRgb( c.red(), c.green(), ( *it ).m_strValue.toInt() );
                else if ( ( *it ).m_strName == "bleftpt" )
                    lpt = ( *it ).m_strValue.toDouble();
                else if ( ( *it ).m_strName == "brightpt" )
                    rpt = ( *it ).m_strValue.toDouble();
                else if ( ( *it ).m_strName == "btoppt" )
                    tpt = ( *it ).m_strValue.toDouble();
                else if ( ( *it ).m_strName == "bbottompt" )
                    bpt = ( *it ).m_strValue.toDouble();
                else if ( ( *it ).m_strName == "bleftmm" )
                    lmm = ( *it ).m_strValue.toDouble();
                else if ( ( *it ).m_strName == "brightmm" )
                    rmm = ( *it ).m_strValue.toDouble();
                else if ( ( *it ).m_strName == "btopmm" )
                    tmm = ( *it ).m_strValue.toDouble();
                else if ( ( *it ).m_strName == "bbottommm" )
                    bmm = ( *it ).m_strValue.toDouble();
                else if ( ( *it ).m_strName == "bleftinch" )
                    linch = ( *it ).m_strValue.toDouble();
                else if ( ( *it ).m_strName == "brightinch" )
                    rinch = ( *it ).m_strValue.toDouble();
                else if ( ( *it ).m_strName == "btopinch" )
                    tinch = ( *it ).m_strValue.toDouble();
                else if ((*it).m_strName == "bbottominch")
                    binch = ( *it ).m_strValue.toDouble();
                else if ((*it).m_strName == "autoCreateNewFrame")
                    autoCreateNewValue = static_cast<FrameBehaviour>( ( *it ).m_strValue.toInt() );
                else if ((*it).m_strName == "newFrameBehaviour")
                    newFrameBehaviour = static_cast<NewFrameBehaviour>( ( *it ).m_strValue.toInt() );
                else if ((*it).m_strName == "sheetSide")
                    sheetSide = static_cast<SheetSide>( ( *it ).m_strValue.toInt() );
            }
            KWFrame *_frame = new KWFrame(this, rect.x(), rect.y(), rect.width(), rect.height(), rect.getRunAround(), rainch == -1 ? rect.getRunAroundGap() : KWUnit( rapt, ramm, rainch ) );
            _frame->setLeftBorder( l );
            _frame->setRightBorder( r );
            _frame->setTopBorder( t );
            _frame->setBottomBorder( b );
            _frame->setBackgroundColor( QBrush( c ) );
            _frame->setBLeft( KWUnit( lpt, lmm, linch ) );
            _frame->setBRight( KWUnit( rpt, rmm, rinch ) );
            _frame->setBTop( KWUnit( tpt, tmm, tinch ) );
            _frame->setBBottom( KWUnit( bpt, bmm, binch ) );
            _frame->setFrameBehaviour( autoCreateNewValue );
            _frame->setSheetSide( sheetSide );
            _frame->setNewFrameBehaviour( newFrameBehaviour);
            frames.append( _frame );
        } else
            kdError(32001) << "Unknown tag '" << tag << "' in FRAMESET" << endl;

        if ( !parser.close( tag ) ) {
            kdError(32001) << "Closing " << tag << endl;
            return;
        }
    }

    updateCounters();
}

/*================================================================*/
void KWTextFrameSet::updateCounters()
{
    KWParag *p = getFirstParag();

    int counterData[ 16 ], listData[ 16 ];
    unsigned int i = 0;
    for ( i = 0; i < 16; i++ ) {
        counterData[ i ] = -2;
        listData[ i ] = -2;
    }
    KWParagLayout::CounterType ct = KWParagLayout::CT_NONE;

    while ( p ) {
        if ( p->getParagLayout()->getCounterType() != KWParagLayout::CT_NONE ) {
            if ( p->getParagLayout()->getNumberingType() == KWParagLayout::NT_CHAPTER ) {
                counterData[ p->getParagLayout()->getCounterDepth() ]++;
                for ( i = 0; i < 16; i++ ) {
                    if ( counterData[ i ] < 0 ) {
                        counterData[ i ] = p->getParagLayout()->getStartCounter();
                    }
                    p->getCounterData()[ i ] = counterData[ i ];
                }
                p->makeCounterText();
                for ( i = p->getParagLayout()->getCounterDepth() + 1; i < 16; i++ )
                    counterData[ i ] = -2;
                if ( listData[ 0 ] != -2 ) {
                    for ( i = 0; i < 16; i++ )
                        listData[ i ] = -2;
                }
            } else {
                if ( ct != p->getParagLayout()->getCounterType() ) {
                    for ( i = 0; i < 16; i++ )
                        listData[ i ] = -2;
                }
                ct = p->getParagLayout()->getCounterType();
                if ( p->getParagLayout()->getCounterType() != KWParagLayout::CT_BULLET ) {
                    listData[ p->getParagLayout()->getCounterDepth() ]++;
                } else if ( listData[ 0 ] != -2 ) {
                    for ( i = 0; i < 16; i++ )
                        listData[ i ] = -2;
                }
                for ( i = 0; i < 16; i++ ) {
                    if ( listData[ i ] < 0 ) {
                      listData[ i ] = p->getParagLayout()->getStartCounter();
                    }
                    p->getCounterData()[ i ] = listData[ i ];
                }
                p->makeCounterText();
                for ( i = p->getParagLayout()->getCounterDepth() + 1; i < 16; i++ )
                    listData[ i ] = -2;
            }
        } else if ( listData[ 0 ] != -2 ) {
            for ( i = 0; i < 16; i++ )
                listData[ i ] = -2;
        }
        p = p->getNext();
    }
}

/*================================================================*/
void KWTextFrameSet::updateAllStyles()
{
    KWParag *p = getFirstParag();

    while ( p ) {
        if ( doc->isStyleChanged( p->getParagLayout()->getName() ) )
            p->applyStyle( p->getParagLayout()->getName() );
        p = p->getNext();
    }

    updateCounters();
}

/*================================================================*/
KWParag *KWTextFrameSet::getLastParag()
{
    KWParag *p = getFirstParag();
    KWParag *last = p;

    while ( p ) {
        last = p;
        p = p->getNext();
    }

    return last;
}

/*================================================================*/
KWTextFrameSet *KWTextFrameSet::getCopy() {
    /* returns a deep copy of self */
    KWTextFrameSet *newFS = new KWTextFrameSet(doc);
    newFS->setFrameInfo(getFrameInfo());
    newFS->setVisible(isVisible());
    newFS->setName(getName());
    newFS->setIsRemoveableHeader(isRemoveableHeader());
    for(unsigned int i=0; i< getNumFrames();i++) {
        KWFrame *thisFrame=getFrame(i)->getCopy();
        newFS->addFrame(thisFrame);
    }
    if(newFS->getNumFrames() >0)
        newFS->assign(this);
    return newFS;
}


/******************************************************************/
/* Class: KWPictureFrameSet                                       */
/******************************************************************/

/*================================================================*/
KWPictureFrameSet::~KWPictureFrameSet() {
    if(image) {
        image->decRef();
    }
}

/*================================================================*/
void KWPictureFrameSet::setFileName( QString _filename )
{
    int dashdash = _filename.findRev( "--" );
    if ( dashdash != -1 )
        _filename == _filename.left( dashdash );

    if ( image ) {
        image->decRef();
        image = 0L;
    }

    filename = _filename;

    bool del = false;
    // retrieve image from imageCollection
    KWImage *_image = doc->getImageCollection()->getImage( filename );
    if ( !_image ) {
        del = true;
        _image = new KWImage( doc, filename );
    }

    QString key;
    image = doc->getImageCollection()->getImage( *_image, key );

    if ( del )
        delete _image;
}

/*================================================================*/
void KWPictureFrameSet::setFileName( QString _filename, QSize _imgSize )
{
    int dashdash = _filename.findRev( "--" );
    if ( dashdash != -1 )
        _filename == _filename.left( dashdash );

    if ( image ) {
        image->decRef();
        image = 0L;
    }

    filename = _filename;

    bool del = false;
    KWImage *_image = doc->getImageCollection()->getImage( filename );
    if ( !_image ) {
        del = true;
        _image = new KWImage( doc, filename );
    }

    QString key;
    image = doc->getImageCollection()->getImage( *_image, key, _imgSize );

    if ( del )
        delete _image;
}

/*================================================================*/
void KWPictureFrameSet::setSize( QSize _imgSize )
{
    if ( image && _imgSize == image->size() ) return;

    QString key;
    image = doc->getImageCollection()->getImage( *image, key, _imgSize );
    if ( !image )
        setFileName( filename, _imgSize );
}

/*================================================================*/
void KWPictureFrameSet::save( QTextStream&out )
{
    out << otag << "<FRAMESET frameType=\"" << static_cast<int>( getFrameType() ) << "\" frameInfo=\""
        << static_cast<int>( frameInfo ) << "\">" << endl;

    KWFrameSet::save( out );

    out << otag << "<IMAGE>" << endl;
    image->save( out );
    out << etag << "</IMAGE>" << endl;

    out << etag << "</FRAMESET>" << endl;
}

/*================================================================*/
void KWPictureFrameSet::load( KOMLParser& parser, QValueList<KOMLAttrib>& lst )
{
    QString tag;
    QString name;

    while ( parser.open( QString::null, tag ) ) {
        parser.parseTag( tag, name, lst );

        if ( name == "IMAGE" ) {
            parser.parseTag( tag, name, lst );
            //QValueList<KOMLAttrib>::ConstIterator it = lst.begin();
            //for( ; it != lst.end(); ++it ) {
            //}

            KWImage *_image = new KWImage();
            _image->load( parser, lst, doc );
            //setFileName( _image->getFilename() );
            doc->addImageRequest( _image->getFilename(), this );
            delete _image;
        } else if ( name == "FRAME" ) {
            KWFrame rect;
            KWParagLayout::Border l, r, t, b;
            double lmm = 0.0, linch = 0.0, rmm = 0.0, rinch = 0.0, tmm = 0.0, tinch = 0.0, bmm = 0.0, binch = 0.0, ramm = 1.0, rainch = 0.0393701;
            double lpt = 0.0, rpt = 0.0, tpt = 0.0, bpt = 0.0, rapt = 2.83465;

            l.color = Qt::white;
            l.style = KWParagLayout::SOLID;
            l.ptWidth = 1;
            r.color = Qt::white;
            r.style = KWParagLayout::SOLID;
            r.ptWidth = 1;
            t.color = Qt::white;
            t.style = KWParagLayout::SOLID;
            t.ptWidth = 1;
            b.color = Qt::white;
            b.style = KWParagLayout::SOLID;
            b.ptWidth = 1;
            QColor c( Qt::white );

            parser.parseTag( tag, name, lst );
            QValueList<KOMLAttrib>::ConstIterator it = lst.begin();
            for( ; it != lst.end(); ++it ) {
                if ( ( *it ).m_strName == "left" )
                    rect.setLeft( ( *it ).m_strValue.toInt() );
                else if ( ( *it ).m_strName == "top" )
                    rect.setTop( ( *it ).m_strValue.toInt() );
                else if ( ( *it ).m_strName == "right" )
                    rect.setRight( ( *it ).m_strValue.toInt() );
                else if ( ( *it ).m_strName == "bottom" )
                    rect.setBottom( ( *it ).m_strValue.toInt() );
                else if ( ( *it ).m_strName == "runaround" )
                    rect.setRunAround( static_cast<RunAround>( ( *it ).m_strValue.toInt() ) );
                else if ( ( *it ).m_strName == "runaroundGap" )
                    rect.setRunAroundGap( KWUnit( ( *it ).m_strValue.toDouble() ) );
                else if ( ( *it ).m_strName == "runaGapPT" )
                    rapt = ( *it ).m_strValue.toDouble();
                else if ( ( *it ).m_strName == "runaGapMM" )
                    ramm = ( *it ).m_strValue.toDouble();
                else if ( ( *it ).m_strName == "runaGapINCH" )
                    rainch = ( *it ).m_strValue.toDouble();
                else if ( ( *it ).m_strName == "lWidth" )
                    l.ptWidth = ( *it ).m_strValue.toInt();
                else if ( ( *it ).m_strName == "rWidth" )
                    r.ptWidth = ( *it ).m_strValue.toInt();
                else if ( ( *it ).m_strName == "tWidth" )
                    t.ptWidth = ( *it ).m_strValue.toInt();
                else if ( ( *it ).m_strName == "bWidth" )
                    b.ptWidth = ( *it ).m_strValue.toInt();
                else if ( ( *it ).m_strName == "lRed" )
                    l.color.setRgb( ( *it ).m_strValue.toInt(), l.color.green(), l.color.blue() );
                else if ( ( *it ).m_strName == "rRed" )
                    r.color.setRgb( ( *it ).m_strValue.toInt(), r.color.green(), r.color.blue() );
                else if ( ( *it ).m_strName == "tRed" )
                    t.color.setRgb( ( *it ).m_strValue.toInt(), t.color.green(), t.color.blue() );
                else if ( ( *it ).m_strName == "bRed" )
                    b.color.setRgb( ( *it ).m_strValue.toInt(), b.color.green(), b.color.blue() );
                else if ( ( *it ).m_strName == "lGreen" )
                    l.color.setRgb( l.color.red(), ( *it ).m_strValue.toInt(), l.color.blue() );
                else if ( ( *it ).m_strName == "rGreen" )
                    r.color.setRgb( r.color.red(), ( *it ).m_strValue.toInt(), r.color.blue() );
                else if ( ( *it ).m_strName == "tGreen" )
                    t.color.setRgb( t.color.red(), ( *it ).m_strValue.toInt(), t.color.blue() );
                else if ( ( *it ).m_strName == "bGreen" )
                    b.color.setRgb( b.color.red(), ( *it ).m_strValue.toInt(), b.color.blue() );
                else if ( ( *it ).m_strName == "lBlue" )
                    l.color.setRgb( l.color.red(), l.color.green(), ( *it ).m_strValue.toInt() );
                else if ( ( *it ).m_strName == "rBlue" )
                    r.color.setRgb( r.color.red(), r.color.green(), ( *it ).m_strValue.toInt() );
                else if ( ( *it ).m_strName == "tBlue" )
                    t.color.setRgb( t.color.red(), t.color.green(), ( *it ).m_strValue.toInt() );
                else if ( ( *it ).m_strName == "bBlue" )
                    b.color.setRgb( b.color.red(), b.color.green(), ( *it ).m_strValue.toInt() );
                else if ( ( *it ).m_strName == "lStyle" )
                    l.style = static_cast<KWParagLayout::BorderStyle>( ( *it ).m_strValue.toInt() );
                else if ( ( *it ).m_strName == "rStyle" )
                    r.style = static_cast<KWParagLayout::BorderStyle>( ( *it ).m_strValue.toInt() );
                else if ( ( *it ).m_strName == "tStyle" )
                    t.style = static_cast<KWParagLayout::BorderStyle>( ( *it ).m_strValue.toInt() );
                else if ( ( *it ).m_strName == "bStyle" )
                    b.style = static_cast<KWParagLayout::BorderStyle>( ( *it ).m_strValue.toInt() );
                else if ( ( *it ).m_strName == "bkRed" )
                    c.setRgb( ( *it ).m_strValue.toInt(), c.green(), c.blue() );
                else if ( ( *it ).m_strName == "bkGreen" )
                    c.setRgb( c.red(), ( *it ).m_strValue.toInt(), c.blue() );
                else if ( ( *it ).m_strName == "bkBlue" )
                    c.setRgb( c.red(), c.green(), ( *it ).m_strValue.toInt() );
                else if ( ( *it ).m_strName == "bleftpt" )
                    lpt = ( *it ).m_strValue.toDouble();
                else if ( ( *it ).m_strName == "brightpt" )
                    rpt = ( *it ).m_strValue.toDouble();
                else if ( ( *it ).m_strName == "btoppt" )
                    tpt = ( *it ).m_strValue.toDouble();
                else if ( ( *it ).m_strName == "bbottompt" )
                    bpt = ( *it ).m_strValue.toDouble();
                else if ( ( *it ).m_strName == "bleftmm" )
                    lmm = ( *it ).m_strValue.toDouble();
                else if ( ( *it ).m_strName == "brightmm" )
                    rmm = ( *it ).m_strValue.toDouble();
                else if ( ( *it ).m_strName == "btopmm" )
                    tmm = ( *it ).m_strValue.toDouble();
                else if ( ( *it ).m_strName == "bbottommm" )
                    bmm = ( *it ).m_strValue.toDouble();
                else if ( ( *it ).m_strName == "bleftinch" )
                    linch = ( *it ).m_strValue.toDouble();
                else if ( ( *it ).m_strName == "brightinch" )
                    rinch = ( *it ).m_strValue.toDouble();
                else if ( ( *it ).m_strName == "btopinch" )
                    tinch = ( *it ).m_strValue.toDouble();
                else if ((*it).m_strName == "bbottominch")
                    binch = ( *it ).m_strValue.toDouble();
            }
            KWFrame *_frame = new KWFrame(this, rect.x(), rect.y(), rect.width(), rect.height(), rect.getRunAround(), rainch == -1 ? rect.getRunAroundGap() : KWUnit( rapt, ramm, rainch ) );
            _frame->setLeftBorder( l );
            _frame->setRightBorder( r );
            _frame->setTopBorder( t );
            _frame->setBottomBorder( b );
            _frame->setBackgroundColor( QBrush( c ) );
            _frame->setBLeft( KWUnit( lpt, lmm, linch ) );
            _frame->setBRight( KWUnit( rpt, rmm, rinch ) );
            _frame->setBTop( KWUnit( tpt, tmm, tinch ) );
            _frame->setBBottom( KWUnit( bpt, bmm, binch ) );
            frames.append( _frame );
        } else
            kdError(32001) << "Unknown tag '" << tag << "' in FRAMESET" << endl;

        if ( !parser.close( tag ) ) {
            kdError(32001) << "Closing " << tag << endl;
            return;
        }
    }
}

/******************************************************************/
/* Class: KWPartFrameSet                                          */
/******************************************************************/

/*================================================================*/
KWPartFrameSet::KWPartFrameSet( KWordDocument *_doc, KWordChild *_child )
    : KWFrameSet( _doc )
{
    child = _child;
    _enableDrawing = true;
    frame = 0L;
}

/*================================================================*/
KWPartFrameSet::~KWPartFrameSet()
{
}

/*================================================================*/
QPicture *KWPartFrameSet::getPicture()
{
    if ( !_enableDrawing )
        return 0;

    // No ! This moves back the child, when moving it while activated (David)
    //update();

    QPainter p( &pic );
    //child->transform( p );

    if ( child && child->document() )
        child->document()->paintEverything( p, QRect( QPoint( 0, 0 ),
                                            QSize( frames.at( 0 )->width(), frames.at( 0 )->height() ) ),
                                        TRUE, 0 );

    return &pic;
}

/*================================================================*/
void KWPartFrameSet::activate( QWidget *_widget )
{
    update();
    KWordView *view = (KWordView*)_widget;
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
void KWPartFrameSet::update()
{
    child->setGeometry( QRect( frames.at( 0 )->x(), frames.at( 0 )->y(),
                               frames.at( 0 )->width(), frames.at( 0 )->height() ) );
}

/*================================================================*/
void KWPartFrameSet::save( QTextStream&out )
{
    KWFrameSet::save( out );
}

/*================================================================*/
void KWPartFrameSet::load( KOMLParser& parser, QValueList<KOMLAttrib>& lst )
{
    QString tag;
    QString name;

    while ( parser.open( QString::null, tag ) ) {
        parser.parseTag( tag, name, lst );

        if ( name == "FRAME" ) {
            KWFrame rect;
            KWParagLayout::Border l, r, t, b;
            double lmm = 0.0, linch = 0.0, rmm = 0.0, rinch = 0.0, tmm = 0.0, tinch = 0.0, bmm = 0.0, binch = 0.0, ramm = 0.0, rainch = -1.0;
            double lpt = 0.0, rpt = 0.0, tpt = 0.0, bpt = 0.0, rapt = 0.0;

            l.color = Qt::white;
            l.style = KWParagLayout::SOLID;
            l.ptWidth = 1;
            r.color = Qt::white;
            r.style = KWParagLayout::SOLID;
            r.ptWidth = 1;
            t.color = Qt::white;
            t.style = KWParagLayout::SOLID;
            t.ptWidth = 1;
            b.color = Qt::white;
            b.style = KWParagLayout::SOLID;
            b.ptWidth = 1;
            QColor c( Qt::white );

            parser.parseTag( tag, name, lst );
            QValueList<KOMLAttrib>::ConstIterator it = lst.begin();
            for( ; it != lst.end(); ++it ) {
                if ( ( *it ).m_strName == "left" )
                    rect.setLeft( ( *it ).m_strValue.toInt() );
                else if ( ( *it ).m_strName == "top" )
                    rect.setTop( ( *it ).m_strValue.toInt() );
                else if ( ( *it ).m_strName == "right" )
                    rect.setRight( ( *it ).m_strValue.toInt() );
                else if ( ( *it ).m_strName == "bottom" )
                    rect.setBottom( ( *it ).m_strValue.toInt() );
                else if ( ( *it ).m_strName == "runaround" )
                    rect.setRunAround( static_cast<RunAround>( ( *it ).m_strValue.toInt() ) );
                else if ( ( *it ).m_strName == "runaroundGap" )
                    rect.setRunAroundGap( KWUnit( ( *it ).m_strValue.toDouble() ) );
                else if ( ( *it ).m_strName == "runaGapPT" )
                    rapt = ( *it ).m_strValue.toDouble();
                else if ( ( *it ).m_strName == "runaGapMM" )
                    ramm = ( *it ).m_strValue.toDouble();
                else if ( ( *it ).m_strName == "runaGapINCH" )
                    rainch = ( *it ).m_strValue.toDouble();
                else if ( ( *it ).m_strName == "lWidth" )
                    l.ptWidth = ( *it ).m_strValue.toInt();
                else if ( ( *it ).m_strName == "rWidth" )
                    r.ptWidth = ( *it ).m_strValue.toInt();
                else if ( ( *it ).m_strName == "tWidth" )
                    t.ptWidth = ( *it ).m_strValue.toInt();
                else if ( ( *it ).m_strName == "bWidth" )
                    b.ptWidth = ( *it ).m_strValue.toInt();
                else if ( ( *it ).m_strName == "lRed" )
                    l.color.setRgb( ( *it ).m_strValue.toInt(), l.color.green(), l.color.blue() );
                else if ( ( *it ).m_strName == "rRed" )
                    r.color.setRgb( ( *it ).m_strValue.toInt(), r.color.green(), r.color.blue() );
                else if ( ( *it ).m_strName == "tRed" )
                    t.color.setRgb( ( *it ).m_strValue.toInt(), t.color.green(), t.color.blue() );
                else if ( ( *it ).m_strName == "bRed" )
                    b.color.setRgb( ( *it ).m_strValue.toInt(), b.color.green(), b.color.blue() );
                else if ( ( *it ).m_strName == "lGreen" )
                    l.color.setRgb( l.color.red(), ( *it ).m_strValue.toInt(), l.color.blue() );
                else if ( ( *it ).m_strName == "rGreen" )
                    r.color.setRgb( r.color.red(), ( *it ).m_strValue.toInt(), r.color.blue() );
                else if ( ( *it ).m_strName == "tGreen" )
                    t.color.setRgb( t.color.red(), ( *it ).m_strValue.toInt(), t.color.blue() );
                else if ( ( *it ).m_strName == "bGreen" )
                    b.color.setRgb( b.color.red(), ( *it ).m_strValue.toInt(), b.color.blue() );
                else if ( ( *it ).m_strName == "lBlue" )
                    l.color.setRgb( l.color.red(), l.color.green(), ( *it ).m_strValue.toInt() );
                else if ( ( *it ).m_strName == "rBlue" )
                    r.color.setRgb( r.color.red(), r.color.green(), ( *it ).m_strValue.toInt() );
                else if ( ( *it ).m_strName == "tBlue" )
                    t.color.setRgb( t.color.red(), t.color.green(), ( *it ).m_strValue.toInt() );
                else if ( ( *it ).m_strName == "bBlue" )
                    b.color.setRgb( b.color.red(), b.color.green(), ( *it ).m_strValue.toInt() );
                else if ( ( *it ).m_strName == "lStyle" )
                    l.style = static_cast<KWParagLayout::BorderStyle>( ( *it ).m_strValue.toInt() );
                else if ( ( *it ).m_strName == "rStyle" )
                    r.style = static_cast<KWParagLayout::BorderStyle>( ( *it ).m_strValue.toInt() );
                else if ( ( *it ).m_strName == "tStyle" )
                    t.style = static_cast<KWParagLayout::BorderStyle>( ( *it ).m_strValue.toInt() );
                else if ( ( *it ).m_strName == "bStyle" )
                    b.style = static_cast<KWParagLayout::BorderStyle>( ( *it ).m_strValue.toInt() );
                else if ( ( *it ).m_strName == "bkRed" )
                    c.setRgb( ( *it ).m_strValue.toInt(), c.green(), c.blue() );
                else if ( ( *it ).m_strName == "bkGreen" )
                    c.setRgb( c.red(), ( *it ).m_strValue.toInt(), c.blue() );
                else if ( ( *it ).m_strName == "bkBlue" )
                    c.setRgb( c.red(), c.green(), ( *it ).m_strValue.toInt() );
                else if ( ( *it ).m_strName == "bleftpt" )
                    lpt = ( *it ).m_strValue.toDouble();
                else if ( ( *it ).m_strName == "brightpt" )
                    rpt = ( *it ).m_strValue.toDouble();
                else if ( ( *it ).m_strName == "btoppt" )
                    tpt = ( *it ).m_strValue.toDouble();
                else if ( ( *it ).m_strName == "bbottompt" )
                    bpt = ( *it ).m_strValue.toDouble();
                else if ( ( *it ).m_strName == "bleftmm" )
                    lmm = ( *it ).m_strValue.toDouble();
                else if ( ( *it ).m_strName == "brightmm" )
                    rmm = ( *it ).m_strValue.toDouble();
                else if ( ( *it ).m_strName == "btopmm" )
                    tmm = ( *it ).m_strValue.toDouble();
                else if ( ( *it ).m_strName == "bbottommm" )
                    bmm = ( *it ).m_strValue.toDouble();
                else if ( ( *it ).m_strName == "bleftinch" )
                    linch = ( *it ).m_strValue.toDouble();
                else if ( ( *it ).m_strName == "brightinch" )
                    rinch = ( *it ).m_strValue.toDouble();
                else if ( ( *it ).m_strName == "btopinch" )
                    tinch = ( *it ).m_strValue.toDouble();
                else if ((*it).m_strName == "bbottominch")
                    binch = ( *it ).m_strValue.toDouble();
            }
            KWFrame *_frame = new KWFrame(this, rect.x(), rect.y(), rect.width(), rect.height(), rect.getRunAround(), rainch == -1 ? rect.getRunAroundGap() : KWUnit( rapt, ramm, rainch ) );
            _frame->setLeftBorder( l );
            _frame->setRightBorder( r );
            _frame->setTopBorder( t );
            _frame->setBottomBorder( b );
            _frame->setBackgroundColor( QBrush( c ) );
            _frame->setBLeft( KWUnit( lpt, lmm, linch ) );
            _frame->setBRight( KWUnit( rpt, rmm, rinch ) );
            _frame->setBTop( KWUnit( tpt, tmm, tinch ) );
            _frame->setBBottom( KWUnit( bpt, bmm, binch ) );
            frames.append( _frame );
        } else
            kdError(32001) << "Unknown tag '" << tag << "' in FRAMESET" << endl;

        if ( !parser.close( tag ) ) {
            kdError(32001) << "Closing " << tag << endl;
            return;
        }
    }
}

#include <kformulaedit.h>

/******************************************************************/
/* Class: KWFormulaFrameSet                                       */
/******************************************************************/

/*================================================================*/
KWFormulaFrameSet::KWFormulaFrameSet( KWordDocument *_doc, QWidget *parent )
    : KWFrameSet( _doc ), format(0L), pic( 0 ), font( "times", 12 ), color( Qt::black )

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
KWFormulaFrameSet::KWFormulaFrameSet( KWordDocument *_doc )
    : KWFrameSet( _doc ), formulaEdit( 0 ), format(0L), pic( 0 )
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
    update();
}

/*================================================================*/
KWFormat *KWFormulaFrameSet::getFormat()
{
    if(format==0L)
        format = new KWFormat( doc );
    format->setUserFont( doc->findUserFont( font.family() ) );
    format->setPTFontSize( font.pointSize() );
    format->setWeight( font.weight() );
    format->setUnderline( font.underline() );
    format->setItalic( font.italic() );
    format->setVertAlign( KWFormat::VA_NORMAL );
    format->setColor( color );

    return format;
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
        update();
        return;
    }

    formulaEdit = new KFormulaEdit( ( (QScrollView*)parent )->viewport() );
    ( (QScrollView*)parent )->addChild( formulaEdit );
    formulaEdit->getFormula()->setFont( font );
    formulaEdit->getFormula()->setBackColor( frames.at( 0 )->getBackgroundColor().color() );
    formulaEdit->getFormula()->setForeColor( color );
    formulaEdit->hide();
    formulaEdit->setText( text );
    update();
}

/*================================================================*/
void KWFormulaFrameSet::update()
{
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
void KWFormulaFrameSet::save( QTextStream&out )
{
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
}

/*================================================================*/
void KWFormulaFrameSet::load( KOMLParser& parser, QValueList<KOMLAttrib>& lst )
{
    QString tag;
    QString name;
    //QString tmp;

    while ( parser.open( QString::null, tag ) ) {
        parser.parseTag( tag, name, lst );

        if ( name == "FORMULA" ) {
            parser.parseTag( tag, name, lst );
            //vector<KOMLAttrib>::const_iterator it = lst.begin();
            //for( ; it != lst.end(); it++ ) {
            //}
            parser.readText( text );
            text = text.stripWhiteSpace();
        } if ( name == "FORMAT" ) {
            KWFormat f( doc );
            f.load( parser, lst, doc );
            font = QFont( f.getUserFont()->getFontName() );
            font.setPointSize( f.getPTFontSize() );
            font.setWeight( f.getWeight() );
            font.setUnderline( f.getUnderline() );
            font.setItalic( f.getItalic() );
            color = f.getColor();
        } else if ( name == "FRAME" ) {
            KWFrame rect;
            KWParagLayout::Border l, r, t, b;
            double lmm = 0.0, linch = 0.0, rmm = 0.0, rinch = 0.0, tmm = 0.0, tinch = 0.0, bmm = 0.0, binch = 0.0, ramm = 1.0, rainch = 0.0393701;
            double lpt = 0.0, rpt = 0.0, tpt = 0.0, bpt = 0.0, rapt = 2.83465;

            l.color = Qt::white;
            l.style = KWParagLayout::SOLID;
            l.ptWidth = 1;
            r.color = Qt::white;
            r.style = KWParagLayout::SOLID;
            r.ptWidth = 1;
            t.color = Qt::white;
            t.style = KWParagLayout::SOLID;
            t.ptWidth = 1;
            b.color = Qt::white;
            b.style = KWParagLayout::SOLID;
            b.ptWidth = 1;
            QColor c( Qt::white );

            parser.parseTag( tag, name, lst );
            QValueList<KOMLAttrib>::ConstIterator it = lst.begin();
            for( ; it != lst.end(); ++it ) {
                if ( ( *it ).m_strName == "left" )
                    rect.setLeft( ( *it ).m_strValue.toInt() );
                else if ( ( *it ).m_strName == "top" )
                    rect.setTop( ( *it ).m_strValue.toInt() );
                else if ( ( *it ).m_strName == "right" )
                    rect.setRight( ( *it ).m_strValue.toInt() );
                else if ( ( *it ).m_strName == "bottom" )
                    rect.setBottom( ( *it ).m_strValue.toInt() );
                else if ( ( *it ).m_strName == "runaround" )
                    rect.setRunAround( static_cast<RunAround>( ( *it ).m_strValue.toInt() ) );
                else if ( ( *it ).m_strName == "runaroundGap" )
                    rect.setRunAroundGap( KWUnit( ( *it ).m_strValue.toDouble() ) );
                else if ( ( *it ).m_strName == "runaGapPT" )
                    rapt = ( *it ).m_strValue.toDouble();
                else if ( ( *it ).m_strName == "runaGapMM" )
                    ramm = ( *it ).m_strValue.toDouble();
                else if ( ( *it ).m_strName == "runaGapINCH" )
                    rainch = ( *it ).m_strValue.toDouble();
                else if ( ( *it ).m_strName == "lWidth" )
                    l.ptWidth = ( *it ).m_strValue.toInt();
                else if ( ( *it ).m_strName == "rWidth" )
                    r.ptWidth = ( *it ).m_strValue.toInt();
                else if ( ( *it ).m_strName == "tWidth" )
                    t.ptWidth = ( *it ).m_strValue.toInt();
                else if ( ( *it ).m_strName == "bWidth" )
                    b.ptWidth = ( *it ).m_strValue.toInt();
                else if ( ( *it ).m_strName == "lRed" )
                    l.color.setRgb( ( *it ).m_strValue.toInt(), l.color.green(), l.color.blue() );
                else if ( ( *it ).m_strName == "rRed" )
                    r.color.setRgb( ( *it ).m_strValue.toInt(), r.color.green(), r.color.blue() );
                else if ( ( *it ).m_strName == "tRed" )
                    t.color.setRgb( ( *it ).m_strValue.toInt(), t.color.green(), t.color.blue() );
                else if ( ( *it ).m_strName == "bRed" )
                    b.color.setRgb( ( *it ).m_strValue.toInt(), b.color.green(), b.color.blue() );
                else if ( ( *it ).m_strName == "lGreen" )
                    l.color.setRgb( l.color.red(), ( *it ).m_strValue.toInt(), l.color.blue() );
                else if ( ( *it ).m_strName == "rGreen" )
                    r.color.setRgb( r.color.red(), ( *it ).m_strValue.toInt(), r.color.blue() );
                else if ( ( *it ).m_strName == "tGreen" )
                    t.color.setRgb( t.color.red(), ( *it ).m_strValue.toInt(), t.color.blue() );
                else if ( ( *it ).m_strName == "bGreen" )
                    b.color.setRgb( b.color.red(), ( *it ).m_strValue.toInt(), b.color.blue() );
                else if ( ( *it ).m_strName == "lBlue" )
                    l.color.setRgb( l.color.red(), l.color.green(), ( *it ).m_strValue.toInt() );
                else if ( ( *it ).m_strName == "rBlue" )
                    r.color.setRgb( r.color.red(), r.color.green(), ( *it ).m_strValue.toInt() );
                else if ( ( *it ).m_strName == "tBlue" )
                    t.color.setRgb( t.color.red(), t.color.green(), ( *it ).m_strValue.toInt() );
                else if ( ( *it ).m_strName == "bBlue" )
                    b.color.setRgb( b.color.red(), b.color.green(), ( *it ).m_strValue.toInt() );
                else if ( ( *it ).m_strName == "lStyle" )
                    l.style = static_cast<KWParagLayout::BorderStyle>( ( *it ).m_strValue.toInt() );
                else if ( ( *it ).m_strName == "rStyle" )
                    r.style = static_cast<KWParagLayout::BorderStyle>( ( *it ).m_strValue.toInt() );
                else if ( ( *it ).m_strName == "tStyle" )
                    t.style = static_cast<KWParagLayout::BorderStyle>( ( *it ).m_strValue.toInt() );
                else if ( ( *it ).m_strName == "bStyle" )
                    b.style = static_cast<KWParagLayout::BorderStyle>( ( *it ).m_strValue.toInt() );
                else if ( ( *it ).m_strName == "bkRed" )
                    c.setRgb( ( *it ).m_strValue.toInt(), c.green(), c.blue() );
                else if ( ( *it ).m_strName == "bkGreen" )
                    c.setRgb( c.red(), ( *it ).m_strValue.toInt(),  c.blue() );
                else if ( ( *it ).m_strName == "bkBlue" )
                    c.setRgb( c.red(), c.green(), ( *it ).m_strValue.toInt() );
                else if ( ( *it ).m_strName == "bleftpt" )
                    lpt = ( *it ).m_strValue.toDouble();
                else if ( ( *it ).m_strName == "brightpt" )
                    rpt = ( *it ).m_strValue.toDouble();
                else if ( ( *it ).m_strName == "btoppt" )
                    tpt = ( *it ).m_strValue.toDouble();
                else if ( ( *it ).m_strName == "bbottompt" )
                    bpt = ( *it ).m_strValue.toDouble();
                else if ( ( *it ).m_strName == "bleftmm" )
                    lmm = ( *it ).m_strValue.toDouble();
                else if ( ( *it ).m_strName == "brightmm" )
                    rmm = ( *it ).m_strValue.toDouble();
                else if ( ( *it ).m_strName == "btopmm" )
                    tmm = ( *it ).m_strValue.toDouble();
                else if ( ( *it ).m_strName == "bbottommm" )
                    bmm = ( *it ).m_strValue.toDouble();
                else if ( ( *it ).m_strName == "bleftinch" )
                    linch = ( *it ).m_strValue.toDouble();
                else if ( ( *it ).m_strName == "brightinch" )
                    rinch = ( *it ).m_strValue.toDouble();
                else if ( ( *it ).m_strName == "btopinch" )
                    tinch = ( *it ).m_strValue.toDouble();
                else if ((*it).m_strName == "bbottominch")
                    binch = ( *it ).m_strValue.toDouble();
            } KWFrame *_frame = new KWFrame(this, rect.x(), rect.y(), rect.width(), rect.height(), rect.getRunAround(), rainch == -1 ? rect.getRunAroundGap() : KWUnit( rapt, ramm, rainch ) );
            _frame->setLeftBorder( l );
            _frame->setRightBorder( r );
            _frame->setTopBorder( t );
            _frame->setBottomBorder( b );
            _frame->setBackgroundColor( QBrush( c ) );
            _frame->setBLeft( KWUnit( lpt, lmm, linch ) );
            _frame->setBRight( KWUnit( rpt, rmm, rinch ) );
            _frame->setBTop( KWUnit( tpt, tmm, tinch ) );
            _frame->setBBottom( KWUnit( bpt, bmm, binch ) );
            frames.append( _frame );
        } else
            kdError(32001) << "Unknown tag '" << tag << "' in FRAMESET" << endl;

        if ( !parser.close( tag ) ) {
            kdError(32001) << "Closing " << tag << endl;
            return;
        }
    }
}


/******************************************************************/
/* Class: KWGroupManager                                          */
/******************************************************************/

/*================================================================*/
KWGroupManager::KWGroupManager( KWordDocument *_doc ) :
    KWCharAnchor(), showHeaderOnAllPages( true ), hasTmpHeaders( false ), active( true )
{
    doc = _doc;
    cells.setAutoDelete( true );
    rows = 0;
    cols = 0;
    name = QString::null;
}

/*================================================================*/
KWGroupManager::KWGroupManager( const KWGroupManager &original ) :
    KWCharAnchor(original)
{
    showHeaderOnAllPages = original.showHeaderOnAllPages;
    hasTmpHeaders = original.hasTmpHeaders;
    active = original.active;
    doc = original.doc;
    rows = original.rows;
    cols = original.cols;
    name = original.name;

    // copy all cells
    cells.setAutoDelete( true );
    QList<Cell> lCells = original.cells;
    lCells.setAutoDelete(false);
    for (unsigned int i=0; i< lCells.count();i++) {
        if(lCells.at(i))  {
            Cell *cell = new Cell;
            cell->row = lCells.at(i)->row;
            cell->col = lCells.at(i)->col;
            cell->rows = lCells.at(i)->rows;
            cell->cols = lCells.at(i)->cols;
            cell->frameSet= dynamic_cast<KWTextFrameSet*>(lCells.at(i)->frameSet)->getCopy();
            cell->frameSet->setGroupManager(this);
            cells.append( cell );
        }
    }

    doc->addGroupManager(this);
    init();
}

/*================================================================*/
KWGroupManager::~KWGroupManager() {
    if(doc) doc->delGroupManager(this, false);
    doc=0L;
}

/*================================================================*/
void KWGroupManager::addFrameSet( KWFrameSet *fs, unsigned int row, unsigned int col )
{
    unsigned int i;

    rows = QMAX( row + 1, rows );
    cols = QMAX( col + 1, cols );

    for (i = 0; i < cells.count() && (cells.at(i)->row < row || 
        cells.at(i)->row == row  && cells.at(i)->col < col) ; i++ ); 
    if(fs->getName().isEmpty())
        fs->setName(QString ("Cell %1,%2").arg(col+1).arg(row+1));

    // If the group is anchored, we must adjust the incoming frameset.
    if ( anchored ) {
        KWFrame *topLeftFrame = fs->getFrame( 0 );

        if (topLeftFrame)
            topLeftFrame->moveBy( origin.x(), origin.y() );
    }

    Cell *cell = new Cell;
    cell->frameSet = fs;
    cell->row = row;
    cell->col = col;
    cell->rows = 1;
    cell->cols = 1;

    cells.insert( i, cell );
}

/*================================================================*/
KWFrameSet *KWGroupManager::getFrameSet( unsigned int row, unsigned int col )
{
    Cell *cell=getCell(row,col);
    if(cell) return cell->frameSet;
    return 0L;
}

/*================================================================*/
/* returns the cell that occupies row, col. */
KWGroupManager::Cell *KWGroupManager::getCell( unsigned int row, unsigned int col )
{
    for ( unsigned int i = 0; i < cells.count(); i++ ) {
        if ( cells.at( i )->row <= row &&
                cells.at( i )->col <= col &&
                cells.at( i )->row+cells.at( i )->rows > row &&
                cells.at( i )->col+cells.at( i )->cols > col ) {
            return cells.at( i );
        }
    }
    return 0L;
}

/*================================================================*/
KWGroupManager::Cell *KWGroupManager::getCell( KWFrameSet *f ) {
    for ( unsigned int i = 0; i < cells.count(); i++ ) {
        if ( cells.at( i )->frameSet == f) {
            return cells.at(i);
        }
    }
    return 0L;
}

/*================================================================*/
bool KWGroupManager::isTableHeader( KWFrameSet *fs )
{
    Cell *cell = getCell(fs);
    if(cell) 
        return fs->isRemoveableHeader() || cell->row==0;

    return false;
}

/*================================================================*/
void KWGroupManager::init( unsigned int x, unsigned int y, unsigned int width, unsigned int height,
                           KWTblCellSize widthScaling, KWTblCellSize heightScaling )
{
    if ( widthScaling == TblAuto ) {
        x = doc->getPTLeftBorder();
        width = doc->getPTPaperWidth() -
                ( doc->getPTLeftBorder() + doc->getPTRightBorder() );
    }

    double baseWidth = (width - (cols-1) * tableCellSpacing) / cols;
    double baseHeight=0;
    if(heightScaling!=TblAuto)
        baseHeight = (height - (rows-1) * tableCellSpacing) / rows;

    // I will create 1 mm margins, this will recalculate the actual size needed for the frame.
    KWUnit oneMm;
    oneMm.setMM( 1 );
    double minBaseHeight= doc->getDefaultParagLayout()->getFormat().getPTFontSize() + 
            oneMm.pt() * 2;
    if(baseHeight < minBaseHeight + oneMm.pt() * 2)
        baseHeight =minBaseHeight + oneMm.pt() * 2;
    if(baseWidth < minFrameWidth + oneMm.pt() * 2)
        baseWidth = minFrameWidth +  oneMm.pt() * 2;
    // cast them only one time up here..
    unsigned int frameWidth = static_cast<unsigned int> (baseWidth + 0.5); 
    unsigned int frameHeight = static_cast<unsigned int> (baseHeight + 0.5);
    // move/size the cells
    for ( unsigned int i = 0; i < rows; i++ ) {
        for ( unsigned int j = 0; j < cols; j++ ) {
            KWFrame *frame = getFrameSet( i, j )->getFrame( 0 );
            frame->setBLeft( oneMm );
            frame->setBRight( oneMm );
            frame->setBTop( oneMm );
            frame->setBBottom( oneMm );
            frame->setNewFrameBehaviour( NoFollowup );
            frame->setRect( x + j * frameWidth + j * tableCellSpacing, 
                y + i * frameHeight + i * tableCellSpacing, baseWidth, baseHeight );
        }
    }

    for ( unsigned int k = 0; k < cells.count(); k++ )
        doc->addFrameSet( cells.at( k )->frameSet );
}

/*================================================================*/
void KWGroupManager::init()
{
    for ( unsigned int k = 0; k < cells.count(); k++ )
        doc->addFrameSet( cells.at( k )->frameSet );
}

/*================================================================*/
void KWGroupManager::recalcCols()
{
    unsigned int row=0,col=0;
    if(! cells.isEmpty() && isOneSelected(row,col)) {
        // ** check/set sizes of frames **
        // we assume only left or only right pos has changed.
        // check if leftCoordinate is same as rest of tableRow
        Cell *activeCell = getCell(row,col);
        Cell *cell;
        int coordinate;
        // find old coord.
        coordinate=activeCell->frameSet->getFrame(0)->left();
        if(col!=0) { // calculate the old position.
            coordinate = getCell(row, col-1)->frameSet->getFrame(0)->right() + tableCellSpacing;
        } else { // is leftmost, so lets look at other rows..
            for ( unsigned int i = 0; i < rows; i++) {
                if(i!=row) {
                    cell=getCell(i,col);
                    if(cell->col==col) {
                        coordinate=cell->frameSet->getFrame(0)->left();
                        break;
                    }
                }
            }
        }
            
        int postAdjust=0;
        if(coordinate != activeCell->frameSet->getFrame(0)->left()) { // left pos changed
            // we are now going to move the rest of the cells in this column as well.
            for ( unsigned int i = 0; i < rows; i++) {
                int difference=0;
                if(col==0) {// left most cell
                    cell = getCell(i,col);
                    if(cell==activeCell)
                        cell=0;
                    else
                        difference=(activeCell->frameSet->getFrame(0)->left() - coordinate) * -1;
                } else {
                    cell = getCell(i,col-1);
                    if(cell->row == i) // dont resize joined cells more then ones.
                        difference=activeCell->frameSet->getFrame(0)->left() - coordinate;
                    else 
                        cell=0;
                }
                if(cell) {
                    // rescale this cell with the calculated difference
                    int newWidth=cell->frameSet->getFrame(0)->width() + difference;
                    if(newWidth<minFrameWidth) {
                        if(minFrameWidth-newWidth > postAdjust)
                            postAdjust = minFrameWidth-newWidth;
                    }
                    cell->frameSet->getFrame(0)->setWidth(newWidth);
                }
            }

            // Because we are scaling the cells left of this one, the activeCell has to be
            // returned to its original size.
            if(col!=0)
                activeCell->frameSet->getFrame(0)->setWidth(
                  activeCell->frameSet->getFrame(0)->width() +
                  activeCell->frameSet->getFrame(0)->left() - coordinate); 

            // if we found cells that ware made to small, we adjust them using the postAdjust var.
            for ( unsigned int i = 0; i < rows; i++) {
                if(col==0) col++;
                cell = getCell(i,col-1);
                if(cell->row == i) 
                    cell->frameSet->getFrame(0)->setWidth( cell->frameSet->getFrame(0)->width()+postAdjust);
            }
        } else {
            col+=activeCell->cols-1;
            // find old coord.
            coordinate=activeCell->frameSet->getFrame(0)->right();
            bool found=false;
            for ( unsigned int i = 0; i < rows; i++) {
                if(i!=row) {
                    cell=getCell(i,activeCell->cols+activeCell->col-1);
                    if(cell->col+cell->cols==activeCell->cols+activeCell->col) {
                        coordinate=cell->frameSet->getFrame(0)->right();
                        found=true;
                        break;
                    }
                }
            }
            if(! found && activeCell->col + activeCell->cols < cols) { // if we did not find it and we are not on the right edge of the table.
               // use the position of the next cell.
               coordinate = getCell(activeCell->row, activeCell->col + activeCell->cols)->frameSet->getFrame(0)->left() - tableCellSpacing;
            }

            if(coordinate != activeCell->frameSet->getFrame(0)->right()) { // right pos changed.
                for ( unsigned int i = 0; i < rows; i++) {
                    Cell *cell = getCell(i,col);
                    if(cell != activeCell && cell->row == i) {
                        int newWidth= cell->frameSet->getFrame(0)->width() +
                            activeCell->frameSet->getFrame(0)->right() - coordinate;
                        if(newWidth<minFrameWidth) {
                            if(minFrameWidth-newWidth > postAdjust)
                                postAdjust = minFrameWidth-newWidth;
                        }
                        cell->frameSet->getFrame(0)->setWidth(newWidth);
                    }
                }
                for ( unsigned int i = 0; i < rows; i++) {
                    cell = getCell(i,col);
                    if(cell->row == i) 
                        cell->frameSet->getFrame(0)->setWidth( cell->frameSet->getFrame(0)->width()+postAdjust);
                }
            }
        }

        // Move cells
        unsigned int x, nextX=0;
        if(getFrameSet(0,0) &&  getFrameSet( 0, 0 )->getFrame( 0 ))
            nextX =getFrameSet( 0, 0 )->getFrame( 0 )->x();

        for ( unsigned int i = 0; i < cols; i++ ) {
            x=nextX;
            for ( unsigned int j = 0; j < rows; j++ ) {
                Cell *cell = getCell(j,i);
                if(cell->col==i && cell->row==j) {
                    cell->frameSet->getFrame( 0 )->moveTopLeft( QPoint( x, cell->frameSet->getFrame( 0 )->y() ) );
                }
                if(cell->col + cell->cols -1 == i)
                    nextX=cell->frameSet->getFrame(0) -> right() + tableCellSpacing;
            }
        }
    }
}

/*================================================================*/
void KWGroupManager::recalcRows()
{
    // remove automatically added headers
    for ( unsigned int j = 0; j < rows; j++ ) {
        if ( getFrameSet( j, 0 )->isRemoveableHeader() ) {
            deleteRow( j, false );
            j--;
        }
    }
    hasTmpHeaders = false;
    // check/set sizes of frames
    unsigned int row=0,col=0;
    if(! cells.isEmpty() && isOneSelected(row,col)) {
        // check if topCoordinate is same as rest of tableRow
        Cell *activeCell = getCell(row,col);
        Cell *cell;
        int coordinate;
        // find old coord.
        coordinate=activeCell->frameSet->getFrame(0)->top();
        for ( unsigned int i = 0; i < cols; i++) {
            if(i!=col) {
                cell=getCell(row,i);
                if(cell->row==row) {
                    coordinate=cell->frameSet->getFrame(0)->top();
                    break;
                }
            }
        }
        int postAdjust=0;
        if(coordinate != activeCell->frameSet->getFrame(0)->top()) { // top pos changed
            for ( unsigned int i = 0; i < cols; i++) {
                int difference=0;
                if(row==0) { // top cell
                    cell = getCell(0,i);
                    if(cell==activeCell)
                        cell=0;
                    else
                        difference= (activeCell->frameSet->getFrame(0)->top()- coordinate) * -1;
                } else {
                    cell = getCell(row-1,i);
                    if(cell->col == i) // dont resize joined cells more then ones.
                        difference= activeCell->frameSet->getFrame(0)->top()- coordinate;
                    else 
                        cell=0;
                }
                if(cell) {
                    int newHeight= cell->frameSet->getFrame(0)->height() + difference;
                    if(newHeight<minFrameHeight) {
                        if(minFrameHeight-newHeight > postAdjust)
                            postAdjust = minFrameHeight-newHeight;
                    }
                    cell->frameSet->getFrame(0)->setHeight(newHeight);
                }
            }
            if(row!=0)
                activeCell->frameSet->getFrame(0)->setHeight(
                    activeCell->frameSet->getFrame(0)->height() +
                    activeCell->frameSet->getFrame(0)->top()- coordinate);
            if(postAdjust!=0) {
                if(row==0) row++;
                for ( unsigned int i = 0; i < cols; i++) {
                    cell = getCell(row-1,i);
                    if(cell->col == i)
                        cell->frameSet->getFrame(0)->setHeight(
                            cell->frameSet->getFrame(0)->height() + postAdjust);
                }
            }
        } else { // bottom pos has changed
            row+=activeCell->rows-1;
            // find old coord.
            coordinate=activeCell->frameSet->getFrame(0)->bottom();
            for ( unsigned int i = 0; i < cols; i++) {
                if(i!=col) {
                    cell=getCell(activeCell->row+activeCell->rows-1,i);
                    if(cell->row+cell->rows==activeCell->row+activeCell->rows) {
                        coordinate=cell->frameSet->getFrame(0)->bottom();
                        break;
                    }
                }
            }
            if(coordinate != activeCell->frameSet->getFrame(0)->bottom()) {
                for ( unsigned int i = 0; i < cols; i++) {
                    cell = getCell(row,i);
                    if(cell != activeCell && cell->col == i) {
                        int newHeight= cell->frameSet->getFrame(0)->height() +
                            activeCell->frameSet->getFrame(0)->bottom() - coordinate;
                        if(newHeight<minFrameHeight) {
                            if(minFrameHeight-newHeight > postAdjust)
                                postAdjust = minFrameHeight-newHeight;
                        }
                        cell->frameSet->getFrame(0)->setHeight(newHeight);
                    }
                }
            }
            if(postAdjust!=0) {
                for ( unsigned int i = 0; i < cols; i++) {
                    cell = getCell(row,i);
                    if(cell->col == i) cell->frameSet->getFrame(0)->setHeight(
                        cell->frameSet->getFrame(0)->height() + postAdjust);
                }
            }
        }
    }

    // do positioning of frames
    unsigned int y,nextY = getFrameSet( 0, 0 )->getFrame( 0 )->y();
    unsigned int doingPage = getFrameSet(0,0)->getPageOfFrame(0);
    for ( unsigned int j = 0; j < rows; j++ ) {
        y=nextY;
        unsigned int i = 0;
        bool _addRow = false;

        for ( i = 0; i < cols; i++ ) {
            Cell *cell = getCell(j,i);
            if(!(cell->frameSet && cell->frameSet->getFrame(0))) { // sanity check.
                kdDebug() << "screwy table cell!! row:" << cell->row << ", col: " << cell->col << endl;
                continue;
            }
            if(cell->col==i && cell->row==j) { // beware of multi cell frames.
                cell->frameSet->getFrame( 0 )->moveTopLeft( QPoint( cell->frameSet->getFrame( 0 )->x(), y ) );
                cell->frameSet->getFrame( 0 )->setPageNum(doingPage);
            }
            if(cell->row + cell->rows -1 == j)
                nextY=cell->frameSet->getFrame(0) -> bottom() + tableCellSpacing;
        }

        // check all cells on this row if one might have fallen off the page.
        if( j == 0 ) continue;
        unsigned int fromRow=j;
        for(i = 0; i < cols; i++) {
            Cell *cell = getCell(j,i);
            KWFrameSet *fs=cell->frameSet;
            if(cell->row < fromRow) 
                fromRow = cell->row;
            if ( fs->getFrame( 0 )->bottom() >  // fits on page?
                  static_cast<int>((doingPage+1) * doc->getPTPaperHeight() - doc->getPTBottomBorder())) { // no
                y = (doingPage+1) * doc->getPTPaperHeight() + doc->getPTTopBorder();
                _addRow = true;
            }
        }
        if ( _addRow ) {
            j=fromRow;
            doingPage++;

            if ( y >=  doc->getPTPaperHeight() * doc->getPages() )
                doc->appendPage( doc->getPages() - 1 );

            if ( showHeaderOnAllPages ) {
                hasTmpHeaders = true;
                insertRow( j, false, true );
            }
            for(i = 0; i < cols; i++) {
                Cell *cell = getCell (j,i);
                if ( showHeaderOnAllPages ) {
                    KWTextFrameSet *newFrameSet = dynamic_cast<KWTextFrameSet*>( cell->frameSet );
                    KWTextFrameSet *baseFrameSet = dynamic_cast<KWTextFrameSet*>( getFrameSet( 0, i ) );
                    newFrameSet->assign( baseFrameSet );
                    newFrameSet->getFrame(0)->setHeight(baseFrameSet->getFrame(0)->height());
                }
                cell->frameSet->getFrame( 0 )->moveTopLeft( QPoint( cell->frameSet->getFrame( 0 )->x(), y ) );
                cell->frameSet->getFrame( 0 )->setPageNum(doingPage);
                if(cell->row + cell->rows -1 == j) {
                    nextY=cell->frameSet->getFrame(0) -> bottom() + tableCellSpacing;
                }
            }
        }
    }
}

/*================================================================*/
QRect KWGroupManager::getBoundingRect()
{
    QRect r1, r2;
    KWFrame *first = getFrameSet( 0, 0 )->getFrame( 0 );
    assert(first);
    KWFrame *last = getFrameSet( rows - 1, cols - 1 )->getFrame( 0 );
    assert(last);

    r1 = QRect( first->x(), first->y(), first->width(), first->height() );
    r2 = QRect( last->x(), last->y(), last->width(), last->height() );

    r1 = r1.unite( r2 );
    return QRect( r1 );
}

/*================================================================*/
bool KWGroupManager::hasSelectedFrame()
{
    unsigned int a=0,b=0;
    return getFirstSelected(a,b);
}

/*================================================================*/
void KWGroupManager::moveBy( int dx, int dy )
{
    dx = 0; // Ignore the x-offset.
    if(dy==0) return;
    for ( unsigned int i = 0; i < cells.count(); i++ )
        cells.at( i )->frameSet->getFrame( 0 )->moveBy( dx, dy );
}

/*================================================================*/
void KWGroupManager::drawAllRects( QPainter &p, int xOffset, int yOffset )
{
    KWFrame *frame;

    for ( unsigned int i = 0; i < cells.count(); i++ ) {
        frame = cells.at( i )->frameSet->getFrame( 0 );
        p.drawRect( frame->x() - xOffset, frame->y() - yOffset, frame->width(), frame->height() );
    }
}


/*================================================================*/
void KWGroupManager::deselectAll()
{
    for ( unsigned int i = 0; i < cells.count(); i++ )
        cells.at( i )->frameSet->getFrame( 0 )->setSelected( false );
}

/*================================================================*/
/* the selectUntil method will select all frames from the first 
   selected to the frame of the argument frameset.
   The page argument is the page where the resize handles will be
    drawn/erased.
*/
void KWGroupManager::selectUntil( KWFrameSet *fs, KWPage *page ) {
    unsigned int toRow = 0, toCol = 0;
    Cell *cell = getCell(fs);
    toRow=cell->row;
    toCol=cell->col;

    unsigned int fromRow = 0, fromCol = 0;
    getFirstSelected( fromRow, fromCol );

    if ( fromRow > toRow ) { // doSwap
        fromRow = fromRow^toRow;
        toRow = fromRow^toRow;
        fromRow = fromRow^toRow;
    }

    if ( fromCol > toCol ) { // doSwap
        fromCol = fromCol^toCol;
        toCol = fromCol^toCol;
        fromCol = fromCol^toCol;
    }


    for ( unsigned int i = 0; i < cells.count(); i++ ) {
        cell = cells.at(i);
        // check if cell falls completely in square.
        unsigned int row = cell->row + cell->rows -1;
        unsigned int col = cell->col + cell->cols -1;
        if(row >= fromRow && row <= toRow && col >= fromCol && col <= toCol) {
            cell->frameSet->getFrame( 0 )->setSelected( true );
            cell->frameSet->getFrame(0)->createResizeHandlesForPage(page);
            cell->frameSet->getFrame(0)->updateResizeHandles();
        } else {
            cell->frameSet->getFrame( 0 )->setSelected( false );
            cell->frameSet->getFrame(0)->removeResizeHandles();
        }
    }
}

/*================================================================*/
/* Return true if exactly one frame is selected. The parameters row
   and col will receive the values of the active row and col.
   When no frame or more then one frame is selected row and col will
   stay unchanged (and false is returned).
*/
bool KWGroupManager::isOneSelected(unsigned int &row, unsigned int &col) {
    int selectedCell=-1;
    for ( unsigned int i = 0; i < cells.count(); i++ ) {
        if(cells.at(i)->frameSet->getFrame(0)->isSelected())  {
            if(selectedCell==-1)
                selectedCell=i;
            else
                selectedCell=cells.count()+1;
        }
    }
    if(selectedCell>=0 && selectedCell<= static_cast<int> (cells.count())) {
        row=cells.at(selectedCell)->row;
        col=cells.at(selectedCell)->col;
        return true;
    }
    return false;
}

/*================================================================*/
/* returns true if at least one is selected, excluding the argument frameset.
*/
bool KWGroupManager::getFirstSelected( unsigned int &row, unsigned int &col )
{
    for ( unsigned int i = 0; i < cells.count(); i++ ) {
        if (cells.at( i )->frameSet->getFrame( 0 )->isSelected()) {
            row = cells.at( i )->row;
            col = cells.at( i )->col;
            return true;
        }
    }
    return false;
}

/*================================================================*/
void KWGroupManager::insertRow( unsigned int _idx, bool _recalc, bool isAHeader )
{
    unsigned int i = 0;
    unsigned int _rows = rows;

    QValueList<int> colStart;
    QRect r = getBoundingRect();

    for ( i = 0; i < cells.count(); i++ ) {
        Cell *cell = cells.at(i);
        if ( cell->row == 0 ) {
            for( int rowspan=cell->cols; rowspan>0; rowspan--)
                colStart.append(cell->frameSet->getFrame( 0 )->width() / cell->cols );
        }
        if ( cell->row >= _idx ) cell->row++;
    }

    for( unsigned int col = 0; col < colStart.count(); col++) {
        for ( i = 0; i < cells.count(); i++ ) {
            if(cells.at(i)->col == col) {
                colStart[col]=cells.at(i)->frameSet->getFrame(0)->left();
                break;
            }
        }
    }

    colStart.append(r.right());

    QList<KWTextFrameSet> nCells;
    nCells.setAutoDelete( false );

    int ww = 0;
    for ( i = 0; i < getCols(); i++ ) {
        KWFrame *frame = new KWFrame(0L, colStart[i], r.y(), colStart[i+1] - colStart[i], doc->getDefaultParagLayout()->getFormat().getPTFontSize() + 10 );
        frame->setFrameBehaviour(AutoExtendFrame);
        frame->setNewFrameBehaviour(NoFollowup);

        KWTextFrameSet *_frameSet = new KWTextFrameSet( doc );
        _frameSet->setGroupManager( this );
        _frameSet->setIsRemoveableHeader( isAHeader );
        _frameSet->addFrame( frame );

        // If the group is anchored, we must avoid double-application of
        // the anchor offset.
        if ( anchored ) {
            KWFrame *topLeftFrame = _frameSet->getFrame( 0 );

            if (topLeftFrame)
                topLeftFrame->moveBy( -origin.x(), -origin.y() );
        }
        addFrameSet( _frameSet, _idx, i );

        nCells.append( _frameSet );
        ww += colStart[ i ] + 2;

        if(isAHeader) {
            // copy behav from row0
            Cell *cell = getCell(0,i);
            _frameSet->getFrame(0)->setWidth(cell->frameSet->getFrame(0)->width());
            if(cell->cols>1) {
                getCell(_idx,i)->cols= cell->cols;
                i+=getCell(0,i)->cols -1 ;
            }
        }

    }

    rows = ++_rows;

    for ( i = 0; i < nCells.count(); i++ ) {
        KWUnit u;
        u.setMM( 1 );
        doc->addFrameSet( nCells.at( i ) );
        KWFrame *frame = nCells.at( i )->getFrame( 0 );
        frame->setBLeft( u );
        frame->setBRight( u );
        frame->setBTop( u );
        frame->setBBottom( u );

    }


    if ( _recalc )
        recalcRows();
}

/*================================================================*/
void KWGroupManager::insertCol( unsigned int _idx )
{
    unsigned int i = 0;
    unsigned int _cols = cols;

    QList<int> h;
    h.setAutoDelete( true );
    QRect r = getBoundingRect();

    for ( i = 0; i < cells.count(); i++ ) {
        Cell *cell = cells.at(i);
        if ( cell->col == 0 ) 
            for( int colspan=cell->rows; colspan>0; colspan--)
                h.append( new int( cell->frameSet->getFrame( 0 )->height() / cell->rows ) );
        if ( cell->col >= _idx ) cell->col++;
    }

    QList<KWTextFrameSet> nCells;
    nCells.setAutoDelete( false );

    int hh = 0;
    for ( i = 0; i < getRows(); i++ ) {
        KWTextFrameSet *_frameSet = new KWTextFrameSet( doc );
        _frameSet->setGroupManager( this );

        KWFrame *frame = new KWFrame(_frameSet, r.x(), r.y() + hh, 60, *h.at( i ) );
        frame->setFrameBehaviour(AutoExtendFrame);
        _frameSet->addFrame( frame );

        // If the group is anchored, we must avoid double-application of
        // the anchor offset.
        if ( anchored ) {
            KWFrame *topLeftFrame = _frameSet->getFrame( 0 );

            if (topLeftFrame)
                topLeftFrame->moveBy( -origin.x(), -origin.y() );
        }
        addFrameSet( _frameSet, i, _idx );

        nCells.append( _frameSet );
        hh += *h.at( i ) + 2;
    }

    cols = ++_cols;

    for ( i = 0; i < nCells.count(); i++ ) {
        KWUnit u;
        u.setMM( 1 );
        doc->addFrameSet( nCells.at( i ) );
        KWFrame *frame = nCells.at( i )->getFrame( 0 );
        frame->setBLeft( u );
        frame->setBRight( u );
        frame->setBTop( u );
        frame->setBBottom( u );
    }

    recalcCols();
}

/*================================================================*/
/* Delete all cells that are completely in this row.              */
/*================================================================*/

void KWGroupManager::deleteRow( unsigned int row, bool _recalc )
{
    unsigned int height=0;
    unsigned int rowspan=1;
    // I want to know the height of the row(s) I am removing.
    for (unsigned int rowspan=1; rowspan < rows && height==0; rowspan++) {
        for ( unsigned int i = 0; i < cells.count(); i++ ) {
            if(cells.at(i)->row == row && cells.at(i)->rows==rowspan) {
                height=cells.at(i)->frameSet->getFrame(0)->height();
                break;
            }
        }
    }

    // move/delete cells.
    for ( unsigned int i = 0; i < cells.count(); i++ ) {
        Cell *cell = cells.at(i);
        if ( row >= cell->row  && row < cell->row + cell->rows) { // cell is indeed in row
            if(cell->rows == 1) { // lets remove it
                doc->delFrameSet( cells.at( i )->frameSet );
                cells.at( i )->frameSet = 0L;
                cells.remove( i );
                i--;
            } else { // make cell span rowspan less rows
                cell->rows -= rowspan;
                cell->frameSet->getFrame(0)->setHeight( cell->frameSet->getFrame(0)->height() - height - (rowspan -1) * tableCellSpacing);
            }
        } else if ( cell->row > row ) {
            // move cells to the left
            cell->row -= rowspan;
            cell->frameSet->getFrame(0)->moveBy( 0, -height);
        }
    }
    rows -= rowspan;

    if ( _recalc )
        recalcRows();
}

/*================================================================*/
/* Delete all cells that are completely in this col.              */
/*================================================================*/
void KWGroupManager::deleteCol( unsigned int col )
{
    unsigned int width=0;
    unsigned int colspan=1;
    // I want to know the width of the col(s) I am removing.
    for (unsigned int colspan=1; colspan < cols && width==0; colspan++) {
        for ( unsigned int i = 0; i < cells.count(); i++ ) {
            if(cells.at(i)->col == col && cells.at(i)->cols==colspan) {
                width=cells.at(i)->frameSet->getFrame(0)->width();
                break;
            }
        }
    }

    // move/delete cells.
    for ( unsigned int i = 0; i < cells.count(); i++ ) {
        Cell *cell = cells.at(i);
        if ( col >= cell->col  && col < cell->col + cell->cols) { // cell is indeed in col
            if(cell->cols == 1) { // lets remove it
                doc->delFrameSet( cells.at( i )->frameSet );
                cells.at( i )->frameSet = 0L;
                cells.remove( i );
                i--;
            } else { // make cell span colspan less cols
                cell->cols -= colspan;
                cell->frameSet->getFrame(0)->setWidth(
                        cell->frameSet->getFrame(0)->width() - width - (colspan-1) * tableCellSpacing);
            }
        } else if ( cell->col > col ) {
            // move cells to the left
            cell->col -= colspan;
            cell->frameSet->getFrame(0)->moveBy( -width, 0);
        }
    }
    cols -= colspan;

    recalcCols();
}

/*================================================================*/
void KWGroupManager::updateTempHeaders()
{
    if ( !hasTmpHeaders ) return;

    for ( unsigned int i = 1; i < rows; i++ ) {
        for ( unsigned int j = 0; j < cols; j++ ) {
            KWFrameSet *fs = getFrameSet( i, j );
            if ( fs->isRemoveableHeader() ) {
                dynamic_cast<KWTextFrameSet*>( fs )->assign( dynamic_cast<KWTextFrameSet*>( getFrameSet( 0, j ) ) );

                QPainter p;
                QPicture pic;
                p.begin( &pic );

                KWFormatContext fc( doc, doc->getFrameSetNum( fs ) + 1 );
                fc.init( dynamic_cast<KWTextFrameSet*>( fs )->getFirstParag(), true );

                bool bend = false;
                while ( !bend )
                    bend = !fc.makeNextLineLayout();

                p.end();
            }
        }
    }
}

/*================================================================*/
void KWGroupManager::ungroup()
{
    for ( unsigned int i = 0; i < cells.count(); i++ )
        cells.at( i )->frameSet->setGroupManager( 0L );

    cells.setAutoDelete( false );
    cells.clear();

    active = false;
}

/*================================================================*/
bool KWGroupManager::joinCells() {
    unsigned int colBegin, rowBegin, colEnd,rowEnd;
    if ( !getFirstSelected( rowBegin, colBegin ) ) return false;
    Cell *firstCell = getCell(rowBegin, colBegin);
    colEnd=colBegin+firstCell->cols-1;
    rowEnd=rowBegin+firstCell->rows-1;

    while(colEnd+1 <getCols()) { // count all horizontal selected cells
        Cell *cell = getCell(rowEnd,colEnd+1);
        if(cell->frameSet->getFrame(0)->isSelected()) {
            colEnd+=cell->cols;
        } else
            break;
    }

    while(rowEnd+1 < getRows()) { // count all vertical selected cells
        Cell *cell = getCell(rowEnd+1, colBegin);
        if(cell->frameSet->getFrame(0)->isSelected()) {
            for(unsigned int j=1; j <= cell->rows; j++) {
                for(unsigned int i=colBegin; i<=colEnd; i++) {
                    if(! getCell(rowEnd+j,i)->frameSet->getFrame(0)->isSelected())
                        return false; // can't use this selection..
                }
            }
            rowEnd+=cell->rows;
        } else
            break;
    }
    // if just one cell selected for joining; exit.
    if(rowBegin == rowEnd && colBegin == colEnd ||
            getCell(rowBegin,colBegin) == getCell(rowEnd,colEnd))
        return false;

    int bottom=getCell(rowEnd, colBegin)->frameSet->getFrame(0)->bottom();
    int right=getCell(rowEnd, colEnd)->frameSet->getFrame(0)->right();

    // do the actual merge.
    for(unsigned int i=colBegin; i<=colEnd;i++) {
        for(unsigned int j=rowBegin; j<=rowEnd;j++) {
            Cell *cell = getCell(j,i);
            if(cell && cell!=firstCell) {
                doc->delFrameSet( cell->frameSet );
                cell->frameSet = 0L;
                cells.remove(cell);
            }
        }
    }

    // update  firstcell properties te reflect the merge
    firstCell->cols=colEnd-colBegin+1;
    firstCell->rows=rowEnd-rowBegin+1;
    firstCell->frameSet->getFrame(0)->setRight(right);
    firstCell->frameSet->getFrame(0)->setBottom(bottom);
    firstCell->frameSet->getFrame(0)->updateResizeHandles();

    recalcCols();
    recalcRows();

    return true;
}

/*================================================================*/
bool KWGroupManager::splitCell(unsigned int intoRows, unsigned int intoCols)
{

/* This is not finished. Joining vertically merged cells crashes..
   Thomas
*/
    return false;
    if(intoRows < 1 || intoCols < 1) return false; // assertion.

    unsigned int col, row;
    if ( !isOneSelected( row, col ) ) return false;

    Cell *cell=getCell(row,col);
    KWFrame *firstFrame = cell->frameSet->getFrame(0);
    unsigned int height = firstFrame->height() / intoRows -  tableCellSpacing;
    unsigned int width = firstFrame->width() / intoCols -  tableCellSpacing;
kdDebug() << "width: " << firstFrame->width() << ", " << intoCols << " = " << width << endl;

kdDebug() << "splitting cell: " << row << ", " << col << " into " << intoRows << ", " << intoCols << endl;

    // will it fit?
    if(height < minFrameHeight) return false;
    if(width < minFrameWidth) return false;

    int newRows = intoRows-cell->rows;
    int newCols = intoCols-cell->cols;

    // adjust cellspan and rowspan on other cells.
    for (unsigned int i=0; i< cells.count() ; i++) {
        Cell *theCell = cells.at(i);
        if(cell == theCell) continue;

kdDebug() << "1: cell: " << theCell->row << ", " << theCell->col << "  size: " << theCell->rows << ", " << theCell->cols << endl;
        if(newRows>0) {
            if(row >= theCell->row && row < theCell->row + theCell->rows)
                theCell->rows+=newRows;
            if(theCell->row > row) theCell->row+=newRows;
        }
        if(newCols>0) {
            if(col >= theCell->col && col < theCell->col + theCell->cols)
                theCell->cols+=newCols;
            if(theCell->col > col) theCell->col+=newCols;
        }
kdDebug() << "2: cell: " << theCell->row << ", " << theCell->col << "  size: " << theCell->rows << ", " << theCell->cols << endl;
    }
   
    firstFrame->setWidth(width);
    firstFrame->setHeight(height); 
    cell->rows = cell->rows - intoRows +1;
    if(cell->rows < 1)  cell->rows=1;
    cell->cols = cell->cols - intoCols +1;
    if(cell->cols < 1)  cell->cols=1;

kdDebug() << "orig cell is now at: (" << cell->row << "," << cell->col << " size " << cell->rows << "," << cell->cols << endl;

    // create new cells
    for (unsigned int y = 0; y < intoRows; y++) {
        for (unsigned int x = 0; x < intoCols; x++){
            if(x==0 && y==0) continue; // the orig cell takes this spot.

            Cell *newCell = new Cell;

            KWTextFrameSet *_frameSet = new KWTextFrameSet( doc );
            _frameSet->setName(QString("split cell"));
            _frameSet->setGroupManager( this );
kdDebug() << "y: " << y << " x: " << x << endl;
kdDebug() << "x: " << firstFrame->left() + width * x << " y: " << firstFrame->top() + height * y << 
            ", width: "<< width << ", height: " << height << endl;

            KWFrame *frame = new KWFrame(_frameSet, 
                    firstFrame->left() + (width+tableCellSpacing) * x, 
                    firstFrame->top() + (height+tableCellSpacing) * y, 
                    width, height);
            frame->setFrameBehaviour(AutoExtendFrame);
            frame->setNewFrameBehaviour(NoFollowup);
            _frameSet->addFrame( frame );
            if ( anchored ) { // is this needed?
                KWFrame *topLeftFrame = _frameSet->getFrame( 0 );
     
                if (topLeftFrame)
                    topLeftFrame->moveBy( -origin.x(), -origin.y() );
            } 
            doc->addFrameSet(_frameSet);

            newCell->frameSet = _frameSet;
            newCell->row = y + row;
            newCell->col = x + col;
            newCell->rows = 1;
            newCell->cols = 1;

            // if the orig cell spans more rows/cols than it is split into, make first col/row wider.
            if(newRows <0 && x==0)
                newCell->rows -=newRows;
            if(newCols <0 && y==0)
                newCell->cols -=newCols;

kdDebug() << "creating cell: " << newCell->row << ", " << newCell->col << " width: " << newCell->rows << ", " << newCell->cols << endl;

            unsigned int i;
            for (i = 0; i < cells.count() && (cells.at(i)->row < row || 
                cells.at(i)->row == row  && cells.at(i)->col < col) ; i++ ); 
            cells.insert(i, newCell );
        }
    } 
    rows+= newRows;
    cols+= newCols;
    recalcCols();

    return true;
}

/*================================================================*/
QString KWGroupManager::anchorType()
{
    return "grpMgr";
}

QString KWGroupManager::anchorInstance()
{
    return name;
}

/*================================================================*/
void KWGroupManager::viewFormatting( QPainter &painter, int )
{
    KWFrame *topLeftFrame;

    // If we have been populated, then draw a line from the origin to the
    // top left corner.
    if ( cells.count() > 0 )
    {
        topLeftFrame = cells.at( 0 )->frameSet->getFrame( 0 );
        painter.drawLine( origin.x(), origin.y(), topLeftFrame->x(), topLeftFrame->y());
    }
}

/*================================================================*/
/* checks the cells for missing cells or duplicates, will correct
   mistakes. 
*/
void KWGroupManager::validate()
{
    for (unsigned int j=0; j < getNumCells() ; j++) {
        KWFrame *frame = getCell(j)->frameSet->getFrame(0);
        if(frame->getFrameBehaviour()==AutoCreateNewFrame) {
            frame->setFrameBehaviour(AutoExtendFrame);
            kdDebug() << "Table cell property frameBehaviour was incorrect; fixed" << endl;
        }
        if(frame->getNewFrameBehaviour()!=NoFollowup) {
            kdDebug() << "Table cell property newFrameBehaviour was incorrect; fixed" << endl;
            frame->setNewFrameBehaviour(NoFollowup);
        }
    }

    QList<Cell> misplacedCells;

    for(unsigned int row=0; row < getRows(); row++) {
        for(unsigned int col=0; col <getCols(); col++) {
            bool found=false;
            for ( unsigned int i = 0; i < cells.count(); i++ ) {
                if ( cells.at( i )->row <= row &&
                        cells.at( i )->col <= col &&
                        cells.at( i )->row+cells.at( i )->rows > row &&
                        cells.at( i )->col+cells.at( i )->cols > col ) {
                    if(found==true) {
                        kdDebug() << "Found duplicate cell, (" << cells.at(i)->row << ", " << cells.at(i)->col << ") moving one out of the way" << endl;
                        misplacedCells.append(cells.take(i--));
                    }
                    found=true;
                }
            }
            if(! found) { 
                kdDebug() << "Missing cell, creating a new one; ("<< row << "," << col<<")" << endl;
                Cell *cell = new Cell;
                KWTextFrameSet *_frameSet = new KWTextFrameSet( doc );
                _frameSet->setName(QString("Auto added cell"));
                _frameSet->setGroupManager( this );
                int x=-1, y=-1, width=-1, height=-1;
                for (unsigned int i=0; i < cells.count(); i++) {
                    if(cells.at(i)->row==row)
                        y=cells.at(i)->frameSet->getFrame(0)->y();
                    if(cells.at(i)->col==col)
                        x=cells.at(i)->frameSet->getFrame(0)->x();
                    if(cells.at(i)->col==col && cells.at(i)->cols==1)
                        width=cells.at(i)->frameSet->getFrame(0)->width();
                    if(cells.at(i)->row==row && cells.at(i)->rows==1)
                        height=cells.at(i)->frameSet->getFrame(0)->height();
                    if(x!=-1 && y!=-1 && width!=-1 && height != -1) 
                        break;
                }
                if(x== -1) x=0;
                if(y== -1) y=0;
                if(width== -1) width=minFrameWidth;
                if(height== -1) height=minFrameHeight;
                kdDebug() << " x: " << x << ", y:" << y << ", width: " << width << ", height: " << height << endl;
                KWFrame *frame = new KWFrame(_frameSet, x, y, width, height );
                frame->setFrameBehaviour(AutoExtendFrame);
                frame->setNewFrameBehaviour(NoFollowup);
                _frameSet->addFrame( frame );
                if ( anchored ) {
                    KWFrame *topLeftFrame = _frameSet->getFrame( 0 );
         
                    if (topLeftFrame)
                        topLeftFrame->moveBy( -origin.x(), -origin.y() );
                }
                doc->addFrameSet(_frameSet);

                cell->frameSet = _frameSet;
                cell->row = row;
                cell->col = col;
                cell->rows = 1;
                cell->cols = 1;

                unsigned int i;
                for (i = 0; i < cells.count() && (cells.at(i)->row < row || 
                    cells.at(i)->row == row  && cells.at(i)->col < col) ; i++ ); 
                cells.insert(i, cell );
            }
        }
    }
    unsigned int bottom = getCell(rows-1,0)->frameSet->getFrame(0)->bottom();
    while (! misplacedCells.isEmpty()) {
        // append cell at botom of table.
        Cell *cell = misplacedCells.take(0);
        cell->frameSet->getFrame(0)->setWidth(getBoundingRect().width());
        cell->frameSet->getFrame(0)->moveBy( getBoundingRect().left() -
            cell->frameSet->getFrame(0)->left(),
            bottom - cell->frameSet->getFrame(0)->top() - tableCellSpacing);
        cell->row=rows++;
        cell->col=0;
        cell->cols=cols;
        cell->rows=1;
        bottom=cell->frameSet->getFrame(0)->bottom();
        cells.append(cell);
    }
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
