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
#ifndef KCOLLABORATE_MESSAGE_USER_H
#define KCOLLABORATE_MESSAGE_USER_H

#include <QObject>
#include <QString>
#include <QMap>
#include <QDomDocument>
#include <QDomElement>

#include <libcollaboration/network/messages/Generic.h>

namespace kcollaborate
{
namespace Message
{

class KCOLLABORATE_EXPORT User : public Generic
{
        Q_OBJECT
    public:
        User();
        User(const User& user);
        User( QDomElement elt, QObject *parent = 0 );
        User( const QString &name, const QString &pictureUrl, QObject *parent = 0 );
        virtual ~User();

        void addIm( const QString &imType, const QString &account );

        const QString & name() const;
        const QString & pictureUrl() const;
        const QMap<QString, QString> & im() const;
        const QString im( const QString &imType ) const;

        virtual QString tagName() const;
        virtual void toXML( QDomDocument &doc, QDomElement &elt ) const;
        virtual void fromXML( QDomElement &elt );
    private:
        QString m_name;
        QString m_pictureUrl;
        QMap<QString, QString> m_im;
};

};
};

#endif
