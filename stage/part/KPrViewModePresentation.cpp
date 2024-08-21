/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2007-2008 Thorsten Zachmann <zachmann@kde.org>
 * SPDX-FileCopyrightText: 2008 Fredy Yanardi <fyanardi@kde.org>
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

#include <QApplication>
#include <QEvent>
#include <QKeyEvent>
#include <QPainter>

#include <kcursor.h>

#include <KoCanvasController.h>
#include <KoPACanvas.h>
#include <KoPADocument.h>
#include <KoPAViewBase.h>
#include <KoPageApp.h>
#include <KoPointerEvent.h>
#include <KoZoomHandler.h>

#include "KPrDocument.h"
#include "KPrEndOfSlideShowPage.h"
#include "KPrPresenterViewWidget.h"
#include "StageDebug.h"
#include <KPrView.h>

KPrViewModePresentation::KPrViewModePresentation(KoPAViewBase *view, KoPACanvasBase *canvas)
    : KoPAViewMode(view, canvas)
    , m_savedParent(nullptr)
    , m_tool(new KPrPresentationTool(*this))
    , m_animationDirector(nullptr)
    , m_pvAnimationDirector(nullptr)
    , m_presenterViewCanvas(nullptr)
    , m_presenterViewWidget(nullptr)
    , m_endOfSlideShowPage(nullptr)
    , m_view(static_cast<KPrView *>(view))
{
    // TODO: make this viewmode work with non-QWidget-based canvases as well
    m_baseCanvas = dynamic_cast<KoPACanvas *>(canvas);
}

KPrViewModePresentation::~KPrViewModePresentation()
{
    delete m_animationDirector;
    delete m_tool;
}

KoViewConverter *KPrViewModePresentation::viewConverter(KoPACanvasBase *canvas)
{
    if (m_baseCanvas && m_animationDirector && m_baseCanvas == canvas) {
        return m_animationDirector->viewConverter();
    } else if (m_pvAnimationDirector && m_presenterViewCanvas == canvas) {
        return m_pvAnimationDirector->viewConverter();
    } else {
        // the m_animationDirector is not yet set up fully therefore return the viewConverter of the view
        return m_view->viewConverter();
    }
}

void KPrViewModePresentation::paint(KoPACanvasBase *canvas, QPainter &painter, const QRectF &paintRect)
{
    if (m_baseCanvas && m_baseCanvas == canvas && m_animationDirector) {
        m_animationDirector->paint(painter, paintRect);
    } else if (m_presenterViewCanvas == canvas && m_pvAnimationDirector) {
        m_pvAnimationDirector->paint(painter, paintRect);
    }
}

void KPrViewModePresentation::tabletEvent(QTabletEvent *event, const QPointF &point)
{
    Q_UNUSED(event);
    Q_UNUSED(point);
}

void KPrViewModePresentation::mousePressEvent(QMouseEvent *event, const QPointF &point)
{
    KoPointerEvent ev(event, point);

    m_tool->mousePressEvent(&ev);
}

void KPrViewModePresentation::mouseDoubleClickEvent(QMouseEvent *event, const QPointF &point)
{
    KoPointerEvent ev(event, point);

    m_tool->mouseDoubleClickEvent(&ev);
}

void KPrViewModePresentation::mouseMoveEvent(QMouseEvent *event, const QPointF &point)
{
    KoPointerEvent ev(event, point);

    m_tool->mouseMoveEvent(&ev);
}

void KPrViewModePresentation::mouseReleaseEvent(QMouseEvent *event, const QPointF &point)
{
    KoPointerEvent ev(event, point);

    m_tool->mouseReleaseEvent(&ev);
}

void KPrViewModePresentation::shortcutOverrideEvent(QKeyEvent *event)
{
    m_tool->shortcutOverrideEvent(event);
}

void KPrViewModePresentation::keyPressEvent(QKeyEvent *event)
{
    m_tool->keyPressEvent(event);
}

void KPrViewModePresentation::keyReleaseEvent(QKeyEvent *event)
{
    m_tool->keyReleaseEvent(event);
}

