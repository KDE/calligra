/* This file is part of the KDE project
   Copyright (C) 1998, 1999 Reginald Stadlbauer <reggie@kde.org>
   Copyright (C) 2002-2005 Thorsten Zachmann <zachmann@kde.org>

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

#include <stdlib.h>
#include <math.h>

#include <qprogressdialog.h>
#include <qfile.h>
#include <qtextstream.h>
#include <qpainter.h>
#include <qpaintdevicemetrics.h>
#include <qwmatrix.h>
#include <qapplication.h>
#include <kpopupmenu.h>
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
#include <kotextzoomhandler.h>
#include <koStore.h>
#include <koStoreDrag.h>
#include "koPointArray.h"

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
#include "kprcommand.h"
#include "kppolylineobject.h"
#include "kpclosedlineobject.h"
#include "kprpage.h"
#include "kprvariable.h"
#include "kpgroupobject.h"
#include "kpresenter_doc.h"

#include "kprcanvas.h"
#include "kprcanvas.moc"

#include "effecthandler.h"
#include "pageeffects.h"
#include <unistd.h>

KPrCanvas::KPrCanvas( QWidget *parent, const char *name, KPresenterView *_view )
: QWidget( parent, name, WStaticContents|WResizeNoErase|WRepaintNoErase )
, buffer( size() )
, m_gl( _view, _view->zoomHandler() )
, m_autoGl( _view, _view->zoomHandler(), true )
, m_moveGuides( false )    
{
    m_presMenu = 0;
    m_currentTextObjectView=0L;
    m_activePage=0L;
    m_xOffset = 0;
    m_yOffset = 0;

    m_keyPressEvent = false;
    m_drawSymetricObject = false;
    if ( parent ) {
        showingLastSlide = false;
        mousePressed = false;
        drawContour = false;
        modType = MT_NONE;
        m_resizeObject = 0L;
        editNum = 0L;
        m_rotateObject = 0L;
        setBackgroundMode( Qt::NoBackground );
        m_view = _view;
        setupMenus();
        setMouseTracking( true );
        show();
        editMode = true;
        m_step.m_pageNumber = 0;
        m_step.m_step = 0;
        m_step.m_subStep = 0;
        goingBack = false;
        m_drawMode = false;
        fillBlack = true;
        drawRubber = false;
        m_zoomRubberDraw = false;
        toolEditMode = TEM_MOUSE;
        setAcceptDrops( true );
        m_ratio = 0.0;
        m_keepRatio = false;
        m_isMoving = false;
        m_isResizing = false;
        mouseSelectedObject = false;
        selectedObjectPosition = -1;
        m_setPageTimer = true;
        m_drawLineInDrawMode = false;
        soundPlayer = 0;
        m_changeSnap = false;
        m_drawPolyline = false;
        m_drawCubicBezierCurve = false;
        m_drawLineWithCubicBezierCurve = true;
        m_oldCubicBezierPointArray.putPoints( 0, 4, 0.0, 0.0, 0.0, 0.0,
                                              0.0, 0.0, 0.0, 0.0 );
        m_effectHandler = 0;
        m_pageEffect = 0;
    } else {
        m_view = 0;
        hide();
    }

    setFocusPolicy( QWidget::StrongFocus );
    setFocus();
    setKeyCompression( true );
    setInputMethodEnabled( true ); // ensure using the InputMethod
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
    delete m_currentTextObjectView;
    m_currentTextObjectView = 0;
    if ( editNum )
        editNum = 0;

    exitEditMode();

    delete m_presMenu;

    stopSound();
    delete soundPlayer;
    blockSignals(false);
}

void KPrCanvas::scrollX( int x )
{
    // Relative movement
    int dx = m_xOffset - x;
    // new position
    m_xOffset = x;
    bitBlt( &buffer, dx, 0, &buffer );
    scroll( dx, 0 );
}

void KPrCanvas::scrollY( int y )
{
    // Relative movement
    int dy = m_yOffset - y;
    // new position
    m_yOffset = y;
    bitBlt( &buffer, 0, dy, &buffer );
    scroll( 0, dy );
}

bool KPrCanvas::eventFilter( QObject *o, QEvent *e )
{
    if ( !o || !e )
        return TRUE;
    if ( m_currentTextObjectView  )
        KCursor::autoHideEventFilter( o, e );
    switch ( e->type() )
    {
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
        QKeyEvent * keyev = static_cast<QKeyEvent *>(e);
        if ( m_currentTextObjectView &&
                (keyev->key()==Qt::Key_Home ||keyev->key()==Key_End
                 || keyev->key()==Qt::Key_Tab || keyev->key()==Key_Prior
                 || keyev->key()==Qt::Key_Next || keyev->key() == Key_Backtab) )
        {
            m_currentTextObjectView->keyPressEvent( keyev );
            return true;
        }
        else if ( !m_currentTextObjectView && keyev->key()==Qt::Key_Tab  )
        {
            keyPressEvent(keyev);
            return true;
        }
        break;
    }
    case QEvent::AccelOverride:
    {
#ifndef NDEBUG
      QKeyEvent * keyev = static_cast<QKeyEvent *>(e);
        // Debug keys
        if ( ( keyev->state() & ControlButton ) && ( keyev->state() & ShiftButton ) )
        {
            switch ( keyev->key() ) {
            case Qt::Key_P: // 'P' -> paragraph debug
                printRTDebug( 0 );
                keyev->accept();
                break;
            case Qt::Key_V: // 'V' -> verbose parag debug
                printRTDebug( 1 );
                keyev->accept();
                break;
            default:
                break;
            }
        }
#endif
        break;
    }
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
        KPresenterDoc *doc =m_view->kPresenterDoc();

        if ( ! m_moveGuides  )
        {
            //kdDebug(33001) << "KPrCanvas::paintEvent" << endl;
            QPainter bufPainter;
            bufPainter.begin( &buffer, this ); // double-buffering - (the buffer is as big as the widget)
            bufPainter.translate( -diffx(), -diffy() );
            bufPainter.setBrushOrigin( -diffx(), -diffy() );

            QRect crect( paintEvent->rect() ); // the rectangle that needs to be repainted, in widget coordinates
            bufPainter.setClipRect( crect );

            //kdDebug(33001) << "KPrCanvas::paintEvent " << DEBUGRECT( crect ) << ", " << size() << endl;

            crect.moveBy( diffx(), diffy() ); // now in contents coordinates
            //kdDebug(33001) << "KPrCanvas::paintEvent after applying diffx/diffy: " << DEBUGRECT( crect ) << endl;

            if ( editMode || !fillBlack )
                bufPainter.fillRect( crect, white );
            else
                bufPainter.fillRect( crect, black );

            KPrPage * page = editMode ? m_activePage : doc->pageList().at( m_step.m_pageNumber );
            drawBackground( &bufPainter, crect, page, editMode );

            if ( editMode )
            {
                SelectionMode selectionMode;

                if ( toolEditMode == TEM_MOUSE || toolEditMode == TEM_ZOOM )
                    selectionMode = SM_MOVERESIZE;
                else if ( toolEditMode == TEM_ROTATE )
                    selectionMode = SM_ROTATE;
                else
                    selectionMode = SM_NONE;

                if ( doc->showGrid() && !doc->gridToFront() || ( !doc->showGrid() && doc->snapToGrid() && ( toolEditMode != TEM_MOUSE || m_isMoving || m_isResizing ) ) )
                    drawGrid( &bufPainter, crect);

                drawEditPage( &bufPainter, crect, page, selectionMode );

                if ( doc->showGrid() && doc->gridToFront())
                    drawGrid( &bufPainter, crect);
            }
            else
            {
                // Center the slide in the screen, if it's smaller...
#if 0 // this works but isn't enough - e.g. object effects need the same offsets
                // so we should store them, but they don't work like diffx/diffy...
                // (e.g. the painter mustn't be translated when painting the background)
                QRect desk = KGlobalSettings::desktopGeometry(this);
                QRect pgRect = m_view->kPresenterDoc()->pageList().at(0)->getZoomPageRect();
                int offx = 0, offy = 0;
                if ( desk.width() > pgRect.width() )
                    offx = ( desk.width() - pgRect.width() ) / 2;
                if ( desk.height() > pgRect.height() )
                    offy = ( desk.height() - pgRect.height() ) / 2;
                bufPainter.translate( offx, offy );
#endif

                PresStep step( m_step.m_pageNumber, m_step.m_step, m_step.m_subStep, m_effectTimer.isActive(), !goingBack );
                drawPresPage( &bufPainter, crect, step );
                if ( m_drawMode && m_drawModeLines.count() )
                {
                    bufPainter.save();
                    bufPainter.setPen( m_view->kPresenterDoc()->presPen() );
                    for ( unsigned int i = 0; i < m_drawModeLines.count(); ++i )
                    {
                        bufPainter.drawPolyline( m_drawModeLines[i] );
                    }
                    bufPainter.restore();
                }
            }

            bufPainter.end();
        }


        if ( editMode && doc->showHelplines() ) 
        {
            QPixmap m_guideBuffer( buffer );
            QPainter guidePainter( &m_guideBuffer, &buffer );
            guidePainter.translate( -diffx(), -diffy() );
            guidePainter.setBrushOrigin( -diffx(), -diffy() );
            m_gl.paintGuides( guidePainter );
            m_autoGl.paintGuides( guidePainter );
            guidePainter.end();
            bitBlt( this, paintEvent->rect().topLeft(), &m_guideBuffer, paintEvent->rect() );
        }
        else
        {
            bitBlt( this, paintEvent->rect().topLeft(), &buffer, paintEvent->rect() );
        }
    }
    //else kdDebug(33001) << "KPrCanvas::paintEvent with updates disabled" << endl;
}


void KPrCanvas::drawBackground( QPainter *painter, const QRect& rect, KPrPage * page, bool edit ) const
{
    if ( edit )
    {
        QRect pageRect = page->getZoomPageRect();

        if ( rect.intersects( pageRect ) )
        {
            if ( page->displayBackground() )
            {
                if ( page->masterPage() && page->useMasterBackground() )
                    page->masterPage()->background()->drawBackground( painter, m_view->zoomHandler(), rect, true );
                else
                    page->background()->drawBackground( painter, m_view->zoomHandler(), rect, true );
            }
        }
        // Include the border
        pageRect.rLeft() -= 1;
        pageRect.rTop() -= 1;
        pageRect.rRight() += 1;
        pageRect.rBottom() += 1;

        QRegion grayRegion( rect );
        grayRegion -= pageRect;

        // In edit mode we also want to draw the gray area out of the pages
        if ( !grayRegion.isEmpty() )
            eraseEmptySpace( painter, grayRegion, QApplication::palette().active().brush( QColorGroup::Mid ) );
    }
    else
    {
        // Old code, left a black area if zoomX != zoomY
        //page->background()->draw( painter, m_view->zoomHandler(), rect, false );

        QRect desk = KGlobalSettings::desktopGeometry(getView());
        QRect crect = desk.intersect( rect );
        if ( crect.isEmpty() || !page->displayBackground())
            return;

        if ( page->masterPage() && page->useMasterBackground() )
            page->masterPage()->background()->drawBackground( painter, desk.size(), crect, false );
        else
            page->background()->drawBackground( painter, desk.size(), crect, false );
    }
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


void KPrCanvas::drawObjects( QPainter *painter, const QPtrList<KPObject> &objects, SelectionMode selectionMode,
                             bool contour, KPTextView * textView, int pageNum ) const
{
    QPtrListIterator<KPObject> it( objects );
    for ( ; it.current(); ++it )
    {
        SelectionMode selMode = selectionMode;
        if ( selectionMode != SM_NONE
             && it.current()->isSelected()
             && ( m_view->kPresenterDoc()->isHeaderFooter(it.current())
                  || it.current()->isProtect() ) )
            selMode = SM_PROTECT;

        it.current()->draw( painter, m_view->zoomHandler(), pageNum, selMode, (it.current()->isSelected()) && contour );

        it.current()->setSubPresStep( 0 );
        it.current()->doSpecificEffects( false );
    }

    if ( textView )
    {
        textView->kpTextObject()->paintEdited( painter, m_view->zoomHandler(), false /*onlyChanged. Pass as param ?*/,
                                               textView->cursor(), true /* idem */ );
    }
}


void KPrCanvas::drawObjectsPres( QPainter *painter, const QPtrList<KPObject> &_objects, PresStep step ) const
{
    QPtrList<KPObject> objects;

    QPtrListIterator<KPObject> it( _objects );
    for ( ; it.current(); ++it )
    {
        if ( objectIsAHeaderFooterHidden(it.current()) )
            continue;
        if ( it.current()->getAppearStep() <= step.m_step
             && ( ! it.current()->getDisappear()
                  || it.current()->getDisappear()
                     && it.current()->getDisappearStep() > step.m_step ) )
        {
            if ( step.m_animate && it.current()->getAppearStep() == step.m_step && it.current()->getEffect() != EF_NONE )
                continue;

            if ( step.m_animateSub && it.current()->getAppearStep() == step.m_step )
            {
                it.current()->setSubPresStep( step.m_subStep );
                it.current()->doSpecificEffects( true, false );
            }

            objects.append( it.current() );
        }
    }
    drawObjects( painter, objects, SM_NONE, false, NULL, step.m_pageNumber );
}


