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

#include <QDebug>

KisToolInvocationAction::KisToolInvocationAction(KisCanvas2* canvas, KoToolProxy* tool)
    : KisAbstractInputAction(canvas, tool)
{

}

KisToolInvocationAction::~KisToolInvocationAction()
{
}

void KisToolInvocationAction::begin()
{
    qDebug() << Q_FUNC_INFO;
    QMouseEvent *pressEvent = new QMouseEvent(QEvent::MouseButtonPress, QCursor::pos(), Qt::LeftButton, Qt::LeftButton, 0);
    toolProxy()->mousePressEvent(pressEvent, pressEvent->pos());
}

void KisToolInvocationAction::end()
{
    qDebug() << Q_FUNC_INFO;
    QMouseEvent *releaseEvent = new QMouseEvent(QEvent::MouseButtonRelease, QCursor::pos(), Qt::LeftButton, Qt::LeftButton, 0);
    toolProxy()->mouseReleaseEvent(releaseEvent, releaseEvent->pos());
}

void KisToolInvocationAction::inputEvent(QEvent* event)
{
    if( event->type() == QEvent::MouseMove ) {
        QMouseEvent* evt = static_cast<QMouseEvent*>(event);
        toolProxy()->mouseMoveEvent(evt, evt->pos());
    }
}
