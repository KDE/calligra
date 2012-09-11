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

#include "kis_gesture_action.h"
#include "kis_input_manager.h"
#include <kis_canvas2.h>
#include <kis_view2.h>
#include <KoCanvasController.h>
#include <KoZoomController.h>

#include <QGestureEvent>
#include <QPanGesture>
#include <QPinchGesture>

KisGestureAction::KisGestureAction(KisInputManager* manager): KisAbstractInputAction(manager)
{

}

KisGestureAction::~KisGestureAction()
{

}

void KisGestureAction::begin(int shortcut)
{

}

void KisGestureAction::end()
{

}

void KisGestureAction::inputEvent(QEvent* event)
{
    if(event->type() != QEvent::Gesture)
        return;

    QGestureEvent *gevent = static_cast<QGestureEvent*>(event);

    foreach(QGesture *gesture, gevent->activeGestures()) {
        switch(gesture->gestureType()) {
            case Qt::PanGesture: {
                QPanGesture *pgesture = static_cast<QPanGesture*>(gesture);
                inputManager()->canvas()->canvasController()->pan(-pgesture->delta().toPoint());
                break;
            }
            case Qt::PinchGesture: {
                QPinchGesture *pgesture = static_cast<QPinchGesture*>(gesture);
                qreal diff = pgesture->scaleFactor() - 1.0f;
                if(qAbs(diff) > 0.01f) {
                    float zoom = inputManager()->canvas()->view()->zoomController()->zoomAction()->effectiveZoom() + diff;
                    inputManager()->canvas()->view()->zoomController()->setZoom(KoZoomMode::ZOOM_CONSTANT, zoom);
                    break;
                }
            }
        }
    }
}
