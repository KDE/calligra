/* This file is part of the KDE project

   SPDX-FileCopyrightText: 2006-2007, 2010 Thomas Zander <zander@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "KoInteractionTool.h"
#include "KoCanvasBase.h"
#include "KoInteractionTool_p.h"
#include "KoPanTool.h"
#include "KoPointerEvent.h"
#include "KoToolBase_p.h"

KoInteractionTool::KoInteractionTool(KoCanvasBase *canvas)
    : KoToolBase(*(new KoInteractionToolPrivate(this, canvas)))
{
}

KoInteractionTool::~KoInteractionTool() = default;

void KoInteractionTool::paint(QPainter &painter, const KoViewConverter &converter)
{
    Q_D(KoInteractionTool);
    if (d->currentStrategy)
        d->currentStrategy->paint(painter, converter);
}

void KoInteractionTool::mousePressEvent(KoPointerEvent *event)
{
    Q_D(KoInteractionTool);
    if (d->currentStrategy) { // possible if the user presses an extra mouse button
        cancelCurrentStrategy();
        return;
    }
    d->currentStrategy = createStrategy(event);
    if (d->currentStrategy == nullptr)
        event->ignore();
}

void KoInteractionTool::mouseMoveEvent(KoPointerEvent *event)
{
    Q_D(KoInteractionTool);
    d->lastPoint = event->point;
    if (d->currentStrategy)
        d->currentStrategy->handleMouseMove(d->lastPoint, event->modifiers());
    else
        event->ignore();
}

void KoInteractionTool::mouseReleaseEvent(KoPointerEvent *event)
{
    Q_D(KoInteractionTool);
    if (d->currentStrategy) {
        d->currentStrategy->finishInteraction(event->modifiers());
        KUndo2Command *command = d->currentStrategy->createCommand();
        if (command)
            d->canvas->addCommand(command);
        delete d->currentStrategy;
        d->currentStrategy = nullptr;
        repaintDecorations();
    } else
        event->ignore();
}

void KoInteractionTool::keyPressEvent(QKeyEvent *event)
{
    Q_D(KoInteractionTool);
    event->ignore();
    if (d->currentStrategy
        && (event->key() == Qt::Key_Control || event->key() == Qt::Key_Alt || event->key() == Qt::Key_Shift || event->key() == Qt::Key_Meta)) {
        d->currentStrategy->handleMouseMove(d->lastPoint, event->modifiers());
        event->accept();
    }
}

void KoInteractionTool::keyReleaseEvent(QKeyEvent *event)
{
    Q_D(KoInteractionTool);
    if (d->currentStrategy == nullptr) { // catch all cases where no current strategy is needed
        if (event->key() == Qt::Key_Space)
            Q_EMIT activateTemporary(KoPanTool_ID);
    } else if (event->key() == Qt::Key_Escape) {
        cancelCurrentStrategy();
        event->accept();
    } else if (event->key() == Qt::Key_Control || event->key() == Qt::Key_Alt || event->key() == Qt::Key_Shift || event->key() == Qt::Key_Meta) {
        d->currentStrategy->handleMouseMove(d->lastPoint, event->modifiers());
    }
}

KoInteractionStrategy *KoInteractionTool::currentStrategy()
{
    Q_D(KoInteractionTool);
    return d->currentStrategy;
}

void KoInteractionTool::cancelCurrentStrategy()
{
    Q_D(KoInteractionTool);
    if (d->currentStrategy) {
        d->currentStrategy->cancelInteraction();
        delete d->currentStrategy;
        d->currentStrategy = nullptr;
    }
}

KoInteractionTool::KoInteractionTool(KoInteractionToolPrivate &dd)
    : KoToolBase(dd)
{
}
