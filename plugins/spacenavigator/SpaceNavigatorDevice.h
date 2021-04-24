/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2008 Jan Hambrecht <jaham@gmx.net>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef SPACENAVIGATORDEVICE_H
#define SPACENAVIGATORDEVICE_H

#include <KoInputDeviceHandler.h>

class SpaceNavigatorPollingThread;

class SpaceNavigatorDevice : public KoInputDeviceHandler
{
    Q_OBJECT
public:
    explicit SpaceNavigatorDevice(QObject *parent);
    virtual ~SpaceNavigatorDevice();

    virtual bool start() override;
    virtual bool stop() override;
private Q_SLOTS:
    void slotMoveEvent( int x, int y, int z, int rx, int ry, int rz, Qt::MouseButtons );
    void slotButtonEvent( int x, int y, int z, int rx, int ry, int rz, Qt::MouseButtons, Qt::MouseButton, int type );

private:
    SpaceNavigatorPollingThread * m_thread;
};

#endif // SPACENAVIGATORDEVICE_H
