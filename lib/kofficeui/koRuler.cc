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

// Description: Ruler (header)

/******************************************************************/

#include "koRuler.h"
#include <klocale.h>
#include <kdebug.h>
#include <kiconloader.h>
#include <qcursor.h>
#include <qpainter.h>
#include <qpopupmenu.h>
#include <koUnit.h>

class KoRulerPrivate {
public:
    KoRulerPrivate() {
    }
    ~KoRulerPrivate() {}

    QWidget *canvas;
    int flags;
    int oldMx, oldMy;
    bool mousePressed;
    KoRuler::Action action;
    bool whileMovingBorderLeft, whileMovingBorderRight;
    bool whileMovingBorderTop, whileMovingBorderBottom;
    bool rtl;
    QPixmap pmFirst, pmLeft;
    KoTabChooser *tabChooser;
    KoTabulatorList tabList;
    KoTabulatorList::Iterator removeTab;     // Do we have to remove a certain tab in the DC Event?
    KoTabulatorList::Iterator currTab;
    QPopupMenu *rb_menu;
    int mMM, mPT, mINCH, mRemoveTab, mPageLayout; // menu item ids
    int frameEnd;
    double i_right;
    bool m_bReadWrite;
};


/******************************************************************/
/* Class: KoRuler                                                 */
/******************************************************************/

const int KoRuler::F_TABS = 1;
const int KoRuler::F_INDENTS = 2;
const int KoRuler::F_HELPLINES = 4;
const int KoRuler::F_NORESIZE = 8;

/*================================================================*/
KoRuler::KoRuler( QWidget *_parent, QWidget *_canvas, Orientation _orientation,
                 const KoPageLayout& _layout, int _flags, KoUnit::Unit _unit, KoTabChooser *_tabChooser )
    : QFrame( _parent ), buffer( width(), height() ), m_zoom(1.0), m_1_zoom(1.0),
      m_unit( _unit )
{
    setWFlags( WResizeNoErase | WRepaintNoErase );
    setFrameStyle( Box | Raised );

    d=new KoRulerPrivate();

    d->tabChooser = _tabChooser;

    d->canvas = _canvas;
    orientation = _orientation;
    layout = _layout;
    d->flags = _flags;

    d->m_bReadWrite=true;
    diffx = 0;
    diffy = 0;
    i_left=0.0;
    i_first=0.0;
    d->i_right=0.0;

    setMouseTracking( true );
    d->mousePressed = false;
    d->action = A_NONE;

    d->oldMx = 0;
    d->oldMy = 0;
    d->rtl = false;

    showMPos = false;
    mposX = 0;
    mposY = 0;
    gridSize=0.0;
    hasToDelete = false;
    d->whileMovingBorderLeft = d->whileMovingBorderRight = d->whileMovingBorderTop = d->whileMovingBorderBottom = false;

    d->pmFirst = UserIcon( "koRulerFirst" );
    d->pmLeft = UserIcon( "koRulerLeft" );
    d->currTab = d->tabList.end();

    d->removeTab = d->tabList.end();
    if ( orientation == Qt::Horizontal ) {
        frameStart = qRound( zoomIt(layout.ptLeft) );
        d->frameEnd = qRound( zoomIt(layout.ptWidth - layout.ptRight) );
    } else {
        frameStart = qRound( zoomIt(layout.ptTop) );
        d->frameEnd = qRound( zoomIt(layout.ptHeight - layout.ptBottom) );
    }
    m_bFrameStartSet = false;

    setupMenu();

    // For compatibility, emitting doubleClicked shall emit openPageLayoutDia
    connect( this, SIGNAL( doubleClicked() ), this, SIGNAL( openPageLayoutDia() ) );
}

/*================================================================*/
KoRuler::~KoRuler()
{
    delete d->rb_menu;
    delete d;
}

void KoRuler::setPageLayoutMenuItemEnabled(bool b)
{
    d->rb_menu->setItemEnabled(d->mPageLayout, b);
}

/*================================================================*/
void KoRuler::setMousePos( int mx, int my )
{
    if ( !showMPos || ( mx == mposX && my == mposY ) ) return;

    QPainter p( this );
    p.setRasterOp( Qt::NotROP );

    if ( orientation == Qt::Horizontal ) {
        if ( hasToDelete )
            p.drawLine( mposX, 1, mposX, height() - 1 );
        p.drawLine( mx, 1, mx, height() - 1 );
        hasToDelete = true;
    }
    else {
        if ( hasToDelete )
            p.drawLine( 1, mposY, width() - 1, mposY );
        p.drawLine( 1, my, width() - 1, my );
        hasToDelete = true;
    }
    p.end();

    mposX = mx;
    mposY = my;
}