void KPrCanvas::drawObjectsEdit( QPainter *painter, const KoRect &rect, const QPtrList<KPObject> &_objects,
                                 SelectionMode selectionMode, int pageNum ) const
{
    QPtrList<KPObject> objects;

    KPTextView * textView = NULL;
    QPtrListIterator<KPObject> it( _objects );
    for ( ; it.current(); ++it )
    {
        if ( objectIsAHeaderFooterHidden(it.current()) )
            continue;

        if ( rect.intersects( it.current()->getRepaintRect() ) )
        {
            if ( m_currentTextObjectView && m_currentTextObjectView->kpTextObject() == it.current() )
            {
                textView = m_currentTextObjectView;
            }

            objects.append( it.current() );
        }
    }
    drawObjects( painter, objects, selectionMode, drawContour, textView, pageNum );
}


void KPrCanvas::drawEditPage( QPainter *painter, const QRect &_rect,
                              KPrPage *page, SelectionMode selectionMode ) const
{
    KoRect rect = m_view->zoomHandler()->unzoomRect(_rect);

    int pageNum = m_view->kPresenterDoc()->pageList().findRef( page );
    //objects in current page
    drawObjectsEdit( painter, rect, page->objectList(), selectionMode, pageNum );
    //draw sticky object
    if ( page->masterPage() && page->displayObjectFromMasterPage() )
        drawObjectsEdit( painter, rect, page->masterPage()->objectList(), selectionMode, pageNum );
}


void KPrCanvas::drawPresPage( QPainter *painter, const QRect &_rect, PresStep step ) const
{
    KoRect rect = m_view->zoomHandler()->unzoomRect(_rect);

    KPrPage * page = m_view->kPresenterDoc()->pageList().at(step.m_pageNumber);
    //objects in current page
    drawObjectsPres( painter, page->objectList(), step );
    //draw master page object
    if (  page->masterPage() && page->displayObjectFromMasterPage() )
        drawObjectsPres( painter, page->masterPage()->objectList(), step );
}


void KPrCanvas::drawGrid(QPainter *painter, const QRect &rect2) const
{
    KPresenterDoc *doc=m_view->kPresenterDoc();

    if(!doc->isReadWrite())
        return;
    QPen _pen = QPen( doc->gridColor(), 6, Qt::DotLine );
    painter->save();
    painter->setPen( _pen );
    QRect pageRect = m_activePage->getZoomPageRect();

    int zoomedX,  zoomedY;
    double offsetX = doc->getGridX();
    double offsetY = doc->getGridY();

    for ( double i = offsetX; ( zoomedX = m_view->zoomHandler()->zoomItX( i )+pageRect.left() ) < pageRect.right(); i += offsetX )
        for ( double j = offsetY; ( zoomedY = m_view->zoomHandler()->zoomItY( j )+pageRect.top() ) < pageRect.bottom(); j += offsetY )
            if( rect2.contains( zoomedX, zoomedY ) )
                painter->drawPoint( zoomedX, zoomedY );

    painter->restore();
}


// This one is used to generate the pixmaps for the HTML presentation,
// for the pres-structure-dialog, for the sidebar previews, for template icons.
void KPrCanvas::drawAllObjectsInPage( QPainter *painter, const QPtrList<KPObject> & obj, int pageNum ) const
{
    QPtrListIterator<KPObject> it( obj );
    for ( ; it.current(); ++it ) {
        if ( objectIsAHeaderFooterHidden( it.current() ) )
            continue;
        it.current()->draw( painter, m_view->zoomHandler(), pageNum, SM_NONE, false );
    }
}

void KPrCanvas::recalcAutoGuides( )
{
    QValueList<double> horizontalPos;
    QValueList<double> verticalPos;
    QPtrListIterator<KPObject> it( m_activePage->objectList() );
    for ( ; it.current(); ++it )
    {
        if( ! it.current()->isSelected() )
            it.current()->addSelfToGuides( horizontalPos, verticalPos);
    }

    m_autoGl.setGuideLines( horizontalPos, verticalPos );
}

void KPrCanvas::mousePressEvent( QMouseEvent *e )
{
    QPoint contentsPoint( e->pos().x()+diffx(), e->pos().y()+diffy() );
    KoPoint docPoint = m_view->zoomHandler()->unzoomPoint( contentsPoint );

    if(!m_view->koDocument()->isReadWrite())
        return;

    if ( editMode && toolEditMode == TEM_MOUSE && m_gl.mousePressEvent( e ) )
        return;

    m_moveStartPosMouse = objectRect( false ).topLeft();
    if(m_currentTextObjectView)
    {
        KPTextObject *txtObj=m_currentTextObjectView->kpTextObject();
        Q_ASSERT(txtObj);
        if(txtObj->contains( docPoint ))
        {
            KoPoint pos = docPoint - txtObj->innerRect().topLeft(); // in pt, but now translated into the object's coordinate system
            // This is common to all mouse buttons, so that RMB and MMB place the cursor too
            m_currentTextObjectView->mousePressEvent(e, m_view->zoomHandler()->ptToLayoutUnitPix( pos ) ); // in LU pixels
            mousePressed = true;
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
            return;
        }
    }


    KPObject *kpobject = 0;

    m_savedMousePos = contentsPoint;

    bool const doApplyGrid = !( ( (e->state() & ShiftButton) && m_view->kPresenterDoc()->snapToGrid() ) || ( !(e->state() & ShiftButton) && !m_view->kPresenterDoc()->snapToGrid() ) );
    m_changeSnap = e->state() & ShiftButton;
    
    QPoint rasterPoint =  e->pos();
    if ( doApplyGrid )
        rasterPoint = applyGrid( e->pos(), true );

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
                firstX = contentsPoint.x();
                firstY = contentsPoint.y();
                KPObject *kpobject = getObjectAt( docPoint, true );

                if ( kpobject ) {
                    // use ctrl + Button to select / deselect object
                    if ( e->state() & ControlButton && kpobject->isSelected() )
                        deSelectObj( kpobject );
                    else if ( e->state() & ControlButton )
                    {
                        selectObj( kpobject );
                        raiseObject( kpobject );
                        m_moveStartPosMouse = objectRect( false ).topLeft();
                    }
                    else
                    {
                        if ( modType != MT_MOVE || !kpobject->isSelected() )
                            deSelectAllObj();

                        selectObj( kpobject );
                        raiseObject( kpobject );
                        m_moveStartPosMouse = objectRect( false ).topLeft();
                    }

                    // start resizing
                    if ( modType != MT_MOVE && modType != MT_NONE )
                    {
                        deSelectAllObj();
                        selectObj( kpobject );
                        raiseObject( kpobject );

                        m_resizeObject = kpobject;

                        m_keepRatio = false;
                        if ( e->state() & ControlButton )
                            m_keepRatio = true;

                        m_keepRatio = m_keepRatio || m_resizeObject->isKeepRatio();
                        m_ratio = static_cast<double>( kpobject->getSize().width() ) /
                                static_cast<double>( kpobject->getSize().height() );
                        m_rectBeforeResize = kpobject->getRect();
                    }
                    recalcAutoGuides();
                }
                else
                {
                    KPrPage *masterPage = m_activePage->masterPage();
                    if ( masterPage && m_activePage->displayObjectFromMasterPage() )
                    {
                        kpobject = masterPage->getObjectAt( docPoint, true );

                        if ( kpobject && objectIsAHeaderFooterHidden( kpobject ) )
                        {
                            kpobject = 0;
                        }
                    }

                    if ( kpobject )
                    {
                        QString msg( i18n( "The object you are trying to select belongs to the master slide. "
                                           "Editing the object can only be done on the master slide.\n"
                                           "Go there now?" ) );
                        if ( KMessageBox::questionYesNo( this, msg ) == KMessageBox::Yes )
                        {
                            getView()->setEditMaster( true );
                        }
                        else
                        {
                            deSelectAllObj();
                            modType = MT_NONE;
                        }
                    }
                    else {
                        modType = MT_NONE;
                        if ( !( e->state() & ShiftButton ) && !( e->state() & ControlButton ) )
                            deSelectAllObj();

                            drawRubber = true;
                            rubber = QRect( e->x(), e->y(), 0, 0 );
                    }
                }
                m_origMousePos = docPoint;
            } break;
            case TEM_ZOOM: {
                modType = MT_NONE;
                drawRubber = true;
                m_zoomRubberDraw = false;
                rubber = QRect( e->x(), e->y(), 0, 0 );
            }break;
            case TEM_ROTATE: {

                firstX = contentsPoint.x();
                firstY = contentsPoint.y();

                KPObject *kpobject = getObjectAt( docPoint );

                // clear old selections even if shift or control are pressed
                // we don't support rotating multiple objects yet
                deSelectAllObj();

                if ( kpobject && !kpobject->isProtect())
                {
                    m_rotateObject = kpobject;
                    m_angleBeforeRotate = kpobject->getAngle();
                    selectObj( kpobject );
                    raiseObject( kpobject );

                    // set center of selected object bounding rect
                    m_rotateCenter = kpobject->getRealRect().center();
                }
            } break;
            case INS_FREEHAND: case INS_CLOSED_FREEHAND: {
                deSelectAllObj();
                mousePressed = true;
                QPoint tmp = e->pos();
                if ( doApplyGrid )
                  tmp = applyGrid ( e->pos(), true );
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
                QPoint tmp = e->pos();
                if ( doApplyGrid )
                  tmp = applyGrid ( e->pos(), true );

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
                QPoint tmp = e->pos();
                if ( doApplyGrid )
                  tmp = applyGrid ( e->pos(), true );

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
                QPoint tmp = e->pos();
                if ( doApplyGrid )
                  tmp = applyGrid ( e->pos(), true );
                insRect = QRect( tmp.x(), tmp.y(), 0, 0 );

                m_indexPointArray = 0;
                m_dragStartPoint = tmp;
                m_dragEndPoint = m_dragStartPoint;
            } break;
            default: {
                deSelectAllObj();
                mousePressed = true;
                QPoint tmp = e->pos();
                if ( doApplyGrid )
                  tmp = applyGrid ( e->pos(), true );
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
                QPoint point = e->pos();
                if ( doApplyGrid )
                  point = applyGrid ( e->pos(), true );
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
            KPObject * obj = getObjectAt( docPoint );
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
            if ( m_drawMode ) {
                setCursor( KPresenterUtils::penCursor() );
                m_drawLineInDrawMode = true;
                m_drawModeLineIndex = 0;
                m_drawModeLines.append( QPointArray() );
                m_drawModeLines[m_drawModeLines.count() - 1].putPoints( m_drawModeLineIndex++, 1, e->x(), e->y() );
            }
            else
                m_view->screenNext();
        } else if ( e->button() == MidButton )
            m_view->screenPrev();
        else if ( e->button() == RightButton ) {
            if ( !m_drawMode && !spManualSwitch() )
            {
                finishObjectEffects();
                finishPageEffect();
                m_view->stopAutoPresTimer();
            }

            setCursor( arrowCursor );
            QPoint pnt = QCursor::pos();
            int ret = m_presMenu->exec( pnt );
            // we have to continue the timer if the menu was canceled and we draw mode is not active
            if ( ret == -1 && !m_presMenu->isItemChecked( PM_DM ) && !spManualSwitch() )
                m_view->continueAutoPresTimer();
        }
    }


#if 0 // Where do you need this ? (toshitaka)
    // ME: I have no idea why this is needed at all
    if ( toolEditMode == TEM_MOUSE )
        mouseMoveEvent( e );
#endif
}

KoRect KPrCanvas::getAlignBoundingRect() const
{
    KoRect boundingRect;

    QPtrListIterator<KPObject> it( m_activePage->objectList() );
    for ( ; it.current() ; ++it )
    {
        if ( it.current() == m_view->kPresenterDoc()->header() ||
                it.current() == m_view->kPresenterDoc()->footer() )
            continue;

        if( it.current()->isSelected() && !it.current()->isProtect() ) {
            boundingRect |= it.current()->getRealRect();
        }
    }
    return boundingRect;
}

