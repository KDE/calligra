/* This file is part of the KDE project
 * Copyright ( C ) 2007 Thorsten Zachmann <zachmann@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "KoPAViewMode.h"

#include "KoPACanvasBase.h"
#include "KoPAPageBase.h"
#include "KoPAViewBase.h"
#include <KoCanvasController.h>

#include <QCloseEvent>

KoPAViewMode::KoPAViewMode(KoPAViewBase *view, KoPACanvasBase *canvas, const QString &name)
    : m_canvas(canvas)
    , m_toolProxy(canvas->toolProxy())
    , m_view(view)
    , m_name(name)
{
}

KoPAViewMode::~KoPAViewMode() = default;

void KoPAViewMode::closeEvent(QCloseEvent *event)
{
    event->ignore();
}

void KoPAViewMode::setMasterMode(bool master)
{
    Q_UNUSED(master);
}

bool KoPAViewMode::masterMode()
{
    return false;
}

void KoPAViewMode::activate(KoPAViewMode *previousViewMode)
{
    Q_UNUSED(previousViewMode);
    m_canvas->updateSize();
    updateActivePage(m_view->activePage());
    // this is done to set the preferred center
    m_canvas->canvasController()->setCanvasMode(KoCanvasController::Centered);
    m_canvas->canvasController()->recenterPreferred();
}

void KoPAViewMode::deactivate()
{
}

KoPACanvasBase *KoPAViewMode::canvas() const
{
    return m_canvas;
}

KoPAViewBase *KoPAViewMode::view() const
{
    return m_view;
}

KoViewConverter *KoPAViewMode::viewConverter(KoPACanvasBase *canvas)
{
    return m_view->KoPAViewBase::viewConverter(canvas);
}

void KoPAViewMode::updateActivePage(KoPAPageBase *page)
{
    m_view->doUpdateActivePage(page);
}

void KoPAViewMode::addShape(KoShape *shape)
{
    Q_UNUSED(shape);
}

void KoPAViewMode::removeShape(KoShape *shape)
{
    Q_UNUSED(shape);
}

const KoPageLayout &KoPAViewMode::activePageLayout() const
{
    return m_view->activePage()->pageLayout();
}

void KoPAViewMode::changePageLayout(const KoPageLayout &pageLayout, bool applyToDocument, KUndo2Command *parent)
{
    Q_UNUSED(pageLayout)
    Q_UNUSED(applyToDocument)
    Q_UNUSED(parent)
}

QPointF KoPAViewMode::origin()
{
    return m_origin;
}

void KoPAViewMode::setOrigin(const QPointF &o)
{
    m_origin = o;
}

void KoPAViewMode::setName(const QString &name)
{
    m_name = name;
}

QString KoPAViewMode::name() const
{
    return m_name;
}