/*================================================================*/
void KoRuler::drawHorizontal( QPainter *_painter )
{
    // Use a double-buffer pixmap
    QPainter p( &buffer );
    p.fillRect( 0, 0, width(), height(), QBrush( colorGroup().brush( QColorGroup::Background ) ) );

    double dist=0.0;
    int totalw = qRound( zoomIt(layout.ptWidth) );
    QString str;
    QFont font; // Use the global KDE font. Let's hope it's appropriate.
    font.setPointSize( 8 );
    QFontMetrics fm( font );

    p.setBrush( colorGroup().brush( QColorGroup::Base ) );

    // Draw white rect
    QRect r;
    if ( !d->whileMovingBorderLeft )
        r.setLeft( -diffx + frameStart );
    else
        r.setLeft( d->oldMx );
    r.setTop( 0 );
    if ( !d->whileMovingBorderRight )
        r.setWidth(d->frameEnd-frameStart);
    else
        r.setRight( d->oldMx );
    r.setBottom( height() );

    p.drawRect( r );
    p.setFont( font );

    // Draw the numbers
    switch( m_unit ) {
    case KoUnit::U_INCH:
        dist = INCH_TO_POINT (m_zoom);
        break;
    case KoUnit::U_PT:
        dist = 100.0 * m_zoom;
        break;
    case KoUnit::U_MM:
        dist = MM_TO_POINT ( 10.0 * m_zoom );
        break;
    }

    int j = 0;
    int maxwidth = 0;
    for ( double i = 0.0;i <= (double)totalw;i += dist ) {
        str=QString::number(j++);
        if ( m_unit == KoUnit::U_PT && j!=1)
            str+="00";
        int textwidth = fm.width( str );
        p.drawText( qRound(i) - diffx - qRound(textwidth * 0.5),
                    qRound(( height() - fm.height() ) * 0.5),
                    textwidth, height(), AlignLeft | AlignTop, str );
        maxwidth = QMAX( maxwidth, textwidth );
    }

    // Draw the medium-sized lines
    // Only if we have enough space (i.e. not at 33%)
    if ( dist > maxwidth + 1 )
    {
        for ( double i = dist * 0.5;i <= (double)totalw;i += dist ) {
            int ii=qRound(i);
            p.drawLine( ii - diffx, 5, ii - diffx, height() - 5 );
        }
    }

    // Draw the small lines
    // Only if we have enough space (i.e. not at 33%)
    if ( dist * 0.5 > maxwidth + 1 )
    {
        for ( double i = dist * 0.25;i <= (double)totalw;i += dist * 0.5 ) {
            int ii=qRound(i);
            p.drawLine( ii - diffx, 7, ii - diffx, height() - 7 );
        }
    }

    // Draw ending bar (at page width)
    int constant=zoomIt(1);
    p.drawLine( totalw - diffx + constant, 1, totalw - diffx + constant, height() - 1 );
    p.setPen( colorGroup().color( QColorGroup::Base ) );
    p.drawLine( totalw - diffx, 1, totalw - diffx, height() - 1 );

    // Draw starting bar (at 0)
    p.setPen( colorGroup().color( QColorGroup::Text ) );
    p.drawLine( -diffx, 1, -diffx, height() - 1 );
    p.setPen( colorGroup().color( QColorGroup::Base ) );
    p.drawLine( -diffx - constant, 1, -diffx - constant, height() - 1 );

    // Draw the indents triangles
    if ( d->flags & F_INDENTS ) {
        int top = 2;
        p.drawPixmap( qRound(applyRtlAndZoom(i_first) - d->pmFirst.width() * 0.5 +
                                 static_cast<double>(r.left())), top, d->pmFirst );
        int bottom = height() - d->pmLeft.height() - 2;
        p.drawPixmap( qRound(applyRtlAndZoom(i_left) - d->pmLeft.width() * 0.5 +
                             static_cast<double>(r.left())), bottom, d->pmLeft );
        p.drawPixmap( qRound(static_cast<double>(r.right()) - applyRtlAndZoom(d->i_right)
                             - d->pmLeft.width() * 0.5 ), bottom, d->pmLeft );
    }

    // Show the mouse position
    if ( d->action == A_NONE && showMPos ) {
        p.setPen( colorGroup().color( QColorGroup::Text ) );
        p.drawLine( mposX, 1, mposX, height() - 1 );
    }
    hasToDelete = false;

    // Draw the tabs
    if ( d->tabChooser && ( d->flags & F_TABS ) && !d->tabList.isEmpty() )
        drawTabs( p );

    p.end();
    _painter->drawPixmap( 0, 0, buffer );
}

