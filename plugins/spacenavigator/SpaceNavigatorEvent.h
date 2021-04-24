/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2008 Jan Hambrecht <jaham@gmx.net>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef SPACENAVIGATOREVENT_H
#define SPACENAVIGATOREVENT_H

#include <KoInputDeviceHandlerEvent.h>

class KoPointerEvent;

class SpaceNavigatorEvent : public KoInputDeviceHandlerEvent
{
public:
    explicit SpaceNavigatorEvent(KoInputDeviceHandlerEvent::Type type);
    virtual ~SpaceNavigatorEvent();

    void setPosition( int x, int y, int z );
    void setRotation( int rx, int ry, int rz );

    virtual KoPointerEvent * pointerEvent() override;

private:
    int m_x, m_y, m_z;
    int m_rx, m_ry, m_rz;
};

#endif // SPACENAVIGATOREVENT_H
