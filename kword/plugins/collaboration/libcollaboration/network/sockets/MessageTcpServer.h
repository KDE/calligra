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
#ifndef KCOLLABORATE_MESSAGETCPSERVER_H
#define KCOLLABORATE_MESSAGETCPSERVER_H

#include <QTcpServer>
#include <libcollaboration/kcollaborate_export.h>

namespace kcollaborate
{
class MessageTcpSocket;

///Allows to recieve connections wrapped in the MessageTcpSocket class
class KCOLLABORATE_EXPORT MessageTcpServer : public QTcpServer
{
        Q_OBJECT
    public:
        MessageTcpServer( bool keepalive = true, QObject *parent = 0 );
        virtual ~MessageTcpServer();

        bool keepalive() const { return keepalive_; };
        void setKeepalive( bool keepalive ) { keepalive_ = keepalive; };

    signals:
        void incomingConnection( MessageTcpSocket *messageTcpSocket );

    private slots:
        void incomingConnection( int socketDescriptor );

    private:
        bool keepalive_;
};

};

#endif
