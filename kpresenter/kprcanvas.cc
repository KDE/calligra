// -*- Mode: c++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4; -*-
/* This file is part of the KDE project
   Copyright (C) 1998, 1999 Reginald Stadlbauer <reggie@kde.org>
   Copyright (C) 2002-2004 Thorsten Zachmann <zachmann@kde.org>

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

#include <stdlib.h>
#include <math.h>

#include <qprogressdialog.h>
#include <qfile.h>
#include <qtextstream.h>
#include <qpainter.h>
#include <qwmatrix.h>
#include <qapplication.h>
#include <qpopupmenu.h>
#include <qimage.h>
#include <qdatetime.h>
#include <qdropsite.h>
#include <qrect.h>
#include <qsize.h>
#include <qpoint.h>
#include <qclipboard.h>

#include <kapplication.h>
#include <kmimemagic.h>
#include <klocale.h>
#include <kiconloader.h>
#include <kprinter.h>
#include <kglobal.h>
#include <kglobalsettings.h>
#include <ktempfile.h>
#include <kdebug.h>
#include <kcursor.h>
#include <kmessagebox.h>
#include <kmultipledrag.h>
#include <kconfig.h>
#include <kurl.h>
#include <kurldrag.h>
#include <kio/netaccess.h>

#include <koparagcounter.h>
#include <koPoint.h>
#include <kozoomhandler.h>
#include <koStore.h>
#include <koStoreDrag.h>
#include <koPointArray.h>

#include "styledia.h"
#include "kpresenter_view.h"
#include "kpbackground.h"
#include "kppixmapobject.h"
#include "kpfreehandobject.h"
#include "kpcubicbeziercurveobject.h"
#include "kpquadricbeziercurveobject.h"
#include "gotopage.h"
#include "kptextobject.h"
#include "kpresenter_sound_player.h"
#include "notebar.h"
#include "kppartobject.h"
#include "kpresenter_utils.h"
#include "kppageeffects.h"
#include "kprcommand.h"
#include "kppolylineobject.h"
#include "kpclosedlineobject.h"
#include "kprpage.h"
#include "kprvariable.h"
#include "kpgroupobject.h"

#include "kprcanvas.h"
#include "kprcanvas.moc"


KPrCanvas::KPrCanvas( QWidget *parent, const char *name, KPresenterView *_view )
    : QWidget( parent, name, WStaticContents|WResizeNoErase|WRepaintNoErase ), buffer( size() )
{
    presMenu = 0;
    m_currentTextObjectView=0L;
    m_activePage=0L;
    m_xOffset = 0;
    m_yOffset = 0;

    m_tmpHorizHelpline = -1;
    m_tmpVertHelpline = -1;
    tmpHelpLinePosX = -1;
    tmpHelpLinePosY = -1;

    m_tmpHelpPoint = -1;
    tmpHelpPointPos = KoPoint( -1, -1);
    m_keyPressEvent = false;
    m_drawSymetricObject = false;
    if ( parent ) {
        showingLastSlide = false;
        mousePressed = false;
        drawContour = false;
        modType = MT_NONE;
        resizeObjNum = 0L;
        editNum = 0L;
        rotateNum = 0L;
        setBackgroundMode( Qt::NoBackground );
        m_view = _view;
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
        m_zoomRubberDraw = false;
        toolEditMode = TEM_MOUSE;
        tmpObjs.setAutoDelete( false );
        setAcceptDrops( true );
        inEffect = false;
        ratio = 0.0;
        keepRatio = false;
        mouseSelectedObject = false;
        selectedObjectPosition = -1;
        nextPageTimer = true;
        drawLineInDrawMode = false;
        soundPlayer = 0;
        m_drawPolyline = false;
        m_drawCubicBezierCurve = false;
        m_drawLineWithCubicBezierCurve = true;
        m_oldCubicBezierPointArray.putPoints( 0, 4, (double)0,(double)0, (double)0,(double)0,
                                              (double)0,(double)0, (double)0,(double)0 );
    } else {
        m_view = 0;
        hide();
    }

    setFocusPolicy( QWidget::StrongFocus );
    setFocus();
    setKeyCompression( true );
    installEventFilter( this );
    KCursor::setAutoHideCursor( this, true, true );
    m_zoomBeforePresentation=100;

    if( m_view)
    {
        m_activePage=m_view->kPresenterDoc()->pageList().getFirst();
        connect( m_view->kPresenterDoc(), SIGNAL( sig_terminateEditing( KPTextObject * ) ),
                 this, SLOT( terminateEditing( KPTextObject * ) ) );
    }
}

KPrCanvas::~KPrCanvas()
{
    // block all signals (save for destroyed()) to avoid crashes on exit
    // (exitEditMode) emits signals
    blockSignals(true);

    // deactivate possible opened textobject to avoid double deletion, KPTextObject deletes this already

    exitEditMode();

    delete presMenu;

    stopSound();
    delete soundPlayer;
    blockSignals(false);
}

void KPrCanvas::scrollX( int x )
{
    int oldXOffset = m_xOffset;
    m_xOffset = x;
    scroll( oldXOffset - x, 0 );
}

void KPrCanvas::scrollY( int y )
{
    int oldYOffset = m_yOffset;
    m_yOffset = y;
    scroll( 0, oldYOffset - y );
}

bool KPrCanvas::eventFilter( QObject *o, QEvent *e )
{

    if ( !o || !e )
        return TRUE;
    if ( m_currentTextObjectView  )
        KCursor::autoHideEventFilter( o, e );
    switch ( e->type() )
    {
    case QEvent::AccelOverride:
    {
        QKeyEvent * keyev = static_cast<QKeyEvent *>(e);
        if ( m_currentTextObjectView &&
             (keyev->key()==Key_Home ||keyev->key()==Key_End
              || keyev->key()==Key_Tab || keyev->key()==Key_Prior
              || keyev->key()==Key_Next) )
        {
            m_currentTextObjectView->keyPressEvent( keyev );
            return true;
        }
        else if ( !m_currentTextObjectView && keyev->key()==Key_Tab  )
        {
            keyPressEvent(keyev);
            return true;
        }
    }
    case QEvent::FocusIn:
        if ( m_currentTextObjectView )
            m_currentTextObjectView->focusInEvent();
        return TRUE;
    case QEvent::FocusOut:
        if ( m_currentTextObjectView  )
            m_currentTextObjectView->focusOutEvent();
        return TRUE;
    case QEvent::KeyPress:
    {
#ifndef NDEBUG
        QKeyEvent * keyev = static_cast<QKeyEvent *>(e);
        // Debug keys
        if ( ( keyev->state() & ControlButton ) && ( keyev->state() & ShiftButton ) )
        {
            switch ( keyev->key() ) {
            case Key_P: // 'P' -> paragraph debug
                printRTDebug( 0 );
                break;
            case Key_V: // 'V' -> verbose parag debug
                printRTDebug( 1 );
                break;
            default:
                break;
            }
        }
#endif
    }
    break;
    default:
        break;
    }
    return QWidget::eventFilter(o,e);
}

bool KPrCanvas::focusNextPrevChild( bool )
{
    return TRUE; // Don't allow to go out of the canvas widget by pressing "Tab"
}

void KPrCanvas::paintEvent( QPaintEvent* paintEvent )
{
    if ( isUpdatesEnabled() )
    {
        QPainter bufPainter;
        bufPainter.begin( &buffer, this ); // double-buffering - (the buffer is as big as the widget)
        bufPainter.translate( -diffx(), -diffy() );
        bufPainter.setBrushOrigin( -diffx(), -diffy() );

        QRect crect( paintEvent->rect() ); // the rectangle that needs to be repainted, in widget coordinates
        //kdDebug(33001) << "KPrCanvas::paintEvent " << DEBUGRECT( crect ) << endl;
        crect.moveBy( diffx(), diffy() ); // now in contents coordinates
        //kdDebug(33001) << "KPrCanvas::paintEvent after applying diffx/diffy: " << DEBUGRECT( crect ) << endl;

        if ( editMode || !fillBlack )
            bufPainter.fillRect( crect, white );
        else
            bufPainter.fillRect( crect, black );

        drawBackground( &bufPainter, crect );

        SelectionMode selectionMode;

        if ( toolEditMode == TEM_MOUSE || toolEditMode == TEM_ZOOM )
            selectionMode = SM_MOVERESIZE;
        else if ( toolEditMode == TEM_ROTATE )
            selectionMode = SM_ROTATE;
        else
            selectionMode = SM_NONE;

        KPresenterDoc *doc =m_view->kPresenterDoc();
        if ( editMode )
        {
            if ( doc->showGrid() && !doc->gridToFront())
                drawGrid( &bufPainter, crect);

            if ( doc->showHelplines() && !doc->helpLineToFront() && editMode)
            {
                drawHelplines( &bufPainter, crect);
                drawHelpPoints( &bufPainter, crect);
            }
        }

        if ( !editMode )
            selectionMode = SM_NONE; // case of screen presentation mode
        drawObjects( &bufPainter, crect, true, selectionMode, true );
        if ( editMode )
        {
            if ( doc->showGrid() && doc->gridToFront())
                drawGrid( &bufPainter, crect);

            if ( doc->showHelplines() && doc->helpLineToFront())
            {
                drawHelplines( &bufPainter, crect);
                drawHelpPoints( &bufPainter, crect);
            }

        }
        else
        {
            if ( drawMode && m_drawModeLines.count() )
            {
                bufPainter.save();
                bufPainter.setPen( m_view->kPresenterDoc()->presPen() );
                for ( int i = 0; i < m_drawModeLines.count(); ++i )
                {
                  bufPainter.drawPolyline( m_drawModeLines[i] );
                }
                bufPainter.restore();
            }
        }
        bufPainter.end();

        bitBlt( this, paintEvent->rect().topLeft(), &buffer, paintEvent->rect() );
    }
    //else kdDebug(33001) << "KPrCanvas::paintEvent with updates disabled" << endl;
}

void KPrCanvas::drawBackground( QPainter *painter, const QRect& rect ) const
{
    QRegion grayRegion( rect );
    if ( editMode )
    {
        //kdDebug(33001) << "KPrCanvas::drawBackground drawing bg for page " << i+1 << " editMode=" << editMode << endl;
        QRect pageRect = m_activePage->getZoomPageRect();
        //kdDebug(33001) << "KPrCanvas::drawBackground pageRect=" << DEBUGRECT(pageRect) << endl;
        //kdDebug(33001) << "KPrCanvas::drawBackground rect=" << DEBUGRECT(rect) << endl;
        if ( rect.intersects( pageRect ) )
            m_activePage->background()->draw( painter, m_view->zoomHandler(), rect, true );
        // Include the border now
        pageRect.rLeft() -= 1;
        pageRect.rTop() -= 1;
        pageRect.rRight() += 1;
        pageRect.rBottom() += 1;
        grayRegion -= pageRect;

        // In edit mode we also want to draw the gray area out of the pages
        if ( !grayRegion.isEmpty() )
            eraseEmptySpace( painter, grayRegion, QApplication::palette().active().brush( QColorGroup::Mid ) );
    }
    else
        // TODO use m_pageList
        m_view->kPresenterDoc()->pageList().at( currPresPage-1 )->background()->draw( painter, m_view->zoomHandler(),
                                                                                      rect, false );
}

// 100% stolen from KWord
void KPrCanvas::eraseEmptySpace( QPainter * painter, const QRegion & emptySpaceRegion, const QBrush & brush ) const
{
    painter->save();
    painter->setClipRegion( emptySpaceRegion, QPainter::CoordPainter );
    painter->setPen( Qt::NoPen );

    //kdDebug(33001) << "KWDocument::eraseEmptySpace emptySpaceRegion: " << DEBUGRECT( emptySpaceRegion.boundingRect() ) << endl;
    painter->fillRect( emptySpaceRegion.boundingRect(), brush );
    painter->restore();
}

// Draw all object in page : draw object in current page and sticky objects
void KPrCanvas::drawObjectsInPage(QPainter *painter, const KoRect& rect2, bool drawCursor,
                                  SelectionMode selectMode, bool doSpecificEffects,
                                  const QPtrList<KPObject> & obj) const
{
    QPtrListIterator<KPObject> it( obj );
    for ( ; it.current() ; ++it )
    {
        SelectionMode selectionMode=selectMode;
        if ( objectIsAHeaderFooterHidden(it.current()))
            continue;
        //don't draw rotate indicator when we are a header or footer
        if( m_view->kPresenterDoc()->isHeaderFooter(it.current()) || it.current()->isProtect())
            selectionMode=SM_PROTECT;

        if ( ( rect2.intersects( it.current()->getBoundingRect() ) && editMode )
             || ( !editMode && it.current()->getPresNum() <= static_cast<int>( currPresStep )
                  && ( !it.current()->getDisappear() || it.current()->getDisappear()
                       && it.current()->getDisappearNum() > static_cast<int>( currPresStep ) ) ) )

        {
            if ( inEffect && it.current()->getPresNum() >= static_cast<int>( currPresStep ) )
                continue;

            if ( !editMode && doSpecificEffects && !goingBack
                 && static_cast<int>( currPresStep ) == it.current()->getPresNum() )
            {
                //kdDebug(33001) << "                 setSubPresStep " << subPresStep << endl;
                it.current()->setSubPresStep( subPresStep );
                it.current()->doSpecificEffects( true, false );
            }
            //kdDebug(33001) << "                 drawing object at " << diffx() << "," << diffy() << "  and setting subpresstep to 0 !" << endl;
            if ( drawCursor && it.current()->getType() == OT_TEXT && m_currentTextObjectView )
            {
                KPTextObject* textObject = static_cast<KPTextObject*>( it.current() );
                if ( m_currentTextObjectView->kpTextObject() == textObject ) // This is the object we are editing
                    textObject->paintEdited( painter,m_view->zoomHandler(),
                                             false /*onlyChanged. Pass as param ?*/,
                                             m_currentTextObjectView->cursor(), true /* idem */ );
                else
                    it.current()->draw( painter, m_view->zoomHandler(), selectionMode,
                                        ((it.current())->isSelected()) && drawContour );
            }
            else
                it.current()->draw( painter, m_view->zoomHandler(), selectionMode,
                                    ((it.current())->isSelected()) && drawContour );
            it.current()->setSubPresStep( 0 );
            it.current()->doSpecificEffects( false );
        }
    }
}

void KPrCanvas::drawObjects( QPainter *painter, const QRect& rect, bool drawCursor,
                             SelectionMode selectionMode, bool doSpecificEffects ) const
{
    int pgNum = editMode ? (int)m_view->getCurrPgNum() : currPresPage;
    KoRect rect2 = m_view->zoomHandler()->unzoomRect(rect);

    //objects in current page
    drawObjectsInPage( painter, rect2, drawCursor, selectionMode, doSpecificEffects,
                       m_view->kPresenterDoc()->pageList().at(pgNum-1)->objectList());

    //draw sticky object
    drawObjectsInPage( painter, rect2, drawCursor, selectionMode, doSpecificEffects,
                       stickyPage()->objectList());
}

void KPrCanvas::drawGrid(QPainter *painter, const QRect &rect2) const
{
    KPresenterDoc *doc=m_view->kPresenterDoc();

    if(!doc->isReadWrite())
        return;
    QPen _pen = QPen( doc->gridColor(), 6, Qt::DotLine );
    painter->save();
    painter->setPen( _pen );
    QRect pageRect=activePage()->getZoomPageRect();

    int zoomedX,  zoomedY;
    double offsetX = doc->getGridX();
    double offsetY = doc->getGridY();

    for ( double i = offsetX; ( zoomedX = m_view->zoomHandler()->zoomItX( i )+pageRect.left() ) < pageRect.right(); i += offsetX )
        for ( double j = offsetY; ( zoomedY = m_view->zoomHandler()->zoomItY( j ) +pageRect.top()) < pageRect.bottom(); j += offsetY )
            if( rect2.contains(  zoomedX, zoomedY ) )
                painter->drawPoint( zoomedX, zoomedY );

    painter->restore();
}

void KPrCanvas::drawHelpPoints( QPainter *painter, const QRect &rect2) const
{
    KPresenterDoc *doc=m_view->kPresenterDoc();
    if(!doc->isReadWrite())
        return;
    KoRect rect = m_view->zoomHandler()->unzoomRect(rect2);

    QValueList<KoPoint>::Iterator i;
    QPen _pen = QPen( Qt::black, 1, Qt::DotLine );
    painter->save();
    painter->setPen( _pen );

    for(i = doc->helpPoints().begin(); i != doc->helpPoints().end(); ++i)
    {
        KoPoint vi = *i ;
        if( rect.contains( vi ) )
        {
            QPoint point=m_view->zoomHandler()->zoomPoint( vi);
            painter->drawLine( point.x(), point.y()-20, point.x(), point.y()+20);
            painter->drawLine( point.x()-20, point.y(), point.x()+20, point.y());
        }
    }
    painter->restore();
}

void KPrCanvas::drawHelplines(QPainter *painter, const QRect &rect2) const
{
    KPresenterDoc *doc=m_view->kPresenterDoc();

    if(!doc->isReadWrite())
        return;
    KoRect rect = m_view->zoomHandler()->unzoomRect(rect2);
    QValueList<double>::ConstIterator i;
    QPen _pen = QPen( Qt::black, 1, Qt::DotLine );
    painter->save();
    painter->setPen( _pen );
    QRect pageRect=activePage()->getZoomPageRect();
    for(i = doc->horizHelplines().begin(); i != doc->horizHelplines().end(); ++i)
    {
        double vi = *i ;
        if( rect.contains(rect.x(), vi) )
            painter->drawLine(pageRect.left(), m_view->zoomHandler()->zoomItY(vi), pageRect.right(), m_view->zoomHandler()->zoomItY(vi));
    }

    for(i = doc->vertHelplines().begin(); i != doc->vertHelplines().end(); ++i)
    {
        double vi = *i ;
        if( rect.contains(vi, rect.y()) )
            painter->drawLine(m_view->zoomHandler()->zoomItX(vi), pageRect.top(), m_view->zoomHandler()->zoomItX(vi), pageRect.bottom());
    }
    painter->restore();
}


// This one is used to generate the pixmaps for the HTML presentation,
// for the pres-structure-dialog, for the sidebar previews, for template icons.
void KPrCanvas::drawAllObjectsInPage( QPainter *painter, const QPtrList<KPObject> & obj ) const
{
    QPtrListIterator<KPObject> it( obj );
    for ( ; it.current(); ++it ) {
        if ( objectIsAHeaderFooterHidden( it.current() ) )
            continue;
        it.current()->draw( painter, m_view->zoomHandler(), SM_NONE, false );
    }
}

QRect KPrCanvas::getOldBoundingRect( const KPObject *obj )
{
    KoRect oldKoBoundingRect = obj->getBoundingRect();
    double _dx = oldKoBoundingRect.x() - 5.0;
    double _dy = oldKoBoundingRect.y() - 5.0;
    double _dw = oldKoBoundingRect.width() + 10.0;
    double _dh = oldKoBoundingRect.height() + 10.0;
    oldKoBoundingRect.setRect( _dx, _dy, _dw, _dh );
    oldBoundingRect = m_view->zoomHandler()->zoomRect( oldKoBoundingRect );
    return oldBoundingRect;
}

