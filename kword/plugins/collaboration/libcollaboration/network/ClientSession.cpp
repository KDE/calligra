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
#include "ClientSession.h"
#include <KUser>
#include "sockets/MessageTcpSocket.h"
#include "messages/MessageFactory.h"
#include "messages/User.h"
using namespace kcollaborate;

ClientSession::ClientSession( const Url &url, QObject *parent ):
        Session( url, parent )
{
    qRegisterMetaType<kcollaborate::Message::Hello>( "kcollaborate::Message::Hello" );
    qRegisterMetaType<kcollaborate::Message::HelloAnswer>( "kcollaborate::Message::HelloAnswer" );
    qRegisterMetaType<kcollaborate::Message::Update>( "kcollaborate::Message::Update" );
    qRegisterMetaType<kcollaborate::Message::UpdateAnswer>( "kcollaborate::Message::UpdateAnswer" );

    messageFactory = new Message::MessageFactory( this );
    connect( messageFactory, SIGNAL( messageReceivedHello( const kcollaborate::Message::Hello & ) ),
             this, SLOT( messageReceivedHello( const kcollaborate::Message::Hello & ) ) );
    connect( messageFactory, SIGNAL( messageReceivedHelloAnswer( const kcollaborate::Message::HelloAnswer & ) ),
             this, SLOT( messageReceivedHelloAnswer( const kcollaborate::Message::HelloAnswer & ) ) );
    connect( messageFactory, SIGNAL( messageReceivedUpdate( const kcollaborate::Message::Update & ) ),
             this, SLOT( messageReceivedUpdate( const kcollaborate::Message::Update & ) ) );
    connect( messageFactory, SIGNAL( messageReceivedUpdateAnswer( const kcollaborate::Message::UpdateAnswer & ) ),
             this, SLOT( messageReceivedUpdateAnswer( const kcollaborate::Message::UpdateAnswer & ) ) );
    connect( messageFactory, SIGNAL( messageReceivedSessionClosed( const QString & ) ),
             this, SLOT( messageReceivedSessionClosed( const QString & ) ) );

    socket = new MessageTcpSocket( true, this );
    connect( socket, SIGNAL( msgReceived( const QString & ) ),
             messageFactory, SLOT( processRawMessage( const QString & ) ) );
    connect( socket, SIGNAL( connected() ),
             this, SLOT( connected() ) );

    socket->connectToHost( Session::url().hostAddress(), Session::url().port() );
}

ClientSession::~ClientSession()
{}

void ClientSession::close()
{
    socket->close();
}

void ClientSession::sendMsg( const QString& msg )
{
    socket->sendMsg( "<Messages>" + msg + "</Messages>" );
}

void ClientSession::connected()
{
    KUser kuser;
    //TODO: pictureUrl, im
    Message::Hello hello( id(), "0.1", url().invitationNumber(), new Message::User( kuser.fullName(), "" ) );
    sendMsg( hello.toString() );
}

void ClientSession::messageReceivedHello( const kcollaborate::Message::Hello &msg )
{
    qDebug() << "[ClientSession::messageReceivedHello] Unexpected message:" << msg.toString();
}

void ClientSession::messageReceivedHelloAnswer( const kcollaborate::Message::HelloAnswer &msg )
{
    qDebug() << "[ClientSession::messageReceivedHello] dump:" << msg.toString();
    if ( msg.status() == Message::HelloAnswer::Accepted ) {
        emit connected();
    }
}

void ClientSession::messageReceivedUpdate( const Message::Update &msg )
{
    qDebug() << "[ClientSession::messageReceivedUpdate] dump:" << msg.toString();
    if ( id() != msg.sessionId() ) {
        qDebug() << "[ClientSession::messageReceivedUpdate] Message has been recieved with different sessionId. Changes in the transport? (" << id() << ", " << msg.sessionId() << ")";
        return;
    }

    emit messageUpdate( msg );
}

void ClientSession::messageReceivedUpdateAnswer( const kcollaborate::Message::UpdateAnswer &msg )
{
    qDebug() << "[ClientSession::messageReceivedUpdateAnswer] dump:" << msg.toString();
    if ( id() != msg.sessionId() ) {
        qDebug() << "[ClientSession::messageReceivedUpdateAnswer] Message has been recieved with different sessionId. Changes in the transport? (" << id() << ", " << msg.sessionId() << ")";
        return;
    }

    emit messageUpdateAnswer( msg );
}

void ClientSession::messageReceivedSessionClosed( const QString &sessionId )
{
    qDebug() << "[ClientSession::messageReceivedSessionClosed] dump:" << sessionId;
    if ( id() != sessionId ) {
        qDebug() << "[ClientSession::messageReceivedSessionClosed] Message has been recieved with different sessionId. Changes in the transport? (" << id() << ", " << sessionId << ")";
        return;
    }
    close();
    emit disconnected();
}

#include "ClientSession.moc"
