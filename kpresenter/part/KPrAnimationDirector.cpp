/* This file is part of the KDE project
 * Copyright ( C ) 2007 Thorsten Zachmann <zachmann@kde.org>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (  at your option ) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#include "KPrAnimationDirector.h"

#include <QPainter>
#include <QPaintEvent>
#include <QWidget>
#include <kapplication.h>
#include <KoPageLayout.h>
#include <KoShapeManager.h>
#include <KoViewConverter.h>
#include <KoPACanvas.h>
#include <KoPAPageBase.h>
#include <KoPAView.h>
#include "pageeffects/KPrPageEffect.h"
#include "pageeffects/KPrCoverDownEffect.h"

KPrAnimationDirector::KPrAnimationDirector( KoPAView * view, const QList<KoPAPageBase*> & pages )
: m_view( view )
, m_canvas( view->kopaCanvas() )
, m_pages( pages )
, m_pageEffect( 0 )                  
{
    updateActivePage( m_pages[0] );
    connect( &m_timeLine, SIGNAL( valueChanged( qreal ) ), this, SLOT( animate() ) );
    // this is needed as after a call to m_canvas->showFullScreen the canvas is not made fullscreen right away
    connect( m_canvas, SIGNAL( sizeChanged( const QSize & ) ), this, SLOT( updateZoom( const QSize & ) ) );
    m_timeLine.setCurveShape( QTimeLine::LinearCurve );
    m_timeLine.setUpdateInterval( 20 ); 
}

KPrAnimationDirector::~KPrAnimationDirector()
{
}

void KPrAnimationDirector::paintEvent( QPaintEvent* event )
{
    QPainter painter( m_canvas );

    if ( m_pageEffect )
    {
        if ( !m_pageEffect->paint( painter, m_timeLine.currentTime() ) )
        {
            delete m_pageEffect;
            m_pageEffect = 0;
        }
    }
    else
    {
        QRect clipRect = m_pageRect.intersected( event->rect() );
        painter.setClipRect( clipRect );
        painter.setRenderHint( QPainter::Antialiasing );
        paintStep( painter );
    }
    // This is needed as otherwise on some ATI graphic cards it leads to
    // 100% CPU load of the x server and no more key events are received 
    // until the page effect is finished. With it is made sure that key
    // events still get through so that it is possible to cancel the 
    // events. It looks like this is not a problem with nvidia graphic
    // cards.
    KApplication::kApplication()->syncX();
}

bool KPrAnimationDirector::navigate()
{
    if ( m_pageEffect ) {
        m_pageEffect->finish();
        m_timeLine.stop();
    }
    else {
        nextStep();
        
        QPixmap oldPage( m_canvas->size() );
        m_canvas->render( &oldPage );

        updateActivePage( m_pages[m_pageIndex] );

        QPixmap newPage( m_canvas->size() );
        newPage.fill( Qt::white ); // TODO
        QPainter newPainter( &newPage );
        newPainter.setClipRect( m_pageRect );
        newPainter.setRenderHint( QPainter::Antialiasing );
        paintStep( newPainter );
        m_pageEffect = new KPrCoverDownEffect( oldPage, newPage, m_canvas );
        m_timeLine.setCurrentTime( 0 );
        m_timeLine.setDuration( m_pageEffect->duration() );
        m_timeLine.start();
    }

    return true;
}

void KPrAnimationDirector::updateActivePage( KoPAPageBase * page )
{
    m_view->setActivePage( page );
    // it can be that the pages have different sizes. So we need to recalulate
    // the zoom when we change the page
    updateZoom( m_canvas->size() );
}

void KPrAnimationDirector::updateZoom( const QSize & size )
{
    KoPageLayout pageLayout = m_view->activePage()->pageLayout();
    double zoom = size.width() / ( m_zoomHandler.resolutionX() * pageLayout.width );
    zoom = qMin( zoom, size.height() / ( m_zoomHandler.resolutionY() * pageLayout.height ) );
    m_zoomHandler.setZoom( zoom );

    int width = int( 0.5 + m_zoomHandler.documentToViewX( pageLayout.width ) );
    int height = int( 0.5 + m_zoomHandler.documentToViewY( pageLayout.height ) );
    int x = int( ( m_canvas->width() - width ) / 2.0 );
    int y = int( ( m_canvas->height() - height ) / 2.0 );

    m_pageRect = QRect( x, y, width, height );
}

void KPrAnimationDirector::paintStep( QPainter & painter )
{
    painter.drawRect( m_pageRect );
    painter.translate( m_pageRect.topLeft() );
    m_canvas->masterShapeManager()->paint( painter, m_zoomHandler, false );
    m_canvas->shapeManager()->paint( painter, m_zoomHandler, false );
}

void KPrAnimationDirector::nextStep()
{
    m_pageIndex = m_pageIndex < m_pages.size() - 1 ? m_pageIndex + 1 : 0;
}

void KPrAnimationDirector::animate()
{
    if ( m_pageEffect ) {
        m_pageEffect->next( m_timeLine.currentTime() );
    }
}

#include "KPrAnimationDirector.moc"
