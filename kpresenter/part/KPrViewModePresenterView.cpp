/* This file is part of the KDE project
 * Copyright (C) 2008 Fredy Yanardi <fyanardi@gmail.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
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

#include "KPrViewModePresenterView.h"

#include <QtCore/QEvent>
#include <QtGui/QPainter>
#include <QtGui/QLabel>
#include <QtGui/QDesktopWidget>

#include <KDebug>

#include <KoPointerEvent.h>
#include <KoCanvasResourceProvider.h>
#include <KoMainWindow.h>
#include <KoRuler.h>
#include <KoSelection.h>
#include <KoShapeLayer.h>
#include <KoShapeManager.h>
#include <KoText.h>
#include <KoToolManager.h>
#include <KoToolProxy.h>
#include <KoZoomController.h>

#include <KoPACanvas.h>
#include <KoPADocument.h>
#include <KoPAPage.h>
#include <KoPAPageBase.h>
#include <KoPAMasterPage.h>
#include <KoPAView.h>

#include "KPrDocument.h"
#include "KPrPresenterViewTool.h"
#include "KPrPresenterViewWidget.h"

KPrViewModePresenterView::KPrViewModePresenterView( KoPAView *view, KoPACanvas *canvas, KPrViewModePresentation *presentationMode )
    : KPrViewModePresentation( view, canvas )
    , m_presentationMode( presentationMode )
{
    m_savedParent = m_canvas->parentWidget();
    m_canvas->setParent( ( QWidget* )0 ); // detach it from the view
    m_presenterViewWidget = new KPrPresenterViewWidget( this, m_canvas );
    m_presenterViewTool = new KPrPresenterViewTool( *this, m_presentationMode->presentationTool(), m_tool );
    m_presentationMode->setPresenterViewTool( m_presenterViewTool );
    updateActivePage( m_view->activePage() );
}

KPrViewModePresenterView::~KPrViewModePresenterView()
{
    delete m_presenterViewWidget;
    delete m_presenterViewTool;
}

void KPrViewModePresenterView::tabletEvent(QTabletEvent *event, const QPointF &point)
{
}

void KPrViewModePresenterView::mousePressEvent(QMouseEvent *event, const QPointF &point)
{
    KoPointerEvent ev( event, point );
    m_presenterViewTool->mousePressEvent( &ev );
}

void KPrViewModePresenterView::mouseDoubleClickEvent(QMouseEvent *event, const QPointF &point)
{
    KoPointerEvent ev( event, point );
    m_presenterViewTool->mouseDoubleClickEvent( &ev );
}

void KPrViewModePresenterView::mouseMoveEvent(QMouseEvent *event, const QPointF &point)
{
    KoPointerEvent ev( event, point );
    m_presenterViewTool->mouseMoveEvent( &ev );
}

void KPrViewModePresenterView::mouseReleaseEvent(QMouseEvent *event, const QPointF &point)
{
    KoPointerEvent ev( event, point );
    m_presenterViewTool->mouseReleaseEvent( &ev );
}

void KPrViewModePresenterView::keyPressEvent(QKeyEvent *event)
{
    m_presenterViewTool->keyPressEvent( event );
}

void KPrViewModePresenterView::keyReleaseEvent(QKeyEvent *event)
{
    m_presenterViewTool->keyReleaseEvent( event );
}

void KPrViewModePresenterView::wheelEvent(QWheelEvent *event, const QPointF &point)
{
    KoPointerEvent ev( event, point );
    m_presenterViewTool->wheelEvent( &ev );
}

void KPrViewModePresenterView::activate(KoPAViewMode *previousViewMode)
{
    Q_UNUSED( previousViewMode );

    m_presenterViewWidget->setParent( ( QWidget* )0, Qt::Window ); // set parent to 0 and

    QDesktopWidget desktop;

    if ( desktop.numScreens() > 1 ) {
        KPrDocument *document = static_cast<KPrDocument *>( m_canvas->document() );
        int presentationscreen = document->presentationMonitor();
        int newscreen = 0;

        if ( desktop.screenNumber( m_savedParent ) != presentationscreen ) {
            // Try to put the widget on the same screen as the current view
            newscreen = desktop.screenNumber( m_savedParent );
        }
        else {
            newscreen = desktop.numScreens() - presentationscreen - 1; // What if we have > 2 screens?
        }

        QRect rect = desktop.availableGeometry( newscreen );
        m_presenterViewWidget->move( rect.topLeft() );

        m_presenterViewWidget->setWindowState( m_canvas->windowState() | Qt::WindowFullScreen ); // detach widget to make
        m_presenterViewWidget->updateWidget( rect.size() ); 
        m_presenterViewWidget->show();
        m_presenterViewWidget->setFocus();    
        m_animationDirector = new KPrAnimationDirector( m_view, m_view->kopaDocument()->pages(), m_view->activePage() );
    }

}

void KPrViewModePresenterView::deactivate()
{
    m_canvas->setParent( m_savedParent, Qt::Widget );
    m_canvas->setFocus();
    m_canvas->setWindowState( m_canvas->windowState() & ~Qt::WindowFullScreen ); // reset
    m_canvas->show();

    KoMainWindow *shell = m_view->shell();

    Q_ASSERT( shell );

    // close the view used for the presenter view
    // shell->slotCloseAllViews();
    shell->setRootDocument( 0L );
    shell->close();
}

void KPrViewModePresenterView::updateActivePage( KoPAPageBase *page )
{
    KPrViewModePresentation::updateActivePage( page );
    m_presenterViewWidget->setActivePage( page );
}