void KPrCanvas::mousePressEvent( QMouseEvent *e )
{
    if(!m_view->koDocument()->isReadWrite())
        return;
    moveStartPosMouse = objectSelectedBoundingRect().topLeft();
    QPoint contentsPoint( e->pos().x()+diffx(), e->pos().y()+diffy() );
    KoPoint docPoint = m_view->zoomHandler()->unzoomPoint( contentsPoint );
    if(m_currentTextObjectView)
    {
        KPTextObject *txtObj=m_currentTextObjectView->kpTextObject();
        Q_ASSERT(txtObj);
        if(txtObj->contains( docPoint ))
        {
            KoPoint pos = docPoint - txtObj->innerRect().topLeft(); // in pt, but now translated into the object's coordinate system
            mousePressed=true;
            if(e->button() == RightButton)
            {
                m_currentTextObjectView->showPopup( m_view, QCursor::pos(), m_view->actionList() );
                mousePressed=false;
            }
            else if( e->button() == MidButton )
            {
                QApplication::clipboard()->setSelectionMode( true );
                m_currentTextObjectView->paste();
                QApplication::clipboard()->setSelectionMode( false );
            }
            else
                m_currentTextObjectView->mousePressEvent(e, m_view->zoomHandler()->ptToLayoutUnitPix( pos ) ); // in LU pixels
            return;
        }
    }


    if ( e->state() & ControlButton )
        keepRatio = true;

    KPObject *kpobject = 0;

    m_savedMousePos = contentsPoint;

    QPoint rasterPoint=applyGrid( e->pos(), true );

    resizeObjNum = 0L;

    exitEditMode();

    if ( editMode ) {
        if ( e->button() == LeftButton ) {
            mousePressed = true;

            if ( m_drawPolyline && ( toolEditMode == INS_POLYLINE || toolEditMode == INS_CLOSED_POLYLINE ) ) {
                m_dragStartPoint = rasterPoint;
                m_pointArray.putPoints( m_indexPointArray, 1,m_view->zoomHandler()->unzoomItX( m_dragStartPoint.x()),
                                        m_view->zoomHandler()->unzoomItY(m_dragStartPoint.y()) );
                ++m_indexPointArray;
                return;
            }

            if ( m_drawCubicBezierCurve && ( toolEditMode == INS_CUBICBEZIERCURVE
                                             || toolEditMode == INS_QUADRICBEZIERCURVE
                                             || toolEditMode == INS_CLOSED_CUBICBEZIERCURVE
                                             || toolEditMode == INS_CLOSED_QUADRICBEZIERCURVE ) ) {
                if ( m_drawLineWithCubicBezierCurve ) {
                    QPainter p( this );
                    p.setPen( QPen( Qt::black, 1, Qt::SolidLine ) );
                    p.setBrush( Qt::NoBrush );
                    p.setRasterOp( Qt::NotROP );

                    QPoint oldStartPoint = m_dragStartPoint;

                    m_dragStartPoint = rasterPoint;

                    p.drawLine( oldStartPoint, m_dragStartPoint );  // erase old line
                    p.end();

                    m_pointArray.putPoints( m_indexPointArray, 1, m_view->zoomHandler()->unzoomItX( m_dragStartPoint.x()),
                                            m_view->zoomHandler()->unzoomItY( m_dragStartPoint.y()) );
                    ++m_indexPointArray;
                    m_drawLineWithCubicBezierCurve = false;
                }
                else {
                    QPoint _oldEndPoint = rasterPoint;
                    QPainter p( this );
                    QPen _pen = QPen( Qt::black, 1, Qt::DashLine );
                    p.setPen( _pen );
                    p.setBrush( Qt::NoBrush );
                    p.setRasterOp( Qt::NotROP );

                    p.save();
                    double _angle = KoPoint::getAngle( _oldEndPoint, m_dragStartPoint );
                    //FIXME
                    drawFigure( L_SQUARE, &p, m_view->zoomHandler()->unzoomPoint( _oldEndPoint ),
                                _pen.color(), _pen.width(), _angle,m_view->zoomHandler()  ); // erase old figure
                    p.restore();

                    p.drawLine( m_dragStartPoint, _oldEndPoint ); // erase old line

                    int p_x = m_dragStartPoint.x() * 2 - _oldEndPoint.x();
                    int p_y = m_dragStartPoint.y() * 2 - _oldEndPoint.y();
                    QPoint _oldSymmetricEndPoint = QPoint( p_x, p_y );

                    p.save();
                    _angle = KoPoint::getAngle( _oldSymmetricEndPoint, m_dragStartPoint );
                    drawFigure( L_SQUARE, &p, m_view->zoomHandler()->unzoomPoint( _oldSymmetricEndPoint ),
                                _pen.color(), _pen.width(), _angle,m_view->zoomHandler() );  // erase old figure
                    p.restore();

                    p.drawLine( m_dragStartPoint, _oldSymmetricEndPoint );  // erase old line

                    m_pointArray.putPoints( m_indexPointArray, 3, m_CubicBezierSecondPoint.x(), m_CubicBezierSecondPoint.y(),
                                            m_CubicBezierThirdPoint.x(), m_CubicBezierThirdPoint.y(),
                                            m_view->zoomHandler()->unzoomItX(m_dragStartPoint.x()),
                                            m_view->zoomHandler()->unzoomItY(m_dragStartPoint.y()) );
                    m_indexPointArray += 3;
                    m_drawLineWithCubicBezierCurve = true;
                    m_oldCubicBezierPointArray = KoPointArray();
                    m_oldCubicBezierPointArray.putPoints( 0, 4, (double)0,(double)0, (double)0,(double)0,
                                                          (double)0,(double)0, (double)0,(double)0 );
                    m_dragEndPoint = m_dragStartPoint;
                }

                return;
            }

            switch ( toolEditMode ) {
            case TEM_MOUSE: {
                bool overObject = false;
                bool deSelAll = true;
                bool _resizeObj = false;
                KPObject *kpobject = 0;

                firstX = contentsPoint.x();
                firstY = contentsPoint.y();
                kpobject = m_activePage->getObjectResized( docPoint, modType, deSelAll, overObject, _resizeObj );
                if ( kpobject ) {
                    if(_resizeObj)
                    {
                        oldBoundingRect = getOldBoundingRect( kpobject );
                        resizeObjNum = kpobject;
                    }
                }
                else
                {
                    _resizeObj = false;
                    kpobject = stickyPage()->getObjectResized( docPoint, modType, deSelAll, overObject, _resizeObj );
                    if( kpobject && m_view->kPresenterDoc()->isHeaderFooter(kpobject))
                    {
                        if(objectIsAHeaderFooterHidden(kpobject))
                        {
                            kpobject=0L;
                            overObject=false;
                        }
                    }
                    if( kpobject && _resizeObj ) {
                        oldBoundingRect = getOldBoundingRect( kpobject );
                        resizeObjNum = kpobject;
                    }
                }
                if ( resizeObjNum )
                    keepRatio = keepRatio || resizeObjNum->isKeepRatio();

                if ( deSelAll && !( e->state() & ShiftButton ) && !( e->state() & ControlButton ) )
                    deSelectAllObj();

                if ( overObject && kpobject) {
                    if ( !(e->state() & ShiftButton)) {
                        selectObj( kpobject );
                        raiseObject( kpobject );
                        moveStartPosMouse = objectSelectedBoundingRect().topLeft();
                    }
                    else
                        deSelectObj( kpobject );
                }
                else {
                    modType = MT_NONE;
                    if( editMode && m_view->kPresenterDoc()->showHelplines())
                    {
                        m_tmpHorizHelpline = m_view->kPresenterDoc()->indexOfHorizHelpline(m_view->zoomHandler()->unzoomItY(e->pos().y()+diffy()));
                        m_tmpVertHelpline = m_view->kPresenterDoc()->indexOfVertHelpline(m_view->zoomHandler()->unzoomItX(e->pos().x()+diffx()));
                        m_tmpHelpPoint = m_view->kPresenterDoc()->indexOfHelpPoint(
                            KoPoint(m_view->zoomHandler()->unzoomItX(e->pos().x()+diffx()), m_view->zoomHandler()->unzoomItX(e->pos().y()+diffy())) );
                        tmpHelpLinePosX=m_view->zoomHandler()->unzoomItX(e->pos().x());
                        tmpHelpLinePosY=m_view->zoomHandler()->unzoomItY(e->pos().y());
                        tmpHelpPointPos=m_view->zoomHandler()->unzoomPoint( e->pos());

                    }
                    else
                    {
                        m_tmpVertHelpline = -1;
                        m_tmpHorizHelpline = -1;
                        m_tmpHelpPoint = -1;
                    }
                    if ( !( e->state() & ShiftButton ) && !( e->state() & ControlButton ) )
                        deSelectAllObj();
                    if (m_tmpHorizHelpline == -1 && m_tmpVertHelpline ==-1 && m_tmpHelpPoint == -1)
                    {
                        drawRubber = true;
                        rubber = QRect( e->x(), e->y(), 0, 0 );
                        tmpHelpLinePosX=-1;
                        tmpHelpLinePosY=-1;
                        tmpHelpPointPos=KoPoint( -1, -1 );
                    }
                }

                // update hotspot
                calcBoundingRect();
                m_hotSpot = docPoint - m_boundingRect.topLeft();
                m_origPos = QPoint(e->x() + diffx(), e->y() + diffy());
            } break;
            case TEM_ZOOM: {
                modType = MT_NONE;
                drawRubber = true;
                m_zoomRubberDraw = false;
                rubber = QRect( e->x(), e->y(), 0, 0 );
            }break;
            case TEM_ROTATE: {
                //bool deSelAll = true;
                //bool _resizeObj = false;
                KPObject *kpobject = 0;

                firstX = contentsPoint.x();
                firstY = contentsPoint.y();

                // find object on active page
                kpobject = m_activePage->getObjectAt( docPoint );

                // find object on sticky page (ignore header/footer)
                if ( !kpobject ) {
                    kpobject = stickyPage()->getObjectAt( docPoint );
                    if( kpobject && m_view->kPresenterDoc()->isHeaderFooter(kpobject))
                        if(objectIsAHeaderFooterHidden(kpobject))
                            kpobject=0L;
                }

                // clear old selections even if shift or control are pressed
                // we don't support rotating multiple objects yet
                deSelectAllObj();

                // deselect all if no object is found
                if ( !kpobject )
                    deSelectAllObj();

                // select and raise object
                else {
                    rotateNum = kpobject;
                    startAngle = -kpobject->getAngle();
                    selectObj( kpobject );
                    raiseObject( kpobject );
                }

                // set axis to center of selected objects bounding rect
                if ( kpobject ) {
                    calcBoundingRect();
                    axisX = m_boundingRect.center().x();
                    axisY = m_boundingRect.center().y();
                }
            } break;
            case INS_FREEHAND: case INS_CLOSED_FREEHAND: {
                deSelectAllObj();
                mousePressed = true;
                QPoint tmp = applyGrid ( e->pos(),true );
                insRect = QRect( tmp.x(),tmp.y(), 0, 0 );

                m_indexPointArray = 0;
                m_dragStartPoint = tmp;
                m_dragEndPoint = m_dragStartPoint;
                m_pointArray.putPoints( m_indexPointArray, 1, m_view->zoomHandler()->unzoomItX(m_dragStartPoint.x()),
                                        m_view->zoomHandler()->unzoomItY(m_dragStartPoint.y()) );
                ++m_indexPointArray;
            } break;
            case INS_POLYLINE: case INS_CLOSED_POLYLINE: {
                deSelectAllObj();
                mousePressed = true;
                QPoint tmp = applyGrid ( e->pos(),true );

                insRect = QRect( tmp.x(),tmp.y(), 0, 0 );

                m_drawPolyline = true;
                m_indexPointArray = 0;
                m_dragStartPoint = tmp;
                m_dragEndPoint = m_dragStartPoint;
                m_pointArray.putPoints( m_indexPointArray, 1, m_view->zoomHandler()->unzoomItX(m_dragStartPoint.x()),
                                        m_view->zoomHandler()->unzoomItY(m_dragStartPoint.y()) );
                ++m_indexPointArray;
            } break;
            case INS_CUBICBEZIERCURVE: case INS_QUADRICBEZIERCURVE:
            case INS_CLOSED_CUBICBEZIERCURVE: case INS_CLOSED_QUADRICBEZIERCURVE: {
                deSelectAllObj();
                mousePressed = true;
                QPoint tmp = applyGrid ( e->pos(),true );

                insRect = QRect( tmp.x(), tmp.y(), 0, 0 );

                m_drawCubicBezierCurve = true;
                m_drawLineWithCubicBezierCurve = true;
                m_indexPointArray = 0;
                m_oldCubicBezierPointArray.putPoints( 0, 4, (double)0,(double)0, (double)0,(double)0,
                                                      (double)0,(double)0, (double)0,(double)0 );
                m_dragStartPoint = tmp;
                m_dragEndPoint = m_dragStartPoint;
                m_pointArray.putPoints( m_indexPointArray, 1, m_view->zoomHandler()->unzoomItX(m_dragStartPoint.x()),
                                        m_view->zoomHandler()->unzoomItY(m_dragStartPoint.y() ));
                ++m_indexPointArray;
            } break;
            case INS_POLYGON: {
                deSelectAllObj();
                mousePressed = true;
                QPoint tmp = applyGrid ( e->pos(),true );
                insRect = QRect( tmp.x(), tmp.y(), 0, 0 );

                m_indexPointArray = 0;
                m_dragStartPoint = tmp;
                m_dragEndPoint = m_dragStartPoint;
            } break;
            default: {
                deSelectAllObj();
                mousePressed = true;
                QPoint tmp = applyGrid ( e->pos(),true );
                insRect = QRect( tmp.x(), tmp.y(), 0, 0 );
            } break;
            }
        }

        if ( e->button() == RightButton && ( toolEditMode == INS_POLYLINE || toolEditMode == INS_CLOSED_POLYLINE )
             && !m_pointArray.isNull() && m_drawPolyline ) {
            if( m_indexPointArray > 1)
            {
                QPainter p( this );
                p.setPen( QPen( black, 1, SolidLine ) );
                p.setBrush( NoBrush );
                p.save();
                p.setRasterOp( NotROP );
                p.drawLine( m_dragStartPoint, m_dragEndPoint ); //

                p.drawLine( m_dragStartPoint, m_view->zoomHandler()->zoomPoint( m_pointArray.at(m_indexPointArray - 2)) );
                p.restore();

                m_indexPointArray= QMAX(1,m_indexPointArray-1);
                m_pointArray.resize(m_indexPointArray);
                m_dragStartPoint=m_view->zoomHandler()->zoomPoint( m_pointArray.at(m_indexPointArray - 1));

                p.drawLine( m_dragStartPoint, m_dragEndPoint );

                p.end();
            }
            else if( m_indexPointArray == 1)
            {
                QPainter p( this );
                p.setPen( QPen( black, 1, SolidLine ) );
                p.setBrush( NoBrush );
                p.setRasterOp( NotROP );
                p.drawLine( m_dragStartPoint, m_dragEndPoint ); //
                p.end();
                m_pointArray = KoPointArray();
                m_indexPointArray = 0;

                m_drawPolyline = false;
                mousePressed = false;
            }
            return;
        }

        if ( e->button() == RightButton && ( toolEditMode == INS_CUBICBEZIERCURVE || toolEditMode == INS_QUADRICBEZIERCURVE
                                             || toolEditMode == INS_CLOSED_CUBICBEZIERCURVE || toolEditMode == INS_CLOSED_QUADRICBEZIERCURVE )
             && !m_pointArray.isNull() && m_drawCubicBezierCurve ) {
            if ( m_drawLineWithCubicBezierCurve ) {
                QPoint point = applyGrid( e->pos(), true);
                m_pointArray.putPoints( m_indexPointArray, 1, m_view->zoomHandler()->unzoomItX(point.x()), m_view->zoomHandler()->unzoomItY(point.y()) );
                ++m_indexPointArray;
            }
            else {
                m_pointArray.putPoints( m_indexPointArray, 2, m_CubicBezierSecondPoint.x(), m_CubicBezierSecondPoint.y(),
                                        m_CubicBezierThirdPoint.x(), m_CubicBezierThirdPoint.y() );
                m_indexPointArray += 2;
            }

            if ( toolEditMode == INS_CUBICBEZIERCURVE || toolEditMode == INS_QUADRICBEZIERCURVE
                 || toolEditMode == INS_CLOSED_CUBICBEZIERCURVE || toolEditMode == INS_CLOSED_QUADRICBEZIERCURVE )
                endDrawCubicBezierCurve();

            mouseMoveEvent( e );

            return;
        }

        if ( e->button() == RightButton && toolEditMode == TEM_MOUSE ) {
            KPObject*obj = getObjectAt( docPoint );
            if(objectIsAHeaderFooterHidden(obj))
                obj=0L;
            if ( obj ) {
                kpobject = obj;
                QPoint pnt = QCursor::pos();
                mousePressed = false;
                bool state=!( e->state() & ShiftButton ) && !( e->state() & ControlButton ) && !kpobject->isSelected();
                ObjType objectType = kpobject->getType();

                if ( objectType == OT_PICTURE || objectType == OT_CLIPART ) {
                    deSelectAllObj();
                    selectObj( kpobject );
                    m_view->openPopupMenuObject( "picmenu_popup", pnt );
                } else if ( objectType == OT_TEXT ) {
                    if ( state )
                        deSelectAllObj();
                    KPTextObject *obj=dynamic_cast<KPTextObject *>(kpobject);
                    selectObj( kpobject );
                    if ( obj )
                        m_view->changeVerticalAlignmentStatus( obj->verticalAlignment() );
                    m_view->openPopupMenuObject( "textobject_popup", pnt );

                } else if ( objectType == OT_PIE ) {
                    if ( state )
                        deSelectAllObj();
                    selectObj( kpobject );
                    m_view->openPopupMenuObject( "piemenu_popup", pnt );
                } else if ( objectType == OT_RECT || objectType == OT_ELLIPSE ) {
                    if ( state )
                        deSelectAllObj();
                    selectObj( kpobject );
                    m_view->openPopupMenuObject( "rectangleobject_popup", pnt );
                } else if ( objectType == OT_PART ) {
                    if ( state )
                        deSelectAllObj();
                    selectObj( kpobject );
                    m_view->openPopupMenuObject( "partobject_popup", pnt );
                } else if ( objectType == OT_POLYGON ) {
                    if ( state )
                        deSelectAllObj();
                    selectObj( kpobject );
                    m_view->openPopupMenuObject( "polygonobject_popup", pnt );
                } else if ( objectType == OT_POLYLINE ) {
                    if ( state )
                        deSelectAllObj();
                    selectObj( kpobject );
                    KPPolylineObject *tmpObj=dynamic_cast<KPPolylineObject *>(kpobject);
                    if ( tmpObj )
                    {
                        if (!tmpObj->isClosed())
                            m_view->openPopupMenuObject( "closed_popup", pnt );
                        else
                            m_view->openPopupMenuObject( "flip_popup", pnt );
                    }
                } else if ( objectType == OT_CUBICBEZIERCURVE ) {
                    if ( state )
                        deSelectAllObj();
                    selectObj( kpobject );
                    KPCubicBezierCurveObject *tmpObj=dynamic_cast<KPCubicBezierCurveObject *>(kpobject);
                    if ( tmpObj )
                    {
                        if (!tmpObj->isClosed())
                            m_view->openPopupMenuObject( "closed_popup", pnt );
                        else
                            m_view->openPopupMenuObject( "flip_popup", pnt );
                    }
                } else if ( objectType == OT_QUADRICBEZIERCURVE ) {
                    if ( state )
                        deSelectAllObj();
                    selectObj( kpobject );
                    KPQuadricBezierCurveObject *tmpObj=dynamic_cast<KPQuadricBezierCurveObject *>(kpobject);
                    if ( tmpObj )
                    {
                        if (!tmpObj->isClosed())
                            m_view->openPopupMenuObject( "closed_popup", pnt );
                        else
                            m_view->openPopupMenuObject( "flip_popup", pnt );
                    }
                } else if ( objectType == OT_FREEHAND ) {
                    if ( state )
                        deSelectAllObj();
                    selectObj( kpobject );
                    KPFreehandObject *tmpObj=dynamic_cast<KPFreehandObject *>(kpobject);
                    if ( tmpObj )
                    {
                        if (!tmpObj->isClosed())
                            m_view->openPopupMenuObject( "closed_popup", pnt );
                        else
                            m_view->openPopupMenuObject( "flip_popup", pnt );
                    }
                } else if ( objectType == OT_LINE ){
                    if ( state )
                        deSelectAllObj();
                    selectObj( kpobject );
                    m_view->openPopupMenuObject( "flip_popup", pnt );
                } else if ( objectType == OT_CLOSED_LINE ) {
                    if ( state )
                        deSelectAllObj();
                    selectObj( kpobject );
                    KPClosedLineObject *tmpObj=dynamic_cast<KPClosedLineObject *>(kpobject);
                    if ( tmpObj )
                        m_view->openPopupMenuObject( "flip_popup", pnt );
                } else if ( objectType == OT_GROUP ) {
                    KPGroupObject *obj=dynamic_cast<KPGroupObject *>(kpobject);
                    if ( state )
                        deSelectAllObj();
                    selectObj( kpobject );
                    if ( obj && oneObjectTextSelected())
                        m_view->openPopupMenuObject( "textobject_popup", pnt );
                    else
                        m_view->openPopupMenuObject( "flip_popup", pnt );

                } else {
                    if ( state )
                        deSelectAllObj();
                    selectObj( kpobject );
                    m_view->openPopupMenuObject( "graphmenu_popup", pnt );
                }
            } else {
                QPoint pnt = QCursor::pos();
                if( m_view->kPresenterDoc()->showHelplines())
                {
                    m_tmpHorizHelpline = m_view->kPresenterDoc()->indexOfHorizHelpline(m_view->zoomHandler()->unzoomItY(e->pos().y()+diffy()));
                    m_tmpVertHelpline = m_view->kPresenterDoc()->indexOfVertHelpline(m_view->zoomHandler()->unzoomItX(e->pos().x()+diffx()));
                    m_tmpHelpPoint = m_view->kPresenterDoc()->indexOfHelpPoint(
                        KoPoint(m_view->zoomHandler()->unzoomItX(e->pos().x()+diffx()), m_view->zoomHandler()->unzoomItX(e->pos().y()+diffy())) );

                    if (m_tmpVertHelpline != -1 || m_tmpHorizHelpline != -1)
                    {
                        m_view->openPopupMenuHelpLine( pnt );
                    }
                    else if( m_tmpHelpPoint != -1 )
                        m_view->openPopupMenuHelpPoint( pnt );
                    else
                        m_view->openPopupMenuMenuPage( pnt );
                }
                else
                    m_view->openPopupMenuMenuPage( pnt );
                mousePressed = false;
            }
            modType = MT_NONE;

        }
        else if( e->button() == RightButton && toolEditMode == TEM_ZOOM ) {
            QPoint pnt = QCursor::pos();
            mousePressed = false;
            m_view->openPopupMenuZoom( pnt );
        }
        else if( e->button() == RightButton && toolEditMode != TEM_MOUSE ) {
            //deactivate tools when you click on right button
            setToolEditMode( TEM_MOUSE );
        }
    } else {
        if ( e->button() == LeftButton ) {
            if ( presMenu->isVisible() ) {
                presMenu->hide();
                setCursor( blankCursor );
            } else {
                if ( drawMode ) {
                    setCursor( KPresenterUtils::penCursor() );
                    drawLineInDrawMode = true;
                    m_drawModeLineIndex = 0;
                    m_drawModeLines.append( QPointArray() );
                    m_drawModeLines[m_drawModeLines.count() - 1].putPoints( m_drawModeLineIndex++, 1, e->x(), e->y() );
                }
                else
                    m_view->screenNext();
            }
        } else if ( e->button() == MidButton )
            m_view->screenPrev();
        else if ( e->button() == RightButton ) {
            if ( !drawMode && !spManualSwitch() )
                m_view->autoScreenPresStopTimer();

            setCursor( arrowCursor );
            QPoint pnt = QCursor::pos();
            presMenu->popup( pnt );
        }
    }
#if 0 // Where do you need this ? (toshitaka)
    // ME: I have no idea why this is needed at all
    if ( toolEditMode == TEM_MOUSE )
        mouseMoveEvent( e );
#endif
    if ( modType != MT_NONE && modType != MT_MOVE ) {
        KPObject *kpobject=resizeObjNum;
        if ( kpobject ) {
            ratio = static_cast<double>( kpobject->getSize().width() ) /
                    static_cast<double>( kpobject->getSize().height() );
            resizeRect = kpobject->getRect();
        }
    }
}

void KPrCanvas::calcBoundingRect()
{
    m_boundingRect = KoRect();

    m_boundingRect=m_activePage->getBoundingRect(m_boundingRect);
    m_boundingRect=stickyPage()->getBoundingRect(m_boundingRect);
    m_origBRect = m_boundingRect;
}

KoRect KPrCanvas::objectSelectedBoundingRect() const
{
    KoRect objBoundingRect=KoRect();

    objBoundingRect = m_activePage->getBoundingRect( objBoundingRect);
    objBoundingRect = stickyPage()->getBoundingRect( objBoundingRect);
    return objBoundingRect;
}

KoRect KPrCanvas::getAlignBoundingRect() const
{
    KoRect boundingRect;

    for ( int i = 0; i < 2; ++i )
    {
        QPtrListIterator<KPObject> it( i == 0 ? activePage()->objectList() : stickyPage()->objectList() );
        for ( ; it.current() ; ++it )
        {
            if ( it.current() == m_view->kPresenterDoc()->header() ||
                 it.current() == m_view->kPresenterDoc()->footer() )
                continue;

            if( it.current()->isSelected() && !it.current()->isProtect() ) {
                boundingRect |= it.current()->getRealRect();
            }
        }
    }
    return boundingRect;
}