void KPrViewModePresentation::wheelEvent(QWheelEvent *event, const QPointF &point)
{
    KoPointerEvent ev(event, point);

    m_tool->wheelEvent(&ev);
}

void KPrViewModePresentation::closeEvent(QCloseEvent *event)
{
    activateSavedViewMode();
    event->ignore();
}

void KPrViewModePresentation::activate(KoPAViewMode *previousViewMode)
{
    if (!m_baseCanvas)
        return;

    m_savedViewMode = previousViewMode; // store the previous view mode
    m_savedParent = m_baseCanvas->parentWidget();
    m_baseCanvas->setParent((QWidget *)nullptr, Qt::Window); // set parent to 0 and

    KPrDocument *document = static_cast<KPrDocument *>(m_view->kopaDocument());
    bool presenterViewEnabled = document->isPresenterViewEnabled();
    int presentationscreen = document->presentationMonitor();

    // add end off slideshow page
    m_endOfSlideShowPage = new KPrEndOfSlideShowPage(qApp->primaryScreen()->geometry(), document);
    QList<KoPAPageBase *> pages = document->slideShow();
    pages.append(m_endOfSlideShowPage);

    QRect presentationRect = qApp->primaryScreen()->geometry();

    m_baseCanvas->setWindowFlags(Qt::Window); // make it a window

    // the main animation director needs to be created first since it will set the active page
    // of the presentation
    // the animation director needs to be set before m_baseCanvas->move is called as this might try to call
    // viewConverter.
    m_animationDirector = new KPrAnimationDirector(m_view, m_baseCanvas, pages, m_view->activePage());
    // move and resize now as otherwise it is not set when we call activate on the tool.
    m_baseCanvas->setGeometry(presentationRect);
    m_baseCanvas->setWindowState(m_baseCanvas->windowState() | Qt::WindowFullScreen); // make it show full screen

    // show and setFocus needs to be done after move and resize as otherwise the move and resize have no effect
    m_baseCanvas->show();
    m_baseCanvas->setFocus();

    KCursor::setAutoHideCursor(m_baseCanvas, true);

    if (presenterViewEnabled) {
        if (qApp->screens().length() > 1) {
            int newscreen = qApp->screens().length() - presentationscreen - 1; // What if we have > 2 screens?
            QRect pvRect = qApp->screens().at(newscreen)->geometry();

            m_presenterViewCanvas = new KoPACanvas(m_view, document);
            m_presenterViewWidget = new KPrPresenterViewWidget(this, pages, m_presenterViewCanvas);
            m_presenterViewWidget->setGeometry(pvRect);
            m_presenterViewWidget->setWindowState(m_presenterViewWidget->windowState() | Qt::WindowFullScreen);
            m_presenterViewWidget->updateWidget(pvRect.size(), presentationRect.size());
            m_presenterViewWidget->show();
            m_presenterViewWidget->setFocus(); // it shown full screen

            m_pvAnimationDirector = new KPrAnimationDirector(m_view, m_presenterViewCanvas, pages, m_view->activePage());
        } else {
            warnStage << "Presenter View is enabled but only found one monitor";
            document->setPresenterViewEnabled(false);
        }
    }

    m_tool->activate(KoToolBase::DefaultActivation, QSet<KoShape *>());

    Q_EMIT activated();
    Q_EMIT pageChanged(m_animationDirector->currentPage(), m_animationDirector->numStepsInPage());
    Q_EMIT stepChanged(m_animationDirector->currentStep());
}

