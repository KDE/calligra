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
#include "Hello.h"
#include "User.h"
using namespace kcollaborate;
using namespace kcollaborate::Message;

Hello::Hello():
        Generic(), m_user( NULL )
{
    qRegisterMetaType<kcollaborate::Message::Hello>("kcollaborate::Message::Hello");
}

Hello::Hello( const Hello& hello ):
        Generic(), m_user( NULL )
{
    qRegisterMetaType<kcollaborate::Message::Hello>("kcollaborate::Message::Hello");

    m_id = hello.id();
    m_protocolVersion = hello.protocolVersion();
    m_invitationNumber = hello.invitationNumber();
    if ( hello.user() != NULL ) {
        m_user = new User( *hello.user() );
    }
}

Hello::Hello( QDomElement elt, QObject *parent ):
        Generic( parent ), m_user( NULL )
{
    qRegisterMetaType<kcollaborate::Message::Hello>("kcollaborate::Message::Hello");

    fromXML( elt );
}

Hello::Hello( const QString &id, const QString &protocolVersion, const QString &invitationNumber,
              const User *user, QObject *parent ):
        Generic( parent ), m_id( id ), m_protocolVersion( protocolVersion ), m_invitationNumber( invitationNumber ),
        m_user( user )
{
    qRegisterMetaType<kcollaborate::Message::Hello>("kcollaborate::Message::Hello");
}

Hello::~Hello()
{
    delete m_user;
}

const QString & Hello::id() const { return m_id; }
const QString & Hello::protocolVersion() const { return m_protocolVersion; }
const QString & Hello::invitationNumber() const { return m_invitationNumber; }
const User * Hello::user() const { return m_user; }

void Hello::setUser( const User *user )
{
    delete m_user;
    m_user = NULL;
    m_user = user;
}

QString Hello::tagName() const
{
    return "Hello";
}

void kcollaborate::Message::Hello::toXML( QDomDocument & doc, QDomElement & elt ) const
{
    elt.setAttribute( "id", id() );
    elt.setAttribute( "protocolVersion", protocolVersion() );
    elt.setAttribute( "invitationNumber", invitationNumber() );
    if ( m_user ) {
        QDomElement userElement = doc.createElement( m_user->tagName() );
        m_user->toXML( doc, userElement );
        elt.appendChild( userElement );
    }
}

void kcollaborate::Message::Hello::fromXML( QDomElement & elt )
{
    m_id = elt.attribute( "id", "" );
    m_protocolVersion = elt.attribute( "protocolVersion", "" );
    m_invitationNumber = elt.attribute( "invitationNumber", "" );

    m_user = NULL;
    QDomElement userElement = elt.firstChildElement( "User" );//TODO: make tagName static?
    if ( !userElement.isNull() ) {
        setUser( new User( userElement ) );
    }
}

#include "Hello.moc"
