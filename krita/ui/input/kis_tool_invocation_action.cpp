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

#include "kis_tool_invocation_action.h"

#include <KoToolProxy.h>
#include <kis_canvas2.h>
#include <kis_coordinates_converter.h>

#include "kis_input_manager.h"

KisToolInvocationAction::KisToolInvocationAction(KisInputManager *manager)
    : KisAbstractInputAction(manager)
{

}

KisToolInvocationAction::~KisToolInvocationAction()
{
}

void KisToolInvocationAction::begin()
{
    QMouseEvent *pressEvent = new QMouseEvent(QEvent::MouseButtonPress, m_inputManager->mousePosition().toPoint(), Qt::LeftButton, Qt::LeftButton, 0);
    m_inputManager->toolProxy()->mousePressEvent(pressEvent, pressEvent->pos());
}

void KisToolInvocationAction::end()
{
    QMouseEvent *releaseEvent = new QMouseEvent(QEvent::MouseButtonRelease, m_inputManager->mousePosition().toPoint(), Qt::LeftButton, Qt::LeftButton, 0);
    m_inputManager->toolProxy()->mouseReleaseEvent(releaseEvent, releaseEvent->pos());
}

void KisToolInvocationAction::inputEvent(QEvent* event)
{
    if(event->type() == QEvent::MouseMove) {
        QMouseEvent* mevent = static_cast<QMouseEvent*>(event);
        m_inputManager->toolProxy()->mouseMoveEvent(mevent, m_inputManager->canvas()->coordinatesConverter()->widgetToDocument(mevent->posF()));
    }
}
