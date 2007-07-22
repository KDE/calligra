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
#ifndef KCOLLABORATE_MESSAGE_UPDATE_H
#define KCOLLABORATE_MESSAGE_UPDATE_H

#include <libcollaboration/network/messages/Generic.h>
#include <QMetaType>

namespace kcollaborate
{
namespace Message
{

class KCOLLABORATE_EXPORT Update : public Generic
{
        Q_OBJECT
    public:
        Update();
        Update( const Update& update );
        Update( const QString &sessionId, QDomElement elt, QObject *parent = 0 );
        Update( const QString &sessionId, const QString & id, const QString &timestamp, const QString &objectName,
                const QString &commandName, const QString &data, QObject *parent = 0 );
        virtual ~Update();

        const QString & sessionId() const;
        const QString & id() const;
        const QString & timestamp() const;
        const QString & objectName() const;
        const QString & commandName() const;
        const QString & data() const;

        virtual QString tagName() const;
        virtual void toXML( QDomDocument &doc, QDomElement &elt ) const;
        virtual void fromXML( QDomElement &elt );
    private:
        QString m_sessionId;
        QString m_id;
        QString m_timestamp;
        QString m_objectName;
        QString m_commandName;
        QString m_data;
};

};
};

Q_DECLARE_METATYPE(kcollaborate::Message::Update)

#endif
