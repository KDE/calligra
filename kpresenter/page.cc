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

#include <qprogressdialog.h>
#include <qdragobject.h>
#include <qfile.h>
#include <qtextstream.h>
#include <qpainter.h>
#include <qwmatrix.h>
#include <qapplication.h>
#include <qpointarray.h>
#include <qpopupmenu.h>
#include <qimage.h>
#include <qdatetime.h>
#include <qdropsite.h>

#include <page.h>
#include <kpresenter_view.h>
#include <footer_header.h>
#include <qwmf.h>
#include <kpbackground.h>
#include <kpclipartobject.h>
#include <kppixmapobject.h>
#include <movecmd.h>
#include <resizecmd.h>
#include <gotopage.h>
#include <kptextobject.h>

#include <kapp.h>
#include <kmimemagic.h>
#include <kurl.h>
#include <klocale.h>
#include <kiconloader.h>
#include <kglobal.h>
#include <ktempfile.h>
#include <kdebug.h>

#include <stdlib.h>

static bool ignoreSkip = FALSE;

/******************************************************************/
/* class Page - Page                                              */
/******************************************************************/

/*====================== constructor =============================*/
Page::Page( QWidget *parent, const char *name, KPresenterView *_view )
    : QWidget( parent, name ), buffer( size() )
{
    setWFlags( WResizeNoErase );

    alignMenu1 = 0;
    alignMenu2 = 0;
    alignMenu3 = 0;
    alignMenu4 = 0;
    alignMenu5 = 0;
    alignMenu6 = 0;
    alignMenu7 = 0;
    graphMenu = 0;
    partMenu = 0;
    rectMenu = 0;
    pieMenu = 0;
    picResizeMenu = 0;
    picMenu = 0;
    clipMenu = 0;
    txtMenu = 0;
    presMenu = 0;
    pageMenu = 0;

    if ( parent ) {
        mousePressed = false;
        modType = MT_NONE;
        resizeObjNum = -1;
        editNum = -1;
        setBackgroundColor( white );
        view = _view;
        setupMenus();
        setMouseTracking( true );
        show();
        editMode = true;
        currPresPage = 1;
        currPresStep = 0;
        subPresStep = 0;
        _presFakt = 1.0;
        goingBack = false;
        drawMode = false;
        fillBlack = true;
        drawRubber = false;
        toolEditMode = TEM_MOUSE;
        tmpObjs.setAutoDelete( false );
        setAcceptDrops( true );
        inEffect = false;
        ratio = 0;
        keepRatio = false;
        mouseSelectedObject = false;
    } else {
        view = 0;
        hide();
    }

    setFocusPolicy( QWidget::StrongFocus );
    setFocus();
    setKeyCompression( true );
}

/*======================== destructor ============================*/
Page::~Page()
{
    // deactivate possible opened textobject to avoid double deletion, KPTextObject deletes this already
    exitEditMode();

    delete alignMenu1;
    delete alignMenu2;
    delete alignMenu3;
    delete alignMenu4;
    delete alignMenu5;
    delete alignMenu6;
    delete alignMenu7;
    delete graphMenu;
    delete partMenu;
    delete rectMenu;
    delete pieMenu;
    delete picResizeMenu;
    delete picMenu;
    delete clipMenu;
    delete txtMenu;
    delete presMenu;
    delete pageMenu;
}

/*============================ draw contents ====================*/
void Page::draw( QRect _rect, QPainter *p )
{
    //kdDebug() << "Page::draw" << endl;
    p->save();
    editMode = false;
    fillBlack = false;
    _presFakt = 1.0;
    currPresStep = 1000;
    subPresStep = 1000;
    currPresPage = currPgNum();

    drawPageInPainter( p, view->getDiffY(), _rect );

    currPresPage = 1;
    currPresStep = 0;
    subPresStep = 0;
    _presFakt = 1.0;
    fillBlack = true;
    editMode = true;
    p->restore();
}

/*======================== paint event ===========================*/
void Page::paintEvent( QPaintEvent* paintEvent )
{
    //kdDebug() << "Page::paintEvent " << paintEvent->rect().x() << "," << paintEvent->rect().y()
    //          << " " << paintEvent->rect().width() << "x" << paintEvent->rect().height() << endl;
    QPainter painter;

    painter.begin( &buffer );

    if ( editMode || !fillBlack )
	painter.fillRect( paintEvent->rect(), white );
    else
	painter.fillRect( paintEvent->rect(), black );

    painter.setClipping( true );
    painter.setClipRect( paintEvent->rect() );

    drawBackground( &painter, paintEvent->rect() );
    drawObjects( &painter, paintEvent->rect() );

    painter.end();

    bitBlt( this, paintEvent->rect().topLeft(), &buffer, paintEvent->rect() );
}

/*======================= draw background ========================*/
void Page::drawBackground( QPainter *painter, QRect rect )
{
    QRegion grayRegion( rect );
    QListIterator<KPBackGround> it(*backgroundList());
    for ( int i = 0 ; it.current(); ++it, ++i ) {
	if ( editMode )
        {
            if ( !ignoreSkip && painter->device()->devType() != QInternal::Printer && i != (int)view->getCurrPgNum() - 1 )
            {
                //kdDebug() << "Page::drawBackground skipping drawing" << endl;
                continue;
            }
            //kdDebug() << "Page::drawBackground drawing bg for page " << i+1 << " editMode=" << editMode << endl;
            QRect pageRect = getPageRect( i, _presFakt );
            if ( rect.intersects( pageRect ) )
                it.current()->draw( painter, pageRect.topLeft(), true );
            // Include the border now
            pageRect.rLeft() -= 1;
            pageRect.rTop() -= 1;
            pageRect.rRight() += 1;
            pageRect.rBottom() += 1;
            grayRegion -= pageRect;
        }
        else if ( !editMode && static_cast<int>( currPresPage ) == i + 1 )
        {
            QRect pgRect = getPageRect( i, _presFakt, false );
                /*kdDebug() << "Page::drawBackground pgRect: " << pgRect.x() << "," << pgRect.y()
                          << " " << pgRect.width() << "x" << pgRect.height() << endl;

                kdDebug() << " with borders, topleft is: " << pgRect.x() +
                    view->kPresenterDoc()->getLeftBorder() * _presFakt
                          << "," << pgRect.y() +
                    view->kPresenterDoc()->getTopBorder() * _presFakt
                          << endl;
                */
            it.current()->draw( painter, QPoint( pgRect.x() +
                                                 qRound(view->kPresenterDoc()->getLeftBorder() * _presFakt),
                                                 pgRect.y() +
                                                 qRound(view->kPresenterDoc()->getTopBorder() * _presFakt) ),
                                false );
	}
    }

    // In edit mode we also want to draw the gray area out of the pages
    if ( editMode && !grayRegion.isEmpty() )
    {
        eraseEmptySpace( painter, grayRegion, QApplication::palette().active().brush( QColorGroup::Mid ) );
    }
}

// 100% stolen from KWord
void Page::eraseEmptySpace( QPainter * painter, const QRegion & emptySpaceRegion, const QBrush & brush )
{
    painter->save();
    // Translate emptySpaceRegion in device coordinates
    // ( ARGL why on earth isn't QPainter::setClipRegion in transformed coordinate system ?? )
    QRegion devReg;
    QArray<QRect>rs = emptySpaceRegion.rects();
    rs.detach();
    for ( uint i = 0 ; i < rs.size() ; ++i )
        rs[i] = painter->xForm( rs[i] );
    devReg.setRects( rs.data(), rs.size() );
    painter->setClipRegion( devReg );
    painter->setPen( Qt::NoPen );

    //kdDebug() << "KWDocument::eraseEmptySpace emptySpaceRegion: " << DEBUGRECT( emptySpaceRegion.boundingRect() ) << endl;
    painter->fillRect( emptySpaceRegion.boundingRect(), brush );
    painter->restore();
}

/*========================= draw objects =========================*/
void Page::drawObjects( QPainter *painter, QRect rect )
{
    int pgNum = editMode ? (int)view->getCurrPgNum() : currPresPage;
    //kdDebug() << "Page::drawObjects ----- pgNum=" << pgNum << " currPresStep=" << currPresStep << " _presFakt=" << _presFakt << endl;

    QListIterator<KPObject> it(*objectList());
    for ( int i = 0 ; it.current(); ++it, ++i ) {
        KPObject *kpobject = it.current();
	int pg = getPageOfObj( i, _presFakt );
        //if (i<10) kdDebug() << "Page::drawObjects object " << i << " page " << pg << " getPresNum=" << kpobject->getPresNum() << endl;

	if ( kpobject->isSticky() ||
	     ( rect.intersects( kpobject->getBoundingRect( diffx(), diffy() ) ) && editMode ) ||
	     ( !editMode && pg == static_cast<int>( currPresPage ) &&
	       kpobject->getPresNum() <= static_cast<int>( currPresStep ) &&
	       ( !kpobject->getDisappear() || kpobject->getDisappear() &&
		 kpobject->getDisappearNum() > static_cast<int>( currPresStep ) ) ) ) {
 	    if ( inEffect && kpobject->getPresNum() >= static_cast<int>( currPresStep ) )
 		continue;

	    if ( !editMode && static_cast<int>( currPresStep ) == kpobject->getPresNum() && !goingBack ) {
                //kdDebug() << "                 setSubPresStep " << subPresStep << endl;
		kpobject->setSubPresStep( subPresStep );
		kpobject->doSpecificEffects( true, false );
	    }

	    if ( !ignoreSkip && !kpobject->isSticky() &&
		 editMode && painter->device()->devType() != QInternal::Printer && pg != pgNum )
		continue;

	    QPoint op;
	    if ( kpobject->isSticky() ) {
		op = kpobject->getOrig();
		kpobject->setOrig( op.x(), op.y() - pg * getPageRect( 0, _presFakt ).height() + pgNum * getPageRect( 0, _presFakt ).height() );
	    }

            //kdDebug() << "                 drawing object at " << diffx() << "," << diffy() << "  and setting subpresstep to 0 !" << endl;
	    kpobject->draw( painter, diffx(), diffy() );
	    kpobject->setSubPresStep( 0 );
	    kpobject->doSpecificEffects( false );
	    if ( kpobject->isSticky() )
		kpobject->setOrig( op );
	}
    }
}

/*==================== handle mouse pressed ======================*/
void Page::mousePressEvent( QMouseEvent *e )
{
    if(!view->koDocument()->isReadWrite())
        return;

    if ( e->state() & ControlButton )
        keepRatio = true;

    KPObject *kpobject = 0;

    oldMx = e->x();
    oldMy = e->y();

    resizeObjNum = -1;

    exitEditMode();

    if ( editMode ) {
        if ( e->button() == LeftButton ) {
            mousePressed = true;

            switch ( toolEditMode ) {
                case TEM_MOUSE: {
                    bool overObject = false;
                    bool deSelAll = true;
                    KPObject *kpobject = 0;

                    firstX = e->x();
                    firstY = e->y();

                    if ( (int)objectList()->count() - 1 >= 0 ) {
                        for ( int i = static_cast<int>( objectList()->count() ) - 1; i >= 0 ; i-- ) {
                            kpobject = objectList()->at( i );
                            QSize s = kpobject->getSize();
                            QPoint pnt = kpobject->getOrig();
                            if ( QRect( pnt.x() - diffx(), pnt.y() - diffy(), s.width(), s.height() ).
                                 contains( QPoint( e->x(), e->y() ) ) ) {
                                overObject = true;
                                if ( kpobject->isSelected() && modType == MT_MOVE ) deSelAll = false;
                                if ( kpobject->isSelected() && modType != MT_MOVE && modType != MT_NONE ) {
                                    oldBoundingRect = kpobject->getBoundingRect( 0, 0 );
                                    resizeObjNum = i;
                                }
                                break;
                            }
                        }
                    }

                    if ( deSelAll && !( e->state() & ShiftButton ) && !( e->state() & ControlButton ) )
                        deSelectAllObj();

                    if ( overObject ) {
                        selectObj( kpobject );
                        modType = MT_NONE;
                    } else {
                        modType = MT_NONE;
                        if ( !( e->state() & ShiftButton ) && !( e->state() & ControlButton ) )
                            deSelectAllObj();
                        drawRubber = true;
                        rubber = QRect( e->x(), e->y(), 0, 0 );
                    }

                } break;
                default: {
                    deSelectAllObj();
                    mousePressed = true;
                    insRect = QRect( ( ( e->x() + diffx() ) / rastX() ) * rastX() - diffx(),
                                     ( ( e->y() + diffy() ) / rastY() ) * rastY() - diffy(), 0, 0 );
                } break;
            }
        }

        if ( e->button() == RightButton && toolEditMode == TEM_MOUSE ) {
            int num = getObjectAt( e->x(), e->y() );
            if ( num != -1 ) {
                kpobject = objectList()->at( num );
                bool state=!( e->state() & ShiftButton ) && !( e->state() & ControlButton ) && !kpobject->isSelected();
                QPoint pnt = QCursor::pos();
                if ( kpobject->getType() == OT_PICTURE ) {
                    mousePressed = false;
                    deSelectAllObj();
                    selectObj( kpobject );
                    picMenu->popup( pnt );
                } else if ( kpobject->getType() == OT_CLIPART ) {
                    mousePressed = false;
                    deSelectAllObj();
                    selectObj( kpobject );
                    clipMenu->popup( pnt );
                } else if ( kpobject->getType() == OT_TEXT ) {
                    if ( state )
                        deSelectAllObj();
                    selectObj( kpobject );
                    txtMenu->popup( pnt );
                    mousePressed = false;
                } else if ( kpobject->getType() == OT_PIE ) {
                    if ( state )
                        deSelectAllObj();
                    selectObj( kpobject );
                    pieMenu->popup( pnt );
                    mousePressed = false;
                } else if ( kpobject->getType() == OT_RECT ) {
                    if ( state )
                        deSelectAllObj();
                    selectObj( kpobject );
                    rectMenu->popup( pnt );
                    mousePressed = false;
                } else if ( kpobject->getType() == OT_PART ) {
                    if ( state )
                        deSelectAllObj();
                    selectObj( kpobject );
                    partMenu->popup( pnt );
                    mousePressed = false;
                } else {
                    if ( state )
                        deSelectAllObj();
                    selectObj( kpobject );
                    graphMenu->popup( pnt );
                    mousePressed = false;
                }
                modType = MT_NONE;
            } else {
                QPoint pnt = QCursor::pos();
                pageMenu->setItemEnabled( delPageId, view->kPresenterDoc()->getPageNums() > 1 );
                pageMenu->popup( pnt );
                mousePressed = false;
                modType = MT_NONE;
            }
        }
        else if( e->button() == RightButton && toolEditMode != TEM_MOUSE ) {
            //desactivate tools when you click on right button
            setToolEditMode( TEM_MOUSE );
        }
    } else {
        oldMx = e->x();
        oldMy = e->y();
        if ( e->button() == LeftButton ) {
            if ( presMenu->isVisible() ) {
                presMenu->hide();
                setCursor( blankCursor );
            } else {
                if ( drawMode )
                {}
                else
                    view->screenNext();
            }
        } else if ( e->button() == MidButton )
            view->screenPrev();
        else if ( e->button() == RightButton ) {
            setCursor( arrowCursor );
            QPoint pnt = QCursor::pos();
            presMenu->popup( pnt );
        }
    }

    mouseMoveEvent( e );

    if ( modType != MT_NONE && modType != MT_MOVE ) {
        KPObject *kpobject = objectList()->at( resizeObjNum );
        if ( kpobject ) {
            ratio = static_cast<double>( static_cast<double>( kpobject->getSize().width() ) /
                                         static_cast<double>( kpobject->getSize().height() ) );
            oldRect = QRect( kpobject->getOrig().x(), kpobject->getOrig().y(),
                             kpobject->getSize().width(), kpobject->getSize().height() );
        }
    }
}

