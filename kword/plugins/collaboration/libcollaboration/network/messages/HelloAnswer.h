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
#ifndef KCOLLABORATE_MESSAGE_HELLOANSWER_H
#define KCOLLABORATE_MESSAGE_HELLOANSWER_H

#include <libcollaboration/network/messages/Generic.h>

namespace kcollaborate
{
namespace Message
{

class KCOLLABORATE_EXPORT HelloAnswer : public Generic
{
        Q_OBJECT
        Q_ENUMS( HelloAnswerStatus )
    public:
        enum HelloAnswerStatus {
            Rejected = 0, Unsupported = 1, Accepted = 2, Timeout = 3
    };

        HelloAnswer( int id, HelloAnswerStatus status, const QString &sessionId, bool isReadOnly,
                     const QString &text, QObject *parent = 0 );
        virtual ~HelloAnswer();

        int id() const;
        HelloAnswerStatus status() const;
        //for Accepted
        const QString& sessionId() const;
        bool isReadOnly() const;
        //for other
        const QString& text() const;

        const QString toMsg() const;
    private:
        int m_id;
        HelloAnswerStatus m_status;
        //for Accepted
        QString m_sessionId;
        bool m_isReadOnly;
        //for other
        QString m_text;
};

};
};

#endif
