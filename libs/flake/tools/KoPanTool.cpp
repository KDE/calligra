/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2007 Thomas Zander <zander@kde.org>
 * SPDX-FileCopyrightText: 2007 Jan Hambrecht <jaham@gmx.net>
 * SPDX-FileCopyrightText: 2010 Boudewijn Rempt <boud@kogmbh.com>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "KoPanTool.h"
#include "KoCanvasBase.h"
#include "KoCanvasController.h"
#include "KoCanvasControllerWidget.h"
#include "KoPointerEvent.h"
#include "KoToolBase_p.h"
#include "KoViewConverter.h"

#include <FlakeDebug.h>
#include <QKeyEvent>
#include <QScrollBar>

KoPanTool::KoPanTool(KoCanvasBase *canvas)
    : KoToolBase(canvas)
    , m_controller(nullptr)
    , m_temporary(false)
{
}

bool KoPanTool::wantsAutoScroll() const
{
    return false;
}

void KoPanTool::mousePressEvent(KoPointerEvent *event)
{
    m_lastPosition = documentToViewport(event->point);
    event->accept();
    useCursor(QCursor(Qt::ClosedHandCursor));
}

void KoPanTool::mouseMoveEvent(KoPointerEvent *event)
{
    Q_ASSERT(m_controller);
    if (event->buttons() == 0)
        return;
    event->accept();

    QPointF actualPosition = documentToViewport(event->point);
    QPointF distance(m_lastPosition - actualPosition);
    m_controller->pan(distance.toPoint());

    m_lastPosition = actualPosition;
}

void KoPanTool::mouseReleaseEvent(KoPointerEvent *event)
{
    event->accept();
    useCursor(QCursor(Qt::OpenHandCursor));
    if (m_temporary)
        Q_EMIT done();
}

void KoPanTool::keyPressEvent(QKeyEvent *event)
{
    // XXX: Make widget-independent!
    KoCanvasControllerWidget *canvasControllerWidget = dynamic_cast<KoCanvasControllerWidget *>(m_controller);
    if (!canvasControllerWidget) {
        return;
    }
    switch (event->key()) {
    case Qt::Key_Up:
        m_controller->pan(QPoint(0, -canvasControllerWidget->verticalScrollBar()->singleStep()));
        break;
    case Qt::Key_Down:
        m_controller->pan(QPoint(0, canvasControllerWidget->verticalScrollBar()->singleStep()));
        break;
    case Qt::Key_Left:
        m_controller->pan(QPoint(-canvasControllerWidget->horizontalScrollBar()->singleStep(), 0));
        break;
    case Qt::Key_Right:
        m_controller->pan(QPoint(canvasControllerWidget->horizontalScrollBar()->singleStep(), 0));
        break;
    }
    event->accept();
}

void KoPanTool::activate(ToolActivation toolActivation, const QSet<KoShape *> &)
{
    if (m_controller == nullptr) {
        Q_EMIT done();
        return;
    }
    m_temporary = toolActivation == TemporaryActivation;
    useCursor(QCursor(Qt::OpenHandCursor));
}

void KoPanTool::customMoveEvent(KoPointerEvent *event)
{
    m_controller->pan(QPoint(-event->x(), -event->y()));
    event->accept();
}

QPointF KoPanTool::documentToViewport(const QPointF &p)
{
    Q_D(KoToolBase);
    QPointF viewportPoint = d->canvas->viewConverter()->documentToView(p);
    viewportPoint += d->canvas->documentOrigin();
    viewportPoint += QPoint(m_controller->canvasOffsetX(), m_controller->canvasOffsetY());

    return viewportPoint;
}

void KoPanTool::mouseDoubleClickEvent(KoPointerEvent *event)
{
    mousePressEvent(event);
}