/*=================== handle mouse released ======================*/
void Page::mouseReleaseEvent( QMouseEvent *e )
{
    if ( e->button() != LeftButton ) {
        ratio = 0;
        keepRatio = false;
        return;
    }

    int mx = e->x();
    int my = e->y();
    mx = ( mx / rastX() ) * rastX();
    my = ( my / rastY() ) * rastY();
    firstX = ( firstX / rastX() ) * rastX();
    firstY = ( firstY / rastY() ) * rastY();
    QList<KPObject> _objects;
    _objects.setAutoDelete( false );
    KPObject *kpobject = 0;

    if ( toolEditMode != INS_LINE )
        insRect = insRect.normalize();

    QPoint mv;
    QSize sz;
    if ( toolEditMode == TEM_MOUSE && modType != MT_NONE && modType != MT_MOVE ) {
        kpobject = objectList()->at( resizeObjNum );
        if ( kpobject ) {
            mv = QPoint( kpobject->getOrig().x() - oldRect.x(),
                         kpobject->getOrig().y() - oldRect.y() );
            sz = QSize( kpobject->getSize().width() - oldRect.width(),
                        kpobject->getSize().height() - oldRect.height() );
        }
        kpobject = 0L;
    }

    switch ( toolEditMode ) {
    case TEM_MOUSE: {
        switch ( modType ) {
        case MT_NONE: {
            if ( drawRubber ) {
                QPainter p;
                p.begin( this );
                p.setRasterOp( NotROP );
                p.setPen( QPen( black, 0, DotLine ) );
                p.drawRect( rubber );
                p.end();
                drawRubber = false;

                rubber = rubber.normalize();
                KPObject *kpobject = 0;
                if ( (int)objectList()->count() - 1 >= 0 ) {
                    for ( int i = static_cast<int>( objectList()->count() ) - 1; i >= 0; i-- ) {
                        kpobject = objectList()->at( i );
                        if ( kpobject->intersects( rubber, diffx(), diffy() ) )
                            selectObj( kpobject );
                    }
                }
            }
        } break;
        case MT_MOVE: {
            if ( firstX != mx || firstY != my ) {
                if ( (int)objectList()->count() - 1 >= 0 ) {
                    for ( int i = static_cast<int>( objectList()->count() ) - 1; i >= 0; i-- ) {
                        kpobject = objectList()->at( i );
                        if ( kpobject->isSelected() ) {
                            kpobject->setMove( false );
                            _objects.append( kpobject );
                            _repaint( QRect( kpobject->getBoundingRect( 0, 0 ).x() + ( firstX - mx ),
                                             kpobject->getBoundingRect( 0, 0 ).y() + ( firstY - my ),
                                             kpobject->getBoundingRect( 0, 0 ).width(),
                                             kpobject->getBoundingRect( 0, 0 ).height() ) );
                            _repaint( kpobject );
                        }
                    }
                }
                MoveByCmd *moveByCmd = new MoveByCmd( i18n( "Move object(s)" ),
                                                      QPoint( mx - firstX, my - firstY ),
                                                      _objects, view->kPresenterDoc() );
                view->kPresenterDoc()->commands()->addCommand( moveByCmd );
            } else
                if ( (int)objectList()->count() - 1 >= 0 ) {
                    for ( int i = static_cast<int>( objectList()->count() ) - 1; i >= 0; i-- ) {
                        kpobject = objectList()->at( i );
                        if ( kpobject->isSelected() ) {
                            kpobject->setMove( false );
                            _repaint( kpobject );
                        }
                    }
                }
        } break;
        case MT_RESIZE_UP: {
            if ( resizeObjNum < 0 ) break;
            if ( firstX != mx || firstY != my ) {
                kpobject = objectList()->at( resizeObjNum );
                ResizeCmd *resizeCmd = new ResizeCmd( i18n( "Resize object up" ), mv, sz,
                                                      kpobject, view->kPresenterDoc() );
                kpobject->setMove( false );
                resizeCmd->unexecute( false );
                resizeCmd->execute();
                view->kPresenterDoc()->commands()->addCommand( resizeCmd );
            }
            kpobject = objectList()->at( resizeObjNum );
            kpobject->setMove( false );
            _repaint( oldBoundingRect );
            _repaint( kpobject );
        } break;
        case MT_RESIZE_DN: {
            if ( resizeObjNum < 0 ) break;
            if ( firstX != mx || firstY != my ) {
                kpobject = objectList()->at( resizeObjNum );
                ResizeCmd *resizeCmd = new ResizeCmd( i18n( "Resize object down" ), mv, sz,
                                                      kpobject, view->kPresenterDoc() );
                kpobject->setMove( false );
                resizeCmd->unexecute( false );
                resizeCmd->execute();
                view->kPresenterDoc()->commands()->addCommand( resizeCmd );
            }
            kpobject = objectList()->at( resizeObjNum );
            kpobject->setMove( false );
            _repaint( oldBoundingRect );
            _repaint( kpobject );
        } break;
        case MT_RESIZE_LF: {
            if ( resizeObjNum < 0 ) break;
            if ( firstX != mx || firstY != my ) {
                kpobject = objectList()->at( resizeObjNum );
                ResizeCmd *resizeCmd = new ResizeCmd( i18n( "Resize object left" ), mv, sz,
                                                      kpobject, view->kPresenterDoc() );
                kpobject->setMove( false );
                resizeCmd->unexecute( false );
                resizeCmd->execute();
                view->kPresenterDoc()->commands()->addCommand( resizeCmd );
            }
            kpobject = objectList()->at( resizeObjNum );
            kpobject->setMove( false );
            _repaint( oldBoundingRect );
            _repaint( kpobject );
        } break;
        case MT_RESIZE_RT: {
            if ( resizeObjNum < 0 ) break;
            if ( firstX != mx || firstY != my ) {
                kpobject = objectList()->at( resizeObjNum );
                ResizeCmd *resizeCmd = new ResizeCmd( i18n( "Resize object right" ), mv, sz,
                                                      kpobject, view->kPresenterDoc() );
                kpobject->setMove( false );
                resizeCmd->unexecute( false );
                resizeCmd->execute();
                view->kPresenterDoc()->commands()->addCommand( resizeCmd );
            }
            kpobject = objectList()->at( resizeObjNum );
            kpobject->setMove( false );
            _repaint( oldBoundingRect );
            _repaint( kpobject );
        } break;
        case MT_RESIZE_LU: {
            if ( resizeObjNum < 0 ) break;
            if ( firstX != mx || firstY != my ) {
                kpobject = objectList()->at( resizeObjNum );
                ResizeCmd *resizeCmd = new ResizeCmd( i18n( "Resize object left up" ), mv, sz,
                                                      kpobject, view->kPresenterDoc() );
                kpobject->setMove( false );
                resizeCmd->unexecute( false );
                resizeCmd->execute();
                view->kPresenterDoc()->commands()->addCommand( resizeCmd );
            }
            kpobject = objectList()->at( resizeObjNum );
            kpobject->setMove( false );
            _repaint( oldBoundingRect );
            _repaint( kpobject );
        } break;
        case MT_RESIZE_LD: {
            if ( resizeObjNum < 0 ) break;
            if ( firstX != mx || firstY != my ) {
                kpobject = objectList()->at( resizeObjNum );
                ResizeCmd *resizeCmd = new ResizeCmd( i18n( "Resize object left and down" ), mv, sz,
                                                      kpobject, view->kPresenterDoc() );
                kpobject->setMove( false );
                resizeCmd->unexecute( false );
                resizeCmd->execute();
                view->kPresenterDoc()->commands()->addCommand( resizeCmd );
            }
            kpobject = objectList()->at( resizeObjNum );
            kpobject->setMove( false );
            _repaint( oldBoundingRect );
            _repaint( kpobject );
        } break;
        case MT_RESIZE_RU: {
            if ( resizeObjNum < 0 ) break;
            if ( firstX != mx || firstY != my ) {
                kpobject = objectList()->at( resizeObjNum );
                ResizeCmd *resizeCmd = new ResizeCmd( i18n( "Resize object right and up" ), mv, sz,
                                                      kpobject, view->kPresenterDoc() );
                kpobject->setMove( false );
                resizeCmd->unexecute( false );
                resizeCmd->execute();
                view->kPresenterDoc()->commands()->addCommand( resizeCmd );
            }
            kpobject = objectList()->at( resizeObjNum );
            kpobject->setMove( false );
            _repaint( oldBoundingRect );
            _repaint( kpobject );
        } break;
        case MT_RESIZE_RD: {
            if ( resizeObjNum < 0 ) break;
            if ( firstX != mx || firstY != my ) {
                kpobject = objectList()->at( resizeObjNum );
                ResizeCmd *resizeCmd = new ResizeCmd( i18n( "Resize object right and down" ), mv, sz,
                                                      kpobject, view->kPresenterDoc() );
                kpobject->setMove( false );
                resizeCmd->unexecute( false );
                resizeCmd->execute();
                view->kPresenterDoc()->commands()->addCommand( resizeCmd );
            }
            kpobject = objectList()->at( resizeObjNum );
            kpobject->setMove( false );
            _repaint( oldBoundingRect );
            _repaint( kpobject );
        } break;
        }
    } break;
    case INS_TEXT: {
        if ( insRect.width() > 0 && insRect.height() > 0 ) {
            insertText( insRect );
            setToolEditMode( TEM_MOUSE );
        }
    } break;
    case INS_LINE: {
        if ( insRect.width() != 0 && insRect.height() != 0 ) {
            if ( insRect.top() == insRect.bottom() ) {
                bool reverse = insRect.left() > insRect.right();
                insRect = insRect.normalize();
                insRect.setRect( insRect.left(), insRect.top() - rastY() / 2,
                                 insRect.width(), rastY() );
                insertLineH( insRect, reverse );
            } else if ( insRect.left() == insRect.right() ) {
                bool reverse = insRect.top() > insRect.bottom();
                insRect = insRect.normalize();
                insRect.setRect( insRect.left() - rastX() / 2, insRect.top(),
                                 rastX(), insRect.height() );
                insertLineV( insRect, reverse );
            } else if ( insRect.left() < insRect.right() && insRect.top() < insRect.bottom() ||
                      insRect.left() > insRect.right() && insRect.top() > insRect.bottom() ) {
                bool reverse = insRect.left() > insRect.right() && insRect.top() > insRect.bottom();
                insertLineD1( insRect.normalize(), reverse );
            } else {
                bool reverse = insRect.right() < insRect.left() && insRect.top() < insRect.bottom();
                insertLineD2( insRect.normalize(), reverse );
            }
        }
    } break;
    case INS_RECT:
        if ( insRect.width() > 0 && insRect.height() > 0 ) insertRect( insRect );
        break;
    case INS_ELLIPSE:
        if ( insRect.width() > 0 && insRect.height() > 0 ) insertEllipse( insRect );
        break;
    case INS_PIE:
        if ( insRect.width() > 0 && insRect.height() > 0 ) insertPie( insRect );
        break;
    case INS_OBJECT: {
        if ( insRect.width() > 0 && insRect.height() > 0 ) insertObject( insRect );
        setToolEditMode( TEM_MOUSE );
    } break;
    case INS_DIAGRAMM: {
        if ( insRect.width() > 0 && insRect.height() > 0 ) insertDiagramm( insRect );
        setToolEditMode( TEM_MOUSE );
    } break;
    case INS_TABLE: {
        if ( insRect.width() > 0 && insRect.height() > 0 ) insertTable( insRect );
        setToolEditMode( TEM_MOUSE );
    } break;
    case INS_FORMULA: {
        if ( insRect.width() > 0 && insRect.height() > 0 ) insertFormula( insRect );
        setToolEditMode( TEM_MOUSE );
    } break;
    case INS_AUTOFORM: {
        bool reverse = insRect.left() > insRect.right() || insRect.top() > insRect.bottom();
        if ( insRect.width() > 0 && insRect.height() > 0 ) insertAutoform( insRect, reverse );
        setToolEditMode( TEM_MOUSE );
    } break;
    }
    emit objectSelectedChanged();
    if ( toolEditMode != TEM_MOUSE && editMode )
        repaint( false );

    mousePressed = false;
    modType = MT_NONE;
    resizeObjNum = -1;
    mouseMoveEvent( e );
    ratio = 0;
    keepRatio = false;
}

/*==================== handle mouse moved ========================*/
void Page::mouseMoveEvent( QMouseEvent *e )
{
    if ( editMode ) {
	view->setRulerMousePos( e->x(), e->y() );

	KPObject *kpobject;

	if ( ( !mousePressed || ( !drawRubber && modType == MT_NONE ) ) &&
	     toolEditMode == TEM_MOUSE ) {
	    bool cursorAlreadySet = false;
	    if ( (int)objectList()->count() - 1 >= 0 ) {
		for ( int i = static_cast<int>( objectList()->count() ) - 1; i >= 0; i-- ) {
		    kpobject = objectList()->at( i );
		    QSize s = kpobject->getSize();
		    QPoint pnt = kpobject->getOrig();
		    if ( QRect( pnt.x() - diffx(), pnt.y() - diffy(), s.width(), s.height() ).
			 contains( QPoint( e->x(), e->y() ) ) ) {
			if ( kpobject->isSelected() ) {
			    int dy = diffy();
			    setCursor( kpobject->getCursor( QPoint( e->x(), e->y() ), diffx(), dy, modType ) );
			    cursorAlreadySet = true;
			    break;
			}
		    }
		}
	    }
	    if ( !cursorAlreadySet )
		setCursor( arrowCursor );
	    else
		return;
	} else if ( mousePressed ) {
	    int mx = e->x();
	    int my = e->y();
	    mx = ( mx / rastX() ) * rastX();
	    my = ( my / rastY() ) * rastY();

	    switch ( toolEditMode ) {
	    case TEM_MOUSE: {
		oldMx = ( oldMx / rastX() ) * rastX();
		oldMy = ( oldMy / rastY() ) * rastY();

		if ( modType == MT_NONE ) {
		    if ( drawRubber ) {
			QPainter p;
			p.begin( this );
			p.setRasterOp( NotROP );
			p.setPen( QPen( black, 0, DotLine ) );
			p.drawRect( rubber );
			rubber.setRight( e->x() );
			rubber.setBottom( e->y() );
			p.drawRect( rubber );
			p.end();
		    }
		} else if ( modType == MT_MOVE ) {
		    moveObject( mx - oldMx, my - oldMy, false );
		} else if ( modType != MT_NONE && resizeObjNum != -1 ) {
		    QPainter p;
		    p.begin( this );

		    QRect oldRect;
		    kpobject = objectList()->at( resizeObjNum );
		    oldRect = kpobject->getBoundingRect( 0, 0 );
		    kpobject->setMove( true );
		    kpobject->draw( &p, diffx(), diffy() );

		    int dx = mx - oldMx;
		    int dy = my - oldMy;

		    switch ( modType ) {
		    case MT_RESIZE_LU: {
			if ( keepRatio && ratio != 0.0 )
			    if ( !calcRatio( dx, dy, kpobject, ratio ) )
				break;
			kpobject->moveBy( QPoint( dx, dy ) );
			kpobject->resizeBy( QSize( -dx, -dy ) );
			resizeObject();
		    } break;
		    case MT_RESIZE_LF: {
			dy = 0;
			if ( keepRatio && ratio != 0.0 )
			    if ( !calcRatio( dx, dy, kpobject, ratio ) )
				break;
			kpobject->moveBy( QPoint( dx, 0 ) );
			kpobject->resizeBy( QSize( -dx, -dy ) );
			resizeObject();
		    } break;
		    case MT_RESIZE_LD: {
			if ( keepRatio && ratio != 0.0 )
			    break;
			kpobject->moveBy( QPoint( dx, 0 ) );
			kpobject->resizeBy( QSize( -dx, dy ) );
			resizeObject();
		    } break;
		    case MT_RESIZE_RU: {
			if ( keepRatio && ratio != 0.0 )
			    break;
			kpobject->moveBy( QPoint( 0, dy ) );
			kpobject->resizeBy( QSize( dx, -dy ) );
			resizeObject();
		    } break;
		    case MT_RESIZE_RT: {
			dy = 0;
			if ( keepRatio && ratio != 0.0 )
			    if ( !calcRatio( dx, dy, kpobject, ratio ) )
				break;
			kpobject->resizeBy( QSize( dx, dy ) );
			resizeObject();
		    } break;
		    case MT_RESIZE_RD: {
			if ( keepRatio && ratio != 0.0 )
			    if ( !calcRatio( dx, dy, kpobject, ratio ) )
				break;
			kpobject->resizeBy( QSize( dx, dy ) );
			resizeObject();
		    } break;
		    case MT_RESIZE_UP: {
			dx = 0;
			if ( keepRatio && ratio != 0.0 )
			    if ( !calcRatio( dx, dy, kpobject, ratio ) )
				break;
			kpobject->moveBy( QPoint( 0, dy ) );
			kpobject->resizeBy( QSize( -dx, -dy ) );
			resizeObject();
		    } break;
		    case MT_RESIZE_DN: {
			dx = 0;
			if ( keepRatio && ratio != 0.0 )
			    if ( !calcRatio( dx, dy, kpobject, ratio ) )
				break;
			kpobject->resizeBy( QSize( dx, dy ) );
			resizeObject();
		    } break;
		    default: break;
		    }
		    kpobject->draw( &p, diffx(), diffy() );
		    p.end();
		}

		oldMx = e->x();
		oldMy = e->y();
	    } break;
	    case INS_TEXT: case INS_OBJECT: case INS_TABLE:
	    case INS_DIAGRAMM: case INS_FORMULA: case INS_AUTOFORM: {
		QPainter p( this );
		p.setPen( QPen( black, 1, SolidLine ) );
		p.setBrush( NoBrush );
		p.setRasterOp( NotROP );
		if ( insRect.width() != 0 && insRect.height() != 0 )
		    p.drawRect( insRect );
		insRect.setRight( ( ( e->x() + diffx() ) / rastX() ) * rastX() - diffx() );
		insRect.setBottom( ( ( e->y() + diffy() ) / rastY() ) * rastY() - diffy() );
		p.drawRect( insRect );
		p.end();

		mouseSelectedObject = true;
	    } break;
	    case INS_ELLIPSE: {
		QPainter p( this );
		p.setPen( QPen( black, 1, SolidLine ) );
		p.setBrush( NoBrush );
		p.setRasterOp( NotROP );
		if ( insRect.width() != 0 && insRect.height() != 0 )
		    p.drawEllipse( insRect );
		insRect.setRight( ( ( e->x() + diffx() ) / rastX() ) * rastX() - diffx() );
		insRect.setBottom( ( ( e->y() + diffy() ) / rastY() ) * rastY() - diffy() );
		p.drawEllipse( insRect );
		p.end();

		mouseSelectedObject = true;
	    } break;
	    case INS_RECT: {
		QPainter p( this );
		p.setPen( QPen( black, 1, SolidLine ) );
		p.setBrush( NoBrush );
		p.setRasterOp( NotROP );
		if ( insRect.width() != 0 && insRect.height() != 0 )
		    p.drawRoundRect( insRect, view->getRndX(), view->getRndY() );
		insRect.setRight( ( ( e->x() + diffx() ) / rastX() ) * rastX() - diffx() );
		insRect.setBottom( ( ( e->y() + diffy() ) / rastY() ) * rastY() - diffy() );
		p.drawRoundRect( insRect, view->getRndX(), view->getRndY() );
		p.end();

		mouseSelectedObject = true;
	    } break;
	    case INS_LINE: {
		QPainter p( this );
		p.setPen( QPen( black, 1, SolidLine ) );
		p.setBrush( NoBrush );
		p.setRasterOp( NotROP );
		if ( insRect.width() != 0 && insRect.height() != 0 )
		    p.drawLine( insRect.topLeft(), insRect.bottomRight() );
		insRect.setRight( ( ( e->x() + diffx() ) / rastX() ) * rastX() - diffx() );
		insRect.setBottom( ( ( e->y() + diffy() ) / rastY() ) * rastY() - diffy() );
		p.drawLine( insRect.topLeft(), insRect.bottomRight() );
		p.end();

		mouseSelectedObject = true;
	    } break;
	    case INS_PIE: {
		QPainter p( this );
		p.setPen( QPen( black, 1, SolidLine ) );
		p.setBrush( NoBrush );
		p.setRasterOp( NotROP );
		if ( insRect.width() != 0 && insRect.height() != 0 ) {
		    switch ( view->getPieType() ) {
		    case PT_PIE:
			p.drawPie( insRect.x(), insRect.y(), insRect.width() - 2,
				   insRect.height() - 2, view->getPieAngle(), view->getPieLength() );
			break;
		    case PT_ARC:
			p.drawArc( insRect.x(), insRect.y(), insRect.width() - 2,
				   insRect.height() - 2, view->getPieAngle(), view->getPieLength() );
			break;
		    case PT_CHORD:
			p.drawChord( insRect.x(), insRect.y(), insRect.width() - 2,
				     insRect.height() - 2, view->getPieAngle(), view->getPieLength() );
			break;
		    default: break;
		    }
		}
		insRect.setRight( ( ( e->x() + diffx() ) / rastX() ) * rastX() - diffx() );
		insRect.setBottom( ( ( e->y() + diffy() ) / rastY() ) * rastY() - diffy() );
		switch ( view->getPieType() ) {
		case PT_PIE:
		    p.drawPie( insRect.x(), insRect.y(), insRect.width() - 2,
			       insRect.height() - 2, view->getPieAngle(), view->getPieLength() );
		    break;
		case PT_ARC:
		    p.drawArc( insRect.x(), insRect.y(), insRect.width() - 2,
			       insRect.height() - 2, view->getPieAngle(), view->getPieLength() );
		    break;
		case PT_CHORD:
		    p.drawChord( insRect.x(), insRect.y(), insRect.width() - 2,
				 insRect.height() - 2, view->getPieAngle(), view->getPieLength() );
		    break;
		default: break;
		}
		p.end();

		mouseSelectedObject = true;
	    } break;
	    }
	}
    } else if ( !editMode && drawMode ) {
	QPainter p;
	p.begin( this );
	p.setPen( view->kPresenterDoc()->presPen() );
	p.drawLine( oldMx, oldMy, e->x(), e->y() );
	oldMx = e->x();
	oldMy = e->y();
	p.end();
    }
    if ( !editMode && !drawMode && !presMenu->isVisible() && fillBlack )
	setCursor( blankCursor );
}

/*==================== mouse double click ========================*/
void Page::mouseDoubleClickEvent( QMouseEvent *e )
{
 if(!view->koDocument()->isReadWrite())
    return;

    if ( toolEditMode != TEM_MOUSE || !editMode ) return;

    deSelectAllObj();
    KPObject *kpobject = 0;

    if ( (int)objectList()->count() - 1 >= 0 ) {
	for ( int i = static_cast<int>( objectList()->count() ) - 1; i >= 0; i-- ) {
	    kpobject = objectList()->at( i );
	    if ( kpobject->contains( QPoint( e->x(), e->y() ), diffx(), diffy() ) ) {
		if ( kpobject->getType() == OT_TEXT ) {
		    KPTextObject *kptextobject = dynamic_cast<KPTextObject*>( kpobject );

		    kpobject->activate( this, diffx(), diffy() );
		    setTextBackground( kptextobject );
		    connect( kptextobject->getKTextObject(), SIGNAL( currentFontChanged( const QFont & ) ),
			     this, SLOT( toFontChanged( const QFont & ) ) );
		    connect( kptextobject->getKTextObject(), SIGNAL( currentColorChanged( const QColor & ) ),
			     this, SLOT( toColorChanged( const QColor & ) ) );
		    connect( kptextobject->getKTextObject(), SIGNAL( currentAlignmentChanged( int ) ),
			     this, SLOT( toAlignChanged( int ) ) );
		    connect( kptextobject->getKTextObject(), SIGNAL( exitEditMode() ),
			     this, SLOT( exitEditMode() ) );
		    kptextobject->getKTextObject()->setFocus();
		    editNum = i;
		    break;
		} else if ( kpobject->getType() == OT_PART ) {
		    kpobject->activate( view, diffx(), diffy() );
		    editNum = i;
		    break;
		}
	    }
	}
    }
}

/*====================== mouse wheel event =========================*/
void Page::wheelEvent( QWheelEvent *e )
{
    if ( !editMode ) {
        if ( e->delta() == -120 )     // wheel down
            view->screenNext();
        else if ( e->delta() == 120 ) // wheel up
            view->screenPrev();
        e->accept();
    }
}


/*====================== key press event =========================*/
void Page::keyPressEvent( QKeyEvent *e )
{
    if ( !editMode ) {
	switch ( e->key() ) {
	case Key_Space: case Key_Right: case Key_Down: case Key_Next:
	    view->screenNext(); break;
	case Key_Backspace: case Key_Left: case Key_Up: case Key_Prior:
	    view->screenPrev(); break;
	case Key_Escape: case Key_Q: case Key_X:
	    view->screenStop(); break;
	case Key_G:
	    slotGotoPage(); break;
	default: break;
	}
    } else if ( editNum != -1 ) {
	if ( e->key() == Key_Escape ) {
            exitEditMode();
	} else if ( objectList()->at( editNum )->getType() == OT_TEXT )
	    QApplication::sendEvent( dynamic_cast<KPTextObject*>( objectList()->at( editNum ) )->
				     getKTextObject(), e );
    } else if ( mouseSelectedObject ) {
	if ( e->state() & ControlButton ) {
	    switch ( e->key() ) {
	    case Key_Up:
		moveObject( 0, -10, true );
		break;
	    case Key_Down:
		moveObject( 0, 10, true );
		break;
	    case Key_Right:
		moveObject( 10, 0, true );
		break;
	    case Key_Left:
		moveObject( -10, 0, true );
		break;
	    default: break;
	    }
	} else {
	    switch ( e->key() ) {
	    case Key_Up:
		moveObject( 0, -1, true );
		break;
	    case Key_Down:
		moveObject( 0, 1, true );
		break;
	    case Key_Right:
		moveObject( 1, 0, true );
		break;
	    case Key_Left:
		moveObject( -1, 0, true );
		break;
	    case Key_Delete: case Key_Backspace:
		view->editDelete();
		break;
	    case Key_Escape:
		setToolEditMode( TEM_MOUSE );
		break;
	    default: break;
	    }
	}
    } else {
	switch ( e->key() ) {
	case Key_Next:
	    view->nextPage();
	    break;
	case Key_Prior:
	    view->prevPage();
	    break;
	case Key_Down:
	    view->getVScrollBar()->addLine();
	    break;
	case Key_Up:
	    view->getVScrollBar()->subtractLine();
	    break;
	case Key_Right:
	    view->getHScrollBar()->addLine();
	    break;
	case Key_Left:
	    view->getHScrollBar()->subtractLine();
	    break;
	case Key_Tab:
	    selectNext();
	    break;
	case Key_Backtab:
	    selectPrev();
	    break;
	case Key_Home:
	    view->getVScrollBar()->setValue( 0 );
	    break;
	case Key_End:
	    view->getVScrollBar()->setValue( view->getVScrollBar()->maxValue());
	    break;
	default: break;
	}
    }
}

