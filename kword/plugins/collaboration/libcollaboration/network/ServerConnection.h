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
#ifndef KCOLLABORATE_SERVERCONNECTION_H
#define KCOLLABORATE_SERVERCONNECTION_H

#include <QObject>

#include <libcollaboration/network/messages/Hello.h>
#include <libcollaboration/network/messages/HelloAnswer.h>
#include <libcollaboration/network/messages/Update.h>
#include <libcollaboration/network/messages/UpdateAnswer.h>

namespace kcollaborate
{
class MessageTcpSocket;
class ServerSession;

namespace Message
{
class MessageFactory;
};

/*
    This class stores info about connected user and his status.
*/
class KCOLLABORATE_EXPORT ServerConnection : public QObject
{
        Q_OBJECT

    public:
        ServerConnection( MessageTcpSocket *socket, ServerSession *parent );
        virtual ~ServerConnection();

        const QString& id() const { return id_; };
        void close();

    signals:
        void connected();
        void disconnected();

        void messageUpdate( const kcollaborate::Message::Update &msg );
        void messageUpdateAnswer( const kcollaborate::Message::UpdateAnswer &msg );

    private slots:
        void messageReceivedHello( const kcollaborate::Message::Hello &msg );
        void messageReceivedHelloAnswer( const kcollaborate::Message::HelloAnswer &msg );
        void messageReceivedUpdate( const kcollaborate::Message::Update &msg );
        void messageReceivedUpdateAnswer( const kcollaborate::Message::UpdateAnswer &msg );
        void messageReceivedSessionClosed( const QString &sessionId );

    private:
        QString id_;
        MessageTcpSocket *socket;
        Message::MessageFactory *messageFactory;

        void sendMsg( const QString& msg );
};

};

#endif