/*================================================================*/
void KoRuler::drawTabs( QPainter &_painter )
{
    int ptPos = 0;

    _painter.setPen( QPen( colorGroup().color( QColorGroup::Text ), 2, SolidLine ) );

    KoTabulatorList::Iterator it = d->tabList.begin();
    for ( ; it != d->tabList.end() ; it++ ) {
        ptPos = qRound(applyRtlAndZoom((*it).ptPos)) - diffx + frameStart;
        switch ( (*it).type ) {
        case T_LEFT: {
            ptPos -= 4;
            _painter.drawLine( ptPos + 4, height() - 4, ptPos + 20 - 4, height() - 4 );
            _painter.drawLine( ptPos + 5, 4, ptPos + 5, height() - 4 );
        } break;
        case T_CENTER: {
            ptPos -= 10;
            _painter.drawLine( ptPos + 4, height() - 4, ptPos + 20 - 4, height() - 4 );
            _painter.drawLine( ptPos + 20 / 2, 4, ptPos + 20 / 2, height() - 4 );
        } break;
        case T_RIGHT: {
            ptPos -= 16;
            _painter.drawLine( ptPos + 4, height() - 4, ptPos + 20 - 4, height() - 4 );
            _painter.drawLine( ptPos + 20 - 5, 4, ptPos + 20 - 5, height() - 4 );
        } break;
        case T_DEC_PNT: {
            ptPos -= 10;
            _painter.drawLine( ptPos + 4, height() - 4, ptPos + 20 - 4, height() - 4 );
            _painter.drawLine( ptPos + 20 / 2, 4, ptPos + 20 / 2, height() - 4 );
            _painter.fillRect( ptPos + 20 / 2 + 2, height() - 9, 3, 3,
                               colorGroup().color( QColorGroup::Text ) );
        } break;
        default: break;
        }
    }
}

/*================================================================*/
void KoRuler::drawVertical( QPainter *_painter )
{
    QPainter p( &buffer );
    p.fillRect( 0, 0, width(), height(), QBrush( colorGroup().brush( QColorGroup::Background ) ) );

    double dist=0.0;
    int j = 0;
    int totalh = qRound( zoomIt(layout.ptHeight) );
    QString str;
    QFont font; // Use the global KDE font. Let's hope it's appropriate.
    font.setPointSize( 8 ); // Hardcode the size? (Werner)
    QFontMetrics fm( font );

    p.setBrush( colorGroup().brush( QColorGroup::Base ) );

    // Draw white rect
    QRect r;
    if ( !d->whileMovingBorderTop )
        r.setTop( -diffy + frameStart );
    else
        r.setTop( d->oldMy );
    r.setLeft( 0 );
    if ( !d->whileMovingBorderBottom )
        r.setHeight(d->frameEnd-frameStart);
    else
        r.setBottom( d->oldMy );
    r.setRight( width() );

    p.drawRect( r );
    p.setFont( font );

    // Draw the numbers
    switch( m_unit ) {
    case KoUnit::U_INCH:
        dist = INCH_TO_POINT ( m_zoom );
        break;
    case KoUnit::U_PT:
        dist = 100.0 * m_zoom;
        break;
    case KoUnit::U_MM:
        dist = MM_TO_POINT ( 10.0 * m_zoom );
        break;
    }

    int maxheight = 0;
    for ( double i = 0.0;i <= (double)totalh;i += dist ) {
        str=QString::number(j++);
        if ( m_unit == KoUnit::U_PT && j!=1 )
            str+="00";
        int textheight = fm.height();
        maxheight = QMAX( maxheight, textheight );
        p.drawText( qRound(( width() - fm.width( str ) ) * 0.5),
                    qRound(i) - diffy - qRound(textheight * 0.5),
                    width(), textheight, AlignLeft | AlignTop, str );
    }

    // Draw the medium-sized lines
    if ( dist > maxheight + 1 )
    {
        for ( double i = dist * 0.5;i <= (double)totalh;i += dist ) {
            int ii=qRound(i);
            p.drawLine( 5, ii - diffy, width() - 5, ii - diffy );
        }
    }

    // Draw the small lines
    if ( dist * 0.5 > maxheight + 1 )
    {
        for ( double i = dist * 0.25;i <=(double)totalh;i += dist *0.5 ) {
            int ii=qRound(i);
            p.drawLine( 7, ii - diffy, width() - 7, ii - diffy );
        }
    }

    // Draw ending bar (at page height)
    p.drawLine( 1, totalh - diffy + 1, width() - 1, totalh - diffy + 1 );
    p.setPen( colorGroup().color( QColorGroup::Base ) );
    p.drawLine( 1, totalh - diffy, width() - 1, totalh - diffy );

    // Draw starting bar (at 0)
    p.setPen( colorGroup().color( QColorGroup::Text ) );
    p.drawLine( 1, -diffy, width() - 1, -diffy );
    p.setPen( colorGroup().color( QColorGroup::Base ) );
    p.drawLine( 1, -diffy - 1, width() - 1, -diffy - 1 );

    // Show the mouse position
    if ( d->action == A_NONE && showMPos ) {
        p.setPen( colorGroup().color( QColorGroup::Text ) );
        p.drawLine( 1, mposY, width() - 1, mposY );
    }
    hasToDelete = false;

    p.end();
    _painter->drawPixmap( 0, 0, buffer );
}

