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

#include "BridgeRequestHandler.h"
#include "BridgeServer.h"

#include <QDataStream>
#include <QByteArray>
#include <QLocalSocket>
#include <QDir>

BridgeRequestHandler::BridgeRequestHandler(QLocalSocket *inSocket, QObject *parent) :
    QObject(parent),
    m_inSocket(inSocket),
    m_stream(m_inSocket)
{
    Q_ASSERT(m_inSocket);

    connect(m_inSocket, SIGNAL(disconnected()), m_inSocket, SLOT(deleteLater()));
    connect(m_inSocket, SIGNAL(readyRead()), this, SLOT(handle()));
}

void BridgeRequestHandler::handle()
{
    QString data;

    m_stream >> data;

    QByteArray block;
    QDataStream out(&block, QIODevice::ReadWrite);

    //add return statuses to output stream

    out.device()->seek(0);
    qint64 bytesWritten = m_inSocket->write(block);        //write block to output socket
    m_inSocket->flush();

    if (bytesWritten < 0) {
        qDebug() << "Error while writing to output socket. " << m_outSocket->errorString();
    }
}

BridgeRequestHandler::~BridgeRequestHandler()
{
}