void KPrCanvas::mouseReleaseEvent( QMouseEvent *e )
{
    QPoint contentsPoint( e->pos().x()+diffx(), e->pos().y()+diffy() );
    if(m_currentTextObjectView)
    {
        m_currentTextObjectView->mouseReleaseEvent( e, contentsPoint );
        emit objectSelectedChanged();
        mousePressed=false;
        return;
    }

    if ( e->button() != LeftButton ) {
        ratio = 0.0;
        keepRatio = false;
        return;
    }

    if ( drawMode ) {
        drawLineInDrawMode = false;
        m_drawModeLines[m_drawModeLines.count() - 1].putPoints( m_drawModeLineIndex++, 1, contentsPoint.x(), contentsPoint.y() );
        return;
    }
    bool state = m_view->kPresenterDoc()->snapToGrid();
    int mx = state ? applyGridOnPosX( contentsPoint.x()) : contentsPoint.x();
    int my = state ? applyGridOnPosY( contentsPoint.y()) : contentsPoint.y();
    firstX = state ? applyGridOnPosX( firstX) : firstX;
    firstY = state ? applyGridOnPosY( firstY) : firstY;
    QPtrList<KPObject> _objects;
    _objects.setAutoDelete( false );
    KPObject *kpobject = 0;

    if ( ( m_drawPolyline && ( toolEditMode == INS_POLYLINE || toolEditMode == INS_CLOSED_POLYLINE ) )
         || ( m_drawCubicBezierCurve && ( toolEditMode == INS_CUBICBEZIERCURVE || toolEditMode == INS_QUADRICBEZIERCURVE
                                          || toolEditMode == INS_CLOSED_CUBICBEZIERCURVE || toolEditMode == INS_CLOSED_QUADRICBEZIERCURVE ) ) ) {
        return;
    }

    if ( toolEditMode != INS_LINE )
        insRect = insRect.normalize();

    KoPoint mv;
    KoSize sz;
    if ( toolEditMode == TEM_MOUSE && modType != MT_NONE && modType != MT_MOVE  && resizeObjNum ) {
        kpobject = resizeObjNum;
        if ( kpobject ) {
            mv = KoPoint( kpobject->getOrig().x() - resizeRect.x(),
                          kpobject->getOrig().y() - resizeRect.y() );
            sz = KoSize( kpobject->getSize().width() - resizeRect.width(),
                         kpobject->getSize().height() - resizeRect.height() );
        }
        kpobject = 0L;
    }

    switch ( toolEditMode ) {
    case TEM_MOUSE: {
        drawContour = FALSE;
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
                rubber.moveBy(diffx(),diffy());

                QPtrListIterator<KPObject> it( getObjectList() );
                for ( ; it.current() ; ++it )
                {
                    if ( it.current()->intersects( m_view->zoomHandler()->unzoomRect(rubber) ) )
                        selectObj( it.current() );
                }

                QPtrListIterator<KPObject> sIt(stickyPage()->objectList() );
                for ( ; sIt.current() ; ++sIt )
                {
                    if ( sIt.current()->intersects( m_view->zoomHandler()->unzoomRect(rubber) ) )
                    {
                        if( m_view->kPresenterDoc()->isHeaderFooter(sIt.current()))
                        {
                            if( objectIsAHeaderFooterHidden(sIt.current()))
                                continue;
                        }
                        selectObj( sIt.current() );
                    }
                }


            }
            if ( m_tmpVertHelpline != -1 || m_tmpHorizHelpline != -1)
                moveHelpLine( e->pos() );
            else if ( m_tmpHelpPoint != -1 )
                moveHelpPoint( e->pos() );
        } break;
        case MT_MOVE: {
            if ( firstX != mx || firstY != my ) {
                KMacroCommand *macro=0L;
                KoPoint move( objectSelectedBoundingRect().topLeft() - moveStartPosMouse );
                KCommand *cmd=m_activePage->moveObject(m_view, move.x(), move.y());
                if(cmd)
                {
                    if ( !macro )
                        macro=new KMacroCommand(i18n("Move Objects"));
                    macro->addCommand(cmd);
                }
                cmd=stickyPage()->moveObject(m_view, move.x(), move.y());
                if(cmd)
                {
                    if ( !macro )
                        macro=new KMacroCommand(i18n("Move Objects"));

                    macro->addCommand(cmd);
                }
                if(macro)
                    m_view->kPresenterDoc()->addCommand(macro );
            } else
            {
                stickyPage()->repaintObj();
                m_activePage->repaintObj();
            }
        }
            break;
        case MT_RESIZE_UP: {
            if ( !resizeObjNum ) break;
            kpobject = resizeObjNum;
            if ( firstX != mx || firstY != my ) {
                ResizeCmd *resizeCmd = new ResizeCmd( i18n( "Resize Object Up" ), mv, sz,
                                                      kpobject, m_view->kPresenterDoc() );
                resizeCmd->unexecute( false );
                resizeCmd->execute();
                m_view->kPresenterDoc()->addCommand( resizeCmd );
            }
            _repaint( oldBoundingRect );
            m_view->kPresenterDoc()->layout(kpobject);
            _repaint( kpobject );
        } break;
        case MT_RESIZE_DN: {
            if ( !resizeObjNum ) break;
            kpobject =  resizeObjNum;
            if ( firstX != mx || firstY != my ) {
                ResizeCmd *resizeCmd = new ResizeCmd( i18n( "Resize Object Down" ), mv, sz,
                                                      kpobject, m_view->kPresenterDoc() );
                resizeCmd->unexecute( false );
                resizeCmd->execute();
                m_view->kPresenterDoc()->addCommand( resizeCmd );
            }
            _repaint( oldBoundingRect );
            _repaint( kpobject );
        } break;
        case MT_RESIZE_LF: {
            if ( !resizeObjNum ) break;
            kpobject = resizeObjNum;
            if ( firstX != mx || firstY != my ) {
                ResizeCmd *resizeCmd = new ResizeCmd( i18n( "Resize Object Left" ), mv, sz,
                                                      kpobject, m_view->kPresenterDoc() );
                resizeCmd->unexecute( false );
                resizeCmd->execute();
                m_view->kPresenterDoc()->addCommand( resizeCmd );
            }
            _repaint( oldBoundingRect );
            m_view->kPresenterDoc()->layout(kpobject);
            _repaint( kpobject );
        } break;
        case MT_RESIZE_RT: {
            if ( !resizeObjNum ) break;
            kpobject =  resizeObjNum ;

            if ( firstX != mx || firstY != my ) {
                ResizeCmd *resizeCmd = new ResizeCmd( i18n( "Resize Object Right" ), mv, sz,
                                                      kpobject, m_view->kPresenterDoc() );
                resizeCmd->unexecute( false );
                resizeCmd->execute();
                m_view->kPresenterDoc()->addCommand( resizeCmd );
            }
            _repaint( oldBoundingRect );
            m_view->kPresenterDoc()->layout(kpobject);
            _repaint( kpobject );
        } break;
        case MT_RESIZE_LU: {
            if ( !resizeObjNum ) break;
            kpobject = resizeObjNum;
            if ( firstX != mx || firstY != my ) {
                ResizeCmd *resizeCmd = new ResizeCmd( i18n( "Resize Object Left && Up" ), mv, sz,
                                                      kpobject, m_view->kPresenterDoc() );
                resizeCmd->unexecute( false );
                resizeCmd->execute();
                m_view->kPresenterDoc()->addCommand( resizeCmd );
            }
            _repaint( oldBoundingRect );
            m_view->kPresenterDoc()->layout(kpobject);
            _repaint( kpobject );
        } break;
        case MT_RESIZE_LD: {
            if ( !resizeObjNum ) break;
            kpobject =  resizeObjNum;
            if ( firstX != mx || firstY != my ) {
                ResizeCmd *resizeCmd = new ResizeCmd( i18n( "Resize Object Left && Down" ), mv, sz,
                                                      kpobject, m_view->kPresenterDoc() );
                resizeCmd->unexecute( false );
                resizeCmd->execute();
                m_view->kPresenterDoc()->addCommand( resizeCmd );
            }
            _repaint( oldBoundingRect );
            m_view->kPresenterDoc()->layout(kpobject);
            _repaint( kpobject );
        } break;
        case MT_RESIZE_RU: {
            if ( !resizeObjNum ) break;
            kpobject =  resizeObjNum;
            if ( firstX != mx || firstY != my ) {
                ResizeCmd *resizeCmd = new ResizeCmd( i18n( "Resize Object Right && Up" ), mv, sz,
                                                      kpobject, m_view->kPresenterDoc() );
                resizeCmd->unexecute( false );
                resizeCmd->execute();
                m_view->kPresenterDoc()->addCommand( resizeCmd );
            }
            _repaint( oldBoundingRect );
            m_view->kPresenterDoc()->layout(kpobject);
            _repaint( kpobject );
        } break;
        case MT_RESIZE_RD: {
            if ( !resizeObjNum ) break;
            kpobject = resizeObjNum;
            if ( firstX != mx || firstY != my ) {
                ResizeCmd *resizeCmd = new ResizeCmd( i18n( "Resize Object Right && Down" ), mv, sz,
                                                      kpobject, m_view->kPresenterDoc() );
                resizeCmd->unexecute( false );
                resizeCmd->execute();
                m_view->kPresenterDoc()->addCommand( resizeCmd );
            }
            _repaint( oldBoundingRect );
            m_view->kPresenterDoc()->layout(kpobject);
            _repaint( kpobject );
        } break;
        }
    } break;
    case INS_TEXT: {
        if ( !insRect.isNull() ) {
            rectSymetricalObjet();
            KPTextObject* kptextobject = insertTextObject( insRect );
            setToolEditMode( TEM_MOUSE );

            // User-friendlyness: automatically start editing this textobject
            activePage()->deSelectAllObj();
            stickyPage()->deSelectAllObj();
            createEditing( kptextobject );
            //setTextBackground( kptextobject );
            //setCursor( arrowCursor );
        }
    } break;
    case TEM_ZOOM:{
        drawContour = FALSE;
        if( modType == MT_NONE && drawRubber )
        {
            QPainter p;
            p.begin( this );
            p.setRasterOp( NotROP );
            p.setPen( QPen( black, 0, DotLine ) );
            p.drawRect( rubber );
            p.end();
            drawRubber = false;
            rubber = rubber.normalize();
            rubber.moveBy(diffx(),diffy());
            m_view->setZoomRect(rubber, m_zoomRubberDraw );
            m_zoomRubberDraw = false;
        }
    }break;
    case TEM_ROTATE: {
        drawContour = FALSE;
        if ( !rotateNum )
            break;
        if ( startAngle != rotateNum->getAngle() ) {
            QPtrList<RotateCmd::RotateValues> list;
            RotateCmd::RotateValues *v = new RotateCmd::RotateValues;
            v->angle = startAngle;
            list.append( v );
            QPtrList<KPObject> objects;
            objects.append( rotateNum );
            RotateCmd *rotateCmd = new RotateCmd( i18n( "Change Rotation" ), list,
                                                  rotateNum->getAngle(),
                                                  objects, m_view->kPresenterDoc() );
            m_view->kPresenterDoc()->addCommand( rotateCmd );
        }
    }break;
    case INS_LINE: {
        if ( insRect.width() != 0 && insRect.height() != 0 ) {
            rectSymetricalObjet();
            if ( insRect.top() == insRect.bottom() ) {
                bool reverse = insRect.left() > insRect.right();
                insRect = insRect.normalize();
                insRect.setRect( insRect.left(), insRect.top() - static_cast<int>(m_view->zoomHandler()->zoomItY(m_view->kPresenterDoc()->getGridY()) / 2),
                                 insRect.width(), m_view->zoomHandler()->zoomItY(m_view->kPresenterDoc()->getGridY()) );
                insertLineH( insRect, reverse );
            } else if ( insRect.left() == insRect.right() ) {
                bool reverse = insRect.top() > insRect.bottom();
                insRect = insRect.normalize();
                insRect.setRect( insRect.left() - static_cast<int>(m_view->zoomHandler()->zoomItX(m_view->kPresenterDoc()->getGridX()) / 2), insRect.top(),
                                 m_view->zoomHandler()->zoomItX(m_view->kPresenterDoc()->getGridX()), insRect.height() );
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
        if ( !insRect.isNull() )
        {
            rectSymetricalObjet();
            insertRect( insRect );
        }
        break;
    case INS_ELLIPSE:
        if ( !insRect.isNull() )
        {
            rectSymetricalObjet();
            insertEllipse( insRect );
        }
        break;
    case INS_PIE:
        if ( !insRect.isNull() )
        {
            rectSymetricalObjet();
            insertPie( insRect );
        }
        break;
    case INS_OBJECT:
    case INS_DIAGRAMM:
    case INS_TABLE:
    case INS_FORMULA: {
        if ( !insRect.isNull() ) {
            rectSymetricalObjet();
            KPPartObject *kpPartObject = insertObject( insRect );
            setToolEditMode( TEM_MOUSE );

            activePage()->deSelectAllObj();
            stickyPage()->deSelectAllObj();

            if ( kpPartObject ) {
                kpPartObject->activate( m_view );
                editNum = kpPartObject;
            }
        }
    } break;
    case INS_AUTOFORM: {
        bool reverse = insRect.left() > insRect.right() || insRect.top() > insRect.bottom();
        if ( !insRect.isNull() )
        {
            rectSymetricalObjet();
            insertAutoform( insRect, reverse );
        }
        setToolEditMode( TEM_MOUSE );
    } break;
    case INS_FREEHAND:
        if ( !m_pointArray.isNull() ) insertFreehand( m_pointArray );
        break;
    case INS_POLYGON:
        if ( !m_pointArray.isNull() )
            insertPolygon( m_pointArray );
        break;
    case INS_PICTURE:
    case INS_CLIPART: {
        if ( insRect.width() > 10 && insRect.height() > 10 )
            insertPicture( insRect );
        else
            insertPicture( QRect(), insRect.topLeft() ); // use the default size
        setToolEditMode( TEM_MOUSE );
    } break;
    case INS_CLOSED_FREEHAND: {
        if ( !m_pointArray.isNull() )
            insertClosedLine( m_pointArray );
    }break;
    default: break;
    }

    emit objectSelectedChanged();

    if ( toolEditMode != TEM_MOUSE && editMode )
        repaint( false );

    mousePressed = false;
    modType = MT_NONE;
    resizeObjNum = 0L;
    mouseMoveEvent( e );
    ratio = 0.0;
    keepRatio = false;
    calcBoundingRect();
}

void KPrCanvas::mouseMoveEvent( QMouseEvent *e )
{
    QPoint contentsPoint( e->pos().x()+diffx(), e->pos().y()+diffy() );
    int oldMx = m_savedMousePos.x();
    int oldMy = m_savedMousePos.y();
    m_savedMousePos = contentsPoint;
    KoPoint docPoint = m_view->zoomHandler()->unzoomPoint( contentsPoint );
    if(m_currentTextObjectView)
    {
        KPTextObject *txtObj=m_currentTextObjectView->kpTextObject();
        Q_ASSERT(txtObj);
        if(txtObj->contains( docPoint )&&mousePressed)
        {
            KoPoint pos = docPoint - txtObj->innerRect().topLeft();
            m_currentTextObjectView->mouseMoveEvent( e, m_view->zoomHandler()->ptToLayoutUnitPix( pos ) ); // in LU pixels
        }
        return;
    }

    if ( editMode ) {
        m_view->setRulerMousePos( e->x(), e->y() );

        KPObject *kpobject;

        if ( ( !mousePressed || ( m_tmpHorizHelpline !=-1 && m_tmpVertHelpline != -1 && modType == MT_NONE ) )&&
             ( !mousePressed || ( !drawRubber && modType == MT_NONE ) ) &&
             toolEditMode == TEM_MOUSE  ) {
            bool cursorAlreadySet = false;
            if ( (int)objectList().count() - 1 >= 0 || (int)stickyPage()->objectList().count() -1>=0 )
            {
                kpobject=m_activePage->getCursor(docPoint);
                if( kpobject)
                {
                    setCursor( kpobject->getCursor( docPoint, modType,m_view->kPresenterDoc()) );

                    cursorAlreadySet = true;
                }
                else
                {
                    kpobject=stickyPage()->getCursor(docPoint );
                    if( kpobject)
                    {
                        setCursor( kpobject->getCursor( docPoint, modType,m_view->kPresenterDoc() ) );
                        cursorAlreadySet = true;
                    }
                }
            }
            if( editMode && m_view->kPresenterDoc()->showHelplines())
            {
                if( m_view->kPresenterDoc()->indexOfHorizHelpline(m_view->zoomHandler()->unzoomItY(e->pos().y()+diffy()))!=-1)
                {
                    setCursor ( Qt::sizeVerCursor );
                    cursorAlreadySet = true;
                }
                else if ( m_view->kPresenterDoc()->indexOfVertHelpline(m_view->zoomHandler()->unzoomItX(e->pos().x()+diffx()))!=-1)
                {
                    setCursor ( Qt::sizeHorCursor );
                    cursorAlreadySet = true;
                }
                else if ( m_view->kPresenterDoc()->indexOfHelpPoint(KoPoint( m_view->zoomHandler()->unzoomItX(e->pos().x()+diffx()),
                                                                             m_view->zoomHandler()->unzoomItY(e->pos().y()+diffy())))!=-1)
                {
                    setCursor ( Qt::SizeAllCursor );
                    cursorAlreadySet = true;
                }
            }

            if ( !cursorAlreadySet )
                setCursor( arrowCursor );
            else
                return;
        } else if ( mousePressed ) {
            switch ( toolEditMode ) {
            case TEM_MOUSE: {
                drawContour = TRUE;
                if ( modType == MT_NONE ) {
                    if ( m_tmpVertHelpline !=-1 || m_tmpHorizHelpline !=-1)
                    {
                        tmpMoveHelpLine( e->pos() );
                    }
                    else if ( m_tmpHelpPoint != -1 )
                    {
                        tmpDrawMoveHelpPoint( e->pos() );
                    }
                    else if ( drawRubber ) {
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
                    m_hotSpot = docPoint - m_boundingRect.topLeft();
                    int x = e->x() + diffx();
                    int y = e->y() + diffy();
                    moveObject( x - m_origPos.x(), y - m_origPos.y(), false );
                } else if ( modType != MT_NONE && resizeObjNum ) {
                    int mx = e->x()+diffx();
                    int my = e->y()+diffy();

                    if ( m_view->kPresenterDoc()->snapToGrid() )
                    {
                        mx = applyGridOnPosX( mx );
                        my = applyGridOnPosY( my );
                        oldMx = applyGridOnPosX( oldMx );
                        oldMy = applyGridOnPosY( oldMy );
                    }

                    resizeObject( modType, mx - oldMx, my - oldMy );
                }
            } break;
            case TEM_ZOOM : {
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
                    m_zoomRubberDraw = true;
                }
            }break;
            case TEM_ROTATE: {
                drawContour = TRUE;
                double angle = KoPoint::getAngle( KoPoint( e->x() + diffx(), e->y() + diffy() ),
                                                  KoPoint( axisX, axisY ) );
                double angle1 = KoPoint::getAngle( KoPoint( firstX, firstY ),
                                                   KoPoint( axisX, axisY ) );

                angle -= angle1;
                angle -= startAngle;
                if ( angle < 0 )
                    angle += 360;
                else if ( angle > 360 )
                    angle -= 360;

                activePage()->rotateObj( angle );
                stickyPage()->rotateObj( angle );
            }break;
            case INS_TEXT: case INS_OBJECT: case INS_TABLE:
            case INS_DIAGRAMM: case INS_FORMULA: case INS_AUTOFORM:
            case INS_PICTURE: case INS_CLIPART: {
                QPainter p( this );
                p.setPen( QPen( black, 1, SolidLine ) );
                p.setBrush( NoBrush );
                p.setRasterOp( NotROP );
                if ( insRect.width() != 0 && insRect.height() != 0 )
                {
                    if ( !m_drawSymetricObject)
                        p.drawRect( insRect );
                    else
                    {
                        QRect tmpRect( insRect );
                        tmpRect.moveBy( -insRect.width(), -insRect.height());
                        tmpRect.setSize( 2*insRect.size() );
                        p.drawRect( tmpRect );
                    }
                }

                QPoint tmp = applyGrid( e->pos(), true);
                insRect.setRight( tmp.x() );
                insRect.setBottom( tmp.y() );
                limitSizeOfObject();

                QRect tmpRect( insRect );

                if ( e->state() & AltButton )
                {
                    m_drawSymetricObject = true;
                    tmpRect.moveBy( -insRect.width(), -insRect.height());
                    tmpRect.setSize( 2*insRect.size() );
                }
                else
                    m_drawSymetricObject = false;

                p.drawRect( tmpRect );
                p.end();

                mouseSelectedObject = true;

                m_view->penColorChanged( m_view->getPen() );
                m_view->brushColorChanged( m_view->getBrush() );
            } break;
            case INS_ELLIPSE: {
                QPainter p( this );
                p.setPen( QPen( black, 1, SolidLine ) );
                p.setBrush( NoBrush );
                p.setRasterOp( NotROP );
                if ( insRect.width() != 0 && insRect.height() != 0 )
                {
                    if ( !m_drawSymetricObject)
                        p.drawEllipse( insRect );
                    else
                    {
                        QRect tmpRect( insRect );
                        tmpRect.moveBy( -insRect.width(), -insRect.height());
                        tmpRect.setSize( 2*insRect.size() );
                        p.drawEllipse( tmpRect );
                    }
                }
                QPoint tmp = applyGrid( e->pos(), true);
                insRect.setRight( tmp.x() );
                insRect.setBottom( tmp.y() );
                limitSizeOfObject();


                QRect tmpRect( insRect );

                if ( e->state() & AltButton )
                {
                    m_drawSymetricObject = true;
                    tmpRect.moveBy( -insRect.width(), -insRect.height());
                    tmpRect.setSize( 2*insRect.size() );
                }
                else
                    m_drawSymetricObject = false;

                p.drawEllipse( tmpRect );
                p.end();

                mouseSelectedObject = true;

                m_view->penColorChanged( m_view->getPen() );
                m_view->brushColorChanged( m_view->getBrush() );
            } break;
            case INS_RECT: {
                QPainter p( this );
                p.setPen( QPen( black, 1, SolidLine ) );
                p.setBrush( NoBrush );
                p.setRasterOp( NotROP );
                if ( insRect.width() != 0 && insRect.height() != 0 )
                {
                    if ( !m_drawSymetricObject)
                        p.drawRoundRect( insRect, m_view->getRndX(), m_view->getRndY() );
                    else
                    {
                        QRect tmpRect( insRect );
                        tmpRect.moveBy( -insRect.width(), -insRect.height());
                        tmpRect.setSize( 2*insRect.size() );
                        p.drawRoundRect( tmpRect, m_view->getRndX(), m_view->getRndY() );
                    }
                }
                QPoint tmp = applyGrid( e->pos(), true);
                insRect.setRight( tmp.x() );
                insRect.setBottom( tmp.y() );
                limitSizeOfObject();

                QRect tmpRect( insRect );

                if ( e->state() & AltButton )
                {
                    m_drawSymetricObject = true;
                    tmpRect.moveBy( -insRect.width(), -insRect.height());
                    tmpRect.setSize( 2*insRect.size() );
                }
                else
                    m_drawSymetricObject = false;

                p.drawRoundRect( tmpRect, m_view->getRndX(), m_view->getRndY() );
                p.end();

                mouseSelectedObject = true;

                m_view->penColorChanged( m_view->getPen() );
                m_view->brushColorChanged( m_view->getBrush() );
            } break;
            case INS_LINE: {
                QPainter p( this );
                p.setPen( QPen( black, 1, SolidLine ) );
                p.setBrush( NoBrush );
                p.setRasterOp( NotROP );
                if ( insRect.width() != 0 && insRect.height() != 0 )
                {
                    if ( !m_drawSymetricObject)
                        p.drawLine( insRect.topLeft(), insRect.bottomRight() );
                    else
                    {
                        QRect tmpRect( insRect );
                        tmpRect.moveBy( -insRect.width(), -insRect.height());
                        tmpRect.setSize( 2*insRect.size() );
                        p.drawLine( tmpRect.topLeft(), tmpRect.bottomRight() );
                    }
                }
                QPoint tmp = applyGrid( e->pos(), true);
                int right = tmp.x();
                int bottom = tmp.y();
                if ( e->state() & ShiftButton )
                {
                    int witdh = QABS( right -insRect.left() );
                    int height = QABS( bottom - insRect.top() );
                    if ( witdh > height )
                        bottom = insRect.top();
                    else if ( witdh < height )
                        right = insRect.left();
                }
                insRect.setRight( right );
                insRect.setBottom( bottom );
                limitSizeOfObject();

                QRect lineRect( insRect );
                if ( e->state() & AltButton )
                {
                    m_drawSymetricObject = true;
                    lineRect.moveBy( -insRect.width(), -insRect.height());
                    lineRect.setSize( 2*insRect.size() );
                }
                else
                    m_drawSymetricObject = false;
                p.drawLine( lineRect.topLeft(), lineRect.bottomRight() );
                p.end();

                mouseSelectedObject = true;

                m_view->penColorChanged( m_view->getPen() );
                m_view->brushColorChanged( m_view->getBrush() );
            } break;
            case INS_PIE: {
                QPainter p( this );
                p.setPen( QPen( black, 1, SolidLine ) );
                p.setBrush( NoBrush );
                p.setRasterOp( NotROP );
                if ( insRect.width() != 0 && insRect.height() != 0 ) {
                    if ( !m_drawSymetricObject)
                        drawPieObject(&p, insRect);
                    else
                    {
                        QRect tmpRect( insRect );
                        tmpRect.moveBy( -insRect.width(), -insRect.height());
                        tmpRect.setSize( 2*insRect.size() );
                        drawPieObject(&p, tmpRect);
                    }
                }
                QPoint tmp = applyGrid( e->pos(), true);

                insRect.setRight( tmp.x());
                insRect.setBottom( tmp.y());
                limitSizeOfObject();

                QRect lineRect( insRect );
                if ( e->state() & AltButton )
                {
                    m_drawSymetricObject = true;
                    lineRect.moveBy( -insRect.width(), -insRect.height());
                    lineRect.setSize( 2*insRect.size() );
                }
                else
                    m_drawSymetricObject = false;

                drawPieObject(&p, lineRect);
                p.end();

                mouseSelectedObject = true;

                m_view->penColorChanged( m_view->getPen() );
                m_view->brushColorChanged( m_view->getBrush() );
            } break;
            case INS_FREEHAND: case INS_CLOSED_FREEHAND: {
                m_dragEndPoint = QPoint( e->x() , e->y() );

                QPainter p( this );
                p.setPen( QPen( black, 1, SolidLine ) );
                p.setBrush( NoBrush );
                p.setRasterOp( NotROP );

                m_dragEndPoint=limitOfPoint(m_dragEndPoint);

                p.drawLine( m_dragStartPoint, m_dragEndPoint );
                p.end();

                m_pointArray.putPoints( m_indexPointArray, 1, m_view->zoomHandler()->unzoomItX(m_dragStartPoint.x()),
                                        m_view->zoomHandler()->unzoomItY(m_dragStartPoint.y()) );
                ++m_indexPointArray;
                m_dragStartPoint = m_dragEndPoint;

                mouseSelectedObject = true;

                m_view->penColorChanged( m_view->getPen() );
                m_view->brushColorChanged( m_view->getBrush() );
            } break;
            case INS_POLYLINE: case INS_CLOSED_POLYLINE: {
                QPainter p( this );
                p.setPen( QPen( black, 1, SolidLine ) );
                p.setBrush( NoBrush );
                p.setRasterOp( NotROP );
                p.drawLine( m_dragStartPoint, m_dragEndPoint ); //
                QPoint tmp = applyGrid( e->pos(), true);

                int posX = tmp.x();
                int posY = tmp.y();
                if ( e->state() & ShiftButton )
                {
                    int witdh = QABS( posX -m_dragStartPoint.x() );
                    int height = QABS( posY - m_dragStartPoint.y() );
                    if ( witdh > height )
                        posY = m_dragStartPoint.y();
                    else if ( witdh < height )
                        posX = m_dragStartPoint.x();
                }


                m_dragEndPoint = QPoint( posX, posY);
                m_dragEndPoint=limitOfPoint(m_dragEndPoint);

                p.drawLine( m_dragStartPoint, m_dragEndPoint );
                p.end();

                mouseSelectedObject = true;

                m_view->penColorChanged( m_view->getPen() );
                m_view->brushColorChanged( m_view->getBrush() );
            } break;
            case INS_CUBICBEZIERCURVE: case INS_QUADRICBEZIERCURVE:
            case INS_CLOSED_CUBICBEZIERCURVE: case INS_CLOSED_QUADRICBEZIERCURVE:{
                QPoint tmp = applyGrid( e->pos(), true);

                drawCubicBezierCurve( tmp.x(),
                                      tmp.y());

                mouseSelectedObject = true;

                m_view->penColorChanged( m_view->getPen() );
                m_view->brushColorChanged( m_view->getBrush() );
            } break;
            case INS_POLYGON: {
                drawPolygon( m_view->zoomHandler()->unzoomPoint( m_dragStartPoint ),
                             m_view->zoomHandler()->unzoomPoint( m_dragEndPoint ) ); // erase old polygon
                QPoint tmp = applyGrid( e->pos(), true);

                m_dragEndPoint = QPoint( tmp.x(),
                                         tmp.y() );
                m_dragEndPoint=limitOfPoint(m_dragEndPoint);

                drawPolygon( m_view->zoomHandler()->unzoomPoint( m_dragStartPoint ),
                             m_view->zoomHandler()->unzoomPoint( m_dragEndPoint ) ); // draw new polygon

                mouseSelectedObject = true;

                m_view->penColorChanged( m_view->getPen() );
                m_view->brushColorChanged( m_view->getBrush() );
            } break;
            default: break;
            }
        }
    } else if ( !editMode && drawMode && drawLineInDrawMode ) {
        QPainter p;
        p.begin( this );
        p.setPen( m_view->kPresenterDoc()->presPen() );
        p.drawLine( oldMx, oldMy, e->x(), e->y() );
        p.end();
        m_drawModeLines[m_drawModeLines.count() - 1].putPoints( m_drawModeLineIndex++, 1, e->x(), e->y() );
    }

    if ( !editMode && !drawMode && !presMenu->isVisible() && fillBlack )
        setCursor( blankCursor );
}

void KPrCanvas::mouseDoubleClickEvent( QMouseEvent *e )
{
    if(!m_view->koDocument()->isReadWrite())
        return;
    QPoint contentsPoint( e->pos().x()+diffx(), e->pos().y()+diffy() );
    KoPoint docPoint = m_view->zoomHandler()->unzoomPoint( contentsPoint );
    if(m_currentTextObjectView)
    {
        KPTextObject *txtObj=m_currentTextObjectView->kpTextObject();
        Q_ASSERT(txtObj);
        if(txtObj->contains( docPoint ))
        {
            KoPoint pos = contentsPoint - txtObj->getOrig();
            //pos=m_view->zoomHandler()->pixelToLayoutUnit(QPoint(pos.x(),pos.y()));
            m_currentTextObjectView->mouseDoubleClickEvent( e, m_view->zoomHandler()->ptToLayoutUnitPix( pos ) );
            return;
        }
    }

    //disallow activating objects outside the "page"
    if ( !m_activePage->getPageRect().contains(docPoint,m_view->zoomHandler()))
        return;


    if ( ( toolEditMode == INS_POLYLINE || toolEditMode == INS_CLOSED_POLYLINE ) && !m_pointArray.isNull() && m_drawPolyline ) {
        m_dragStartPoint = applyGrid( e->pos(), true);
        m_pointArray.putPoints( m_indexPointArray, 1, m_view->zoomHandler()->unzoomItX(m_dragStartPoint.x()),
                                m_view->zoomHandler()->unzoomItY(m_dragStartPoint.y() ));
        ++m_indexPointArray;
        endDrawPolyline();

        mouseMoveEvent( e );
        return;
    }


    if ( toolEditMode != TEM_MOUSE || !editMode ) return;

    deSelectAllObj();
    KPObject *kpobject = 0;
    kpobject=m_activePage->getObjectAt(docPoint);
    if( !kpobject)
    {
        kpobject=stickyPage()->getObjectAt(docPoint );
        if( kpobject && m_view->kPresenterDoc()->isHeaderFooter(kpobject))
        {
            if( objectIsAHeaderFooterHidden(kpobject))
                kpobject=0L;
        }
    }
    if(kpobject)
    {
        if ( kpobject->getType() == OT_TEXT )
        {
            KPTextObject *kptextobject = dynamic_cast<KPTextObject*>( kpobject );
            if(kptextobject && (!kptextobject->isProtectContent() || kptextobject->isProtectContent()
                                && m_view->kPresenterDoc()->cursorInProtectedArea()))
            {
                if(m_currentTextObjectView)
                {
                    m_currentTextObjectView->terminate();
                    delete m_currentTextObjectView;
                }
                m_currentTextObjectView=kptextobject->createKPTextView(this);

                //setTextBackground( kptextobject );
                setCursor( arrowCursor );
                editNum = kpobject;
            }
        }
        else if ( kpobject->getType() == OT_PART )
        {
            KPPartObject * obj=dynamic_cast<KPPartObject *>(kpobject);
            if(obj)
            {
                obj->activate( m_view );
                editNum = obj;
            }
        }
    }
}

void KPrCanvas::drawPieObject(QPainter *p,  const QRect & rect)
{
    switch ( m_view->getPieType() ) {
    case PT_PIE:
        p->drawPie( rect.x(), rect.y(), rect.width() - 2,
                    rect.height() - 2, m_view->getPieAngle(), m_view->getPieLength() );
        break;
    case PT_ARC:
        p->drawArc( rect.x(), insRect.y(), rect.width() - 2,
                    rect.height() - 2, m_view->getPieAngle(), m_view->getPieLength() );
        break;
    case PT_CHORD:
        p->drawChord( rect.x(), rect.y(), rect.width() - 2,
                      rect.height() - 2, m_view->getPieAngle(), m_view->getPieLength() );
        break;
    default: break;
    }

}

void KPrCanvas::limitSizeOfObject()
{
    QRect pageRect=m_activePage->getZoomPageRect();

    if(insRect.right()>pageRect.right()-1)
        insRect.setRight(pageRect.right()-1);
    else if( insRect.right()<pageRect.left()-1)
        insRect.setRight(pageRect.left()+1);
    if(insRect.bottom()>pageRect.bottom()-1)
        insRect.setBottom(pageRect.bottom()-1);
    else if( insRect.bottom()<pageRect.top()-1)
        insRect.setBottom(pageRect.top()+1);
}

QPoint KPrCanvas::limitOfPoint(const QPoint& _point) const
{
    QRect pageRect=m_activePage->getZoomPageRect();
    QPoint point(_point);
    if(point.x()>pageRect.right()-1)
        point.setX(pageRect.right()-1);
    else if( point.x()<pageRect.left()-1)
        point.setX(pageRect.left()+1);
    if(point.y()>pageRect.bottom()-1)
        point.setY(pageRect.bottom()-1);
    else if( point.y()<pageRect.top()-1)
        point.setY(pageRect.top()+1);
    return point;
}

void KPrCanvas::wheelEvent( QWheelEvent *e )
{
    if ( !editMode && !drawMode ) {
        if ( e->delta() == -120 )     // wheel down
            m_view->screenNext();
        else if ( e->delta() == 120 ) // wheel up
            m_view->screenPrev();
        e->accept();
    }
    else if ( editMode )
        emit sigMouseWheelEvent( e );
}

void KPrCanvas::keyPressEvent( QKeyEvent *e )
{
    if ( !editMode ) {
        switch ( e->key() ) {
        case Key_Space: case Key_Right: case Key_Down: case Key_Next:
            m_view->screenNext(); break;
        case Key_Backspace: case Key_Left: case Key_Up: case Key_Prior:
            m_view->screenPrev(); break;
        case Key_Escape: case Key_Q: case Key_X:
            m_view->screenStop(); break;
        case Key_G:
            if ( !spManualSwitch() )
                m_view->autoScreenPresStopTimer();
            slotGotoPage(); break;
        case Key_Home:  // go to first page
            if ( slideListIterator != slideList.begin() ) {
                presGotoFirstPage();
                if ( !spManualSwitch() ) {
                    m_view->setCurrentTimer( 1 );
                    setNextPageTimer( true );
                }
            }
            break;
        case Key_End:  // go to last page
            if ( slideListIterator != slideList.end() ) {
                gotoPage( *(--slideList.end()) );
                if ( !spManualSwitch() ) {
                    m_view->setCurrentTimer( 1 );
                    setNextPageTimer( true );
                }
            }
            break;
        default: break;
        }
    } else if ( editNum ) {
        if ( e->key() == Key_Escape ) {
            exitEditMode();
        }
        else if ( m_currentTextObjectView )
        {
            if ( !m_currentTextObjectView->kpTextObject()->isProtectContent() || (e->text().length() == 0))
                m_currentTextObjectView->keyPressEvent( e );
            else
                KMessageBox::information(this, i18n("Read-only content cannot be changed. No modifications will be accepted."));
        }
    } else if ( mouseSelectedObject ) {
        m_hotSpot = KoPoint(0,0);
        if ( e->state() & ControlButton ) {
            int offsetx, offsety;

            if( !m_view->kPresenterDoc()->snapToGrid() ) {
                offsetx = QMAX(1,m_view->zoomHandler()->zoomItX(10));
                offsety = QMAX(1,m_view->zoomHandler()->zoomItY(10));
            } else {
                offsetx = QMAX(1,m_view->zoomHandler()->zoomItX(m_view->kPresenterDoc()->getGridX()));
                offsety = QMAX(1,m_view->zoomHandler()->zoomItY(m_view->kPresenterDoc()->getGridY()));
            }

            if ( !m_keyPressEvent )
            {
                moveStartPosKey = m_boundingRect.topLeft();
            }
            switch ( e->key() ) {
            case Key_Up:
                m_keyPressEvent = true;
                moveObject( 0, -offsety, false );
                m_origBRect = m_boundingRect;
                break;
            case Key_Down:
                m_keyPressEvent = true;
                moveObject( 0, offsety, false );
                m_origBRect = m_boundingRect;
                break;
            case Key_Right:
                m_keyPressEvent = true;
                moveObject( offsetx, 0, false );
                m_origBRect = m_boundingRect;
                break;
            case Key_Left:
                m_keyPressEvent = true;
                moveObject( -offsetx, 0, false );
                m_origBRect = m_boundingRect;
                break;
            default: break;
            }

        } else {
            int offsetx, offsety;

            if( !m_view->kPresenterDoc()->snapToGrid() ) {
                offsetx = 1;
                offsety = 1;
            } else {
                offsetx = QMAX(1,m_view->zoomHandler()->zoomItX(m_view->kPresenterDoc()->getGridX()));
                offsety = QMAX(1,m_view->zoomHandler()->zoomItY(m_view->kPresenterDoc()->getGridY()));
            }

            if ( !m_keyPressEvent )
            {
                moveStartPosKey = m_boundingRect.topLeft();
            }
            switch ( e->key() ) {
            case Key_Up:
                m_keyPressEvent = true;
                moveObject( 0, -offsety, false );
                m_origBRect = m_boundingRect;
                break;
            case Key_Down:
                m_keyPressEvent = true;
                moveObject( 0, offsety, false );
                m_origBRect = m_boundingRect;
                break;
            case Key_Right:
                m_keyPressEvent = true;
                moveObject( offsetx, 0, false );
                m_origBRect = m_boundingRect;
                break;
            case Key_Left:
                m_keyPressEvent = true;
                moveObject( -offsetx, 0, false );
                m_origBRect = m_boundingRect;
                break;
            case Key_Delete: case Key_Backspace:
                m_view->editDelete();
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
            m_view->nextPage();
            break;
        case Key_Prior:
            m_view->prevPage();
            break;
        case Key_Down:
            m_view->getVScrollBar()->addLine();
            break;
        case Key_Up:
            m_view->getVScrollBar()->subtractLine();
            break;
        case Key_Right:
            m_view->getHScrollBar()->addLine();
            break;
        case Key_Left:
            m_view->getHScrollBar()->subtractLine();
            break;
        case Key_Tab:
            selectNext();
            break;
        case Key_Backtab:
            selectPrev();
            break;
        case Key_Home:
            m_view->getVScrollBar()->setValue( 0 );
            break;
        case Key_End:
            m_view->getVScrollBar()->setValue( m_view->getVScrollBar()->maxValue());
            break;
        case Key_Escape:
            if ( toolEditMode == TEM_ZOOM )
                setToolEditMode( TEM_MOUSE );
            break;
        default: break;
        }
    }
}

void KPrCanvas::keyReleaseEvent( QKeyEvent *e )
{
    if ( editMode && m_currentTextObjectView )
        m_currentTextObjectView->keyReleaseEvent( e );
    else
    {
        if ( mouseSelectedObject )
        {
            if(e->key()==Key_Up || e->key()==Key_Down || e->key()==Key_Right || e->key()==Key_Left)
            {
                if ( !e->isAutoRepeat() )
                {
                    KMacroCommand *macro=0L;
                    KoPoint move( m_boundingRect.topLeft() - moveStartPosKey);
                    KCommand *cmd=m_activePage->moveObject(m_view, move.x(), move.y());
                    if(cmd)
                    {
                        if ( ! macro )
                            macro=new KMacroCommand(i18n("Move Objects"));
                        macro->addCommand(cmd);
                    }
                    cmd=stickyPage()->moveObject(m_view, move.x(), move.y());
                    if(cmd)
                    {
                        if ( ! macro )
                            macro=new KMacroCommand(i18n("Move Objects"));

                        macro->addCommand(cmd);
                    }
                    if(macro)
                        m_view->kPresenterDoc()->addCommand(macro );
                    m_keyPressEvent = false;
                }
                emit objectSelectedChanged();
            }
        }
    }
}

void KPrCanvas::resizeEvent( QResizeEvent *e )
{
    if ( editMode )
        QWidget::resizeEvent( e );
    else
#if KDE_IS_VERSION(3,1,90)
        QWidget::resizeEvent( new QResizeEvent( KGlobalSettings::desktopGeometry(this).size(),
                                                e->oldSize() ) );
#else
        QWidget::resizeEvent( new QResizeEvent( QApplication::desktop()->screenGeometry(this).size(),
                                                e->oldSize() ) );
#endif
    buffer.resize( size() );
}

KPObject* KPrCanvas::getObjectAt( const KoPoint&pos )
{
    KPObject *kpobject=m_activePage->getObjectAt(pos);
    if( !kpobject)
        kpobject=stickyPage()->getObjectAt(pos);
    return kpobject;
}

void KPrCanvas::selectObj( KPObject *kpobject )
{
    kpobject->setSelected( true );
    m_view->penColorChanged( m_activePage->getPen( QPen( Qt::black, 1, Qt::SolidLine ) ) );
    m_view->brushColorChanged( m_activePage->getBrush( QBrush( Qt::white, Qt::SolidPattern ) ) );
    _repaint( kpobject );
    emit objectSelectedChanged();

    mouseSelectedObject = true;
}

void KPrCanvas::deSelectObj( KPObject *kpobject )
{
    kpobject->setSelected( false );
    _repaint( kpobject );

    mouseSelectedObject = false;
    emit objectSelectedChanged();
}
bool KPrCanvas::exportPage( int nPage,
                            int nWidth,
                            int nHeight,
                            const KURL& _fileURL,
                            const char* format,
                            int quality )
{
    bool res = false;
    const QCursor oldCursor( cursor() );
    setCursor( waitCursor );
    QPixmap pix( nWidth, nHeight );
    drawPageInPix( pix, nPage, 0, true, nWidth, nHeight );
    if( !pix.isNull() ){
        // Depending on the desired target size due to rounding
        // errors during zoom the resulting pixmap *might* be
        // 1 pixel or 2 pixels wider/higher than desired: we just
        // remove the additional columns/rows.  This can be done
        // since KPresenter is leaving a minimal border below/at
        // the right of the image anyway.
        const QSize desiredSize(nWidth, nHeight);
        if( desiredSize != pix.size() )
            pix.resize( desiredSize );
        // save the pixmap to the desired file
        KURL fileURL(_fileURL);
        if( fileURL.protocol().isEmpty() )
            fileURL.setProtocol( "file" );
        const bool bLocalFile = fileURL.isLocalFile();
        KTempFile* tmpFile = bLocalFile ? NULL : new KTempFile();
        if( !bLocalFile )
            tmpFile->setAutoDelete( true );
        if( bLocalFile || 0 == tmpFile->status() ){
            QFile file( bLocalFile ? fileURL.path(0) : tmpFile->name() );
            if ( file.open( IO_ReadWrite ) ) {
#if KDE_IS_VERSION(3,0,90)
		res = pix.save( file.name(), format, quality );
#else		    
		res = pix.save( &file, format, quality );
#endif
		file.close();
            }
            if( !bLocalFile ){
                if( res ){
#if KDE_IS_VERSION(3,1,90)
                    res = KIO::NetAccess::upload( tmpFile->name(), fileURL, this );
#else
                    res = KIO::NetAccess::upload( tmpFile->name(), fileURL );
#endif
                }
            }
        }
        if( !bLocalFile ){
            delete tmpFile;
        }
    }
    setCursor( oldCursor );
    return res;
}


void KPrCanvas::selectAllObj()
{
    int nbObj=objectList().count()+stickyPage()->objectList().count();
    if(nbObj==(stickyPage()->numSelected()+m_activePage->numSelected()))
        return;

    QProgressDialog progress( i18n( "Selecting..." ), 0,
                              nbObj, this );
    int i=0;
    QPtrListIterator<KPObject> it( stickyPage()->objectList() );
    for ( ; it.current() ; ++it )
    {
        if ( !objectIsAHeaderFooterHidden(it.current()) )
            selectObj(it.current());
        progress.setProgress( i );
        kapp->processEvents();
        i++;
    }

    it= m_activePage->objectList();
    for ( ; it.current() ; ++it )
    {
        selectObj(it.current());
        progress.setProgress( i );

        kapp->processEvents();
        i++;
    }

    mouseSelectedObject = true;
    emit objectSelectedChanged();
}


void KPrCanvas::deSelectAllObj()
{
    if( m_activePage->numSelected()==0 && stickyPage()->numSelected() == 0 )
        return;

    if ( !m_view->kPresenterDoc()->raiseAndLowerObject && selectedObjectPosition != -1 ) {
        lowerObject();
        selectedObjectPosition = -1;
    }
    else
        m_view->kPresenterDoc()->raiseAndLowerObject = false;

    m_activePage->deSelectAllObj();
    stickyPage()->deSelectAllObj();

    //desactivate kptextview when we switch of page
    if(m_currentTextObjectView)
    {
        m_currentTextObjectView->terminate();
        m_currentTextObjectView->kpTextObject()->setEditingTextObj( false );
        delete m_currentTextObjectView;
        m_currentTextObjectView=0L;
    }
    mouseSelectedObject = false;
    emit objectSelectedChanged();
}

void KPrCanvas::setMouseSelectedObject(bool b)
{
    mouseSelectedObject = b;
    emit objectSelectedChanged();
}

void KPrCanvas::setupMenus()
{
    // create right button presentation menu
    presMenu = new QPopupMenu();
    Q_CHECK_PTR( presMenu );
    presMenu->setCheckable( true );
    PM_SM = presMenu->insertItem( i18n( "&Switching Mode" ), this, SLOT( switchingMode() ) );
    PM_DM = presMenu->insertItem( i18n( "&Drawing Mode" ), this, SLOT( drawingMode() ) );
    presMenu->insertSeparator();
    presMenu->insertItem( SmallIcon("goto"), i18n( "&Goto Slide..." ), this, SLOT( slotGotoPage() ) );
    presMenu->insertSeparator();
    presMenu->insertItem( i18n( "&End Slide Show" ), this, SLOT( slotExitPres() ) );
    presMenu->setItemChecked( PM_SM, true );
    presMenu->setItemChecked( PM_DM, false );
    presMenu->setMouseTracking( true );
}

void KPrCanvas::clipCut()
{
    if ( m_currentTextObjectView )
        m_currentTextObjectView->cut();
    m_view->editCut();
}

void KPrCanvas::clipCopy()
{
    if ( m_currentTextObjectView )
        m_currentTextObjectView->copy();
    m_view->editCopy();
}

void KPrCanvas::clipPaste()
{
    if ( m_currentTextObjectView )
        m_currentTextObjectView->paste();
    m_view->editPaste();
}

void KPrCanvas::chPic()
{
    bool state=m_activePage->chPic( m_view);
    if( state)
        return;
    stickyPage()->chPic(m_view);
}

void KPrCanvas::savePicture()
{
    bool state=m_activePage->savePicture( m_view);
    if( state)
        return;
    stickyPage()->savePicture(m_view);
}

void KPrCanvas::setTextFormat(const KoTextFormat &format, int flags)
{
    QPtrList<KoTextFormatInterface> lst = applicableTextInterfaces();
    if ( lst.isEmpty() ) return;
    KMacroCommand* macroCmd = new KMacroCommand( i18n("Change Text Font") );
    QPtrListIterator<KoTextFormatInterface> it( lst );
    for ( ; it.current() ; ++it ) {
        KCommand *cmd = it.current()->setFormatCommand(&format, flags, true);

        if (cmd)
            macroCmd->addCommand( cmd );
    }
    m_view->kPresenterDoc()->addCommand( macroCmd );
}

void KPrCanvas::setTextColor( const QColor &color )
{
    QPtrList<KoTextFormatInterface> lst = applicableTextInterfaces();
    if ( lst.isEmpty() ) return;
    QPtrListIterator<KoTextFormatInterface> it( lst );
    KMacroCommand* macroCmd = new KMacroCommand( i18n("Set Text Color") );
    for ( ; it.current() ; ++it ) {
        KCommand* cmd = it.current()->setTextColorCommand( color );
        if (cmd)
            macroCmd->addCommand( cmd );
    }
    m_view->kPresenterDoc()->addCommand( macroCmd );
}

void KPrCanvas::setTextBackgroundColor( const QColor &color )
{
    QPtrList<KoTextFormatInterface> lst = applicableTextInterfaces();
    if ( lst.isEmpty() ) return;
    QPtrListIterator<KoTextFormatInterface> it( lst );
    KMacroCommand* macroCmd = 0L;
    for ( ; it.current() ; ++it ) {
        KCommand* cmd = it.current()->setTextBackgroundColorCommand( color );
        if ( cmd )
        {
            if ( !macroCmd )
                macroCmd = new KMacroCommand( i18n("Set Text Background Color") );
            macroCmd->addCommand( cmd );
        }
    }
    if (macroCmd )
        m_view->kPresenterDoc()->addCommand( macroCmd );
}

void KPrCanvas::setTextBold( bool b )
{
    QPtrList<KoTextFormatInterface> lst = applicableTextInterfaces();
    if ( lst.isEmpty() ) return;
    QPtrListIterator<KoTextFormatInterface> it( lst );
    KMacroCommand* macroCmd = 0L;
    for ( ; it.current() ; ++it ) {
        KCommand* cmd = it.current()->setBoldCommand( b );
        if ( cmd )
        {
            if ( !macroCmd )
                macroCmd = new KMacroCommand( i18n("Make Text Bold") );
            macroCmd->addCommand( cmd );
        }
    }
    if ( macroCmd )
        m_view->kPresenterDoc()->addCommand( macroCmd );
}

void KPrCanvas::setTextItalic( bool b )
{
    QPtrList<KoTextFormatInterface> lst = applicableTextInterfaces();
    if ( lst.isEmpty() ) return;
    QPtrListIterator<KoTextFormatInterface> it( lst );
    KMacroCommand* macroCmd = 0L;
    for ( ; it.current() ; ++it ) {
        KCommand* cmd = it.current()->setItalicCommand( b );
        if ( cmd )
        {
            if ( !macroCmd )
                macroCmd = new KMacroCommand( i18n("Make Text Italic") );
            macroCmd->addCommand( cmd );
        }
    }
    if (macroCmd )
        m_view->kPresenterDoc()->addCommand( macroCmd );
}

void KPrCanvas::setTextUnderline( bool b )
{
    QPtrList<KoTextFormatInterface> lst = applicableTextInterfaces();
    if ( lst.isEmpty() ) return;
    QPtrListIterator<KoTextFormatInterface> it( lst );
    KMacroCommand* macroCmd = 0L;
    for ( ; it.current() ; ++it ) {
        KCommand* cmd = it.current()->setUnderlineCommand( b );
        if ( cmd )
        {
            if ( !macroCmd )
                macroCmd = new KMacroCommand( i18n("Underline Text") );
            macroCmd->addCommand( cmd );
        }
    }
    if ( macroCmd )
        m_view->kPresenterDoc()->addCommand( macroCmd );
}

void KPrCanvas::setTextStrikeOut( bool b )
{
    QPtrList<KoTextFormatInterface> lst = applicableTextInterfaces();
    if ( lst.isEmpty() ) return;
    KMacroCommand* macroCmd = 0L;
    QPtrListIterator<KoTextFormatInterface> it( lst );
    for ( ; it.current() ; ++it ) {
        KCommand *cmd = it.current()->setStrikeOutCommand( b );
        if ( cmd )
        {
            if ( !macroCmd )
                macroCmd = new KMacroCommand( i18n("Make Text Italic") );
            macroCmd->addCommand( cmd );
        }
    }
    if ( macroCmd )
        m_view->kPresenterDoc()->addCommand( macroCmd );
}

void KPrCanvas::setTextFamily( const QString &f )
{
    QPtrList<KoTextFormatInterface> lst = applicableTextInterfaces();
    if ( lst.isEmpty() ) return;
    KMacroCommand* macroCmd = 0L;
    QPtrListIterator<KoTextFormatInterface> it( lst );
    for ( ; it.current() ; ++it ) {
        KCommand* cmd = it.current()->setFamilyCommand( f );
        if ( cmd )
        {
            if ( !macroCmd )
                macroCmd = new KMacroCommand( i18n("Set Text Font") );
            macroCmd->addCommand( cmd );
        }
    }
    if ( macroCmd)
        m_view->kPresenterDoc()->addCommand( macroCmd );
}

void KPrCanvas::setTextPointSize( int s )
{
    QPtrList<KoTextFormatInterface> lst = applicableTextInterfaces();
    if ( lst.isEmpty() ) return;
    KMacroCommand* macroCmd = 0L;
    QPtrListIterator<KoTextFormatInterface> it( lst );
    for ( ; it.current() ; ++it ) {
        KCommand* cmd = it.current()->setPointSizeCommand( s );
        if ( cmd )
        {
            if ( !macroCmd )
                macroCmd = new KMacroCommand( i18n("Change Text Size") );
            macroCmd->addCommand( cmd );
        }

    }
    if (macroCmd)
        m_view->kPresenterDoc()->addCommand( macroCmd );
}

void KPrCanvas::setTextSubScript( bool b )
{
    QPtrList<KoTextFormatInterface> lst = applicableTextInterfaces();
    if ( lst.isEmpty() ) return;
    QPtrListIterator<KoTextFormatInterface> it( lst );
    KMacroCommand* macroCmd = 0L;
    for ( ; it.current() ; ++it ) {
        KCommand* cmd = it.current()->setTextSubScriptCommand( b );
        if ( cmd )
        {
            if ( !macroCmd )
                macroCmd = new KMacroCommand( i18n("Set Text Subscript") );
            macroCmd->addCommand( cmd );
        }
    }
    if ( macroCmd )
        m_view->kPresenterDoc()->addCommand( macroCmd );
}

void KPrCanvas::setTextSuperScript( bool b )
{
    QPtrList<KoTextFormatInterface> lst = applicableTextInterfaces();
    if ( lst.isEmpty() ) return;
    QPtrListIterator<KoTextFormatInterface> it( lst );
    KMacroCommand* macroCmd = 0L;
    for ( ; it.current() ; ++it ) {
        KCommand* cmd = it.current()->setTextSuperScriptCommand( b );
        if ( cmd )
        {
            if ( !macroCmd )
                macroCmd = new KMacroCommand( i18n("Set Text Superscript") );
            macroCmd->addCommand( cmd );
        }
    }
    if ( macroCmd )
        m_view->kPresenterDoc()->addCommand( macroCmd );
}

void KPrCanvas::setTextDefaultFormat( )
{
    QPtrList<KoTextFormatInterface> lst = applicableTextInterfaces();
    if ( lst.isEmpty() ) return;
    QPtrListIterator<KoTextFormatInterface> it( lst );
    KMacroCommand* macroCmd = 0L;
    for ( ; it.current() ; ++it ) {
        KCommand* cmd = it.current()->setDefaultFormatCommand( );
        if ( cmd )
        {
            if ( !macroCmd )
                macroCmd = new KMacroCommand( i18n("Apply Default Format") );
            macroCmd->addCommand( cmd );
        }
    }
    if (macroCmd)
        m_view->kPresenterDoc()->addCommand( macroCmd );
}

void KPrCanvas::setIncreaseFontSize()
{
    QPtrList<KoTextFormatInterface> lst = applicableTextInterfaces();
    if ( lst.isEmpty() ) return;
    QPtrListIterator<KoTextFormatInterface> it( lst );
    int size = it.current()->currentFormat()->pointSize();
    KMacroCommand* macroCmd =0L;
    for ( ; it.current() ; ++it ) {
        KCommand* cmd = it.current()->setPointSizeCommand( size+1 );
        if ( cmd )
        {
            if ( !macroCmd )
                macroCmd = new KMacroCommand( i18n("Increase Font Size") );
            macroCmd->addCommand( cmd );
        }
    }
    if ( macroCmd)
        m_view->kPresenterDoc()->addCommand( macroCmd );
}

void KPrCanvas::setDecreaseFontSize()
{
    QPtrList<KoTextFormatInterface> lst = applicableTextInterfaces();
    if ( lst.isEmpty() ) return;
    QPtrListIterator<KoTextFormatInterface> it( lst );
    int size = it.current()->currentFormat()->pointSize();
    KMacroCommand* macroCmd = 0L;
    for ( ; it.current() ; ++it ) {
        KCommand* cmd = it.current()->setPointSizeCommand( size-1 );
        if ( cmd )
        {
            if ( !macroCmd )
                macroCmd = new KMacroCommand( i18n("Decrease Font Size") );
            macroCmd->addCommand( cmd );
        }
    }
    if ( macroCmd )
        m_view->kPresenterDoc()->addCommand( macroCmd );
}

void KPrCanvas::setTextAlign( int align )
{
    QPtrList<KoTextFormatInterface> lst = applicableTextInterfaces();
    if ( lst.isEmpty() ) return;
    QPtrListIterator<KoTextFormatInterface> it( lst );
    KMacroCommand* macroCmd = 0L;
    for ( ; it.current() ; ++it ) {
        KCommand* cmd = it.current()->setAlignCommand(align);
        if ( cmd )
        {
            if ( !macroCmd)
                macroCmd = new KMacroCommand( i18n("Set Text Align") );
            macroCmd->addCommand( cmd );
        }
    }
    if ( macroCmd )
        m_view->kPresenterDoc()->addCommand( macroCmd );
}

void KPrCanvas::setTabList( const KoTabulatorList & tabList )
{
    QPtrList<KoTextFormatInterface> lst = applicableTextInterfaces();
    if ( lst.isEmpty() ) return;
    QPtrListIterator<KoTextFormatInterface> it( lst );
    KMacroCommand* macroCmd = 0L;
    for ( ; it.current() ; ++it ) {
        KCommand* cmd = it.current()->setTabListCommand(tabList );
        if ( cmd )
        {
            if ( !macroCmd )
                macroCmd = new KMacroCommand( i18n("Change Tabulators") );
            macroCmd->addCommand( cmd );
        }
    }
    if ( macroCmd)
        m_view->kPresenterDoc()->addCommand( macroCmd );
}

void KPrCanvas::setTextDepthPlus()
{
    QPtrList<KoTextFormatInterface> lst = applicableTextInterfaces();
    if ( lst.isEmpty() ) return;
    double leftMargin=0.0;
    if(!lst.isEmpty())
        leftMargin=lst.first()->currentParagLayoutFormat()->margins[QStyleSheetItem::MarginLeft];
    double indent = m_view->kPresenterDoc()->getIndentValue();
    double newVal = leftMargin + indent;
    QPtrListIterator<KoTextFormatInterface> it( lst );
    KMacroCommand* macroCmd = 0L;
    for ( ; it.current() ; ++it ) {
        KCommand* cmd = it.current()->setMarginCommand(QStyleSheetItem::MarginLeft, newVal);
        if ( cmd )
        {
            if ( !macroCmd )
                macroCmd = new KMacroCommand( i18n("Increase Paragraph Depth") );
            macroCmd->addCommand( cmd );
        }
    }
    if ( macroCmd )
        m_view->kPresenterDoc()->addCommand( macroCmd );
    if(!lst.isEmpty())
    {
        const KoParagLayout *layout=lst.first()->currentParagLayoutFormat();
        m_view->showRulerIndent( layout->margins[QStyleSheetItem::MarginLeft], layout->margins[QStyleSheetItem::MarginFirstLine],
                                 layout->margins[QStyleSheetItem::MarginRight], lst.first()->rtl());
    }
}

void KPrCanvas::setTextDepthMinus()
{
    QPtrList<KoTextFormatInterface> lst = applicableTextInterfaces();
    if ( lst.isEmpty() ) return;
    double leftMargin=0.0;
    if(!lst.isEmpty())
        leftMargin=lst.first()->currentParagLayoutFormat()->margins[QStyleSheetItem::MarginLeft];
    double indent = m_view->kPresenterDoc()->getIndentValue();
    QPtrListIterator<KoTextFormatInterface> it( lst );
    double newVal = leftMargin - indent;
    KMacroCommand* macroCmd = 0L;
    for ( ; it.current() ; ++it ) {
        KCommand* cmd = it.current()->setMarginCommand(QStyleSheetItem::MarginLeft, QMAX( newVal, 0 ));
        if ( cmd )
        {
            if ( !macroCmd )
                macroCmd = new KMacroCommand( i18n("Decrease Paragraph Depth") );
            macroCmd->addCommand( cmd );
        }
    }
    if (macroCmd )
        m_view->kPresenterDoc()->addCommand( macroCmd );
    if(!lst.isEmpty())
    {
        const KoParagLayout *layout=lst.first()->currentParagLayoutFormat();
        m_view->showRulerIndent( layout->margins[QStyleSheetItem::MarginLeft], layout->margins[QStyleSheetItem::MarginFirstLine],
                                 layout->margins[QStyleSheetItem::MarginRight], lst.first()->rtl());
    }
}

void KPrCanvas::setNewFirstIndent(double _firstIndent)
{
    QPtrList<KoTextFormatInterface> lst = applicableTextInterfaces();
    if ( lst.isEmpty() ) return;
    QPtrListIterator<KoTextFormatInterface> it( lst );
    KMacroCommand* macroCmd = 0L;
    for ( ; it.current() ; ++it ) {
        KCommand* cmd = it.current()->setMarginCommand(QStyleSheetItem::MarginFirstLine, _firstIndent);
        if ( cmd )
        {
            if ( !macroCmd )
                macroCmd = new KMacroCommand( i18n("Change First Line Indent") );
            macroCmd->addCommand( cmd );
        }
    }
    if ( macroCmd )
        m_view->kPresenterDoc()->addCommand( macroCmd );
}

void KPrCanvas::setNewLeftIndent(double _leftIndent)
{
    QPtrList<KoTextFormatInterface> lst = applicableTextInterfaces();
    if ( lst.isEmpty() ) return;
    QPtrListIterator<KoTextFormatInterface> it( lst );
    KMacroCommand* macroCmd = 0L;
    for ( ; it.current() ; ++it ) {
        KCommand* cmd = it.current()->setMarginCommand(QStyleSheetItem::MarginLeft, _leftIndent);
        if ( cmd )
        {
            if ( !macroCmd )
                macroCmd = new KMacroCommand( i18n("Change Left Indent") );
            macroCmd->addCommand( cmd );
        }
    }
    if ( macroCmd )
        m_view->kPresenterDoc()->addCommand( macroCmd );
}

void KPrCanvas::setNewRightIndent(double _rightIndent)
{
    QPtrList<KoTextFormatInterface> lst = applicableTextInterfaces();
    if ( lst.isEmpty() ) return;
    QPtrListIterator<KoTextFormatInterface> it( lst );
    KMacroCommand* macroCmd = 0L;
    for ( ; it.current() ; ++it ) {
        KCommand* cmd = it.current()->setMarginCommand(QStyleSheetItem::MarginRight, _rightIndent);
        if ( cmd )
        {
            if ( !macroCmd )
                macroCmd = new KMacroCommand( i18n("Change Right Indent") );
            macroCmd->addCommand( cmd );
        }
    }
    if ( macroCmd )
        m_view->kPresenterDoc()->addCommand( macroCmd );
}

/*void KPrCanvas::setTextCounter(KoParagCounter counter)
{
    QPtrList<KoTextFormatInterface> lst = applicableTextInterfaces();
    if ( lst.isEmpty() ) return;
    QPtrListIterator<KoTextFormatInterface> it( lst );
    KMacroCommand* macroCmd = 0L;
    for ( ; it.current() ; ++it ) {
        KCommand* cmd = it.current()->setCounterCommand(counter );
        if ( cmd )
        {
            if ( !macroCmd )
                macroCmd = new KMacroCommand( i18n("Change Paragraph Type") );
            macroCmd->addCommand( cmd );
        }
    }
    if ( macroCmd )
        m_view->kPresenterDoc()->addCommand( macroCmd );
}*/

#ifndef NDEBUG
void KPrCanvas::printRTDebug( int info )
{
    KPTextObject *kpTxtObj = 0;
    if ( m_currentTextObjectView )
        kpTxtObj = m_currentTextObjectView->kpTextObject();
    else
        kpTxtObj = selectedTextObjs().first();
    if ( kpTxtObj )
        kpTxtObj->textObject()->printRTDebug( info );
}
#endif

bool KPrCanvas::haveASelectedPartObj() const
{
    return m_activePage->haveASelectedPartObj() ||
        stickyPage()->haveASelectedPartObj();
}

bool KPrCanvas::haveASelectedGroupObj() const
{
    return m_activePage->haveASelectedGroupObj() ||
        stickyPage()->haveASelectedGroupObj();
}

bool KPrCanvas::haveASelectedPixmapObj() const
{
    return m_activePage->haveASelectedPixmapObj() ||
        stickyPage()->haveASelectedPixmapObj();
}

QPtrList<KPTextObject> KPrCanvas::applicableTextObjects() const
{
    QPtrList<KPTextObject> lst;
    // If we're editing a text object, then that's the one we return
    if ( m_currentTextObjectView )
        lst.append( m_currentTextObjectView->kpTextObject() );
    else
        lst = selectedTextObjs();
    return lst;
}

QPtrList<KoTextFormatInterface> KPrCanvas::applicableTextInterfaces() const
{
    QPtrList<KoTextFormatInterface> lst;
    QPtrList<KPObject> lstObj;
    // If we're editing a text object, then that's the one we return
    if ( m_currentTextObjectView )
    {
        if ( !m_currentTextObjectView->kpTextObject()->isProtectContent())
            lst.append( m_currentTextObjectView );
    }
    else
    {
        m_activePage->getAllObjectSelectedList(lstObj);
        QPtrListIterator<KPObject> it(lstObj);
        for ( ; it.current(); ++it ) {
            if ( it.current()->getType() == OT_TEXT )
            {
                KPTextObject * obj = static_cast<KPTextObject*>( it.current() );
                if ( !obj->isProtectContent() )
                    lst.append( obj->textObject() );
            }
        }
        //get sticky obj
        lstObj.clear();
        stickyPage()->getAllObjectSelectedList(lstObj);
        it=QPtrListIterator<KPObject>( lstObj );
        for ( ; it.current(); ++it ) {
            if ( it.current()->getType() == OT_TEXT )
            {
                KPTextObject * obj = static_cast<KPTextObject*>( it.current() );
                if ( !obj->isProtectContent() )
                    lst.append( obj->textObject() );
            }
        }
    }
    return lst;
}

QPtrList<KPTextObject> KPrCanvas::selectedTextObjs() const
{
    QPtrList<KPTextObject> lst;
    QPtrListIterator<KPObject> it(getObjectList());
    for ( ; it.current(); ++it ) {
        if ( it.current()->isSelected() && it.current()->getType() == OT_TEXT )
            lst.append( static_cast<KPTextObject*>( it.current() ) );
    }
    //get sticky obj
    it=stickyPage()->objectList();
    for ( ; it.current(); ++it ) {
        if ( it.current()->isSelected() && it.current()->getType() == OT_TEXT )
            lst.append( static_cast<KPTextObject*>( it.current() ));
    }
    return lst;
}

void KPrCanvas::startScreenPresentation( float presFakt, int curPgNum /* 1-based */)
{
    _presFakt = presFakt;
    //kdDebug(33001) << "KPrCanvas::startScreenPresentation curPgNum=" << curPgNum << endl;
    //kdDebug(33001) << "                              _presFakt=" << _presFakt << endl;

    presMenu->setItemChecked( PM_SM, true );
    presMenu->setItemChecked( PM_DM, false );

    setCursor( waitCursor );
    tmpObjs.clear();

    exitEditMode();

    //kdDebug(33001) << "Page::startScreenPresentation Zooming backgrounds" << endl;
    // Zoom backgrounds to the correct size for full screen
    KPresenterDoc * doc = m_view->kPresenterDoc();
    m_activePageBeforePresentation = doc->activePage();
    doc->displayActivePage( doc->pageList().at( curPgNum-1 ) );

    m_zoomBeforePresentation=doc->zoomHandler()->zoom();
    // ## TODO get rid of presFakt
    doc->zoomHandler()->setZoomAndResolution( qRound(_presFakt*m_zoomBeforePresentation),
                                              QPaintDevice::x11AppDpiX(), QPaintDevice::x11AppDpiY() );
    doc->newZoomAndResolution(false,false);

    QPtrListIterator<KPObject> oIt(doc->pageList().at( curPgNum-1 )->objectList());
    for (; oIt.current(); ++oIt )
        tmpObjs.append( oIt.current() );

    // add all selected slides
    slideList.clear();
    QValueList<int> selected = doc->selectedSlides();
    for ( QValueList<int>::Iterator it = selected.begin() ; it != selected.end(); ++ it )
    {
        // ARGLLLRGLRLGRLG selectedSlides gets us 0-based numbers,
        // and here we want 1-based numbers !
        int slideno = *it + 1;
        slideList.append( slideno );
    }

    // no slide selected ? end the slide show immediately...
    if( !slideList.count() )
    {
        //kdDebug(33001) << "No slide: end the slide show" << endl;
        stopScreenPresentation();
        return;
    }

    // find first selected slide after curPgNum
    unsigned slide = 0;
    for( unsigned i = 0; i<slideList.count(); i++ )
        if( slideList[i] >= curPgNum )
        {
            slide = slideList[i];
            break;
        }

    setCursor( blankCursor );

    currPresPage = (unsigned int) -1; // force gotoPage to do something
    gotoPage( slide );
    //kdDebug(33001) << "Page::startScreenPresentation - done" << endl;
}

void KPrCanvas::stopScreenPresentation()
{
    //kdDebug(33001) << "KPrCanvas::stopScreenPresentation m_showOnlyPage=" << m_showOnlyPage << endl;
    setCursor( waitCursor );

    KPresenterDoc * doc = m_view->kPresenterDoc();
    doc->displayActivePage( m_activePageBeforePresentation );
    _presFakt = 1.0;
    doc->zoomHandler()->setZoomAndResolution( m_zoomBeforePresentation,
                                              QPaintDevice::x11AppDpiX(), QPaintDevice::x11AppDpiY() );
    doc->newZoomAndResolution(false,false);
    goingBack = false;
    currPresPage = 1;
    editMode = true;
    drawMode = false;
    repaint( false );
    setToolEditMode( toolEditMode );
    tmpObjs.clear();
    setWFlags( WResizeNoErase );
}

bool KPrCanvas::pNext( bool )
{
    //bool clearSubPres = false;

    goingBack = false;

    // clear drawed lines
    m_drawModeLines.clear();

    //kdDebug(33001) << "\n-------\nKPrCanvas::pNext currPresStep=" << currPresStep << " subPresStep=" << subPresStep << endl;

    // First try to go one sub-step further, if any object requires it
    QPtrListIterator<KPObject> oit(getObjectList());
    for ( int i = 0 ; oit.current(); ++oit, ++i )
    {
        KPObject *kpobject = oit.current();
        if ( kpobject->getPresNum() == static_cast<int>( currPresStep )
             && kpobject->getType() == OT_TEXT && kpobject->getEffect2() != EF2_NONE )
        {
            if ( static_cast<int>( subPresStep + 1 ) < kpobject->getSubPresSteps() )
            {
                //kdDebug(33001) << "Page::pNext addSubPres subPresStep is now " << subPresStep+1 << endl;
                subPresStep++;
                doObjEffects();
                return false;
            }
        }
    }

    // Then try to see if there is still one step to do in the current page
    if ( (int)currPresStep < *( --presStepList.end() ) )
    {
        QValueList<int>::ConstIterator it = presStepList.find( currPresStep );
        currPresStep = *( ++it );
        subPresStep = 0;
        //kdDebug(33001) << "Page::pNext setting currPresStep to " << currPresStep << endl;

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
    QValueList<int>::ConstIterator test(  slideListIterator );
    if ( ++test != slideList.end() )
    {
        KPresenterDoc * doc = m_view->kPresenterDoc();
        if ( !spManualSwitch() && nextPageTimer ) {
            QValueList<int>::ConstIterator it( slideListIterator );
            m_view->setCurrentTimer( doc->pageList().at((*it) - 1 )->getPageTimer() / doc->getPresSpeed() );

            nextPageTimer = false;

            return false;
        }

#if KDE_IS_VERSION(3,1,90)
        QRect desk = KGlobalSettings::desktopGeometry(this);
#else
        QRect desk = QApplication::desktop()->screenGeometry(this);
#endif
        QPixmap _pix1( desk.width(), desk.height() );
        drawCurrentPageInPix( _pix1 );

        m_view->setPresentationDuration( currPresPage - 1 );

        currPresPage = *( ++slideListIterator );
        subPresStep = 0;
        //kdDebug(33001) << "Page::pNext going to page " << currPresPage << endl;

        tmpObjs.clear();

        doc->displayActivePage( doc->pageList().at( currPresPage-1 ) );

        setActivePage(doc->pageList().at(currPresPage-1));
        QPtrListIterator<KPObject> oIt( getObjectList() );
        for (; oIt.current(); ++oIt )
            tmpObjs.append(oIt.current());

        presStepList = doc->reorderPage( currPresPage-1 );
        currPresStep = *presStepList.begin();

#if KDE_IS_VERSION(3,1,90)
        QPixmap _pix2( desk.width(), desk.height() );
#else
        QPixmap _pix2( QApplication::desktop()->width(), QApplication::desktop()->height() );
#endif
        int pageHeight = doc->pageList().at(currPresPage-1)->getZoomPageRect().height();
        int yOffset = ( presPage() - 1 ) * pageHeight;
        if ( height() > pageHeight )
            yOffset -= ( height() - pageHeight ) / 2;
        drawCurrentPageInPix( _pix2 );

        QValueList<int>::ConstIterator it( slideListIterator );
        --it;

        if ( !spManualSwitch() )
            m_view->autoScreenPresStopTimer();

        KPBackGround * backtmp=doc->pageList().at( ( *it ) - 1 )->background();
        PageEffect _pageEffect = backtmp->getPageEffect();

        bool _soundEffect = backtmp->getPageSoundEffect();
        QString _soundFileName = backtmp->getPageSoundFileName();

        if ( _pageEffect != PEF_NONE && _soundEffect && !_soundFileName.isEmpty() ) {
            stopSound();
            playSound( _soundFileName );
        }

        kPchangePages( this, _pix1, _pix2, _pageEffect, pageSpeedFakt() );


        if ( !spManualSwitch() )
            m_view->autoScreenPresReStartTimer();

        return true;
    }

    //kdDebug(33001) << "Page::pNext last slide -> End of presentation" << endl;

    // When we are in manual mode or in automatic mode with no infinite loop
    // we display the 'End of presentation' slide.
    if ( ( spManualSwitch() || !spInfiniteLoop() ) && !showingLastSlide )
    {
        m_view->setPresentationDuration( currPresPage - 1 );

#if KDE_IS_VERSION(3,1,90)
        QRect desk = KGlobalSettings::desktopGeometry(this);
#else
        QRect desk = QApplication::desktop()->screenGeometry(this);
#endif
        QPixmap lastSlide( desk.width(), desk.height() );
        QFont font( m_view->kPresenterDoc()->defaultFont().family() );
        QPainter p( &lastSlide );

        p.setFont( font );
        p.setPen( white );
        p.fillRect( p.viewport(), black );
        p.drawText( 50, 50, i18n( "End of presentation. Click to exit." ) );
        bitBlt( this, 0, 0, &lastSlide, 0, 0, lastSlide.width(), lastSlide.height() );
        showingLastSlide = true;
        emit stopPres(); // no automatic mode for last slide
    }
    else if ( showingLastSlide ) // after last slide stop presentation
    {
        showingLastSlide = false;
        m_view->screenStop();
    }
    else
    {
        m_view->setPresentationDuration( currPresPage - 1 );
        emit stopPres(); // tells automatic mode to restart
    }

    return false;
}

bool KPrCanvas::pPrev( bool /*manual*/ )
{
    goingBack = true;
    subPresStep = 0;

    // clear drawed lines
    m_drawModeLines.clear();

    if ( (int)currPresStep > *presStepList.begin() ) {
        QValueList<int>::ConstIterator it = presStepList.find( currPresStep );
        currPresStep = *( --it );
        repaint( false );
        return false;
    } else {
        if ( slideListIterator == slideList.begin() ) {
            presStepList = m_view->kPresenterDoc()->reorderPage( currPresPage - 1 );
            currPresStep = *presStepList.begin();
            repaint( false );
            return false;
        }
        m_view->setPresentationDuration( currPresPage - 1);

        currPresPage = *( --slideListIterator );

        tmpObjs.clear();
        KPresenterDoc * doc = m_view->kPresenterDoc();
        doc->displayActivePage( doc->pageList().at( currPresPage-1 ) );

        //change active page.
        setActivePage(doc->pageList().at( currPresPage - 1 ) );
        QPtrListIterator<KPObject> oIt( getObjectList() );
        for (; oIt.current(); ++oIt )
            tmpObjs.append(oIt.current());
        presStepList = doc->reorderPage( currPresPage - 1 );
        currPresStep = *( --presStepList.end() );

        return true;
    }

    return false;
}

bool KPrCanvas::canAssignEffect( QPtrList<KPObject> &objs ) const
{
    QPtrListIterator<KPObject> oIt( m_activePage->objectList() );
    for (; oIt.current(); ++oIt )
        if ( oIt.current()->isSelected() )
            objs.append( oIt.current() );
    oIt = stickyPage()->objectList();
    for (; oIt.current(); ++oIt )
    {
        //can't assign a effect to header/footer
        if(m_view->kPresenterDoc()->isHeaderFooter(oIt.current()))
            continue;
        if ( oIt.current()->isSelected() )
            objs.append( oIt.current() );
    }
    return !objs.isEmpty();
}

bool KPrCanvas::isOneObjectSelected() const
{
    bool state=m_activePage->isOneObjectSelected();
    if( state)
        return true;
    return stickyPage()->isOneObjectSelected();
}

// This one is used to generate the pixmaps for the HTML presentation,
// for the pres-structure-dialog, for the sidebar previews, for template icons.
// Set forceWidth and/or forceHeight to override the zoom factor
// and obtain a pixmap of the specified width and/or height.
// By omitting one of them you make sure that the aspect ratio
// of your page is used for the resulting image.
void KPrCanvas::drawPageInPix( QPixmap &_pix, int pgnum, int zoom,
                               bool forceRealVariableValue,
                               int forceWidth,
                               int forceHeight )
{
    //kdDebug(33001) << "Page::drawPageInPix" << endl;
    currPresPage = pgnum + 1;

    KPresenterDoc *doc = m_view->kPresenterDoc();
    int oldZoom = doc->zoomHandler()->zoom();
    bool oldDisplayFieldValue = false;

    if( 0 < forceWidth || 0 < forceHeight )
    {
        const QRect rect( doc->getPageRect( true ) );
        const double dRectHeight = static_cast<double>(rect.height());
        const double dRectWidth  = static_cast<double>(rect.width());
        double dForceHeight      = static_cast<double>(forceHeight);
        double dForceWidth       = static_cast<double>(forceWidth);

        // adjust width or height, in case one of them is missing
        if( 0 >= forceWidth )
            dForceWidth = dForceHeight * dRectWidth / dRectHeight;
        else if( 0 >= forceHeight )
            dForceHeight = dForceWidth * dRectHeight / dRectWidth;

        // set the stretching values
        doc->zoomHandler()->setResolution( dForceWidth / dRectWidth,
                                           dForceHeight / dRectHeight );
        // As of yet (Feb. 2004) the following call results
        // in a NOP but be prepared for the future...
        doc->newZoomAndResolution( false, false );
    }else{
        m_view->zoomDocument(zoom);
    }

    if ( forceRealVariableValue )
    {
        oldDisplayFieldValue = m_view->kPresenterDoc()->getVariableCollection()->variableSetting()->displayFieldCode();
        if ( oldDisplayFieldValue )
        {
            m_view->kPresenterDoc()->getVariableCollection()->variableSetting()->setDisplayFieldCode(false);
            m_view->kPresenterDoc()->recalcVariables( VT_ALL );
        }
    }

    QRect rect = m_view->kPresenterDoc()->pageList().at(pgnum)->getZoomPageRect( );
    _pix.resize( rect.size() );
    _pix.fill( Qt::white );

    QPainter p;
    p.begin( &_pix );

    bool _editMode = editMode;
    editMode = false;

    drawBackground( &p, _pix.rect() );

    //objects in current page
    QPtrList<KPObject> _list = m_view->kPresenterDoc()->pageList().at( currPresPage-1 )->objectList();

    // check if object is selected, if so put it on the right place for the output
    if( _list.count() > 1 && (int)_list.count() > selectedObjectPosition && selectedObjectPosition >= 0) {
        _list.setAutoDelete( false );
        KPObject *kpobject = _list.last();
        if ( kpobject->isSelected() ) {
            _list.take( _list.count() - 1 );
            _list.insert( selectedObjectPosition, kpobject );
        }
    }

    drawAllObjectsInPage( &p, _list );

    //draw sticky object
    //the numbers for the sticky page have to be recalculated
    KPrPage* saveActivePage = m_activePage;
    doc->displayActivePage( doc->pageList().at( currPresPage-1 ) );
    setActivePage(doc->pageList().at( currPresPage - 1 ) );
    //setActivePage(m_view->kPresenterDoc()->pageList().at(currPresPage-1));
    drawAllObjectsInPage( &p, stickyPage()->objectList() );
    setActivePage( saveActivePage );

    editMode = _editMode;
    p.end();

    if ( forceRealVariableValue )
    {
        if ( oldDisplayFieldValue )
        {
            m_view->kPresenterDoc()->getVariableCollection()->variableSetting()->setDisplayFieldCode(true);
            m_view->kPresenterDoc()->recalcVariables(  VT_ALL );
        }
    }

    m_view->zoomDocument(oldZoom);
}

// This one is used in fullscreenmode, to generate the pixmaps used for the
// page effects.
void KPrCanvas::drawCurrentPageInPix( QPixmap &_pix ) const
{
    //kdDebug(33001) << "Page::drawCurrentPageInPix" << endl;

    _pix.fill(Qt::black);      // avoid garbage on "weird" DPIs

    QPainter p;
    p.begin( &_pix );

    drawBackground( &p, _pix.rect() );
    drawObjects( &p, _pix.rect(), false/*no cursor*/, SM_NONE, true/*obj-specific effects*/ );

    p.end();
}

void KPrCanvas::printPage( QPainter* painter, int pageNum )
{
    //kdDebug(33001) << "KPrCanvas::printPage" << endl;
    KPrPage* saveActivePage = m_activePage;
    KPresenterDoc *doc = m_view->kPresenterDoc();
    KPrPage* page = doc->pageList().at( pageNum );
    QRect rect = page->getZoomPageRect();
    doc->displayActivePage( page );
    setActivePage( page );
    drawBackground( painter, rect );
    drawObjects( painter, rect, false, SM_NONE, false/*no specific effects*/ );
    setActivePage( saveActivePage );
}

void KPrCanvas::doObjEffects()
{
    /// ### Note: this is for full-screen mode only.
    /// ### There should be NO use of diffx(), diffy() anywhere in this method!

    QPixmap screen_orig( kapp->desktop()->width(), kapp->desktop()->height() );
    bool drawn = false;

    // YABADABADOOOOOOO.... That's a hack :-)
    if ( subPresStep == 0 && currPresPage > 0 )
    {
        //kdDebug(33001) << "Page::doObjEffects - in the strange hack" << endl;
        inEffect = true;
        QPainter p;
        p.begin( &screen_orig );
        QRect desktopRect = QRect( 0, 0, kapp->desktop()->width(), kapp->desktop()->height() );
        drawBackground( &p, desktopRect );
        drawObjects( &p, desktopRect, false, SM_NONE, true );
        p.end();
        inEffect = false;
        bitBlt( this, 0, 0, &screen_orig, 0, 0, screen_orig.width(), screen_orig.height() );
        drawn = true;
    }

    QPtrList<KPObject> _objList;
    QTime _time;
    int _step = 0, _steps1 = 0, _steps2 = 0, x_pos1 = 0, y_pos1 = 0;
    int x_pos2 = kapp->desktop()->width(), y_pos2 = kapp->desktop()->height(), _step_width = 0, _step_height = 0;
    int w_pos1 = 0, h_pos1;
    bool effects = false;
    bool nothingHappens = false;
    int timer = 0;
    bool _soundEffect = false;
    QString _soundFileName = QString::null;
    if ( !drawn )
        bitBlt( &screen_orig, 0, 0, this, 0, 0, kapp->desktop()->width(), kapp->desktop()->height() );
    QPixmap *screen = new QPixmap( screen_orig );

    QPtrListIterator<KPObject> oit(getObjectList());
    for ( int i = 0 ; oit.current(); ++oit, ++i )
    {
        KPObject *kpobject = oit.current();
        if (  kpobject->getPresNum() == static_cast<int>( currPresStep ) )
        {
            if ( !spManualSwitch() )
                timer = kpobject->getAppearTimer();

            if ( kpobject->getEffect() != EF_NONE )
            {
                _soundEffect = kpobject->getAppearSoundEffect();
                _soundFileName = kpobject->getAppearSoundEffectFileName();
                _objList.append( kpobject );

                QRect br = m_view->zoomHandler()->zoomRect( kpobject->getBoundingRect() );
                int x = br.x();
                int y = br.y();
                int w = br.width();
                int h = br.height();

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
        else if (  kpobject->getDisappear() && kpobject->getDisappearNum() == static_cast<int>( currPresStep ) )
        {
            if ( !spManualSwitch() )
                timer = kpobject->getDisappearTimer();

            if ( kpobject->getEffect3() != EF3_NONE )
            {
                _soundEffect = kpobject->getDisappearSoundEffect();
                _soundFileName = kpobject->getDisappearSoundEffectFileName();

                _objList.append( kpobject );

                int x = 0, y = 0, w = 0, h = 0;
                QRect br = m_view->zoomHandler()->zoomRect( kpobject->getBoundingRect() );
                x = br.x(); y = br.y(); w = br.width(); h = br.height();

                switch ( kpobject->getEffect3() )
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
        if ( !spManualSwitch() && timer > 0 )
            m_view->autoScreenPresStopTimer();

        if ( _soundEffect && !_soundFileName.isEmpty() ) {
            stopSound();
            playSound( _soundFileName );
        }

        _step_width = static_cast<int>( ( static_cast<float>( kapp->desktop()->width() ) / objSpeedFakt() ) );
        _step_height = static_cast<int>( ( static_cast<float>( kapp->desktop()->height() ) / objSpeedFakt() ) );
        _steps1 = x_pos1 > y_pos1 ? x_pos1 / _step_width : y_pos1 / _step_height;
        _steps2 = kapp->desktop()->width() - x_pos2 > kapp->desktop()->height() - y_pos2 ?
                  ( kapp->desktop()->width() - x_pos2 ) / _step_width : ( kapp->desktop()->height() - y_pos2 ) / _step_height;
        _time.start();

        QPtrList<QRect> xy;
        xy.setAutoDelete( true );

        for ( ; ; )
        {
            kapp->processEvents();
            if ( nothingHappens ) break; // || _step >= _steps1 && _step >= _steps2 ) break;

            QPtrList<QRect> changes;
            changes.setAutoDelete( true );

            if ( _time.elapsed() >= 1 )
            {
                nothingHappens = true;
                _step++;

                changes.clear();

                for ( int i = 0; i < static_cast<int>( _objList.count() ); i++ )
                {
                    KPObject * kpobject = _objList.at( i );
                    // Origin of the object, in pixels
                    QPoint objectOrig = m_view->zoomHandler()->zoomPoint( kpobject->getOrig() );
                    // Distance from object to bottom right position of the screen...
                    int _w =  kapp->desktop()->width() - ( objectOrig.x() /*- diffx()*/ );
                    int _h =  kapp->desktop()->height() - ( objectOrig.y() /*- diffy()*/ );
                    QRect objectRect = m_view->zoomHandler()->zoomRect( kpobject->getRect() );
                    int ox = objectRect.x();
                    int oy = objectRect.y();
                    int ow = objectRect.width();
                    int oh = objectRect.height();

                    QRect br = m_view->zoomHandler()->zoomRect( kpobject->getBoundingRect() );
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
                        oldRect.setRect( bx - ( diffx() - xy.at( i )->x() ), by - ( diffy() - xy.at( i )->y() ),
                                         bw - xy.at( i )->width(), bh - xy.at( i )->height() );

                    ObjType objectType = kpobject->getType();
                    Effect2 effect2 = kpobject->getEffect2();

                    if ( !kpobject->getDisappear() || kpobject->getDisappear()
                         && kpobject->getDisappearNum() != static_cast<int>( currPresStep ) )
                    {
                        switch ( kpobject->getEffect() )
                        {
                        case EF_NONE:
                        {
                            if ( subPresStep == 0 || subPresStep != 0 && objectType == OT_TEXT
                                 && effect2 == EF2T_PARA )
                                drawObject( kpobject, screen, ox, oy, 0, 0, 0, 0 );
                        } break;
                        case EF_COME_LEFT:
                        {
                            if ( subPresStep == 0 || subPresStep != 0 && objectType == OT_TEXT
                                 && effect2 == EF2T_PARA )
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
                            if ( subPresStep == 0 || subPresStep != 0 && objectType == OT_TEXT
                                 && effect2 == EF2T_PARA )
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
                            if ( subPresStep == 0 || subPresStep != 0 && objectType == OT_TEXT
                                 && effect2 == EF2T_PARA )
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
                            if ( subPresStep == 0 || subPresStep != 0 && objectType == OT_TEXT
                                 && effect2 == EF2T_PARA )
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
                            if ( subPresStep == 0 || subPresStep != 0
                                 && objectType == OT_TEXT && effect2 == EF2T_PARA )
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
                            if ( subPresStep == 0 || subPresStep != 0 && objectType == OT_TEXT
                                 && effect2 == EF2T_PARA )
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
                            if ( subPresStep == 0 || subPresStep != 0 && objectType == OT_TEXT
                                 && effect2 == EF2T_PARA )
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
                            if ( subPresStep == 0 || subPresStep != 0 && objectType == OT_TEXT
                                 && effect2 == EF2T_PARA )
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
                            if ( subPresStep == 0 || subPresStep != 0 && objectType == OT_TEXT
                                 && effect2 == EF2T_PARA )
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
                            if ( subPresStep == 0 || subPresStep != 0 && objectType == OT_TEXT
                                 && effect2 == EF2T_PARA )
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
                            if ( subPresStep == 0 || subPresStep != 0 && objectType == OT_TEXT
                                 && effect2 == EF2T_PARA )
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
                            if ( subPresStep == 0 || subPresStep != 0 && objectType == OT_TEXT
                                 && effect2 == EF2T_PARA )
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
                            case EF3_WIPE_LEFT:
                            {
                                if ( subPresStep == 0 || subPresStep != 0 && objectType == OT_TEXT &&
                                     effect2 == EF2T_PARA )
                                {
                                    w_pos1 = _step_width * ( _step - 1 );
                                    x_pos1 = w_pos1;
                                    drawObject( kpobject, screen, 0, 0, w_pos1, 0, x_pos1, 0 );
                                    if ( ( _step_width * ( _steps1 - _step ) ) != 0 ) nothingHappens = false;
                                    xy.at( i )->setX( x_pos1 );
                                    xy.at( i )->setY( 0 );
                                    xy.at( i )->setWidth( w_pos1 );
                                    xy.at( i )->setHeight( 0 );
                                }
                            } break;
                            case EF3_WIPE_RIGHT:
                            {
                                if ( subPresStep == 0 || subPresStep != 0 && objectType == OT_TEXT &&
                                     effect2 == EF2T_PARA )
                                {
                                    w_pos1 = _step_width * ( _step - 1 );
                                    drawObject( kpobject, screen, 0, 0, w_pos1, 0, 0, 0 );
                                    if ( ( _step_width * ( _steps1 - _step ) ) != 0 ) nothingHappens = false;
                                    xy.at( i )->setX( 0 );
                                    xy.at( i )->setY( 0 );
                                    xy.at( i )->setWidth( w_pos1 );
                                    xy.at( i )->setHeight( 0 );
                                }
                            } break;
                            case EF3_WIPE_TOP:
                            {
                                if ( subPresStep == 0 || subPresStep != 0 && objectType == OT_TEXT &&
                                     effect2 == EF2T_PARA )
                                {
                                    h_pos1 = _step_height * ( _step - 1 );
                                    y_pos1 = h_pos1;
                                    drawObject( kpobject, screen, 0, 0, 0, h_pos1, 0, y_pos1 );
                                    if ( ( _step_height * ( _steps1 - _step ) ) != 0 ) nothingHappens = false;
                                    xy.at( i )->setX( 0 );
                                    xy.at( i )->setY( y_pos1 );
                                    xy.at( i )->setWidth( 0 );
                                    xy.at( i )->setHeight( h_pos1 );
                                }
                            } break;
                            case EF3_WIPE_BOTTOM:
                            {
                                if ( subPresStep == 0 || subPresStep != 0 && objectType == OT_TEXT &&
                                     effect2 == EF2T_PARA )
                                {
                                    h_pos1 = _step_height * ( _step - 1 );
                                    drawObject( kpobject, screen, 0, 0, 0, h_pos1, 0, 0 );
                                    if ( ( _step_height * ( _steps1 - _step ) ) != 0 ) nothingHappens = false;
                                    xy.at( i )->setX( 0 );
                                    xy.at( i )->setY( 0 );
                                    xy.at( i )->setWidth( 0 );
                                    xy.at( i )->setHeight( h_pos1 );
                                }
                            } break;
                            default:
                                break;
                            }
                        }
                    }
                    newRect.setRect( bx - ( diffx() - xy.at( i )->x() ), by - ( diffy() - xy.at( i )->y() ),
                                     bw - xy.at( i )->width(), bh - xy.at( i )->height() );
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
                    bitBlt( this, changed->x(), changed->y(), screen, changed->x(), changed->y(),
                            changed->width(), changed->height() );
                }

                delete screen;
                screen = new QPixmap( screen_orig );

                _time.restart();
            }
        }
    }

    if ( !effects )
    {
        //kdDebug(33001) << "KPrCanvas::doObjEffects no effects" << endl;
        QPainter p;
        p.begin( this );
        p.drawPixmap( 0, 0, screen_orig );
        drawObjects( &p, QRect( 0, 0, kapp->desktop()->width(), kapp->desktop()->height() ),
                     false, SM_NONE, true );
        p.end();
    }
    else
    {
        //kdDebug(33001) << "KPrCanvas::doObjEffects effects" << endl;
        QPainter p;
        p.begin( screen );
        drawObjects( &p, QRect( 0, 0, kapp->desktop()->width(), kapp->desktop()->height() ),
                     false, SM_NONE, true );
        p.end();
        bitBlt( this, 0, 0, screen );
    }

    if ( !spManualSwitch() && timer > 0 )
        m_view->setCurrentTimer( timer );

    delete screen;
}

void KPrCanvas::drawObject( KPObject *kpobject, QPixmap *screen, int _x, int _y, int _w, int _h,
                            int _cx, int _cy )
{
    if ( kpobject->getDisappear() &&
         kpobject->getDisappearNum() < static_cast<int>( currPresStep ) )
        return;
    int ox, oy, ow, oh;
    KoRect br = kpobject->getBoundingRect();
    QRect brpix = m_view->zoomHandler()->zoomRect( br );
    ox = brpix.x(); oy = brpix.y(); ow = brpix.width(); oh = brpix.height();
    bool ownClipping = true;

    QPainter p;
    p.begin( screen );

    if ( _w != 0 || _h != 0 )
    {
        p.setClipping( true );
        p.setClipRect( ox + _cx, oy + _cy, ow - _w, oh - _h, QPainter::CoordPainter );
        ownClipping = false;
    }

    if ( !editMode && static_cast<int>( currPresStep ) == kpobject->getPresNum() && !goingBack )
    {
        kpobject->setSubPresStep( subPresStep );
        kpobject->doSpecificEffects( true );
        kpobject->setOwnClipping( ownClipping );
    }
    p.translate(_x,_y);
    kpobject->draw( &p, m_view->zoomHandler(), SM_NONE,
                    ( kpobject->isSelected()) && drawContour );
    kpobject->setSubPresStep( 0 );
    kpobject->doSpecificEffects( false );
    kpobject->setOwnClipping( true );

    KPObject *obj = 0;
    for ( unsigned int i = tmpObjs.findRef( kpobject ) +1 ; i < tmpObjs.count(); i++ ) {
        obj = tmpObjs.at( i );
        if ( kpobject->getBoundingRect().intersects( obj->getBoundingRect() )
             && obj->getPresNum() < static_cast<int>( currPresStep ) )
            obj->draw( &p, m_view->zoomHandler(), SM_NONE, (obj->isSelected()) && drawContour);
    }

    p.end();
}

void KPrCanvas::print( QPainter *painter, KPrinter *printer, float /*left_margin*/, float /*top_margin*/ )
{
    //deSelectAllObj(); // already done in KPresenterView::setupPrinter
    printer->setFullPage( true );
    int i = 0;

    repaint( false );
    kapp->processEvents();

    editMode = false;
    fillBlack = false;
    _presFakt = 1.0;

    //int _xOffset = diffx();
    //int _yOffset = diffy();

    currPresStep = 1000;
    subPresStep = 1000;

    //m_view->setDiffX( -static_cast<int>( MM_TO_POINT( left_margin ) ) );
    //m_view->setDiffY( -static_cast<int>( MM_TO_POINT( top_margin ) ) );

    QProgressDialog progress( i18n( "Printing..." ), i18n( "Cancel" ),
                              printer->toPage() - printer->fromPage() + 2, this );

    int j = 0;
    progress.setProgress( 0 );

    /*if ( printer->fromPage() > 1 )
      m_view->setDiffY( ( printer->fromPage() - 1 ) * ( getPageRect( 1, 1.0, false ).height() ) -
      (int)MM_TO_POINT( top_margin ) );*/
    QValueList<int> list=printer->pageList(); // 1-based
    QValueList<int>::iterator it;
    for( it=list.begin();it!=list.end();++it)
    {
        i=(*it);
        progress.setProgress( ++j );
        kapp->processEvents();

        if ( progress.wasCancelled() )
            break;

        currPresPage = i;
        if ( !list.isEmpty() && i > list.first() )
            printer->newPage();

        painter->resetXForm();
        painter->fillRect( m_activePage->getZoomPageRect(), white );

        printPage( painter, i - 1 );
        kapp->processEvents();

        painter->resetXForm();
        kapp->processEvents();

        /*m_view->setDiffY( i * ( getPageRect( 1, 1.0, false ).height() )
          - static_cast<int>( MM_TO_POINT( top_margin ) ) );*/
    }

    KConfig *config=KPresenterFactory::global()->config();
    config->setGroup("Misc");
    bool printNotes = config->readBoolEntry("PrintNotes", true);

    NoteBar *noteBar = m_view->getNoteBar();
    //don't print notes when there is no note to print or it's disabled
    if ( noteBar && !noteBar->getNotesTextForPrinting(list).isEmpty()
         && !progress.wasCancelled() && printNotes )
    {
        printer->newPage();
        painter->resetXForm();
        noteBar->printNotes( painter, printer, list );
        painter->resetXForm();
    }

    setToolEditMode( toolEditMode );
    //m_view->setDiffX( _xOffset );
    //m_view->setDiffY( _yOffset );

    progress.setProgress( printer->toPage() - printer->fromPage() + 2 );

    currPresPage = 1;
    currPresStep = 0;
    subPresStep = 0;
    _presFakt = 1.0;
    fillBlack = true;
    editMode = true;
    repaint( false );
}

KPTextObject* KPrCanvas::insertTextObject( const QRect& _r )
{
    QRect r(_r);
    r.moveBy(diffx(),diffy());
    KoRect rect=m_view->zoomHandler()->unzoomRect(r);
    KPTextObject* obj = m_activePage->insertTextObject( rect );
    selectObj( obj );
    return obj;
}

void KPrCanvas::insertLineH( const QRect& _r, bool rev )
{
    QRect r(_r);
    r.moveBy(diffx(),diffy());
    KoRect rect=m_view->zoomHandler()->unzoomRect(r);
    m_activePage->insertLine( rect, m_view->getPen(),
                              !rev ? m_view->getLineBegin() : m_view->getLineEnd(), !rev ? m_view->getLineEnd() : m_view->getLineBegin(),
                              LT_HORZ );
}

void KPrCanvas::insertLineV( const QRect &_r, bool rev )
{
    QRect r(_r);
    r.moveBy(diffx(),diffy());
    KoRect rect=m_view->zoomHandler()->unzoomRect(r);
    m_activePage->insertLine( rect, m_view->getPen(),
                              !rev ? m_view->getLineBegin() : m_view->getLineEnd(), !rev ? m_view->getLineEnd() : m_view->getLineBegin(),
                              LT_VERT );
}

void KPrCanvas::insertLineD1( const QRect &_r, bool rev )
{
    QRect r(_r);
    r.moveBy(diffx(),diffy());
    KoRect rect=m_view->zoomHandler()->unzoomRect(r);
    m_activePage->insertLine( rect, m_view->getPen(),
                              !rev ? m_view->getLineBegin() : m_view->getLineEnd(), !rev ? m_view->getLineEnd() : m_view->getLineBegin(),
                              LT_LU_RD );
}

void KPrCanvas::insertLineD2( const QRect &_r, bool rev )
{
    QRect r(_r);
    r.moveBy(diffx(),diffy());
    KoRect rect=m_view->zoomHandler()->unzoomRect(r);
    m_activePage->insertLine(rect, m_view->getPen(),
                             !rev ? m_view->getLineBegin() : m_view->getLineEnd(), !rev ? m_view->getLineEnd() : m_view->getLineBegin(),
                             LT_LD_RU );
}

void KPrCanvas::insertRect( const QRect& _r )
{
    QRect r(_r);
    r.moveBy(diffx(),diffy());
    KoRect rect=m_view->zoomHandler()->unzoomRect(r);
    m_activePage->insertRectangle( rect, m_view->getPen(), m_view->getBrush(), m_view->getFillType(),
                                   m_view->getGColor1(), m_view->getGColor2(), m_view->getGType(), m_view->getRndX(), m_view->getRndY(),
                                   m_view->getGUnbalanced(), m_view->getGXFactor(), m_view->getGYFactor() );
}

void KPrCanvas::insertEllipse( const QRect &_r )
{
    QRect r(_r);
    r.moveBy(diffx(),diffy());
    KoRect rect=m_view->zoomHandler()->unzoomRect(r);
    m_activePage->insertCircleOrEllipse( rect, m_view->getPen(), m_view->getBrush(), m_view->getFillType(),
                                         m_view->getGColor1(), m_view->getGColor2(),
                                         m_view->getGType(), m_view->getGUnbalanced(), m_view->getGXFactor(), m_view->getGYFactor() );
}

void KPrCanvas::insertPie( const QRect &_r )
{
    QRect r(_r);
    r.moveBy(diffx(),diffy());
    KoRect rect=m_view->zoomHandler()->unzoomRect(r);
    m_activePage->insertPie( rect, m_view->getPen(), m_view->getBrush(), m_view->getFillType(),
                             m_view->getGColor1(), m_view->getGColor2(), m_view->getGType(),
                             m_view->getPieType(), m_view->getPieAngle(), m_view->getPieLength(),
                             m_view->getLineBegin(), m_view->getLineEnd(), m_view->getGUnbalanced(), m_view->getGXFactor(),
                             m_view->getGYFactor() );
}

void KPrCanvas::insertAutoform( const QRect &_r, bool rev )
{
    rev = false;
    QRect r(_r);
    r.moveBy(diffx(),diffy());
    KoRect rect=m_view->zoomHandler()->unzoomRect(r);
    m_activePage->insertAutoform( rect, m_view->getPen(), m_view->getBrush(),
                                  !rev ? m_view->getLineBegin() : m_view->getLineEnd(), !rev ? m_view->getLineEnd() : m_view->getLineBegin(),
                                  m_view->getFillType(), m_view->getGColor1(), m_view->getGColor2(), m_view->getGType(),
                                  autoform, m_view->getGUnbalanced(), m_view->getGXFactor(), m_view->getGYFactor() );
}

KPPartObject* KPrCanvas::insertObject( const QRect &_r )
{
    QRect r(_r);
    r.moveBy(diffx(),diffy());
    KoRect rect=m_view->zoomHandler()->unzoomRect(r);
    KPPartObject *kpPartObject = m_activePage->insertObject( rect, partEntry );
    return kpPartObject;
}

void KPrCanvas::insertFreehand( const KoPointArray &_pointArray )
{
    KoRect rect = _pointArray.boundingRect();

    double ox = rect.x();
    double oy = rect.y();

    unsigned int index = 0;

    KoPointArray points( _pointArray );
    KoPointArray tmpPoints;
    KoPointArray::ConstIterator it;
    for ( it = points.begin(); it != points.end(); ++it ) {
        KoPoint point = (*it);
        double tmpX = point.x() - ox ;
        double tmpY = point.y() - oy ;
        tmpPoints.putPoints( index, 1, tmpX,tmpY );
        ++index;
    }
    rect.moveBy(m_view->zoomHandler()->unzoomItX(diffx()),m_view->zoomHandler()->unzoomItY(diffy()));
    m_activePage->insertFreehand( tmpPoints, rect, m_view->getPen(), m_view->getLineBegin(),
                                  m_view->getLineEnd() );

    m_pointArray = KoPointArray();
    m_indexPointArray = 0;
}

void KPrCanvas::insertPolyline( const KoPointArray &_pointArray )
{
    if( _pointArray.count()> 1)
    {
        KoRect rect = _pointArray.boundingRect();

        double ox = rect.x();
        double oy = rect.y();
        unsigned int index = 0;

        KoPointArray points( _pointArray );
        KoPointArray tmpPoints;
        KoPointArray::ConstIterator it;
        for ( it = points.begin(); it != points.end(); ++it ) {
            KoPoint point = (*it);
            double tmpX = point.x() - ox ;
            double tmpY = point.y() - oy ;
            tmpPoints.putPoints( index, 1, tmpX,tmpY );
            ++index;
        }
        rect.moveBy(m_view->zoomHandler()->unzoomItX(diffx()),m_view->zoomHandler()->unzoomItY(diffy()));
        m_activePage->insertPolyline( tmpPoints, rect, m_view->getPen(), m_view->getLineBegin(),
                                      m_view->getLineEnd() );
    }
    m_pointArray = KoPointArray();
    m_indexPointArray = 0;
}

void KPrCanvas::insertCubicBezierCurve( const KoPointArray &_pointArray )
{
    if( _pointArray.count()> 1)
    {

        KoPointArray _points( _pointArray );
        KoPointArray _allPoints;
        unsigned int pointCount = _points.count();
        KoRect _rect;

        if ( pointCount == 2 ) { // line
            _rect = _points.boundingRect();
            _allPoints = _points;
        }
        else { // cubic bezier curve
            KoPointArray tmpPointArray;
            unsigned int _tmpIndex = 0;
            unsigned int count = 0;
            while ( count < pointCount ) {
                if ( pointCount >= ( count + 4 ) ) { // for cubic bezier curve
                    double _firstX = _points.at( count ).x();
                    double _firstY = _points.at( count ).y();

                    double _fourthX = _points.at( count + 1 ).x();
                    double _fourthY = _points.at( count + 1 ).y();

                    double _secondX = _points.at( count + 2 ).x();
                    double _secondY = _points.at( count + 2 ).y();

                    double _thirdX = _points.at( count + 3 ).x();
                    double _thirdY = _points.at( count + 3 ).y();

                    KoPointArray _cubicBezierPoint;
                    _cubicBezierPoint.putPoints( 0, 4, _firstX,_firstY, _secondX,_secondY, _thirdX,_thirdY, _fourthX,_fourthY );

                    _cubicBezierPoint = _cubicBezierPoint.cubicBezier();

                    KoPointArray::ConstIterator it;
                    for ( it = _cubicBezierPoint.begin(); it != _cubicBezierPoint.end(); ++it ) {
                        KoPoint _point = (*it);
                        tmpPointArray.putPoints( _tmpIndex, 1, _point.x(), _point.y() );
                        ++_tmpIndex;
                    }

                    count += 4;
                }
                else { // for line
                    double _x1 = _points.at( count ).x();
                    double _y1 = _points.at( count ).y();

                    double _x2 = _points.at( count + 1 ).x();
                    double _y2 = _points.at( count + 1 ).y();

                    tmpPointArray.putPoints( _tmpIndex, 2, _x1,_y1, _x2,_y2 );
                    _tmpIndex += 2;
                    count += 2;
                }
            }

            _rect = tmpPointArray.boundingRect();
            _allPoints = tmpPointArray;
        }

        double ox = _rect.x();
        double oy = _rect.y();
        unsigned int index = 0;

        KoPointArray points( _pointArray );
        KoPointArray tmpPoints;
        KoPointArray::ConstIterator it;
        for ( it = points.begin(); it != points.end(); ++it ) {
            KoPoint point = (*it);
            double tmpX = point.x() - ox;
            double tmpY = point.y() - oy;
            tmpPoints.putPoints( index, 1, tmpX,tmpY );
            ++index;
        }

        index = 0;
        KoPointArray tmpAllPoints;
        for ( it = _allPoints.begin(); it != _allPoints.end(); ++it ) {
            KoPoint point = (*it);
            double tmpX = point.x() - ox ;
            double tmpY = point.y() - oy;
            tmpAllPoints.putPoints( index, 1, tmpX,tmpY );
            ++index;
        }
        _rect.moveBy(m_view->zoomHandler()->unzoomItX(diffx()),m_view->zoomHandler()->unzoomItY(diffy()));
        if ( toolEditMode == INS_CUBICBEZIERCURVE )
            m_activePage->insertCubicBezierCurve( tmpPoints, tmpAllPoints, _rect, m_view->getPen(),
                                                  m_view->getLineBegin(), m_view->getLineEnd() );
        else if ( toolEditMode == INS_QUADRICBEZIERCURVE )
            m_activePage->insertQuadricBezierCurve( tmpPoints, tmpAllPoints, _rect, m_view->getPen(),
                                                    m_view->getLineBegin(), m_view->getLineEnd() );
        else if ( toolEditMode == INS_CLOSED_CUBICBEZIERCURVE || toolEditMode == INS_CLOSED_QUADRICBEZIERCURVE )
            m_activePage->insertClosedLine( tmpAllPoints, _rect, m_view->getPen(), m_view->getBrush(), m_view->getFillType(),
                                            m_view->getGColor1(), m_view->getGColor2(), m_view->getGType(), m_view->getGUnbalanced(),
                                            m_view->getGXFactor(), m_view->getGYFactor(), toolEditMode );

    }
    m_pointArray = KoPointArray();
    m_indexPointArray = 0;
}

void KPrCanvas::insertPolygon( const KoPointArray &_pointArray )
{
    KoPointArray points( _pointArray );
    KoRect rect= points.boundingRect();
    double ox = rect.x();
    double oy = rect.y();
    unsigned int index = 0;

    KoPointArray tmpPoints;
    KoPointArray::ConstIterator it;
    for ( it = points.begin(); it != points.end(); ++it ) {
        KoPoint point = (*it);
        double tmpX = point.x() - ox;
        double tmpY = point.y() - oy;
        tmpPoints.putPoints( index, 1, tmpX,tmpY );
        ++index;
    }
    rect.moveBy(m_view->zoomHandler()->unzoomItX(diffx()),m_view->zoomHandler()->unzoomItY(diffy()));
    m_activePage->insertPolygon( tmpPoints, rect, m_view->getPen(), m_view->getBrush(), m_view->getFillType(),
                                 m_view->getGColor1(), m_view->getGColor2(), m_view->getGType(), m_view->getGUnbalanced(),
                                 m_view->getGXFactor(), m_view->getGYFactor(),
                                 m_view->getCheckConcavePolygon(), m_view->getCornersValue(), m_view->getSharpnessValue() );

    m_pointArray = KoPointArray();
    m_indexPointArray = 0;
}

void KPrCanvas::insertPicture( const QRect &_r, const QPoint & tl )
{
    QString file = m_activePage->insPictureFile();

    QCursor c = cursor();
    setCursor( waitCursor );
    if ( !file.isEmpty() ) {
        if (_r.isValid())
        {
            QRect r( _r );
            r.moveBy( diffx(), diffy() );
            KoRect rect = m_view->zoomHandler()->unzoomRect( r );
            m_activePage->insertPicture( file, rect );
        }
        else
        {
            QPoint topleft(tl);
            topleft += QPoint(diffx(), diffy());
            m_activePage->insertPicture(file, topleft.x(), topleft.y());
        }
        m_activePage->setInsPictureFile( QString::null );
    }
    setCursor( c );
}

void KPrCanvas::insertClosedLine( const KoPointArray &_pointArray )
{
    KoPointArray points( _pointArray );
    KoRect rect =  points.boundingRect();
    double ox = rect.x();
    double oy = rect.y();
    unsigned int index = 0;

    KoPointArray tmpPoints;
    KoPointArray::ConstIterator it;
    for ( it = points.begin(); it != points.end(); ++it ) {
        KoPoint point = (*it);
        double tmpX = point.x() - ox;
        double tmpY = point.y() - oy;
        tmpPoints.putPoints( index, 1, tmpX,tmpY );
        ++index;
    }
    rect.moveBy( m_view->zoomHandler()->unzoomItX( diffx() ), m_view->zoomHandler()->unzoomItY( diffy() ) );

    m_activePage->insertClosedLine( tmpPoints, rect, m_view->getPen(), m_view->getBrush(), m_view->getFillType(),
                                    m_view->getGColor1(), m_view->getGColor2(), m_view->getGType(), m_view->getGUnbalanced(),
                                    m_view->getGXFactor(), m_view->getGYFactor(), toolEditMode );

    m_pointArray = KoPointArray();
    m_indexPointArray = 0;
}

void KPrCanvas::setToolEditMode( ToolEditMode _m, bool updateView )
{
    //store m_pointArray if !m_pointArray.isNull()
    if ( ( toolEditMode == INS_POLYLINE || toolEditMode == INS_CLOSED_POLYLINE ) && !m_pointArray.isNull())
        endDrawPolyline();

    if ( ( toolEditMode == INS_CUBICBEZIERCURVE || toolEditMode == INS_QUADRICBEZIERCURVE
           || toolEditMode == INS_CLOSED_CUBICBEZIERCURVE || toolEditMode == INS_CLOSED_QUADRICBEZIERCURVE ) && !m_pointArray.isNull() )
        endDrawCubicBezierCurve();

    exitEditMode();
    toolEditMode = _m;

    if ( toolEditMode == TEM_MOUSE )
    {
        setCursor( arrowCursor );
        QPoint pos=QCursor::pos();
        KPObject *obj=m_activePage->getCursor( pos);
        if(obj)
            setCursor( obj->getCursor( pos, modType,m_view->kPresenterDoc() ) );
        else
        {
            obj=stickyPage()->getCursor( pos );
            if(obj)
                setCursor( obj->getCursor( pos, modType,m_view->kPresenterDoc() ) );
        }
    }
    else if ( toolEditMode == INS_FREEHAND || toolEditMode == INS_CLOSED_FREEHAND )
        setCursor( KPresenterUtils::penCursor() );
    else if ( toolEditMode == TEM_ROTATE )
        setCursor( KPresenterUtils::rotateCursor() );
    else
        setCursor( crossCursor );

    if ( updateView )
        m_view->setTool( toolEditMode );
    repaint();
}


void KPrCanvas::endDrawPolyline()
{
    m_drawPolyline = false;

    if ( toolEditMode == INS_POLYLINE )
        insertPolyline( m_pointArray );
    else if ( toolEditMode == INS_CLOSED_POLYLINE )
        insertClosedLine( m_pointArray );

    emit objectSelectedChanged();
    if ( toolEditMode != TEM_MOUSE && editMode )
        repaint( false );
    mousePressed = false;
    modType = MT_NONE;
    resizeObjNum = 0L;
    ratio = 0.0;
    keepRatio = false;
}

void KPrCanvas::endDrawCubicBezierCurve()
{
    m_drawCubicBezierCurve = false;
    m_oldCubicBezierPointArray = KoPointArray();
    insertCubicBezierCurve( m_pointArray );
    emit objectSelectedChanged();
    if ( toolEditMode != TEM_MOUSE && editMode )
        repaint( false );
    mousePressed = false;
    modType = MT_NONE;
    resizeObjNum = 0L;
    ratio = 0.0;
    keepRatio = false;
}

void KPrCanvas::selectNext()
{
    if ( objectList().count() == 0 ) return;

    if ( m_activePage->numSelected() == 0 )
        objectList().at( 0 )->setSelected( true );
    else {
        int i = objectList().findRef( m_activePage->getSelectedObj() );
        if ( i < static_cast<int>( objectList().count() ) - 1 ) {
            m_view->kPresenterDoc()->deSelectAllObj();
            objectList().at( ++i )->setSelected( true );
        } else {
            m_view->kPresenterDoc()->deSelectAllObj();
            objectList().at( 0 )->setSelected( true );
        }
    }
    QRect r = m_view->zoomHandler()->zoomRect( m_activePage->getSelectedObj()->getBoundingRect() );
    if ( !QRect( diffx(), diffy(), width(), height() ).contains( r ) )
        m_view->makeRectVisible( r );
    _repaint( false );
}

void KPrCanvas::selectPrev()
{
    if ( objectList().count() == 0 ) return;
    if ( m_activePage->numSelected() == 0 )
        objectList().at( objectList().count() - 1 )->setSelected( true );
    else {
        int i = objectList().findRef( m_activePage->getSelectedObj() );
        if ( i > 0 ) {
            m_view->kPresenterDoc()->deSelectAllObj();
            objectList().at( --i )->setSelected( true );
        } else {
            m_view->kPresenterDoc()->deSelectAllObj();
            objectList().at( objectList().count() - 1 )->setSelected( true );
        }
    }
    m_view->makeRectVisible( m_view->zoomHandler()->zoomRect(m_activePage->getSelectedObj()->getBoundingRect()) );
    _repaint( false );
}

void KPrCanvas::dragEnterEvent( QDragEnterEvent *e )
{
    if ( m_currentTextObjectView )
        m_currentTextObjectView->dragEnterEvent( e );
    else if ( QTextDrag::canDecode( e )
              || QImageDrag::canDecode( e )
              || KURLDrag::canDecode(e)) {
        e->accept();
    }
    else
        e->ignore();
}

void KPrCanvas::dragMoveEvent( QDragMoveEvent *e )
{
    if( m_currentTextObjectView)
    {
        KPTextObject * obj = textUnderMouse( e->pos());
        bool emitChanged = false;
        if ( obj )
            emitChanged = checkCurrentTextEdit( obj );
        if ( m_currentTextObjectView )
        {
            m_currentTextObjectView->dragMoveEvent( e, QPoint() );
            if ( emitChanged )
                emit currentObjectEditChanged();
        }
    }
    else if ( QTextDrag::canDecode( e )
              || QImageDrag::canDecode( e )
              || KURLDrag::canDecode(e)) {
        e->accept();
    }
    else
        e->ignore();
}

void KPrCanvas::dropImage( QMimeSource * data, bool resizeImageToOriginalSize, int posX, int posY )
{
    setToolEditMode( TEM_MOUSE );
    deSelectAllObj();

    QImage pix;
    QImageDrag::decode( data, pix );

    KTempFile tmpFile;
    tmpFile.setAutoDelete(true);

    if( tmpFile.status() != 0 )
        return;

    pix.save( tmpFile.name(), "PNG" );
    QCursor c = cursor();
    setCursor( waitCursor );
    m_activePage->insertPicture( tmpFile.name(), posX, posY  );

    tmpFile.close();

    if ( resizeImageToOriginalSize )
        picViewOriginalSize();
    setCursor( c );
}

void KPrCanvas::dropEvent( QDropEvent *e )
{
    //disallow dropping objects outside the "page"
    KoPoint docPoint = m_view->zoomHandler()->unzoomPoint( e->pos()+QPoint(diffx(),diffy()) );
    if ( !m_activePage->getZoomPageRect().contains(e->pos()))
        return;

    if ( QImageDrag::canDecode( e ) ) {
        dropImage( e, true, e->pos().x(), e->pos().y() );
        e->accept();
    } else if ( KURLDrag::canDecode( e ) ) {
        setToolEditMode( TEM_MOUSE );
        deSelectAllObj();

        KURL::List lst;
        KURLDrag::decode( e, lst );

        KURL::List::ConstIterator it = lst.begin();
        for ( ; it != lst.end(); ++it ) {
            const KURL &url( *it );

            QString filename;
            if ( !url.isLocalFile() ) {
                if ( !KIO::NetAccess::download( url, filename ) )
                    continue;
            } else {
                filename = url.path();
            }

            KMimeMagicResult *res = KMimeMagic::self()->findFileType( filename );

            if ( res && res->isValid() ) {
                QString mimetype = res->mimeType();
                if ( mimetype.contains( "image" ) ) {
                    QCursor c = cursor();
                    setCursor( waitCursor );
                    m_activePage->insertPicture( filename, e->pos().x(), e->pos().y()  );
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
                    m_activePage->insertTextObject( m_view->zoomHandler()->unzoomRect(QRect( e->pos().x(), e->pos().y(), 250, 250 )),
                                                    text, m_view );

                    setCursor( c );
                }
            }
            KIO::NetAccess::removeTempFile( filename );
        }
    }
    else if (m_currentTextObjectView)
    {
        m_currentTextObjectView->dropEvent( e );
    }
    else if ( QTextDrag::canDecode( e ) ) {
        setToolEditMode( TEM_MOUSE );
        deSelectAllObj();

        QString text;
        QTextDrag::decode( e, text );
        m_activePage->insertTextObject( m_view->zoomHandler()->unzoomRect( QRect( e->pos().x(), e->pos().y(), 250, 250 )),
                                        text, m_view );
        e->accept();
    } else
        e->ignore();

}

void KPrCanvas::slotGotoPage()
{
    setCursor( blankCursor );
    int pg = currPresPage;

    m_view->setPresentationDuration( pg - 1 );

    pg = KPGotoPage::gotoPage( m_view->kPresenterDoc(), slideList, pg, this );
    gotoPage( pg );

    if ( !spManualSwitch() ) {
        m_view->setCurrentTimer( 1 );
        setNextPageTimer( true );
    }

    if ( presMenu->isItemChecked ( PM_DM ) )
        setCursor( KPresenterUtils::penCursor() );
}

void KPrCanvas::gotoPage( int pg )
{
    if ( pg != static_cast<int>( currPresPage ) ) {
        // clear drawed lines
        m_drawModeLines.clear();

        currPresPage = pg;
        kdDebug(33001) << "Page::gotoPage currPresPage=" << currPresPage << endl;
        slideListIterator = slideList.find( currPresPage );
        editMode = false;
        drawMode = false;
        presStepList = m_view->kPresenterDoc()->reorderPage( currPresPage-1);
        currPresStep = *presStepList.begin();
        subPresStep = 0;
        //change active page
        m_activePage=m_view->kPresenterDoc()->pageList().at(currPresPage-1);
        //recalculate the page numbers
        m_view->kPresenterDoc()->recalcPageNum();

#if KDE_IS_VERSION(3,1,90)
        QRect desk = KGlobalSettings::desktopGeometry(this);
        resize( desk.width(), desk.height() );
#else
        resize( QApplication::desktop()->screenGeometry(this).size());
#endif
        repaint( false );
        setFocus();
        m_view->refreshPageButton();
    }
}

void KPrCanvas::presGotoFirstPage()
{
    gotoPage( *slideList.begin() );
}

KPTextObject* KPrCanvas::kpTxtObj() const
{
    return ( ( editNum && editNum->getType() == OT_TEXT ) ?
             dynamic_cast<KPTextObject*>( editNum ) : 0 );
    // ### return m_currentTextObjectView->kpTextObject()
}

void KPrCanvas::copyObjs()
{
    QDomDocument doc("DOC");
    QDomElement presenter=doc.createElement("DOC");
    presenter.setAttribute("editor", "KPresenter");
    presenter.setAttribute("mime", "application/x-kpresenter");

    doc.appendChild(presenter);

    QPtrList<KoDocumentChild> embeddedObjectsActivePage;
    QPtrList<KoDocumentChild> embeddedObjectsStickyPage;

    KoStoreDrag *kd = new KoStoreDrag( "application/x-kpresenter", 0L );
    QDragObject* dragObject = kd;
    QByteArray arr;
    QBuffer buffer(arr);
    KoStore* store = KoStore::createStore( &buffer, KoStore::Write, "application/x-kpresenter" );

    m_activePage->getAllEmbeddedObjectSelected(embeddedObjectsActivePage );
    stickyPage()->getAllEmbeddedObjectSelected(embeddedObjectsStickyPage );

    // Save internal embedded objects first, since it might change their URL
    int i = 0;
    QValueList<KoPictureKey> savePictures;
    QPtrListIterator<KoDocumentChild> chl( embeddedObjectsActivePage );
    for( ; chl.current(); ++chl ) {
        KoDocument* childDoc = chl.current()->document();
        if ( childDoc && !childDoc->isStoredExtern() )
            (void) childDoc->saveToStore( store, QString::number( i++ ) );
    }
    QPtrListIterator<KoDocumentChild> chl2( embeddedObjectsStickyPage );
    for( ; chl2.current(); ++chl2 ) {
        KoDocument* childDoc = chl2.current()->document();
        if ( childDoc && !childDoc->isStoredExtern() )
            (void) childDoc->saveToStore( store, QString::number( i++ ) );
    }

    m_activePage->copyObjs(doc, presenter, savePictures);
    stickyPage()->copyObjs(doc, presenter, savePictures);

    KPresenterDoc* kprdoc = m_view->kPresenterDoc();
    if ( !embeddedObjectsStickyPage.isEmpty() )
    {
        kprdoc->saveEmbeddedObject(stickyPage(), embeddedObjectsStickyPage, doc, presenter );
    }
    if ( !embeddedObjectsActivePage.isEmpty())
    {
        kprdoc->saveEmbeddedObject(m_activePage, embeddedObjectsActivePage,doc,presenter);
    }

    if ( !savePictures.isEmpty() ) {
        // Save picture list at the end of the main XML
        presenter.appendChild( kprdoc->pictureCollection()->saveXML( KoPictureCollection::CollectionPicture, doc, savePictures ) );
        // Save the actual picture data into the store
        kprdoc->pictureCollection()->saveToStore( KoPictureCollection::CollectionPicture, store, savePictures );
        // Single image -> put it in dragobject too
        if ( savePictures.count() == 1 )
        {
            KoPicture pic = kprdoc->pictureCollection()->findPicture( savePictures.first() );
            QDragObject* picDrag = pic.dragObject( 0L );
            if ( picDrag ) {
                KMultipleDrag* multipleDrag = new KMultipleDrag( 0L );
                multipleDrag->addDragObject( kd );
                multipleDrag->addDragObject( picDrag );
                dragObject = multipleDrag;
            }
        }
    }

    if ( store->open( "root" ) )
    {
        QCString s = doc.toCString(); // this is already Utf8!
        //kdDebug(33001) << "KPrCanvas::copyObject: " << s << endl;
        (void)store->write( s.data(), s.size()-1 );
        store->close();
    }

    delete store;
    kd->setEncodedData( arr );
    QApplication::clipboard()->setData( dragObject, QClipboard::Clipboard );
}

void KPrCanvas::deleteObjs()
{
    KMacroCommand *macro=0L;
    KCommand *cmd=m_activePage->deleteObjs();
    if( cmd)
    {
        if ( !macro )
            macro=new KMacroCommand(i18n( "Delete Objects" ));
        macro->addCommand(cmd);
    }
    cmd=stickyPage()->deleteObjs();
    if( cmd)
    {
        if ( !macro )
            macro=new KMacroCommand(i18n( "Delete Objects" ));
        macro->addCommand(cmd);
    }
    m_view->kPresenterDoc()->deSelectAllObj();
    if(macro)
        m_view->kPresenterDoc()->addCommand(macro);
    emit objectSelectedChanged();
    setToolEditMode( toolEditMode );
}

void KPrCanvas::rotateObjs()
{
    m_view->extraRotate();
    setToolEditMode( toolEditMode );
}

void KPrCanvas::shadowObjs()
{
    m_view->extraShadow();
    setToolEditMode( toolEditMode );
}

void KPrCanvas::enterEvent( QEvent *e )
{
    m_view->setRulerMousePos( ( ( QMouseEvent* )e )->x(), ( ( QMouseEvent* )e )->y() );
    m_view->setRulerMouseShow( true );
}

void KPrCanvas::leaveEvent( QEvent * /*e*/ )
{
    m_view->setRulerMouseShow( false );
}

QPtrList<KPObject> KPrCanvas::objectList() const
{
    return m_activePage->objectList();
}

const QPtrList<KPObject> &KPrCanvas::getObjectList() const
{
    return m_activePage->objectList();
}

unsigned int KPrCanvas::currPgNum() const
{
    return m_view->getCurrPgNum();
}

QColor KPrCanvas::txtBackCol() const
{
    return m_view->kPresenterDoc()->txtBackCol();
}

bool KPrCanvas::spInfiniteLoop() const
{
    return m_view->kPresenterDoc()->spInfiniteLoop();
}

bool KPrCanvas::spManualSwitch() const
{
    return m_view->kPresenterDoc()->spManualSwitch();
}

QRect KPrCanvas::getPageRect( bool decBorders ) const
{
    return m_view->kPresenterDoc()->getPageRect( decBorders );
}

unsigned int KPrCanvas::pageNums() const
{
    return m_view->kPresenterDoc()->getPageNums();
}

float KPrCanvas::objSpeedFakt() const
{
    /*
      Used to be 0(slow)->70, 1(medium)->50, 2(fast)->30.
      It's now 0->75, 1->50, 2->37, etc. That's the reason for this strange formula :)
    */
    return 150.0 / static_cast<float>( m_view->kPresenterDoc()->getPresSpeed() + 2 );
    //return ObjSpeed[ static_cast<int>( m_view->kPresenterDoc()->getPresSpeed() ) ];
}

float KPrCanvas::pageSpeedFakt() const
{
    /*
      Used to be 0(slow)->8, 1(medium)->16, 2(fast)->32.
      It's now 0->10, 1->20, 2->30, 3->40, 4->50......
    */
    return 10.0 * ( m_view->kPresenterDoc()->getPresSpeed() + 1 );
    //return PageSpeed[ static_cast<int>( m_view->kPresenterDoc()->getPresSpeed() ) ];
}

void KPrCanvas::_repaint( bool /*erase*/ )
{
    m_view->kPresenterDoc()->repaint( false );
}

void KPrCanvas::_repaint( const QRect &r )
{
    m_view->kPresenterDoc()->repaint( r );
}

void KPrCanvas::_repaint( KPObject *o )
{
    m_view->kPresenterDoc()->repaint( o );
}

void KPrCanvas::slotExitPres()
{
    m_view->screenStop();
}

void KPrCanvas::drawingMode()
{
    setCursor( KPresenterUtils::penCursor() );
    if(!presMenu->isItemChecked ( PM_DM ))
    {
        presMenu->setItemChecked( PM_DM, true );
        presMenu->setItemChecked( PM_SM, false );
        drawMode = true;
        //setCursor( KPresenterUtils::penCursor() );
    }
}

void KPrCanvas::switchingMode()
{
    if(!presMenu->isItemChecked ( PM_SM ))
    {
        presMenu->setItemChecked( PM_DM, false );
        presMenu->setItemChecked( PM_SM, true );
        drawMode = false; setCursor( blankCursor );

        if ( !spManualSwitch() )
            m_view->autoScreenPresIntervalTimer();
    }
}

void KPrCanvas::calcRatio( double &dx, double &dy, ModifyType _modType, double ratio ) const
{
    if ( fabs( dy ) > fabs( dx ) )
    {
        if ( _modType == MT_RESIZE_LD || _modType == MT_RESIZE_RU )
            dx = ( dy ) * -ratio ;
        else
            dx = ( dy ) * ratio ;
    }
    else
    {
        if ( _modType == MT_RESIZE_LD || _modType == MT_RESIZE_RU )
            dy =  -dx  / ratio;
        else
            dy =  dx  / ratio;
    }
}

void KPrCanvas::exitEditMode()
{
    if ( editNum )
    {
        if ( editNum->getType() == OT_TEXT )
        {
            if(m_currentTextObjectView)
            {
                m_currentTextObjectView->clearSelection();
                //hide cursor when we desactivate textObjectView
                m_currentTextObjectView->drawCursor( false );
                m_currentTextObjectView->terminate();
                KPTextObject *kpTextObj = m_currentTextObjectView->kpTextObject();
                kpTextObj->setEditingTextObj( false );
                delete m_currentTextObjectView;
                m_currentTextObjectView=0L;

                _repaint( static_cast<KPObject*>( kpTextObj ) );
            }
            // Title of slide may have changed
            emit updateSideBarItem( currPgNum()-1 );
            emit objectSelectedChanged();
            editNum=0L;
        }
        else if (editNum->getType() == OT_PART )
        {
            static_cast<KPPartObject *>(editNum)->deactivate();
            _repaint( editNum );
            editNum=0L;
            return;
        }
    }
}

bool KPrCanvas::getPixmapOrigAndCurrentSize( KPPixmapObject *&obj, KoSize *origSize, KoSize *currentSize )
{
    *origSize = KoSize(obj->originalSize().width(),obj->originalSize().height());
    *currentSize = obj->getSize();
    return true;
}

void KPrCanvas::picViewOriginalSize()
{
    picViewOrigHelper( -1, -1 );
}

void KPrCanvas::picViewOrig640x480()
{
    picViewOrigHelper(640, 480);
}

void KPrCanvas::picViewOrig800x600()
{
    picViewOrigHelper(800, 600);
}

void KPrCanvas::picViewOrig1024x768()
{
    picViewOrigHelper(1024, 768);
}

void KPrCanvas::picViewOrig1280x1024()
{
    picViewOrigHelper(1280, 1024);
}

void KPrCanvas::picViewOrig1600x1200()
{
    picViewOrigHelper(1600, 1200);
}

void KPrCanvas::picViewOrigHelper(int x, int y)
{
    KPPixmapObject *obj = 0;
    KoSize origSize;
    KoSize currentSize;

    obj=m_activePage->picViewOrigHelper();
    if (!obj)
        obj=stickyPage()->picViewOrigHelper();

    if ( obj && !getPixmapOrigAndCurrentSize( obj, &origSize, &currentSize ) )
        return;
    KoSize pgSize = m_activePage->getPageRect().size();

    if ( x == -1 && y == -1 ) {
        x = (int)origSize.width();
        y = (int)origSize.height();
    }

    QSize presSize( x, y );

    scalePixmapToBeOrigIn( currentSize, pgSize, presSize, obj );
}

void KPrCanvas::picViewOrigFactor()
{
}

void KPrCanvas::scalePixmapToBeOrigIn( const KoSize &currentSize, const KoSize &pgSize,
                                       const QSize &presSize, KPPixmapObject *obj )
{
#if KDE_IS_VERSION(3,1,90)
    QRect desk = KGlobalSettings::desktopGeometry(this);
    double faktX = (double)presSize.width() / (double)desk.width();
    double faktY = (double)presSize.height() / (double)desk.height();
#else
    double faktX = (double)presSize.width() / (double)QApplication::desktop()->screenGeometry(this).width();
    double faktY = (double)presSize.height() / (double)QApplication::desktop()->screenGeometry(this).height();
#endif
    double w = pgSize.width() * faktX;
    double h = pgSize.height() * faktY;

    ResizeCmd *resizeCmd = new ResizeCmd( i18n( "Scale Picture to be Shown 1:1 in Presentation Mode" ),
                                          KoPoint( 0, 0 ), KoSize( w - currentSize.width(), h - currentSize.height() ),
                                          obj, m_view->kPresenterDoc() );
    resizeCmd->execute();
    m_view->kPresenterDoc()->addCommand( resizeCmd );
}

void KPrCanvas::setTextBackground( KPTextObject */*obj*/ )
{
    // This is called when double-clicking on a text object.
    // What should happen exactly ? (DF)
#if 0
    QPixmap pix( m_activePage->getZoomPageRect().size() );
    QPainter painter( &pix );
    m_activePage->background()->draw( &painter, FALSE );
    QPixmap bpix( obj->getSize().toQSize() ); // ## zoom it !
    bitBlt( &bpix, 0, 0, &pix, obj->getOrig().x(), obj->getOrig().y() -
            m_activePage->getZoomPageRect().height() * ( m_view->getCurrPgNum() - 1 ), bpix.width(), bpix.height() );
    QBrush b( white, bpix );
    QPalette pal( obj->textObjectView()->palette() );
    pal.setBrush( QColorGroup::Base, b );
    obj->textObjectView()->setPalette( pal );
#endif
}

QValueList<int> KPrCanvas::pages(const QString &range) const {

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

bool KPrCanvas::pagesHelper(const QString &chunk, QValueList<int> &list) const {

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

void KPrCanvas::moveObject( int x, int y, bool key )
{
    double newPosX=m_view->zoomHandler()->unzoomItX(x);
    double newPosY=m_view->zoomHandler()->unzoomItY(y);
    KoRect boundingRect = m_boundingRect;
    m_boundingRect = m_origBRect;
    KoPoint point( m_boundingRect.topLeft() );
    KoRect pageRect=m_activePage->getPageRect();
    point.setX( m_boundingRect.x()+newPosX );
    m_boundingRect.moveTopLeft( point );
    if ( ( boundingRect.left()+m_hotSpot.x() < pageRect.left() ) || ( m_boundingRect.left() < pageRect.left() ) )
    {
        point.setX( pageRect.left() );
        m_boundingRect.moveTopLeft( point );
    }
    else if ( ( boundingRect.left()+m_hotSpot.x() > pageRect.right( ) ) || ( m_boundingRect.right() > pageRect.right() ) )
    {
        point.setX( pageRect.right()-m_boundingRect.width() );
        m_boundingRect.moveTopLeft( point );
    }

    point = m_boundingRect.topLeft();
    point.setY( m_boundingRect.y()+newPosY );
    m_boundingRect.moveTopLeft( point );

    if ( ( boundingRect.top()+m_hotSpot.y() < pageRect.top() ) || ( m_boundingRect.top() < pageRect.top() ) )
    {
        point.setY( pageRect.top() );
        m_boundingRect.moveTopLeft( point );
    }
    else if( ( boundingRect.top()+m_hotSpot.y() > pageRect.bottom() ) || ( m_boundingRect.bottom() > pageRect.bottom() ) )
    {
        point.setY( pageRect.bottom() - m_boundingRect.height() );
        m_boundingRect.moveTopLeft( point );
    }

    point = applyGrid(m_boundingRect.topLeft());
    m_boundingRect.moveTopLeft(point);

    if( m_boundingRect.topLeft() == boundingRect.topLeft() )
        return; // nothing happende (probably due to the grid)

    scrollCanvas(boundingRect);

    KoPoint _move=m_boundingRect.topLeft()-boundingRect.topLeft();
    KMacroCommand *macro=0L;
    KCommand *cmd=m_activePage->moveObject(m_view,_move,key);
    if( cmd && key)
    {
        if ( !macro)
            macro=new KMacroCommand(i18n( "Move Objects" ));
        macro->addCommand(cmd);
    }
    cmd=stickyPage()->moveObject(m_view,_move,key);
    if( cmd && key)
    {
        if ( !macro)
            macro=new KMacroCommand(i18n( "Move Objects" ));
        macro->addCommand(cmd);
    }
    if(macro)
        m_view->kPresenterDoc()->addCommand(macro);
}

void KPrCanvas::resizeObject( ModifyType _modType, int _dx, int _dy )
{
    double dx = m_view->zoomHandler()->unzoomItX( _dx);
    double dy = m_view->zoomHandler()->unzoomItY( _dy);
    KPObject *kpobject=resizeObjNum;

    //keepRatio = keepRatio || kpobject->isKeepRatio();

    KoSize objSize = kpobject->getSize();
    KoRect objRect=kpobject->getBoundingRect();
    KoRect pageRect=m_activePage->getPageRect();
    QPainter p;
    p.begin( this );
    kpobject->moveBy(m_view->zoomHandler()->unzoomItX(-diffx()),m_view->zoomHandler()->unzoomItY(-diffy()));
    kpobject->draw( &p, m_view->zoomHandler(), SM_MOVERESIZE,
                    (kpobject->isSelected()) && drawContour);
    switch ( _modType ) {
    case MT_RESIZE_LU: {
        // let the edge of the page be on the grid, this makes it
        // also possible to resize a object which is close to the edge
        if( (objRect.left() + dx) < (pageRect.left() - 1) )
            dx = pageRect.left() - objRect.left();
        if( (objRect.top() + dy) < (pageRect.top() - 1) )
            dy = pageRect.top() - objRect.top();
        // align to the grid
        dx = applyGridX( objRect.left() + dx ) - objRect.left();
        dy = applyGridY( objRect.top() + dy ) - objRect.top();
        if ( keepRatio && ratio != 0.0 )
            calcRatio( dx, dy, _modType, ratio );
        kpobject->resizeBy( -dx, -dy );
        if ( objSize.width() != (kpobject->getSize()).width() )
            kpobject->moveBy( KoPoint( dx, 0 ) );
        if ( objSize.height() != (kpobject->getSize()).height() )
            kpobject->moveBy( KoPoint( 0, dy ) );
    } break;
    case MT_RESIZE_LF: {
        dy = 0;
        if( (objRect.left() + dx) < (pageRect.left() - 1))
            dx = pageRect.left() - objRect.left();
        dx = applyGridX( objRect.left() + dx ) - objRect.left();
        if ( keepRatio && ratio != 0.0 )
            calcRatio( dx, dy, _modType, ratio );
        kpobject->resizeBy( -dx, -dy );
        if ( objSize != kpobject->getSize() )
            kpobject->moveBy( KoPoint( dx, 0 ) );
    } break;
    case MT_RESIZE_LD: {
        if( (objRect.bottom() + dy) > pageRect.bottom())
            dy = pageRect.bottom() - objRect.bottom();
        if( (objRect.left() + dx) < (pageRect.left() - 1) )
            dx = pageRect.left() - objRect.left();
        dx = applyGridX( objRect.left() + dx ) - objRect.left();
        dy = applyGridY( objRect.bottom() + dy ) - objRect.bottom();
        if ( keepRatio && ratio != 0.0 )
            calcRatio( dx, dy, _modType, ratio );
        kpobject->resizeBy( -dx, dy );
        if ( objSize.width() != (kpobject->getSize()).width() )
            kpobject->moveBy( KoPoint( dx, 0 ) );
    } break;
    case MT_RESIZE_RU: {
        if( (objRect.right() + dx) > pageRect.right() )
            dx = pageRect.right() - objRect.right();
        if( (objRect.top() + dy) < (pageRect.top() - 1) )
            dy = pageRect.top() - objRect.top();
        dx = applyGridX( objRect.right() + dx ) - objRect.right();
        dy = applyGridY( objRect.top() + dy ) - objRect.top();
        if ( keepRatio && ratio != 0.0 )
            calcRatio( dx, dy, _modType, ratio );
        kpobject->resizeBy( dx, -dy );
        if ( objSize.height() != (kpobject->getSize()).height() )
            kpobject->moveBy( KoPoint( 0, dy ) );
    } break;
    case MT_RESIZE_RT: {
        dy = 0;
        if( (objRect.right() + dx) > pageRect.right() )
            dx = pageRect.right() - objRect.right();
        dx = applyGridX( objRect.right() + dx ) - objRect.right();
        if ( keepRatio && ratio != 0.0 )
            calcRatio( dx, dy, _modType, ratio );
        kpobject->resizeBy( dx, dy );
    } break;
    case MT_RESIZE_RD: {
        if( (objRect.bottom() + dy) > pageRect.bottom() )
            dy = pageRect.bottom() - objRect.bottom();
        if( (objRect.right() + dx) > pageRect.right() )
            dx = pageRect.right() - objRect.right();
        dx = applyGridX( objRect.right() + dx ) - objRect.right();
        dy = applyGridY( objRect.bottom() + dy ) - objRect.bottom();
        if ( keepRatio && ratio != 0.0 )
            calcRatio( dx, dy, _modType, ratio );
        kpobject->resizeBy( dx, dy );
    } break;
    case MT_RESIZE_UP: {
        dx = 0;
        if( (objRect.top() + dy) < (pageRect.top() - 1) )
            dy = pageRect.top() - objRect.top();
        dy = applyGridY( objRect.top() + dy) - objRect.top();
        if ( keepRatio && ratio != 0.0 )
            calcRatio( dx, dy, _modType, ratio );
        kpobject->resizeBy( -dx, -dy );
        if ( objSize != kpobject->getSize() )
            kpobject->moveBy( KoPoint( 0, dy ) );

    } break;
    case MT_RESIZE_DN: {
        dx = 0;
        if( (objRect.bottom() + dy) > pageRect.bottom() )
            dy = pageRect.bottom() - objRect.bottom();
        dy = applyGridY( objRect.bottom() + dy ) - objRect.bottom();
        if ( keepRatio && ratio != 0.0 )
            calcRatio( dx, dy, _modType, ratio );
        kpobject->resizeBy( dx, dy );
    } break;
    default: break;
    }
    kpobject->draw( &p, m_view->zoomHandler(), SM_MOVERESIZE,
                    (kpobject->isSelected()) && drawContour );
    kpobject->moveBy(m_view->zoomHandler()->unzoomItX(diffx()),m_view->zoomHandler()->unzoomItY(diffy()));
    p.end();

    _repaint( oldBoundingRect );
    _repaint( kpobject );
    emit objectSizeChanged();
    oldBoundingRect = getOldBoundingRect(kpobject);
}

void KPrCanvas::raiseObject( KPObject *_kpobject )
{
    if( objectList().count() <= 1 )
        return;
    if ( selectedObjectPosition == -1 ) {
        if ( m_activePage->numSelected() == 1 ) { // execute this if user selected is one object.
            QPtrList<KPObject> _list = objectList();
            _list.setAutoDelete( false );

            if ( _kpobject->isSelected() ) {
                selectedObjectPosition = objectList().findRef( _kpobject );
                _list.take( selectedObjectPosition );
                _list.append( _kpobject );
            }

            m_activePage->setObjectList( _list );
        }
        else
            selectedObjectPosition = -1;
    }
}

void KPrCanvas::lowerObject()
{
    if( objectList().count() <= 1 || (int)objectList().count() <= selectedObjectPosition )
        return;
    KPObject *kpobject = objectList().last();
    QPtrList<KPObject> _list = objectList();
    _list.setAutoDelete( false );

    if ( kpobject->isSelected() ) {
        _list.take( _list.count() - 1 );
        if ( objectList().findRef( kpobject ) != -1 )
            _list.insert( selectedObjectPosition, kpobject );
    }
    m_activePage->setObjectList( _list );
}

void KPrCanvas::playSound( const QString &soundFileName )
{
    if(soundPlayer)
        delete soundPlayer;
    soundPlayer = new KPresenterSoundPlayer( soundFileName );
    soundPlayer->play();
}

void KPrCanvas::stopSound()
{
    if ( soundPlayer ) {
        soundPlayer->stop();
        delete soundPlayer;
        soundPlayer = 0;
    }
}

void KPrCanvas::setXimPosition( int x, int y, int w, int h, QFont *f )
{
    QWidget::setMicroFocusHint( x - diffx(), y - diffy(), w, h, true, f );
}

void KPrCanvas::createEditing( KPTextObject *textObj )
{
    if( m_currentTextObjectView)
    {
        m_currentTextObjectView->terminate();
        delete m_currentTextObjectView;
        m_currentTextObjectView = 0L;
        editNum = 0L;
    }
    m_currentTextObjectView=textObj->createKPTextView( this );
    editNum=textObj;
}

void KPrCanvas::terminateEditing( KPTextObject *textObj )
{
    if ( m_currentTextObjectView && m_currentTextObjectView->kpTextObject() == textObj )
    {
        m_currentTextObjectView->terminate();
        delete m_currentTextObjectView;
        m_currentTextObjectView = 0L;
        editNum = 0L;
    }
}

void KPrCanvas::drawCubicBezierCurve( int _dx, int _dy )
{
    QPoint oldEndPoint = m_dragEndPoint;
    m_dragEndPoint = QPoint( _dx, _dy );

    unsigned int pointCount = m_pointArray.count();

    QPainter p( this );

    if ( !m_drawLineWithCubicBezierCurve ) {
        QPen _pen = QPen( Qt::black, 1, Qt::DashLine );
        p.setPen( _pen );
        p.setBrush( Qt::NoBrush );
        p.setRasterOp( Qt::NotROP );

        p.save();
        double _angle = KoPoint::getAngle( oldEndPoint, m_dragStartPoint );
        drawFigure( L_SQUARE, &p, m_view->zoomHandler()->unzoomPoint( oldEndPoint ), _pen.color(),
                    _pen.width(), _angle,m_view->zoomHandler() ); // erase old figure
        p.restore();

        p.drawLine( m_dragStartPoint, oldEndPoint ); // erase old line

        int p_x = m_dragStartPoint.x() * 2 - oldEndPoint.x();
        int p_y = m_dragStartPoint.y() * 2 - oldEndPoint.y();
        m_dragSymmetricEndPoint = QPoint( p_x, p_y );

        p.save();
        _angle = KoPoint::getAngle( m_dragSymmetricEndPoint, m_dragStartPoint );
        drawFigure( L_SQUARE, &p, m_view->zoomHandler()->unzoomPoint( m_dragSymmetricEndPoint ),
                    _pen.color(), _pen.width(), _angle,m_view->zoomHandler() );  // erase old figure
        p.restore();

        p.drawLine( m_dragStartPoint, m_dragSymmetricEndPoint );  // erase old line


        p.save();
        _angle = KoPoint::getAngle( m_dragEndPoint, m_dragStartPoint );
        drawFigure( L_SQUARE, &p, m_view->zoomHandler()->unzoomPoint( m_dragEndPoint ),
                    _pen.color(), _pen.width(), _angle,m_view->zoomHandler() ); // draw new figure
        p.restore();

        p.drawLine( m_dragStartPoint, m_dragEndPoint );  // draw new line

        p_x = m_dragStartPoint.x() * 2 - m_dragEndPoint.x();
        p_y = m_dragStartPoint.y() * 2 - m_dragEndPoint.y();
        m_dragSymmetricEndPoint = QPoint( p_x, p_y );

        p.save();
        _angle = KoPoint::getAngle( m_dragSymmetricEndPoint, m_dragStartPoint );
        drawFigure( L_SQUARE, &p, m_view->zoomHandler()->unzoomPoint( m_dragSymmetricEndPoint ),
                    _pen.color(), _pen.width(), _angle,m_view->zoomHandler() ); // draw new figure
        p.restore();

        p.drawLine( m_dragStartPoint, m_dragSymmetricEndPoint );  // draw new line
    }
    else if ( m_drawLineWithCubicBezierCurve ) {
        p.setPen( QPen( Qt::black, 1, Qt::SolidLine ) );
        p.setBrush( Qt::NoBrush );
        p.setRasterOp( Qt::NotROP );

        QPoint startPoint( m_view->zoomHandler()->zoomItX( m_pointArray.at( m_indexPointArray - 1 ).x() ),
                           m_view->zoomHandler()->zoomItY( m_pointArray.at( m_indexPointArray - 1 ).y() ) );

        p.drawLine( startPoint, oldEndPoint );  // erase old line

        p.drawLine( startPoint, m_dragEndPoint );  // draw new line
    }

    if ( !m_drawLineWithCubicBezierCurve && ( ( pointCount % 2 ) == 0 ) ) {
        p.save();

        p.setPen( QPen( Qt::black, 1, Qt::SolidLine ) );
        p.setBrush( Qt::NoBrush );
        p.setRasterOp( Qt::NotROP );
        // erase old cubic bezier curve
        p.drawCubicBezier( m_oldCubicBezierPointArray.zoomPointArray( m_view->zoomHandler() ) );

        double _firstX = m_pointArray.at( m_indexPointArray - 2 ).x();
        double _firstY = m_pointArray.at( m_indexPointArray - 2 ).y();

        double _fourthX = m_pointArray.at( m_indexPointArray - 1 ).x();
        double _fourthY = m_pointArray.at( m_indexPointArray - 1 ).y();

        double _midpointX = (_firstX + _fourthX ) / 2;
        double _midpointY = (_firstY + _fourthY ) / 2;
        double _diffX = _fourthX - _midpointX;
        double _diffY = _fourthY - _midpointY;

        double _secondX = m_view->zoomHandler()->unzoomItX( m_dragEndPoint.x() ) - _diffX;
        double _secondY = m_view->zoomHandler()->unzoomItY( m_dragEndPoint.y() ) - _diffY;
        m_CubicBezierSecondPoint = KoPoint( _secondX, _secondY );

        double _thirdX = m_view->zoomHandler()->unzoomItX( m_dragSymmetricEndPoint.x() ) - _diffX;
        double _thirdY = m_view->zoomHandler()->unzoomItY( m_dragSymmetricEndPoint.y() ) - _diffY;
        m_CubicBezierThirdPoint = KoPoint( _thirdX, _thirdY );

        if ( toolEditMode == INS_QUADRICBEZIERCURVE || toolEditMode == INS_CLOSED_QUADRICBEZIERCURVE ) {
            _secondX = _thirdX;
            _secondY = _thirdY;
            m_CubicBezierSecondPoint = KoPoint( _secondX, _secondY );
        }

        KoPointArray points;
        points.putPoints( 0, 4, _firstX,_firstY, _secondX,_secondY, _thirdX,_thirdY, _fourthX,_fourthY );
        // draw new cubic bezier curve
        p.drawCubicBezier( points.zoomPointArray( m_view->zoomHandler() ) );

        m_oldCubicBezierPointArray = points;

        p.restore();
    }

    p.end();
}

void KPrCanvas::drawPolygon( const KoPoint &startPoint, const KoPoint &endPoint )
{
    bool checkConcavePolygon = m_view->getCheckConcavePolygon();
    int cornersValue = m_view->getCornersValue();
    int sharpnessValue = m_view->getSharpnessValue();

    QPainter p;
    p.begin( this );
    p.setPen( QPen( Qt::black, 1, Qt::SolidLine ) );
    p.setRasterOp( Qt::NotROP );

    double angle = 2 * M_PI / cornersValue;
    double dx = QABS( startPoint.x () - endPoint.x () );
    double dy = QABS( startPoint.y () - endPoint.y () );
    double radius = ( dx > dy ? dx / 2.0 : dy / 2.0 );

    //xoff / yoff : coordinate of centre of the circle.
    double xoff = startPoint.x() + ( startPoint.x() < endPoint.x() ? radius : -radius );
    double yoff = startPoint.y() + ( startPoint.y() < endPoint.y() ? radius : -radius );

    KoPointArray points( checkConcavePolygon ? cornersValue * 2 : cornersValue );
    points.setPoint( 0, xoff, -radius + yoff );

    if ( checkConcavePolygon ) {
        angle = angle / 2.0;
        double a = angle;
        double r = radius - ( sharpnessValue / 100.0 * radius );
        for ( int i = 1; i < cornersValue * 2; ++i ) {
            double xp, yp;
            if ( i % 2 ) {
                xp =  r * sin( a );
                yp = -r * cos( a );
            }
            else {
                xp = radius * sin( a );
                yp = -radius * cos( a );
            }
            a += angle;
            points.setPoint( i, xp + xoff, yp + yoff );
        }
    }
    else {
        double a = angle;
        for ( int i = 1; i < cornersValue; ++i ) {
            double xp = radius * sin( a );
            double yp = -radius * cos( a );
            a += angle;
            points.setPoint( i, xp + xoff, yp + yoff );
        }
    }
    p.drawPolygon( points.zoomPointArray( m_view->zoomHandler() ) );
    p.end();

    m_pointArray = points;
}


bool KPrCanvas::oneObjectTextSelected() const
{
    return m_activePage->oneObjectTextSelected() ||
        stickyPage()->oneObjectTextSelected();
}

bool KPrCanvas::oneObjectTextExist() const
{
    return m_activePage->oneObjectTextExist() ||
        stickyPage()->oneObjectTextExist();
}

KPrPage* KPrCanvas::activePage() const
{
    return m_activePage;
}

void KPrCanvas::setActivePage( KPrPage* active )
{
    Q_ASSERT(active);
    //kdDebug(33001)<<"KPrCanvas::setActivePage( KPrPage* active) :"<<active<<endl;
    m_activePage = active;
    m_view->kPresenterDoc()->recalcPageNum();
}

//return true if object is a header/footer hidden
bool KPrCanvas::objectIsAHeaderFooterHidden(KPObject *obj) const
{
    if ((obj==m_view->kPresenterDoc()->header() && !m_view->kPresenterDoc()->hasHeader())
        || (obj==m_view->kPresenterDoc()->footer() && !m_view->kPresenterDoc()->hasFooter()))
        return true;
    return false;
}

int KPrCanvas::numberOfObjectSelected() const
{
    int nb=activePage()->numSelected();
    nb+=stickyPage()->numSelected();
    return nb;
}

KPObject *KPrCanvas::getSelectedObj() const
{
    KPObject *obj=activePage()->getSelectedObj();
    if(obj)
        return obj;
    obj=stickyPage()->getSelectedObj();
    return obj;
}

int KPrCanvas::getPenBrushFlags() const
{
    int flags=0;
    flags=activePage()->getPenBrushFlags(activePage()->objectList());
    flags=flags |stickyPage()->getPenBrushFlags(stickyPage()->objectList());
    if(flags==0)
        flags = StyleDia::SdAll;
    return flags;
}

void KPrCanvas::ungroupObjects()
{
    KMacroCommand *macro = new KMacroCommand(i18n( "Ungroup Objects" ));
    KCommand *cmd = m_activePage->ungroupObjects();
    if ( cmd )
    {
        macro = new KMacroCommand(i18n( "Ungroup Objects" ));
        macro->addCommand( cmd );
    }
    cmd = stickyPage()->ungroupObjects();
    if ( cmd )
    {
        if ( !macro)
            macro = new KMacroCommand(i18n( "Ungroup Objects" ));
        macro->addCommand( cmd );
    }
    if ( macro )
        m_view->kPresenterDoc()->addCommand( macro );
}

void KPrCanvas::groupObjects()
{
    m_activePage->groupObjects();
    stickyPage()->groupObjects();
}

KPrPage *KPrCanvas::stickyPage() const
{
    return m_view->kPresenterDoc()->stickyPage();
}

void KPrCanvas::scrollTopLeftPoint( const QPoint & pos )
{
    m_view->getHScrollBar()->setValue( pos.x() );
    m_view->getVScrollBar()->setValue( pos.y() );
}

void KPrCanvas::scrollCanvas(const KoRect & oldPos)
{
    QRect rect = m_view->zoomHandler()->zoomRect( oldPos );
    KoRect visiblePage = m_view->zoomHandler()->unzoomRect( visibleRect() );
    double tmpdiffx = m_view->zoomHandler()->unzoomItX( diffx() );
    double tmpdiffy = m_view->zoomHandler()->unzoomItY( diffy() );
    if( m_boundingRect.bottom() > ( visiblePage.bottom() + tmpdiffy ) )
    {
        m_view->kPresenterDoc()->repaint( rect );
        int y = m_view->zoomHandler()->zoomItY( m_boundingRect.bottom() )
              - m_view->zoomHandler()->zoomItY( visiblePage.bottom() + tmpdiffy );
        m_view->getVScrollBar()->setValue( m_view->getVScrollBar()->value() + y );
    }
    else if( m_boundingRect.top() < visiblePage.top() + tmpdiffy )
    {
        m_view->kPresenterDoc()->repaint( rect );
        int y = m_view->zoomHandler()->zoomItY( visiblePage.top() + tmpdiffy )
              - m_view->zoomHandler()->zoomItY( m_boundingRect.top() );
        m_view->getVScrollBar()->setValue( m_view->getVScrollBar()->value() - y );
    }

    if( m_boundingRect.left() < ( visiblePage.left() + tmpdiffx ) )
    {
        m_view->kPresenterDoc()->repaint( rect );
        int x = m_view->zoomHandler()->zoomItX( visiblePage.left() + tmpdiffx )
              - m_view->zoomHandler()->zoomItX( m_boundingRect.left() );
        m_view->getHScrollBar()->setValue( m_view->getHScrollBar()->value() - x );
    }
    else if ( m_boundingRect.right() > ( visiblePage.right() + tmpdiffx ) )
    {
        m_view->kPresenterDoc()->repaint( rect );

        int x = m_view->zoomHandler()->zoomItX( m_boundingRect.right() )
              - m_view->zoomHandler()->zoomItX( visiblePage.right() + tmpdiffx );
        m_view->getHScrollBar()->setValue( m_view->getHScrollBar()->value() + x );
    }
}

void KPrCanvas::changePicture( const KURL & url )
{
    m_activePage->changePicture( url );
    stickyPage()->changePicture( url );
}

unsigned int KPrCanvas::objNums() const
{
    return (m_activePage->objNums() + stickyPage()->objNums());
}

void KPrCanvas::moveHelpLine( const QPoint & pos)
{
    QRect rect = m_activePage->getZoomPageRect();

    if ( m_tmpHorizHelpline != -1)
    {
        double vi = m_view->kPresenterDoc()->horizHelplines()[m_tmpHorizHelpline];
        m_view->kPresenterDoc()->repaint(QRect( rect.left(), m_view->zoomHandler()->zoomItY(vi) -4,
                                                rect.right(), m_view->zoomHandler()->zoomItY(vi) + 4 ));
        if( pos.y() + diffy() <= 0)
            removeHelpLine();
        else
            m_view->kPresenterDoc()->updateHorizHelpline( m_tmpHorizHelpline, m_view->zoomHandler()->unzoomItY(pos.y()+diffy()));
    }
    else if ( m_tmpVertHelpline != -1 )
    {
        double vi = m_view->kPresenterDoc()->vertHelplines()[m_tmpVertHelpline];

        m_view->kPresenterDoc()->repaint(QRect( m_view->zoomHandler()->zoomItX(vi) - 4,
                                                rect.top(), m_view->zoomHandler()->zoomItX(vi) + 4 , rect.bottom()));
        if( pos.x() + diffx() <= 0)
            removeHelpLine();
        else
            m_view->kPresenterDoc()->updateVertHelpline( m_tmpVertHelpline, m_view->zoomHandler()->unzoomItX(pos.x()+diffx()));

    }
    m_tmpVertHelpline = -1;
    m_tmpHorizHelpline = -1;
    tmpHelpLinePosX = -1;
    tmpHelpLinePosY = -1;
}

void KPrCanvas::tmpDrawMoveHelpLine( const QPoint & newPos, bool _horizontal )
{
    QPainter p;
    p.begin( this );
    p.setRasterOp( NotROP );
    p.setPen( QPen( black, 0, DotLine ) );
    QRect rect = m_activePage->getZoomPageRect();
    if (  _horizontal )
    {
        double vi = tmpHelpLinePosY ;
        p.drawLine(rect.left(), m_view->zoomHandler()->zoomItY(vi), rect.right(), m_view->zoomHandler()->zoomItY(vi));

        p.setPen( QPen( black, 1, DotLine ) );

        vi = m_view->zoomHandler()->unzoomItY(newPos.y());

        p.drawLine(rect.left(), m_view->zoomHandler()->zoomItY(vi), rect.right(), m_view->zoomHandler()->zoomItY(vi));
        tmpHelpLinePosY = vi;
    }
    else
    {
        double vi = tmpHelpLinePosX;
        p.drawLine(m_view->zoomHandler()->zoomItX(vi), rect.top(), m_view->zoomHandler()->zoomItX(vi), rect.bottom());

        p.setPen( QPen( black, 1, DotLine ) );

        vi = m_view->zoomHandler()->unzoomItX(newPos.x());

        p.drawLine(m_view->zoomHandler()->zoomItX(vi), rect.top(), m_view->zoomHandler()->zoomItX(vi), rect.bottom());
        tmpHelpLinePosX = vi;
    }
    p.end();
    m_view->kPresenterDoc()->setModified(true);

}

void KPrCanvas::tmpMoveHelpLine( const QPoint & newPos)
{
    if( m_tmpHorizHelpline != -1 )
        tmpDrawMoveHelpLine( newPos, true );
    else if( m_tmpVertHelpline != -1 )
        tmpDrawMoveHelpLine( newPos, false );
}

void KPrCanvas::removeHelpLine()
{
    if ( m_tmpVertHelpline != -1)
        m_view->kPresenterDoc()->removeVertHelpline( m_tmpVertHelpline );
    else if ( m_tmpHorizHelpline != -1)
        m_view->kPresenterDoc()->removeHorizHelpline( m_tmpHorizHelpline );
    m_tmpHorizHelpline = -1;
    m_tmpVertHelpline = -1;
    tmpHelpLinePosX = -1;
    tmpHelpLinePosY = -1;
    m_view->kPresenterDoc()->setModified(true);
    m_view->kPresenterDoc()->repaint(false);
}

void KPrCanvas::changeHelpLinePosition( double newPos )
{
    if ( m_tmpVertHelpline != -1)
    {
        if( newPos < 0)
            m_view->kPresenterDoc()->removeVertHelpline( m_tmpVertHelpline );
        else
            m_view->kPresenterDoc()->updateVertHelpline( m_tmpVertHelpline, newPos  );
    }
    else if ( m_tmpHorizHelpline != -1)
    {
        if( newPos < 0)
            m_view->kPresenterDoc()->removeHorizHelpline( m_tmpHorizHelpline );
        else
            m_view->kPresenterDoc()->updateHorizHelpline( m_tmpHorizHelpline, newPos );
    }
    m_tmpHorizHelpline = -1;
    m_tmpVertHelpline = -1;
    tmpHelpLinePosX = -1;
    tmpHelpLinePosY = -1;
    m_view->kPresenterDoc()->setModified(true);
    m_view->kPresenterDoc()->repaint(false);
}

void KPrCanvas::changeHelpPointPosition( KoPoint newPos)
{
    if ( m_tmpHelpPoint != -1)
        m_view->kPresenterDoc()->updateHelpPoint( m_tmpHelpPoint, newPos);
    m_tmpHelpPoint = -1;
    tmpHelpPointPos = KoPoint( -1, -1 );
    m_view->kPresenterDoc()->setModified(true);
    m_view->kPresenterDoc()->repaint(false);
}

void KPrCanvas::removeHelpPoint()
{
    if ( m_tmpHelpPoint != -1)
        m_view->kPresenterDoc()->removeHelpPoint( m_tmpHelpPoint );
    m_tmpHelpPoint = -1;
    tmpHelpPointPos = KoPoint( -1, -1 );
    m_view->kPresenterDoc()->setModified(true);
    m_view->kPresenterDoc()->repaint(false);
}

void KPrCanvas::tmpDrawMoveHelpPoint( const QPoint & newPos )
{
    QPainter p;
    p.begin( this );
    p.setRasterOp( NotROP );
    p.setPen( QPen( black, 0, DotLine ) );
    KoPoint vi = tmpHelpPointPos;

    QPoint point=m_view->zoomHandler()->zoomPoint( vi );
    p.drawLine( point.x(), point.y()-20, point.x(), point.y()+20);
    p.drawLine( point.x()-20, point.y(), point.x()+20, point.y());

    p.setPen( QPen( black, 1, DotLine ) );

    vi = m_view->zoomHandler()->unzoomPoint(newPos);

    point=m_view->zoomHandler()->zoomPoint( vi );
    p.drawLine( point.x(), point.y()-20, point.x(), point.y()+20);
    p.drawLine( point.x()-20, point.y(), point.x()+20, point.y());

    tmpHelpPointPos = vi;

    p.end();
    m_view->kPresenterDoc()->setModified(true);
}

void KPrCanvas::moveHelpPoint( const QPoint & newPos )
{
    if ( m_tmpHelpPoint != -1)
    {
        KoPoint vi = m_view->kPresenterDoc()->helpPoints()[m_tmpHelpPoint];
        m_view->kPresenterDoc()->repaint(QRect( m_view->zoomHandler()->zoomItX(vi.x())-25,
                                                m_view->zoomHandler()->zoomItY(vi.y())-25, 50, 50));

        m_view->kPresenterDoc()->updateHelpPoint( m_tmpHelpPoint , m_view->zoomHandler()->unzoomPoint(
                                                      QPoint( newPos.x()+diffx(),newPos.y()+diffy())));
    }
    m_tmpHelpPoint = -1;
    tmpHelpPointPos = KoPoint( -1, -1);
}


void KPrCanvas::ensureVisible( int x, int y, int xmargin, int ymargin )
{

    int newy= (y+ymargin)-(visibleRect().bottom()+diffy());

    if( newy > 0)
        m_view->getVScrollBar()->setValue(m_view->getVScrollBar()->value()+newy);
    else
    {
        newy= (y )-(visibleRect().top()+diffy());
        if( newy < 0)
            m_view->getVScrollBar()->setValue(m_view->getVScrollBar()->value()+newy  - ymargin);
    }

    int newx= (x+xmargin)-(visibleRect().right()+diffx());
    if( newx > 0)
        m_view->getHScrollBar()->setValue(m_view->getHScrollBar()->value()+newx);
    else
    {
        newx= x -(visibleRect().left()+diffx());
        if( newx < 0)
            m_view->getHScrollBar()->setValue(m_view->getHScrollBar()->value()+newx - xmargin );
    }
}

// "Extend Contents to Object Height"
void KPrCanvas::textContentsToHeight()
{
    QPtrList<KPTextObject> lst = applicableTextObjects();
    if ( lst.isEmpty() )
        return;
    QPtrListIterator<KPTextObject> it( lst );
    KMacroCommand * macro = 0L;
    for ( ; it.current() ; ++it ) {
        KCommand *cmd= it.current()->textContentsToHeight();
        if( cmd )
        {
            if ( !macro )
                macro = new KMacroCommand(i18n("Extend Text Contents to Height"));
            macro->addCommand( cmd);
            _repaint( it.current() );
        }
    }
    if( macro )
    {
        macro->execute();
        m_view->kPresenterDoc()->addCommand( macro );
        m_view->kPresenterDoc()->repaint(this);
    }
}

// "Resize Object to fit Contents"
void KPrCanvas::textObjectToContents()
{
    QPtrList<KPTextObject> lst = applicableTextObjects();
    if ( lst.isEmpty() )
        return;
    QPtrListIterator<KPTextObject> it( lst );
    KMacroCommand * macro = 0L;
    for ( ; it.current() ; ++it ) {
        KCommand *cmd= it.current()->textObjectToContents();
        if( cmd )
        {
            if (!macro )
                macro = new KMacroCommand(i18n("Extend Text to Contents"));
            macro->addCommand( cmd);
        }
    }

    if( macro)
    {
        macro->execute();
        m_view->kPresenterDoc()->addCommand( macro );
        m_view->kPresenterDoc()->repaint(this);
    }

}

void KPrCanvas::flipObject( bool _horizontal )
{
    QPtrList<KPObject> lst;
    QPtrListIterator<KPObject> it(getObjectList());
    for ( ; it.current(); ++it ) {
        if ( it.current()->isSelected() &&
             it.current()->getType() != OT_AUTOFORM &&
             it.current()->getType() != OT_PART &&
             it.current()->getType() != OT_TEXT ) {
            lst.append( it.current() );
        }
    }
    //get sticky obj
    it=stickyPage()->objectList();
    for ( ; it.current(); ++it ) {
        if ( it.current()->isSelected() &&
             it.current()->getType() != OT_AUTOFORM &&
             it.current()->getType() != OT_PART &&
             it.current()->getType() != OT_TEXT ) {
            lst.append( it.current() );
        }
    }
    if ( lst.isEmpty() )
        return;

    KPrFlipObjectCommand *flipCmd = new KPrFlipObjectCommand( i18n("Flip Objects"),
                                                              m_view->kPresenterDoc(),
                                                              _horizontal, lst );
    flipCmd->execute();
    m_view->kPresenterDoc()->addCommand( flipCmd );
}

KCommand *KPrCanvas::setKeepRatioObj( bool p )
{
    QPtrList<KPObject> lst;
    QValueList<bool> listKeepRatio;
    QPtrListIterator<KPObject> it(getObjectList());
    for ( ; it.current(); ++it ) {
        if ( it.current()->isSelected() )
        {
            lst.append( it.current() );
            listKeepRatio.append( it.current()->isKeepRatio());
        }
    }
    //get sticky obj
    it=stickyPage()->objectList();
    for ( ; it.current(); ++it ) {
        if ( it.current()->isSelected() )
        {
            lst.append( it.current() );
            listKeepRatio.append( it.current()->isKeepRatio());
        }
    }
    if ( lst.isEmpty())
        return 0L;
    KCommand *cmd= new KPrGeometryPropertiesCommand( i18n("Keep Ratio"), listKeepRatio, lst, p, m_view->kPresenterDoc(),
                                                     KPrGeometryPropertiesCommand::KeepRatio);
    cmd->execute();
    return cmd;
}

KCommand *KPrCanvas::setProtectSizeObj(bool protect)
{
    QPtrList<KPObject> lst;
    QValueList<bool> listProt;
    QPtrListIterator<KPObject> it(getObjectList());
    for ( ; it.current(); ++it ) {
        if ( it.current()->isSelected() )
        {
            lst.append( it.current() );
            listProt.append( it.current()->isProtect());
        }
    }
    //get sticky obj
    it=stickyPage()->objectList();
    for ( ; it.current(); ++it ) {
        if ( it.current()->isSelected() )
        {
            lst.append( it.current() );
            listProt.append( it.current()->isProtect());
        }
    }
    if ( lst.isEmpty())
        return 0L;
    KCommand *cmd= new KPrGeometryPropertiesCommand( i18n("Protect Object"), listProt, lst, protect,
                                                     m_view->kPresenterDoc(),KPrGeometryPropertiesCommand::ProtectSize );
    cmd->execute();
    return cmd;

}

KoRect KPrCanvas::zoomAllObject()
{
    KoRect objBoundingRect=KoRect();

    objBoundingRect = m_activePage->getBoundingAllObjectRect( objBoundingRect);
    objBoundingRect = stickyPage()->getBoundingAllObjectRect( objBoundingRect);
    return objBoundingRect;
}

QPtrList<KPTextObject> KPrCanvas::listOfTextObjs() const
{
    QPtrList<KPTextObject> lst;
    QPtrListIterator<KPObject> it(getObjectList());
    for ( ; it.current(); ++it ) {
        if (  it.current()->getType() == OT_TEXT )
            lst.append( static_cast<KPTextObject*>( it.current() ) );
    }
    //get sticky obj
    it=stickyPage()->objectList();
    for ( ; it.current(); ++it ) {
        if ( it.current()->getType() == OT_TEXT )
        {
            if ( objectIsAHeaderFooterHidden(it.current()))
                continue;
            else
                lst.append( static_cast<KPTextObject*>( it.current() ));
        }
    }
    return lst;
}

KPTextObject * KPrCanvas::textObjectByPos( int pos ) const
{
    if ( pos < 0 )
        return 0L;
    return listOfTextObjs().at( pos );
}

int KPrCanvas::textObjectNum( KPTextObject * obj ) const
{
    int num = 0;
    QPtrList<KPTextObject> obj2 = listOfTextObjs();
    QPtrListIterator<KPTextObject> it2(obj2 );
    for ( ; it2.current() ; ++it2, num++ ) {
        if ( it2.current() == obj )
        {
            return num;
        }
    }
    return -1;
}

KPTextObject* KPrCanvas::textUnderMouse( const QPoint & point )
{
    QPtrList<KPTextObject> obj = listOfTextObjs();
    QPtrListIterator<KPTextObject> it2(obj );
    for ( ; it2.current() ; ++it2 ) {
        QRect outerRect( m_view->kPresenterDoc()->zoomHandler()->zoomRect( it2.current()->getRect()) );
        if ( !it2.current()->isProtectContent() && outerRect.contains( point) )
            return it2.current();
    }
    return 0L;
}

bool KPrCanvas::checkCurrentTextEdit( KPTextObject * textObj )
{
    bool emitChanged = false;
    if ( textObj && m_currentTextObjectView && m_currentTextObjectView->kpTextObject() != textObj )
    {
        m_currentTextObjectView->kpTextObject()->setEditingTextObj( false );
        _repaint( m_currentTextObjectView->kpTextObject() );
        //don't remove selection otherwise break dnd.
        m_currentTextObjectView->terminate(false);
        delete m_currentTextObjectView;
        m_currentTextObjectView = 0L;
        emitChanged = true;
    }

    // Edit the frameset under the mouse, if any
    if ( textObj && !m_currentTextObjectView )
    {
        m_currentTextObjectView = textObj->createKPTextView( this );
        emitChanged = true;
    }
    return emitChanged;
}

void KPrCanvas::alignObjLeft()
{
    KMacroCommand *macro= 0L;
    KCommand *cmd=0L;
    KPresenterDoc *doc =m_view->kPresenterDoc();
    KoRect rect = (numberOfObjectSelected() > 1) ? getAlignBoundingRect() : activePage()->getPageRect();
    cmd=activePage()->alignObjsLeft(rect);
    if(cmd)
    {
        if ( !macro )
            macro= new KMacroCommand(i18n( "Align Objects Left" ));
        macro->addCommand(cmd);
    }
    cmd=stickyPage()->alignObjsLeft(rect);
    if(cmd)
    {
        if ( !macro )
            macro= new KMacroCommand(i18n( "Align Objects Left" ));

        macro->addCommand(cmd);
    }
    if(macro)
        doc->addCommand(macro);
}

void KPrCanvas::alignObjCenterH()
{
    KMacroCommand *macro= 0L;
    KCommand *cmd=0L;
    KPresenterDoc *doc =m_view->kPresenterDoc();
    KoRect rect = (numberOfObjectSelected() > 1) ? getAlignBoundingRect() : activePage()->getPageRect();
    cmd=activePage()->alignObjsCenterH(rect);
    if(cmd)
    {
        if ( !macro )
            macro= new KMacroCommand(i18n( "Align Objects Centered (horizontal)"));
        macro->addCommand(cmd);
    }
    cmd=doc->stickyPage()->alignObjsCenterH(rect);
    if(cmd)
    {
        if ( !macro )
            macro= new KMacroCommand(i18n( "Align Objects Centered (horizontal)"));
        macro->addCommand(cmd);
    }
    if(macro)
        doc->addCommand(macro);
}

void KPrCanvas::alignObjRight()
{
    KMacroCommand *macro= 0L;
    KCommand *cmd=0L;
    KPresenterDoc *doc =m_view->kPresenterDoc();
    KoRect rect = (numberOfObjectSelected() > 1) ? getAlignBoundingRect() : activePage()->getPageRect();

    cmd=activePage()->alignObjsRight(rect);
    if(cmd)
    {
        if ( !macro )
            macro= new KMacroCommand(i18n( "Align Objects Right" ));

        macro->addCommand(cmd);
    }
    cmd=doc->stickyPage()->alignObjsRight(rect);
    if(cmd)
    {
        if ( !macro )
            macro= new KMacroCommand(i18n( "Align Objects Right" ));

        macro->addCommand(cmd);
    }
    if(macro)
        doc->addCommand(macro);
}

void KPrCanvas::alignObjTop()
{
    KMacroCommand *macro= new KMacroCommand(i18n( "Align Objects Top" ));
    KCommand *cmd=0L;
    KPresenterDoc *doc =m_view->kPresenterDoc();
    KoRect rect = (numberOfObjectSelected() > 1) ? getAlignBoundingRect() : activePage()->getPageRect();

    cmd=activePage()->alignObjsTop(rect);
    if(cmd)
    {
        if ( !macro )
            macro= new KMacroCommand(i18n( "Align Objects Top" ));
        macro->addCommand(cmd);
    }
    cmd=doc->stickyPage()->alignObjsTop(rect);
    if(cmd)
    {
        if ( !macro )
            macro= new KMacroCommand(i18n( "Align Objects Top" ));
        macro->addCommand(cmd);
    }
    if(macro)
        doc->addCommand(macro);
}

void KPrCanvas::alignObjCenterV()
{
    KMacroCommand *macro= 0L;
    KCommand *cmd=0L;
    KPresenterDoc *doc =m_view->kPresenterDoc();
    KoRect rect = (numberOfObjectSelected() > 1) ? getAlignBoundingRect() : activePage()->getPageRect();

    cmd=activePage()->alignObjsCenterV(rect);
    if(cmd)
    {
        if ( !macro )
            macro= new KMacroCommand(i18n( "Align Objects Center/Vertical" ));
        macro->addCommand(cmd);
    }
    cmd=doc->stickyPage()->alignObjsCenterV(rect);
    if(cmd)
    {
        if ( !macro )
            macro= new KMacroCommand(i18n( "Align Objects Center/Vertical" ));

        macro->addCommand(cmd);
    }
    if(macro)
        doc->addCommand(macro);
}

void KPrCanvas::alignObjBottom()
{
    KMacroCommand *macro=0L;
    KCommand *cmd=0L;
    KPresenterDoc *doc =m_view->kPresenterDoc();
    KoRect rect = (numberOfObjectSelected() > 1) ? getAlignBoundingRect() : activePage()->getPageRect();

    cmd=activePage()->alignObjsBottom(rect);
    if(cmd)
    {
        if ( !macro )
            macro= new KMacroCommand(i18n( "Align Objects Bottom" ));
        macro->addCommand(cmd);
    }
    cmd=doc->stickyPage()->alignObjsBottom(rect);
    if(cmd)
    {
        if ( !macro )
            macro= new KMacroCommand(i18n( "Align Objects Bottom" ));
        macro->addCommand(cmd);
    }
    if(macro)
        doc->addCommand(macro);
}

bool KPrCanvas::canMoveOneObject() const
{
    return (m_activePage->canMoveOneObject() || stickyPage()->canMoveOneObject());
}

void KPrCanvas::rectSymetricalObjet()
{
    if ( m_drawSymetricObject )
    {
        m_drawSymetricObject = false;
        insRect.moveBy( -insRect.width(), -insRect.height());
        insRect.setSize( 2*insRect.size() );
    }
}

bool KPrCanvas::getSticky( bool sticky ) const
{
    bool result =m_activePage->getSticky( sticky );
    if ( result != sticky)
        return result;
    return stickyPage()->getSticky( sticky );
}

bool KPrCanvas::getProtect( bool prot ) const
{
    bool result =m_activePage->getProtect( prot );
    if ( result != prot)
        return result;
    return stickyPage()->getProtect( prot );
}

bool KPrCanvas::differentProtect( bool p )const
{
    bool result = m_activePage->differentProtect( p );
    if ( result )
        return true;
    return stickyPage()->differentProtect( p );
}

bool KPrCanvas::getKeepRatio( bool _ratio ) const
{
    bool result =m_activePage->getKeepRatio( _ratio );
    if ( result != _ratio)
        return result;
    return stickyPage()->getKeepRatio( _ratio );
}

bool KPrCanvas::differentKeepRatio( bool p )const
{
    bool result = m_activePage->differentKeepRatio( p );
    if ( result )
        return true;
    return stickyPage()->differentKeepRatio( p );
}

QPen KPrCanvas::getPen( const QPen & _pen )const
{
    QPen pen = m_activePage->getPen( _pen );
    if ( pen != _pen )
        return pen;
    return stickyPage()->getPen( _pen);
}

QBrush KPrCanvas::getBrush( const QBrush & _brush )const
{
    QBrush brush = m_activePage->getBrush( _brush );
    if ( brush != _brush )
        return brush;
    return stickyPage()->getBrush( _brush);
}

LineEnd KPrCanvas::getLineBegin( LineEnd  _end )const
{
    LineEnd end = m_activePage->getLineBegin( _end );
    if ( end != _end )
        return end;
    return stickyPage()->getLineBegin( _end);
}

LineEnd KPrCanvas::getLineEnd( LineEnd  _end )const
{
    LineEnd end = m_activePage->getLineEnd( _end );
    if ( end != _end )
        return end;
    return stickyPage()->getLineEnd( _end);
}

FillType KPrCanvas::getFillType( FillType _fillType ) const
{
    FillType fill = m_activePage->getFillType( _fillType );
    if ( fill != _fillType )
        return fill;
    return stickyPage()->getFillType( _fillType);

}

QColor KPrCanvas::getGColor1( const QColor & _col )const
{
    QColor col = m_activePage->getGColor1( _col );
    if ( col != _col )
        return col;
    return stickyPage()->getGColor1( _col);
}

QColor KPrCanvas::getGColor2( const QColor & _col )const
{
    QColor col = m_activePage->getGColor2( _col );
    if ( col != _col )
        return col;
    return stickyPage()->getGColor2( _col);
}

BCType KPrCanvas::getGType( BCType _gt )const
{
    BCType type = m_activePage->getGType( _gt );
    if ( type != _gt )
        return type;
    return stickyPage()->getGType( _gt);
}

bool KPrCanvas::getGUnbalanced( bool _g )const
{
    bool type= m_activePage->getGUnbalanced( _g );
    if ( type != _g )
        return type;
    return stickyPage()->getGUnbalanced( _g);
}

int KPrCanvas::getGXFactor( int _g )const
{
    int type= m_activePage->getGXFactor( _g );
    if ( type != _g )
        return type;
    return stickyPage()->getGXFactor( _g);
}

int KPrCanvas::getGYFactor( int _g )const
{
    int type= m_activePage->getGYFactor( _g );
    if ( type != _g )
        return type;
    return stickyPage()->getGYFactor( _g);
}

int KPrCanvas::getRndY( int _ry )const
{
    int value = m_activePage->getRndY(_ry);
    if (value != _ry)
        return value;
    return stickyPage()->getRndY(_ry);
}

int KPrCanvas::getRndX( int _rx )const
{
    int value = m_activePage->getRndX(_rx);
    if (value != _rx)
        return value;
    return stickyPage()->getRndX(_rx);
}

int KPrCanvas::getPieAngle( int pieAngle )const
{
    int value = m_activePage->getPieAngle(pieAngle);
    if (value != pieAngle)
        return value;
    return stickyPage()->getPieAngle(pieAngle);
}

int KPrCanvas::getPieLength( int pieLength )const
{
    int value = m_activePage->getPieLength(pieLength);
    if (value != pieLength)
        return value;
    return stickyPage()->getPieLength(pieLength);
}

PieType KPrCanvas::getPieType( PieType pieType )const
{
    PieType type = m_activePage->getPieType(pieType);
    if (type != pieType)
        return type;
    return stickyPage()->getPieType(pieType);
}

bool KPrCanvas::getCheckConcavePolygon( bool check ) const
{
    bool value = m_activePage->getCheckConcavePolygon(check);
    if (value != check)
        return value;
    return stickyPage()->getCheckConcavePolygon(check);
}

int KPrCanvas::getCornersValue( int corners ) const
{
    int value = m_activePage->getCornersValue(corners);
    if (value != corners)
        return value;
    return stickyPage()->getCornersValue(corners);
}

int KPrCanvas::getSharpnessValue( int sharpness ) const
{
    int value = m_activePage->getSharpnessValue(sharpness);
    if (value != sharpness)
        return value;
    return stickyPage()->getSharpnessValue(sharpness);
}

PictureMirrorType KPrCanvas::getPictureMirrorType( PictureMirrorType type ) const
{
    PictureMirrorType value = m_activePage->getPictureMirrorType(type);
    if (value != type)
        return value;
    return stickyPage()->getPictureMirrorType(type);
}

int KPrCanvas::getPictureDepth( int depth ) const
{
    int value = m_activePage->getPictureDepth(depth);
    if (value != depth)
        return value;
    return stickyPage()->getPictureDepth(depth);
}

bool KPrCanvas::getPictureSwapRGB( bool swapRGB ) const
{
    bool value = m_activePage->getPictureSwapRGB(swapRGB);
    if (value != swapRGB)
        return value;
    return stickyPage()->getPictureSwapRGB(swapRGB);
}

bool KPrCanvas::getPictureGrayscal( bool grayscal ) const
{
    bool value = m_activePage->getPictureGrayscal(grayscal);
    if (value != grayscal)
        return value;
    return stickyPage()->getPictureGrayscal(grayscal);
}

int KPrCanvas::getPictureBright( int bright ) const
{
    int value = m_activePage->getPictureBright(bright);
    if (value != bright)
        return value;
    return stickyPage()->getPictureBright(bright);
}

QPixmap KPrCanvas::getPicturePixmap() const
{
    QPixmap pixmap = m_activePage->getPicturePixmap();
    if (!pixmap.isNull())
        return pixmap;
    return stickyPage()->getPicturePixmap();
}

KCommand *KPrCanvas::setProtectContent( bool b )
{
    KMacroCommand *macro = 0L;
    QPtrList<KPTextObject> list;
    QPtrListIterator<KPObject> it(getObjectList());
    for ( ; it.current(); ++it ) {
        if ( it.current()->isSelected() && it.current()->getType()==OT_TEXT)
        {
            if ( !macro )
                macro = new KMacroCommand( i18n("Protect Content"));
            KPrProtectContentCommand * cmd = new KPrProtectContentCommand( i18n("Protect Content"), b,
                                                                           static_cast<KPTextObject *>(it.current()),
                                                                           m_view->kPresenterDoc() );
            cmd->execute();
            macro->addCommand( cmd );
        }
    }
    //get sticky obj
    it=stickyPage()->objectList();
    for ( ; it.current(); ++it ) {
        if ( it.current()->isSelected() && it.current()->getType()==OT_TEXT)
        {
            if ( !macro )
                macro = new KMacroCommand( i18n("Protect Content"));
            KPrProtectContentCommand * cmd = new KPrProtectContentCommand( i18n("Protect Content"), b,
                                                                           static_cast<KPTextObject *>(it.current()),
                                                                           m_view->kPresenterDoc() );
            cmd->execute();
            macro->addCommand( cmd );
        }
    }
    return macro;
}

bool KPrCanvas::getProtectContent(bool prot) const
{
    bool result =m_activePage->getProtectContent(prot);
    if ( result != prot)
        return result;
    return stickyPage()->getProtectContent( prot );
}

void KPrCanvas::closeObject(bool /*close*/)
{
    QPtrList<KPObject> lst;
    QPtrListIterator<KPObject> it(getObjectList());
    for ( ; it.current(); ++it ) {
        if ( it.current()->isSelected()
             && (it.current()->getType() == OT_POLYLINE || it.current()->getType() == OT_FREEHAND
                 || it.current()->getType() == OT_QUADRICBEZIERCURVE
                 || it.current()->getType() == OT_CUBICBEZIERCURVE ))
            lst.append( it.current()  );
    }
    //get sticky obj
    it=stickyPage()->objectList();
    for ( ; it.current(); ++it ) {
        if ( it.current()->isSelected()
             && (it.current()->getType() == OT_POLYLINE ||it.current()->getType() == OT_FREEHAND
                 || it.current()->getType() == OT_QUADRICBEZIERCURVE
                 || it.current()->getType() == OT_CUBICBEZIERCURVE ))
            lst.append(  it.current() );
    }
    if ( lst.isEmpty())
        return;

    KMacroCommand *macro = new KMacroCommand( i18n("Close Object"));
    QPtrListIterator<KPObject> it2( lst );
    for ( ; it2.current() ; ++it2 ) {
        KCommand * cmd= new KPrCloseObjectCommand(i18n("Close Object"), it2.current(), m_view->kPresenterDoc());
        macro->addCommand(cmd);
    }
    macro->execute();
    m_view->kPresenterDoc()->addCommand(macro);
}

void KPrCanvas::layout()
{
    QPtrListIterator<KPObject> it(getObjectList());
    for ( ; it.current(); ++it ) {
        if ( it.current()->getType() == OT_TEXT )
            static_cast<KPTextObject *>( it.current() )->layout();
    }
    it=stickyPage()->objectList();
    for ( ; it.current(); ++it ) {
        if ( it.current()->getType() == OT_TEXT )
            static_cast<KPTextObject *>( it.current() )->layout();
    }
}

QPoint KPrCanvas::applyGrid( const QPoint &pos,bool offset )
{
    bool state =m_view->kPresenterDoc()->snapToGrid();
    if (  !state && offset )
        return pos;

    double gridX = m_view->kPresenterDoc()->getGridX();
    double gridY = m_view->kPresenterDoc()->getGridY();
    if ( !state && ! offset )
    {
        gridX = 1.0;
        gridY = 1.0;
    }
    KoPoint newPos;
    if (offset )
        newPos = m_view->kPresenterDoc()->zoomHandler()->unzoomPoint( pos+QPoint(diffx(),diffy()) );
    else
        newPos = m_view->kPresenterDoc()->zoomHandler()->unzoomPoint( pos );
    newPos.setX( static_cast<int>( newPos.x() / gridX ) * gridX );
    newPos.setY( static_cast<int>( newPos.y() / gridY ) * gridY );
    QPoint point( m_view->kPresenterDoc()->zoomHandler()->zoomPoint( newPos ) );
    if ( offset )
      point -= QPoint( diffx(), diffy() );
    return point;
}

int KPrCanvas::applyGridOnPosX( int pos ) const
{
    double gridX = m_view->kPresenterDoc()->getGridX();
    double point = m_view->kPresenterDoc()->zoomHandler()->unzoomItX( pos );
    double result = static_cast<int>( point / gridX ) * gridX;
    return m_view->kPresenterDoc()->zoomHandler()->zoomItX( result );
}

int KPrCanvas::applyGridOnPosY( int pos ) const
{
    double gridY = m_view->kPresenterDoc()->getGridY();
    double point = m_view->kPresenterDoc()->zoomHandler()->unzoomItY( pos );
    double result = static_cast<int>( point / gridY ) * gridY;
    return m_view->kPresenterDoc()->zoomHandler()->zoomItY( result );
}

void KPrCanvas::alignVertical( VerticalAlignmentType _type )
{
    KMacroCommand *macro = 0L;
    KCommand *cmd = m_activePage->alignVertical( _type );
    if ( cmd )
    {
        if ( !macro )
            macro = new KMacroCommand( i18n("Change Vertical Alignment"));
        macro->addCommand(cmd );
    }
    cmd = stickyPage()->alignVertical( _type );
    if ( cmd )
    {
        if ( !macro )
            macro = new KMacroCommand( i18n("Change Vertical Alignment"));

        macro->addCommand(cmd );
    }

    if ( macro )
        m_view->kPresenterDoc()->addCommand( macro );
}

ImageEffect KPrCanvas::getImageEffect(ImageEffect eff) const
{
    ImageEffect value = m_activePage->getImageEffect(eff);
    if (value != eff)
        return value;
    return stickyPage()->getImageEffect(eff);
}

KPPixmapObject * KPrCanvas::getSelectedImage() const
{
    KPPixmapObject *obj=activePage()->getSelectedImage();
    if(obj)
        return obj;
    obj=stickyPage()->getSelectedImage();
    return obj;
}


double KPrCanvas::applyGridX( double x )
{
    if (  !m_view->kPresenterDoc()->snapToGrid() )
        return x;

    double gridX = m_view->kPresenterDoc()->getGridX();
    return qRound( x / gridX ) * gridX;
}


double KPrCanvas::applyGridY( double y )
{
    if (  !m_view->kPresenterDoc()->snapToGrid() )
        return y;

    double gridY = m_view->kPresenterDoc()->getGridY();
    return qRound( y / gridY ) * gridY;
}


KoPoint KPrCanvas::applyGrid( const KoPoint &pos )
{
    if (  !m_view->kPresenterDoc()->snapToGrid() )
        return pos;

    KoPoint newPos;
    newPos.setX( applyGridX( pos.x() ) );
    newPos.setY( applyGridY( pos.y() ) );

    return newPos;
}