void KoRuler::mousePressEvent( QMouseEvent *e )
{
    if( !d->m_bReadWrite)
        return;

    d->oldMx = e->x();
    d->oldMy = e->y();
    d->mousePressed = true;
    d->removeTab=d->tabList.end();

    switch ( e->button() ) {
    case RightButton:
        if(d->currTab==d->tabList.end() || !(d->flags & F_TABS))
            d->rb_menu->setItemEnabled(d->mRemoveTab, false);
        else
            d->rb_menu->setItemEnabled(d->mRemoveTab, true);
        d->rb_menu->popup( QCursor::pos() );
        d->action = A_NONE;
        d->mousePressed = false;
        return;
    case MidButton:
        // MMB shall do like double-click (it opens a dialog).
        handleDoubleClick();
        return;
    case LeftButton:
        if ( d->action == A_BR_RIGHT || d->action == A_BR_LEFT ) {
            if ( d->action == A_BR_RIGHT )
                d->whileMovingBorderRight = true;
            else
                d->whileMovingBorderLeft = true;

            if ( d->canvas )
                drawLine(d->oldMx, -1);
            update();
        } else if ( d->action == A_BR_TOP || d->action == A_BR_BOTTOM ) {
            if ( d->action == A_BR_TOP )
                d->whileMovingBorderTop = true;
            else
                d->whileMovingBorderBottom = true;

            if ( d->canvas ) {
                QPainter p( d->canvas );
                p.setRasterOp( Qt::NotROP );
                p.drawLine( 0, d->oldMy, d->canvas->width(), d->oldMy );
                p.end();
            }
            update();
        } else if ( d->action == A_FIRST_INDENT || d->action == A_LEFT_INDENT || d->action == A_RIGHT_INDENT ) {
            if ( d->canvas )
                drawLine(d->oldMx, -1);
        } else if ( d->action == A_TAB ) {
            if ( d->canvas && d->currTab!=d->tabList.end()) {
                drawLine( qRound( applyRtlAndZoom((*d->currTab).ptPos) ) + frameStart - diffx, -1 );
            }
        } else if ( d->tabChooser && ( d->flags & F_TABS ) && d->tabChooser->getCurrTabType() != 0 ) {
            int left = frameStart - diffx;
            int right = d->frameEnd - diffx;

            if( e->x()-left < 0 || right-e->x() < 0 )
                return;
            KoTabulator tab;
            tab.filling = TF_BLANK;
            tab.ptWidth = 0.5;
            switch ( d->tabChooser->getCurrTabType() ) {
            case KoTabChooser::TAB_LEFT:
                tab.type = T_LEFT;
                break;
            case KoTabChooser::TAB_CENTER:
                tab.type = T_CENTER;
                break;
            case KoTabChooser::TAB_RIGHT:
                tab.type = T_RIGHT;
                break;
            case KoTabChooser::TAB_DEC_PNT:
                tab.type = T_DEC_PNT;
                break;
            default: break;
            }
            tab.ptPos = unZoomItRtl( e->x() + diffx - frameStart );

            KoTabulatorList::Iterator it=d->tabList.begin();
            while ( it!=d->tabList.end() && tab > (*it) )
		++it;

            d->removeTab=d->tabList.insert(it, tab);

            d->action=A_TAB;
            d->currTab=d->removeTab;

            emit tabListChanged( d->tabList );
            update();
        }
        else if ( d->flags & F_HELPLINES )
        {
	    setCursor( orientation == Qt::Horizontal ?
		       Qt::sizeVerCursor : Qt::sizeHorCursor );
            d->action=A_HELPLINES;
        }
    default:
        break;
    }
}

