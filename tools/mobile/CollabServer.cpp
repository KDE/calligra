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

#include "CollabServer.h"

CollabServer::CollabServer(const QString &nick, const QString &filename, quint16 port, QObject *parent)
    : Collaborate(nick, parent)
    , port(port)
    , readySignal(0)
    , disconnectSignal(0)
{
    this->filename = filename;

    readySignal = new QSignalMapper(this);
    connect(readySignal, SIGNAL(mapped(int)), this, SLOT(readyRead(int)));

    disconnectSignal = new QSignalMapper(this);
    connect(disconnectSignal, SIGNAL(mapped(int)), this, SLOT(disconnected(int)));

    connect(&server, SIGNAL(newConnection()), this, SLOT(processConnection()));

    if (!server.listen(QHostAddress::Any, port))
        emit error(server.serverError());

    qDebug() << "Collaborate-server: created server";
}

CollabServer::~CollabServer()
{
    for(int i = 0; i < peers.size(); ++i) {
        delete peers.at(i);
    }
}

void CollabServer::processConnection() {
    QTcpSocket* socket = server.nextPendingConnection();

    //char name[NICKLEN];
    //quint64 n = socket->read(name, NICKLEN);

    qDebug() << "New client from " << socket->peerAddress().toString();

    QObject::connect(socket, SIGNAL(readyRead()), readySignal, SLOT(map()));
    readySignal->setMapping(socket, peers.size());

    QObject::connect(socket, SIGNAL(disconnected()), disconnectSignal, SLOT(map()));
    disconnectSignal->setMapping(socket, peers.size());

    peers.append(new connection(socket));

    //sendFile(peers.back());

}


// TODO: Create abstract virtual Collaborate::initialise() and merge
//        this and CollabClient::readyRead into Collaborate.cpp
void CollabServer::readyRead(int source)
{
    connection* conn = peers.at(source);
    QDataStream& stream = *(peers.at(source)->stream);

    uint flag = 0;
    stream >> flag;

    if (flag == CollabInit) {
        QByteArray nick;
        stream >> nick;
        conn->nick = new QString(nick);
        sendFile(conn);
        qDebug() << "Collaborate-server: new client nick: " << *conn->nick;
        flag = 0;
    }

    while (conn->socket->bytesAvailable() > 0) {
        Collaborate::readyRead(stream, flag, source);
        flag = 0;
    }
}

// TODO: better handle disconnects
void CollabServer::disconnected(int source)
{
    delete peers.at(source);
    peers.replace(source, 0);
}

/*
void CollabServer::sendUserList()
{
    QStringList userList;
    foreach(QString user, users.values())
        userList << user;

    foreach(QTcpSocket *client, clients)
        client->write(QString("/users:" + userList.join(",") + "\n").toUtf8());
}
*/

void CollabServer::sendFile(connection const* peer)
{
    QDataStream& stream = *(peer->stream);

    stream << (uint)CollabInit;

    QByteArray filename = this->filename.toUtf8();
    stream << filename;

    qDebug() << *peer->nick << ":::::::==============================:::::::::" << filename;

    QString tempFilePath("/tmp/" + peer->nick->toUtf8() + filename);
    emit saveFile(tempFilePath);

    QFile tempFile(tempFilePath);
    if(!tempFile.open(QIODevice::ReadOnly)) {
        qDebug()<<"sdfja;gd;ldjnv;ldfn;lvsdnf;vlcould not open file : " <<tempFilePath;
        return;
    }

    QByteArray contents = tempFile.readAll();
    stream.writeBytes(contents.constData(), contents.length());

    tempFile.remove();
}
