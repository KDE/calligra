// -*- Mode: c++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4; -*-
/* This file is part of the KDE project
   Copyright (C) 2005 Thorsten Zachmann <zachmann@kde.org>

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
   Boston, MA 02110-1301, USA.
*/

#include "koGuides.h"

#include <qcursor.h>
#include <qpainter.h>
#include <qpixmap.h>

#include <klocale.h>
#include <kpopupmenu.h>

#include <koDocument.h>
#include <koPoint.h>
#include <koRect.h>
#include <koView.h>
#include <kozoomhandler.h>

#include "koGuideLineDia.h"

class KoGuides::Popup : public KPopupMenu
{
public: 
    Popup( KoGuides * guides )
    {
        m_title = insertTitle( i18n( "Guide Line" ) );
        m_delete = insertItem( i18n( "&Delete" ), guides, SLOT( slotRemove() ) );
        m_seperator = insertSeparator();
        m_pos = insertItem( i18n( "&Set Position..." ), guides, SLOT( slotChangePosition() ) );
    }

    void update( int count )
    {
        if ( count == 1 )
        {
            changeTitle( m_title, i18n( "Guide Line" ) );
            setItemVisible( m_seperator, true );
            setItemVisible( m_pos, true );
        }
        else
        {
            changeTitle( m_title, i18n( "Guide Lines" ) );
            setItemVisible( m_seperator, false );
            setItemVisible( m_pos, false );
        }
    }
private:
    int m_title;
    int m_delete;
    int m_seperator;
    int m_pos;
};

KoGuides::KoGuides( KoView *view, KoZoomHandler *zoomHandler, bool autoStyle )
: m_view( view )
, m_zoomHandler( zoomHandler )
, m_autoStyle(autoStyle)
{
    m_popup = new Popup( this );
}


KoGuides::~KoGuides()
{
    delete m_popup;
}


void KoGuides::paintGuides( QPainter &painter )
{
    //painter.setRasterOp( NotROP );
    painter.setPen( QPen( blue, 0, DotLine ) );
    const KoPageLayout& pl = m_view->koDocument()->pageLayout();
    int width = QMAX( m_view->canvas()->width(), m_zoomHandler->zoomItX( pl.ptWidth ) );
    int height = QMAX( m_view->canvas()->height(), m_zoomHandler->zoomItY( pl.ptHeight ) );

    QValueList<KoGuideLine *>::iterator it = m_guideLines.begin();
    for ( ; it != m_guideLines.end(); ++it )
    {
        if (! m_autoStyle || ( *it )->snapping )
        {
            // dont paint autoStyle guides when they are not snapping

            painter.save();
            if ( ( *it )->orientation == Qt::Vertical )
            {
                painter.translate( m_zoomHandler->zoomItX( ( *it )->position ), 0 );
                painter.drawLine( 0, 0, 0, height );
            }
            else
            {
                painter.translate( 0, m_zoomHandler->zoomItY( ( *it )->position ) );
                painter.drawLine( 0, 0, width, 0 );
            }
            painter.restore();
        }
    }

    painter.setPen( QPen( red, 0, DotLine ) );
    it = m_selectedGuideLines.begin();
    for ( ; it != m_selectedGuideLines.end(); ++it )
    {
        // no need to take care of autoStyle guides as they are never selected
        painter.save();
        if ( ( *it )->orientation == Qt::Vertical )
        {
            painter.translate( m_zoomHandler->zoomItX( ( *it )->position ), 0 );
            painter.drawLine( 0, 0, 0, height );
        }
        else
        {
            painter.translate( 0, m_zoomHandler->zoomItY( ( *it )->position ) );
            painter.drawLine( 0, 0, width, 0 );
        }
        painter.restore();
    }
}

