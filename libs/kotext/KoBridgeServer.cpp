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

#include "KoBridgeServer.h"
#include "KoBridgeActions.h"
#include "BibliographyDb.h"

#include <QMessageBox>
#include <QDir>
#include <QList>
#include <QDataStream>
#include <QMessageBox>
#include <QLocalServer>
#include <QLocalSocket>
#include <QSignalMapper>

#ifdef Q_OS_UNIX
const QString KoBridgeServer::pipeIn = QDir::home().absolutePath().append(QDir::separator()).append(".calligra")
        .append(QDir::separator()).append("pipe.in");
#else
const QString KoBridgeServer::pipeIn = QDir("\\\\.\\pipe\\pipe.in");
#endif

KoBridgeServer::KoBridgeServer(KoTextEditor *editor, QObject *parent) :
    QObject(parent),
    m_server(new QLocalServer(this)),
    m_editor(editor),
    m_mapper(new QSignalMapper(this))
{
    Q_ASSERT(m_editor);
    initServer();

    connect(m_mapper, SIGNAL(mapped(QObject*)), this, SLOT(handle(QObject*)));
}

void KoBridgeServer::initServer()
{
    if(!m_server->listen(pipeIn)) {
        qDebug() << "Listen call to local socket failed" << m_server->errorString();
        return;
    } else {
        connect(m_server, SIGNAL(newConnection()), this, SLOT(handleNewEngine()));
    }
}

void KoBridgeServer::handleNewEngine()
{
    QLocalSocket *socket = m_server->nextPendingConnection();
    connect(socket, SIGNAL(disconnected()), socket, SLOT(deleteLater()));
    connect(socket, SIGNAL(readyRead()), m_mapper, SLOT(map()));
    qDebug() << "New engine request";
    m_mapper->setMapping(socket, socket);

    connect(m_mapper, SIGNAL(mapped(QObject*)), this, SLOT(handle(QObject*)));
}

void KoBridgeServer::handle(QObject *o)
{
    QLocalSocket *socket = static_cast<QLocalSocket*>(o);

    QDataStream in(socket);
    QString buffer;
    QByteArray bufferData, block;
    QDataStream out(&block, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_0);

    in >> buffer;

    bufferData.append(qPrintable(buffer));
    QVariantMap res = m_parser.parse(bufferData, &m_ok).toMap();

    if (res["action"].toString().isEmpty()) {         //check for actions
        return;
    } else if (res["action"].toString() == "insert_citation") {
        out << "Action insert_citation";
        new InsertCitationBridgeAction(res, m_editor);
    } else if (res["action"].toString() == "insert_bibliography") {
        out << "Action insert_bibliography";
        new InsertBibliographyBridgeAction(res, m_editor);
    } else if (res["action"].toString() == "insert_cite_record") {
        out << "Action insert_cite_record";
        QFileInfo biblioFile(BibliographyDb::tableDir.absolutePath().append(QDir::separator()).append("biblio.kexi"));
        BibliographyDb *biblioDb = new BibliographyDb(this, biblioFile.dir().absolutePath(), biblioFile.fileName());

        new InsertCiteRecordBridgeAction(res, biblioDb);
    }

    out.device()->seek(0);
    qint64 bytesWritten = socket->write(block);        //write block to output socket
    socket->flush();

    qDebug() << "Wrote " << bytesWritten;
    if (bytesWritten < 0) {
        qDebug() << "Error while writing to output socket. " << socket->errorString();
    }
}

KoBridgeServer::~KoBridgeServer()
{
    m_server->close();
}