void KoRuler::mouseReleaseEvent( QMouseEvent *e )
{
    d->mousePressed = false;

    // Hacky, but necessary to prevent multiple tabs with the same coordinates (Werner)
    bool fakeMovement=false;
    if(d->removeTab!=d->tabList.end()) {
        mouseMoveEvent(e);
        fakeMovement=true;
    }

    if ( d->action == A_BR_RIGHT || d->action == A_BR_LEFT ) {
        d->whileMovingBorderRight = false;
        d->whileMovingBorderLeft = false;

        if ( d->canvas )
            drawLine(d->oldMx, -1);
        update();
        emit newPageLayout( layout );
    } else if ( d->action == A_BR_TOP || d->action == A_BR_BOTTOM ) {
        d->whileMovingBorderTop = false;
        d->whileMovingBorderBottom = false;

        if ( d->canvas ) {
            QPainter p( d->canvas );
            p.setRasterOp( Qt::NotROP );
            p.drawLine( 0, d->oldMy, d->canvas->width(), d->oldMy );
            p.end();
        }
        update();
        emit newPageLayout( layout );
    } else if ( d->action == A_FIRST_INDENT ) {
        if ( d->canvas )
            drawLine(d->oldMx, -1);
        update();
        emit newFirstIndent( i_first );
    } else if ( d->action == A_LEFT_INDENT ) {
        if ( d->canvas )
            drawLine(d->oldMx, -1);
        update();
        double _tmp = i_first;
        emit newLeftIndent( i_left );
        i_first = _tmp;
        emit newFirstIndent( i_first );
    } else if ( d->action == A_RIGHT_INDENT ) {
        if ( d->canvas )
            drawLine(d->oldMx, -1);
        update();
        emit newRightIndent( d->i_right );
    } else if ( d->action == A_TAB ) {
        if ( d->canvas && !fakeMovement ) {
            drawLine( qRound( applyRtlAndZoom( (*d->currTab).ptPos ) ) + frameStart - diffx, -1);
        }
        if ( (e->y() < -50 || e->y() > height() + 50) && d->currTab!=d->tabList.end() )
            d->tabList.remove(d->currTab);

        qHeapSort( d->tabList );

        // Delete the new tabulator if it is placed on top of another.
        KoTabulatorList::iterator tmpTab=d->tabList.begin();
        int count=0;
        while(tmpTab!=d->tabList.end()) {
            if((*(tmpTab)).ptPos ==  (*(d->currTab)).ptPos) {
                count++;
                if(count > 1) {
                    d->tabList.remove(d->currTab);
                    break;
                }
            }
            tmpTab++;
        }
        searchTab( e->x() );
        emit tabListChanged( d->tabList );
        update();
    }
    else if( d->action == A_HELPLINES )
    {
        emit addHelpline( e->pos(), orientation == Qt::Horizontal);
        setCursor( ArrowCursor );
    }
}