/*========================= resize Event =========================*/
void Page::resizeEvent( QResizeEvent *e )
{
    if ( editMode )
        QWidget::resizeEvent( e );
    else
        QWidget::resizeEvent( new QResizeEvent( QApplication::desktop()->size(),
                                                e->oldSize() ) );
    if ( editMode )
        buffer.resize( size() );
}

/*========================== get object ==========================*/
int Page::getObjectAt( int x, int y )
{
    for ( int i = static_cast<int>( objectList()->count() ) - 1; i >= 0 ; i-- ) {
        KPObject * kpobject = objectList()->at( i );
        if ( kpobject->contains( QPoint( x, y ), diffx(), diffy() ) )
            return i;
    }

    return -1;
}

/*================================================================*/
int Page::diffx() const
{
    return view->getDiffX();
}

/*================================================================*/
int Page::diffy() const
{
    return view->getDiffY();
}
/*======================= select object ==========================*/
void Page::selectObj( int num )
{
    if ( num < static_cast<int>( objectList()->count() ) ) {
        selectObj( objectList()->at( num ) );
        if ( objectList()->at( num )->getType() == OT_TEXT ) {
            KPTextObject *kptextobject = dynamic_cast<KPTextObject*>( objectList()->at( num ) );
            QFont f( kptextobject->getKTextObject()->font() );
            QColor c( kptextobject->getKTextObject()->color() );
            toFontChanged( f );
            toColorChanged( c );
            toAlignChanged( kptextobject->getKTextObject()->alignment() );
        }
        emit objectSelectedChanged();
    }
}

/*======================= deselect object ========================*/
void Page::deSelectObj( int num )
{
    if ( num < static_cast<int>( objectList()->count() ) )
        deSelectObj( objectList()->at( num ) );
}

/*======================= select object ==========================*/
void Page::selectObj( KPObject *kpobject )
{
    kpobject->setSelected( true );
    if ( kpobject->getType() == OT_TEXT ) {
        KPTextObject *kptextobject = dynamic_cast<KPTextObject*>( kpobject );
        QFont f( kptextobject->getKTextObject()->font() );
        QColor c( kptextobject->getKTextObject()->color() );
        toFontChanged( f );
        toColorChanged( c );
        toAlignChanged( kptextobject->getKTextObject()->alignment() );
    }
    _repaint( kpobject );
    emit objectSelectedChanged();

    mouseSelectedObject = true;
}

/*======================= deselect object ========================*/
void Page::deSelectObj( KPObject *kpobject )
{
    kpobject->setSelected( false );
    _repaint( kpobject );

    mouseSelectedObject = false;
    emit objectSelectedChanged();
}

/*====================== select all objects ======================*/
void Page::selectAllObj()
{
    QProgressDialog progress( i18n( "Selecting..." ), 0,
                              objectList()->count(), this );

    for ( uint i = 0; i <= objectList()->count(); i++ ) {
        selectObj( i );

        progress.setProgress( i );
        kapp->processEvents();
    }

    mouseSelectedObject = true;
    emit objectSelectedChanged();
}


/*==================== deselect all objects ======================*/
void Page::deSelectAllObj()
{
    KPObject *kpobject;

    for ( int i = 0; i < static_cast<int>( objectList()->count() ); i++ )
    {
        kpobject = objectList()->at( i );
        if ( kpobject->isSelected() ) deSelectObj( kpobject );
    }

    mouseSelectedObject = false;
    emit objectSelectedChanged();
}


void Page::setMouseSelectedObject(bool b)
{
    mouseSelectedObject = b;
    emit objectSelectedChanged();
}

/*======================== setup menus ===========================*/
void Page::setupMenus()
{
    // create right button object align menu
    alignMenu1 = new QPopupMenu();
    CHECK_PTR( alignMenu1 );
    alignMenu1->insertItem( KPBarIcon( "aoleft" ), this, SLOT( alignObjLeft() ) );
    alignMenu1->insertSeparator();
    alignMenu1->insertItem( KPBarIcon( "aocenterh" ), this, SLOT( alignObjCenterH() ) );
    alignMenu1->insertSeparator();
    alignMenu1->insertItem( KPBarIcon( "aoright" ), this, SLOT( alignObjRight() ) );
    alignMenu1->insertSeparator();
    alignMenu1->insertItem( KPBarIcon( "aotop" ), this, SLOT( alignObjTop() ) );
    alignMenu1->insertSeparator();
    alignMenu1->insertItem( KPBarIcon( "aocenterv" ), this, SLOT( alignObjCenterV() ) );
    alignMenu1->insertSeparator();
    alignMenu1->insertItem( KPBarIcon( "aobottom" ), this, SLOT( alignObjBottom() ) );
    alignMenu1->setMouseTracking( true );
    alignMenu1->setCheckable( false );

    alignMenu2 = new QPopupMenu();
    CHECK_PTR( alignMenu2 );
    alignMenu2->insertItem( KPBarIcon( "aoleft" ), this, SLOT( alignObjLeft() ) );
    alignMenu2->insertSeparator();
    alignMenu2->insertItem( KPBarIcon( "aocenterh" ), this, SLOT( alignObjCenterH() ) );
    alignMenu2->insertSeparator();
    alignMenu2->insertItem( KPBarIcon( "aoright" ), this, SLOT( alignObjRight() ) );
    alignMenu2->insertSeparator();
    alignMenu2->insertItem( KPBarIcon( "aotop" ), this, SLOT( alignObjTop() ) );
    alignMenu2->insertSeparator();
    alignMenu2->insertItem( KPBarIcon( "aocenterv" ), this, SLOT( alignObjCenterV() ) );
    alignMenu2->insertSeparator();
    alignMenu2->insertItem( KPBarIcon( "aobottom" ), this, SLOT( alignObjBottom() ) );
    alignMenu2->setMouseTracking( true );
    alignMenu2->setCheckable( false );

    alignMenu3 = new QPopupMenu();
    CHECK_PTR( alignMenu3 );
    alignMenu3->insertItem( KPBarIcon( "aoleft" ), this, SLOT( alignObjLeft() ) );
    alignMenu3->insertSeparator();
    alignMenu3->insertItem( KPBarIcon( "aocenterh" ), this, SLOT( alignObjCenterH() ) );
    alignMenu3->insertSeparator();
    alignMenu3->insertItem( KPBarIcon( "aoright" ), this, SLOT( alignObjRight() ) );
    alignMenu3->insertSeparator();
    alignMenu3->insertItem( KPBarIcon( "aotop" ), this, SLOT( alignObjTop() ) );
    alignMenu3->insertSeparator();
    alignMenu3->insertItem( KPBarIcon( "aocenterv" ), this, SLOT( alignObjCenterV() ) );
    alignMenu3->insertSeparator();
    alignMenu3->insertItem( KPBarIcon( "aobottom" ), this, SLOT( alignObjBottom() ) );
    alignMenu3->setMouseTracking( true );
    alignMenu3->setCheckable( false );

    alignMenu4 = new QPopupMenu();
    CHECK_PTR( alignMenu4 );
    alignMenu4->insertItem( KPBarIcon( "aoleft" ), this, SLOT( alignObjLeft() ) );
    alignMenu4->insertSeparator();
    alignMenu4->insertItem( KPBarIcon( "aocenterh" ), this, SLOT( alignObjCenterH() ) );
    alignMenu4->insertSeparator();
    alignMenu4->insertItem( KPBarIcon( "aoright" ), this, SLOT( alignObjRight() ) );
    alignMenu4->insertSeparator();
    alignMenu4->insertItem( KPBarIcon( "aotop" ), this, SLOT( alignObjTop() ) );
    alignMenu4->insertSeparator();
    alignMenu4->insertItem( KPBarIcon( "aocenterv" ), this, SLOT( alignObjCenterV() ) );
    alignMenu4->insertSeparator();
    alignMenu4->insertItem( KPBarIcon( "aobottom" ), this, SLOT( alignObjBottom() ) );
    alignMenu4->setMouseTracking( true );
    alignMenu4->setCheckable( false );

    alignMenu5 = new QPopupMenu();
    CHECK_PTR( alignMenu5 );
    alignMenu5->insertItem( KPBarIcon( "aoleft" ), this, SLOT( alignObjLeft() ) );
    alignMenu5->insertSeparator();
    alignMenu5->insertItem( KPBarIcon( "aocenterh" ), this, SLOT( alignObjCenterH() ) );
    alignMenu5->insertSeparator();
    alignMenu5->insertItem( KPBarIcon( "aoright" ), this, SLOT( alignObjRight() ) );
    alignMenu5->insertSeparator();
    alignMenu5->insertItem( KPBarIcon( "aotop" ), this, SLOT( alignObjTop() ) );
    alignMenu5->insertSeparator();
    alignMenu5->insertItem( KPBarIcon( "aocenterv" ), this, SLOT( alignObjCenterV() ) );
    alignMenu5->insertSeparator();
    alignMenu5->insertItem( KPBarIcon( "aobottom" ), this, SLOT( alignObjBottom() ) );
    alignMenu5->setMouseTracking( true );
    alignMenu5->setCheckable( false );

    alignMenu6 = new QPopupMenu();
    CHECK_PTR( alignMenu6 );
    alignMenu6->insertItem( KPBarIcon( "aoleft" ), this, SLOT( alignObjLeft() ) );
    alignMenu6->insertSeparator();
    alignMenu6->insertItem( KPBarIcon( "aocenterh" ), this, SLOT( alignObjCenterH() ) );
    alignMenu6->insertSeparator();
    alignMenu6->insertItem( KPBarIcon( "aoright" ), this, SLOT( alignObjRight() ) );
    alignMenu6->insertSeparator();
    alignMenu6->insertItem( KPBarIcon( "aotop" ), this, SLOT( alignObjTop() ) );
    alignMenu6->insertSeparator();
    alignMenu6->insertItem( KPBarIcon( "aocenterv" ), this, SLOT( alignObjCenterV() ) );
    alignMenu6->insertSeparator();
    alignMenu6->insertItem( KPBarIcon( "aobottom" ), this, SLOT( alignObjBottom() ) );
    alignMenu6->setMouseTracking( true );
    alignMenu6->setCheckable( false );

    alignMenu7 = new QPopupMenu();
    CHECK_PTR( alignMenu7 );
    alignMenu7->insertItem( KPBarIcon( "aoleft" ), this, SLOT( alignObjLeft() ) );
    alignMenu7->insertSeparator();
    alignMenu7->insertItem( KPBarIcon( "aocenterh" ), this, SLOT( alignObjCenterH() ) );
    alignMenu7->insertSeparator();
    alignMenu7->insertItem( KPBarIcon( "aoright" ), this, SLOT( alignObjRight() ) );
    alignMenu7->insertSeparator();
    alignMenu7->insertItem( KPBarIcon( "aotop" ), this, SLOT( alignObjTop() ) );
    alignMenu7->insertSeparator();
    alignMenu7->insertItem( KPBarIcon( "aocenterv" ), this, SLOT( alignObjCenterV() ) );
    alignMenu7->insertSeparator();
    alignMenu7->insertItem( KPBarIcon( "aobottom" ), this, SLOT( alignObjBottom() ) );
    alignMenu7->setMouseTracking( true );
    alignMenu7->setCheckable( false );

    // create right button graph menu
    graphMenu = new QPopupMenu();
    CHECK_PTR( graphMenu );
    graphMenu->insertItem( SmallIcon("editcut"), i18n( "&Cut" ), this, SLOT( clipCut() ) );
    graphMenu->insertItem( SmallIcon("editcopy"), i18n( "C&opy" ), this, SLOT( clipCopy() ) );
    graphMenu->insertItem( SmallIcon("editdelete"), i18n( "&Delete" ), this, SLOT( deleteObjs() ) );
    graphMenu->insertSeparator();
    graphMenu->insertItem( KPBarIcon( "rotate" ), i18n( "&Rotate..." ), this, SLOT( rotateObjs() ) );
    graphMenu->insertItem( KPBarIcon( "shadow" ), i18n( "&Shadow..." ), this, SLOT( shadowObjs() ) );
    graphMenu->insertSeparator();
    graphMenu->insertItem( KPBarIcon( "style" ), i18n( "&Properties..." ), this, SLOT( objProperties() ) );
    graphMenu->insertSeparator();
    graphMenu->insertItem( KPBarIcon( "effect" ), i18n( "&Assign effect..." ), this, SLOT( assignEffect() ) );
    graphMenu->insertSeparator();
    graphMenu->insertItem( KPBarIcon( "alignobjs" ), i18n( "&Align objects" ), alignMenu1 );
    graphMenu->setMouseTracking( true );

    // create right button part menu
    partMenu = new QPopupMenu();
    CHECK_PTR( partMenu );
    partMenu->insertItem( SmallIcon("editcut"), i18n( "&Cut" ), this, SLOT( clipCut() ) );
    partMenu->insertItem( SmallIcon("editcopy"), i18n( "C&opy" ), this, SLOT( clipCopy() ) );
    partMenu->insertItem( SmallIcon("editdelete"), i18n( "&Delete" ), this, SLOT( deleteObjs() ) );
    partMenu->insertSeparator();
    partMenu->insertItem( KPBarIcon( "rotate" ), i18n( "&Rotate..." ), this, SLOT( rotateObjs() ) );
    partMenu->insertSeparator();
    partMenu->insertItem( KPBarIcon( "style" ), i18n( "&Properties..." ), this, SLOT( objProperties() ) );
    partMenu->insertSeparator();
    partMenu->insertItem( KPBarIcon( "effect" ), i18n( "&Assign effect..." ), this, SLOT( assignEffect() ) );
    partMenu->insertSeparator();
    partMenu->insertItem( KPBarIcon( "alignobjs" ), i18n( "&Align objects" ), alignMenu7 );
    partMenu->setMouseTracking( true );

  // create right button rect menu
    rectMenu = new QPopupMenu();
    CHECK_PTR( rectMenu );
    rectMenu->insertItem( SmallIcon("editcut"), i18n( "&Cut" ), this, SLOT( clipCut() ) );
    rectMenu->insertItem( SmallIcon("editcopy"), i18n( "C&opy" ), this, SLOT( clipCopy() ) );
    rectMenu->insertItem( SmallIcon("editdelete"), i18n( "&Delete" ), this, SLOT( deleteObjs() ) );
    rectMenu->insertSeparator();
    rectMenu->insertItem( KPBarIcon( "rotate" ), i18n( "&Rotate..." ), this, SLOT( rotateObjs() ) );
    rectMenu->insertItem( KPBarIcon( "shadow" ), i18n( "&Shadow..." ), this, SLOT( shadowObjs() ) );
    rectMenu->insertSeparator();
    rectMenu->insertItem( KPBarIcon( "style" ), i18n( "&Properties..." ), this, SLOT( objProperties() ) );
    rectMenu->insertItem( KPBarIcon( "rectangle2" ), i18n( "&Configure Rectangle..." ), this, SLOT( objConfigRect() ) );
    rectMenu->insertSeparator();
    rectMenu->insertItem( KPBarIcon( "effect" ), i18n( "&Assign effect..." ), this, SLOT( assignEffect() ) );
    rectMenu->insertSeparator();
    rectMenu->insertItem( KPBarIcon( "alignobjs" ), i18n( "&Align objects" ), alignMenu6 );
    rectMenu->setMouseTracking( true );

  // create right button pie menu
    pieMenu = new QPopupMenu();
    CHECK_PTR( pieMenu );
    pieMenu->insertItem( SmallIcon("editcut"), i18n( "&Cut" ), this, SLOT( clipCut() ) );
    pieMenu->insertItem( SmallIcon("editcopy"), i18n( "C&opy" ), this, SLOT( clipCopy() ) );
    pieMenu->insertItem( SmallIcon("editdelete"), i18n( "&Delete" ), this, SLOT( deleteObjs() ) );
    pieMenu->insertSeparator();
    pieMenu->insertItem( KPBarIcon( "rotate" ), i18n( "&Rotate..." ), this, SLOT( rotateObjs() ) );
    pieMenu->insertItem( KPBarIcon( "shadow" ), i18n( "&Shadow..." ), this, SLOT( shadowObjs() ) );
    pieMenu->insertSeparator();
    pieMenu->insertItem( KPBarIcon( "style" ), i18n( "&Properties..." ), this, SLOT( objProperties() ) );
    pieMenu->insertItem( KPBarIcon( "edit_pie" ), i18n( "&Configure Pie/Arc/Chord..." ), this, SLOT( objConfigPie() ) );
    pieMenu->insertSeparator();
    pieMenu->insertItem( KPBarIcon( "effect" ), i18n( "&Assign effect..." ), this, SLOT( assignEffect() ) );
    pieMenu->insertSeparator();
    pieMenu->insertItem( KPBarIcon( "alignobjs" ), i18n( "&Align objects" ), alignMenu5 );
    pieMenu->setMouseTracking( true );

    // pic-resize menu
    picResizeMenu = new QPopupMenu();
    picResizeMenu->insertItem( i18n( "640x480" ), this, SLOT( picViewOrig640x480() ) );
    picResizeMenu->insertItem( i18n( "800x600" ), this, SLOT( picViewOrig800x600() ) );
    picResizeMenu->insertItem( i18n( "1024x768" ), this, SLOT( picViewOrig1024x768() ) );
    picResizeMenu->insertItem( i18n( "1280x1024" ), this, SLOT( picViewOrig1280x1024() ) );
    picResizeMenu->insertItem( i18n( "1600x1200" ), this, SLOT( picViewOrig1600x1200() ) );
#if 0
    picResizeMenu->insertSeparator();
    picResizeMenu->insertItem( i18n( "Enter Custom Factor..." ), this, SLOT( picViewOrigFactor() ) );
#endif

    // create right button picture menu
    picMenu = new QPopupMenu();
    CHECK_PTR( picMenu );
    picMenu->insertItem( SmallIcon("editcut"), i18n( "&Cut" ), this, SLOT( clipCut() ) );
    picMenu->insertItem( SmallIcon("editcopy"), i18n( "C&opy" ), this, SLOT( clipCopy() ) );
    picMenu->insertItem( SmallIcon("editdelete"), i18n( "&Delete" ), this, SLOT( deleteObjs() ) );
    picMenu->insertSeparator();
    picMenu->insertItem( KPBarIcon( "rotate" ), i18n( "&Rotate..." ), this, SLOT( rotateObjs() ) );
    picMenu->insertItem( KPBarIcon( "shadow" ), i18n( "&Shadow..." ), this, SLOT( shadowObjs() ) );
    picMenu->insertSeparator();
    picMenu->insertItem( KPBarIcon( "frame_image" ), i18n( "&Change Picture..." ), this, SLOT( chPic() ) );
    picMenu->insertItem( KPBarIcon( "style" ), i18n( "&Properties..." ), this, SLOT( objProperties() ) );
    picMenu->insertSeparator();
    picMenu->insertItem( KPBarIcon( "effect" ), i18n( "&Assign effect..." ), this, SLOT( assignEffect() ) );
    picMenu->insertSeparator();
    picMenu->insertItem( KPBarIcon( "alignobjs" ), i18n( "&Align objects" ), alignMenu2 );
    picMenu->insertSeparator();
    picMenu->insertItem( i18n( "&Scale to show the Picture 1:1 in" ), picResizeMenu );
    picMenu->setMouseTracking( true );

  // create right button clipart menu
    clipMenu = new QPopupMenu();
    CHECK_PTR( clipMenu );
    clipMenu->insertItem( SmallIcon("editcut"), i18n( "&Cut" ), this, SLOT( clipCut() ) );
    clipMenu->insertItem( SmallIcon("editcopy"), i18n( "C&opy" ), this, SLOT( clipCopy() ) );
    clipMenu->insertItem( SmallIcon("editdelete"), i18n( "&Delete" ), this, SLOT( deleteObjs() ) );
    clipMenu->insertSeparator();
    clipMenu->insertItem( KPBarIcon( "rotate" ), i18n( "&Rotate..." ), this, SLOT( rotateObjs() ) );
    clipMenu->insertItem( KPBarIcon( "shadow" ), i18n( "&Shadow..." ), this, SLOT( shadowObjs() ) );
    clipMenu->insertSeparator();
    clipMenu->insertItem( KPBarIcon( "clipart" ), i18n( "&Change Clipart..." ), this, SLOT( chClip() ) );
    clipMenu->insertItem( KPBarIcon( "style" ), i18n( "&Properties..." ), this, SLOT( objProperties() ) );
    clipMenu->insertSeparator();
    clipMenu->insertItem( KPBarIcon( "effect" ), i18n( "&Assign effect..." ), this, SLOT( assignEffect() ) );
    clipMenu->insertSeparator();
    clipMenu->insertItem( KPBarIcon( "alignobjs" ), i18n( "&Align objects" ), alignMenu3 );
    clipMenu->setMouseTracking( true );

  // create right button text menu
    txtMenu = new QPopupMenu();
    CHECK_PTR( txtMenu );
    txtMenu->insertItem( SmallIcon("editcut"), i18n( "&Cut" ), this, SLOT( clipCut() ) );
    txtMenu->insertItem( SmallIcon("editcopy"), i18n( "C&opy" ), this, SLOT( clipCopy() ) );
    txtMenu->insertItem( SmallIcon("editdelete"), i18n( "&Delete" ), this, SLOT( deleteObjs() ) );
    txtMenu->insertSeparator();
    txtMenu->insertItem( KPBarIcon( "rotate" ), i18n( "&Rotate..." ), this, SLOT( rotateObjs() ) );
    txtMenu->insertItem( KPBarIcon( "shadow" ), i18n( "&Shadow..." ), this, SLOT( shadowObjs() ) );
    txtMenu->insertSeparator();
    txtMenu->insertItem( KPBarIcon( "style" ), i18n( "&Properties..." ), this, SLOT( objProperties() ) );
    txtMenu->insertItem( SmallIcon( "configure" ), i18n( "&Settings..." ), view, SLOT( textSettings() ) );
    txtMenu->insertSeparator();
    txtMenu->insertItem( KPBarIcon( "effect" ), i18n( "&Assign effect..." ), this, SLOT( assignEffect() ) );
    txtMenu->insertSeparator();
    txtMenu->insertItem( KPBarIcon( "alignobjs" ), i18n( "&Align objects" ), alignMenu4 );
    txtMenu->insertSeparator();
    txtMenu->insertItem( i18n( "&Extend Contents to Object Height" ), this, SLOT( slotTextContents2Height() ) );
    txtMenu->insertItem( i18n( "&Resize Object to fit the Contents" ), this, SLOT( slotTextObj2Contents() ) );
    txtMenu->setMouseTracking( true );

    // create right button presentation menu
    presMenu = new QPopupMenu();
    CHECK_PTR( presMenu );
    presMenu->setCheckable( true );
    PM_SM = presMenu->insertItem( i18n( "&Switching mode" ), this, SLOT( switchingMode() ) );
    PM_DM = presMenu->insertItem( i18n( "&Drawing mode" ), this, SLOT( drawingMode() ) );
    presMenu->insertSeparator();
    presMenu->insertItem( SmallIcon("goto"), i18n( "&Goto Page..." ), this, SLOT( slotGotoPage() ) );
    presMenu->insertSeparator();
    presMenu->insertItem( i18n( "&Exit Presentation" ), this, SLOT( slotExitPres() ) );
    presMenu->setItemChecked( PM_SM, true );
    presMenu->setItemChecked( PM_DM, false );
    presMenu->setMouseTracking( true );

  // create right button page menu
    pageMenu = new QPopupMenu();
    CHECK_PTR( pageMenu );
    pageMenu->insertItem( i18n( "Pa&ge Layout..." ), this, SLOT( pageLayout() ) );
    pageMenu->insertItem( SmallIcon("background"), i18n( "Page &Background..." ), this, SLOT( pageBackground() ) );
    pageMenu->insertSeparator();
    pageMenu->insertItem( i18n( "&Configure pages..." ), this, SLOT( configPages() ) );
    pageMenu->insertItem( i18n( "&Open presentation structure viewer..." ), this, SLOT( presStructView() ) );
    pageMenu->insertSeparator();
    pageMenu->insertItem( i18n( "&Insert Page..." ), this, SLOT( pageInsert() ) );
    pageMenu->insertItem( i18n( "&Use current slide as default template" ), this, SLOT( pageDefaultTemplate() ) );
    pageMenu->insertItem( KPBarIcon( "newslide" ), i18n( "&Duplicate Page" ), this, SLOT( duplicateCopy() ) );
    delPageId = pageMenu->insertItem( KPBarIcon( "delslide" ), i18n( "D&elete Page..." ), this, SLOT( pageDelete() ) );
    pageMenu->insertSeparator();
    pageMenu->insertItem( i18n( "Edit &Header/Footer..." ), this, SLOT( slotEditHF() ) );
    pageMenu->insertSeparator();
    pageMenu->insertItem( SmallIcon("editpaste"), i18n( "&Paste" ), this, SLOT( pagePaste() ) );
    pageMenu->setMouseTracking( true );
}

