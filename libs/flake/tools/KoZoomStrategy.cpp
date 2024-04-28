/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2006-2007 Thomas Zander <zander@kde.org>
 * SPDX-FileCopyrightText: 2007 C. Boemann <cbo@boemann.dk>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "KoZoomStrategy.h"
#include "KoCanvasBase.h"
#include "KoCanvasController.h"
#include "KoShapeRubberSelectStrategy_p.h"
#include "KoViewConverter.h"
#include "KoZoomTool.h"

#include <FlakeDebug.h>

KoZoomStrategy::KoZoomStrategy(KoZoomTool *tool, KoCanvasController *controller, const QPointF &clicked)
    : KoShapeRubberSelectStrategy(tool, clicked, false)
    , m_controller(controller)
    , m_forceZoomOut(false)
{
}

void KoZoomStrategy::finishInteraction(Qt::KeyboardModifiers modifiers)
{
    Q_D(KoShapeRubberSelectStrategy);
    QRect pixelRect = m_controller->canvas()->viewConverter()->documentToView(d->selectedRect()).toRect();
    pixelRect.translate(m_controller->canvas()->documentOrigin());

    bool m_zoomOut = m_forceZoomOut;
    if (modifiers & Qt::ControlModifier) {
        m_zoomOut = !m_zoomOut;
    }
    if (m_zoomOut) {
        m_controller->zoomOut(pixelRect.center());
    } else if (pixelRect.width() > 5 && pixelRect.height() > 5) {
        m_controller->zoomTo(pixelRect);
    } else {
        m_controller->zoomIn(pixelRect.center());
    }
}

void KoZoomStrategy::cancelInteraction()
{
    Q_D(KoShapeRubberSelectStrategy);
    d->tool->repaintDecorations();
    d->tool->canvas()->updateCanvas(d->selectedRect().toRect().normalized());
}

void KoZoomStrategy::forceZoomOut()
{
    m_forceZoomOut = true;
}

void KoZoomStrategy::forceZoomIn()
{
    m_forceZoomOut = false;
}
