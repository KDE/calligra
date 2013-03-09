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

#include "kis_gesture_shortcut.h"

#include <QGestureEvent>
#include <QGesture>

class KisGestureShortcut::Private
{
public:
    Qt::GestureType gesture;
};

KisGestureShortcut::KisGestureShortcut(KisAbstractInputAction* action, int index)
    : KisAbstractShortcut(action, index), d(new Private)
{

}

KisGestureShortcut::~KisGestureShortcut()
{

}

int KisGestureShortcut::priority() const
{
    return 10 + d->gesture;
}

void KisGestureShortcut::setGesture(Qt::GestureType gesture)
{
    d->gesture = gesture;
}

QGesture* KisGestureShortcut::match(QGestureEvent* event)
{
    foreach(QGesture *gesture, event->activeGestures()) {
        if(gesture->gestureType() == d->gesture) {
            return gesture;
        }
    }

    return 0;
}