/*======================== clipboard cut =========================*/
void Page::clipCut()
{
    if ( editNum != -1 && objectList()->at( editNum )->getType() == OT_TEXT )
        dynamic_cast<KPTextObject*>( objectList()->at( editNum ) )->getKTextObject()->cut();
    view->editCut();
}

/*======================== clipboard copy ========================*/
void Page::clipCopy()
{
    if ( editNum != -1 && objectList()->at( editNum )->getType() == OT_TEXT )
        dynamic_cast<KPTextObject*>( objectList()->at( editNum ) )->getKTextObject()->copy();
    view->editCopy();
}

/*====================== clipboard paste =========================*/
void Page::clipPaste()
{
    if ( editNum != -1 && objectList()->at( editNum )->getType() == OT_TEXT )
        dynamic_cast<KPTextObject*>( objectList()->at( editNum ) )->getKTextObject()->paste();
    view->editPaste();
}

/*======================= object properties  =====================*/
void Page::objProperties()
{
    view->extraPenBrush();
}

/*======================= change picture  ========================*/
void Page::chPic()
{
    KPObject *kpobject = 0;

    for ( unsigned int i = 0; i < objectList()->count(); i++ )
    {
        kpobject = objectList()->at( i );
        if ( kpobject->isSelected() && kpobject->getType() == OT_PICTURE )
        {
            view->changePicture( dynamic_cast<KPPixmapObject*>( kpobject )->getFileName() );
            break;
        }
    }
}

/*======================= change clipart  ========================*/
void Page::chClip()
{
    KPObject *kpobject = 0;

    for ( unsigned int i = 0; i < objectList()->count(); i++ )
    {
        kpobject = objectList()->at( i );
        if ( kpobject->isSelected() && kpobject->getType() == OT_CLIPART )
        {
            view->changeClipart( dynamic_cast<KPClipartObject*>( kpobject )->getFileName() );
            break;
        }
    }
}

/*======================= set text font ==========================*/
void Page::setTextFont( const QFont &font )
{
    if ( editNum != -1 && objectList()->at( editNum )->getType() == OT_TEXT ) {
	dynamic_cast<KPTextObject*>( objectList()->at( editNum ) )->getKTextObject()->setFocus();
	dynamic_cast<KPTextObject*>( objectList()->at( editNum ) )->getKTextObject()->setFont( font );
    } else {
	KPObject *kpobject = 0;

	for ( unsigned int i = 0; i < objectList()->count(); i++ ) {
	    kpobject = objectList()->at( i );
	    if ( kpobject->isSelected() && kpobject->getType() == OT_TEXT ) {
		dynamic_cast<KPTextObject*>( kpobject )->getKTextObject()->document()->setFontToAll( font );
		dynamic_cast<KPTextObject*>( kpobject )->getKTextObject()->updateCurrentFormat();
	    }
	}
	repaint( false );
    }
}

/*======================= set text color =========================*/
void Page::setTextColor( const QColor &color )
{
    if ( editNum != -1 && objectList()->at( editNum )->getType() == OT_TEXT ) {
	dynamic_cast<KPTextObject*>( objectList()->at( editNum ) )->getKTextObject()->setFocus();
	dynamic_cast<KPTextObject*>( objectList()->at( editNum ) )->getKTextObject()->setColor( color );
    } else {
	KPObject *kpobject = 0;

	for ( unsigned int i = 0; i < objectList()->count(); i++ )
	{
	    kpobject = objectList()->at( i );
	    if ( kpobject->isSelected() && kpobject->getType() == OT_TEXT ) {
		dynamic_cast<KPTextObject*>( kpobject )->getKTextObject()->document()->setColorToAll( color );
		dynamic_cast<KPTextObject*>( kpobject )->getKTextObject()->updateCurrentFormat();
	    }
	}
	repaint( false );
    }
}

void Page::setTextBold( bool b )
{
    if ( editNum != -1 && objectList()->at( editNum )->getType() == OT_TEXT ) {
	dynamic_cast<KPTextObject*>( objectList()->at( editNum ) )->getKTextObject()->setFocus();
	dynamic_cast<KPTextObject*>( objectList()->at( editNum ) )->getKTextObject()->setBold( b );
    } else {
	KPObject *kpobject = 0;

	for ( unsigned int i = 0; i < objectList()->count(); i++ ) {
	    kpobject = objectList()->at( i );
	    if ( kpobject->isSelected() && kpobject->getType() == OT_TEXT ) {
		dynamic_cast<KPTextObject*>( kpobject )->getKTextObject()->document()->setBoldToAll( b );
		dynamic_cast<KPTextObject*>( kpobject )->getKTextObject()->updateCurrentFormat();
	    }
	}
	repaint( false );
    }
}

void Page::setTextItalic( bool b )
{
    if ( editNum != -1 && objectList()->at( editNum )->getType() == OT_TEXT ) {
	dynamic_cast<KPTextObject*>( objectList()->at( editNum ) )->getKTextObject()->setFocus();
	dynamic_cast<KPTextObject*>( objectList()->at( editNum ) )->getKTextObject()->setItalic( b );
    } else {
	KPObject *kpobject = 0;

	for ( unsigned int i = 0; i < objectList()->count(); i++ ) {
	    kpobject = objectList()->at( i );
	    if ( kpobject->isSelected() && kpobject->getType() == OT_TEXT ) {
		dynamic_cast<KPTextObject*>( kpobject )->getKTextObject()->document()->setItalicToAll( b );
		dynamic_cast<KPTextObject*>( kpobject )->getKTextObject()->updateCurrentFormat();
	    }
	}
	repaint( false );
    }
}

void Page::setTextUnderline( bool b )
{
    if ( editNum != -1 && objectList()->at( editNum )->getType() == OT_TEXT ) {
	dynamic_cast<KPTextObject*>( objectList()->at( editNum ) )->getKTextObject()->setFocus();
	dynamic_cast<KPTextObject*>( objectList()->at( editNum ) )->getKTextObject()->setUnderline( b );
    } else {
	KPObject *kpobject = 0;

	for ( unsigned int i = 0; i < objectList()->count(); i++ ) {
	    kpobject = objectList()->at( i );
	    if ( kpobject->isSelected() && kpobject->getType() == OT_TEXT ) {
		dynamic_cast<KPTextObject*>( kpobject )->getKTextObject()->document()->setUnderlineToAll( b );
		dynamic_cast<KPTextObject*>( kpobject )->getKTextObject()->updateCurrentFormat();
	    }
	}
	repaint( false );
    }
}

void Page::setTextFamily( const QString &f )
{
    if ( editNum != -1 && objectList()->at( editNum )->getType() == OT_TEXT ) {
	dynamic_cast<KPTextObject*>( objectList()->at( editNum ) )->getKTextObject()->setFocus();
	dynamic_cast<KPTextObject*>( objectList()->at( editNum ) )->getKTextObject()->setFamily( f );
    } else {
	KPObject *kpobject = 0;

	for ( unsigned int i = 0; i < objectList()->count(); i++ ) {
	    kpobject = objectList()->at( i );
	    if ( kpobject->isSelected() && kpobject->getType() == OT_TEXT ) {
		dynamic_cast<KPTextObject*>( kpobject )->getKTextObject()->document()->setFamilyToAll( f );
		dynamic_cast<KPTextObject*>( kpobject )->getKTextObject()->updateCurrentFormat();
	    }
	}
	repaint( false );
    }
}

void Page::setTextPointSize( int s )
{
    if ( editNum != -1 && objectList()->at( editNum )->getType() == OT_TEXT ) {
	dynamic_cast<KPTextObject*>( objectList()->at( editNum ) )->getKTextObject()->setFocus();
	dynamic_cast<KPTextObject*>( objectList()->at( editNum ) )->getKTextObject()->setPointSize( s );
    } else {
	KPObject *kpobject = 0;

	for ( unsigned int i = 0; i < objectList()->count(); i++ ) {
	    kpobject = objectList()->at( i );
	    if ( kpobject->isSelected() && kpobject->getType() == OT_TEXT ) {
		dynamic_cast<KPTextObject*>( kpobject )->getKTextObject()->document()->setPointSizeToAll( s );
		dynamic_cast<KPTextObject*>( kpobject )->getKTextObject()->updateCurrentFormat();
	    }
	}
	repaint( false );
    }
}

/*===================== set text alignment =======================*/
void Page::setTextAlign( int align )
{
    if ( editNum != -1 && objectList()->at( editNum )->getType() == OT_TEXT ) {
        ( (KPTextObject*)objectList()->at( editNum ) )->getKTextObject()->setAlignment( align );
        repaint( FALSE );
    } else {
        KPObject *kpobject = 0;

        for ( unsigned int i = 0; i < objectList()->count(); i++ ) {
            kpobject = objectList()->at( i );
            if ( kpobject->isSelected() && kpobject->getType() == OT_TEXT )
                dynamic_cast<KPTextObject*>( kpobject )->getKTextObject()->document()->setAlignmentToAll( align );
        }
        repaint( FALSE );
    }
}

/*================================================================*/
bool Page::isASelectedTextObj()
{
    if ( editNum != -1 && objectList()->at( editNum )->getType() == OT_TEXT )
    {
        return true;
    }
    KPObject *kpobject = 0;
    for ( unsigned int i = 0; i < objectList()->count(); i++ )
    {
        kpobject = objectList()->at( i );
        if ( kpobject->isSelected() && kpobject->getType() == OT_TEXT )
            return true;
    }

    return false;
}

/*================================================================*/
KTextEdit *Page::haveASelectedTextObj()
{
    KPObject *kpobject = 0;

    for ( unsigned int i = 0; i < objectList()->count(); i++ )
    {
        kpobject = objectList()->at( i );
        if ( kpobject->isSelected() && kpobject->getType() == OT_TEXT )
            return dynamic_cast<KPTextObject*>( kpobject )->getKTextObject();
    }

    return 0L;
}

/*================================================================*/
KPTextObject *Page::haveASelectedKPTextObj()
{
    KPObject *kpobject = 0;

    for ( unsigned int i = 0; i < objectList()->count(); i++ )
    {
        kpobject = objectList()->at( i );
        if ( kpobject->isSelected() && kpobject->getType() == OT_TEXT )
            return dynamic_cast<KPTextObject*>( kpobject );
    }

    return 0L;
}

/*====================== start screenpresentation ================*/
void Page::startScreenPresentation( float presFakt, int curPgNum /* 1-based */)
{
    _presFakt = presFakt;
    m_showOnlyPage = curPgNum;
    //kdDebug() << "Page::startScreenPresentation m_showOnlyPage=" << m_showOnlyPage << endl;
    //kdDebug() << "                              _presFakt=" << _presFakt << endl;

    presMenu->setItemChecked( PM_SM, true );
    presMenu->setItemChecked( PM_DM, false );

    setCursor( waitCursor );
    tmpObjs.clear();

    exitEditMode();

    //kdDebug() << "Page::startScreenPresentation Zooming backgrounds" << endl;
    // Zoom backgrounds to the correct size for full screen
    if ( m_showOnlyPage == -1 )
    {
        // Maybe we should do this on demand ?
        QListIterator<KPBackGround> it(*backgroundList());
        for ( int i = 0 ; it.current(); ++it, ++i )
	    it.current()->setBgSize( getPageRect( i, _presFakt ).size() );
    }
    else
        backgroundList()->at( m_showOnlyPage-1 )->setBgSize( getPageRect( m_showOnlyPage-1, _presFakt ).size() );

    //kdDebug() << "Page::startScreenPresentation Zooming objects" << endl;
    // Zoom objects to the correct size for full screen
    // (might need a progress bar!)
    QListIterator<KPObject> it(*objectList());
    for ( int i = 0 ; it.current(); ++it, ++i )
    {
        int objPage = getPageOfObj( i, _presFakt ); // 1-based
	// We need to zoom ALL objects, even to show only one page.
	// Otherwise, the non-zoomed ones would interfer, being at the wrong page
        //if ( m_showOnlyPage == -1 || m_showOnlyPage == objPage )
        //{
            //kdDebug() << "Zooming object " << i << endl;
            it.current()->zoom( _presFakt );
            //kdDebug() << "Zooming object " << i << " - done" << endl;
            it.current()->drawSelection( false );

            // Objects to draw initially are those on first page (or m_showOnlyPage page, if set)
            if ( (m_showOnlyPage == -1 && objPage == 1) || m_showOnlyPage == objPage )
            {
                //kdDebug() << "Adding object " << i << " in page " << objPage << endl;
                tmpObjs.append( it.current() );
            }
        //}
    }

    KPresenterDoc * doc = view->kPresenterDoc();
    if ( doc->hasHeader() && doc->header() )
	doc->header()->zoom( _presFakt );
    if ( doc->hasFooter() && doc->footer() )
	doc->footer()->zoom( _presFakt );

    if ( m_showOnlyPage != -1 ) // only one page
    {
        slideList.clear();
        slideList << m_showOnlyPage;
    }
    else // all selected pages
    {
        slideList = doc->selectedSlides();
        // ARGLLLRGLRLGRLG selectedSlides gets us 0-based numbers,
        // and here we want 1-based numbers !
        QString debugstr;
        for ( QValueList<int>::Iterator it = slideList.begin() ; it != slideList.end(); ++ it )
        {
            *it = (*it)+1;
            debugstr += QString::number(*it) + ',';
        }
        //kdDebug() << "selectedSlides : " << debugstr << endl;
    }
    ASSERT( slideList.count() );
    slideListIterator = slideList.begin();

    setCursor( blankCursor );

    currPresPage = (unsigned int) -1; // force gotoPage to do something
    gotoPage( *slideListIterator );

    doc->getHeaderFooterEdit()->updateSizes();
    //kdDebug() << "Page::startScreenPresentation - done" << endl;
}

/*====================== stop screenpresentation =================*/
void Page::stopScreenPresentation()
{
    //kdDebug() << "Page::stopScreenPresentation m_showOnlyPage=" << m_showOnlyPage << endl;
    setCursor( waitCursor );

    QListIterator<KPObject> it(*objectList());
    for ( int i = 0 ; it.current(); ++it, ++i )
    {
        if ( it.current()->isZoomed() )
        {
            //kdDebug() << "Page::stopScreenPresentation zooming back object " << i << endl;
            it.current()->zoomOrig();
            it.current()->drawSelection( true );
        }
    }

    _presFakt = 1.0;

    // Zoom backgrounds back
    if ( m_showOnlyPage == -1 )
    {
        QListIterator<KPBackGround> it(*backgroundList());
        for ( int i = 0 ; it.current(); ++it, ++i )
	    it.current()->setBgSize( getPageRect( i ).size() );
    }
    else
        backgroundList()->at( m_showOnlyPage-1 )->setBgSize( getPageRect( m_showOnlyPage-1 ).size() );

    KPresenterDoc * doc = view->kPresenterDoc();
    if ( doc->hasHeader() && doc->header() )
        doc->header()->zoomOrig();
    if ( doc->hasFooter() && doc->footer() )
        doc->footer()->zoomOrig();

    goingBack = false;
    currPresPage = 1;
    editMode = true;
    repaint( false );
    setToolEditMode( toolEditMode );
    tmpObjs.clear();
    setWFlags( WResizeNoErase );
}

/*========================== next ================================*/
bool Page::pNext( bool )
{
    bool addSubPres = false;
    bool clearSubPres = false;

    goingBack = false;

    kdDebug() << "\n-------\nPage::pNext currPresStep=" << currPresStep << " subPresStep=" << subPresStep << endl;

    // First try to go one sub-step further, if any object requires it
    QListIterator<KPObject> oit(*objectList());
    for ( int i = 0 ; oit.current(); ++oit, ++i )
    {
        KPObject *kpobject = oit.current();
        if ( getPageOfObj( i, _presFakt ) == static_cast<int>( currPresPage )
             && kpobject->getPresNum() == static_cast<int>( currPresStep )
             && kpobject->getType() == OT_TEXT && kpobject->getEffect2() != EF2_NONE )
        {
            if ( static_cast<int>( subPresStep ) < kpobject->getSubPresSteps() )
            {
                addSubPres = true;
                kdDebug() << "Page::pNext adding subpres" << endl;
            }
            else
                clearSubPres = true;
        }

        if ( addSubPres )
        {
            kdDebug() << "Page::pNext addSubPres subPresStep is now " << subPresStep+1 << endl;
            subPresStep++;
            doObjEffects();
            return false;
        }

        if ( clearSubPres )           // not sure what that's for (the bool)
            subPresStep = 0;
    }

    // Then try to see if there is still one step to do in the current page
    if ( (int)currPresStep < *( --presStepList.end() ) )
    {
        QValueList<int>::Iterator it = presStepList.find( currPresStep );
        currPresStep = *( ++it );
        //kdDebug() << "Page::pNext setting currPresStep to " << currPresStep << endl;

        if ( currPresStep == 0 )
        {
            QPainter p;
            p.begin( this );
            drawBackground( &p, QRect( 0, 0, kapp->desktop()->width(), kapp->desktop()->height() ) );
            p.end();
        }

        doObjEffects();
        return false;
    }

    // No more steps in this page, try to go to the next page
    QValueList<int>::Iterator test(  slideListIterator );
    if ( ++test != slideList.end() )
    {
        QPixmap _pix1( QApplication::desktop()->width(), QApplication::desktop()->height() );
        drawPageInPix( _pix1, view->getDiffY() );

        currPresPage = *( ++slideListIterator );
        kdDebug() << "Page::pNext going to page " << currPresPage << endl;

        tmpObjs.clear();
        for ( int j = 0; j < static_cast<int>( objectList()->count() ); j++ )
        {
            if ( getPageOfObj( j, _presFakt ) == static_cast<int>( currPresPage ) )
                tmpObjs.append( objectList()->at( j ) );
        }

        presStepList = view->kPresenterDoc()->reorderPage( currPresPage, diffx(), diffy(), _presFakt );
        currPresStep = *presStepList.begin();

        QPixmap _pix2( QApplication::desktop()->width(), QApplication::desktop()->height() );
        int pageHeight = view->kPresenterDoc()->getPageRect( 0, 0, 0, presFakt(), false ).height();
        int yOffset = ( presPage() - 1 ) * pageHeight;
        if ( height() > pageHeight )
            yOffset -= ( height() - pageHeight ) / 2;
        drawPageInPix( _pix2, yOffset );

        QValueList<int>::Iterator it( slideListIterator );
        --it;
        changePages( _pix1, _pix2, backgroundList()->at( ( *it ) - 1 )->getPageEffect() );

        return true;
    }

    // No more slides. Redisplay last one, then
    kdDebug() << "Page::pNext last slide -> again" << endl;
    emit stopPres();
    presStepList = view->kPresenterDoc()->reorderPage( currPresPage, diffx(), diffy(), _presFakt );
    currPresStep = *presStepList.begin();
    doObjEffects();
    return false;
}

