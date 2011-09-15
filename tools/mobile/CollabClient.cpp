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

#include <QFile>
#include "CollabClient.h"

CollabClient::CollabClient(const QString &nick, const QHostAddress &address,
                           quint16 port, QObject *parent) :
    Collaborate(nick, parent),
    address(address),
    port(port)
{
    QTcpSocket* socket = new QTcpSocket(this);
    socket->connectToHost(address, port);

    connect(socket, SIGNAL(connected()), this, SLOT(connected()));
    connect(socket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(socketError(QAbstractSocket::SocketError)));
    connect(socket, SIGNAL(readyRead()), this, SLOT(readyRead()));

    qDebug() << "Collaborate-client: created";

    peers.append(new connection(socket));
}

CollabClient::~CollabClient()
{
}

void CollabClient::connected() {
    QDataStream& stream = *(peers.at(0)->stream);

    stream << (uint)CollabInit;
    stream << nick.toUtf8();

    qDebug() << "Collaborate-client: connected";

}

void CollabClient::socketError(QAbstractSocket::SocketError socketError) {
    emit error((SocketError << 7) + socketError);
}

void CollabClient::readyRead(int /*source*/)
{
    QDataStream& stream = *(peers.at(0)->stream);

    uint flag = 0;

    stream >> flag;

    QByteArray data;

    if (flag == CollabInit) {

        stream >> data;

        filename = QString("/home/user/MyDocs/collab" + data);

        stream >> data;

        QFile file(filename);
        if (!file.open(QIODevice::WriteOnly))
            emit error((FileError << 7) + file.error());

        if (file.write(data) != data.length())
            emit error((FileError << 7) + file.error());

        qDebug() << "Collaborate-client: opening file: " << filename;

        file.close();

        emit openFile(filename);

        flag = 0;
    }

    while (peers.at(0)->socket->bytesAvailable() > 0) {
        Collaborate::readyRead(stream, flag, 0);
        flag = 0;
    }
}

void CollabClient::disconnected(int /*source*/) {
    qDebug() << "Disconnected";
    // TODO: create a disconnection signal and emit it
}
