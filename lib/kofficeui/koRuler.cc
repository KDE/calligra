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
#include <kglobal.h>
#include <kdebug.h>
#include <kiconloader.h>
#include <qfont.h>
#include <qfontmetrics.h>
#include <qpen.h>
#include <qbrush.h>
#include <qpainter.h>
#include <qpopupmenu.h>
#include <qtl.h>

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
    QPixmap pmFirst, pmLeft;
    KoTabChooser *tabChooser;
    KoTabulatorList tabList;
    KoTabulatorList::Iterator removeTab;     // Do we have to remove a certain tab in the DC Event?
    KoTabulatorList::Iterator currTab;
    QPopupMenu *rb_menu;
    int mMM, mPT, mINCH, mRemoveTab;
    int frameEnd;
    bool m_bReadWrite;
};


/******************************************************************/
/* Class: KoRuler                                                 */
/******************************************************************/

/*================================================================*/
KoRuler::KoRuler( QWidget *_parent, QWidget *_canvas, Orientation _orientation,
                 KoPageLayout _layout, int _flags, KoTabChooser *_tabChooser )
    : QFrame( _parent ), buffer( width(), height() ), m_zoom(1.0), m_1_zoom(1.0),
      unit( "mm" )
{
    setWFlags( WResizeNoErase );
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

    setMouseTracking( true );
    d->mousePressed = false;
    d->action = A_NONE;

    d->oldMx = 0;
    d->oldMy = 0;

    showMPos = false;
    mposX = 0;
    mposY = 0;
    hasToDelete = false;
    d->whileMovingBorderLeft = d->whileMovingBorderRight = d->whileMovingBorderTop = d->whileMovingBorderBottom = false;

    d->pmFirst = UserIcon( "koRulerFirst" );
    d->pmLeft = UserIcon( "koRulerLeft" );
    d->currTab = d->tabList.end();

    d->removeTab = d->tabList.end();
    frameStart = qRound( zoomIt(layout.ptLeft) );
    d->frameEnd = qRound( zoomIt(layout.ptWidth - layout.ptRight) );
    m_bFrameStartSet = false;

    setupMenu();
}

