/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2008 Jan Hambrecht <jaham@gmx.net>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "SpaceNavigatorDevice.h"
#include "SpaceNavigatorDebug.h"
#include "SpaceNavigatorEvent.h"
#include "SpaceNavigatorPollingThread.h"

#include <KoCanvasController.h>
#include <KoToolManager.h>

#include <math.h>
#include <spnav.h>

#define SpaceNavigatorDevice_ID "SpaceNavigator"

SpaceNavigatorDevice::SpaceNavigatorDevice(QObject *parent)
    : KoInputDeviceHandler(parent, SpaceNavigatorDevice_ID)
    , m_thread(new SpaceNavigatorPollingThread(this))
{
    qRegisterMetaType<Qt::MouseButtons>("Qt::MouseButtons");
    qRegisterMetaType<Qt::MouseButton>("Qt::MouseButton");
    connect(m_thread, &SpaceNavigatorPollingThread::moveEvent, this, &SpaceNavigatorDevice::slotMoveEvent);
    connect(m_thread, &SpaceNavigatorPollingThread::buttonEvent, this, &SpaceNavigatorDevice::slotButtonEvent);
}

SpaceNavigatorDevice::~SpaceNavigatorDevice()
{
}

bool SpaceNavigatorDevice::start()
{
    if (m_thread->isRunning())
        return true;

    m_thread->start();

    return true;
}

bool SpaceNavigatorDevice::stop()
{
    if (!m_thread->isRunning())
        return true;

    m_thread->stop();

    if (!m_thread->wait(500))
        m_thread->terminate();

    spnav_close();

    return true;
}

void SpaceNavigatorDevice::slotMoveEvent(int x, int y, int z, int rx, int ry, int rz, Qt::MouseButtons buttons)
{
    SpaceNavigatorEvent e(KoInputDeviceHandlerEvent::PositionChanged);
    e.setPosition(x, y, z);
    e.setRotation(rx, ry, rz);
    e.setButton(Qt::NoButton);
    e.setButtons(buttons);
    KoToolManager::instance()->injectDeviceEvent(&e);

    if (!e.isAccepted()) {
        // no tool wants the event, so do some standard actions
        KoCanvasController *controller = KoToolManager::instance()->activeCanvasController();
        // check if the z-movement is dominant
        if (qAbs(z) > qAbs(x) && qAbs(z) > qAbs(y)) {
            // zoom
            controller->zoomBy(controller->preferredCenter().toPoint(), pow(1.01, -z / 10));
        } else {
            // pan
            controller->pan(QPoint(-x, -y));
        }
    }
}

void SpaceNavigatorDevice::slotButtonEvent(int x, int y, int z, int rx, int ry, int rz, Qt::MouseButtons buttons, Qt::MouseButton button, int type)
{
    SpaceNavigatorEvent e(static_cast<KoInputDeviceHandlerEvent::Type>(type));
    e.setPosition(x, y, z);
    e.setRotation(rx, ry, rz);
    e.setButton(button);
    e.setButtons(buttons);
    KoToolManager::instance()->injectDeviceEvent(&e);
}
