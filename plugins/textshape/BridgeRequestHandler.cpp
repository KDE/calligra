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

#include "qjson/parser.h"

#include "BridgeRequestHandler.h"
#include "BridgeServer.h"
#include "BridgeActions.h"

#include <QDataStream>
#include <QByteArray>
#include <QLocalSocket>
#include <QVariantMap>
#include <QDir>

BridgeRequestHandler::BridgeRequestHandler(QLocalSocket *inSocket, KoTextEditor *editor, QObject *parent) :
    QObject(parent),
    m_inSocket(inSocket),
    m_stream(m_inSocket),
    m_editor(editor)
{
    Q_ASSERT(m_inSocket);
    Q_ASSERT(m_editor);

    connect(m_inSocket, SIGNAL(disconnected()), m_inSocket, SLOT(deleteLater()));
    connect(m_inSocket, SIGNAL(readyRead()), this, SLOT(handle()));
}

void BridgeRequestHandler::handle()
{
    QString buffer;
    QByteArray bufferData, block;
    QDataStream out(&block, QIODevice::WriteOnly);

    m_stream >> buffer;

    bufferData.append(qPrintable(buffer));
    QVariantMap res = m_parser.parse(bufferData, &m_ok).toMap();

    if (!res.keys().contains(QString("action"))) {         //check for actions
        out << "Invalid message. Specify action parameter";
        return;
    } else if (res["action"].toString() == "insert_citation") {
        qDebug() << "Action insert_citation";
        new InsertCitationBridgeAction(res, m_editor);
    } else if (res["action"].toString() == "insert_bibliography") {
        qDebug() << "Action insert_bibliography";
        new InsertBibliographyBridgeAction(res, m_editor);
    } else if (res["action"].toString() == "insert_cite_record") {
        qDebug() << "Action insert_cite_record";
        new InsertCiteRecordBridgeAction(res);
    }

    out.device()->seek(0);
    qint64 bytesWritten = m_inSocket->write(block);        //write block to output socket
    m_inSocket->flush();

    qDebug() << "Wrote " << bytesWritten;
    if (bytesWritten < 0) {
        qDebug() << "Error while writing to output socket. " << m_inSocket->errorString();
    }
}

BridgeRequestHandler::~BridgeRequestHandler()
{
}