/*====================== previous ================================*/
bool Page::pPrev( bool /*manual*/ )
{
    goingBack = true;
    subPresStep = 0;

    if ( (int)currPresStep > *presStepList.begin() ) {
        QValueList<int>::Iterator it = presStepList.find( currPresStep );
        currPresStep = *( --it );
        repaint( false );
        return false;
    } else {
        if ( slideListIterator == slideList.begin() ) {
            presStepList = view->kPresenterDoc()->reorderPage( currPresPage, diffx(), diffy(), _presFakt );
            currPresStep = *presStepList.begin();
            repaint( false );
            return false;
        }
        currPresPage = *( --slideListIterator );

        tmpObjs.clear();
        for ( int j = 0; j < static_cast<int>( objectList()->count() ); j++ ) {
            if ( getPageOfObj( j, _presFakt ) == static_cast<int>( currPresPage ) )
                tmpObjs.append( objectList()->at( j ) );
        }

        presStepList = view->kPresenterDoc()->reorderPage( currPresPage, diffx(), diffy(), _presFakt );
        currPresStep = *( --presStepList.end() );
        return true;
    }

    return false;
}

/*================================================================*/
bool Page::canAssignEffect( QList<KPObject> &objs ) const
{
    QListIterator<KPObject> oIt( *objectList() );
    for (; oIt.current(); ++oIt )
        if ( oIt.current()->isSelected() )
            objs.append( oIt.current() );

    return !objs.isEmpty();
}

/*================================================================*/
bool Page::isOneObjectSelected()
{
    QListIterator<KPObject> oIt( *objectList() );
    for (; oIt.current(); ++oIt )
        if ( oIt.current()->isSelected() )
            return true;

    return false;
}

/*================================================================*/
void Page::drawPageInPix2( QPixmap &_pix, int __diffy, int pgnum, float /*_zoom*/ )
{
    //kdDebug() << "Page::drawPageInPix2" << endl;
    ignoreSkip = TRUE;
    currPresPage = pgnum + 1;
    int _yOffset = view->getDiffY();
    view->setDiffY( __diffy );

    QPainter p;
    p.begin( &_pix );

    QListIterator<KPObject> oIt( *objectList() );
    for (; oIt.current(); ++oIt )
        oIt.current()->drawSelection( false );

    bool _editMode = editMode;
    editMode = false;
    drawBackground( &p, _pix.rect() );
    editMode = _editMode;

    drawObjects( &p, _pix.rect() );

    p.end();

    view->setDiffY( _yOffset );

    oIt.toFirst();
    for (; oIt.current(); ++oIt )
        oIt.current()->drawSelection( true );

    ignoreSkip = FALSE;
}

/*==================== draw a page in a pixmap ===================*/
void Page::drawPageInPix( QPixmap &_pix, int __diffy )
{
    //kdDebug() << "Page::drawPageInPix" << endl;
    ignoreSkip = TRUE;
    int _yOffset = view->getDiffY();
    view->setDiffY( __diffy );

    QPainter p;
    p.begin( &_pix );

    drawBackground( &p, _pix.rect() );
    drawObjects( &p, _pix.rect() );

    p.end();

    view->setDiffY( _yOffset );
    ignoreSkip = FALSE;
}

/*==================== draw a page in a painter ===================*/
void Page::drawPageInPainter( QPainter* painter, int __diffy, QRect _rect )
{
    //kdDebug() << "Page::drawPageInPainter" << endl;
    ignoreSkip = TRUE;
    int _yOffset = view->getDiffY();
    view->setDiffY( __diffy );

    drawBackground( painter, _rect );
    drawObjects( painter, _rect );

    view->setDiffY( _yOffset );
    ignoreSkip = FALSE;
}

/*=========================== change pages =======================*/
void Page::changePages( QPixmap _pix1, QPixmap _pix2, PageEffect _effect )
{
    QTime _time;
    int _step = 0, _steps, _h, _w, _x, _y;

    switch ( _effect )
    {
    case PEF_NONE:
    {
        bitBlt( this, 0, 0, &_pix2, 0, 0, _pix2.width(), _pix2.height() );
    } break;
    case PEF_CLOSE_HORZ:
    {
        _steps = static_cast<int>( static_cast<float>( kapp->desktop()->height() ) / pageSpeedFakt() );
        _time.start();

        for ( ; ; )
        {
            kapp->processEvents();
            if ( _time.elapsed() >= 1 )
            {
                _step++;
                _h = ( _pix2.height()/( 2 * _steps ) ) * _step;
                _h = _h > _pix2.height() / 2 ? _pix2.height() / 2 : _h;

                bitBlt( this, 0, 0, &_pix2, 0, _pix2.height() / 2 - _h, width(), _h );
                bitBlt( this, 0, height() - _h, &_pix2, 0, _pix2.height() / 2, width(), _h );

                _time.restart();
            }
            if ( ( _pix2.height()/( 2 * _steps ) ) * _step >= _pix2.height() / 2 ) break;
        }
    } break;
    case PEF_CLOSE_VERT:
    {
        _steps = static_cast<int>( static_cast<float>( kapp->desktop()->width() ) / pageSpeedFakt() );
        _time.start();

        for ( ; ; )
        {
            kapp->processEvents();
            if ( _time.elapsed() >= 1 )
            {
                _step++;
                _w = ( _pix2.width()/( 2 * _steps ) ) * _step;
                _w = _w > _pix2.width() / 2 ? _pix2.width() / 2 : _w;

                bitBlt( this, 0, 0, &_pix2, _pix2.width() / 2 - _w, 0, _w, height() );
                bitBlt( this, width() - _w, 0, &_pix2, _pix2.width() / 2, 0, _w, height() );

                _time.restart();
            }
            if ( ( _pix2.width()/( 2 * _steps ) ) * _step >= _pix2.width() / 2 ) break;
        }
    } break;
    case PEF_CLOSE_ALL:
    {
        _steps = static_cast<int>( static_cast<float>( kapp->desktop()->width() ) / pageSpeedFakt() );
        _time.start();

        for ( ; ; )
        {
            kapp->processEvents();
            if ( _time.elapsed() >= 1 )
            {
                _step++;
                _w = ( _pix2.width()/( 2 * _steps ) ) * _step;
                _w = _w > _pix2.width() / 2 ? _pix2.width() / 2 : _w;

                _h = ( _pix2.height()/( 2 * _steps ) ) * _step;
                _h = _h > _pix2.height() / 2 ? _pix2.height() / 2 : _h;

                bitBlt( this, 0, 0, &_pix2, 0, 0, _w, _h );
                bitBlt( this, width() - _w, 0, &_pix2, width() - _w, 0, _w, _h );
                bitBlt( this, 0, height() - _h, &_pix2, 0, height() - _h, _w, _h );
                bitBlt( this, width() - _w, height() - _h, &_pix2, width() - _w, height() - _h, _w, _h );

                _time.restart();
            }
            if ( ( _pix2.width()/( 2 * _steps ) ) * _step >= _pix2.width() / 2
                 && ( _pix2.height()/( 2 * _steps ) ) * _step >= _pix2.height() / 2 ) break;
        }
    } break;
    case PEF_OPEN_HORZ:
    {
        _steps = static_cast<int>( static_cast<float>( kapp->desktop()->height() ) / pageSpeedFakt() );
        _time.start();

        for ( ; ; )
        {
            kapp->processEvents();
            if ( _time.elapsed() >= 1 )
            {
                _step++;
                _h = ( _pix2.height() / _steps ) * _step;
                _h = _h > _pix2.height() ? _pix2.height() : _h;

                _y = _pix2.height() / 2;

                bitBlt( this, 0, _y - _h / 2, &_pix2, 0, _y - _h / 2, width(), _h );

                _time.restart();
            }
            if ( ( _pix2.height() / _steps ) * _step >= _pix2.height() ) break;
        }
    } break;
    case PEF_OPEN_VERT:
    {
        _steps = static_cast<int>( static_cast<float>( kapp->desktop()->width() ) / pageSpeedFakt() );
        _time.start();

        for ( ; ; )
        {
            kapp->processEvents();
            if ( _time.elapsed() >= 1 )
            {
                _step++;
                _w = ( _pix2.width() / _steps ) * _step;
                _w = _w > _pix2.width() ? _pix2.width() : _w;

                _x = _pix2.width() / 2;

                bitBlt( this, _x - _w / 2, 0, &_pix2, _x - _w / 2, 0, _w, height() );

                _time.restart();
            }
            if ( ( _pix2.width() / _steps ) * _step >= _pix2.width() ) break;
        }
    } break;
    case PEF_OPEN_ALL:
    {
        _steps = static_cast<int>( static_cast<float>( kapp->desktop()->width() ) / pageSpeedFakt() );
        _time.start();

        for ( ; ; )
        {
            kapp->processEvents();
            if ( _time.elapsed() >= 1 )
            {
                _step++;
                _w = ( _pix2.width() / _steps ) * _step;
                _w = _w > _pix2.width() ? _pix2.width() : _w;

                _x = _pix2.width() / 2;

                _h = ( _pix2.height() / _steps ) * _step;
                _h = _h > _pix2.height() ? _pix2.height() : _h;

                _y = _pix2.height() / 2;

                bitBlt( this, _x - _w / 2, _y - _h / 2, &_pix2, _x - _w / 2, _y - _h / 2, _w, _h );

                _time.restart();
            }
            if ( ( _pix2.width() / _steps ) * _step >= _pix2.width() &&
                 ( _pix2.height() / _steps ) * _step >= _pix2.height() ) break;
        }
    } break;
    case PEF_INTERLOCKING_HORZ_1:
    {
        _steps = static_cast<int>( static_cast<float>( kapp->desktop()->width() ) / pageSpeedFakt() );
        _time.start();

        for ( ; ; )
        {
            kapp->processEvents();
            if ( _time.elapsed() >= 1 )
            {
                _step++;
                _w = ( _pix2.width() / _steps ) * _step;
                _w = _w > _pix2.width() ? _pix2.width() : _w;

                bitBlt( this, 0, 0, &_pix2, 0, 0, _w, height() / 4 );
                bitBlt( this, 0, height() / 2, &_pix2, 0, height() / 2, _w, height() / 4 );
                bitBlt( this, width() - _w, height() / 4, &_pix2, width() - _w, height() / 4, _w, height() / 4 );
                bitBlt( this, width() - _w, height() / 2 + height() / 4, &_pix2, width() - _w,
                        height() / 2 + height() / 4, _w, height() / 4 );

                _time.restart();
            }
            if ( ( _pix2.width() / _steps ) * _step >= _pix2.width() ) break;
        }
    } break;
    case PEF_INTERLOCKING_HORZ_2:
    {
        _steps = static_cast<int>( static_cast<float>( kapp->desktop()->width() ) / pageSpeedFakt() );
        _time.start();

        for ( ; ; )
        {
            kapp->processEvents();
            if ( _time.elapsed() >= 1 )
            {
                _step++;
                _w = ( _pix2.width() / _steps ) * _step;
                _w = _w > _pix2.width() ? _pix2.width() : _w;

                bitBlt( this, 0, height() / 4, &_pix2, 0, height() / 4, _w, height() / 4 );
                bitBlt( this, 0, height() / 2 + height() / 4, &_pix2, 0, height() / 2 + height() / 4, _w, height() / 4 );
                bitBlt( this, width() - _w, 0, &_pix2, width() - _w, 0, _w, height() / 4 );
                bitBlt( this, width() - _w, height() / 2, &_pix2, width() - _w, height() / 2, _w, height() / 4 );

                _time.restart();
            }
            if ( ( _pix2.width() / _steps ) * _step >= _pix2.width() ) break;
        }
    } break;
    case PEF_INTERLOCKING_VERT_1:
    {
        _steps = static_cast<int>( static_cast<float>( kapp->desktop()->height() ) / pageSpeedFakt() );
        _time.start();

        for ( ; ; )
        {
            kapp->processEvents();
            if ( _time.elapsed() >= 1 )
            {
                _step++;
                _h = ( _pix2.height() / _steps ) * _step;
                _h = _h > _pix2.height() ? _pix2.height() : _h;

                bitBlt( this, 0, 0, &_pix2, 0, 0, width() / 4, _h );
                bitBlt( this, width() / 2, 0, &_pix2, width() / 2, 0, width() / 4, _h );
                bitBlt( this, width() / 4, height() - _h, &_pix2, width() / 4, height() - _h, width() / 4, _h );
                bitBlt( this, width() / 2 + width() / 4, height() - _h, &_pix2, width() / 2 + width() / 4, height() - _h,
                        width() / 4, _h );

                _time.restart();
            }
            if ( ( _pix2.height() / _steps ) * _step >= _pix2.height() ) break;
        }
    } break;
    case PEF_INTERLOCKING_VERT_2:
    {
        _steps = static_cast<int>( static_cast<float>( kapp->desktop()->height() ) / pageSpeedFakt() );
        _time.start();

        for ( ; ; )
        {
            kapp->processEvents();
            if ( _time.elapsed() >= 1 )
            {
                _step++;
                _h = ( _pix2.height() / _steps ) * _step;
                _h = _h > _pix2.height() ? _pix2.height() : _h;

                bitBlt( this, width() / 4, 0, &_pix2, width() / 4, 0, width() / 4, _h );
                bitBlt( this, width() / 2 + width() / 4, 0, &_pix2, width() / 2 + width() / 4, 0, width() / 4, _h );
                bitBlt( this, 0, height() - _h, &_pix2, 0, height() - _h, width() / 4, _h );
                bitBlt( this, width() / 2, height() - _h, &_pix2, width() / 2, height() - _h, width() / 4, _h );

                _time.restart();
            }
            if ( ( _pix2.height() / _steps ) * _step >= _pix2.height() ) break;
        }
    } break;
    case PEF_SURROUND1:
    {
        int wid = kapp->desktop()->width() / 10;
        int hei = kapp->desktop()->height() / 10;

        int curr = 1;
        int curr2 = 1;

        _steps = static_cast<int>( static_cast<float>( kapp->desktop()->width() ) / pageSpeedFakt() );
        _time.start();

        for ( ; ; )
        {
            kapp->processEvents();
            if ( _time.elapsed() >= 1 )
            {
                _step++;

                if ( curr == 1 || curr == 5 || curr == 9 || curr == 13 || curr == 17 )
                {
                    int dx = ( curr2 / 4 ) * wid;
                    int dy = ( curr2 / 4 ) * hei;
                    _h = ( _pix2.height() / _steps ) * _step;
                    if ( _h >= _pix2.height() - 2 * dy )
                    {
                        _h = _pix2.height() - 2 * dy;
                        curr++;
                        _step = 0;
                    }
                    bitBlt( this, dx, dy, &_pix2, dx, dy, wid, _h );
                }
                else if ( curr == 2 || curr == 6 || curr == 10 || curr == 14 || curr == 18 )
                {
                    int dx = ( curr2 / 4 ) * wid;
                    int dy = ( curr2 / 4 ) * hei;
                    _w = ( _pix2.width() / _steps ) * _step;
                    if ( _w >= _pix2.width() - wid - 2 * dx )
                    {
                        _w = _pix2.width() - wid - 2 * dx;
                        curr++;
                        _step = 0;
                    }
                    bitBlt( this, dx + wid, height() - hei - dy, &_pix2, dx + wid, height() - hei - dy, _w, hei );
                }
                else if ( curr == 3 || curr == 7 || curr == 11 || curr == 15 || curr == 19 )
                {
                    int dx = ( curr2 / 4 ) * wid;
                    int dy = ( curr2 / 4 ) * hei;
                    _h = ( _pix2.height() / _steps ) * _step;
                    if ( _h >= _pix2.height() - hei - 2 * dy )
                    {
                        _h = _pix2.height() - hei - 2 * dy;
                        curr++;
                        _step = 0;
                    }
                    bitBlt( this, _pix2.width() - wid - dx, _pix2.height() - hei - dy - _h, &_pix2,
                            _pix2.width() - wid - dx, _pix2.height() - hei - dy - _h, wid, _h );
                }
                else if ( curr == 4 || curr == 8 || curr == 12 || curr == 16 || curr == 20 )
                {
                    int dx = ( curr2 / 4 ) * wid;
                    int dy = ( curr2 / 4 ) * hei;
                    _w = ( _pix2.width() / _steps ) * _step;
                    if ( _w >= _pix2.width() - 2 * wid - 2 * dx )
                    {
                        _w = _pix2.width() - 2 * wid - 2 * dx;
                        _steps *= 2;
                        _steps = static_cast<int>( static_cast<float>( _steps ) / 1.5 );
                        curr++;
                        curr2 += 4;
                        _step = 0;
                    }
                    bitBlt( this, _pix2.width() - dx - wid - _w, dy, &_pix2, _pix2.width() - dx - wid - _w,
                            dy, _w, hei );
                }
                _time.restart();
            }
            if ( curr == 21 )
            {
                bitBlt( this, 0, 0, &_pix2, 0, 0, _pix2.width(), _pix2.height() );
                break;
            }
        }
    } break;
    case PEF_FLY1:
    {
        _steps = static_cast<int>( static_cast<float>( kapp->desktop()->height() ) / pageSpeedFakt() );
        _time.start();

        int _psteps = _steps / 5;
        QRect oldRect( 0, 0, kapp->desktop()->width(), kapp->desktop()->height() );
        QSize ps;
        QPixmap pix3;

        for ( ; ; )
        {
            kapp->processEvents();
            if ( _time.elapsed() >= 1 )
            {
                _step++;
                if ( _step < _psteps )
                {
                    pix3 = QPixmap( _pix1 );
                    QPixmap pix4( _pix2 );
                    float dw = static_cast<float>( _step * ( ( pix3.width() - ( pix3.width() / 10 ) ) /
                                                             ( 2 * _psteps ) ) );
                    float dh = static_cast<float>( _step * ( ( pix3.height() - ( pix3.height() / 10 ) ) /
                                                             ( 2 * _psteps ) ) );

                    dw *= 2;
                    dh *= 2;

                    QWMatrix m;
                    m.scale( static_cast<float>( pix3.width() - dw ) / static_cast<float>( pix3.width() ),
                             static_cast<float>( pix3.height() - dh ) / static_cast<float>( pix3.height() ) );
                    pix3 = pix3.xForm( m );
                    ps = pix3.size();

                    bitBlt( &pix4, ( pix4.width() - pix3.width() ) / 2, ( pix4.height() - pix3.height() ) / 2,
                            &pix3, 0, 0, pix3.width(), pix3.height() );
                    QRect newRect( ( pix4.width() - pix3.width() ) / 2, ( pix4.height() - pix3.height() ) / 2,
                                   pix3.width(), pix3.height() );
                    QRect r = newRect.unite( oldRect );
                    bitBlt( this, r.x(), r.y(), &pix4, r.x(), r.y(), r.width(), r.height() );
                    oldRect = newRect;
                }
                if ( _step > _psteps && _step < _psteps * 2 )
                {
                    QPixmap pix4( _pix2 );
                    int yy = ( _pix1.height() - pix3.height() ) / 2 - ( ( ( _pix1.height() - pix3.height() ) / 2 ) /
                                                                        _psteps ) * ( _step - _psteps );

                    bitBlt( &pix4, ( pix4.width() - pix3.width() ) / 2, yy,
                            &pix3, 0, 0, pix3.width(), pix3.height() );
                    QRect newRect( ( pix4.width() - pix3.width() ) / 2, yy,
                                   pix3.width(), pix3.height() );
                    QRect r = newRect.unite( oldRect );
                    bitBlt( this, r.x(), r.y(), &pix4, r.x(), r.y(), r.width(), r.height() );
                    oldRect = newRect;
                }
                if ( _step > 2 * _psteps && _step < _psteps * 3 )
                {
                    QPixmap pix4( _pix2 );
                    int xx = ( _pix1.width() - pix3.width() ) / 2 - ( ( ( _pix1.width() - pix3.width() ) / 2 ) /
                                                                      _psteps ) * ( _step - 2 * _psteps );
                    int yy = ( ( ( _pix1.height() - pix3.height() ) / 2 ) / _psteps ) * ( _step - 2 * _psteps );

                    bitBlt( &pix4, xx, yy, &pix3, 0, 0, pix3.width(), pix3.height() );
                    QRect newRect( xx, yy, pix3.width(), pix3.height() );
                    QRect r = newRect.unite( oldRect );
                    bitBlt( this, r.x(), r.y(), &pix4, r.x(), r.y(), r.width(), r.height() );
                    oldRect = newRect;
                }
                if ( _step > 3 * _psteps && _step < _psteps * 5 )
                {
                    QPixmap pix4( _pix2 );
                    int xx = ( ( _pix1.width() - pix3.width() ) / _psteps ) * ( _step - 3 * _psteps );
                    int yy = ( ( _pix1.height() - pix3.height() ) / 2 ) +
                             ( ( ( _pix1.height() - pix3.height() ) / 2 ) / _psteps ) * ( _step - 3 * _psteps );

                    bitBlt( &pix4, xx, yy, &pix3, 0, 0, pix3.width(), pix3.height() );
                    QRect newRect( xx, yy, pix3.width(), pix3.height() );
                    QRect r = newRect.unite( oldRect );
                    bitBlt( this, r.x(), r.y(), &pix4, r.x(), r.y(), r.width(), r.height() );
                    oldRect = newRect;
                }
                _time.restart();
            }
            if ( _step >= _steps )
            {
                bitBlt( this, 0, 0, &_pix2, 0, 0, _pix2.width(), _pix2.height() );
                break;
            }
        }
    } break;
    }
}

