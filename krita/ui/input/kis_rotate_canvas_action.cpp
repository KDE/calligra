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

#include "kis_rotate_canvas_action.h"

#include <QDebug>
#include <QApplication>

#include <Eigen/Geometry>

#include "kis_canvas2.h"
#include "kis_input_manager.h"

using namespace Eigen;

class KisRotateCanvasAction::Private
{
public:
    QPointF lastMousePosition;
};

KisRotateCanvasAction::KisRotateCanvasAction(KisInputManager* manager)
    : KisAbstractInputAction(manager), d(new Private)
{
}

KisRotateCanvasAction::~KisRotateCanvasAction()
{

}

void KisRotateCanvasAction::begin()
{
    qDebug() << Q_FUNC_INFO;

    d->lastMousePosition = m_inputManager->canvas()->coordinatesConverter()->documentToWidget(m_inputManager->mousePosition());
    QApplication::setOverrideCursor(Qt::OpenHandCursor);
}

void KisRotateCanvasAction::end()
{
    QApplication::restoreOverrideCursor();
}

void KisRotateCanvasAction::inputEvent(QEvent* event)
{
    if(event->type() == QEvent::MouseMove) {
        QMouseEvent *mevent = static_cast<QMouseEvent*>(event);
        if(mevent->buttons()) {
            QPointF relMovement = mevent->posF() - d->lastMousePosition;
            float angle = relMovement.manhattanLength();

            Vector2f dir = Vector2f(relMovement.x(), relMovement.y()).normalized();
            if(qAbs(dir.x()) > qAbs(dir.y())) {
                angle *= dir.x() / qAbs(dir.x());
            } else {
                angle *= dir.y() / qAbs(dir.y());
            }

            m_inputManager->canvas()->rotateCanvas(angle);

            d->lastMousePosition = mevent->posF();
            QApplication::changeOverrideCursor(Qt::ClosedHandCursor);
        } else {
            QApplication::changeOverrideCursor(Qt::OpenHandCursor);
        }
    }
}