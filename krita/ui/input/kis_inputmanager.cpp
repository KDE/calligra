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

#include "kis_inputmanager.h"

#include <QtCore/QDebug>

#include <kis_canvas2.h>
#include <kis_tool_proxy.h>

class KisInputManager::Private
{
public:
    KisCanvas2* canvas;
    KisToolProxy* toolproxy;
};

KisInputManager::KisInputManager(KisCanvas2* canvas)
    : QObject( canvas ), d( new Private )
{
    d->canvas = canvas;
}

bool KisInputManager::eventFilter(QObject* object, QEvent* event)
{
    if(object != d->canvas->canvasWidget())
        return false;

    switch(event->type()) {
        case QEvent::KeyPress:
        case QEvent::KeyRelease:
        case QEvent::MouseButtonPress:
        case QEvent::MouseButtonRelease:
        case QEvent::MouseMove:
        case QEvent::MouseButtonDblClick:
        case QEvent::TabletPress:
        case QEvent::TabletRelease:
        case QEvent::TabletMove:
            qDebug() << event;
            return true;
        default:
            break;
    }

    return false;
}

KisInputManager::~KisInputManager()
{

}

