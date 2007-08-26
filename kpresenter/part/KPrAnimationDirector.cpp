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
#include <KoShapeManagerPaintingStrategy.h>
#include <KoViewConverter.h>
#include <KoPACanvas.h>
#include <KoPAPageBase.h>
#include <KoPAView.h>
#include "KPrPage.h"
#include "KPrShapeManagerAnimationStrategy.h"
#include "pageeffects/KPrPageEffect.h"
#include "pageeffects/KPrCoverDownEffect.h"
#include "shapeanimations/KPrShapeAnimation.h"

KPrAnimationDirector::KPrAnimationDirector( KoPAView * view, const QList<KoPAPageBase*> & pages )
: m_view( view )
, m_canvas( view->kopaCanvas() )
, m_pages( pages )
, m_pageEffect( 0 )
, m_pageIndex( 0 )
, m_stepIndex( 0 )
{
    Q_ASSERT( !m_pages.empty() );

    updateActivePage( m_pages[0] );
    connect( &m_timeLine, SIGNAL( valueChanged( qreal ) ), this, SLOT( animate() ) );
    // this is needed as after a call to m_canvas->showFullScreen the canvas is not made fullscreen right away
    connect( m_canvas, SIGNAL( sizeChanged( const QSize & ) ), this, SLOT( updateZoom( const QSize & ) ) );
    m_timeLine.setCurveShape( QTimeLine::LinearCurve );
    m_timeLine.setUpdateInterval( 20 ); 
    // set the animation strategy in the KoShapeManagers
    // TODO also set for the master shape manager
    m_canvas->shapeManager()->setPaintingStrategy( new KPrShapeManagerAnimationStrategy( m_canvas->shapeManager(), this ) );
}

KPrAnimationDirector::~KPrAnimationDirector()
{
    //set the KoShapeManagerPaintingStrategy in the KoShapeManagers
    // TODO also set for the master shape manager
    m_canvas->shapeManager()->setPaintingStrategy( new KoShapeManagerPaintingStrategy( m_canvas->shapeManager() ) );
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

            // if there are animations starting right after the page effect start them now
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

bool KPrAnimationDirector::navigate( Navigation navigation )
{
    if ( m_pageEffect ) {
        m_pageEffect->finish();
        m_timeLine.stop();
    }
    else if ( m_timeLine.state() == QTimeLine::Running ) { // there are still shape animations running
        Q_ASSERT( !m_animations.isEmpty() );
        QMap<KoShape *, KPrShapeAnimation *>::iterator it( m_animations.begin() );
        for ( ; it != m_animations.end(); ++it ) {
            it.value()->finish( m_canvas );
        }
        m_timeLine.stop();
    }
    else {
        if ( nextStep() )
        {
            // this only needs to be done if there is a new page
            QPixmap oldPage( m_canvas->size() );
            m_canvas->render( &oldPage );

            updateActivePage( m_pages[m_pageIndex] );
            updateAnimations();

            QPixmap newPage( m_canvas->size() );
            newPage.fill( Qt::white ); // TODO
            QPainter newPainter( &newPage );
            newPainter.setClipRect( m_pageRect );
            newPainter.setRenderHint( QPainter::Antialiasing );
            paintStep( newPainter );
            m_pageEffect = new KPrCoverDownEffect( oldPage, newPage, m_canvas );
            m_timeLine.setDuration( m_pageEffect->duration() );
            m_timeLine.setCurrentTime( 0 );
            m_timeLine.start();
        }
        else {
            // todo what happens on a new substep
            m_timeLine.setCurrentTime( 0 );
            //m_timeLine.setDuration( m_maxShapeDuration );
            m_timeLine.start();
        }
    }

    return true;
}

bool KPrAnimationDirector::shapeShown( KoShape * shape )
{
    return !m_animations.contains( shape ) || m_animations[shape] != 0;
}

KPrShapeAnimation * KPrAnimationDirector::shapeAnimation( KoShape * shape )
{
    QMap<KoShape *, KPrShapeAnimation *>::iterator it( m_animations.find( shape ) );

    return ( it != m_animations.end() ) ? it.value() : 0;
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

bool KPrAnimationDirector::nextStep()
{
    bool newPage = false;

    // check if there are still substeps
    if ( ! m_steps.isEmpty() && m_stepIndex < m_steps.size() - 1 ) {
        // if there are sub steps go to the next substep
        ++m_stepIndex;
        updateAnimations();
    }
    else {
        // if there are no more sub steps go to the next page
        // The active page and the substeps are updated later as
        // first the current page has to be painted again for the page effect
        m_pageIndex = m_pageIndex < m_pages.size() - 1 ? m_pageIndex + 1 : 0;
        m_stepIndex = 0;
        newPage = true;
    }
    return newPage;
}

void KPrAnimationDirector::updateAnimations()
{
    m_animations.clear();
    // TODO also get the animations for the master page
    KPrAnimationController * controller = dynamic_cast<KPrAnimationController*>( m_pages[m_pageIndex] );
    Q_ASSERT( controller );
    if ( m_steps.size() > m_stepIndex ) {
        m_animations = controller->animations().animations( m_steps[m_stepIndex] );
    }
}

void KPrAnimationDirector::animate()
{
    if ( m_pageEffect ) {
        m_pageEffect->next( m_timeLine.currentTime() );
    }
    else if ( ! m_animations.empty() ) { //if there are animnations
        QMap<KoShape *, KPrShapeAnimation *>::iterator it( m_animations.begin() );
        for ( ; it != m_animations.end(); ++it ) {
            it.value()->next( m_timeLine.currentTime(), m_canvas );
        }
    }
}

#include "KPrAnimationDirector.moc"
