/*
 * Copyright (C) 2007 Igor Stepin <igor_for_os@stepin.name>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Library General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */
#include "ServerSession.h"
#include <QDebug>
#include "Url.h"
#include "ServerConnection.h"
#include "sockets/MessageTcpServer.h"
using namespace kcollaborate;

ServerSession::ServerSession( const Url &url, bool aPublishUsingZeroconf, QObject *parent ):
        Session( url, parent ), server( NULL ),
        publicService( QString(), "_kcollaboration._tcp", url.port() ), publishUsingZeroconf( aPublishUsingZeroconf )
{
    server = new MessageTcpServer( this );
    connect( server, SIGNAL( incomingConnection( MessageTcpSocket * ) ),
             this, SLOT( newConnection( MessageTcpSocket * ) ) );

    setConnected();
}

ServerSession::~ServerSession()
{}

void ServerSession::setConnected()
{
    server->listen( url().hostAddress(), url().port() );
    if ( publishUsingZeroconf ) {
        publicService.publishAsync();
    }
    qDebug() << "[ServerSession::setConnected] listen on " << url().hostAddress().toString() << ":" << url().port();

    emit connected();
}

void ServerSession::setDisconnected()
{
    server->close();
    publicService.stop();
    emit disconnected();
}

void ServerSession::newConnection( MessageTcpSocket *socket )
{
    qDebug() << "[ServerSession::newConnection] fired";
    ServerConnection *serverConnection = new ServerConnection( socket, this );
    serverConnections[serverConnection->id()] = serverConnection;
}

void ServerSession::closeConnection( ServerConnection *serverConnection )
{
    QString key = serverConnections.key( serverConnection );
    if ( !key.isEmpty() ) {
        closeConnectionById( key );
    }
}

void ServerSession::closeConnectionById( const QString &sessionId )
{
    ServerConnection *serverConnection = serverConnections.take( sessionId );
    if ( serverConnection != NULL ) {
        serverConnection->close();
        delete serverConnection;
    }
}
#include "ServerSession.moc"