/*================================================================*/
KoRuler::~KoRuler()
{
    delete d->rb_menu;
    delete d;
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

    double dist;
    int j = 0;
    int totalw = qRound( zoomIt(layout.ptWidth) );
    QString str;
    QFont font = QFont( "helvetica", 8 ); // Ugh... hardcoded (Werner)
    QFontMetrics fm( font );

    p.setBrush( Qt::white );

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
    if ( unit == "inch" )
        dist = INCH_TO_POINT (m_zoom);
    else if ( unit == "pt" )
        dist = 100.0 * m_zoom;
    else
        dist = MM_TO_POINT ( 10.0 * m_zoom );

    for ( double i = 0.0;i <= (double)totalw;i += dist ) {
        str=QString::number(j++);
        if ( unit == "pt" && j!=1)
            str+="00";
        p.drawText( qRound(i) - diffx - qRound(fm.width( str ) * 0.5),
                    qRound(( height() - fm.height() ) * 0.5),
                    fm.width( str ), height(), AlignLeft | AlignTop, str );
    }

    // Draw the medium-sized lines
    for ( double i = dist * 0.5;i <= (double)totalw;i += dist ) {
        int ii=qRound(i);
        p.drawLine( ii - diffx, 5, ii - diffx, height() - 5 );
    }

    // Draw the small lines
    for ( double i = dist * 0.25;i <= (double)totalw;i += dist * 0.5 ) {
        int ii=qRound(i);
        p.drawLine( ii - diffx, 7, ii - diffx, height() - 7 );
    }

    // Draw ending bar (at page width)
    int constant=zoomIt(1);
    p.drawLine( totalw - diffx + constant, 1, totalw - diffx + constant, height() - 1 );
    p.setPen( Qt::white );
    p.drawLine( totalw - diffx, 1, totalw - diffx, height() - 1 );

    // Draw starting bar (at 0) - can we see it ?
    p.setPen( Qt::black );
    p.drawLine( -diffx, 1, -diffx, height() - 1 );
    p.setPen( Qt::white );
    p.drawLine( -diffx - constant, 1, -diffx - constant, height() - 1 );

    // Draw the indents triangles
    if ( d->flags & F_INDENTS ) {
        p.drawPixmap( qRound(zoomIt(i_first) - d->pmFirst.size().width() * 0.5 +
                                 static_cast<double>(r.left())), 2, d->pmFirst );
        p.drawPixmap( qRound(zoomIt(i_left) - d->pmLeft.size().width() * 0.5 +
                                 static_cast<double>(r.left())),
                      height() - d->pmLeft.size().height() - 2, d->pmLeft );
    }

    // Show the mouse position
    if ( d->action == A_NONE && showMPos ) {
        p.setPen( Qt::black );
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

    _painter.setPen( QPen( Qt::black, 2, SolidLine ) );

    KoTabulatorList::Iterator it = d->tabList.begin();
    for ( ; it != d->tabList.end() ; it++ ) {
        ptPos = qRound(zoomIt((*it).ptPos)) - diffx + frameStart;
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
            _painter.fillRect( ptPos + 20 / 2 + 2, height() - 9, 3, 3, black );
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

    double dist;
    int j = 0;
    double ph = zoomIt(layout.ptHeight);
    int i_ph=qRound(ph);
    QString str;
    QFont font = QFont( "helvetica", 8 );  // Hardcode the size? (Werner)
    QFontMetrics fm( font );

    p.setBrush( Qt::white );
    QRect r;

    if ( !d->whileMovingBorderTop )
        r.setTop( -diffy + qRound(zoomIt(layout.ptTop)) );
    else
        r.setTop( d->oldMy );
    r.setLeft( 0 );
    if ( !d->whileMovingBorderBottom )
        r.setBottom( -diffy + i_ph - qRound(zoomIt(layout.ptBottom)) );
    else
        r.setBottom( d->oldMy );
    r.setRight( width() );

    p.drawRect( r );
    p.setFont( font );

    if ( unit == "inch" )
        dist = INCH_TO_POINT ( m_zoom );
    else if ( unit == "pt" )
        dist = 100.0 * m_zoom;
    else
        dist = MM_TO_POINT ( 10.0 * m_zoom );

    for ( double i = 0.0;i <= ph;i += dist ) {
        str=QString::number(j++);
        if ( unit == "pt" && j!=1 )
            str+="00";
        p.drawText( qRound(( width() - fm.width( str ) ) * 0.5),
                    qRound(i) - diffy - qRound(fm.height() * 0.5),
                    width(), fm.height(), AlignLeft | AlignTop, str );
    }

    for ( double i = dist * 0.5;i <= ph;i += dist ) {
        int ii=qRound(i);
        p.drawLine( 5, ii - diffy, width() - 5, ii - diffy );
    }

    for ( double i = dist * 0.25;i <= ph;i += dist *0.5 ) {
        int ii=qRound(i);
        p.drawLine( 7, ii - diffy, width() - 7, ii - diffy );
    }

    p.drawLine( 1, i_ph - diffy + 1, width() - 1, i_ph - diffy + 1 );
    p.setPen( Qt::white );
    p.drawLine( 1, i_ph - diffy, width() - 1, i_ph - diffy );

    p.setPen( Qt::black );
    p.drawLine( 1, -diffy, width() - 1, -diffy );
    p.setPen( Qt::white );
    p.drawLine( 1, -diffy - 1, width() - 1, -diffy - 1 );

    if ( d->action == A_NONE && showMPos ) {
        p.setPen( Qt::black );
        p.drawLine( 1, mposY, width() - 1, mposY );
    }
    hasToDelete = false;

    p.end();
    _painter->drawPixmap( 0, 0, buffer );
}

/*================================================================*/
void KoRuler::mousePressEvent( QMouseEvent *e )
{
    if( !d->m_bReadWrite)
        return;

    d->oldMx = e->x();
    d->oldMy = e->y();
    d->mousePressed = true;
    d->removeTab=d->tabList.end();

    if ( e->button() == RightButton ) {
        if(d->currTab==d->tabList.end())
            d->rb_menu->setItemEnabled(d->mRemoveTab, false);
        else
            d->rb_menu->setItemEnabled(d->mRemoveTab, true);
        d->rb_menu->popup( QCursor::pos() );
        d->action = A_NONE;
        d->mousePressed = false;
        return;
    }

    if ( d->action == A_BR_RIGHT || d->action == A_BR_LEFT ) {
        if ( d->action == A_BR_RIGHT )
            d->whileMovingBorderRight = true;
        else
            d->whileMovingBorderLeft = true;

        if ( d->canvas )
            drawLine(d->oldMx, -1);
        repaint( false );
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
        repaint( false );
    } else if ( d->action == A_FIRST_INDENT || d->action == A_LEFT_INDENT ) {
        if ( d->canvas )
            drawLine(d->oldMx, -1);
    } else if ( d->action == A_TAB ) {
        if ( d->canvas && d->currTab!=d->tabList.end()) {
            drawLine( qRound( zoomIt((*d->currTab).ptPos) ) + frameStart, -1);
        }
    } else if ( d->tabChooser && ( d->flags & F_TABS ) && d->tabChooser->getCurrTabType() != 0 ) {
        int left = frameStart - diffx;
        int right = d->frameEnd - diffx;

        if( e->x()-left < 0 || right-e->x() < 0 )
            return;
        KoTabulator tab;
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
        tab.ptPos = unZoomIt( static_cast<double>( e->x() + diffx - frameStart ) );

        KoTabulatorList::Iterator it=d->tabList.begin();
        for( ; (it!=d->tabList.end() && tab > (*it)); ++it);   // DON'T remove that trailing ';'  :-)
        // You know, a while() would clearer (DF) :)

        d->removeTab=d->tabList.insert(it, tab);

        emit tabListChanged( d->tabList );
        repaint( false );
    }
}

/*================================================================*/
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
        repaint( false );
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
        repaint( false );
        emit newPageLayout( layout );
    } else if ( d->action == A_FIRST_INDENT ) {
        if ( d->canvas )
            drawLine(d->oldMx, -1);
        repaint( false );
        emit newFirstIndent( i_first );
    } else if ( d->action == A_LEFT_INDENT ) {
        if ( d->canvas )
            drawLine(d->oldMx, -1);
        repaint( false );
        double _tmp = i_first;
        emit newLeftIndent( i_left );
        i_first = _tmp;
        emit newFirstIndent( i_first );
    } else if ( d->action == A_TAB ) {
        if ( d->canvas && !fakeMovement ) {
            drawLine( qRound( zoomIt((*d->currTab).ptPos) ) + frameStart, -1);
        }
        if ( (e->y() < -50 || e->y() > height() + 50) && d->currTab!=d->tabList.end() )
            d->tabList.remove(d->currTab);

        qHeapSort( d->tabList );
        searchTab( e->x() );
        emit tabListChanged( d->tabList );
        repaint( false );
    }
}

