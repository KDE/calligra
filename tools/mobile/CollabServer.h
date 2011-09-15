/*
 * This file is part of Maemo 5 Office UI for Calligra
 *
 * Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
 *
 * Contact: Arjun Asthana <arjun@iiitd.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
 * 02110-1301 USA
 *
 */

#ifndef COLLABSERVER_H
#define COLLABSERVER_H

#include <QTcpServer>
#include <QTcpSocket>
#include <QTextCursor>
#include <QTextDocument>
#include <QDataStream>
#include <QSignalMapper>
#include <KoDocument.h>

#include "Collaborate.h"
//#include <MainWindow.h>

// TODO: Make singleton?

class CollabServer : public Collaborate
{
    Q_OBJECT

public:
    CollabServer(const QString &nick, const QString &filename, quint16 port, QObject *parent=0);
    ~CollabServer();

    //void sendUserList();
    using Collaborate::readyRead;

protected slots:

    virtual void readyRead(int source = -1);
    virtual void disconnected(int source = -1);
    virtual void processConnection();

protected:
    void sendFile(connection const* peer);

    quint16 port;
    QTcpServer server;

    QSignalMapper* readySignal;
    QSignalMapper* disconnectSignal;

signals:
    void saveFile(const QString &filepath);

};

#endif // COLLABSERVER_H
