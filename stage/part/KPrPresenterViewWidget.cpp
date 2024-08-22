/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2008 Fredy Yanardi <fyanardi@gmail.com>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "KPrPresenterViewWidget.h"

#include <QBoxLayout>
#include <QKeyEvent>
#include <QStackedLayout>

#include <KLocalizedString>

#include <KoPACanvas.h>
#include <KoPAPageBase.h>
#include <KoPAViewBase.h>
#include <KoShape.h>

#include "KPrAnimationDirector.h"
#include "KPrPresenterViewInterface.h"
#include "KPrPresenterViewSlidesInterface.h"
#include "KPrPresenterViewToolWidget.h"
#include "KPrViewModePresentation.h"
#include "StageDebug.h"

KPrPresenterViewWidget::KPrPresenterViewWidget(KPrViewModePresentation *viewMode, const QList<KoPAPageBase *> &pages, KoPACanvas *canvas, QWidget *parent)
    : QWidget(parent)
    , m_viewMode(viewMode)
    , m_pages(pages)
    , m_canvas(canvas)
{
    QVBoxLayout *vLayout = new QVBoxLayout;

    vLayout->setContentsMargins(20, 20, 20, 0);

    m_stackedLayout = new QStackedLayout;
    m_mainWidget = new KPrPresenterViewInterface(pages, m_canvas);
    m_stackedLayout->addWidget(m_mainWidget);

    m_slidesWidget = new KPrPresenterViewSlidesInterface(pages);
    m_stackedLayout->addWidget(m_slidesWidget);
    connect(m_slidesWidget, &KPrPresenterViewSlidesInterface::selectedPageChanged, this, &KPrPresenterViewWidget::requestChangePage);

    vLayout->addLayout(m_stackedLayout);

    QHBoxLayout *hLayout = new QHBoxLayout;
    hLayout->addStretch();
    m_toolWidget = new KPrPresenterViewToolWidget;
    connect(m_toolWidget, &KPrPresenterViewToolWidget::slideThumbnailsToggled, this, &KPrPresenterViewWidget::showSlideThumbnails);
    connect(m_toolWidget, &KPrPresenterViewToolWidget::previousSlideClicked, this, &KPrPresenterViewWidget::requestPreviousSlide);
    connect(m_toolWidget, &KPrPresenterViewToolWidget::nextSlideClicked, this, &KPrPresenterViewWidget::requestNextSlide);
    hLayout->addWidget(m_toolWidget);
    hLayout->addStretch();

    vLayout->addLayout(hLayout);

    setLayout(vLayout);

    m_activeWidget = m_mainWidget;
    KoPAPageBase *activePage = m_viewMode->view()->activePage();
    if (!m_pages.contains(activePage)) {
        activePage = m_pages[0];
    }
    m_activeWidget->setActivePage(activePage);
}

KPrPresenterViewWidget::~KPrPresenterViewWidget() = default;

void KPrPresenterViewWidget::setActivePage(KoPAPageBase *page)
{
    m_activeWidget->setActivePage(page);
}

void KPrPresenterViewWidget::setActivePage(int pageIndex)
{
    m_activeWidget->setActivePage(pageIndex);
}

void KPrPresenterViewWidget::updateWidget(const QSize &widgetSize, const QSize &canvasSize)
{
    // a better way to resize the canvas, still need to find optimum value

    // try to make the height 40% of the widget height
    int previewHeight = 0.4 * canvasSize.height();
    double ratio = (double)canvasSize.width() / canvasSize.height();
    int previewWidth = ratio * previewHeight;

    // if it doesn't fit, make the width 40% of the widget width
    if (previewWidth * 2 > 0.8 * widgetSize.width()) {
        previewWidth = 0.4 * widgetSize.width();
        previewHeight = previewWidth / ratio;
    }

    QSize previewSize(previewHeight * ratio, previewHeight);

    m_mainWidget->setPreviewSize(previewSize);
}

void KPrPresenterViewWidget::showSlideThumbnails(bool show)
{
    if (show) {
        m_stackedLayout->setCurrentIndex(1);
        m_activeWidget = m_slidesWidget;
    } else {
        m_stackedLayout->setCurrentIndex(0);
        m_activeWidget = m_mainWidget;
    }
}

void KPrPresenterViewWidget::requestPreviousSlide()
{
    m_viewMode->keyPressEvent(new QKeyEvent(QEvent::KeyPress, Qt::Key_PageUp, Qt::NoModifier));
}

void KPrPresenterViewWidget::requestNextSlide()
{
    m_viewMode->keyPressEvent(new QKeyEvent(QEvent::KeyPress, Qt::Key_PageDown, Qt::NoModifier));
}

void KPrPresenterViewWidget::requestChangePage(int index, bool enableMainView)
{
    if (enableMainView) {
        m_toolWidget->toggleSlideThumbnails(false);
    }
    m_viewMode->navigateToPage(index);
    m_mainWidget->setActivePage(index);
    m_slidesWidget->setActivePage(index);
}
