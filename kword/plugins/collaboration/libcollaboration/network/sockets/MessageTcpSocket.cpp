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
#include "MessageTcpSocket.h"
#include <QByteArray>
#include <QDataStream>
using namespace kcollaborate;

static const int KEEPALIVE_INTERVAL = 5 * 1000;

MessageTcpSocket::MessageTcpSocket( bool aKeepalive, QObject *parent ):
        QTcpSocket( parent ), keepalive( aKeepalive ), blockSize( 0 )
{
    QObject::connect( this, SIGNAL( readyRead() ), this, SLOT( dataRecieved() ) );

    if ( keepalive ) {
        QObject::connect( this, SIGNAL( disconnected() ), &keepaliveTimer, SLOT( stop() ) );
        QObject::connect( &keepaliveTimer, SIGNAL( timeout() ), this, SLOT( sendKeepalive() ) );
        keepaliveTimer.setInterval( KEEPALIVE_INTERVAL );
    }
}

MessageTcpSocket::MessageTcpSocket( int socketDescriptor, bool aKeepalive, QObject *parent ):
        QTcpSocket( parent ), keepalive( aKeepalive ), blockSize( 0 )
{
    setSocketDescriptor( socketDescriptor );

    QObject::connect( this, SIGNAL( readyRead() ), this, SLOT( dataRecieved() ) );

    if ( keepalive ) {
        QObject::connect( this, SIGNAL( disconnected() ), &keepaliveTimer, SLOT( stop() ) );
        QObject::connect( &keepaliveTimer, SIGNAL( timeout() ), this, SLOT( sendKeepalive() ) );
        keepaliveTimer.setInterval( KEEPALIVE_INTERVAL );
    }
}

MessageTcpSocket::~MessageTcpSocket()
{}

bool MessageTcpSocket::sendMsg( const QString &msg )
{
    if ( msg.isEmpty() ) {
        return false;
    }

    QByteArray block;
    QDataStream out( &block, QIODevice::WriteOnly );
    out.setVersion( QDataStream::Qt_4_0 );
    out << ( quint16 )0;
    out << msg;
    out.device()->seek( 0 );
    out << ( quint16 )( block.size() - sizeof( quint16 ) );

    return write( block ) == block.size();
}

void MessageTcpSocket::sendKeepalive()
{
    sendMsg( " " );
}

void MessageTcpSocket::dataRecieved()
{
    QDataStream in( this );
    in.setVersion( QDataStream::Qt_4_0 );

    if ( blockSize == 0 ) {
        if ( bytesAvailable() < ( int )sizeof( quint16 ) )
            return;

        in >> blockSize;
    }

    if ( bytesAvailable() < blockSize )
        return;

    QString msg;
    in >> msg;

    if ( msg.size() != 1 || msg.at( 0 ) != ' ' ) { //skip keepalive
        emit msgReceived( msg );
    }
}

#include "MessageTcpSocket.moc"
