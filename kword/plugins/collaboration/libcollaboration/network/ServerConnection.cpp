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
#include "ServerConnection.h"
#include "ServerSession.h"
#include <QUuid>
#include <QDebug>
#include "sockets/MessageTcpSocket.h"
#include "messages/MessageFactory.h"
using namespace kcollaborate;

ServerConnection::ServerConnection( MessageTcpSocket *aSocket, ServerSession *parent ):
        QObject( parent ), socket( aSocket )
{
    Q_ASSERT( parent );
    id_ = QUuid::createUuid().toString();

    messageFactory = new Message::MessageFactory( this );
    connect( messageFactory, SIGNAL( messageReceivedHello( const Message::Hello & ) ),
             this, SLOT( messageReceivedHello( const Message::Hello & ) ) );
    connect( messageFactory, SIGNAL( messageReceivedHelloAnswer( const Message::HelloAnswer & ) ),
             this, SLOT( messageReceivedHelloAnswer( const Message::HelloAnswer & ) ) );
    connect( messageFactory, SIGNAL( messageReceivedUpdate( const Message::Update & ) ),
             this, SLOT( messageReceivedUpdate( const Message::Update & ) ) );
    connect( messageFactory, SIGNAL( messageReceivedUpdateAnswer( const Message::UpdateAnswer & ) ),
             this, SLOT( messageReceivedUpdateAnswer( const Message::UpdateAnswer & ) ) );
    connect( messageFactory, SIGNAL( messageReceivedSessionClosed( const QString & ) ),
             this, SLOT( messageReceivedSessionClosed( const QString & ) ) );

    connect( socket, SIGNAL( msgReceived( const QString & ) ),
             messageFactory, SLOT( processRawMessage( const QString & ) ) );
}

ServerConnection::~ServerConnection()
{}

void ServerConnection::messageReceivedHello( const Message::Hello & msg )
{
    qDebug() << "[ServerConnection::messageReceivedHello] invitationNumber:" << msg.invitationNumber();
    ServerSession *session = qobject_cast<ServerSession*>( parent() );
    if ( session->id() == msg.invitationNumber() ) {
        Message::HelloAnswer answer( msg.id(), Message::HelloAnswer::Accepted, id(), false, "" ); //TODO: read-only UI
        socket->sendMsg( answer.toString() );
    } else {
        //TODO: allow message to UI to ask reject or not user
        Message::HelloAnswer answer( msg.id(), Message::HelloAnswer::Rejected, "", false, "Incorrect invitation number" );
        socket->sendMsg( answer.toString() );
    }
}

void ServerConnection::messageReceivedHelloAnswer( const Message::HelloAnswer & msg )
{
    qDebug() << "[ServerConnection::messageReceivedHello] Unexpected message:" << msg.toString();
}

void ServerConnection::messageReceivedUpdate( const Message::Update & msg )
{
    qDebug() << "[ServerConnection::messageReceivedUpdate] stub:" << msg.toString();
}

void ServerConnection::messageReceivedUpdateAnswer( const Message::UpdateAnswer & msg )
{
    qDebug() << "[ServerConnection::messageReceivedUpdateAnswer] stub:" << msg.toString();
}

void ServerConnection::messageReceivedSessionClosed( const QString & sessionId )
{
    qDebug() << "[ServerConnection::messageReceivedSessionClosed]";
    //TODO:we do not need sessionId here, because only one session: kword creates new plugin for new document.
    ServerSession *session = qobject_cast<ServerSession*>( parent() );
    session->closeConnection( this );
}

void ServerConnection::close()
{
    socket->close();
}

#include "ServerConnection.moc"