/*================================================================*/
void Page::doObjEffects()
{
    QPixmap screen_orig( kapp->desktop()->width(), kapp->desktop()->height() );
    bool drawn = false;

    // YABADABADOOOOOOO.... That's a hack :-)
    if ( subPresStep == 0 && currPresPage > 0 )
    {
        //kdDebug() << "Page::doObjEffects - in the strange hack" << endl;
        inEffect = true;
        QPainter p;
        p.begin( &screen_orig );
        drawBackground( &p, QRect( 0, 0, kapp->desktop()->width(), kapp->desktop()->height() ) );
        drawObjects( &p, QRect( 0, 0, kapp->desktop()->width(), kapp->desktop()->height() ) );
        p.end();
        inEffect = false;
        bitBlt( this, 0, 0, &screen_orig, 0, 0, screen_orig.width(), screen_orig.height() );
        drawn = true;
    }

    QList<KPObject> _objList;
    QTime _time;
    int _step = 0, _steps1 = 0, _steps2 = 0, x_pos1 = 0, y_pos1 = 0;
    int x_pos2 = kapp->desktop()->width(), y_pos2 = kapp->desktop()->height(), _step_width = 0, _step_height = 0;
    int w_pos1 = 0, h_pos1;
    bool effects = false;
    bool nothingHappens = false;
    if ( !drawn )
        bitBlt( &screen_orig, 0, 0, this, 0, 0, kapp->desktop()->width(), kapp->desktop()->height() );
    QPixmap *screen = new QPixmap( screen_orig );

    QListIterator<KPObject> oit(*objectList());
    for ( int i = 0 ; oit.current(); ++oit, ++i )
    {
        KPObject *kpobject = oit.current();
        if ( getPageOfObj( i, _presFakt ) == static_cast<int>( currPresPage )
             && kpobject->getPresNum() == static_cast<int>( currPresStep ) )
        {
            if ( kpobject->getEffect() != EF_NONE )
            {
                _objList.append( kpobject );

                int x = 0, y = 0, w = 0, h = 0;
                QRect br = kpobject->getBoundingRect( 0, 0 );
                x = br.x(); y = br.y(); w = br.width(); h = br.height();

                switch ( kpobject->getEffect() )
                {
                case EF_COME_LEFT:
                    x_pos1 = QMAX( x_pos1, x - diffx() + w );
                    break;
                case EF_COME_TOP:
                    y_pos1 = QMAX( y_pos1, y - diffy() + h );
                    break;
                case EF_COME_RIGHT:
                    x_pos2 = QMIN( x_pos2, x - diffx() );
                    break;
                case EF_COME_BOTTOM:
                    y_pos2 = QMIN( y_pos2, y - diffy() );
                    break;
                case EF_COME_LEFT_TOP:
                {
                    x_pos1 = QMAX( x_pos1, x - diffx() + w );
                    y_pos1 = QMAX( y_pos1, y - diffy() + h );
                } break;
                case EF_COME_LEFT_BOTTOM:
                {
                    x_pos1 = QMAX( x_pos1, x - diffx() + w );
                    y_pos2 = QMIN( y_pos2, y - diffy() );
                } break;
                case EF_COME_RIGHT_TOP:
                {
                    x_pos2 = QMIN( x_pos2, x - diffx() );
                    y_pos1 = QMAX( y_pos1, y - diffy() + h );
                } break;
                case EF_COME_RIGHT_BOTTOM:
                {
                    x_pos2 = QMIN( x_pos2, x - diffx() );
                    y_pos2 = QMIN( y_pos2, y - diffy() );
                } break;
                case EF_WIPE_LEFT:
                    x_pos1 = QMAX( x_pos1, w );
                    break;
                case EF_WIPE_RIGHT:
                    x_pos1 = QMAX( x_pos1, w );
                    break;
                case EF_WIPE_TOP:
                    y_pos1 = QMAX( y_pos1, h );
                    break;
                case EF_WIPE_BOTTOM:
                    y_pos1 = QMAX( y_pos1, h );
                    break;
                default: break;
                }
                effects = true;
            }
        }
        else if ( getPageOfObj( i, _presFakt ) == static_cast<int>( currPresPage )
                  && kpobject->getDisappear() && kpobject->getDisappearNum() == static_cast<int>( currPresStep ) )
        {
            if ( kpobject->getEffect3() != EF3_NONE )
            {
                _objList.append( kpobject );

                int x = 0, y = 0, w = 0, h = 0;
                QRect br = kpobject->getBoundingRect( 0, 0 );
                x = br.x(); y = br.y(); w = br.width(); h = br.height();

                switch ( kpobject->getEffect() )
                {
                case EF3_GO_LEFT:
                    x_pos1 = QMAX( x_pos1, x - diffx() + w );
                    break;
                case EF3_GO_TOP:
                    y_pos1 = QMAX( y_pos1, y - diffy() + h );
                    break;
                case EF3_GO_RIGHT:
                    x_pos2 = QMIN( x_pos2, x - diffx() );
                    break;
                case EF3_GO_BOTTOM:
                    y_pos2 = QMIN( y_pos2, y - diffy() );
                    break;
                case EF3_GO_LEFT_TOP:
                {
                    x_pos1 = QMAX( x_pos1, x - diffx() + w );
                    y_pos1 = QMAX( y_pos1, y - diffy() + h );
                } break;
                case EF3_GO_LEFT_BOTTOM:
                {
                    x_pos1 = QMAX( x_pos1, x - diffx() + w );
                    y_pos2 = QMIN( y_pos2, y - diffy() );
                } break;
                case EF3_GO_RIGHT_TOP:
                {
                    x_pos2 = QMIN( x_pos2, x - diffx() );
                    y_pos1 = QMAX( y_pos1, y - diffy() + h );
                } break;
                case EF3_GO_RIGHT_BOTTOM:
                {
                    x_pos2 = QMIN( x_pos2, x - diffx() );
                    y_pos2 = QMIN( y_pos2, y - diffy() );
                } break;
                case EF3_WIPE_LEFT:
                    x_pos1 = QMAX( x_pos1, w );
                    break;
                case EF3_WIPE_RIGHT:
                    x_pos1 = QMAX( x_pos1, w );
                    break;
                case EF3_WIPE_TOP:
                    y_pos1 = QMAX( y_pos1, h );
                    break;
                case EF3_WIPE_BOTTOM:
                    y_pos1 = QMAX( y_pos1, h );
                    break;
                default: break;
                }
                effects = true;
            }
        }
    }

    if ( effects )
    {
        _step_width = static_cast<int>( ( static_cast<float>( kapp->desktop()->width() ) / objSpeedFakt() ) );
        _step_height = static_cast<int>( ( static_cast<float>( kapp->desktop()->height() ) / objSpeedFakt() ) );
        _steps1 = x_pos1 > y_pos1 ? x_pos1 / _step_width : y_pos1 / _step_height;
        _steps2 = kapp->desktop()->width() - x_pos2 > kapp->desktop()->height() - y_pos2 ?
                  ( kapp->desktop()->width() - x_pos2 ) / _step_width : ( kapp->desktop()->height() - y_pos2 ) / _step_height;
        _time.start();

        QList<QRect> xy;
        xy.setAutoDelete( true );

        for ( ; ; )
        {
            kapp->processEvents();
            if ( nothingHappens ) break; // || _step >= _steps1 && _step >= _steps2 ) break;

            QList<QRect> changes;
            changes.setAutoDelete( true );

            if ( _time.elapsed() >= 1 )
            {
                nothingHappens = true;
                _step++;

                changes.clear();

                for ( int i = 0; i < static_cast<int>( _objList.count() ); i++ )
                {
                    KPObject * kpobject = _objList.at( i );
                    int _w =  kapp->desktop()->width() - ( kpobject->getOrig().x() - diffx() );
                    int _h =  kapp->desktop()->height() - ( kpobject->getOrig().y() - diffy() );
                    int ox = 0, oy = 0, ow = 0, oh = 0;
                    ox = kpobject->getOrig().x();
                    oy = kpobject->getOrig().y();
                    ow = kpobject->getSize().width();
                    oh = kpobject->getSize().height();

                    QRect br = kpobject->getBoundingRect( 0, 0 );
                    int bx = br.x();
                    int by = br.y();
                    int bw = br.width();
                    int bh = br.height();

                    QRect oldRect;
                    QRect newRect;

                    if ( static_cast<int>( xy.count() - 1 ) < i )
                    {
                        xy.append( new QRect( 0, 0, 0, 0 ) );
                        oldRect.setRect( bx - diffx(), by - diffy(), bw, bh );
                    }
                    else
                        oldRect.setRect( bx - ( diffx() - xy.at( i )->x() ), by - ( diffy() - xy.at( i )->y() ), bw - xy.at( i )->width(), bh - xy.at( i )->height() );

                    if ( !kpobject->getDisappear() || kpobject->getDisappear() && kpobject->getDisappearNum() != static_cast<int>( currPresStep ) )
                    {
                        switch ( kpobject->getEffect() )
                        {
                        case EF_NONE:
                        {
                            if ( subPresStep == 0 || subPresStep != 0 && kpobject->getType() == OT_TEXT && kpobject->getEffect2() == EF2T_PARA )
                                drawObject( kpobject, screen, ox, oy, 0, 0, 0, 0 );
                        } break;
                        case EF_COME_LEFT:
                        {
                            if ( subPresStep == 0 || subPresStep != 0 && kpobject->getType() == OT_TEXT && kpobject->getEffect2() == EF2T_PARA )
                            {
                                x_pos1 = _step_width * _step < ox - diffx() + ow ?
                                         ox - diffx() + ow - _step_width * _step : 0;
                                y_pos1 = 0;
                                drawObject( kpobject, screen, -x_pos1, y_pos1, 0, 0, 0, 0 );
                                if ( x_pos1 != 0 ) nothingHappens = false;
                                xy.at( i )->setX( -x_pos1 );
                                xy.at( i )->setY( y_pos1 );
                                xy.at( i )->setWidth( 0 );
                                xy.at( i )->setHeight( 0 );
                            }
                        } break;
                        case EF_COME_TOP:
                        {
                            if ( subPresStep == 0 || subPresStep != 0 && kpobject->getType() == OT_TEXT && kpobject->getEffect2() == EF2T_PARA )
                            {
                                y_pos1 = _step_height * _step < oy - diffy() + oh ?
                                         oy - diffy() + oh - _step_height * _step : 0;
                                x_pos1 = 0;
                                drawObject( kpobject, screen, x_pos1, -y_pos1, 0, 0, 0, 0 );
                                if ( y_pos1 != 0 ) nothingHappens = false;
                                xy.at( i )->setX( x_pos1 );
                                xy.at( i )->setY( -y_pos1 );
                                xy.at( i )->setWidth( 0 );
                                xy.at( i )->setHeight( 0 );
                            }
                        } break;
                        case EF_COME_RIGHT:
                        {
                            if ( subPresStep == 0 || subPresStep != 0 && kpobject->getType() == OT_TEXT && kpobject->getEffect2() == EF2T_PARA )
                            {
                                x_pos2 = _w - ( _step_width * _step ) + ( ox - diffx() ) > ox - diffx() ?
                                         _w - ( _step_width * _step ) : 0;
                                y_pos2 = 0;
                                drawObject( kpobject, screen, x_pos2, y_pos2, 0, 0, 0, 0 );
                                if ( x_pos2 != 0 ) nothingHappens = false;
                                xy.at( i )->setX( x_pos2 );
                                xy.at( i )->setY( y_pos2 );
                                xy.at( i )->setWidth( 0 );
                                xy.at( i )->setHeight( 0 );
                            }
                        } break;
                        case EF_COME_BOTTOM:
                        {
                            if ( subPresStep == 0 || subPresStep != 0 && kpobject->getType() == OT_TEXT && kpobject->getEffect2() == EF2T_PARA )
                            {
                                y_pos2 = _h - ( _step_height * _step ) + ( oy - diffy() ) > oy - diffy() ?
                                         _h - ( _step_height * _step ) : 0;
                                x_pos2 = 0;
                                drawObject( kpobject, screen, x_pos2, y_pos2, 0, 0, 0, 0 );
                                if ( y_pos2 != 0 ) nothingHappens = false;
                                xy.at( i )->setX( x_pos2 );
                                xy.at( i )->setY( y_pos2 );
                                xy.at( i )->setWidth( 0 );
                                xy.at( i )->setHeight( 0 );
                            }
                        } break;
                        case EF_COME_LEFT_TOP:
                        {
                            if ( subPresStep == 0 || subPresStep != 0 && kpobject->getType() == OT_TEXT && kpobject->getEffect2() == EF2T_PARA )
                            {
                                x_pos1 = _step_width * _step < ox - diffx() + ow ?
                                         ox - diffx() + ow - _step_width * _step : 0;
                                y_pos1 = _step_height * _step < oy - diffy() + oh ?
                                         oy - diffy() + oh - _step_height * _step : 0;
                                drawObject( kpobject, screen, -x_pos1, -y_pos1, 0, 0, 0, 0 );
                                if ( x_pos1 != 0 || y_pos1 != 0 ) nothingHappens = false;
                                xy.at( i )->setX( -x_pos1 );
                                xy.at( i )->setY( -y_pos1 );
                                xy.at( i )->setWidth( 0 );
                                xy.at( i )->setHeight( 0 );
                            }
                        } break;
                        case EF_COME_LEFT_BOTTOM:
                        {
                            if ( subPresStep == 0 || subPresStep != 0 && kpobject->getType() == OT_TEXT && kpobject->getEffect2() == EF2T_PARA )
                            {
                                x_pos1 = _step_width * _step < ox - diffx() + ow ?
                                         ox - diffx() + ow - _step_width * _step : 0;
                                y_pos2 = _h - ( _step_height * _step ) + ( oy - diffy() ) > oy - diffy() ?
                                         _h - ( _step_height * _step ) : 0;
                                drawObject( kpobject, screen, -x_pos1, y_pos2, 0, 0, 0, 0 );
                                if ( x_pos1 != 0 || y_pos2 != 0 ) nothingHappens = false;
                                xy.at( i )->setX( -x_pos1 );
                                xy.at( i )->setY( y_pos2 );
                                xy.at( i )->setWidth( 0 );
                                xy.at( i )->setHeight( 0 );
                            }
                        } break;
                        case EF_COME_RIGHT_TOP:
                        {
                            if ( subPresStep == 0 || subPresStep != 0 && kpobject->getType() == OT_TEXT && kpobject->getEffect2() == EF2T_PARA )
                            {
                                x_pos2 = _w - ( _step_width * _step ) + ( ox - diffx() ) > ox - diffx() ?
                                         _w - ( _step_width * _step ) : 0;
                                y_pos1 = _step_height * _step < oy - diffy() + oh ?
                                         oy - diffy() + oh - _step_height * _step : 0;
                                drawObject( kpobject, screen, x_pos2, -y_pos1, 0, 0, 0, 0 );
                                if ( x_pos2 != 0 || y_pos1 != 0 ) nothingHappens = false;
                                xy.at( i )->setX( x_pos2 );
                                xy.at( i )->setY( -y_pos1 );
                                xy.at( i )->setWidth( 0 );
                                xy.at( i )->setHeight( 0 );
                            }
                        } break;
                        case EF_COME_RIGHT_BOTTOM:
                        {
                            if ( subPresStep == 0 || subPresStep != 0 && kpobject->getType() == OT_TEXT && kpobject->getEffect2() == EF2T_PARA )
                            {
                                x_pos2 = _w - ( _step_width * _step ) + ( ox - diffx() ) > ox - diffx() ?
                                         _w - ( _step_width * _step ) : 0;
                                y_pos2 = _h - ( _step_height * _step ) + ( oy - diffy() ) > oy - diffy() ?
                                         _h - ( _step_height * _step ) : 0;
                                drawObject( kpobject, screen, x_pos2, y_pos2, 0, 0, 0, 0 );
                                if ( x_pos2 != 0 || y_pos2 != 0 ) nothingHappens = false;
                                xy.at( i )->setX( x_pos2 );
                                xy.at( i )->setY( y_pos2 );
                                xy.at( i )->setWidth( 0 );
                                xy.at( i )->setHeight( 0 );
                            }
                        } break;
                        case EF_WIPE_LEFT:
                        {
                            if ( subPresStep == 0 || subPresStep != 0 && kpobject->getType() == OT_TEXT && kpobject->getEffect2() == EF2T_PARA )
                            {
                                w_pos1 = _step_width * ( _steps1 - _step ) > 0 ? _step_width * ( _steps1 - _step ) : 0;
                                drawObject( kpobject, screen, 0, 0, w_pos1, 0, 0, 0 );
                                if ( w_pos1 != 0 ) nothingHappens = false;
                                xy.at( i )->setX( 0 );
                                xy.at( i )->setY( 0 );
                                xy.at( i )->setWidth( w_pos1 );
                                xy.at( i )->setHeight( 0 );
                            }
                        } break;
                        case EF_WIPE_RIGHT:
                        {
                            if ( subPresStep == 0 || subPresStep != 0 && kpobject->getType() == OT_TEXT && kpobject->getEffect2() == EF2T_PARA )
                            {
                                w_pos1 = _step_width * ( _steps1 - _step ) > 0 ? _step_width * ( _steps1 - _step ) : 0;
                                x_pos1 = w_pos1;
                                drawObject( kpobject, screen, 0, 0, w_pos1, 0, x_pos1, 0 );
                                if ( w_pos1 != 0 ) nothingHappens = false;
                                xy.at( i )->setX( x_pos1 );
                                xy.at( i )->setY( 0 );
                                xy.at( i )->setWidth( w_pos1 );
                                xy.at( i )->setHeight( 0 );
                            }
                        } break;
                        case EF_WIPE_TOP:
                        {
                            if ( subPresStep == 0 || subPresStep != 0 && kpobject->getType() == OT_TEXT && kpobject->getEffect2() == EF2T_PARA )
                            {
                                h_pos1 = _step_height * ( _steps1 - _step ) > 0 ? _step_height * ( _steps1 - _step ) : 0;
                                drawObject( kpobject, screen, 0, 0, 0, h_pos1, 0, 0 );
                                if ( h_pos1 != 0 ) nothingHappens = false;
                                xy.at( i )->setX( 0 );
                                xy.at( i )->setY( 0 );
                                xy.at( i )->setWidth( 0 );
                                xy.at( i )->setHeight( h_pos1 );
                            }
                        } break;
                        case EF_WIPE_BOTTOM:
                        {
                            if ( subPresStep == 0 || subPresStep != 0 && kpobject->getType() == OT_TEXT && kpobject->getEffect2() == EF2T_PARA )
                            {
                                h_pos1 = _step_height * ( _steps1 - _step ) > 0 ? _step_height * ( _steps1 - _step ) : 0;
                                y_pos1 = h_pos1;
                                drawObject( kpobject, screen, 0, 0, 0, h_pos1, 0, y_pos1 );
                                if ( h_pos1 != 0 ) nothingHappens = false;
                                xy.at( i )->setX( 0 );
                                xy.at( i )->setY( y_pos1 );
                                xy.at( i )->setWidth( 0 );
                                xy.at( i )->setHeight( h_pos1 );
                            }
                        } break;
                        default: break;
                        }
                    }
                    else
                    {
                        if ( subPresStep == 0 )
                        {
                            switch ( kpobject->getEffect3() )
                            {
                            case EF3_NONE:
                                //drawObject( kpobject, screen, ox, oy, 0, 0, 0, 0 );
                                break;
                            case EF3_GO_LEFT:
                            {
                                x_pos1 = _step_width * _step < ox - diffx() + ow ?
                                         ox - diffx() + ow - _step_width * _step : 0;
                                y_pos1 = 0;
                                drawObject( kpobject, screen, -( ox + ow - x_pos1 ), y_pos1, 0, 0, 0, 0 );
                                if ( x_pos1 != 0 ) nothingHappens = false;
                                xy.at( i )->setX( -( ox + ow - x_pos1 ) );
                                xy.at( i )->setY( y_pos1 );
                                xy.at( i )->setWidth( 0 );
                                xy.at( i )->setHeight( 0 );
                            } break;
                            case EF3_GO_TOP:
                            {
                                y_pos1 = _step_height * _step < oy - diffy() + oh ?
                                         oy - diffy() + oh - _step_height * _step : 0;
                                x_pos1 = 0;
                                drawObject( kpobject, screen, x_pos1, -( ( oy - diffy() ) + oh - y_pos1 ), 0, 0, 0, 0 );
                                if ( y_pos1 != 0 ) nothingHappens = false;
                                xy.at( i )->setX( x_pos1 );
                                xy.at( i )->setY( -( ( oy - diffy() ) + oh - y_pos1 ) );
                                xy.at( i )->setWidth( 0 );
                                xy.at( i )->setHeight( 0 );
                            } break;
                            case EF3_GO_RIGHT:
                            {
                                x_pos2 = _w - ( _step_width * _step ) + ( ox - diffx() ) > ox - diffx() ?
                                         _w - ( _step_width * _step ) : 0;
                                y_pos2 = 0;
                                int __w = kapp->desktop()->width() - ox;
                                drawObject( kpobject, screen, __w - x_pos2, y_pos2, 0, 0, 0, 0 );
                                if ( x_pos2 != 0 ) nothingHappens = false;
                                xy.at( i )->setX( __w - x_pos2 );
                                xy.at( i )->setY( y_pos2 );
                                xy.at( i )->setWidth( 0 );
                                xy.at( i )->setHeight( 0 );
                            } break;
                            case EF3_GO_BOTTOM:
                            {
                                y_pos2 = _h - ( _step_height * _step ) + ( oy - diffy() ) > oy - diffy() ?
                                         _h - ( _step_height * _step ) : 0;
                                x_pos2 = 0;
                                int __h = kapp->desktop()->height() - ( oy - diffy() );
                                drawObject( kpobject, screen, x_pos2, __h - y_pos2, 0, 0, 0, 0 );
                                if ( y_pos2 != 0 ) nothingHappens = false;
                                xy.at( i )->setX( x_pos2 );
                                xy.at( i )->setY( __h - y_pos2 );
                                xy.at( i )->setWidth( 0 );
                                xy.at( i )->setHeight( 0 );
                            } break;
                            case EF3_GO_LEFT_TOP:
                            {
                                x_pos1 = _step_width * _step < ox - diffx() + ow ?
                                         ox - diffx() + ow - _step_width * _step : 0;
                                y_pos1 = _step_height * _step < oy - diffy() + oh ?
                                         oy - diffy() + oh - _step_height * _step : 0;
                                drawObject( kpobject, screen, -( ox + ow - x_pos1 ), -( ( oy - diffy() ) + oh - y_pos1 ), 0, 0, 0, 0 );
                                if ( x_pos1 != 0 || y_pos1 != 0 ) nothingHappens = false;
                                xy.at( i )->setX( -( ox + ow - x_pos1 ) );
                                xy.at( i )->setY( -( ( oy - diffy() ) + oh - y_pos1 ) );
                                xy.at( i )->setWidth( 0 );
                                xy.at( i )->setHeight( 0 );
                            } break;
                            case EF3_GO_LEFT_BOTTOM:
                            {
                                x_pos1 = _step_width * _step < ox - diffx() + ow ?
                                         ox - diffx() + ow - _step_width * _step : 0;
                                y_pos2 = _h - ( _step_height * _step ) + ( oy - diffy() ) > oy - diffy() ?
                                         _h - ( _step_height * _step ) : 0;
                                int __h = kapp->desktop()->height() - ( oy - diffy() );
                                drawObject( kpobject, screen, -( ox + ow - x_pos1 ), __h -  y_pos2, 0, 0, 0, 0 );
                                if ( x_pos1 != 0 || y_pos2 != 0 ) nothingHappens = false;
                                xy.at( i )->setX( -( ox + ow - x_pos1 ) );
                                xy.at( i )->setY( __h - y_pos2 );
                                xy.at( i )->setWidth( 0 );
                                xy.at( i )->setHeight( 0 );
                            } break;
                            case EF3_GO_RIGHT_TOP:
                            {
                                x_pos2 = _w - ( _step_width * _step ) + ( ox - diffx() ) > ox - diffx() ?
                                         _w - ( _step_width * _step ) : 0;
                                y_pos1 = _step_height * _step < oy - diffy() + oh ?
                                         oy - diffy() + oh - _step_height * _step : 0;
                                int __w = kapp->desktop()->width() - ox;
                                drawObject( kpobject, screen, __w - x_pos2, -( ( oy - diffy() ) + oh - y_pos1 ), 0, 0, 0, 0 );
                                if ( x_pos2 != 0 || y_pos1 != 0 ) nothingHappens = false;
                                xy.at( i )->setX( __w - x_pos2 );
                                xy.at( i )->setY( -( ( oy - diffy() ) + oh - y_pos1 ) );
                                xy.at( i )->setWidth( 0 );
                                xy.at( i )->setHeight( 0 );
                            } break;
                            case EF3_GO_RIGHT_BOTTOM:
                            {
                                x_pos2 = _w - ( _step_width * _step ) + ( ox - diffx() ) > ox - diffx() ?
                                         _w - ( _step_width * _step ) : 0;
                                y_pos2 = _h - ( _step_height * _step ) + ( oy - diffy() ) > oy - diffy() ?
                                         _h - ( _step_height * _step ) : 0;
                                int __w = kapp->desktop()->width() - ox;
                                int __h = kapp->desktop()->height() - ( oy - diffy() );
                                drawObject( kpobject, screen, __w - x_pos2, __h - y_pos2, 0, 0, 0, 0 );
                                if ( x_pos2 != 0 || y_pos2 != 0 ) nothingHappens = false;
                                xy.at( i )->setX( __w - x_pos2 );
                                xy.at( i )->setY( __h - y_pos2 );
                                xy.at( i )->setWidth( 0 );
                                xy.at( i )->setHeight( 0 );
                            } break;
                            default:
                                break;
                            }
                        }
                    }
                    newRect.setRect( bx - ( diffx() - xy.at( i )->x() ), by - ( diffy() - xy.at( i )->y() ), bw - xy.at( i )->width(), bh - xy.at( i )->height() );
                    newRect = newRect.unite( oldRect );

                    bool append = true;
                    for ( unsigned j = 0; j < changes.count(); j++ )
                    {
                        if ( changes.at( j )->intersects( newRect ) )
                        {
                            QRect r = changes.at( j )->intersect( newRect );
                            int s1 = r.width() * r.height();
                            int s2 = newRect.width() * newRect.height();
                            int s3 = changes.at( j )->width() * changes.at( j )->height();

                            if ( s1 > ( s2 / 100 ) * 50 || s1 > ( s3 / 100 ) * 50 )
                            {
                                QRect rr = changes.at( j )->unite( newRect );
                                changes.at( j )->setRect( rr.x(), rr.y(), rr.width(), rr.height() );
                                append = false;
                            }

                            break;
                        }
                    }

                    if ( append )
                        changes.append( new QRect( newRect ) );
                }

                QRect *changed;
                for ( int i = 0; i < static_cast<int>( changes.count() ); i++ )
                {
                    changed = changes.at( i );
                    bitBlt( this, changed->x(), changed->y(), screen, changed->x(), changed->y(), changed->width(), changed->height() );
                }

                delete screen;
                screen = new QPixmap( screen_orig );

                _time.restart();
            }
        }
    }

    if ( !effects )
    {
        //kdDebug() << "Page::doObjEffects no effects" << endl;
        QPainter p;
        p.begin( this );
        p.drawPixmap( 0, 0, screen_orig );
        drawObjects( &p, QRect( 0, 0, kapp->desktop()->width(), kapp->desktop()->height() ) );
        p.end();
    }
    else
    {
        //kdDebug() << "Page::doObjEffects effects" << endl;
        QPainter p;
        p.begin( screen );
        drawObjects( &p, QRect( 0, 0, kapp->desktop()->width(), kapp->desktop()->height() ) );
        p.end();
        bitBlt( this, 0, 0, screen );
    }

    delete screen;
}

