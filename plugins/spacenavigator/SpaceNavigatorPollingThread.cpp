/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2008 Hans Bakker <hansmbakker@gmail.com>
 * SPDX-FileCopyrightText: 2008 Jan Hambrecht <jaham@gmx.net>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "SpaceNavigatorPollingThread.h"

#include "SpaceNavigatorDebug.h"

#include <KoInputDeviceHandlerEvent.h>

#include <spnav.h>

SpaceNavigatorPollingThread::SpaceNavigatorPollingThread(QObject *parent)
    : QThread(parent)
    , m_stopped(false)
{
}

SpaceNavigatorPollingThread::~SpaceNavigatorPollingThread()
{
}

void SpaceNavigatorPollingThread::run()
{
    m_stopped = false;
    if (spnav_open() == -1)
        return;

    qreal posfactor = 0.1;
    int currX = 0, currY = 0, currZ = 0;
    int currRX = 0, currRY = 0, currRZ = 0;
    Qt::MouseButtons buttons = Qt::NoButton;

    debugSpaceNavigator << "started spacenavigator polling thread";
    while (!m_stopped) {
        spnav_event event;

        if (spnav_poll_event(&event)) {
            if (event.type == SPNAV_EVENT_MOTION) {
                /*
                 * The coordinate system of the space navigator is like the following:
                 * x-axis : from left to right
                 * y-axis : from down to up
                 * z-axis : from front to back
                 * We probably want to make sure that the x- and y-axis match Qt widget
                 * coordinate system:
                 * x-axis : from left to right
                 * y-axis : from back to front
                 * The z-axis would then go from up to down in a right handed coordinate system.
                 * z-axis : from up to down
                 */
                // debugSpaceNavigator << "got motion event: t("<< event.motion.x << event.motion.y << event.motion.z << ") r(" << event.motion.rx <<
                // event.motion.ry << event.motion.rz << ")";
                currX = static_cast<int>(posfactor * event.motion.x);
                currY = -static_cast<int>(posfactor * event.motion.z);
                currZ = -static_cast<int>(posfactor * event.motion.y);
                currRX = static_cast<int>(posfactor * event.motion.rx);
                currRY = static_cast<int>(-posfactor * event.motion.rz);
                currRZ = static_cast<int>(-posfactor * event.motion.ry);
                Q_EMIT moveEvent(currX, currY, currZ, currRX, currRY, currRZ, buttons);
            } else {
                /* SPNAV_EVENT_BUTTON */
                Qt::MouseButton button = event.button.bnum == 0 ? Qt::LeftButton : Qt::RightButton;
                KoInputDeviceHandlerEvent::Type type;
                if (event.button.press) {
                    // debugSpaceNavigator << "got button press event b(" << event.button.bnum << ")";
                    buttons |= button;
                    type = KoInputDeviceHandlerEvent::ButtonPressed;
                } else {
                    // debugSpaceNavigator << "got button release event b(" << event.button.bnum << ")";
                    buttons &= ~button;
                    type = KoInputDeviceHandlerEvent::ButtonReleased;
                }
                Q_EMIT buttonEvent(currX, currY, currZ, currRX, currRY, currRZ, buttons, button, type);
            }
            spnav_remove_events(event.type);
        }
        msleep(10);
    }

    debugSpaceNavigator << "finished spacenavigator polling thread";
}

void SpaceNavigatorPollingThread::stop()
{
    m_stopped = true;
}
