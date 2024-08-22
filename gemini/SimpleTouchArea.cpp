/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2012 Dan Leinir Turthra Jensen <admin@leinir.dk>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#include "SimpleTouchArea.h"
#include <QApplication>
#include <QTouchEvent>

SimpleTouchArea::SimpleTouchArea(QQuickItem *parent)
    : QQuickItem(parent)
{
    setAcceptedMouseButtons(Qt::AllButtons);
}

SimpleTouchArea::~SimpleTouchArea() = default;

bool SimpleTouchArea::event(QEvent *event)
{
    switch (static_cast<int>(event->type())) {
        //         case KisTabletEvent::TabletPressEx:
        //         case KisTabletEvent::TabletReleaseEx:
        //         case KisTabletEvent::TabletMoveEx:
        //             event->ignore();
        //             return true;
    default:
        break;
    }
    return QQuickItem::event(event);
}

void SimpleTouchArea::touchEvent(QTouchEvent *event)
{
    switch (event->type()) {
    case QEvent::TouchBegin:
    case QEvent::TouchUpdate:
    case QEvent::TouchEnd:
    case QEvent::TabletPress:
    case QEvent::GraphicsSceneMousePress:
        event->accept();
        return;
    default:
        break;
    }
}