/*======================= draw object ============================*/
void Page::drawObject( KPObject *kpobject, QPixmap *screen, int _x, int _y, int _w, int _h, int _cx, int _cy )
{
    if ( kpobject->getDisappear() &&
         kpobject->getDisappearNum() < static_cast<int>( currPresStep ) )
        return;

    int ox, oy, ow, oh;
    QRect br = kpobject->getBoundingRect( 0, 0 );
    ox = br.x(); oy = br.y(); ow = br.width(); oh = br.height();
    bool ownClipping = true;

    QPainter p;
    p.begin( screen );

    if ( _w != 0 || _h != 0 )
    {
        p.setClipping( true );
        p.setClipRect( ox - diffx() + _cx, oy - diffy() + _cy, ow - _w, oh - _h );
        ownClipping = false;
    }

    if ( !editMode && static_cast<int>( currPresStep ) == kpobject->getPresNum() && !goingBack )
    {
        kpobject->setSubPresStep( subPresStep );
        kpobject->doSpecificEffects( true );
        kpobject->setOwnClipping( ownClipping );
    }

    kpobject->draw( &p, diffx() - _x, diffy() - _y );
    kpobject->setSubPresStep( 0 );
    kpobject->doSpecificEffects( false );
    kpobject->setOwnClipping( true );

    KPObject *obj = 0;
    for ( unsigned int i = tmpObjs.findRef( kpobject ) + 1; i < tmpObjs.count(); i++ ) {
        obj = tmpObjs.at( i );
        if ( kpobject->getBoundingRect( 0, 0 ).intersects( obj->getBoundingRect( 0, 0 ) ) &&
             obj->getPresNum() < static_cast<int>( currPresStep ) )
            obj->draw( &p, diffx(), diffy() );
    }

    p.end();
}

/*======================== print =================================*/
void Page::print( QPainter *painter, KPrinter *printer, float left_margin, float top_margin )
{
    printer->setFullPage( true );
    int i = 0;

    repaint( false );
    kapp->processEvents();

    editMode = false;
    fillBlack = false;
    _presFakt = 1.0;

    int _xOffset = view->getDiffX();
    int _yOffset = view->getDiffY();

    currPresStep = 1000;
    subPresStep = 1000;

    for ( i = 0; i < static_cast<int>( objectList()->count() ); i++ )
        objectList()->at( i )->drawSelection( false );

    view->setDiffX( -static_cast<int>( MM_TO_POINT( left_margin ) ) );
    view->setDiffY( -static_cast<int>( MM_TO_POINT( top_margin ) ) );

    QColor c = kapp->winStyleHighlightColor();
    kapp->setWinStyleHighlightColor( colorGroup().highlight() );

#ifndef HAVE_KDEPRINT
    QProgressDialog progress( i18n( "Printing..." ), i18n( "Cancel" ),
                              printer->toPage() - printer->fromPage() + 2, this );

    int j = 0;
    progress.setProgress( 0 );

    if ( printer->fromPage() > 1 )
        view->setDiffY( ( printer->fromPage() - 1 ) * ( getPageRect( 1, 1.0, false ).height() ) -
                        MM_TO_POINT( top_margin ) );

    for ( i = printer->fromPage(); i <= printer->toPage(); i++ )
    {
        progress.setProgress( ++j );
        kapp->processEvents();

        if ( progress.wasCancelled() )
            break;

        currPresPage = i;
        if ( i > printer->fromPage() ) printer->newPage();

        painter->resetXForm();
        painter->fillRect( getPageRect( 0 ), white );

        drawPageInPainter( painter, view->getDiffY(), getPageRect( i - 1 ) );
        kapp->processEvents();

        painter->resetXForm();
        kapp->processEvents();

        view->setDiffY( i * ( getPageRect( 1, 1.0, false ).height() ) - MM_TO_POINT( top_margin ) );
    }

    setToolEditMode( toolEditMode );
    view->setDiffX( _xOffset );
    view->setDiffY( _yOffset );

    progress.setProgress( printer->toPage() - printer->fromPage() + 2 );
    kapp->setWinStyleHighlightColor( c );

    QListIterator<KPObject> oIt( *objectList() );
    for (; oIt.current(); ++oIt )
        oIt.current()->drawSelection( true );
#else
    QValueList<int> pages_(pages(printer->option("kde-range")));
    // okay, if it's empty we simply print everything...
    if(pages_.isEmpty()) {
        for(int k=printer->fromPage(); k<=printer->toPage(); ++k)
            pages_.append(k);
    }
    QProgressDialog progress( i18n( "Printing..." ), i18n( "Cancel" ),
                              pages_.count() + 2, this );

    int j = 0;
    progress.setProgress( 0 );

    QValueList<int>::ConstIterator it;
    for ( it=pages_.begin() ; it!=pages_.end(); ++it )
    {
        progress.setProgress( ++j );
        kapp->processEvents();

        if ( progress.wasCancelled() )
            break;

        currPresPage = *it;
        if (j!=1) printer->newPage();

        painter->resetXForm();
        painter->fillRect( getPageRect( 0 ), white );

        view->setDiffY( (*it-1) * ( getPageRect( 1, 1.0, false ).height() ) - qRound(MM_TO_POINT( top_margin )) );
        drawPageInPainter( painter, view->getDiffY(), getPageRect( *it - 1 ) );
        kapp->processEvents();

        painter->resetXForm();
        kapp->processEvents();
    }

    setToolEditMode( toolEditMode );
    view->setDiffX( _xOffset );
    view->setDiffY( _yOffset );

    progress.setProgress( pages_.count() );
    kapp->setWinStyleHighlightColor( c );

    QListIterator<KPObject> oIt( *objectList() );
    for (; oIt.current(); ++oIt )
        oIt.current()->drawSelection( true );
#endif

    currPresPage = 1;
    currPresStep = 0;
    subPresStep = 0;
    _presFakt = 1.0;
    fillBlack = true;
    editMode = true;
    repaint( false );
}

/*================================================================*/
void Page::editSelectedTextArea()
{
    KPObject *kpobject = 0;

    if ( (int)objectList()->count() - 1 >= 0 ) {
	for ( int i = static_cast<int>( objectList()->count() ) - 1; i >= 0; i-- ) {
	    kpobject = objectList()->at( i );
	    if ( kpobject->isSelected() ) {
		if ( kpobject->getType() == OT_TEXT ) {
		    KPTextObject *kptextobject = dynamic_cast<KPTextObject*>( kpobject );

		    kpobject->activate( this, diffx(), diffy() );
		    setTextBackground( kptextobject );
		    connect( kptextobject->getKTextObject(), SIGNAL( currentFontChanged( const QFont & ) ),
			     this, SLOT( toFontChanged( const QFont & ) ) );
		    connect( kptextobject->getKTextObject(), SIGNAL( currentColorChanged( const QColor & ) ),
			     this, SLOT( toColorChanged( const QColor & ) ) );
		    connect( kptextobject->getKTextObject(), SIGNAL( currentAlignmentChanged( int ) ),
			     this, SLOT( toAlignChanged( int ) ) );
		    connect( kptextobject->getKTextObject(), SIGNAL( exitEditMode() ),
			     this, SLOT( exitEditMode() ) );
		    editNum = i;
		    break;
		}
	    }
	}
    }
}

/*================================================================*/
void Page::insertText( QRect _r )
{
    view->kPresenterDoc()->insertText( _r, diffx(), diffy() );
    selectObj( objectList()->last() );
}

/*================================================================*/
void Page::insertLineH( QRect _r, bool rev )
{
    view->kPresenterDoc()->insertLine( _r, view->getPen(),
                                       !rev ? view->getLineBegin() : view->getLineEnd(), !rev ? view->getLineEnd() : view->getLineBegin(),
                                       LT_HORZ, diffx(), diffy() );
}

/*================================================================*/
void Page::insertLineV( QRect _r, bool rev )
{
    view->kPresenterDoc()->insertLine( _r, view->getPen(),
                                       !rev ? view->getLineBegin() : view->getLineEnd(), !rev ? view->getLineEnd() : view->getLineBegin(),
                                       LT_VERT, diffx(), diffy() );
}

/*================================================================*/
void Page::insertLineD1( QRect _r, bool rev )
{
    view->kPresenterDoc()->insertLine( _r, view->getPen(),
                                       !rev ? view->getLineBegin() : view->getLineEnd(), !rev ? view->getLineEnd() : view->getLineBegin(),
                                       LT_LU_RD, diffx(), diffy() );
}

/*================================================================*/
void Page::insertLineD2( QRect _r, bool rev )
{
    view->kPresenterDoc()->insertLine( _r, view->getPen(),
                                       !rev ? view->getLineBegin() : view->getLineEnd(), !rev ? view->getLineEnd() : view->getLineBegin(),
                                       LT_LD_RU, diffx(), diffy() );
}

/*================================================================*/
void Page::insertRect( QRect _r )
{
    view->kPresenterDoc()->insertRectangle( _r, view->getPen(), view->getBrush(), view->getFillType(),
                                            view->getGColor1(), view->getGColor2(), view->getGType(), view->getRndX(), view->getRndY(),
                                            view->getGUnbalanced(), view->getGXFactor(), view->getGYFactor(), diffx(), diffy() );
}

/*================================================================*/
void Page::insertEllipse( QRect _r )
{
    view->kPresenterDoc()->insertCircleOrEllipse( _r, view->getPen(), view->getBrush(), view->getFillType(),
                                                  view->getGColor1(), view->getGColor2(),
                                                  view->getGType(), view->getGUnbalanced(), view->getGXFactor(), view->getGYFactor(),
                                                  diffx(), diffy() );
}

/*================================================================*/
void Page::insertPie( QRect _r )
{
    view->kPresenterDoc()->insertPie( _r, view->getPen(), view->getBrush(), view->getFillType(),
                                      view->getGColor1(), view->getGColor2(), view->getGType(),
                                      view->getPieType(), view->getPieAngle(), view->getPieLength(),
                                      view->getLineBegin(), view->getLineEnd(), view->getGUnbalanced(), view->getGXFactor(), view->getGYFactor(),
                                      diffx(), diffy() );
}

/*================================================================*/
void Page::insertAutoform( QRect _r, bool rev )
{
    rev = false;
    view->kPresenterDoc()->insertAutoform( _r, view->getPen(), view->getBrush(),
                                           !rev ? view->getLineBegin() : view->getLineEnd(), !rev ? view->getLineEnd() : view->getLineBegin(),
                                           view->getFillType(), view->getGColor1(), view->getGColor2(), view->getGType(),
                                           autoform, view->getGUnbalanced(), view->getGXFactor(), view->getGYFactor(),
                                           diffx(), diffy() );
}

/*================================================================*/
void Page::insertObject( QRect _r )
{
    view->kPresenterDoc()->insertObject( _r, partEntry, diffx(), diffy() );
}

/*================================================================*/
void Page::insertTable( QRect _r )
{
    view->kPresenterDoc()->insertObject( _r, partEntry, diffx(), diffy() );
}

/*================================================================*/
void Page::insertDiagramm( QRect _r )
{
    view->kPresenterDoc()->insertObject( _r, partEntry, diffx(), diffy() );
}

/*================================================================*/
void Page::insertFormula( QRect _r )
{
    view->kPresenterDoc()->insertObject( _r, partEntry, diffx(), diffy() );
}

/*================================================================*/
void Page::setToolEditMode( ToolEditMode _m, bool updateView )
{
    exitEditMode();
    toolEditMode = _m;

    if ( toolEditMode == TEM_MOUSE ) {
        setCursor( arrowCursor );
        for ( int i = static_cast<int>( objectList()->count() ) - 1; i >= 0; i-- ) {
            KPObject *kpobject = objectList()->at( i );
            if ( kpobject->contains( QCursor::pos(), diffx(), diffy() ) ) {
                if ( kpobject->isSelected() )
                    setCursor( kpobject->getCursor( QCursor::pos(), diffx(), diffy(), modType ) );
                break;
            }
        }
    } else
        setCursor( crossCursor );

    if ( updateView )
        view->setTool( toolEditMode );
}

/*================================================================*/
void Page::selectNext()
{
    if ( objectList()->count() == 0 ) return;

    if ( view->kPresenterDoc()->numSelected() == 0 )
        objectList()->at( 0 )->setSelected( true );
    else {
        int i = objectList()->findRef( view->kPresenterDoc()->getSelectedObj() );
        if ( i < static_cast<int>( objectList()->count() ) - 1 ) {
            view->kPresenterDoc()->deSelectAllObj();
            objectList()->at( ++i )->setSelected( true );
        } else {
            view->kPresenterDoc()->deSelectAllObj();
            objectList()->at( 0 )->setSelected( true );
        }
    }
    if ( !QRect( diffx(), diffy(), width(), height() ).
         contains( view->kPresenterDoc()->getSelectedObj()->getBoundingRect( 0, 0 ) ) )
        view->makeRectVisible( view->kPresenterDoc()->getSelectedObj()->getBoundingRect( 0, 0 ) );
    _repaint( false );
}

/*================================================================*/
void Page::selectPrev()
{
    if ( objectList()->count() == 0 ) return;

    if ( view->kPresenterDoc()->numSelected() == 0 )
        objectList()->at( objectList()->count() - 1 )->setSelected( true );
    else {
        int i = objectList()->findRef( view->kPresenterDoc()->getSelectedObj() );
        if ( i > 0 ) {
            view->kPresenterDoc()->deSelectAllObj();
            objectList()->at( --i )->setSelected( true );
        } else {
            view->kPresenterDoc()->deSelectAllObj();
            objectList()->at( objectList()->count() - 1 )->setSelected( true );
        }
    }
    view->makeRectVisible( view->kPresenterDoc()->getSelectedObj()->getBoundingRect( 0, 0 ) );
    _repaint( false );
}

