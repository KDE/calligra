/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2008 Jan Hambrecht <jaham@gmx.net>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "SpaceNavigatorEvent.h"
#include <KoPointerEvent.h>

SpaceNavigatorEvent::SpaceNavigatorEvent(KoInputDeviceHandlerEvent::Type type)
    : KoInputDeviceHandlerEvent(type)
{
}

SpaceNavigatorEvent::~SpaceNavigatorEvent()
{
}

void SpaceNavigatorEvent::setPosition(int x, int y, int z)
{
    m_x = x;
    m_y = y;
    m_z = z;
}

void SpaceNavigatorEvent::setRotation(int rx, int ry, int rz)
{
    m_rx = rx;
    m_ry = ry;
    m_rz = rz;
}

KoPointerEvent *SpaceNavigatorEvent::pointerEvent()
{
    if (!m_event)
        m_event = new KoPointerEvent(this, m_x, m_y, m_z, m_rx, m_ry, m_rz);
    return m_event;
}