bool KoGuides::mousePressEvent( QMouseEvent *e )
{
    bool eventProcessed = true;
    bool changed = false;
    m_mouseSelected = false;
    
    KoPoint p( mapFromScreen( e->pos() ) );
    KoGuideLine * guideLine = find( p, m_zoomHandler->unzoomItY( 2 ) );
    if ( guideLine )
    {
        if( m_autoStyle )
            return false;

        m_lastPoint = e->pos();
        if ( e->button() == Qt::LeftButton || e->button() == Qt::RightButton )
        {
            if ( e->button() == Qt::LeftButton )
            {
                m_mouseSelected = true;
            }
            if ( e->state() & Qt::ControlButton )
            {
                if ( guideLine->selected )
                {
                    unselect( guideLine );
                    m_mouseSelected = false;
                }
                else
                {
                    select( guideLine );
                }
                changed = true;
            }
            else if ( ! guideLine->selected )
            {
                unselectAll();
                select( guideLine );
                changed = true;
            }
        }
    }
    else 
    {   
        if ( !( e->state() && Qt::ControlButton ) )
        {
            changed = unselectAll();
        }
        eventProcessed = false;
    }
    
    if ( changed || hasSelected() )
    {
        emit moveGuides( true );
    }

    if ( changed )
    {
        paint();
    }

    if ( changed && ! hasSelected() )
    {
        emit moveGuides( false );
    }

    if ( e->button() == Qt::RightButton && hasSelected() )
    {
        m_popup->update( m_selectedGuideLines.count() );
        m_popup->exec( QCursor::pos() );
    }
    
    return eventProcessed;
}


bool KoGuides::mouseMoveEvent( QMouseEvent *e )
{
    //kdDebug(33001) << "KoGuideLines::mouseMoveEvent" << endl;
    bool eventProcessed = false;
    if ( m_mouseSelected )
    {
        QPoint p( e->pos() );
        p -= m_lastPoint;
        m_lastPoint = e->pos();
        moveSelectedBy( p );
        paint();
        emit guideLinesChanged( m_view );
        eventProcessed = true;
    }
    else if ( e->state() == Qt::NoButton )
    {
        KoPoint p( mapFromScreen( e->pos() ) );
        KoGuideLine * guideLine = find( p, m_zoomHandler->unzoomItY( 2 ) );
        if ( guideLine )
        {
            m_view->canvas()->setCursor( guideLine->orientation == Qt::Vertical ? Qt::sizeHorCursor : Qt::sizeVerCursor );
            eventProcessed = true;
        }
    }
    return eventProcessed;
}


bool KoGuides::mouseReleaseEvent( QMouseEvent *e )
{
    bool eventProcessed = false;
    if ( m_mouseSelected )
    {
        KoPoint p( mapFromScreen( e->pos() ) );
        if ( m_selectedGuideLines.count() == 1 )
        {
            int x1, y1, x2, y2;
            m_view->canvas()->rect().coords( &x1, &y1, &x2, &y2 );
            QPoint gp( m_view->canvas()->mapFromGlobal( e->globalPos() ) );
            if ( m_selectedGuideLines.first()->orientation == Qt::Vertical )
            {
                if ( gp.x() < x1 || gp.x() > x2 )
                    removeSelected();
            }
            else
            {
                if ( gp.y() < y1 || gp.y() > y2 )
                    removeSelected();
            }
        }
        KoGuideLine * guideLine = find( p, m_zoomHandler->unzoomItY( 2 ) );
        if ( guideLine )
        {
            m_view->canvas()->setCursor( guideLine->orientation == Qt::Vertical ? Qt::sizeHorCursor : Qt::sizeVerCursor );
        }
        m_mouseSelected = false;
        eventProcessed = true;
        emit guideLinesChanged( m_view );
    }
        emit moveGuides( false );
    return eventProcessed;
}


bool KoGuides::keyPressEvent( QKeyEvent *e )
{
    //kdDebug( 33001 ) << "KoGuideLines::keyPressEvent" << endl;
    bool eventProcessed = false;
    switch( e->key() )
    {
        case Qt::Key_Delete: 
            if ( hasSelected() )
            {
                removeSelected();
                paint();
                emit guideLinesChanged( m_view );
                eventProcessed = true;
            }
            break;
        default:
            break;
    }
    return eventProcessed;
}

void KoGuides::setGuideLines( const QValueList<double> &horizontalPos, const QValueList<double> &verticalPos )
{
    removeSelected();

    QValueList<KoGuideLine *>::iterator it = m_guideLines.begin();
    for ( ; it != m_guideLines.end(); ++it )
    {
        delete ( *it );
    }
    m_guideLines.clear();

    QValueList<double>::ConstIterator posIt = horizontalPos.begin();
    for ( ; posIt != horizontalPos.end(); ++posIt )
    {
        KoGuideLine *guideLine = new KoGuideLine( Qt::Horizontal, *posIt );
        m_guideLines.append( guideLine );
    }
    posIt = verticalPos.begin();
    for ( ; posIt != verticalPos.end(); ++posIt )
    {
        KoGuideLine *guideLine = new KoGuideLine( Qt::Vertical, *posIt );
        m_guideLines.append( guideLine );
    }
    paint();
}