/*================================================================*/
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
    int ip_left = qRound(zoomIt(i_left));
    int ip_first = qRound(zoomIt(i_first));

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
                                my >=   height() - d->pmLeft.size().height() - 2 && my <= height() - 2 ) {
                        setCursor( ArrowCursor );
                        d->action = A_LEFT_INDENT;
                    }
                }
                if ( d->flags & F_TABS )
                    searchTab(mx);
            } else {
                // Note: All limits should be 0, but KWord crashes currently, when the
                // page is too small (infinite loop in the line breaking algorithm)! (Werner)
                switch ( d->action ) {
                    case A_BR_LEFT: {
                        if ( d->canvas && mx < right-10 && mx+diffx-2 > 0) {
                            drawLine( d->oldMx, mx );
                            layout.ptLeft = unZoomIt(static_cast<double>(mx + diffx));
                            layout.mmLeft = POINT_TO_MM(layout.ptLeft);
                            layout.inchLeft = POINT_TO_INCH(layout.ptLeft);
                            if( ip_first > right-left-15 ) {
                                ip_first=right-left-15;
                                ip_first=ip_first<0 ? 0 : ip_first;
                                i_first=unZoomIt(static_cast<double>(ip_first));
                                emit newFirstIndent( i_first );
                            }
                            if( ip_left > right-left-15 ) {
                                ip_left=right-left-15;
                                ip_left=ip_left<0 ? 0 : ip_left;
                                i_left=unZoomIt(static_cast<double>(ip_left));
                                emit newLeftIndent( i_left );
                            }
                            d->oldMx = mx;
                            d->oldMy = my;
                            repaint( false );
                        }
                        else
                            return;
                    } break;
                    case A_BR_RIGHT: {
                        if ( d->canvas && mx > left+10 && mx+diffx <= pw-2) {
                            drawLine( d->oldMx, mx );
                            layout.ptRight = unZoomIt(static_cast<double>(pw - ( mx + diffx )));
                            layout.mmRight = POINT_TO_MM( layout.ptRight );
                            layout.inchRight = POINT_TO_INCH( layout.ptRight );
                            if( ip_first > right-left-15 ) {
                                ip_first=right-left-15;
                                ip_first=ip_first<0 ? 0 : ip_first;
                                i_first=unZoomIt(static_cast<double>(ip_first));
                                emit newFirstIndent( i_first );
                            }
                            if( ip_left > right-left-15 ) {
                                ip_left=right-left-15;
                                ip_left=ip_left<0 ? 0 : ip_left;
                                i_left=unZoomIt(static_cast<double>(ip_left));
                                emit newLeftIndent( i_left );
                            }
                            d->oldMx = mx;
                            d->oldMy = my;
                            repaint( false );
                        }
                        else
                            return;
                    } break;
                    case A_FIRST_INDENT: {
                        if ( d->canvas ) {
                            if ( mx - left >= 0 && right - mx >= 10 )
                                drawLine( d->oldMx, mx);
                            else
                                return;
                            i_first = unZoomIt(static_cast<double>(mx - left));
                            d->oldMx = mx;
                            d->oldMy = my;
                            repaint( false );
                        }
                    } break;
                    case A_LEFT_INDENT: {
                        if ( d->canvas ) {
                            if ( mx - left >= 0 && right - mx >= 10 )
                                drawLine( d->oldMx, mx);
                            else
                                return;
                            int oldDiff = ip_first-ip_left;
                            ip_left = mx - left;
                            ip_first = ip_left + oldDiff;
                            if( ip_first < 0)
                                ip_first=0;
                            else if( ip_first > right-left-10 )
                                ip_first=right-left-10;
                            i_left=unZoomIt(static_cast<double>(ip_left));
                            i_first=unZoomIt(static_cast<double>(ip_first));
                            d->oldMx = mx;
                            d->oldMy = my;
                            repaint( false );
                        }
                    } break;
                    case A_TAB: {
                        if ( d->canvas && mx-left >= 0 && right-mx >= 0) {
                            QPainter p( d->canvas );
                            p.setRasterOp( Qt::NotROP );
                            double pt = zoomIt((*d->currTab).ptPos);
                            int pt_fr = qRound(pt) + frameStart;
                            p.drawLine( pt_fr, 0, pt_fr, d->canvas->height() );
                            (*d->currTab).ptPos = unZoomIt(static_cast<double>(mx) - frameStart );
                            pt = zoomIt( (*d->currTab).ptPos );
                            pt_fr = qRound(pt) + frameStart;
                            p.drawLine( pt_fr, 0, pt_fr, d->canvas->height() );
                            p.end();
                            d->oldMx = mx;
                            d->oldMy = my;
                            repaint( false );
                        }
                    } break;
                    default: break;
                }
            }
        } break;
        case Qt::Vertical: {
            if ( !d->mousePressed ) {
                setCursor( ArrowCursor );
                d->action = A_NONE;
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
                            layout.mmTop = POINT_TO_MM( layout.ptTop );
                            layout.inchTop = POINT_TO_INCH( layout.ptTop );
                            d->oldMx = mx;
                            d->oldMy = my;
                            repaint( false );
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
                            layout.mmBottom = POINT_TO_MM( layout.ptBottom );
                            layout.inchBottom = POINT_TO_INCH( layout.ptBottom );
                            d->oldMx = mx;
                            d->oldMy = my;
                            repaint( false );
                        }
                        else
                            return;
                    } break;
                    default: break;
                }
            }
        } break;
    }
    d->oldMx = mx;
    d->oldMy = my;
}

