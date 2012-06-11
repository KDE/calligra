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

#include "kis_zoom_action.h"

#include <QApplication>

#include <kis_canvas2.h>

#include "kis_input_manager.h"
#include <kis_view2.h>
#include <kis_zoom_manager.h>
#include <KoCanvasController.h>
#include <KoZoomController.h>

class KisZoomAction::Private
{
public:
    QPointF mouseStart;
    QPointF lastMousePosition;
};

KisZoomAction::KisZoomAction(KisInputManager* manager)
    : KisAbstractInputAction(manager), d(new Private)
{

}

KisZoomAction::~KisZoomAction()
{

}

void KisZoomAction::begin()
{
    d->lastMousePosition = d->mouseStart = m_inputManager->canvas()->coordinatesConverter()->documentToWidget(m_inputManager->mousePosition());
    QApplication::setOverrideCursor(Qt::OpenHandCursor);
}

void KisZoomAction::end()
{
    QApplication::restoreOverrideCursor();
}

void KisZoomAction::inputEvent(QEvent* event)
{
    if(event->type() == QEvent::MouseMove) {
        QMouseEvent *mevent = static_cast<QMouseEvent*>(event);
        if(mevent->buttons()) {
            QPointF relMovement = -(mevent->posF() - d->lastMousePosition);

            float zoom = m_inputManager->canvas()->view()->zoomController()->zoomAction()->effectiveZoom() + relMovement.y() / 100;
            m_inputManager->canvas()->view()->zoomController()->setZoom(KoZoomMode::ZOOM_CONSTANT, zoom + relMovement.y() / 100);

            d->lastMousePosition = mevent->posF();
            QApplication::changeOverrideCursor(Qt::ClosedHandCursor);
        } else {
            QApplication::changeOverrideCursor(Qt::OpenHandCursor);
        }
    }
}