void KoGuides::getGuideLines( QValueList<double> &horizontalPos, QValueList<double> &verticalPos ) const
{
    horizontalPos.clear();
    verticalPos.clear();

    QValueList<KoGuideLine *>::const_iterator it = m_guideLines.begin();
    for ( ; it != m_guideLines.end(); ++it )
    {
        if ( ( *it )->orientation == Qt::Horizontal )
        {
            horizontalPos.append( ( *it )->position );
        }
        else
        {
            verticalPos.append( ( *it )->position );
        }
    }
    it = m_selectedGuideLines.begin();
    for ( ; it != m_selectedGuideLines.end(); ++it )
    {
        if ( ( *it )->orientation == Qt::Horizontal )
        {
            horizontalPos.append( ( *it )->position );
        }
        else
        {
            verticalPos.append( ( *it )->position );
        }
    }
}


KoPoint KoGuides::snapToGuideLines( KoRect &rect, int snap)
{
    bool needRepaint = false;
    KoPoint diff( 10000, 10000 );
    KoGuideLine *vClosest=0;
    KoGuideLine *hClosest=0;

    QValueList<KoGuideLine *>::const_iterator it = m_guideLines.begin();
    for ( ; it != m_guideLines.end(); ++it )
    {
        if(( *it )->snapping)
            needRepaint = true;

        ( *it )->snapping = false;

        if ( ( *it )->orientation == Qt::Horizontal )
        {
            double tmp = rect.top() - (*it)->position;
            if ( QABS( tmp ) < m_zoomHandler->unzoomItY( snap ) )
            {
                if(QABS( tmp ) < diff.y())
                {
                    diff.setY( tmp );
                    hClosest = *it;
                }
            }
            tmp = rect.bottom() - (*it)->position;
            if ( QABS( tmp ) < m_zoomHandler->unzoomItY( snap ) )
            {
                if(QABS( tmp ) < diff.y())
                {
                    diff.setY( tmp );
                    hClosest = *it;
                }
            }
        }
        else
        {
            double tmp = rect.left() - (*it)->position;
            if ( QABS( tmp ) < m_zoomHandler->unzoomItX( snap ) )
            {
                if(QABS( tmp ) < diff.x())
                {
                    diff.setX( tmp );
                    vClosest = *it;
                }
            }
            tmp = rect.right() - (*it)->position;
            if ( QABS( tmp ) < m_zoomHandler->unzoomItX( snap ) )
            {
                if(QABS( tmp ) < diff.x())
                {
                    diff.setX( tmp );
                    vClosest = *it;
                }
            }
        }
    }

    it = m_selectedGuideLines.begin();
    for ( ; it != m_selectedGuideLines.end(); ++it )
    {
        ( *it )->snapping = false;

        if ( ( *it )->orientation == Qt::Horizontal )
        {
            double tmp = rect.top() - (*it)->position;
            if ( QABS( tmp ) < m_zoomHandler->unzoomItY( snap ) )
            {
                if(QABS( tmp ) < diff.y())
                {
                    diff.setY( tmp );
                    hClosest = *it;
                }
            }
            tmp = rect.bottom() - (*it)->position;
            if ( QABS( tmp ) < m_zoomHandler->unzoomItY( snap ) )
            {
                if(QABS( tmp ) < diff.y())
                {
                    diff.setY( tmp );
                    hClosest = *it;
                }
            }
        }
        else
        {
            double tmp = rect.left() - (*it)->position;
            if ( QABS( tmp ) < m_zoomHandler->unzoomItX( snap ) )
            {
                if(QABS( tmp ) < diff.x())
                {
                    diff.setX( tmp );
                    vClosest = *it;
                }
            }
            tmp = rect.right() - (*it)->position;
            if ( QABS( tmp ) < m_zoomHandler->unzoomItX( snap ) )
            {
                if(QABS( tmp ) < diff.x())
                {
                    diff.setX( tmp );
                    vClosest = *it;
                }
            }
        }
     }

    if( vClosest )
    {
        vClosest->snapping = true;
        paint();
    }
    else
        diff.setX( 0 );

    if( hClosest )
    {
        hClosest->snapping = true;
        paint();
    }
    else
        diff.setY( 0 );

    if(needRepaint)
        paint();

    return diff;
}

