/* This file is part of the KDE project
 * Copyright (C) 2012 Smit Patel <smitpatel24@gmail.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#include "BridgeServer.h"
#include "BridgeRequestHandler.h"

#include <QMessageBox>
#include <QDir>
#include <QList>
#include <QDataStream>
#include <QMessageBox>
#include <QtNetwork/QLocalServer>
#include <QtNetwork/QLocalSocket>
#include <QtNetwork>

#ifdef Q_OS_UNIX
const QDir BridgeServer::socketDir = QDir(QDir::home().absolutePath().append(QDir::separator()).append(".calligra"));
#else
const QDir BridgeServer::socketDir = QDir("\\\\.\\pipe\\");
#endif

BridgeServer::BridgeServer(QObject *parent) :
    QObject(parent),
    m_inSocket(new QLocalServer(this)),
    m_handles(new QList<BridgeRequestHandler*>())
{
    initServer();
}

void BridgeServer::initServer()
{
    if(!m_inSocket->listen(socketDir.absolutePath().append(QDir::separator()).append("pipe.in"))) {
        QMessageBox::warning(0, "Error while listening on local server", m_inSocket->errorString());
        return;
    } else {
        connect(m_inSocket, SIGNAL(newConnection()), this, SLOT(handleNewEngine()));
    }
}

void BridgeServer::handleNewEngine()
{
    QLocalSocket *socket = m_inSocket->nextPendingConnection();
    m_handles->append(new BridgeRequestHandler(socket));
}