void KoRuler::mouseMoveEvent( QMouseEvent *e )
{
    hasToDelete = false;

    int pw = d->frameEnd - frameStart;
    int ph = qRound(zoomIt(layout.ptHeight));
    int left = frameStart - diffx;
    int top = qRound(zoomIt(layout.ptTop));
    top -= diffy;
    int right = d->frameEnd - diffx;
    int bottom = qRound(zoomIt(layout.ptBottom));
    bottom = ph - bottom - diffy;
    int ip_left = qRound(applyRtlAndZoom(i_left));
    int ip_first = qRound(applyRtlAndZoom(i_first));
    int ip_right = qRound(applyRtlAndZoom(d->i_right));

    int mx = e->x();
    mx = mx+diffx < 0 ? 0 : mx;
    int my = e->y();
    my = my+diffy < 0 ? 0 : my;

    switch ( orientation ) {
        case Qt::Horizontal: {
            if ( !d->mousePressed ) {
                setCursor( ArrowCursor );
                d->action = A_NONE;
                /////// ######
                // At the moment, moving the left and right border indicators
                // is disabled when setFrameStartEnd has been called (i.e. in KWord)
                // Changing the layout margins directly from it would be utterly wrong
                // (just try the 2-columns modes...). What needs to be done is:
                // emitting a signal frameResized in mouseReleaseEvent, when a left/right
                // border has been moved, and in kword we need to update the margins from
                // there, if the left border of the 1st column or the right border of the
                // last column was moved... and find what to do with the other borders.
                // And for normal frames, resize the frame without touching the page layout.
                // All that is too much for now -> disabling.
                if ( !m_bFrameStartSet )
                {
                    if ( mx > left - 5 && mx < left + 5 ) {
                        setCursor( Qt::sizeHorCursor );
                        d->action = A_BR_LEFT;
                    } else if ( mx > right - 5 && mx < right + 5 ) {
                        setCursor( Qt::sizeHorCursor );
                        d->action = A_BR_RIGHT;
                    }
                }
                if ( d->flags & F_INDENTS ) {
                    if ( mx > left + ip_first - 5 && mx < left + ip_first + 5 &&
                         my >= 2 && my <= d->pmFirst.size().height() + 2 ) {
                        setCursor( ArrowCursor );
                        d->action = A_FIRST_INDENT;
                    } else if ( mx > left + ip_left - 5 && mx < left + ip_left + 5 &&
                                my >= height() - d->pmLeft.size().height() - 2 && my <= height() - 2 ) {
                        setCursor( ArrowCursor );
                        d->action = A_LEFT_INDENT;
                    } else if ( mx > right - ip_right - 5 && mx < right - ip_right + 5 &&
                                my >= height() - d->pmLeft.size().height() - 2 && my <= height() - 2 ) {
                        setCursor( ArrowCursor );
                        d->action = A_RIGHT_INDENT;
                    }
                }
                if ( d->flags & F_TABS )
                    searchTab(mx);
            } else {
                // Calculate the new value.
                int newPos=mx;
                if( newPos!=right && gridSize!=0.0 && (e->state() & ShiftButton)==0) { // apply grid.
                    double grid=zoomIt(gridSize * 16);
                    newPos=qRound( ((newPos * 16 / grid) * grid) / 16 );
                }
                if(newPos-left < 0) newPos=left;
                else if (right-newPos < 0) newPos=right;
                double newValue = unZoomIt(static_cast<double>(newPos) - frameStart + diffx);

                switch ( d->action ) {
                    case A_BR_LEFT: {
                        if ( d->canvas && mx < right-10 && mx+diffx-2 > 0) {
                            drawLine( d->oldMx, mx );
                            layout.ptLeft = unZoomIt(static_cast<double>(mx + diffx));
                            if( ip_first > right-left-15 ) {
                                ip_first=right-left-15;
                                ip_first=ip_first<0 ? 0 : ip_first;
                                i_first=unZoomItRtl( ip_first );
                                emit newFirstIndent( i_first );
                            }
                            if( ip_left > right-left-15 ) {
                                ip_left=right-left-15;
                                ip_left=ip_left<0 ? 0 : ip_left;
                                i_left=unZoomItRtl( ip_left );
                                emit newLeftIndent( i_left );
                            }
                            if ( ip_right > right-left-15 ) {
                                ip_right=right-left-15;
                                ip_right=ip_right<0? 0 : ip_right;
                                d->i_right=unZoomItRtl( ip_right );
                                emit newRightIndent( d->i_right );
                            }
                            d->oldMx = mx;
                            d->oldMy = my;
                            update();
                        }
                        else
                            return;
                    } break;
                    case A_BR_RIGHT: {
                        if ( d->canvas && mx > left+10 && mx+diffx <= pw-2) {
                            drawLine( d->oldMx, mx );
                            layout.ptRight = unZoomIt(static_cast<double>(pw - ( mx + diffx )));
                            if( ip_first > right-left-15 ) {
                                ip_first=right-left-15;
                                ip_first=ip_first<0 ? 0 : ip_first;
                                i_first=unZoomItRtl( ip_first );
                                emit newFirstIndent( i_first );
                            }
                            if( ip_left > right-left-15 ) {
                                ip_left=right-left-15;
                                ip_left=ip_left<0 ? 0 : ip_left;
                                i_left=unZoomItRtl( ip_left );
                                emit newLeftIndent( i_left );
                            }
                            if ( ip_right > right-left-15 ) {
                                ip_right=right-left-15;
                                ip_right=ip_right<0? 0 : ip_right;
                                d->i_right=unZoomItRtl( ip_right );
                                emit newRightIndent( d->i_right );
                            }
                            d->oldMx = mx;
                            d->oldMy = my;
                            update();
                        }
                        else
                            return;
                    } break;
                    case A_FIRST_INDENT: {
                        if ( d->canvas ) {
                            if (d->rtl) newValue = unZoomIt(pw) - newValue;
                            if(newValue == i_first) break;
                            drawLine( d->oldMx, newPos);
                            d->oldMx=newPos;
                            i_first = newValue;
                            update();
                        }
                    } break;
                    case A_LEFT_INDENT: {
                        if ( d->canvas ) {
                            if (d->rtl) newValue = unZoomIt(pw) - newValue;
                            if(newValue == i_left) break;
                            double newFirst =(i_first - i_left) + newValue;
                            if(zoomIt(newFirst) + left > right) break;

                            drawLine( d->oldMx, newPos);
                            i_first = newFirst;
                            i_left = newValue;
                            d->oldMx = newPos;
                            update();
                        }
                    } break;
                    case A_RIGHT_INDENT: {
                        if ( d->canvas ) {
                            double rightValue = unZoomIt(right - newPos);
                            if (d->rtl) rightValue = unZoomIt(pw) - rightValue;
                            if(rightValue == d->i_right) break;
                            drawLine( d->oldMx, newPos);
                            d->i_right=rightValue;
                            d->oldMx = newPos;
                            update();
                        }
                    } break;
                    case A_TAB: {
                        if ( d->canvas) {
                            if (d->rtl) newValue = unZoomIt(pw) - newValue;
                            if(newValue == (*d->currTab).ptPos) break; // no change
                            QPainter p( d->canvas );
                            p.setRasterOp( Qt::NotROP );
                            double pt = applyRtlAndZoom((*d->currTab).ptPos);
                            int pt_fr = qRound(pt) + frameStart - diffx;
                            if(d->currTab != d->removeTab) // prevent drawLine when we just created a new tab.
                                p.drawLine( pt_fr, 0, pt_fr, d->canvas->height() );
                            (*d->currTab).ptPos = newValue;
                            pt = applyRtlAndZoom( (*d->currTab).ptPos );
                            pt_fr = qRound(pt) + frameStart - diffx;
                            p.drawLine( pt_fr, 0, pt_fr, d->canvas->height() );
                            p.end();
                            d->oldMx = mx;
                            d->oldMy = my;
                            d->removeTab=d->tabList.end();
                            update();
                        }
                    } break;
                    default: break;
                }
            }
            if( d->action == A_HELPLINES )
            {
                emit moveHelpLines( e->pos(), orientation == Qt::Horizontal);
            }

            return;
        } break;
        case Qt::Vertical: {
            if ( !d->mousePressed ) {
                setCursor( ArrowCursor );
                d->action = A_NONE;
                if ( d->flags & F_NORESIZE )
                    break;
                if ( my > top - 5 && my < top + 5 ) {
                    setCursor( Qt::sizeVerCursor );
                    d->action = A_BR_TOP;
                } else if ( my > bottom - 5 && my < bottom + 5 ) {
                    setCursor( Qt::sizeVerCursor );
                    d->action = A_BR_BOTTOM;
                }
            } else {
                switch ( d->action ) {
                    case A_BR_TOP: {
                        if ( d->canvas && my < bottom-20 && my+diffy-2 > 0) {
                            QPainter p( d->canvas );
                            p.setRasterOp( Qt::NotROP );
                            p.drawLine( 0, d->oldMy, d->canvas->width(), d->oldMy );
                            p.drawLine( 0, my, d->canvas->width(), my );
                            p.end();
                            layout.ptTop = unZoomIt(static_cast<double>(my + diffy));
                            d->oldMx = mx;
                            d->oldMy = my;
                            update();
                        }
                        else
                            return;
                    } break;
                    case A_BR_BOTTOM: {
                        if ( d->canvas && my > top+20 && my+diffy < ph-2) {
                            QPainter p( d->canvas );
                            p.setRasterOp( Qt::NotROP );
                            p.drawLine( 0, d->oldMy, d->canvas->width(), d->oldMy );
                            p.drawLine( 0, my, d->canvas->width(), my );
                            p.end();
                            layout.ptBottom = unZoomIt(static_cast<double>(ph - ( my + diffy )));
                            d->oldMx = mx;
                            d->oldMy = my;
                            update();
                        }
                        else
                            return;
                    } break;
                    default: break;
                }
            }
        } break;
    }
    if( d->action == A_HELPLINES )
    {
        emit moveHelpLines( e->pos(), orientation == Qt::Horizontal);
    }

    d->oldMx = mx;
    d->oldMy = my;
}