KoPoint KoGuides::diffGuide( KoRect &rect, double diffx, double diffy )
{
    KoPoint move( 0, 0 );
    QValueList<double> horizHelplines;
    QValueList<double> vertHelplines;
    getGuideLines( horizHelplines, vertHelplines );

    QValueList<double>::const_iterator it( vertHelplines.begin() );
    for ( ; it != vertHelplines.end(); ++it )
    {
        double movexl = *it - rect.left();
        double movexr = *it - rect.right();
        if ( diffx > 0 )
        {
            if ( diffx > movexl && movexl > move.x() )
            {
                move.setX( movexl );
            }
            if ( diffx > movexr && movexr > move.x() )
            {
                move.setX( movexr );
            }
        }
        else
        {
            if ( diffx < movexl && movexl < move.x() )
            {
                move.setX( movexl );
            }
            if ( diffx < movexr && movexr < move.x() )
            {
                move.setX( movexr );
            }
        }
    }

    it = horizHelplines.begin();
    for ( ; it != horizHelplines.end(); ++it )
    {
        double moveyl = *it - rect.top();
        double moveyr = *it - rect.bottom();
        if ( diffy > 0 )
        {
            if ( diffy > moveyl && moveyl > move.y() )
            {
                move.setY( moveyl );
            }
            if ( diffy > moveyr && moveyr > move.y() )
            {
                move.setY( moveyr );
            }
        }
        else
        {
            if ( diffy < moveyl && moveyl < move.y() )
            {
                move.setY( moveyl );
            }
            if ( diffy < moveyr && moveyr < move.y() )
            {
                move.setY( moveyr );
            }
        }
    }

    return move;
}


void KoGuides::moveGuide( const QPoint &pos, bool horizontal, int rulerWidth )
{
    int x = pos.x() - rulerWidth;
    int y = pos.y() - rulerWidth;
    QPoint p( x, y );
    if ( !m_insertGuide )
    {
        if ( ! horizontal && x > 0 )
        {
            m_insertGuide = true;
            add( Qt::Vertical, p );
        }
        else if ( horizontal && y > 0 )
        {
            m_insertGuide = true;
            add( Qt::Horizontal, p );
        }
        if ( m_insertGuide )
        {
            QMouseEvent e( QEvent::MouseButtonPress, p, Qt::LeftButton, Qt::LeftButton );
            mousePressEvent( &e );
        }
    }
    else
    {
        QMouseEvent e( QEvent::MouseMove, p, Qt::NoButton, Qt::LeftButton );
        mouseMoveEvent( &e );
    }
}


void KoGuides::addGuide( const QPoint &pos, bool /* horizontal */, int rulerWidth )
{
    int x = pos.x() - rulerWidth;
    int y = pos.y() - rulerWidth;
    QPoint p( x, y );
    m_insertGuide = false;
    QMouseEvent e( QEvent::MouseButtonRelease, p, Qt::LeftButton, Qt::LeftButton );
    mouseReleaseEvent( &e );
}


void KoGuides::slotChangePosition()
{
    KoPoint p( mapFromScreen( m_lastPoint ) );
    KoGuideLine * guideLine = find( p, m_zoomHandler->unzoomItY( 2 ) );

    const KoPageLayout& pl = m_view->koDocument()->pageLayout();
    double max = 0.0;
    if ( guideLine->orientation == Qt::Vertical )
    {
        max = QMAX( pl.ptWidth, m_zoomHandler->unzoomItX( m_view->canvas()->size().width() + m_view->canvasXOffset() - 1 ) );
    }
    else
    {
        max = QMAX( pl.ptHeight, m_zoomHandler->unzoomItY( m_view->canvas()->size().height() + m_view->canvasYOffset() - 1 ) );
    }

    KoGuideLineDia dia( 0, guideLine->position, 0.0, max, m_view->koDocument()->unit() );
    if ( dia.exec() == QDialog::Accepted )
    {
        guideLine->position = dia.pos();
        paint();
    }
}


void KoGuides::slotRemove()
{
    removeSelected();
    paint();
}


void KoGuides::paint()
{
    m_view->canvas()->repaint( false );
}


void KoGuides::add( Qt::Orientation o, QPoint &pos )
{
    KoPoint p( mapFromScreen( pos ) );
    KoGuideLine *guideLine = new KoGuideLine( o, o == Qt::Vertical ? p.x(): p.y() );
    m_guideLines.append( guideLine );
}


void KoGuides::select( KoGuideLine *guideLine )
{
    guideLine->selected = true;
    if ( m_guideLines.remove( guideLine ) == 1 )
    {
        m_selectedGuideLines.append( guideLine );
    }
}


