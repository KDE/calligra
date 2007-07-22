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
#ifndef KCOLLABORATE_MESSAGE_UPDATEANSWER_H
#define KCOLLABORATE_MESSAGE_UPDATEANSWER_H

#include <libcollaboration/network/messages/Generic.h>
#include <QMetaType>

namespace kcollaborate
{
namespace Message
{

class KCOLLABORATE_EXPORT UpdateAnswer : public Generic
{
        Q_OBJECT
        Q_ENUMS( UpdateAnswerStatus )
    public:
        enum UpdateAnswerStatus {
            Rejected = 0, Unsupported = 1, Accepted = 2, Timeout = 3
    };

        UpdateAnswer();
        UpdateAnswer( const UpdateAnswer &updateAnswer );
        UpdateAnswer( const QString &sessionId, QDomElement elt, QObject *parent = 0 );
        UpdateAnswer( const QString &id, UpdateAnswerStatus status, const QString &sessionId, QObject *parent = 0 );
        virtual ~UpdateAnswer();

        const QString & id() const;
        UpdateAnswerStatus status() const;
        const QString & sessionId() const;

        virtual QString tagName() const;
        virtual void toXML( QDomDocument &doc, QDomElement &elt ) const;
        virtual void fromXML( QDomElement &elt );

        static QString status2string( UpdateAnswerStatus status );
        static UpdateAnswerStatus string2status( const QString &string );
    private:
        QString m_id;
        UpdateAnswerStatus m_status;
        QString m_sessionId;
};

};
};

Q_DECLARE_METATYPE(kcollaborate::Message::UpdateAnswer)

#endif