void KoRuler::resizeEvent( QResizeEvent *e )
{
    QFrame::resizeEvent( e );
    buffer.resize( size() );
}

void KoRuler::mouseDoubleClickEvent( QMouseEvent* )
{
    handleDoubleClick();
}

void KoRuler::handleDoubleClick()
{
    if(!d->m_bReadWrite)
        return;
    if ( d->tabChooser && ( d->flags & F_TABS ) ) {
        // Double-click and mousePressed inserted a tab -> need to remove it
        if ( d->tabChooser->getCurrTabType() != 0 && d->removeTab!=d->tabList.end() && !d->tabList.isEmpty()) {
            d->tabList.remove( d->removeTab );
            d->removeTab=d->tabList.end();
            emit tabListChanged( d->tabList );
            update();
        } else if ( d->action == A_TAB ) {
            // Double-click on a tab
            emit doubleClicked( (*d->currTab).ptPos );
            return;
        }
        emit doubleClicked(); // usually paragraph dialog
    } else
        emit doubleClicked(); // usually page layout dialog
}

void KoRuler::setTabList( const KoTabulatorList & _tabList )
{
    KoTabulator rm, curr;
    bool rmEnd=false, currEnd=false;

    if(d->removeTab==d->tabList.end())
        rmEnd=true;
    else
        rm=(*d->removeTab);
    if(d->currTab==d->tabList.end())
        currEnd=true;
    else
        curr=(*d->currTab);

    d->tabList = _tabList;
    qHeapSort(d->tabList);   // "Trust no one." as opposed to "In David we trust."
    if(rmEnd)
        d->removeTab=d->tabList.end(); // different list, so update (David, who doesn't trust werner either anymore)
    else
        d->removeTab=d->tabList.find(rm);
    if(currEnd)
        d->currTab=d->tabList.end();
    else
        d->currTab=d->tabList.find(curr);
    update();
}

double KoRuler::makeIntern( double _v )
{
    return KoUnit::ptFromUnit( _v, m_unit );
}

