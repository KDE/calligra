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
#ifndef KCOLLABORATE_MESSAGETCPSOCKET_H
#define KCOLLABORATE_MESSAGETCPSOCKET_H

#include <QTcpSocket>
#include <QTimer>
#include <libcollaboration/kcollaborate_export.h>

namespace kcollaborate
{

///Allows to send and recieve string messages.
class KCOLLABORATE_EXPORT MessageTcpSocket : public QTcpSocket
{
        Q_OBJECT
    public:
        MessageTcpSocket( bool keepalive, QObject *parent );
        MessageTcpSocket( int socketDescriptor, bool keepalive, QObject *parent );
        virtual ~MessageTcpSocket();

    public slots:
        ///msg=="" will be not sent
        ///msg==" " will be not recieved (internal message)
        bool sendMsg( const QString &msg );

    signals:
        void msgReceived( const QString &msg );

    private slots:
        void dataRecieved();
        void sendKeepalive();

    private:
        bool keepalive;
        QTimer keepaliveTimer;

        quint16 blockSize;
};

};

#endif
