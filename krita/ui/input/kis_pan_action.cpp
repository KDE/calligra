/* This file is part of the KDE project
 * Copyright (C) 2012 Arjen Hiemstra <ahiemstra@heimr.nl>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */

#include "kis_pan_action.h"

#include <QDebug>
#include <QMouseEvent>

#include "kis_input_manager.h"
#include <kis_canvas2.h>
#include <KoCanvasController.h>

class KisPanAction::Private
{
public:
    QPointF lastMousePosition;
};

KisPanAction::KisPanAction(KisInputManager *manager)
    : KisAbstractInputAction(manager), d(new Private)
{

}

KisPanAction::~KisPanAction()
{
}

void KisPanAction::begin()
{
    qDebug() << Q_FUNC_INFO;
    d->lastMousePosition = m_inputManager->mousePosition();
}

void KisPanAction::end()
{
    qDebug() << Q_FUNC_INFO;
}

void KisPanAction::inputEvent(QEvent *event)
{
    if(event->type() == QEvent::MouseMove) {
        QMouseEvent *mevent = static_cast<QMouseEvent*>(event);
        if(mevent->buttons()) {
            QPointF relMovement = mevent->posF() - d->lastMousePosition;
            m_inputManager->canvas()->canvasController()->pan(relMovement.toPoint());
            d->lastMousePosition = mevent->posF();
        }
    }
}