void KoRuler::setupMenu()
{
    d->rb_menu = new QPopupMenu();
    Q_CHECK_PTR( d->rb_menu );
    d->mMM = d->rb_menu->insertItem( KoUnit::unitDescription( KoUnit::U_MM ),
				     this, SLOT( rbMM() ) );
    d->mPT = d->rb_menu->insertItem( KoUnit::unitDescription( KoUnit::U_PT ),
				     this, SLOT( rbPT() ) );
    d->mINCH = d->rb_menu->insertItem( KoUnit::unitDescription( KoUnit::U_INCH ),
				       this, SLOT( rbINCH() ) );
    d->rb_menu->insertSeparator();
    d->mPageLayout=d->rb_menu->insertItem(i18n("Page Layout..."), this, SLOT(pageLayoutDia()));
    d->rb_menu->insertSeparator();
    d->mRemoveTab=d->rb_menu->insertItem(i18n("Remove Tabulator"), this, SLOT(rbRemoveTab()));
    int uid;
    if ( m_unit == KoUnit::U_MM )
	uid = d->mMM;
    else if ( m_unit == KoUnit::U_PT )
	uid = d->mPT;
    else
	uid = d->mINCH;
    d->rb_menu->setItemChecked( uid, true );
    d->rb_menu->setItemEnabled( d->mRemoveTab, false );
}

void KoRuler::uncheckMenu()
{
    d->rb_menu->setItemChecked( d->mMM, false );
    d->rb_menu->setItemChecked( d->mPT, false );
    d->rb_menu->setItemChecked( d->mINCH, false );
}

void KoRuler::setUnit( const QString& _unit )
{
    setUnit( KoUnit::unit( _unit ) );
}

void KoRuler::setUnit( KoUnit::Unit unit )
{
    m_unit = unit;
    uncheckMenu();
    switch ( m_unit ) {
    case KoUnit::U_MM:
        d->rb_menu->setItemChecked( d->mMM, true );
        break;
    case KoUnit::U_PT:
        d->rb_menu->setItemChecked( d->mPT, true );
        break;
    case KoUnit::U_INCH:
        d->rb_menu->setItemChecked( d->mINCH, true );
        break;
    }
    update();
}

void KoRuler::setZoom( const double& zoom )
{
    if(zoom==m_zoom)
        return;
    m_zoom=zoom;
    m_1_zoom=1/m_zoom;
    update();
}

void KoRuler::rbRemoveTab() {

    d->tabList.remove( d->currTab );
    d->currTab=d->tabList.end();
    emit tabListChanged( d->tabList );
    update();
}

void KoRuler::setReadWrite(bool _readWrite)
{
    d->m_bReadWrite=_readWrite;
}

void KoRuler::searchTab(int mx) {

    int pos;
    d->currTab = d->tabList.end();
    KoTabulatorList::Iterator it = d->tabList.begin();
    for ( ; it != d->tabList.end() ; ++it ) {
        pos = qRound(applyRtlAndZoom((*it).ptPos)) - diffx + frameStart;
        if ( mx > pos - 5 && mx < pos + 5 ) {
            setCursor( Qt::sizeHorCursor );
            d->action = A_TAB;
            d->currTab = it;
            break;
        }
    }
}

void KoRuler::drawLine(int oldX, int newX) {

    QPainter p( d->canvas );
    p.setRasterOp( Qt::NotROP );
    p.drawLine( oldX, 0, oldX, d->canvas->height() );
    if(newX!=-1)
        p.drawLine( newX, 0, newX, d->canvas->height() );
    p.end();
}

void KoRuler::showMousePos( bool _showMPos )
{
    showMPos = _showMPos;
    hasToDelete = false;
    mposX = -1;
    mposY = -1;
    update();
}

void KoRuler::setOffset( int _diffx, int _diffy )
{
    //kdDebug() << "KoRuler::setOffset " << _diffx << "," << _diffy << endl;
    diffx = _diffx;
    diffy = _diffy;
    update();
}

void KoRuler::setFrameStartEnd( int _frameStart, int _frameEnd )
{
    if ( _frameStart != frameStart || _frameEnd != d->frameEnd || !m_bFrameStartSet )
    {
        frameStart = _frameStart;
        d->frameEnd = _frameEnd;
        // Remember that setFrameStartEnd was called. This activates a slightly
        // different mode (when moving start and end positions).
        m_bFrameStartSet = true;
        update();
    }
}

void KoRuler::setRightIndent( double _right )
{
    d->i_right = makeIntern( _right );
    update();
}

void KoRuler::setDirection( bool rtl )
{
    d->rtl = rtl;
    update();
}

void KoRuler::changeFlags(int _flags)
{
    d->flags = _flags;
}

int KoRuler::flags() const
{
    return d->flags;
}

double KoRuler::applyRtlAndZoom( double value ) const
{
    int frameWidth = d->frameEnd - frameStart;
    return d->rtl ? ( frameWidth - zoomIt( value ) ) : zoomIt( value );
}

double KoRuler::unZoomItRtl( int pixValue ) const
{
    int frameWidth = d->frameEnd - frameStart;
    return d->rtl ? ( unZoomIt( (double)(frameWidth - pixValue) ) ) : unZoomIt( (double)pixValue );
}

#include "koRuler.moc"
