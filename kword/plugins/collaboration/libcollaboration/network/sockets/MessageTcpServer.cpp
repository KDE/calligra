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
#include "MessageTcpServer.h"
#include "MessageTcpSocket.h"
using namespace kcollaborate;

MessageTcpServer::MessageTcpServer( bool keepalive, QObject *parent ):
        QTcpServer( parent ), keepalive_( keepalive )
{}

MessageTcpServer::~MessageTcpServer()
{}

void MessageTcpServer::incomingConnection( int socketDescriptor )
{
    MessageTcpSocket *messageTcpSocket = new MessageTcpSocket( socketDescriptor, keepalive(), this );
    emit incomingConnection( messageTcpSocket );
}

#include "MessageTcpServer.moc"
