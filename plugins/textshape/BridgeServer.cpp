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
#include <QLocalServer>
#include <QLocalSocket>

#ifdef Q_OS_UNIX
const QString BridgeServer::pipeIn = QDir::home().absolutePath().append(QDir::separator()).append(".calligra")
        .append(QDir::separator()).append("pipe.in");
#else
const QString BridgeServer::pipeIn = QDir("\\\\.\\pipe\\pipe.in");
#endif

BridgeServer::BridgeServer(KoTextEditor *editor, QObject *parent) :
    QObject(parent),
    m_server(new QLocalServer(this)),
    m_handles(new QList<BridgeRequestHandler*>()),
    m_editor(editor)
{
    Q_ASSERT(m_editor);
    initServer();
}

void BridgeServer::initServer()
{
    if(!m_server->listen(pipeIn)) {
        qDebug() << "Listen call to local socket failed" << m_server->errorString();
        return;
    } else {
        connect(m_server, SIGNAL(newConnection()), this, SLOT(handleNewEngine()));
    }
}

void BridgeServer::handleNewEngine()
{
    QLocalSocket *socket = m_server->nextPendingConnection();
    qDebug() << "New connection ";
    m_handles->append(new BridgeRequestHandler(socket, m_editor));
}

BridgeServer::~BridgeServer()
{
    m_server->close();
    qDeleteAll(m_handles->begin(), m_handles->end());
}
