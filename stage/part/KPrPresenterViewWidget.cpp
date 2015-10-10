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

#include <QBoxLayout>
#include <QKeyEvent>
#include <QStackedLayout>

#include <klocalizedstring.h>

#include <KoPACanvas.h>
#include <KoPAPageBase.h>
#include <KoPAViewBase.h>
#include <KoShape.h>

#include "StageDebug.h"
#include "KPrAnimationDirector.h"
#include "KPrPresenterViewInterface.h"
#include "KPrPresenterViewSlidesInterface.h"
#include "KPrPresenterViewToolWidget.h"
#include "KPrViewModePresentation.h"

KPrPresenterViewWidget::KPrPresenterViewWidget( KPrViewModePresentation *viewMode, const QList<KoPAPageBase *> &pages, KoPACanvas *canvas, QWidget *parent )
    : QWidget( parent )
    , m_viewMode( viewMode )
    , m_pages( pages )
    , m_canvas( canvas )
{
    QVBoxLayout *vLayout = new QVBoxLayout;

    vLayout->setContentsMargins( 20, 20, 20, 0 );

    m_stackedLayout = new QStackedLayout;
    m_mainWidget = new KPrPresenterViewInterface( pages, m_canvas );
    m_stackedLayout->addWidget( m_mainWidget );

    m_slidesWidget = new KPrPresenterViewSlidesInterface( pages );
    m_stackedLayout->addWidget( m_slidesWidget );
    connect( m_slidesWidget, SIGNAL(selectedPageChanged(int,bool)), this,
            SLOT(requestChangePage(int,bool)) );
    
    vLayout->addLayout( m_stackedLayout );

    QHBoxLayout *hLayout = new QHBoxLayout;
    hLayout->addStretch();
    m_toolWidget = new KPrPresenterViewToolWidget;
    connect( m_toolWidget, SIGNAL(slideThumbnailsToggled(bool)), this, SLOT(showSlideThumbnails(bool)) );
    connect( m_toolWidget, SIGNAL(previousSlideClicked()), this, SLOT(requestPreviousSlide()) );
    connect( m_toolWidget, SIGNAL(nextSlideClicked()), this, SLOT(requestNextSlide()) );
    hLayout->addWidget( m_toolWidget );
    hLayout->addStretch();

    vLayout->addLayout( hLayout );

    setLayout(vLayout);

    m_activeWidget = m_mainWidget;
    KoPAPageBase *activePage = m_viewMode->view()->activePage();
    if ( !m_pages.contains( activePage ) ) {
        activePage = m_pages[0];
    }
    m_activeWidget->setActivePage( activePage );
}

KPrPresenterViewWidget::~KPrPresenterViewWidget()
{
}

void KPrPresenterViewWidget::setActivePage( KoPAPageBase *page )
{
    m_activeWidget->setActivePage( page );
}

void KPrPresenterViewWidget::setActivePage( int pageIndex )
{
    m_activeWidget->setActivePage( pageIndex );
}

void KPrPresenterViewWidget::updateWidget( const QSize &widgetSize, const QSize &canvasSize )
{
    // a better way to resize the canvas, still need to find optimum value

    // try to make the height 40% of the widget height
    int previewHeight = 0.4 * canvasSize.height();
    double ratio = (double)canvasSize.width() / canvasSize.height();
    int previewWidth = ratio * previewHeight;

    // if it doesn't fit, make the width 40% of the widget width
    if ( previewWidth * 2 > 0.8 * widgetSize.width() ) {
        previewWidth = 0.4 * widgetSize.width();
        previewHeight = previewWidth / ratio;
    }

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

void KPrPresenterViewWidget::requestChangePage( int index, bool enableMainView )
{
    if ( enableMainView ) {
        m_toolWidget->toggleSlideThumbnails( false );
    }
    m_viewMode->navigateToPage( index );
    m_mainWidget->setActivePage( index );
    m_slidesWidget->setActivePage( index );
}