void KoGuides::unselect( KoGuideLine *guideLine )
{
    guideLine->selected = false;
    if ( m_selectedGuideLines.remove( guideLine ) == 1 )
    {
        m_guideLines.append( guideLine );
    }
}


bool KoGuides::unselectAll()
{
    bool selected = m_selectedGuideLines.empty() == false;
    
    QValueList<KoGuideLine *>::iterator it = m_selectedGuideLines.begin();
    for ( ; it != m_selectedGuideLines.end(); ++it )
    {
        ( *it )->selected = false;
        m_guideLines.append( *it );
    }
    m_selectedGuideLines.clear();
    
    return selected;
}


void KoGuides::removeSelected()
{
    QValueList<KoGuideLine *>::iterator it = m_selectedGuideLines.begin();
    for ( ; it != m_selectedGuideLines.end(); ++it )
    {
        delete ( *it );
    }
    m_selectedGuideLines.clear();
}


bool KoGuides::hasSelected()
{
    return m_selectedGuideLines.empty() == false;
}


KoGuides::KoGuideLine * KoGuides::find( KoPoint &p, double diff )
{
    QValueList<KoGuideLine *>::iterator it = m_selectedGuideLines.begin();
    for ( ; it != m_selectedGuideLines.end(); ++it )
    {
        if ( ( *it )->orientation == Qt::Vertical && QABS( ( *it )->position - p.x() ) < diff )
        {
            return *it;
        }
        if ( ( *it )->orientation == Qt::Horizontal && QABS( ( *it )->position - p.y() ) < diff )
        {
            return *it;
        }
    }

    it = m_guideLines.begin();
    for ( ; it != m_guideLines.end(); ++it )
    {
        if ( ( *it )->orientation == Qt::Vertical && QABS( ( *it )->position - p.x() ) < diff )
        {
            return *it;
        }
        if ( ( *it )->orientation == Qt::Horizontal && QABS( ( *it )->position - p.y() ) < diff )
        {
            return *it;
        }
    }
    return 0;
}


void KoGuides::moveSelectedBy( QPoint &p )
{
    KoPoint point( m_zoomHandler->unzoomPoint( p ) );
    if ( m_selectedGuideLines.count() > 1 )
    {
        const KoPageLayout& pl = m_view->koDocument()->pageLayout();
        double right = QMAX( pl.ptWidth, m_zoomHandler->unzoomItX( m_view->canvas()->width() + m_view->canvasXOffset() - 1 ) );
        double bottom = QMAX( pl.ptHeight, m_zoomHandler->unzoomItY( m_view->canvas()->height() + m_view->canvasYOffset() - 1 ) );

        QValueList<KoGuideLine *>::iterator it = m_selectedGuideLines.begin();
        for ( ; it != m_selectedGuideLines.end(); ++it )
        {
            if ( ( *it )->orientation == Qt::Vertical )
            {
                double tmp = ( *it )->position + point.x();
                if ( tmp < 0 )
                {
                    point.setX( point.x() - tmp );
                }
                else if ( tmp > right )
                {
                    point.setX( point.x() - ( tmp - right ) );
                }
            }
            else
            {
                double tmp = ( *it )->position + point.y();
                if ( tmp < 0 )
                {
                    point.setY( point.y() - tmp );
                }
                else if ( tmp > bottom )
                {
                    point.setY( point.y() - ( tmp - bottom ) );
                }
            }
        }
    }
    QValueList<KoGuideLine *>::iterator it = m_selectedGuideLines.begin();
    for ( ; it != m_selectedGuideLines.end(); ++it )
    {
        if ( ( *it )->orientation == Qt::Vertical && p.x() != 0 )
        {
            ( *it )->position = ( *it )->position + point.x();
        }
        else if ( ( *it )->orientation == Qt::Horizontal && p.y() != 0 )
        {
            ( *it )->position = ( *it )->position + point.y();
        }
    }
}


KoPoint KoGuides::mapFromScreen( const QPoint & pos )
{
    int x = pos.x() + m_view->canvasXOffset();
    int y = pos.y() + m_view->canvasYOffset();
    double xf = m_zoomHandler->unzoomItX( x );
    double yf = m_zoomHandler->unzoomItY( y );
    return KoPoint( xf, yf );
}


QPoint KoGuides::mapToScreen( const KoPoint & pos )
{
    int x = m_zoomHandler->zoomItX( pos.x() ) - m_view->canvasXOffset();
    int y = m_zoomHandler->zoomItY( pos.y() ) - m_view->canvasYOffset();
    return QPoint( x, y );
}

#include "koGuides.moc"
