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
#ifndef KCOLLABORATE_MESSAGE_HELLO_H
#define KCOLLABORATE_MESSAGE_HELLO_H

#include <libcollaboration/network/messages/Generic.h>
#include <libcollaboration/network/User.h>

namespace kcollaborate
{
namespace Message
{

class KCOLLABORATE_EXPORT Hello : public Generic
{
        Q_OBJECT
    public:
        Hello( int id, int protocolVersion, int invitationNumber, const User *user,
               bool isRecoverSession, const QString &sessionId, const QString &lastTimestamp,
               QObject *parent = 0 );
        virtual ~Hello();

        int id() const;
        int protocolVersion() const;
        int invitationNumber() const;
        const User& user() const;
        //session related
        bool isRecoverSession() const;
        const QString& sessionId() const;
        const QString& lastTimestamp() const;

        const QString toMsg() const;
    private:
        int m_id;
        int m_protocolVersion;
        int m_invitationNumber;
        const User* m_user;

        //session related
        bool m_isRecoverSession;
        QString m_sessionId;
        QString m_lastTimestamp;
};

};
};

#endif
