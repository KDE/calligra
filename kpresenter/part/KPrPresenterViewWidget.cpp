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

#include "KPrPresenterViewWidget.h"

#include <QtGui/QBoxLayout>
#include <QtGui/QKeyEvent>
#include <QtGui/QStackedLayout>

#include <KDebug>
#include <KLocale>
#include <KIcon>

#include <KoPageLayout.h>
#include <KoPACanvas.h>
#include <KoPADocument.h>
#include <KoPAPageBase.h>
#include <KoPAPageThumbnailModel.h>
#include <KoPAView.h>
#include <KoPAViewMode.h>
#include <KoShape.h>
#include <KoTextShapeData.h>
#include <KoZoomHandler.h>

#include "KPrAnimationDirector.h"
#include "KPrPresenterViewInterface.h"
#include "KPrViewModePresentation.h"

KPrPresenterViewWidget::KPrPresenterViewWidget( KPrViewModePresentation *viewMode, KoPACanvas *canvas, QWidget *parent )
    : QWidget( parent )
    , m_viewMode( viewMode )
    , m_canvas( canvas )
{
    QVBoxLayout *vLayout = new QVBoxLayout;

    vLayout->setContentsMargins( 20, 20, 20, 0 );

    m_stackedLayout = new QStackedLayout;
    m_mainWidget = new KPrPresenterViewInterface( m_canvas->document(), m_canvas );
    m_stackedLayout->addWidget( m_mainWidget );

    m_slidesWidget = new KPrPresenterViewSlidesInterface( m_canvas->document() );
    m_stackedLayout->addWidget( m_slidesWidget );
    connect( m_slidesWidget, SIGNAL( selectedPageChanged( KoPAPageBase *, bool ) ), this,
            SLOT( requestChangePage( KoPAPageBase *, bool ) ) );
    
    vLayout->addLayout( m_stackedLayout );

    QHBoxLayout *hLayout = new QHBoxLayout;
    hLayout->addStretch();
    m_toolWidget = new KPrPresenterViewToolWidget;
    connect( m_toolWidget, SIGNAL( slideThumbnailsToggled( bool ) ), this, SLOT( showSlideThumbnails( bool ) ) );
    connect( m_toolWidget, SIGNAL( previousSlideClicked() ), this, SLOT( requestPreviousSlide() ) );
    connect( m_toolWidget, SIGNAL( nextSlideClicked() ), this, SLOT( requestNextSlide() ) );
    hLayout->addWidget( m_toolWidget );
    hLayout->addStretch();

    vLayout->addLayout( hLayout );

    setLayout(vLayout);

    m_activeWidget = m_mainWidget;
    KoPAPageBase *activePage = m_viewMode->view()->activePage();
    m_activeWidget->setActivePage( activePage );
}

KPrPresenterViewWidget::~KPrPresenterViewWidget()
{
}

void KPrPresenterViewWidget::setActivePage( KoPAPageBase *page )
{
    m_activeWidget->setActivePage( page );
}

void KPrPresenterViewWidget::updateWidget( const QSize &widgetSize )
{
    int previewHeight = 0.5 * widgetSize.height();

    KoPAPageBase *page = m_viewMode->view()->activePage();

    const KoPageLayout &layout = page->pageLayout();
    KoZoomHandler zoomHandler;
    double ratio = ( zoomHandler.resolutionX() * layout.width ) / ( zoomHandler.resolutionY() * layout.height );
    QSize previewSize( previewHeight * ratio, previewHeight );

    m_mainWidget->setPreviewSize( previewSize );
}

void KPrPresenterViewWidget::showSlideThumbnails( bool show )
{
    if ( show ) {
        m_stackedLayout->setCurrentIndex( 1 );
        m_activeWidget = m_slidesWidget;
    }
    else {
        m_stackedLayout->setCurrentIndex( 0 );
        m_activeWidget = m_mainWidget;
    }
}

void KPrPresenterViewWidget::requestPreviousSlide()
{
    m_viewMode->keyPressEvent( new QKeyEvent( QEvent::KeyPress, Qt::Key_PageUp, Qt::NoModifier ) );
}

void KPrPresenterViewWidget::requestNextSlide()
{
    m_viewMode->keyPressEvent( new QKeyEvent( QEvent::KeyPress, Qt::Key_PageDown, Qt::NoModifier ) );
}

void KPrPresenterViewWidget::requestChangePage( KoPAPageBase *page, bool enableMainView )
{
    if ( enableMainView ) {
        m_toolWidget->toggleSlideThumbnails( false );
    }
    m_viewMode->navigateToPage( page );
    m_mainWidget->setActivePage( page );
    m_slidesWidget->setActivePage( page );
}

#include "KPrPresenterViewWidget.moc"

