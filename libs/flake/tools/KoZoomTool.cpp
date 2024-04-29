/* This file is part of the KDE project
 *
 * SPDX-FileCopyrightText: 2006-2007 Thomas Zander <zander@kde.org>
 * SPDX-FileCopyrightText: 2006 Thorsten Zachmann <zachmann@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "KoZoomTool.h"

#include <QStandardPaths>

#include "KoCanvasBase.h"
#include "KoCanvasController.h"
#include "KoPointerEvent.h"
#include "KoZoomStrategy.h"
#include "KoZoomToolWidget.h"

#include <FlakeDebug.h>

KoZoomTool::KoZoomTool(KoCanvasBase *canvas)
    : KoInteractionTool(canvas)
    , m_temporary(false)
    , m_zoomInMode(true)
{
    QPixmap inPixmap, outPixmap;
    inPixmap.load(QStandardPaths::locate(QStandardPaths::GenericDataLocation, "calligra/cursors/zoom_in_cursor.png"));
    outPixmap.load(QStandardPaths::locate(QStandardPaths::GenericDataLocation, "calligra/cursors/zoom_out_cursor.png"));
    m_inCursor = QCursor(inPixmap, 4, 4);
    m_outCursor = QCursor(outPixmap, 4, 4);
}

void KoZoomTool::wheelEvent(KoPointerEvent *event)
{
    // Let KoCanvasController handle this
    event->ignore();
}

void KoZoomTool::mouseReleaseEvent(KoPointerEvent *event)
{
    KoInteractionTool::mouseReleaseEvent(event);
    if (m_temporary) {
        Q_EMIT KoToolBase::done();
    }
}

void KoZoomTool::mouseMoveEvent(KoPointerEvent *event)
{
    updateCursor(event->modifiers() & Qt::ControlModifier);

    if (currentStrategy()) {
        currentStrategy()->handleMouseMove(event->point, event->modifiers());
    }
}

void KoZoomTool::keyPressEvent(QKeyEvent *event)
{
    event->ignore();

    updateCursor(event->modifiers() & Qt::ControlModifier);
}

void KoZoomTool::keyReleaseEvent(QKeyEvent *event)
{
    event->ignore();

    updateCursor(event->modifiers() & Qt::ControlModifier);

    KoInteractionTool::keyReleaseEvent(event);
}

void KoZoomTool::activate(ToolActivation toolActivation, const QSet<KoShape *> &)
{
    m_temporary = toolActivation == TemporaryActivation;
    updateCursor(false);
}

void KoZoomTool::mouseDoubleClickEvent(KoPointerEvent *event)
{
    mousePressEvent(event);
}

KoInteractionStrategy *KoZoomTool::createStrategy(KoPointerEvent *event)
{
    KoZoomStrategy *zs = new KoZoomStrategy(this, m_controller, event->point);
    if (event->button() == Qt::RightButton) {
        if (m_zoomInMode) {
            zs->forceZoomOut();
        } else {
            zs->forceZoomIn();
        }
    } else {
        if (m_zoomInMode) {
            zs->forceZoomIn();
        } else {
            zs->forceZoomOut();
        }
    }
    return zs;
}

QWidget *KoZoomTool::createOptionWidget()
{
    return new KoZoomToolWidget(this);
}

void KoZoomTool::setZoomInMode(bool zoomIn)
{
    m_zoomInMode = zoomIn;
    updateCursor(false);
}

void KoZoomTool::updateCursor(bool swap)
{
    bool setZoomInCursor = m_zoomInMode;
    if (swap) {
        setZoomInCursor = !setZoomInCursor;
    }

    if (setZoomInCursor) {
        useCursor(m_inCursor);
    } else {
        useCursor(m_outCursor);
    }
}
