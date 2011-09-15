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

#ifndef COLLABCLIENT_H
#define COLLABCLIENT_H

#include <QObject>
#include <QHostAddress>
#include <QTcpSocket>
#include <QByteArray>

#include "Collaborate.h"

// TODO: Make singleton?

class CollabClient : public Collaborate
{
    Q_OBJECT

public:
    CollabClient(const QString &nick, const QHostAddress &address,
                 quint16 port, QObject* parent);
    ~CollabClient();

    using Collaborate::readyRead;
protected slots:

    virtual void readyRead(int source = -1);
    virtual void disconnected(int source = -1);

    virtual void connected();
    virtual void socketError(QAbstractSocket::SocketError socketError);

protected:
    QHostAddress address;
    quint16 port;

signals:
    void openFile(const QString &filename);

};

#endif // COLLABCLIENT_H
