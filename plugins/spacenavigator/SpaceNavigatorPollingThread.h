/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2008 Hans Bakker <hansmbakker@gmail.com>
 * SPDX-FileCopyrightText: 2008 Jan Hambrecht <jaham@gmx.net>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef SPACENAVIGATORPOLLINGTHREAD_H
#define SPACENAVIGATORPOLLINGTHREAD_H

#include <QThread>

class SpaceNavigatorPollingThread : public QThread
{
    Q_OBJECT
public:
    explicit SpaceNavigatorPollingThread(QObject *parent);
    ~SpaceNavigatorPollingThread();

    /// Stops the thread
    void stop();

Q_SIGNALS:
    void moveEvent(int x, int y, int z, int rx, int ry, int rz, Qt::MouseButtons);
    void buttonEvent(int x, int y, int z, int rx, int ry, int rz, Qt::MouseButtons, Qt::MouseButton, int type);

protected:
    virtual void run() override;

private:
    bool m_stopped;
};

#endif // SPACENAVIGATORPOLLINGTHREAD_H