void KPrCanvas::mouseReleaseEvent( QMouseEvent *e )
{
    if ( editMode && toolEditMode == TEM_MOUSE && m_gl.mouseReleaseEvent( e ) )
        return;

    QPoint contentsPoint( e->pos().x()+diffx(), e->pos().y()+diffy() );
    if(m_currentTextObjectView)
    {
        m_currentTextObjectView->mouseReleaseEvent( e, contentsPoint );
        emit objectSelectedChanged();
        mousePressed=false;
        return;
    }

    if ( e->button() != LeftButton )
        return;

    if ( m_drawMode ) {
        m_drawLineInDrawMode = false;
        m_drawModeLines[m_drawModeLines.count() - 1].putPoints( m_drawModeLineIndex++, 1, contentsPoint.x(), contentsPoint.y() );
        return;
    }
    bool state = !( ( (e->state() & ShiftButton) && m_view->kPresenterDoc()->snapToGrid() ) || ( !(e->state() & ShiftButton) && !m_view->kPresenterDoc()->snapToGrid() ) );
    int mx = state ? applyGridOnPosX( contentsPoint.x()) : contentsPoint.x();
    int my = state ? applyGridOnPosY( contentsPoint.y()) : contentsPoint.y();
    firstX = state ? applyGridOnPosX( firstX) : firstX;
    firstY = state ? applyGridOnPosY( firstY) : firstY;
    QPtrList<KPObject> _objects;
    _objects.setAutoDelete( false );

    if ( ( m_drawPolyline && ( toolEditMode == INS_POLYLINE || toolEditMode == INS_CLOSED_POLYLINE ) )
         || ( m_drawCubicBezierCurve && ( toolEditMode == INS_CUBICBEZIERCURVE || toolEditMode == INS_QUADRICBEZIERCURVE
                                          || toolEditMode == INS_CLOSED_CUBICBEZIERCURVE || toolEditMode == INS_CLOSED_QUADRICBEZIERCURVE ) ) ) {
        return;
    }

    if ( toolEditMode != INS_LINE )
        insRect = insRect.normalize();

    switch ( toolEditMode ) {
    case TEM_MOUSE: {
        drawContour = FALSE;
        switch ( modType ) {
        case MT_NONE: {
            if ( drawRubber ) {
                // used for selecting multiple object in with the mouse
                QPainter p;
                p.begin( this );
                p.setRasterOp( NotROP );
                p.setPen( QPen( black, 0, DotLine ) );
                p.drawRect( rubber );
                p.end();
                drawRubber = false;

                rubber = rubber.normalize();
                rubber.moveBy(diffx(),diffy());

                KoRect selectedRect = m_view->zoomHandler()->unzoomRect( rubber );
                QPtrListIterator<KPObject> it( getObjectList() );
                for ( ; it.current() ; ++it )
                {
                    if ( it.current()->intersects( selectedRect ) )
                    {
                        if( objectIsAHeaderFooterHidden(it.current()))
                            continue;
                        it.current()->setSelected( true );
                        mouseSelectedObject = true;
                    }
                }

                if ( mouseSelectedObject )
                    _repaint( false );
                emit objectSelectedChanged();
            }
        } break;
        case MT_MOVE: {
            if ( firstX != mx || firstY != my ) {
                KoPoint move( objectRect( false ).topLeft() - m_moveStartPosMouse );
                KCommand *cmd=m_activePage->moveObject(m_view, move.x(), move.y());
                if(cmd)
                    m_view->kPresenterDoc()->addCommand( cmd );
            } else
            {
                m_activePage->repaintObj();
            }
            m_isMoving = false;
            if ( !m_view->kPresenterDoc()->showGrid() && m_view->kPresenterDoc()->snapToGrid() )
                repaint( false );
        }
            break;
        case MT_RESIZE_UP:
            finishResizeObject( i18n( "Resize Object Up" ), mx, my );
            break;
        case MT_RESIZE_DN:
            finishResizeObject( i18n( "Resize Object Down" ), mx, my, false );
            break;
        case MT_RESIZE_LF:
            finishResizeObject( i18n( "Resize Object Left" ), mx, my );
            break;
        case MT_RESIZE_RT:
            finishResizeObject( i18n( "Resize Object Right" ), mx, my );
            break;
        case MT_RESIZE_LU:
            finishResizeObject( i18n( "Resize Object Left && Up" ), mx, my );
            break;
        case MT_RESIZE_LD:
            finishResizeObject( i18n( "Resize Object Left && Down" ), mx, my );
            break;
        case MT_RESIZE_RU:
            finishResizeObject( i18n( "Resize Object Right && Up" ), mx, my );
            break;
        case MT_RESIZE_RD:
            finishResizeObject( i18n( "Resize Object Right && Down" ), mx, my );
            break;
        }
    } break;
    case INS_TEXT: {
        if ( !insRect.isNull() ) {
            rectSymetricalObjet();
            KPTextObject* kptextobject = insertTextObject( insRect );
            setToolEditMode( TEM_MOUSE );

            // User-friendlyness: automatically start editing this textobject
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
            if ( m_zoomRubberDraw )
            {
                m_view->setZoomRect( m_view->zoomHandler()->unzoomRect( rubber ) );
            }
            else
            {
                m_view->zoomPlus();
            }
            m_zoomRubberDraw = false;
        }
    }break;
    case TEM_ROTATE: {
        drawContour = FALSE;
        if ( !m_rotateObject )
            break;
        if ( m_angleBeforeRotate != m_rotateObject->getAngle() ) {
            QPtrList<KPObject> objects;
            objects.append( m_rotateObject );

            /* As the object is allready rotated set the angle to
             * the m_angleBeforeRotate for the creation of the command, and
             * back afterwards. No need for executing the command */
            float newAngle = m_rotateObject->getAngle();
            m_rotateObject->rotate( m_angleBeforeRotate );

            RotateCmd *rotateCmd = new RotateCmd( i18n( "Change Rotation" ), newAngle,
                                                  objects, m_view->kPresenterDoc() );
            m_view->kPresenterDoc()->addCommand( rotateCmd );

            m_rotateObject->rotate( newAngle );
            m_rotateObject = NULL;
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
    mouseMoveEvent( e );
}

void KPrCanvas::mouseMoveEvent( QMouseEvent *e )
{
    if ( editMode && toolEditMode == TEM_MOUSE && m_gl.mouseMoveEvent( e ) )
        return;

    QPoint contentsPoint( e->pos().x()+diffx(), e->pos().y()+diffy() );
    int oldMx = m_savedMousePos.x();
    int oldMy = m_savedMousePos.y();
    m_savedMousePos = contentsPoint;
    KoPoint docPoint = m_view->zoomHandler()->unzoomPoint( contentsPoint );
    if(m_currentTextObjectView)
    {
        if (m_currentTextObjectView->isLinkVariable(e->pos()) )
        {
            setCursor(Qt::PointingHandCursor);
            return;
        }
        setCursor( arrowCursor );

        KPTextObject *txtObj=m_currentTextObjectView->kpTextObject();
        Q_ASSERT(txtObj);
        if(txtObj->contains( docPoint )&&mousePressed)
        {
            KoPoint pos = docPoint - txtObj->innerRect().topLeft();
            m_currentTextObjectView->mouseMoveEvent( e, m_view->zoomHandler()->ptToLayoutUnitPix( pos ) ); // in LU pixels
        }
        return;
    }

    if (m_view->kPresenterDoc()->getVariableCollection()->variableSetting()->displayLink())
    {
        KPObject *tmp_kpobject = getObjectAt( docPoint );
        if(tmp_kpobject && tmp_kpobject->getType() == OT_TEXT)
        {
            KPTextObject *kptextobject = static_cast<KPTextObject*>( tmp_kpobject );
            QPoint iPoint = kptextobject->viewToInternal( e->pos(), this );
            KoLinkVariable* linkVar = dynamic_cast<KoLinkVariable *>( kptextobject->textObject()->variableAtPoint( iPoint ) );

            if ( linkVar )
            {
                setCursor(Qt::PointingHandCursor);
                return;
            }
        }
    }

    m_changeSnap = e->state() & ShiftButton;

    if ( editMode ) {
        m_view->setRulerMousePos( e->x(), e->y() );

        if ( toolEditMode == TEM_ROTATE )
        {
            setCursor( KPresenterUtils::rotateCursor() );
            if ( !mousePressed )
                return;
        }
        KPObject *kpobject;
        if ( ( !mousePressed || ( !drawRubber && modType == MT_NONE ) ) &&
             toolEditMode == TEM_MOUSE  )
        {
            bool cursorAlreadySet = false;
            if ( (int)objectList().count() > 0 )
            {
                kpobject = m_activePage->getCursor( docPoint );
                if( kpobject)
                {
                    setCursor( kpobject->getCursor( docPoint, modType, m_view->kPresenterDoc() ) );

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
                    if ( drawRubber ) 
                    {
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
                    if ( !m_isMoving )
                    {
                        m_moveStartPoint = objectRect( false ).topLeft();
                        m_isMoving = true;
                        repaint( false );
                    }
                    moveObjectsByMouse( docPoint );
                } else if ( modType != MT_NONE && m_resizeObject ) {
                    if ( !m_isResizing )
                    {
                      m_isResizing = true;
                      repaint( false );
                    }
                    int mx = e->x()+diffx();
                    int my = e->y()+diffy();
                    bool const doApplyGrid = !( ( (e->state() & ShiftButton) && m_view->kPresenterDoc()->snapToGrid() ) || ( !(e->state() & ShiftButton) && !m_view->kPresenterDoc()->snapToGrid() ) );

                    if ( doApplyGrid )
                    {
                      mx = applyGridOnPosX( mx );
                      my = applyGridOnPosY( my );
                      oldMx = applyGridOnPosX( oldMx );
                      oldMy = applyGridOnPosY( oldMy );
                    }

                    resizeObject( modType, mx - oldMx, my - oldMy, doApplyGrid );
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
                if ( m_rotateObject )
                {
                    drawContour = TRUE;
                    // angle to mouse pos
                    double angle = KoPoint::getAngle( m_rotateCenter, docPoint );
                    // angle to start of mouse pos
                    double angle1 = KoPoint::getAngle( m_rotateCenter,
                                        m_view->zoomHandler()->unzoomPoint( QPoint( firstX, firstY ) ) );

                    angle -= angle1;
                    angle += m_angleBeforeRotate;
                    if ( angle < 0 )
                        angle += 360;
                    else if ( angle > 360 )
                        angle -= 360;

                    m_rotateObject->rotate( angle );
                    _repaint( m_rotateObject );
                }
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

                bool const doApplyGrid = !( ( (e->state() & ShiftButton) && m_view->kPresenterDoc()->snapToGrid() ) || ( !(e->state() & ShiftButton) && !m_view->kPresenterDoc()->snapToGrid() ) );
                QPoint tmp = e->pos();
                if ( doApplyGrid )
                  tmp = applyGrid( e->pos(), true);
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
                bool const doApplyGrid = !( ( (e->state() & ShiftButton) && m_view->kPresenterDoc()->snapToGrid() ) || ( !(e->state() & ShiftButton) && !m_view->kPresenterDoc()->snapToGrid() ) );
                QPoint tmp = e->pos();
                if ( doApplyGrid )
                  tmp = applyGrid( e->pos(), true);
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
                bool const doApplyGrid = !( ( (e->state() & ShiftButton) && m_view->kPresenterDoc()->snapToGrid() ) || ( !(e->state() & ShiftButton) && !m_view->kPresenterDoc()->snapToGrid() ) );
                QPoint tmp = e->pos();
                if ( doApplyGrid )
                  tmp = applyGrid( e->pos(), true);
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
                bool const doApplyGrid = !( ( (e->state() & ShiftButton) && m_view->kPresenterDoc()->snapToGrid() ) || ( !(e->state() & ShiftButton) && !m_view->kPresenterDoc()->snapToGrid() ) );
                QPoint tmp = e->pos();
                if ( doApplyGrid )
                  tmp = applyGrid( e->pos(), true);
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
                bool const doApplyGrid = !( ( (e->state() & ShiftButton) && m_view->kPresenterDoc()->snapToGrid() ) || ( !(e->state() & ShiftButton) && !m_view->kPresenterDoc()->snapToGrid() ) );
                QPoint tmp = e->pos();
                if ( doApplyGrid )
                  tmp = applyGrid( e->pos(), true);

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
            } break;
            case INS_POLYLINE: case INS_CLOSED_POLYLINE: {
                QPainter p( this );
                p.setPen( QPen( black, 1, SolidLine ) );
                p.setBrush( NoBrush );
                p.setRasterOp( NotROP );
                p.drawLine( m_dragStartPoint, m_dragEndPoint ); //
                bool const doApplyGrid = !( ( (e->state() & ShiftButton) && m_view->kPresenterDoc()->snapToGrid() ) || ( !(e->state() & ShiftButton) && !m_view->kPresenterDoc()->snapToGrid() ) );
                QPoint tmp = e->pos();
                if ( doApplyGrid )
                  tmp = applyGrid( e->pos(), true);

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
            } break;
            case INS_CUBICBEZIERCURVE: case INS_QUADRICBEZIERCURVE:
            case INS_CLOSED_CUBICBEZIERCURVE: case INS_CLOSED_QUADRICBEZIERCURVE:{
                bool const doApplyGrid = !( ( (e->state() & ShiftButton) && m_view->kPresenterDoc()->snapToGrid() ) || ( !(e->state() & ShiftButton) && !m_view->kPresenterDoc()->snapToGrid() ) );
                QPoint tmp = e->pos();
                if ( doApplyGrid )
                    tmp = applyGrid( e->pos(), true);

                drawCubicBezierCurve( tmp.x(),
                                      tmp.y());

                mouseSelectedObject = true;
            } break;
            case INS_POLYGON: {
                drawPolygon( m_view->zoomHandler()->unzoomPoint( m_dragStartPoint ),
                             m_view->zoomHandler()->unzoomPoint( m_dragEndPoint ) ); // erase old polygon
                    bool const doApplyGrid = !( ( (e->state() & ShiftButton) && m_view->kPresenterDoc()->snapToGrid() ) || ( !(e->state() & ShiftButton) && !m_view->kPresenterDoc()->snapToGrid() ) );
                    QPoint tmp = e->pos();
                    if ( doApplyGrid )
                    tmp = applyGrid( e->pos(), true);

                m_dragEndPoint = QPoint( tmp.x(),
                                         tmp.y() );
                m_dragEndPoint=limitOfPoint(m_dragEndPoint);

                drawPolygon( m_view->zoomHandler()->unzoomPoint( m_dragStartPoint ),
                             m_view->zoomHandler()->unzoomPoint( m_dragEndPoint ) ); // draw new polygon

                mouseSelectedObject = true;
            } break;
            default: break;
            }
        }
    } else if ( !editMode && m_drawMode && m_drawLineInDrawMode ) {
        QPainter p;
        p.begin( this );
        p.setPen( m_view->kPresenterDoc()->presPen() );
        p.drawLine( oldMx, oldMy, e->x(), e->y() );
        p.end();
        m_drawModeLines[m_drawModeLines.count() - 1].putPoints( m_drawModeLineIndex++, 1, e->x(), e->y() );
    }

    if ( !editMode && !m_drawMode && !m_presMenu->isVisible() && fillBlack )
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
    if ( !m_activePage->getPageRect().contains(docPoint))
        return;


    if ( ( toolEditMode == INS_POLYLINE || toolEditMode == INS_CLOSED_POLYLINE ) && !m_pointArray.isNull() && m_drawPolyline ) {
        bool const doApplyGrid = !( ( (e->state() & ShiftButton) && m_view->kPresenterDoc()->snapToGrid() ) || ( !(e->state() & ShiftButton) && !m_view->kPresenterDoc()->snapToGrid() ) );
        m_dragStartPoint = doApplyGrid ? applyGrid( e->pos(), true) : e->pos();
        m_pointArray.putPoints( m_indexPointArray, 1, m_view->zoomHandler()->unzoomItX(m_dragStartPoint.x()),
                                m_view->zoomHandler()->unzoomItY(m_dragStartPoint.y() ));
        ++m_indexPointArray;
        endDrawPolyline();

        mouseMoveEvent( e );
        return;
    }


    if ( toolEditMode != TEM_MOUSE || !editMode ) return;

    KPObject *kpobject = getObjectAt( docPoint );
    if(kpobject)
    {
        if ( kpobject->getType() == OT_TEXT )
        {
            deSelectAllObj();
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
            deSelectAllObj();
            KPPartObject * obj=dynamic_cast<KPPartObject *>(kpobject);
            if(obj)
            {
                obj->activate( m_view );
                editNum = obj;
            }
        }
        else
             getView()->extraProperties();
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
    if ( !editMode && !m_drawMode ) {
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
    if ( editMode && m_gl.keyPressEvent( e ) )
        return;
    if ( !editMode ) {
        switch ( e->key() ) {
        case Qt::Key_Space: case Key_Right: case Key_Down:
            setSwitchingMode( false );
            m_view->screenNext();
            break;
        case Qt::Key_Next:
            setSwitchingMode( false );
            m_view->screenNext( true );
            break;
        case Qt::Key_Backspace: case Key_Left: case Key_Up:
            setSwitchingMode( false );
            finishObjectEffects();
            finishPageEffect( true );
            m_view->screenPrev();
            break;
        case Qt::Key_Prior:
            setSwitchingMode( false );
            finishObjectEffects();
            finishPageEffect( true );
            m_view->screenPrev( true );
            break;
        case Qt::Key_Escape: case Key_Q: case Key_X:
            setSwitchingMode( false );
            finishObjectEffects();
            finishPageEffect( true );
            m_view->screenStop();
            break;
        case Qt::Key_G:
            // setSwitchingMode( false ) not needed as it is allready done in slotGotoPage;
            if ( !spManualSwitch() )
                m_view->stopAutoPresTimer();
            slotGotoPage();
            break;
        case Qt::Key_Home:  // go to first page
            setSwitchingMode( false );
            presGotoFirstPage();
            if ( !spManualSwitch() ) {
                m_view->setAutoPresTimer( 1 );
                m_setPageTimer = true;
            }
            break;
        case Qt::Key_End:  // go to last page
            setSwitchingMode( false );
            if ( m_presentationSlidesIterator != m_presentationSlides.end() ) {
                gotoPage( *(--m_presentationSlides.end()) );
                if ( !spManualSwitch() ) {
                    m_view->setAutoPresTimer( 1 );
                    m_setPageTimer = true;
                }
            }
            break;
        default: break;
        }
    } else if ( editNum ) {
        if ( e->key() == Qt::Key_Escape ) {
            exitEditMode();
        }
        else if ( m_currentTextObjectView )
        {
            if ( !m_currentTextObjectView->kpTextObject()->isProtectContent() || (e->text().length() == 0))
                m_currentTextObjectView->keyPressEvent( e );
            else
                KMessageBox::information(this, i18n("Read-only content cannot be changed. No modifications will be accepted."));
        }
    }
    else
    {
        m_changeSnap = e->state() & ShiftButton;

        switch ( e->key() )
        {
            case Qt::Key_Next:
                m_view->nextPage();
                break;
            case Qt::Key_Prior:
                m_view->prevPage();
                break;
            case Qt::Key_Home:  // go to first page
                m_view->screenFirst();
                break;
            case Qt::Key_End:  // go to first page
                m_view->screenLast();
                break;
            default:
                break;
        }

        if ( mouseSelectedObject )
        {
            int offsetx = 1;
            int offsety = 1;

            if ( e->state() & ControlButton )
            {
                offsetx = QMAX(1,m_view->zoomHandler()->zoomItX(10));
                offsety = QMAX(1,m_view->zoomHandler()->zoomItY(10));
            }

            if ( !m_keyPressEvent )
            {
                m_moveStartPosKey = objectRect( false ).topLeft();
            }
            switch ( e->key() )
            {
                case Qt::Key_Up:
                    m_keyPressEvent = true;
                    moveObjectsByKey( 0, -offsety );
                    break;
                case Qt::Key_Down:
                    m_keyPressEvent = true;
                    moveObjectsByKey( 0, offsety );
                    break;
                case Qt::Key_Right:
                    m_keyPressEvent = true;
                    moveObjectsByKey( offsetx, 0 );
                    break;
                case Qt::Key_Left:
                    m_keyPressEvent = true;
                    moveObjectsByKey( -offsetx, 0 );
                    break;
                case Qt::Key_Delete: 
                case Key_Backspace:
                    m_view->editDelete();
                    break;
                case Qt::Key_Escape:
                {
                  if ( mousePressed && toolEditMode == TEM_MOUSE )
                  {
                    switch (modType)
                    {
                      case MT_RESIZE_UP:
                      case MT_RESIZE_DN:
                      case MT_RESIZE_LF:
                      case MT_RESIZE_RT:
                      case MT_RESIZE_LU:
                      case MT_RESIZE_LD:
                      case MT_RESIZE_RU:
                      case MT_RESIZE_RD:
                      {
                        QRect oldBoundingRect = m_view->zoomHandler()->zoomRect( m_resizeObject->getRepaintRect() );
                        m_resizeObject->setOrig( m_rectBeforeResize.topLeft() );
                        m_resizeObject->setSize( m_rectBeforeResize.size() );
                        drawContour = false;
                        m_view->kPresenterDoc()->repaint( oldBoundingRect );
                        m_view->kPresenterDoc()->repaint( m_resizeObject );
                        m_ratio = 0.0;
                        m_keepRatio = false;
                        m_resizeObject = 0;
                        m_isResizing = false;
                        mousePressed = false;
                        modType = MT_NONE;
                        if ( !m_view->kPresenterDoc()->showGrid() && m_view->kPresenterDoc()->snapToGrid() )
                            repaint( false );
                        return;
                      }
                      case MT_MOVE:
                      {
                        if ( m_isMoving )
                        {
                          drawContour = false;
                          KoPoint move( m_moveStartPoint - objectRect( false ).topLeft() );
                          m_activePage->moveObject( m_view, move, false );
                          mousePressed = false;
                          modType = MT_NONE;
                          m_isMoving = false;
                          if ( !m_view->kPresenterDoc()->showGrid() && m_view->kPresenterDoc()->snapToGrid() )
                            repaint( false );
                          return;
                        }
                        break;
                      }
                      default:
                        break;
                    }
                  }
                  setToolEditMode( TEM_MOUSE );
                  break;
                }
                default: break;
            }
        }
        else
        {
            switch ( e->key() ) {
                case Qt::Key_Down:
                    m_view->getVScrollBar()->addLine();
                    break;
                case Qt::Key_Up:
                    m_view->getVScrollBar()->subtractLine();
                    break;
                case Qt::Key_Right:
                    m_view->getHScrollBar()->addLine();
                    break;
                case Qt::Key_Left:
                    m_view->getHScrollBar()->subtractLine();
                    break;
                case Qt::Key_Tab:
                    selectNext();
                    break;
                case Qt::Key_Backtab:
                    selectPrev();
                    break;
                case Qt::Key_Escape:
                    if ( toolEditMode == TEM_ZOOM )
                        setToolEditMode( TEM_MOUSE );
                    break;
                default: break;
            }
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
            if(e->key()==Qt::Key_Up || e->key()==Key_Down || e->key()==Key_Right || e->key()==Key_Left)
            {
                if ( !e->isAutoRepeat() )
                {
                    KoPoint move( objectRect( false ).topLeft() - m_moveStartPosKey);
                    KCommand *cmd=m_activePage->moveObject(m_view, move.x(), move.y());
                    if(cmd)
                        m_view->kPresenterDoc()->addCommand( cmd );
                    m_keyPressEvent = false;
                }
                emit objectSelectedChanged();
            }
        }
    }
}

void KPrCanvas::imStartEvent( QIMEvent * e )
{
    if ( editNum && m_currentTextObjectView )
    {
        if ( !m_currentTextObjectView->kpTextObject()->isProtectContent() )
            m_currentTextObjectView->imStartEvent( e );
        else
            KMessageBox::information(this, i18n("Read-only content cannot be changed. No modifications will be accepted."));
    }
}

void KPrCanvas::imComposeEvent( QIMEvent * e )
{
    if ( editNum && m_currentTextObjectView )
    {
        if ( !m_currentTextObjectView->kpTextObject()->isProtectContent() )
            m_currentTextObjectView->imComposeEvent( e );
    }
}

void KPrCanvas::imEndEvent( QIMEvent * e )
{
    if ( editNum && m_currentTextObjectView )
    {
        if ( !m_currentTextObjectView->kpTextObject()->isProtectContent() )
            m_currentTextObjectView->imEndEvent( e );
    }
}

void KPrCanvas::resizeEvent( QResizeEvent *e )
{
    if ( editMode )
    {
        QWidget::resizeEvent( e );
    }
    else
        QWidget::resizeEvent( new QResizeEvent( KGlobalSettings::desktopGeometry(this).size(),
                                                e->oldSize() ) );
    buffer.resize( size() );
}

KPObject * KPrCanvas::getObjectAt( const KoPoint &pos, bool withoutProtected )
{
    KPObject *object = m_activePage->getObjectAt( pos, withoutProtected );
    if ( objectIsAHeaderFooterHidden( object ) )
        object = 0;

    return object;
}

void KPrCanvas::selectObj( KPObject *kpobject )
{
    kpobject->setSelected( true );
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

void KPrCanvas::selectAllObj()
{
    QPtrListIterator<KPObject> it( m_activePage->objectList() );
    for ( ; it.current() ; ++it )
    {
        if ( !objectIsAHeaderFooterHidden(it.current()) )
            it.current()->setSelected( true );
    }

    mouseSelectedObject = true;
    _repaint( false );
    emit objectSelectedChanged();
}


void KPrCanvas::deSelectAllObj()
{
    if( m_activePage->numSelected() == 0 )
        return;

    lowerObject();

    m_activePage->deSelectAllObj();

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
    m_presMenu = new KPopupMenu();
    Q_CHECK_PTR( m_presMenu );
    m_presMenu->setCheckable( true );
    m_presMenu->insertTitle( i18n( "Slide Show" ) );
    m_presMenu->insertItem( i18n( "&Continue" ), this, SLOT( setSwitchingMode() ) );
    PM_DM = m_presMenu->insertItem( i18n( "&Drawing Mode" ), this, SLOT( setDrawingMode() ) );
    m_presMenu->insertSeparator();
    m_presMenu->insertItem( SmallIcon("goto"), i18n( "&Goto Slide..." ), this, SLOT( slotGotoPage() ) );
    m_presMenu->insertSeparator();
    m_presMenu->insertItem( i18n( "&End" ), this, SLOT( slotExitPres() ) );
    m_presMenu->setItemChecked( PM_DM, false );
    m_presMenu->setMouseTracking( true );
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
    m_activePage->chPic( m_view );
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
                res = pix.save( &file, format, quality );
                file.close();
            }
            if( !bLocalFile ){
                if( res ){
                    res = KIO::NetAccess::upload( tmpFile->name(), fileURL, this );
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

void KPrCanvas::savePicture()
{
    m_activePage->savePicture( m_view );
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
    return m_activePage->haveASelectedPartObj();
}

bool KPrCanvas::haveASelectedGroupObj() const
{
    return m_activePage->haveASelectedGroupObj();
}

bool KPrCanvas::haveASelectedPixmapObj() const
{
    return m_activePage->haveASelectedPixmapObj();
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
    return lst;
}

void KPrCanvas::startScreenPresentation( double zoomX, double zoomY, int curPgNum /* 1-based */)
{
    //kdDebug(33001) << "KPrCanvas::startScreenPresentation curPgNum=" << curPgNum << endl;

    //setup presentation menu
    m_presMenu->setItemChecked( PM_DM, false );

    setCursor( waitCursor );

    exitEditMode();

    KPresenterDoc * doc = m_view->kPresenterDoc();

    // Text can't zoom with a different x and y factor, yet.
    // So we have to choose the smallest zoom (but still paint background everywhere)
    double zoom = kMin( zoomX, zoomY );

    kdDebug() << "zoomX=" << zoomX << " zoomY=" << zoomY << " zoom=" << zoom << endl;

    m_zoomBeforePresentation = doc->zoomHandler()->zoom();
    kdDebug() << "old zoomed resolutions =" << doc->zoomHandler()->zoomedResolutionX() << "," << doc->zoomHandler()->zoomedResolutionY() << endl;
    // Seems to fail (Qt uses the wrong font sizes...)
    //doc->zoomHandler()->setZoomedResolution( zoomX * doc->zoomHandler()->zoomedResolutionX(),
    //                                         zoomY * doc->zoomHandler()->zoomedResolutionY() );
    // Apply the new zooming to the existing one
    doc->zoomHandler()->setZoomAndResolution( qRound( zoom * m_zoomBeforePresentation ),
                                              KoGlobal::dpiX(), KoGlobal::dpiY() );

    doc->newZoomAndResolution( false, false );

    // add all selected slides
    m_presentationSlides.clear();
    QValueList<int> selected = doc->displaySelectedSlides();
    for ( QValueList<int>::Iterator it = selected.begin() ; it != selected.end(); ++ it )
    {
        // ARGLLLRGLRLGRLG selectedSlides gets us 0-based numbers,
        // and here we want 1-based numbers !
        int slideno = *it + 1;
        m_presentationSlides.append( slideno );
    }

    // no slide selected ? end the slide show immediately...
    if( !m_presentationSlides.count() )
    {
        //kdDebug(33001) << "No slide: end the slide show" << endl;
        stopScreenPresentation();
        return;
    }

    // find first selected slide after curPgNum
    unsigned slide = 0;
    for( unsigned i = 0; i<m_presentationSlides.count(); i++ )
        if( m_presentationSlides[i] >= curPgNum )
        {
            slide = m_presentationSlides[i];
            break;
        }

    setCursor( blankCursor );

    m_step.m_pageNumber = (unsigned int) -1; // force gotoPage to do something
    // No need to paint yet, we'll get an update soon (probably due to reparent+showFullScreen)
    setUpdatesEnabled( false );
    gotoPage( slide );
    setUpdatesEnabled( true );
    //kdDebug(33001) << "Page::startScreenPresentation - done" << endl;
}

void KPrCanvas::stopScreenPresentation()
{
    //kdDebug(33001) << "KPrCanvas::stopScreenPresentation m_showOnlyPage=" << m_showOnlyPage << endl;
    setCursor( waitCursor );

    KPresenterDoc * doc = m_view->kPresenterDoc();
    doc->zoomHandler()->setZoomAndResolution( m_zoomBeforePresentation,
                                              KoGlobal::dpiX(), KoGlobal::dpiY() );
    doc->newZoomAndResolution(false,false);
    goingBack = false;
    m_step.m_pageNumber = 0;
    editMode = true;
    m_drawMode = false;
    repaint( false );
    setToolEditMode( toolEditMode );
    setWFlags( WResizeNoErase );
}

bool KPrCanvas::pNext( bool gotoNextPage )
{
    goingBack = false;

    bool objectEffectFinished = finishObjectEffects();

    KPresenterDoc * doc = m_view->kPresenterDoc();

    if ( !gotoNextPage )
    {
        if ( finishPageEffect() )
          return false;

        // clear drawed lines
        m_drawModeLines.clear();

        //kdDebug(33001) << "\n-------\nKPrCanvas::pNext m_step =" << m_step.m_step << " m_subStep =" << m_step.m_subStep << endl;

        // First try to go one sub-step further, if any object requires it
        // ### should we also objects on the sticky page be checked for sub steps?
        QPtrListIterator<KPObject> oit( doc->pageList().at( m_step.m_pageNumber )->objectList() );
        for ( int i = 0 ; oit.current(); ++oit, ++i )
        {
            KPObject *kpobject = oit.current();
            if ( kpobject->getAppearStep() == static_cast<int>( m_step.m_step )
                 && kpobject->getType() == OT_TEXT && kpobject->getEffect2() != EF2_NONE )
            {
                if ( static_cast<int>( m_step.m_subStep + 1 ) < kpobject->getSubPresSteps() )
                {
                    m_step.m_subStep++;
                    //kdDebug(33001) << "Page::pNext addSubPres m_subStep is now " << m_subStep << endl;
                    doObjEffects();
                    return false;
                }
            }
        }

        // Then try to see if there is still one step to do in the current page
        if ( m_step.m_step < *( --m_pageEffectSteps.end() ) )
        {
            QValueList<int>::ConstIterator it = m_pageEffectSteps.find( m_step.m_step );
            m_step.m_step = *( ++it );
            m_step.m_subStep = 0;
            //kdDebug(33001) << "Page::pNext setting currentEffectStep to " << m_step.m_step << endl;

            // if first step on page, draw background
            if ( m_step.m_step == 0 )
            {
                QPainter p;
                p.begin( this );
                drawBackground( &p,
                                QRect( 0, 0, kapp->desktop()->width(), kapp->desktop()->height() ),
                                doc->pageList().at( m_step.m_pageNumber ) );
                p.end();
            }

            doObjEffects();
            return false;
        }

        /*
         * don't go to next slide if we have finished an object effect
         * so that we can see the hole slide before going to the next
         */
        if ( objectEffectFinished )
            return false;
    }

    // No more steps in this page, try to go to the next page
    QValueList<int>::ConstIterator test(  m_presentationSlidesIterator );
    if ( ++test != m_presentationSlides.end() )
    {
        if ( !spManualSwitch() && m_setPageTimer )
        {
            //TODO add global presentation speed
            m_view->setAutoPresTimer( doc->pageList().at( (*m_presentationSlidesIterator) - 1 )->getPageTimer() / 1 );
            m_setPageTimer = false;
            return false;
        }
        m_setPageTimer = true;

        QRect desk = KGlobalSettings::desktopGeometry(this);
        QPixmap _pix1( desk.width(), desk.height() );
        drawCurrentPageInPix( _pix1 );

        m_view->setPageDuration( m_step.m_pageNumber );

        m_step.m_pageNumber = *( ++m_presentationSlidesIterator ) - 1;
        m_step.m_subStep = 0;

        m_pageEffectSteps = doc->getPageEffectSteps( m_step.m_pageNumber );
        m_step.m_step = *m_pageEffectSteps.begin();

        QPixmap _pix2( desk.width(), desk.height() );
        drawCurrentPageInPix( _pix2 );

        QValueList<int>::ConstIterator it( m_presentationSlidesIterator );
        --it;

        KPrPage * page = doc->pageList().at( ( *it ) - 1 );
        PageEffect _pageEffect = page->getPageEffect();

        bool _soundEffect = page->getPageSoundEffect();
        QString _soundFileName = page->getPageSoundFileName();

        if ( _soundEffect && !_soundFileName.isEmpty() ) {
            stopSound();
            playSound( _soundFileName );
        }

        if ( m_pageEffect )
            finishPageEffect();

        m_pageEffect = new KPPageEffects( this, _pix2, _pageEffect, page->getPageEffectSpeed() );
        if ( m_pageEffect->doEffect() )
        {
            delete m_pageEffect;
            m_pageEffect = 0;

            doObjEffects( true );
        }
        else
        {
            connect( &m_pageEffectTimer, SIGNAL( timeout() ), SLOT( slotDoPageEffect() ) );
            m_pageEffectTimer.start( 50, true );
        }

        return true;
    }

    //kdDebug(33001) << "Page::pNext last slide -> End of presentation" << endl;

    // When we are in manual mode or in automatic mode with no infinite loop
    // we display the 'End of presentation' slide.
    if ( ( spManualSwitch() || !spInfiniteLoop() ) && !showingLastSlide )
    {
        m_view->setPageDuration( m_step.m_pageNumber );

        QRect desk = KGlobalSettings::desktopGeometry(this);
        QPixmap lastSlide( desk.width(), desk.height() );
        QFont font( m_view->kPresenterDoc()->defaultFont().family() );
        QPainter p( &lastSlide );

        p.setFont( font );
        p.setPen( white );
        p.fillRect( p.viewport(), black );
        p.drawText( 50, 50, i18n( "End of presentation. Click to exit." ) );
        bitBlt( this, 0, 0, &lastSlide, 0, 0, lastSlide.width(), lastSlide.height() );
        showingLastSlide = true;
        emit stopAutomaticPresentation(); // no automatic mode for last slide
    }
    else if ( showingLastSlide ) // after last slide stop presentation
    {
        showingLastSlide = false;
        m_view->screenStop();
    }
    else
    {
        m_view->setPageDuration( m_step.m_pageNumber );
        emit restartPresentation(); // tells automatic mode to restart
        m_view->setAutoPresTimer( 1 );
    }

    return false;
}

bool KPrCanvas::pPrev( bool gotoPreviousPage )
{
    goingBack = true;
    m_step.m_subStep = 0;

    // clear drawed lines
    m_drawModeLines.clear();

    if ( !gotoPreviousPage && m_step.m_step > *m_pageEffectSteps.begin() ) {
        QValueList<int>::ConstIterator it = m_pageEffectSteps.find( m_step.m_step );
        m_step.m_step = *( --it );
        //hopefully there are never more than 1000 sub steps :-)
        m_step.m_subStep = 1000;
        repaint( false );
        return false;
    } else {
        // when we go back on the first slide, thats like starting the presentation again
        if ( m_presentationSlidesIterator == m_presentationSlides.begin() ) {
            m_pageEffectSteps = m_view->kPresenterDoc()->getPageEffectSteps( m_step.m_pageNumber );
            m_step.m_step = *m_pageEffectSteps.begin();
            goingBack = false;
            doObjEffects();
            return false;
        }
        m_view->setPageDuration( m_step.m_pageNumber );

        m_step.m_pageNumber = *( --m_presentationSlidesIterator ) - 1;

        KPresenterDoc * doc = m_view->kPresenterDoc();

        m_pageEffectSteps = doc->getPageEffectSteps( m_step.m_pageNumber );

        if ( gotoPreviousPage )
        {
            m_step.m_step = *( m_pageEffectSteps.begin() );
            goingBack = false;
            doObjEffects();
        }
        else
        {
            m_step.m_step = *( --m_pageEffectSteps.end() );
            repaint( false );
        }

        return true;
    }

    return false;
}

bool KPrCanvas::canAssignEffect( QPtrList<KPObject> &objs ) const
{
    QPtrListIterator<KPObject> oIt( m_activePage->objectList() );
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
    return m_activePage->isOneObjectSelected();
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
        // ### TODO: this should probably be setZoomedResolution, please test
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

    KPrPage * page = m_view->kPresenterDoc()->pageList().at(pgnum);
    QRect rect = page->getZoomPageRect( );
    _pix.resize( rect.size() );
    _pix.fill( Qt::white );

    QPainter p;
    p.begin( &_pix );

    bool _editMode = editMode;
    editMode = false;

    drawBackground( &p, _pix.rect(), page, true );

    //objects in current page
    QPtrList<KPObject> _list = page->objectList();

    // check if object is selected, if so put it on the right place for the output
    if( _list.count() > 1 && (int)_list.count() > selectedObjectPosition && selectedObjectPosition >= 0) {
        _list.setAutoDelete( false );
        KPObject *kpobject = _list.last();
        if ( kpobject->isSelected() ) {
            _list.take( _list.count() - 1 );
            _list.insert( selectedObjectPosition, kpobject );
        }
    }

    drawAllObjectsInPage( &p, _list, pgnum );

    //draw sticky object
    if ( page->masterPage() && page->displayObjectFromMasterPage() )
        drawAllObjectsInPage( &p, page->masterPage()->objectList(), pgnum );

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

    // avoid garbage on "weird" DPIs
    _pix.fill(Qt::black);

    QPainter p;
    p.begin( &_pix );

    drawBackground( &p, _pix.rect(), m_view->kPresenterDoc()->pageList().at( m_step.m_pageNumber ) );
    PresStep step( m_step.m_pageNumber, m_step.m_step, m_step.m_subStep, true, true );
    drawPresPage( &p, _pix.rect(), step );

    p.end();
}

void KPrCanvas::printPage( QPainter* painter, PresStep step, KPrinter *printer, int rows, int cols )
{
    //kdDebug(33001) << "KPrCanvas::printPage" << endl;
    KPresenterDoc *doc = m_view->kPresenterDoc();
    KPrPage* page = doc->pageList().at( step.m_pageNumber );
    QRect const rect = page->getZoomPageRect();
    bool const drawBorder = printer->option("kde-kpresenter-printslideborders").toInt() && rows>1 && cols>1;

    int height = rect.height();
    int width = rect.width();
    int begin_left = 0;
    int begin_top = 0;

    rows = cols = QMAX( rows, cols ); // all slides have the same size

    if ( rows > 1 )
    {
        height = ( ( height - 80 ) / rows ) - 20;
        begin_top = 40;
    }
    if ( cols > 1 )
    {
        width = (width -5) / cols;
        begin_left = 5;
    }
    int top = begin_top;
    int left = begin_left;
    for (int r = 0; r < rows; r++ )
    {
        for (int c = 0; c < cols; c++ )
        {
            page = doc->pageList().at( step.m_pageNumber );
            if ( !page )
                return;
            painter->setViewport( QRect(left, top, width, height) );
            drawBackground( painter, rect, page, true );
            drawPresPage( painter, rect, step );
            if ( drawBorder )
                painter->drawRect( rect );
            step.m_pageNumber++;
            left += width;
        }
        top += height + 20; // some y-space between the slides
        left = begin_left;
    }

}

void KPrCanvas::doObjEffects( bool isAllreadyPainted )
{
    if ( m_effectHandler )
    {
        m_effectTimer.stop();
        QObject::disconnect( &m_effectTimer, SIGNAL( timeout() ), this, SLOT( slotDoEffect() ) );

        m_effectHandler->finish();
        delete m_effectHandler;
        m_effectHandler = 0;
    }

    QPixmap screen_orig( kapp->desktop()->width(), kapp->desktop()->height() );

    KPrPage *page = m_view->kPresenterDoc()->pageList().at( m_step.m_pageNumber );
    // YABADABADOOOOOOO.... That's a hack :-)
    if ( m_step.m_subStep == 0 && !isAllreadyPainted && isUpdatesEnabled() )
    {
        //kdDebug(33001) << "Page::doObjEffects - in the strange hack" << endl;
        QPainter p;
        p.begin( &screen_orig );
        QRect desktopRect = QRect( 0, 0, kapp->desktop()->width(), kapp->desktop()->height() );
        drawBackground( &p, desktopRect, page );
        PresStep step( m_step.m_pageNumber, m_step.m_step, m_step.m_subStep, true, true );
        drawPresPage( &p, desktopRect ,step );
        p.end();
        bitBlt( this, 0, 0, &screen_orig );
    }
    else
    {
        bitBlt( &screen_orig, 0, 0, this );
    }

    QPtrList<KPObject> allObjects( page->objectList() );

    if ( page->displayObjectFromMasterPage() )
    {
        QPtrListIterator<KPObject> it( page->masterPage()->objectList() );
        for ( ; it.current(); ++it ) {
            if ( objectIsAHeaderFooterHidden( it.current() ) )
                continue;
            else
                allObjects.append( it.current() );
        }
    }
    //TODO add global presentation speed
    m_effectHandler = new EffectHandler( m_step, goingBack, this, &screen_orig, allObjects, m_view, 1 );
    if ( m_effectHandler->doEffect() )
    {
        delete m_effectHandler;
        m_effectHandler = 0;
    }
    else
    {
        connect( &m_effectTimer, SIGNAL( timeout() ), SLOT( slotDoEffect() ) );
        m_effectTimer.start( 50, true );
    }

}

void KPrCanvas::slotDoEffect()
{
    if ( m_effectHandler->doEffect() )
    {
        m_effectTimer.stop();
        QObject::disconnect( &m_effectTimer, SIGNAL( timeout() ), this, SLOT( slotDoEffect() ) );
        delete m_effectHandler;
        m_effectHandler = 0;
    }
    else
    {
        m_effectTimer.start( 50, true );
    }
}


void KPrCanvas::slotDoPageEffect()
{
    if ( m_pageEffect->doEffect() )
    {
        m_pageEffectTimer.stop();
        QObject::disconnect( &m_pageEffectTimer, SIGNAL( timeout() ), this, SLOT( slotDoPageEffect() ) );
        delete m_pageEffect;
        m_pageEffect = 0;

        doObjEffects( true );
    }
    else
    {
        m_pageEffectTimer.start( 50, true );
    }
}


bool KPrCanvas::finishObjectEffects()
{
    bool finished = false;
    if ( m_effectHandler )
    {
        m_effectTimer.stop();
        QObject::disconnect( &m_effectTimer, SIGNAL( timeout() ), this, SLOT( slotDoEffect() ) );
        m_effectHandler->finish();
        delete m_effectHandler;
        m_effectHandler = 0;
        finished = true;
    }
    return finished;
}


bool KPrCanvas::finishPageEffect( bool cancel )
{
    bool finished = false;
    if ( m_pageEffect )
    {
        m_pageEffectTimer.stop();
        QObject::disconnect( &m_pageEffectTimer, SIGNAL( timeout() ), this, SLOT( slotDoPageEffect() ) );
        if ( !cancel )
            m_pageEffect->finish();
        delete m_pageEffect;
        m_pageEffect = 0;

        if ( !cancel )
        {
            doObjEffects( true );
        }
        else if ( !spManualSwitch() )
        {
            m_view->setAutoPresTimer( 1 );
            m_setPageTimer = true;
        }

        finished = true;
    }
    return finished;
}


void KPrCanvas::print( QPainter *painter, KPrinter *printer, float /*left_margin*/, float /*top_margin*/ )
{
    PresStep step( 0, 1000, 1000 );
    //deSelectAllObj(); // already done in KPresenterView::setupPrinter
    printer->setFullPage( true );
    int i = 0;

    repaint( false );
    kapp->processEvents();

    editMode = false;
    fillBlack = false;

    //int _xOffset = diffx();
    //int _yOffset = diffy();

    //m_view->setDiffX( -static_cast<int>( MM_TO_POINT( left_margin ) ) );
    //m_view->setDiffY( -static_cast<int>( MM_TO_POINT( top_margin ) ) );

    QProgressDialog progress( i18n( "Printing..." ), i18n( "Cancel" ),
                              printer->pageList().count() + 2, this );

    int j = 0;
    progress.setProgress( 0 );


    int rows = 1;
    int cols = 1;
    if ( !printer->previewOnly() )
    {
        rows = printer->option("kde-kpresenter-printrows").toInt();
        cols = printer->option("kde-kpresenter-printcolumns").toInt();
    }
    int const slides_per_page = rows * cols;

    /*if ( printer->fromPage() > 1 )
      m_view->setDiffY( ( printer->fromPage() - 1 ) * ( getPageRect( 1, 1.0, false ).height() ) -
      (int)MM_TO_POINT( top_margin ) );*/
    int current_slide = 0;
    QValueList<int> list=printer->pageList(); // 1-based
    QValueList<int>::iterator it;
    for( it=list.begin();it!=list.end();++it)
    {
        i=(*it);
        progress.setProgress( ++j );
        kapp->processEvents();

        if ( progress.wasCancelled() )
            break;

        step.m_pageNumber = i - 1;

        if ( step.m_pageNumber != current_slide )
          continue;

        if ( !list.isEmpty() && i > list.first() )
            printer->newPage();

        painter->resetXForm();
        painter->fillRect( m_view->kPresenterDoc()->pageList().at( m_step.m_pageNumber )->getZoomPageRect(), white );

        printPage( painter, step, printer, rows, cols );
        kapp->processEvents();

        painter->resetXForm();
        kapp->processEvents();

        current_slide += slides_per_page;

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

    progress.setProgress( printer->pageList().count() + 2 );

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
    m_view->showObjectRect( m_activePage->getSelectedObj() );
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
    m_view->showObjectRect( m_activePage->getSelectedObj() );
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
                if ( !KIO::NetAccess::download( url, filename, this ) )
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
                    kdDebug()<<" filename :"<<filename<<endl;
                    if ( f.open( IO_ReadOnly ) ) {
                        while ( !t.eof() ) {
                            tmp = t.readLine();
                            tmp += "\n";
                            text.append( tmp );
                        }
                        f.close();
                    }
                    kdDebug()<<" text :"<<endl;
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
        //kdDebug()<<" QTextDrag::decode( e, text ); :"<<text<<endl;
        m_activePage->insertTextObject( m_view->zoomHandler()->unzoomRect( QRect( e->pos().x(), e->pos().y(), 250, 250 )),
                                        text, m_view );
        e->accept();
    } else
        e->ignore();

}

void KPrCanvas::slotGotoPage()
{
    setSwitchingMode( false );
    //setCursor( blankCursor );
    int pg = m_step.m_pageNumber + 1;

    m_view->setPageDuration( m_step.m_pageNumber );

    pg = KPGotoPage::gotoPage( m_view->kPresenterDoc(), m_presentationSlides, pg, this );
    gotoPage( pg );

    if ( !spManualSwitch() ) {
        m_view->setAutoPresTimer( 1 );
        m_setPageTimer = true;
    }
}

void KPrCanvas::gotoPage( int pg )
{
    int page = pg - 1;
    if ( page != m_step.m_pageNumber || m_step.m_step != *m_pageEffectSteps.begin() || m_step.m_subStep != 0 ) {
        // clear drawed lines
        m_drawModeLines.clear();
        goingBack = false;

        m_step.m_pageNumber = page;
        kdDebug(33001) << "Page::gotoPage m_step.m_pageNumber =" << m_step.m_pageNumber << endl;
        m_presentationSlidesIterator = m_presentationSlides.find( m_step.m_pageNumber + 1 );
        editMode = false;
        m_drawMode = false;
        m_pageEffectSteps = m_view->kPresenterDoc()->getPageEffectSteps( m_step.m_pageNumber );
        m_step.m_step = *m_pageEffectSteps.begin();
        m_step.m_subStep = 0;
#if 0
        QRect desk = KGlobalSettings::desktopGeometry(this);
        resize( desk.width(), desk.height() );
#endif
        doObjEffects();
        setFocus();
        m_view->refreshPageButton();
    }
}

void KPrCanvas::presGotoFirstPage()
{
    gotoPage( *m_presentationSlides.begin() );
}

KPTextObject* KPrCanvas::kpTxtObj() const
{
    return ( ( editNum && editNum->getType() == OT_TEXT ) ?
             dynamic_cast<KPTextObject*>( editNum ) : 0 );
    // ### return m_currentTextObjectView->kpTextObject()
}

void KPrCanvas::copyOasisObjs()
{
    //todo copy object from selected object
    KoStoreDrag *kd = new KoStoreDrag( "application/vnd.oasis.opendocument.presentation", 0L );
    QDragObject* dragObject = kd;
    QByteArray arr;
    QBuffer buffer(arr);
    KoStore* store = KoStore::createStore( &buffer, KoStore::Write, "application/vnd.oasis.opendocument.presentation" );

    delete store;
    kd->setEncodedData( arr );
    QApplication::clipboard()->setData( dragObject, QClipboard::Clipboard );
}

void KPrCanvas::copyObjs()
{
    QDomDocument doc("DOC");
    QDomElement presenter=doc.createElement("DOC");
    presenter.setAttribute("editor", "KPresenter");
    presenter.setAttribute("mime", "application/x-kpresenter");

    doc.appendChild(presenter);

    QPtrList<KoDocumentChild> embeddedObjectsActivePage;

    KoStoreDrag *kd = new KoStoreDrag( "application/x-kpresenter", 0L );
    QDragObject* dragObject = kd;
    QByteArray arr;
    QBuffer buffer(arr);
    KoStore* store = KoStore::createStore( &buffer, KoStore::Write, "application/x-kpresenter" );

    m_activePage->getAllEmbeddedObjectSelected(embeddedObjectsActivePage );

    // Save internal embedded objects first, since it might change their URL
    int i = 0;
    QValueList<KoPictureKey> savePictures;
    QPtrListIterator<KoDocumentChild> chl( embeddedObjectsActivePage );
    for( ; chl.current(); ++chl ) {
        KoDocument* childDoc = chl.current()->document();
        if ( childDoc && !childDoc->isStoredExtern() )
            (void) childDoc->saveToStore( store, QString::number( i++ ) );
    }

    m_activePage->copyObjs(doc, presenter, savePictures);

    KPresenterDoc* kprdoc = m_view->kPresenterDoc();
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
    lowerObject();
    KCommand *cmd=m_activePage->deleteSelectedObjects();
    m_view->kPresenterDoc()->deSelectAllObj();
    if( cmd )
        m_view->kPresenterDoc()->addCommand( cmd );
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

void KPrCanvas::setDrawingMode()
{
    m_presMenu->setItemChecked( PM_DM, true );
    m_drawMode = true;

    setCursor( KPresenterUtils::penCursor() );
}

void KPrCanvas::setSwitchingMode( bool continueTimer )
{
    m_presMenu->setItemChecked( PM_DM, false );

    // the following have to be done even when nothing changed
    // we don't want to see the cursor nor the automatic pesentation stopped
    m_drawMode = false;
    m_drawLineInDrawMode = false;
    setCursor( blankCursor );

    if ( continueTimer && !spManualSwitch() )
        m_view->continueAutoPresTimer();
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
            m_view->kPresenterDoc()->updateSideBarItem( m_activePage );
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
    KPresenterDoc *doc = m_view->kPresenterDoc();
    KPPixmapObject *object = m_activePage->picViewOrigHelper();

    KoSize newSize( doc->zoomHandler()->pixelXToPt( object->originalSize().width() ),
                    doc->zoomHandler()->pixelYToPt( object->originalSize().height() ) );

    KoRect pageRect = m_activePage->getPageRect();
    double fakt = 1.0;

    if ( newSize.width() > pageRect.width() )
        fakt = pageRect.width() / newSize.width();
    if ( newSize.height() > pageRect.height() )
        fakt = QMIN( fakt, pageRect.height() / newSize.height() );

    KoSize diff( newSize.width() * fakt - object->getSize().width(),
                 newSize.height() * fakt - object->getSize().height() );

    // TODO after 1.4 change text to "Scale to Original Size"
    ResizeCmd *resizeCmd = new ResizeCmd( i18n( "Scale Picture to Be Shown 1:1 in Presentation Mode" ),
                                          KoPoint( 0, 0 ), diff, object, doc );
    resizeCmd->execute();
    doc->addCommand( resizeCmd );
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
    QRect desk = KGlobalSettings::desktopGeometry(this);
    double faktX = (double)presSize.width() / (double)desk.width();
    double faktY = (double)presSize.height() / (double)desk.height();
    double w = pgSize.width() * faktX;
    double h = pgSize.height() * faktY;

    ResizeCmd *resizeCmd = new ResizeCmd( i18n( "Scale Picture to Be Shown 1:1 in Presentation Mode" ),
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
    m_activePage->background()->drawBackground( &painter, FALSE );
    QPixmap bpix( obj->getSize().toQSize() ); // ## zoom it !
    bitBlt( &bpix, 0, 0, &pix, obj->getOrig().x(), obj->getOrig().y() -
            m_activePage->getZoomPageRect().height() * ( m_view->getCurrPgNum() - 1 ), bpix.width(), bpix.height() );
    QBrush b( white, bpix );
    QPalette pal( obj->textObjectView()->palette() );
    pal.setBrush( QColorGroup::Base, b );
    obj->textObjectView()->setPalette( pal );
#endif
}


KoPoint KPrCanvas::diffGrid( KoRect &rect, double diffx, double diffy )
{
    KPresenterDoc * doc( m_view->kPresenterDoc() );
    KoPoint move( 0, 0 );

    double tempx = ( int( rect.topLeft().x() / doc->getGridX() ) * doc->getGridX() ) - rect.topLeft().x();
    if ( diffx > 0 )
    {
        tempx += doc->getGridX();
        while ( diffx > tempx )
        {
            move.setX( tempx );
            tempx += doc->getGridX();
        }
    }
    else
    {
        while ( diffx < tempx )
        {
            move.setX( tempx );
            tempx -= doc->getGridX();
        }
    }
    double tempy = ( int( rect.topLeft().y() / doc->getGridY() ) * doc->getGridY() ) - rect.topLeft().y();
    if ( diffy > 0 )
    {
        tempy += doc->getGridY();
        while ( diffy > tempy )
        {
            move.setY( tempy );
            tempy += doc->getGridY();
        }
    }
    else
    {
        while ( diffy < tempy )
        {
            move.setY( tempy );
            tempy -= doc->getGridY();
        }
    }
    
    return move;
}


void KPrCanvas::moveObjectsByKey( int x, int y )
{
    KPresenterDoc *doc( m_view->kPresenterDoc() );

    KoRect rect( objectRect( false ) );
    KoPoint move( 0, 0 );
    double diffx = m_view->zoomHandler()->unzoomItX( x );
    double diffy = m_view->zoomHandler()->unzoomItY( y );

    bool snapToGrid = ( doc->snapToGrid() && !m_changeSnap || !doc->snapToGrid() && m_changeSnap );
    bool snapToGuideLines = ( doc->snapToGuideLines() && !m_changeSnap || !doc->snapToGuideLines() && m_changeSnap );

    if ( snapToGrid && !snapToGuideLines )
    {
        if ( diffx > 0 )
        {
            diffx = doc->getGridX() + 0.1;
        }
        else if ( diffx < 0 )
        {
            diffx = -doc->getGridX() - 0.1;
        }
        if ( diffy > 0 )
        {
            diffy = doc->getGridY() + 0.1;
        }
        else if ( diffy < 0 )
        {
            diffy = -doc->getGridY() - 0.1;
        }
        move = diffGrid( rect, diffx, diffy );
    }
    else if ( snapToGuideLines && !snapToGrid )
    {
        move.setX( diffx );
        move.setY( diffy );
        KoRect movedRect( rect );
        movedRect.moveBy( diffx, diffy );
        if ( m_moveSnapDiff != KoPoint( 0, 0 ) )
        {
            movedRect.moveBy( -m_moveSnapDiff.x(), -m_moveSnapDiff.y() );
            move -= m_moveSnapDiff;
        }

        KoPoint diff( m_gl.snapToGuideLines( movedRect, 2 ) );

        if ( diff != KoPoint( 0, 0 ) )
        {
            m_moveSnapDiff = diff;
            move += diff;
        }
        else
        {
            m_moveSnapDiff = KoPoint( 0, 0 );
        }
    }
    else if ( snapToGrid && snapToGuideLines )
    {
        if ( diffx > 0 )
        {
            diffx = doc->getGridX() + 0.1;
        }
        else if ( diffx < 0 )
        {
            diffx = -doc->getGridX() - 0.1;
        }
        if ( diffy > 0 )
        {
            diffy = doc->getGridY() + 0.1;
        }
        else if ( diffy < 0 )
        {
            diffy = -doc->getGridY() - 0.1;
        }
        move = diffGrid( rect, diffx, diffy );
        KoPoint diff( m_gl.diffGuide( rect, diffx > 0, diffy > 0 ) );
        if ( diffx > 0 )
        {
            if ( diff.x() > 0 && diff.x() < move.x() )
            {
                move.setX( diff.x() );
            }
        }
        else if ( diffx < 0 )
        {
            if ( diff.x() < 0 && diff.x() > move.x() )
            {
                move.setX( diff.x() );
            }
        }
        if ( diffy > 0 )
        {
            if ( diff.y() > 0 && diff.y() < move.y() )
            {
                move.setY( diff.y() );
            }
        }
        else if ( diffy < 0)
        {
            if ( diff.y() < 0 && diff.y() > move.y() )
            {
                move.setY( diff.y() );
            }
        }
    }
    else
    {
        move.setX( diffx );
        move.setY( diffy );
    }

    // don't move object from canvas
    KoRect pageRect( m_activePage->getPageRect() );
    if ( rect.left() + move.x() < pageRect.left() )
    {
        move.setX( pageRect.left() - rect.left() );
    }
    else if ( rect.right() + move.x() > pageRect.right() )
    {
        move.setX( pageRect.right() - rect.right() );
    }
    if ( rect.top() + move.y() < pageRect.top() )
    {
        move.setY( pageRect.top() - rect.top() );
    }
    else if ( rect.bottom() + move.y() > pageRect.bottom() )
    {
        move.setY( pageRect.bottom() - rect.bottom() );
    }

    if ( move != KoPoint( 0, 0 ) )
    {
        //kdDebug(33001) << "moveObjectsByMouse move = " << move << endl;
        m_activePage->moveObject( m_view, move, false );
        scrollCanvas( move );
    }
}


void KPrCanvas::moveObjectsByMouse( KoPoint &pos )
{
    KPresenterDoc *doc( m_view->kPresenterDoc() );

    KoRect rect( objectRect( false ) );
    KoPoint move( 0, 0 );
    double diffx = pos.x() - m_origMousePos.x();
    double diffy = pos.y() - m_origMousePos.y();
    KoPoint mouseDiff( diffx, diffy );

    bool snapToGrid = ( doc->snapToGrid() && !m_changeSnap || !doc->snapToGrid() && m_changeSnap );
    bool snapToGuideLines = ( doc->snapToGuideLines() && !m_changeSnap || !doc->snapToGuideLines() && m_changeSnap );

    if ( snapToGrid && !snapToGuideLines )
    {
        move = diffGrid( rect, diffx, diffy );
        mouseDiff = move;
    }
    else if ( snapToGuideLines && !snapToGrid )
    {
        move.setX( diffx );
        move.setY( diffy );
        KoRect movedRect( rect );
        movedRect.moveBy( diffx, diffy );
        if ( m_moveSnapDiff != KoPoint( 0, 0 ) )
        {
            movedRect.moveBy( -m_moveSnapDiff.x(), -m_moveSnapDiff.y() );
            move -= m_moveSnapDiff;
        }

        KoPoint diff( m_gl.snapToGuideLines( movedRect, 4 ) );

        if ( diff != KoPoint( 0, 0 ) )
        {
            m_moveSnapDiff = diff;
            move += diff;
        }
        else
        {
            m_moveSnapDiff = KoPoint( 0, 0 );
        }
    }
    else if ( snapToGrid && snapToGuideLines )
    {
        move = diffGrid( rect, diffx, diffy );
        KoPoint diff( m_gl.diffGuide( rect, diffx, diffy ) );
        if ( diffx > 0 )
        {
            if ( diff.x() > move.x() )
            {
                move.setX( diff.x() );
            }
        }
        else
        {
            if ( diff.x() < move.x() )
            {
                move.setX( diff.x() );
            }
        }
        if ( diffy > 0 )
        {
            if ( diff.y() > move.y() )
            {
                move.setY( diff.y() );
            }
        }
        else
        {
            if ( diff.y() < move.y() )
            {
                move.setY( diff.y() );
            }
        }
        mouseDiff = move;
    }
    else
    {
        move.setX( diffx );
        move.setY( diffy );
    }

    // don't move object from canvas
    KoRect pageRect( m_activePage->getPageRect() );
    if ( rect.left() + move.x() < pageRect.left() )
    {
        move.setX( pageRect.left() - rect.left() );
        mouseDiff.setX( pageRect.left() - rect.left() );
    }
    else if ( rect.right() + move.x() > pageRect.right() )
    {
        move.setX( pageRect.right() - rect.right() );
        mouseDiff.setX( pageRect.right() - rect.right() );
    }
    if ( rect.top() + move.y() < pageRect.top() )
    {
        move.setY( pageRect.top() - rect.top() );
        mouseDiff.setY( pageRect.top() - rect.top() );
    }
    else if ( rect.bottom() + move.y() > pageRect.bottom() )
    {
        move.setY( pageRect.bottom() - rect.bottom() );
        mouseDiff.setY( pageRect.bottom() - rect.bottom() );
    }

    if ( move != KoPoint( 0, 0 ) )
    {
        //kdDebug(33001) << "moveObjectsByMouse move = " << move << endl;
        m_activePage->moveObject( m_view, move, false );
        scrollCanvas( move );
    }
    m_origMousePos += mouseDiff;
}


void KPrCanvas::resizeObject( ModifyType _modType, int _dx, int _dy, bool doApplyGrid )
{
    double dx = m_view->zoomHandler()->unzoomItX( _dx);
    double dy = m_view->zoomHandler()->unzoomItY( _dy);
    KPObject *kpobject = m_resizeObject;

    QRect oldBoundingRect( m_view->zoomHandler()->zoomRect( kpobject->getRepaintRect() ) );

    KoSize objSize = kpobject->getSize();
    KoRect objRect = kpobject->getRealRect();
    KoRect pageRect=m_activePage->getPageRect();
    int pageNum = m_view->kPresenterDoc()->pageList().findRef( m_activePage );
    QPainter p;
    p.begin( this );
    kpobject->moveBy(m_view->zoomHandler()->unzoomItX(-diffx()),m_view->zoomHandler()->unzoomItY(-diffy()));
    kpobject->draw( &p, m_view->zoomHandler(), pageNum, SM_MOVERESIZE,
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
        dx = ( doApplyGrid ? applyGridX( objRect.left() + dx ) : ( objRect.left() + dx ) ) - objRect.left();
        dy = ( doApplyGrid ? applyGridY( objRect.top() + dy ) : ( objRect.top() + dy ) ) - objRect.top();
        if ( m_keepRatio && m_ratio != 0.0 )
            calcRatio( dx, dy, _modType, m_ratio );
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
        dx = ( doApplyGrid ? applyGridX( objRect.left() + dx ) : ( objRect.left() + dx ) ) - objRect.left();
        if ( m_keepRatio && m_ratio != 0.0 )
            calcRatio( dx, dy, _modType, m_ratio );
        kpobject->resizeBy( -dx, -dy );
        if ( objSize != kpobject->getSize() )
            kpobject->moveBy( KoPoint( dx, 0 ) );
    } break;
    case MT_RESIZE_LD: {
        if( (objRect.bottom() + dy) > pageRect.bottom())
            dy = pageRect.bottom() - objRect.bottom();
        if( (objRect.left() + dx) < (pageRect.left() - 1) )
            dx = pageRect.left() - objRect.left();
        dx = ( doApplyGrid ? applyGridX( objRect.left() + dx ) : ( objRect.left() + dx ) ) - objRect.left();
        dy = ( doApplyGrid ? applyGridY( objRect.bottom() + dy ) : ( objRect.bottom() + dy ) ) - objRect.bottom();
        if ( m_keepRatio && m_ratio != 0.0 )
            calcRatio( dx, dy, _modType, m_ratio );
        kpobject->resizeBy( -dx, dy );
        if ( objSize.width() != (kpobject->getSize()).width() )
            kpobject->moveBy( KoPoint( dx, 0 ) );
    } break;
    case MT_RESIZE_RU: {
        if( (objRect.right() + dx) > pageRect.right() )
            dx = pageRect.right() - objRect.right();
        if( (objRect.top() + dy) < (pageRect.top() - 1) )
            dy = pageRect.top() - objRect.top();
        dx = ( doApplyGrid ? applyGridX( objRect.right() + dx ) : (objRect.right() + dx) )- objRect.right();
        dy = ( doApplyGrid ? applyGridY( objRect.top() + dy ) : (objRect.top() + dy ) ) - objRect.top();
        if ( m_keepRatio && m_ratio != 0.0 )
            calcRatio( dx, dy, _modType, m_ratio );
        kpobject->resizeBy( dx, -dy );
        if ( objSize.height() != (kpobject->getSize()).height() )
            kpobject->moveBy( KoPoint( 0, dy ) );
    } break;
    case MT_RESIZE_RT: {
        dy = 0;
        if( (objRect.right() + dx) > pageRect.right() )
            dx = pageRect.right() - objRect.right();
        dx = ( doApplyGrid ? applyGridX( objRect.right() + dx ) : (objRect.right() + dx) ) - objRect.right();
        if ( m_keepRatio && m_ratio != 0.0 )
            calcRatio( dx, dy, _modType, m_ratio );
        kpobject->resizeBy( dx, dy );
    } break;
    case MT_RESIZE_RD: {
        if( (objRect.bottom() + dy) > pageRect.bottom() )
            dy = pageRect.bottom() - objRect.bottom();
        if( (objRect.right() + dx) > pageRect.right() )
            dx = pageRect.right() - objRect.right();
        dx = ( doApplyGrid ? applyGridX( objRect.right() + dx ) : ( objRect.right() + dx ) )- objRect.right();
        dy = ( doApplyGrid ? applyGridY( objRect.bottom() + dy ) : ( objRect.bottom() + dy ) )- objRect.bottom();
        if ( m_keepRatio && m_ratio != 0.0 )
            calcRatio( dx, dy, _modType, m_ratio );
        kpobject->resizeBy( dx, dy );
    } break;
    case MT_RESIZE_UP: {
        dx = 0;
        if( (objRect.top() + dy) < (pageRect.top() - 1) )
            dy = pageRect.top() - objRect.top();
        dy = ( doApplyGrid ? applyGridY( objRect.top() + dy) : ( objRect.top() + dy) ) - objRect.top();
        if ( m_keepRatio && m_ratio != 0.0 )
            calcRatio( dx, dy, _modType, m_ratio );
        kpobject->resizeBy( -dx, -dy );
        if ( objSize != kpobject->getSize() )
            kpobject->moveBy( KoPoint( 0, dy ) );

    } break;
    case MT_RESIZE_DN: {
        dx = 0;
        if( (objRect.bottom() + dy) > pageRect.bottom() )
            dy = pageRect.bottom() - objRect.bottom();
        dy = ( doApplyGrid ? applyGridY( objRect.bottom() + dy ) : ( objRect.bottom() + dy ) ) - objRect.bottom();
        if ( m_keepRatio && m_ratio != 0.0 )
            calcRatio( dx, dy, _modType, m_ratio );
        kpobject->resizeBy( dx, dy );
    } break;
    default: break;
    }
    kpobject->draw( &p, m_view->zoomHandler(), pageNum, SM_MOVERESIZE,
                    (kpobject->isSelected()) && drawContour );
    kpobject->moveBy(m_view->zoomHandler()->unzoomItX(diffx()),m_view->zoomHandler()->unzoomItY(diffy()));
    p.end();

    _repaint( oldBoundingRect );
    _repaint( kpobject );
    emit objectSizeChanged();
}

void KPrCanvas::finishResizeObject( const QString &name, int mx, int my, bool layout )
{
    if ( m_resizeObject )
    {
        KoPoint move = KoPoint( m_resizeObject->getOrig().x() - m_rectBeforeResize.x(),
                                m_resizeObject->getOrig().y() - m_rectBeforeResize.y() );
        KoSize size = KoSize( m_resizeObject->getSize().width() - m_rectBeforeResize.width(),
                              m_resizeObject->getSize().height() - m_rectBeforeResize.height() );

        if ( firstX != mx || firstY != my ) {
            ResizeCmd *resizeCmd = new ResizeCmd( name, move, size, m_resizeObject,
                                                  m_view->kPresenterDoc() );
            // the command is not executed as the object is allready resized.
            m_view->kPresenterDoc()->addCommand( resizeCmd );
        }

        if ( layout )
            m_view->kPresenterDoc()->layout( m_resizeObject );

        m_ratio = 0.0;
        m_keepRatio = false;
        m_isResizing = false;
        if ( !m_view->kPresenterDoc()->showGrid() && m_view->kPresenterDoc()->snapToGrid() )
          repaint( false );
        else
          _repaint( m_resizeObject );
        m_resizeObject = NULL;
    }
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
        //tz not needed
        else
            selectedObjectPosition = -1;
    }
}

void KPrCanvas::lowerObject()
{
    if ( selectedObjectPosition != -1 )
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
        selectedObjectPosition = -1;
    }
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
    return m_activePage->oneObjectTextSelected();
}

bool KPrCanvas::oneObjectTextExist() const
{
    return m_activePage->oneObjectTextExist();
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
}

void KPrCanvas::setMoveGuides( bool state )
{
    m_moveGuides = state;
}

bool KPrCanvas::objectIsAHeaderFooterHidden(KPObject *obj) const
{
    if (( m_view->kPresenterDoc()->isHeader(obj) && !m_activePage->hasHeader() )
        || ( m_view->kPresenterDoc()->isFooter(obj) && !m_activePage->hasFooter() ) )
        return true;
    return false;
}

int KPrCanvas::numberOfObjectSelected() const
{
    int nb = m_activePage->numSelected();
    return nb;
}

KPObject *KPrCanvas::getSelectedObj() const
{
    KPObject *obj = m_activePage->getSelectedObj();
    return obj;
}

void KPrCanvas::ungroupObjects()
{
    KMacroCommand *macro = 0;
    m_activePage->ungroupObjects( &macro );
    if ( macro )
        m_view->kPresenterDoc()->addCommand( macro );
}

void KPrCanvas::groupObjects()
{
    m_activePage->groupObjects();
}

KoRect KPrCanvas::objectRect( bool all ) const
{
    return m_activePage->getRealRect( all );
}

void KPrCanvas::scrollTopLeftPoint( const QPoint & pos )
{
    m_view->getHScrollBar()->setValue( pos.x() );
    m_view->getVScrollBar()->setValue( pos.y() );
}

void KPrCanvas::scrollCanvas( const KoPoint & diff )
{
    QApplication::sendPostedEvents();
    QRect pageRect = visibleRect();
    pageRect.moveBy( diffx(), diffy() );
    KoRect visiblePageRect = m_view->zoomHandler()->unzoomRect( pageRect );
    KoRect objRect = objectRect( false );

    if ( ! visiblePageRect.contains( objRect ) )
    {
        QPoint d = m_view->zoomHandler()->zoomPoint( diff );
        m_view->getHScrollBar()->setValue( m_view->getHScrollBar()->value() + d.x() );
        m_view->getVScrollBar()->setValue( m_view->getVScrollBar()->value() + d.y() );
    }
}

void KPrCanvas::changePicture( const KURL & url, QWidget *window )
{
    m_activePage->changePicture( url, window );
}

unsigned int KPrCanvas::objNums() const
{
    return m_activePage->objNums();
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
             it.current()->getType() != OT_TEXT )
        {
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

void KPrCanvas::lowerObjs( bool backward )
{
    lowerObject();
    m_activePage->lowerObjs( backward );
}

void KPrCanvas::raiseObjs( bool forward )
{
    lowerObject();
    m_activePage->raiseObjs( forward );
}

QPtrList<KPTextObject> KPrCanvas::listOfTextObjs() const
{
    QPtrList<KPTextObject> lst;
    QPtrListIterator<KPObject> it(getObjectList());
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

void KPrCanvas::alignObjects( AlignType at )
{
    KPresenterDoc * doc = m_view->kPresenterDoc();

    QString name;

    switch ( at )
    {
        case AT_LEFT:
            name = i18n( "Align Objects Left" );
            break;
        case AT_TOP:
            name = i18n( "Align Objects Top" );
            break;
        case AT_RIGHT:
            name = i18n( "Align Objects Right" );
            break;
        case AT_BOTTOM:
            name = i18n( "Align Objects Bottom" );
            break;
        case AT_HCENTER:
            name = i18n( "Align Objects Centered (horizontal)" );
            break;
        case AT_VCENTER:
            name = i18n( "Align Objects Center/Vertical" );
            break;
    }

    QPtrList<KPObject> objects;

    QPtrListIterator<KPObject> it( m_activePage->objectList() );
    for ( ; it.current() ; ++it )
    {
        if ( it.current() == m_view->kPresenterDoc()->header() ||
             it.current() == m_view->kPresenterDoc()->footer() )
            continue;

        if( it.current()->isSelected() && !it.current()->isProtect() ) {
            objects.append( it.current() );
        }
    }

    if ( objects.count() )
    {
        KCommand * cmd = new AlignCmd( name, objects, at, doc );
        cmd->execute();
        doc->addCommand( cmd );
    }
}

bool KPrCanvas::canMoveOneObject() const
{
    return m_activePage->canMoveOneObject();
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
    KCommand *cmd = m_activePage->alignVertical( _type );
    if ( cmd )
        m_view->kPresenterDoc()->addCommand( cmd );
}

KPPixmapObject * KPrCanvas::getSelectedImage() const
{
    return m_activePage->getSelectedImage();
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