void KPrViewModePresentation::deactivate()
{
    Q_EMIT deactivated();

    m_animationDirector->deactivate();
    KoPAPageBase *page = m_view->activePage();
    if (m_endOfSlideShowPage) {
        if (page == m_endOfSlideShowPage) {
            KPrDocument *document = static_cast<KPrDocument *>(m_view->kopaDocument());
            if (document->slideShow().size() > 0) {
                page = document->slideShow().last();
            } else {
                page = document->pages().first();
            }
        }
    }
    m_tool->deactivate();

    if (!m_baseCanvas)
        return;

    m_baseCanvas->setParent(m_savedParent, Qt::Widget);
    m_baseCanvas->setFocus();
    m_baseCanvas->setWindowState(m_baseCanvas->windowState() & ~Qt::WindowFullScreen); // reset
    m_baseCanvas->show();
    KCursor::setAutoHideCursor(m_baseCanvas, false);
    m_baseCanvas->setMouseTracking(true);
    m_view->setActivePage(page);

    // only delete after the new page has been set
    delete m_endOfSlideShowPage;
    m_endOfSlideShowPage = nullptr;

    delete m_animationDirector;
    m_animationDirector = nullptr;

    if (m_presenterViewWidget) {
        m_presenterViewWidget->setWindowState(m_presenterViewWidget->windowState() & ~Qt::WindowFullScreen);
        delete m_pvAnimationDirector;
        m_pvAnimationDirector = nullptr;

        delete m_presenterViewWidget;
        m_presenterViewWidget = nullptr;
        m_presenterViewCanvas = nullptr;
    }
    // make sure the page does not have an offset after finishing a presentation
    m_baseCanvas->setDocumentOffset(QPoint(0, 0));
}

void KPrViewModePresentation::updateActivePage(KoPAPageBase *page)
{
    m_view->setActivePage(page);
    if (m_presenterViewWidget) {
        if (nullptr != m_animationDirector) {
            m_presenterViewWidget->setActivePage(m_animationDirector->currentPage());
        } else {
            m_presenterViewWidget->setActivePage(page);
        }
    }
}

void KPrViewModePresentation::activateSavedViewMode()
{
    m_view->setViewMode(m_savedViewMode);
}

KPrAnimationDirector *KPrViewModePresentation::animationDirector()
{
    return m_animationDirector;
}

int KPrViewModePresentation::numPages() const
{
    Q_ASSERT(nullptr != m_animationDirector);
    return m_animationDirector ? m_animationDirector->numPages() : -1;
}

int KPrViewModePresentation::currentPage() const
{
    Q_ASSERT(nullptr != m_animationDirector);
    return m_animationDirector ? m_animationDirector->currentPage() : -1;
}

int KPrViewModePresentation::numStepsInPage() const
{
    Q_ASSERT(nullptr != m_animationDirector);
    return m_animationDirector ? m_animationDirector->numStepsInPage() : -1;
}

int KPrViewModePresentation::currentStep() const
{
    Q_ASSERT(nullptr != m_animationDirector);
    return m_animationDirector ? m_animationDirector->currentStep() : -1;
}

KPrPresentationTool *KPrViewModePresentation::presentationTool() const
{
    Q_ASSERT(nullptr != m_animationDirector);
    return m_tool;
}

void KPrViewModePresentation::navigate(KPrAnimationDirector::Navigation navigation)
{
    Q_ASSERT(nullptr != m_animationDirector);
    if (nullptr == m_animationDirector) {
        return;
    }
    int previousPage = m_animationDirector->currentPage();
    bool finished = m_animationDirector->navigate(navigation);
    if (m_pvAnimationDirector) {
        finished = m_pvAnimationDirector->navigate(navigation) && finished;
    }

    int newPage = m_animationDirector->currentPage();
    if (previousPage != newPage) {
        Q_EMIT pageChanged(newPage, m_animationDirector->numStepsInPage());
    }
    Q_EMIT stepChanged(m_animationDirector->currentStep());

    if (finished) {
        activateSavedViewMode();
    }
}

void KPrViewModePresentation::navigateToPage(int index)
{
    Q_ASSERT(nullptr != m_animationDirector);
    if (nullptr == m_animationDirector) {
        return;
    }
    m_animationDirector->navigateToPage(index);
    if (m_pvAnimationDirector) {
        m_pvAnimationDirector->navigateToPage(index);
    }

    Q_EMIT pageChanged(m_animationDirector->currentPage(), m_animationDirector->numStepsInPage());
    Q_EMIT stepChanged(m_animationDirector->currentStep());
}

bool KPrViewModePresentation::isActivated()
{
    return m_view->isPresentationRunning();
}
