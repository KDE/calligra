/* This file is part of the KDE project
 * Copyright (C) 2007-2008 Thorsten Zachmann <zachmann@kde.org>
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
#include <kdebug.h>
#include <KoPageLayout.h>
#include <KoShapeManager.h>
#include <KoShapeManagerPaintingStrategy.h>
#include <KoViewConverter.h>
#include <KoPAViewMode.h>
#include <KoPACanvas.h>
#include <KoPAPageBase.h>
#include <KoPAView.h>
#include <KoPAUtil.h>

#include "KPrEndOfSlideShowPage.h"
#include "KPrPage.h"
#include "KPrMasterPage.h"
#include "KPrPageApplicationData.h"
#include "KPrShapeManagerAnimationStrategy.h"
#include "KPrShapeManagerDisplayMasterStrategy.h"
#include "KPrPageSelectStrategyActive.h"
#include "pageeffects/KPrPageEffectRunner.h"
#include "pageeffects/KPrPageEffect.h"
#include "shapeanimations/KPrAnimationData.h"
#include "shapeanimations/KPrShapeAnimationOld.h"

#include <KoShapeLayer.h>
#include <KoPAMasterPage.h>
#include <KoSelection.h>

KPrAnimationDirector::KPrAnimationDirector( KoPAView * view, KoPACanvas * canvas, const QList<KoPAPageBase*> & pages, KoPAPageBase* currentPage )
: m_view( view )
, m_canvas( canvas )
, m_pages( pages )
, m_pageEffectRunner( 0 )
, m_stepIndex( 0 )
, m_maxShapeDuration( 0 )
, m_hasAnimation( false )
{
    Q_ASSERT( !m_pages.empty() );

    if( !currentPage || !pages.contains(currentPage))
        updateActivePage( m_pages[0] );
    else
        updateActivePage( currentPage );

    m_pageIndex = m_pages.indexOf( m_view->activePage() );

    updateAnimations();
    connect( &m_timeLine, SIGNAL( valueChanged( qreal ) ), this, SLOT( animate() ) );
    // this is needed as after a call to m_canvas->showFullScreen the canvas is not made fullscreen right away
    connect( m_canvas, SIGNAL( sizeChanged( const QSize & ) ), this, SLOT( updateZoom( const QSize & ) ) );
    m_timeLine.setCurveShape( QTimeLine::LinearCurve );
    m_timeLine.setUpdateInterval( 20 );
    // set the animation strategy in the KoShapeManagers
    m_canvas->shapeManager()->setPaintingStrategy( new KPrShapeManagerAnimationStrategy( m_canvas->shapeManager(), this,
                                                       new KPrPageSelectStrategyActive( m_view ) ) );
    m_canvas->masterShapeManager()->setPaintingStrategy( new KPrShapeManagerAnimationStrategy( m_canvas->masterShapeManager(), this,
                                                             new KPrPageSelectStrategyActive( m_view ) ) );

    if ( hasAnimation() ) {
        startTimeLine( m_maxShapeDuration );
    }
}

KPrAnimationDirector::~KPrAnimationDirector()
{
    // free used resources
    delete m_pageEffectRunner;
    clearAnimations();
    //set the KoShapeManagerPaintingStrategy in the KoShapeManagers
    m_canvas->shapeManager()->setPaintingStrategy( new KoShapeManagerPaintingStrategy( m_canvas->shapeManager() ) );
    m_canvas->masterShapeManager()->setPaintingStrategy( new KPrShapeManagerDisplayMasterStrategy( m_canvas->masterShapeManager(),
                                                             new KPrPageSelectStrategyActive( m_view ) ) );
}

void KPrAnimationDirector::paintEvent( QPaintEvent* event )
{
    QPainter painter( m_canvas );

    if ( m_pageEffectRunner )
    {
        bool finished = m_pageEffectRunner->isFinished();
        if ( !m_pageEffectRunner->paint( painter ) )
        {
            kDebug(33001) << "XXXXXXXXXXXXXX delete old runner" << m_pageEffectRunner;
            delete m_pageEffectRunner;
            m_pageEffectRunner = 0;

            // check if there where a animation to start
            if ( hasAnimation() ) {
                if ( finished ) {
                    QRect clipRect = m_pageRect.intersected( event->rect() );
                    painter.setClipRect( clipRect );
                    painter.setRenderHint( QPainter::Antialiasing );
                    paintStep( painter );
                }
                else {
                    // start the animations
                    startTimeLine( m_maxShapeDuration );
                }
            }
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

KoViewConverter * KPrAnimationDirector::viewConverter()
{
    return &m_zoomHandler;
}

int KPrAnimationDirector::numPages() const
{
    return m_pages.size();
}

int KPrAnimationDirector::currentPage() const
{
    return m_pageIndex;
}

int KPrAnimationDirector::numStepsInPage() const
{
    return m_steps.size();
}

int KPrAnimationDirector::currentStep() const
{
    return m_stepIndex;
}

bool KPrAnimationDirector::navigate( Navigation navigation )
{
    bool finished = false;
    if ( m_pageEffectRunner ) {
        m_pageEffectRunner->finish();
        finishAnimations();
        // finish on first step
        m_timeLine.stop();
        finished = true;
    }
    else if ( m_timeLine.state() == QTimeLine::Running ) { // there are still shape animations running
        Q_ASSERT( !m_animations.isEmpty() );
        finishAnimations();
        m_timeLine.stop();
        finished = true;
    }

    bool presentationFinished = false;

    switch ( navigation )
    {
        case FirstPage:
        case PreviousPage:
        case NextPage:
        case LastPage:
            presentationFinished = changePage( navigation );
            break;
        case PreviousStep:
            previousStep();
            break;
        case NextStep:
            if ( !finished ) {
                presentationFinished = nextStep();
            }
            break;
        default:
            break;
    }

    return presentationFinished;
}

void KPrAnimationDirector::navigateToPage( int index )
{
    if ( m_pageEffectRunner ) {
        m_pageEffectRunner->finish();
        finishAnimations();
        // finish on first step
        m_timeLine.stop();
    }
    else if ( m_timeLine.state() == QTimeLine::Running ) { // there are still shape animations running
        Q_ASSERT( !m_animations.isEmpty() );
        finishAnimations();
        m_timeLine.stop();
    }

    m_pageIndex = index;
    KoPAPageBase *page = m_pages[m_pageIndex];

    m_stepIndex = 0;

    updateActivePage( page );
    updateAnimations();

    // trigger a repaint
    m_canvas->update();

    if ( hasAnimation() ) {
        startTimeLine( m_maxShapeDuration );
    }
}

bool KPrAnimationDirector::shapeShown( KoShape * shape )
{
    return !m_animations.contains( shape ) || m_animations[shape].first != 0;
}

QPair<KPrShapeAnimationOld *, KPrAnimationData *> KPrAnimationDirector::shapeAnimation( KoShape * shape )
{
    QMap<KoShape *, QPair<KPrShapeAnimationOld *, KPrAnimationData *> >::iterator it( m_animations.find( shape ) );

    return ( it != m_animations.end() ) ? it.value() : QPair<KPrShapeAnimationOld *, KPrAnimationData *>( 0, 0 );
}


void KPrAnimationDirector::updateActivePage( KoPAPageBase * page )
{
    if ( m_canvas == m_view->kopaCanvas() ) {
        m_view->viewMode()->updateActivePage( page );
    }
    else {
        QList<KoShape*> shapes = page->shapes();
        m_canvas->shapeManager()->setShapes(shapes, KoShapeManager::AddWithoutRepaint);
        //Make the top most layer active
        if ( !shapes.isEmpty() ) {
            KoShapeLayer* layer = dynamic_cast<KoShapeLayer*>( shapes.last() );
            m_canvas->shapeManager()->selection()->setActiveLayer( layer );
        }

        // if the page is not a master page itself set shapes of the master page
        KoPAPage * paPage = dynamic_cast<KoPAPage *>( page );

        Q_ASSERT( paPage );
        KoPAMasterPage * masterPage = paPage->masterPage();
        QList<KoShape*> masterShapes = masterPage->shapes();
        m_canvas->masterShapeManager()->setShapes(masterShapes, KoShapeManager::AddWithoutRepaint);
        // Make the top most layer active
        if ( !masterShapes.isEmpty() ) {
            KoShapeLayer* layer = dynamic_cast<KoShapeLayer*>( masterShapes.last() );
            m_canvas->masterShapeManager()->selection()->setActiveLayer( layer );
        }
    }

    // it can be that the pages have different sizes. So we need to recalulate
    // the zoom when we change the page
    updateZoom( m_canvas->size() );

    KPrPage * kprPage = dynamic_cast<KPrPage *>( page );
    Q_ASSERT( kprPage );
    KPrPageData * pageData = dynamic_cast<KPrPageData*>( kprPage->masterPage() );
    Q_ASSERT( pageData );
    QSet<int> steps = kprPage->animations().steps().toSet();
    steps.unite( pageData->animations().steps().toSet() );
    m_steps = steps.toList();
}

bool KPrAnimationDirector::changePage( Navigation navigation )
{
    switch ( navigation )
    {
        case FirstPage:
            m_pageIndex = 0;
            break;
        case PreviousPage:
            m_pageIndex = m_pageIndex > 0 ? m_pageIndex - 1 : 0;
            break;
        case NextPage:
            if ( m_pageIndex < m_pages.size() -1 ) {
                ++m_pageIndex;
            }
            else {
                return true;
            }
            break;
        case LastPage:
            m_pageIndex = m_pages.size() - 1;
            if ( dynamic_cast<KPrEndOfSlideShowPage *>( m_pages[m_pageIndex] ) && m_pageIndex > 0 ) {
                m_pageIndex--;
            }
            break;
        case PreviousStep:
        case NextStep:
        default:
            // this should not happen
            Q_ASSERT( 0 );
            break;
    }
    m_stepIndex = 0;

    updateActivePage( m_pages[m_pageIndex] );
    updateAnimations();

    // trigger a repaint
    m_canvas->update();

    //TODO start animations on step 0
    if ( hasAnimation() ) {
        startTimeLine( m_maxShapeDuration );
    }

    return false;
}

void KPrAnimationDirector::updateZoom( const QSize & size )
{
    KoPageLayout pageLayout = m_view->activePage()->pageLayout();
    KoPAUtil::setZoom( pageLayout, size, m_zoomHandler );
    m_pageRect = KoPAUtil::pageRect( pageLayout, size, m_zoomHandler );
    m_canvas->setDocumentOffset( -m_pageRect.topLeft() );
}

void KPrAnimationDirector::paintStep( QPainter & painter )
{
    painter.translate( m_pageRect.topLeft() );
    m_view->activePage()->paintBackground( painter, m_zoomHandler );

    if ( m_view->activePage()->displayMasterShapes() ) {
        foreach ( KoShape *shape, m_canvas->masterShapeManager()->shapes() ) {
            shape->waitUntilReady( m_zoomHandler, false );
        }

        m_canvas->masterShapeManager()->paint( painter, m_zoomHandler, true );
    }
    foreach ( KoShape *shape, m_canvas->shapeManager()->shapes() ) {
        shape->waitUntilReady( m_zoomHandler, false );
    }
    m_canvas->shapeManager()->paint( painter, m_zoomHandler, true );
}

bool KPrAnimationDirector::nextStep()
{
    if ( ! m_steps.isEmpty() && m_stepIndex < m_steps.size() - 1 ) {
        // if there are sub steps go to the next substep
        ++m_stepIndex;
        updateAnimations();
        startTimeLine( m_maxShapeDuration );
    }
    else {
        // if there are no more sub steps go to the next page
        // The active page and the substeps are updated later as
        // first the current page has to be painted again for the page effect
        if ( m_pageIndex < m_pages.size() -1 ) {
            ++m_pageIndex;
        }
        else {
            return true;
        }
        m_stepIndex = 0;

        KPrPageEffect * effect = KPrPage::pageData( m_pages[m_pageIndex] )->pageEffect();

        // run page effect if there is one
        if ( effect ) {
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

            KPrPageEffectRunner *oldRunner = m_pageEffectRunner;
            m_pageEffectRunner = new KPrPageEffectRunner( oldPage, newPage, m_canvas, effect );
            kDebug(33001) << "XXXXXXXXXXXXXX new pageEffecrRunner old:" << oldRunner << "new:" << m_pageEffectRunner;
            startTimeLine( effect->duration() );
        }
        else {
            updateActivePage( m_pages[m_pageIndex] );
            updateAnimations();

            m_canvas->update();
            if ( hasAnimation() ) {
                startTimeLine( m_maxShapeDuration );
            }
        }
    }
    return false;
}

void KPrAnimationDirector::previousStep()
{
    if ( m_stepIndex > 0 ) {
        --m_stepIndex;
        // trigger a repaint of the running animations
        finishAnimations();
        updateAnimations();
    }
    else {
        if ( m_pageIndex > 0 ) {
            --m_pageIndex;
            updateActivePage( m_pages[m_pageIndex] );
            Q_ASSERT( m_steps.size() > 0 );
            m_stepIndex = m_steps.size() - 1;
            updateAnimations();
            // trigger repaint
            m_canvas->update();
            // cancel a running page effect
            delete m_pageEffectRunner;
            m_pageEffectRunner = 0;
        }
    }
    // when going back you allway go to the end of the effect
    finishAnimations();
}

void KPrAnimationDirector::updateAnimations()
{
    clearAnimations();
    if ( m_steps.size() > m_stepIndex ) {
        m_maxShapeDuration = 0;

        KPrPage * page = dynamic_cast<KPrPage *>( m_pages[m_pageIndex] );
        KPrPageData * pageData = dynamic_cast<KPrPageData*>( page->masterPage() );
        Q_ASSERT( page );
        Q_ASSERT( pageData );

        m_hasAnimation = false;
        insertAnimations( page, m_canvas->shapeManager() );
        insertAnimations( pageData, m_canvas->masterShapeManager() );
    }
}

void KPrAnimationDirector::insertAnimations( KPrPageData * controller, KoShapeManager * shapeManager )
{
    KoPageLayout pageLayout = m_view->activePage()->pageLayout();
    QRectF pageRect( 0, 0, pageLayout.width, pageLayout.height );

    QMap<KoShape *, KPrShapeAnimationOld *> animations = controller->animations().animations( m_steps[m_stepIndex] );
    QMap<KoShape *, KPrShapeAnimationOld *>::iterator it( animations.begin() );
    for ( ; it != animations.end(); ++it ) {
        KPrShapeAnimationOld * animation = it.value();
        if ( animation ) {
            m_animations.insert( it.key(), qMakePair( it.value(), animation->animationData( m_canvas, shapeManager, pageRect ) ) );
            m_hasAnimation = true;
            if ( animation->duration() > m_maxShapeDuration ) {
                m_maxShapeDuration = animation->duration();
            }
        }
        else {
            m_animations.insert( it.key(), QPair<KPrShapeAnimationOld *, KPrAnimationData *>( 0, 0 ) );
        }
    }
}

void KPrAnimationDirector::clearAnimations()
{
    QMap<KoShape *, QPair<KPrShapeAnimationOld *, KPrAnimationData *> >::iterator it( m_animations.begin() );
    for ( ; it != m_animations.end(); ++it ) {
        delete it.value().second;
    }
    m_animations.clear();
}

bool KPrAnimationDirector::hasAnimation()
{
    return m_hasAnimation;
}

void KPrAnimationDirector::animate()
{
    if ( m_pageEffectRunner ) {
        m_pageEffectRunner->next( m_timeLine.currentTime() );
    }
    else if ( ! m_animations.empty() ) { //if there are animnations
        animateShapes( m_timeLine.currentTime() );
    }
}

void KPrAnimationDirector::finishAnimations()
{
    QMap<KoShape *, QPair<KPrShapeAnimationOld *, KPrAnimationData *> >::iterator it( m_animations.begin() );
    for ( ; it != m_animations.end(); ++it ) {
        KPrShapeAnimationOld * animation = it.value().first;
        if ( animation ) {
            animation->finish( it.value().second );
            it.value().second->m_shapeManager->notifyShapeChanged( it.key() );
        }
    }
}


void KPrAnimationDirector::animateShapes( int currentTime )
{
    QMap<KoShape *, QPair<KPrShapeAnimationOld *, KPrAnimationData *> >::iterator it( m_animations.begin() );
    for ( ; it != m_animations.end(); ++it ) {
        KPrShapeAnimationOld * animation = it.value().first;
        if ( animation ) {
            animation->next( currentTime, it.value().second );
            it.value().second->m_shapeManager->notifyShapeChanged( it.key() );
        }
    }
}

void KPrAnimationDirector::startTimeLine( int duration )
{
    m_timeLine.setDuration( duration );
    m_timeLine.setCurrentTime( 0 );
    m_timeLine.start();
}

#include "KPrAnimationDirector.moc"
