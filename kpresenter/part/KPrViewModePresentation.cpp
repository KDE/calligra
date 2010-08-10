/* This file is part of the KDE project
 * Copyright (C) 2007-2008 Thorsten Zachmann <zachmann@kde.org>
 * Copyright (C) 2008 Fredy Yanardi <fyanardi@kde.org>
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

#include "KPrViewModePresentation.h"

#include <QEvent>
#include <QKeyEvent>
#include <QPainter>
#include <QtGui/QDesktopWidget>

#include <kdebug.h>
#include <kcursor.h>

#include <KoPointerEvent.h>
#include <KoCanvasController.h>
#include <KoPageApp.h>
#include <KoPACanvas.h>
#include <KoPADocument.h>
#include <KoPAView.h>
#include <KoZoomHandler.h>
#include <KPrView.h>

#include "KPrDocument.h"
#include "KPrPresenterViewWidget.h"
#include "KPrEndOfSlideShowPage.h"

KPrViewModePresentation::KPrViewModePresentation( KoPAViewBase * view, KoPACanvasBase * canvas )
: KoPAViewMode( view, canvas )
, m_savedParent( 0 )
, m_tool( new KPrPresentationTool( *this ) )
, m_animationDirector( 0 )
, m_pvAnimationDirector( 0 )
, m_presenterViewCanvas( 0 )
, m_presenterViewWidget( 0 )
, m_endOfSlideShowPage( 0 )
, m_view( static_cast<KPrView *>(view) )
{
    m_baseCanvas = dynamic_cast<KoPACanvas*>(canvas);
}

KPrViewModePresentation::~KPrViewModePresentation()
{
    delete m_animationDirector;
    delete m_tool;
}

KoViewConverter * KPrViewModePresentation::viewConverter( KoPACanvasBase * canvas )
{
    if ( m_baseCanvas && m_animationDirector && m_baseCanvas == canvas ) {
        return m_animationDirector->viewConverter();
    }
    else if ( m_pvAnimationDirector && m_presenterViewCanvas == canvas ) {
        return m_pvAnimationDirector->viewConverter();
    }
    else {
        return 0;
    }
}

void KPrViewModePresentation::paintEvent( KoPACanvas * canvas,  QPaintEvent* event )
{
    if ( m_baseCanvas && m_baseCanvas == canvas && m_animationDirector ) {
        m_animationDirector->paintEvent( event );
    } else if ( m_presenterViewCanvas == canvas && m_pvAnimationDirector ) {
        m_pvAnimationDirector->paintEvent( event );
    }
}

void KPrViewModePresentation::tabletEvent( QTabletEvent *event, const QPointF &point )
{
    Q_UNUSED(event);
    Q_UNUSED(point);
}

void KPrViewModePresentation::mousePressEvent( QMouseEvent *event, const QPointF &point )
{
    KoPointerEvent ev( event, point );

    m_tool->mousePressEvent( &ev );
}

void KPrViewModePresentation::mouseDoubleClickEvent( QMouseEvent *event, const QPointF &point )
{
    KoPointerEvent ev( event, point );

    m_tool->mouseDoubleClickEvent( &ev );
}

void KPrViewModePresentation::mouseMoveEvent( QMouseEvent *event, const QPointF &point )
{
    KoPointerEvent ev( event, point );

    m_tool->mouseMoveEvent( &ev );
}

void KPrViewModePresentation::mouseReleaseEvent( QMouseEvent *event, const QPointF &point )
{
    KoPointerEvent ev( event, point );

    m_tool->mouseReleaseEvent( &ev );
}

void KPrViewModePresentation::keyPressEvent( QKeyEvent *event )
{
    m_tool->keyPressEvent( event );
}

void KPrViewModePresentation::keyReleaseEvent( QKeyEvent *event )
{
    m_tool->keyReleaseEvent( event );
}

void KPrViewModePresentation::wheelEvent( QWheelEvent * event, const QPointF &point )
{
    KoPointerEvent ev( event, point );

    m_tool->wheelEvent( &ev );
}

void KPrViewModePresentation::closeEvent( QCloseEvent * event )
{
    activateSavedViewMode();
    event->ignore();
}

void KPrViewModePresentation::activate( KoPAViewMode * previousViewMode )
{
    if (!m_baseCanvas) return;

    m_savedViewMode = previousViewMode;               // store the previous view mode
    m_savedParent = m_baseCanvas->parentWidget();
    m_baseCanvas->setParent( ( QWidget* )0, Qt::Window ); // set parent to 0 and

    QDesktopWidget desktop;

    KPrDocument *document = static_cast<KPrDocument *>( m_view->kopaDocument() );
    bool presenterViewEnabled = document->isPresenterViewEnabled();
    int presentationscreen = document->presentationMonitor();

    // add end off slideshow page
    m_endOfSlideShowPage = new KPrEndOfSlideShowPage( desktop.screenGeometry( presentationscreen ), document );
    QList<KoPAPageBase*> pages = document->slideShow();
    pages.append( m_endOfSlideShowPage );

    QRect presentationRect = desktop.screenGeometry( presentationscreen );

    m_baseCanvas->setParent(desktop.screen(presentationscreen), Qt::Window); // detach widget to the presentation screen
    m_baseCanvas->setWindowState( m_baseCanvas->windowState() | Qt::WindowFullScreen ); // make it show full screen

    // the main animation director needs to be created first since it will set the active page
    // of the presentation
    // the animation director needs to be set before m_baseCanvas->move is called as this might try to call
    // viewConverter.
    m_animationDirector = new KPrAnimationDirector( m_view, m_baseCanvas, pages, m_view->activePage() );
    // move and resize now as otherwise it is not set when we call activate on the tool.
    m_baseCanvas->move( presentationRect.topLeft() );
    m_baseCanvas->resize( presentationRect.size() );
    // show and setFocus needs to be done after move and resize as otherwise the move and resize have no effect
    m_baseCanvas->show();
    m_baseCanvas->setFocus();

    KCursor::setAutoHideCursor( m_baseCanvas, true );

    if ( presenterViewEnabled ) {

        if ( desktop.numScreens() > 1 ) {
            int newscreen = desktop.numScreens() - presentationscreen - 1; // What if we have > 2 screens?
            QRect pvRect = desktop.screenGeometry( newscreen );

            m_presenterViewCanvas = new KoPACanvas( m_view, document );
            m_presenterViewWidget = new KPrPresenterViewWidget( this, pages, m_presenterViewCanvas );
            m_presenterViewWidget->setParent( desktop.screen(newscreen), Qt::Window );
            m_presenterViewWidget->setWindowState(
                    m_presenterViewWidget->windowState() | Qt::WindowFullScreen );
            m_presenterViewWidget->move( pvRect.topLeft() );
            m_presenterViewWidget->resize( pvRect.size() );
            m_presenterViewWidget->updateWidget( pvRect.size(), presentationRect.size() );
            m_presenterViewWidget->show();
            m_presenterViewWidget->setFocus();                             // it shown full screen

            m_pvAnimationDirector = new KPrAnimationDirector( m_view,
                    m_presenterViewCanvas, pages, m_view->activePage() );
        }
        else {
            kWarning() << "Presenter View is enabled but only found one monitor";
            document->setPresenterViewEnabled( false );
        }
    }

    m_tool->activate(KoToolBase::DefaultActivation, QSet<KoShape*>());

    emit activated();
    emit pageChanged( m_animationDirector->currentPage(), m_animationDirector->numStepsInPage() );
    emit stepChanged( m_animationDirector->currentStep() );
}

void KPrViewModePresentation::deactivate()
{
    emit deactivated();

    m_animationDirector->deactivate();
    KoPAPageBase * page = m_view->activePage();
    if ( m_endOfSlideShowPage ) {
        if ( page == m_endOfSlideShowPage ) {
            page = m_view->kopaDocument()->pages().last();
        }
    }
    m_tool->deactivate();

    if (!m_baseCanvas) return;

    m_baseCanvas->setParent( m_savedParent, Qt::Widget );
    m_baseCanvas->setFocus();
    m_baseCanvas->setWindowState( m_baseCanvas->windowState() & ~Qt::WindowFullScreen ); // reset
    m_baseCanvas->show();
    KCursor::setAutoHideCursor( m_baseCanvas, false );
    m_baseCanvas->setMouseTracking( true );
    m_view->setActivePage( page );

    // only delete after the new page has been set
    delete m_endOfSlideShowPage;
    m_endOfSlideShowPage = 0;

    delete m_animationDirector;
    m_animationDirector = 0;

    if ( m_presenterViewWidget ) {
        m_presenterViewWidget->setWindowState(
            m_presenterViewWidget->windowState() & ~Qt::WindowFullScreen );
        delete m_pvAnimationDirector;
        m_pvAnimationDirector = 0;

        delete m_presenterViewWidget;
        m_presenterViewWidget = 0;
        m_presenterViewCanvas = 0;
    }
}

void KPrViewModePresentation::updateActivePage( KoPAPageBase *page )
{
    m_view->setActivePage( page );
    if ( m_presenterViewWidget ) {
        if ( 0 != m_animationDirector ) {
            m_presenterViewWidget->setActivePage( m_animationDirector->currentPage() );
        }
        else {
            m_presenterViewWidget->setActivePage( page );
        }
    }
}

void KPrViewModePresentation::activateSavedViewMode()
{
    m_view->setViewMode( m_savedViewMode );
}

KPrAnimationDirector * KPrViewModePresentation::animationDirector()
{
    return m_animationDirector;
}

int KPrViewModePresentation::numPages() const
{
    Q_ASSERT( 0 != m_animationDirector );
    return m_animationDirector ? m_animationDirector->numPages() : -1;
}

int KPrViewModePresentation::currentPage() const
{
    Q_ASSERT( 0 != m_animationDirector );
    return m_animationDirector ? m_animationDirector->currentPage() : -1;
}

int KPrViewModePresentation::numStepsInPage() const
{
    Q_ASSERT( 0 != m_animationDirector );
    return m_animationDirector ? m_animationDirector->numStepsInPage() : -1;
}

int KPrViewModePresentation::currentStep() const
{
    Q_ASSERT( 0 != m_animationDirector );
    return m_animationDirector ? m_animationDirector->currentStep() : -1;
}

KPrPresentationTool * KPrViewModePresentation::presentationTool() const
{
    Q_ASSERT( 0 != m_animationDirector );
    return m_tool;
}

void KPrViewModePresentation::navigate( KPrAnimationDirector::Navigation navigation )
{
    Q_ASSERT( 0 != m_animationDirector );
    if ( 0 == m_animationDirector ) {
      return;
    }
    int previousPage = m_animationDirector->currentPage();
    bool finished = m_animationDirector->navigate( navigation );
    if ( m_pvAnimationDirector ) {
        finished = m_pvAnimationDirector->navigate( navigation ) && finished;
    }

    int newPage = m_animationDirector->currentPage();
    if ( previousPage != newPage ) {
        emit pageChanged( newPage, m_animationDirector->numStepsInPage() );
    }
    emit stepChanged( m_animationDirector->currentStep() );

    if ( finished ) {
        activateSavedViewMode();
    }
}

void KPrViewModePresentation::navigateToPage( int index )
{
    Q_ASSERT( 0 != m_animationDirector );
    if ( 0 == m_animationDirector ) {
      return;
    }
    m_animationDirector->navigateToPage( index );
    if ( m_pvAnimationDirector ) {
        m_pvAnimationDirector->navigateToPage( index );
    }

    emit pageChanged( m_animationDirector->currentPage(), m_animationDirector->numStepsInPage() );
    emit stepChanged( m_animationDirector->currentStep() );
}

bool KPrViewModePresentation::isActivated()
{
    return m_view->isPresentationRunning();
}