/*================================================================*/
void Page::dragEnterEvent( QDragEnterEvent *e )
{
    if ( QTextDrag::canDecode( e ) ||
         QImageDrag::canDecode( e ) )
        e->accept();
    else
        e->ignore();
}

/*================================================================*/
void Page::dragLeaveEvent( QDragLeaveEvent * /*e*/ )
{
}

/*================================================================*/
void Page::dragMoveEvent( QDragMoveEvent *e )
{
    if ( QTextDrag::canDecode( e ) ||
         QImageDrag::canDecode( e ) )
        e->accept();
    else
        e->ignore();
}

/*================================================================*/
void Page::dropEvent( QDropEvent *e )
{
    KPresenterDoc *doc = view->kPresenterDoc();

    if ( QImageDrag::canDecode( e ) ) {
        setToolEditMode( TEM_MOUSE );
        deSelectAllObj();

        QImage pix;
        QImageDrag::decode( e, pix );

        KTempFile tmpFile;
        tmpFile.setAutoDelete(true);

	if( tmpFile.status() != 0 ) {
	    return;
	}
        tmpFile.close();

        pix.save( tmpFile.name(), "PNG" );
        QCursor c = cursor();
        setCursor( waitCursor );
        doc->insertPicture( tmpFile.name(), e->pos().x(), e->pos().y() +
                            ( currPgNum() - 1) * getPageRect( 0 ).height() );
        setCursor( c );

        e->accept();
    } else if ( QUriDrag::canDecode( e ) ) {
        setToolEditMode( TEM_MOUSE );
        deSelectAllObj();

        QStringList lst;
        QUriDrag::decodeToUnicodeUris( e, lst );

        QStringList::Iterator it = lst.begin();
        for ( ; it != lst.end(); ++it ) {
            KURL url( *it );

            QString filename;
            if ( !url.isLocalFile() ) {
                filename = QString::null;
                // #### todo download file
            } else {
                filename = url.path();
            }

            KMimeMagicResult *res = KMimeMagic::self()->findFileType( filename );

            if ( res && res->isValid() ) {
                QString mimetype = res->mimeType();
                if ( mimetype.contains( "image" ) ) {
                    QCursor c = cursor();
                    setCursor( waitCursor );
                    doc->insertPicture( filename, e->pos().x(), e->pos().y() +
                                        ( currPgNum() - 1) * getPageRect( 0 ).height() );
                    setCursor( c );
                } else if ( mimetype.contains( "text" ) ) {
                    QCursor c = cursor();
                    setCursor( waitCursor );
                    QFile f( filename );
                    QTextStream t( &f );
                    QString text = QString::null, tmp;

                    if ( f.open( IO_ReadOnly ) ) {
                        while ( !t.eof() ) {
                            tmp = t.readLine();
                            tmp += "\n";
                            text.append( tmp );
                        }
                        f.close();
                    }

                    doc->insertText( QRect( e->pos().x(), e->pos().y(), 250, 250 ),
                                     diffx(), diffy(), text, view );

                    setCursor( c );
                }
            }
        }
    } else if ( QTextDrag::canDecode( e ) ) {
        setToolEditMode( TEM_MOUSE );
        deSelectAllObj();

        QString text;
        QTextDrag::decode( e, text );

        doc->insertText( QRect( e->pos().x(), e->pos().y(), 250, 250 ),
                         diffx(), diffy(), text, view );
        e->accept();
    } else
        e->ignore();

}

/*================================================================*/
void Page::slotGotoPage()
{
    setCursor( blankCursor );
    int pg = currPresPage;
    pg = KPGotoPage::gotoPage( view->kPresenterDoc(), _presFakt, slideList, pg, this );
    gotoPage( pg );
}

/*================================================================*/
void Page::gotoPage( int pg )
{
    if ( pg != static_cast<int>( currPresPage ) ) {
        currPresPage = pg;
        //kdDebug() << "Page::gotoPage currPresPage=" << currPresPage << endl;
        slideListIterator = slideList.find( currPresPage );
        editMode = false;
        drawMode = false;
        presStepList = view->kPresenterDoc()->reorderPage( currPresPage, diffx(), diffy(), _presFakt );
        currPresStep = *presStepList.begin();
        subPresStep = 0;

        int yo = view->kPresenterDoc()->getPageRect( 0, 0, 0, presFakt(), false ).height() * ( pg - 1 );
        // ## shouldn't this center the slide if it's smaller ? (see KPresenterView::startScreenPresentation)
        view->setDiffY( yo );
        //kdDebug() << "Page::gotoPage :   setDiffY " << yo << endl;
        resize( QApplication::desktop()->width(), QApplication::desktop()->height() );
        repaint( false );
        setFocus();
        view->refreshPageButton();
    }
}

/*================================================================*/
KTextEdit* Page::kTxtObj()
{
    return ( ( editNum != -1 && objectList()->at( editNum )->getType() == OT_TEXT ) ?
             dynamic_cast<KPTextObject*>( objectList()->at( editNum ) )->getKTextObject() : 0 );
}

/*================================================================*/
KPTextObject* Page::kpTxtObj()
{
    return ( ( editNum != -1 && objectList()->at( editNum )->getType() == OT_TEXT ) ?
             dynamic_cast<KPTextObject*>( objectList()->at( editNum ) ) : 0 );
}

/*================================================================*/
void Page::deleteObjs()
{
    view->kPresenterDoc()->deleteObjs();
    setToolEditMode( toolEditMode );
}

/*================================================================*/
void Page::rotateObjs()
{
    view->extraRotate();
    setToolEditMode( toolEditMode );
}

/*================================================================*/
void Page::shadowObjs()
{
    view->extraShadow();
    setToolEditMode( toolEditMode );
}

/*================================================================*/
void Page::enterEvent( QEvent *e )
{
    view->setRulerMousePos( ( ( QMouseEvent* )e )->x(), ( ( QMouseEvent* )e )->y() );
    view->setRulerMouseShow( true );
}

/*================================================================*/
void Page::leaveEvent( QEvent * /*e*/ )
{
    view->setRulerMouseShow( false );
}

/*================================================================*/
QList<KPBackGround> *Page::backgroundList()
{
    return view->kPresenterDoc()->backgroundList();
}
const QList<KPBackGround> *Page::backgroundList() const
{
    return view->kPresenterDoc()->backgroundList();
}

/*================================================================*/
QList<KPObject> *Page::objectList()
{
    return view->kPresenterDoc()->objectList();
}
const QList<KPObject> *Page::objectList() const
{
    return view->kPresenterDoc()->objectList();
}

/*================================================================*/
unsigned int Page::objNums()
{
    return view->kPresenterDoc()->objNums();
}

/*================================================================*/
unsigned int Page::currPgNum()
{
    return view->getCurrPgNum();
}

/*================================================================*/
unsigned int Page::rastX() const
{
    return view->kPresenterDoc()->rastX();
}

/*================================================================*/
unsigned int Page::rastY() const
{
    return view->kPresenterDoc()->rastY();
}

/*================================================================*/
QColor Page::txtBackCol() const
{
    return view->kPresenterDoc()->txtBackCol();
}

/*================================================================*/
bool Page::spInfinitLoop() const
{
    return view->kPresenterDoc()->spInfinitLoop();
}

/*================================================================*/
bool Page::spManualSwitch() const
{
    return view->kPresenterDoc()->spManualSwitch();
}

/*================================================================*/
QRect Page::getPageRect( unsigned int p, float fakt, bool decBorders )
{
    return view->kPresenterDoc()->getPageRect( p, diffx(), diffy(), fakt, decBorders );
}

/*================================================================*/
unsigned int Page::pageNums()
{
    return view->kPresenterDoc()->getPageNums();
}

/*================================================================*/
int Page::getPageOfObj( int i, float fakt )
{
    // This works much better with 0,0 than with diffx, diffy
    // (the objects don't appear in the view-current-page-fullscreen feature IIRC)
    // See comment in KPresenterDoc::getPageOfObj. Passing diffx/diffy
    // seems useless to me, but if it changes something, then it's not
    // that useless... This needs to be cleaned up :)  (DF)
    return view->kPresenterDoc()->getPageOfObj( i, 0 /*diffx()*/, 0 /*diffy()*/, fakt );
}

/*================================================================*/
float Page::objSpeedFakt()
{
    /*
      Used to be 0(slow)->70, 1(medium)->50, 2(fast)->30.
      It's now 0->75, 1->50, 2->37, etc. That's the reason for this strange formula :)
     */
    return 150.0 / static_cast<float>( view->kPresenterDoc()->getPresSpeed() + 2 );
    //return ObjSpeed[ static_cast<int>( view->kPresenterDoc()->getPresSpeed() ) ];
}

/*================================================================*/
float Page::pageSpeedFakt()
{
    /*
      Used to be 0(slow)->8, 1(medium)->16, 2(fast)->32.
      It's now 0->10, 1->20, 2->30, 3->40, 4->50......
     */
    return 10.0 * ( view->kPresenterDoc()->getPresSpeed() + 1 );
    //return PageSpeed[ static_cast<int>( view->kPresenterDoc()->getPresSpeed() ) ];
}

/*================================================================*/
void Page::_repaint( bool /*erase*/ )
{
    view->kPresenterDoc()->repaint( false );
}

/*================================================================*/
void Page::_repaint( QRect r )
{
    view->kPresenterDoc()->repaint( r );
}

/*================================================================*/
void Page::_repaint( KPObject *o )
{
    view->kPresenterDoc()->repaint( o );
}

/*================================================================*/
void Page::alignObjLeft()
{
    view->extraAlignObjLeft();
}

/*================================================================*/
void Page::alignObjCenterH()
{
    view->extraAlignObjCenterH();
}

/*================================================================*/
void Page::alignObjRight()
{
    view->extraAlignObjRight();
}

/*================================================================*/
void Page::alignObjTop()
{
    view->extraAlignObjTop();
}

/*================================================================*/
void Page::alignObjCenterV()
{
    view->extraAlignObjCenterV();
}

/*================================================================*/
void Page::alignObjBottom()
{
    view->extraAlignObjBottom();
}

/*================================================================*/
void Page::pageLayout()
{
    view->extraLayout();
}

void Page::pageDefaultTemplate()
{
    view->extraDefaultTemplate();
}

/*================================================================*/
void Page::pageBackground()
{
    view->extraBackground();
}

/*================================================================*/
void Page::pageInsert()
{
    view->insertPage();
}

/*================================================================*/
void Page::duplicateCopy()
{
    view->editDuplicatePage();
}

/*================================================================*/
void Page::pageDelete()
{
    view->editDelPage();
}

/*================================================================*/
void Page::pagePaste()
{
    view->editPaste();
}

/*================================================================*/
void Page::configPages()
{
    view->screenConfigPages();
}

/*================================================================*/
void Page::presStructView()
{
    view->screenPresStructView();
}

/*================================================================*/
void Page::slotExitPres()
{
    view->screenStop();
}

/*================================================================*/
void Page::slotEditHF()
{
    view->editHeaderFooter();
}

/*================================================================*/
void Page::slotTextContents2Height()
{
    view->textContentsToHeight();
}

/*================================================================*/
void Page::slotTextObj2Contents()
{
    view->textObjectToContents();
}

/*================================================================*/
void Page::objConfigPie()
{
    view->extraConfigPie();
}

/*================================================================*/
void Page::objConfigRect()
{
    view->extraConfigRect();
}

/*================================================================*/
void Page::assignEffect()
{
    view->screenAssignEffect();
}

/*================================================================*/
void Page::drawingMode()
{
    presMenu->setItemChecked( PM_DM, true );
    presMenu->setItemChecked( PM_SM, false );
    drawMode = true;
    setCursor( arrowCursor );
}

/*================================================================*/
void Page::switchingMode()
{
    presMenu->setItemChecked( PM_DM, false );
    presMenu->setItemChecked( PM_SM, true );
    drawMode = false; setCursor( blankCursor );
}

/*================================================================*/
bool Page::calcRatio( int &dx, int &dy, KPObject *kpobject, double ratio ) const
{
    if ( abs( dy ) > abs( dx ) )
        dx = static_cast<int>( static_cast<double>( dy ) * ratio );
    else
        dy = static_cast<int>( static_cast<double>( dx ) / ratio );
    if ( kpobject->getSize().width() + dx < 20 ||
         kpobject->getSize().height() + dy < 20 )
        return false;
    return true;
}

/*================================================================*/
void Page::exitEditMode()
{
    if ( editNum != -1 ) {
        KPObject *kpobject = objectList()->at( editNum );
        editNum = -1;
        if ( kpobject->getType() == OT_TEXT ) {
            KPTextObject * kptextobject = dynamic_cast<KPTextObject*>( kpobject );
            kptextobject->deactivate( view->kPresenterDoc() );
            kptextobject->getKTextObject()->clearFocus();
            disconnect( kptextobject->getKTextObject(), SIGNAL( currentFontChanged( const QFont & ) ),
                        this, SLOT( toFontChanged( const QFont & ) ) );
            disconnect( kptextobject->getKTextObject(), SIGNAL( currentColorChanged( const QColor & ) ),
                        this, SLOT( toColorChanged( const QColor & ) ) );
            disconnect( kptextobject->getKTextObject(), SIGNAL( currentAlignmentChanged( int ) ),
                        this, SLOT( toAlignChanged( int ) ) );
            disconnect( kptextobject->getKTextObject(), SIGNAL( exitEditMode() ),
                        this, SLOT( exitEditMode() ) );

            //kdDebug() << "Page::exitEditMode page=" << currPgNum()-1 << endl;
            // Title of slide may have changed
            emit updateSideBarItem( currPgNum()-1 );

        } else if ( kpobject->getType() == OT_PART ) {
            kpobject->deactivate();
            _repaint( kpobject );
            return;
        }
    }
}

/*================================================================*/
QSize Page::getPixmapOrigSize( KPPixmapObject *&obj )
{
    obj = 0;
    KPObject *kpobject = 0;
    for ( int i = 0; i < static_cast<int>( objectList()->count() ); i++ ) {
        kpobject = objectList()->at( i );
        if ( kpobject->isSelected() && kpobject->getType() == OT_PICTURE ) {
            KPPixmapObject *o = (KPPixmapObject*)kpobject;
            /*
            QImage *img = view->kPresenterDoc()->getPixmapCollection()->
                          getPixmapDataCollection().findPixmapData( o->key.dataKey );
            if ( img ) {
                obj = o;
                return img->size();
            }
            */
            KPImage img = view->kPresenterDoc()->getImageCollection()->
                          findImage( o->getKey() );
            if ( !img.isNull() )
            {
                obj = o;
                return img.size();
            }
        }
    }

    return QSize( -1, -1 );
}

/*================================================================*/
void Page::picViewOrig640x480()
{
    KPPixmapObject *obj = 0;
    QSize origSize = getPixmapOrigSize( obj );
    QSize pgSize = view->kPresenterDoc()->getPageRect( 0, 0, 0 ).size();
    QSize presSize( 640, 480 );
    if ( origSize == QSize( -1, -1 ) || !obj )
        return;

    scalePixmapToBeOrigIn( origSize, pgSize, presSize, obj );
}

/*================================================================*/
void Page::picViewOrig800x600()
{
    KPPixmapObject *obj = 0;
    QSize origSize = getPixmapOrigSize( obj );
    QSize pgSize = view->kPresenterDoc()->getPageRect( 0, 0, 0 ).size();
    QSize presSize( 800, 600 );
    if ( origSize == QSize( -1, -1 ) || !obj )
        return;

    scalePixmapToBeOrigIn( origSize, pgSize, presSize, obj );
}

/*================================================================*/
void Page::picViewOrig1024x768()
{
    KPPixmapObject *obj = 0;
    QSize origSize = getPixmapOrigSize( obj );
    QSize pgSize = view->kPresenterDoc()->getPageRect( 0, 0, 0 ).size();
    QSize presSize( 1024, 768 );
    if ( origSize == QSize( -1, -1 ) || !obj )
        return;

    scalePixmapToBeOrigIn( origSize, pgSize, presSize, obj );
}

/*================================================================*/
void Page::picViewOrig1280x1024()
{
    KPPixmapObject *obj = 0;
    QSize origSize = getPixmapOrigSize( obj );
    QSize pgSize = view->kPresenterDoc()->getPageRect( 0, 0, 0 ).size();
    QSize presSize( 1280, 1024 );
    if ( origSize == QSize( -1, -1 ) || !obj )
        return;

    scalePixmapToBeOrigIn( origSize, pgSize, presSize, obj );
}

/*================================================================*/
void Page::picViewOrig1600x1200()
{
    KPPixmapObject *obj = 0;
    QSize origSize = getPixmapOrigSize( obj );
    QSize pgSize = view->kPresenterDoc()->getPageRect( 0, 0, 0 ).size();
    QSize presSize( 1600, 1200 );
    if ( origSize == QSize( -1, -1 ) || !obj )
        return;

    scalePixmapToBeOrigIn( origSize, pgSize, presSize, obj );
}

/*================================================================*/
void Page::picViewOrigFactor()
{
}

/*================================================================*/
void Page::scalePixmapToBeOrigIn( const QSize &origSize, const QSize &pgSize,
                                  const QSize &presSize, KPPixmapObject *obj )
{
    float fakt = (float)pgSize.width() / (float)presSize.width();
    int w = (int)( (float)origSize.width() * fakt );
    int h = (int)( (float)origSize.height() * fakt );

    ResizeCmd *resizeCmd = new ResizeCmd( i18n( "Scale Picture to be shown 1:1 in presentation mode" ),
                                          QPoint( 0, 0 ), QSize( w - origSize.width(), h - origSize.height() ),
                                          obj, view->kPresenterDoc() );
    resizeCmd->execute();
    view->kPresenterDoc()->commands()->addCommand( resizeCmd );
}

void Page::setTextBackground( KPTextObject *obj )
{
    QPixmap pix( getPageRect( 0 ).size() );
    QPainter painter( &pix );
    backgroundList()->at( view->getCurrPgNum() - 1 )->draw( &painter, QPoint( 0, 0 ), FALSE );
    QPixmap bpix( obj->getSize() );
    bitBlt( &bpix, 0, 0, &pix, obj->getOrig().x(), obj->getOrig().y() -
	    getPageRect( 0 ).height() * ( view->getCurrPgNum() - 1 ), bpix.width(), bpix.height() );
    QBrush b( white, bpix );
    QPalette pal( obj->getKTextObject()->palette() );
    pal.setBrush( QColorGroup::Base, b );
    obj->getKTextObject()->setPalette( pal );
}

QValueList<int> Page::pages(const QString &range) {

    if(range.isEmpty())
        return QValueList<int> ();
    QValueList<int> list;
    int start=-1;
    int end=range.find(',');
    bool ok=true;
    QString tmp;
    while(end!=-1 && start!=end && ok) {
        tmp=range.mid(start+1, end-start-1);
        ok=pagesHelper(tmp, list);
        start=range.find(',', end);
        end=range.find(',', start+1);
    }
    pagesHelper(range.mid(start+1), list);
    return list;
}

bool Page::pagesHelper(const QString &chunk, QValueList<int> &list) {

    bool ok=true;
    int mid=chunk.find('-');
    if(mid!=-1) {
        int start=chunk.left(mid).toInt(&ok);
        int end=chunk.mid(mid+1).toInt(&ok);
        while(ok && start<=end)
            list.append(start++);
    }
    else
        list.append(chunk.toInt(&ok));
    return ok;
}

void Page::moveObject( int x, int y, bool key )
{
    KPObject *kpobject;
    QList<KPObject> _objects;
    QPainter p;

    _objects.setAutoDelete( false );

    if ( (int)objectList()->count() - 1 >= 0 ) {
        for ( int i = static_cast<int>( objectList()->count() ) - 1; i >= 0; i-- ) {
            kpobject = objectList()->at( i );
            if ( kpobject->isSelected() ) {
                p.begin( this );
                kpobject->setMove( true );
                kpobject->draw( &p, diffx(), diffy() );
                kpobject->moveBy( QPoint( x, y ) );
                kpobject->draw( &p, diffx(), diffy() );
                p.end();

                kpobject->setMove( false );
                _objects.append( kpobject );
                _repaint( QRect( kpobject->getBoundingRect( 0, 0 ).x() + ( -1*x ),
                                 kpobject->getBoundingRect( 0, 0 ).y() + ( -1*y ),
                                 kpobject->getBoundingRect( 0, 0 ).width(),
                                 kpobject->getBoundingRect( 0, 0 ).height() ) );
                _repaint( kpobject );
            }
        }
    }

    if ( key ) {
        MoveByCmd *moveByCmd = new MoveByCmd( i18n( "Move object(s)" ),
                                              QPoint( x, y ),
                                              _objects, view->kPresenterDoc() );
        view->kPresenterDoc()->commands()->addCommand( moveByCmd );
    }
}

void Page::resizeObject()
{
    KPObject *kpobject;

    kpobject = objectList()->at( resizeObjNum );
    kpobject->setMove( false );
    _repaint( oldBoundingRect );
    _repaint( kpobject );

    oldBoundingRect = kpobject->getBoundingRect( 0, 0 );
}

#include <page.moc>