/*================================================================*/
void KoRuler::resizeEvent( QResizeEvent *e )
{
    QFrame::resizeEvent( e );
    buffer.resize( size() );
}

/*================================================================*/
void KoRuler::mouseDoubleClickEvent( QMouseEvent* )
{
    if(!d->m_bReadWrite)
        return;
    if ( d->tabChooser && ( d->flags & F_TABS ) && d->tabChooser->getCurrTabType() != 0 && d->removeTab!=d->tabList.end() && !d->tabList.isEmpty()) {
        d->tabList.remove( d->removeTab );
        d->removeTab=d->tabList.end();
        emit tabListChanged( d->tabList );
        repaint( false );
    }
    emit openPageLayoutDia();
}

/*================================================================*/
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
    if(!rmEnd)
        d->removeTab=d->tabList.find(rm);
    if(!currEnd)
        d->currTab=d->tabList.find(curr);
    repaint( false );
}

/*================================================================*/
double KoRuler::makeIntern( double _v )
{
    if ( unit == "mm" ) return MM_TO_POINT( _v );
    if ( unit == "inch" ) return INCH_TO_POINT( _v );
    return _v;
}

/*================================================================*/
void KoRuler::setupMenu()
{
    d->rb_menu = new QPopupMenu();
    CHECK_PTR( d->rb_menu );
    d->mMM = d->rb_menu->insertItem( i18n( "Millimeters (mm)" ), this, SLOT( rbMM() ) );
    d->mPT = d->rb_menu->insertItem( i18n( "Points (pt)" ), this, SLOT( rbPT() ) );
    d->mINCH = d->rb_menu->insertItem( i18n( "Inches (inch)" ), this, SLOT( rbINCH() ) );
    d->rb_menu->insertSeparator();
    d->rb_menu->insertItem(i18n("Page Layout..."), this, SLOT(pageLayoutDia()));
    d->rb_menu->insertSeparator();
    d->mRemoveTab=d->rb_menu->insertItem(i18n("Remove Tabulator"), this, SLOT(rbRemoveTab()));
    d->rb_menu->setCheckable( false );
    d->rb_menu->setItemChecked( d->mMM, true );
    d->rb_menu->setItemEnabled( d->mRemoveTab, false );
}

