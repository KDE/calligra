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
#include "User.h"
using namespace kcollaborate::Message;

User::User():
        Generic( NULL )
{}

User::User( const User& user ):
        Generic( NULL )
{
    m_name = user.name();
    m_pictureUrl = user.pictureUrl();
    if ( !user.im().isEmpty() ) {
        //TODO: check this
        m_im = user.im();
    }
}

User::User( QDomElement elt, QObject *parent ):
        Generic( parent )
{
    fromXML( elt );
}

User::User( const QString &name, const QString &pictureUrl, QObject *parent ):
        Generic( parent ), m_name( name ), m_pictureUrl( pictureUrl )
{}

User::~User()
{}

const QString & User::name() const
{
    return m_name;
}

const QString & User::pictureUrl() const
{
    return m_pictureUrl;
}

const QMap<QString, QString> & User::im() const
{
    return m_im;
}

const QString User::im( const QString &imType ) const
{
    return m_im[imType];
}

void User::addIm( const QString &imType, const QString &account )
{
    m_im[imType] = account;
}

QString User::tagName() const
{
    return "User";
}

void User::toXML( QDomDocument &doc, QDomElement &elt ) const
{
    elt.setAttribute( "name", name() );
    elt.setAttribute( "pictureUrl", pictureUrl() );

    QMapIterator<QString, QString> i( im() );
    while ( i.hasNext() ) {
        i.next();

        QDomElement imElement = doc.createElement( "Im" );
        imElement.setAttribute( "type", i.key() );
        imElement.setAttribute( "account", i.value() );
        elt.appendChild( imElement );
    }
}

void User::fromXML( QDomElement &elt )
{
    m_name = elt.attribute( "name", "" );
    m_pictureUrl = elt.attribute( "pictureUrl", "" );

    QDomElement element = elt.firstChildElement( "Im" );
    for ( ; !element.isNull(); element = element.nextSiblingElement( "Im" ) ) {
        QString type = element.attribute( "type", "" );
        QString account = element.attribute( "account", "" );
        addIm( type, account );
    }
}

#include "User.moc"