/*================================================================*/
void KoRuler::uncheckMenu()
{
    d->rb_menu->setItemChecked( d->mMM, false );
    d->rb_menu->setItemChecked( d->mPT, false );
    d->rb_menu->setItemChecked( d->mINCH, false );
}

/*================================================================*/
void KoRuler::setUnit( const QString& _unit )
{
    unit = _unit;
    uncheckMenu();

    if ( unit == "mm" ) {
        d->rb_menu->setItemChecked( d->mMM, true );
        layout.unit = PG_MM;
    } else if ( unit == "pt" ) {
        d->rb_menu->setItemChecked( d->mPT, true );
        layout.unit = PG_PT;
    } else if ( unit == "inch" ) {
        d->rb_menu->setItemChecked( d->mINCH, true );
        layout.unit = PG_INCH;
    }
    repaint( false );
}

void KoRuler::setZoom( const double& zoom )
{
    if(zoom==m_zoom)
        return;
    m_zoom=zoom;
    m_1_zoom=1/m_zoom;
    repaint( false );
}

void KoRuler::rbRemoveTab() {

    d->tabList.remove( d->currTab );
    d->currTab=d->tabList.end();
    emit tabListChanged( d->tabList );
    repaint(false);
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
        pos = qRound(zoomIt((*it).ptPos)) - diffx + frameStart;
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
    repaint( false );
}

void KoRuler::setOffset( int _diffx, int _diffy )
{
    diffx = _diffx;
    diffy = _diffy;
    repaint( false );
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
        repaint( false );
    }
}

#include "koRuler.moc"
