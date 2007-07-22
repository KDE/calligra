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
#include "HelloAnswer.h"
using namespace kcollaborate;
using namespace kcollaborate::Message;

HelloAnswer::HelloAnswer():
    Generic()
{
    qRegisterMetaType<kcollaborate::Message::HelloAnswer>("kcollaborate::Message::HelloAnswer");
}

HelloAnswer::HelloAnswer( const HelloAnswer& helloAnswer ):
    Generic()
{
    qRegisterMetaType<kcollaborate::Message::HelloAnswer>("kcollaborate::Message::HelloAnswer");

    m_id = helloAnswer.id();
    m_status = helloAnswer.status();
    m_sessionId = helloAnswer.sessionId();
    m_isReadOnly = helloAnswer.isReadOnly();
    m_text = helloAnswer.text();
}

HelloAnswer::HelloAnswer( QDomElement elt, QObject *parent ):
        Generic( parent )
{
    qRegisterMetaType<kcollaborate::Message::HelloAnswer>("kcollaborate::Message::HelloAnswer");

    fromXML( elt );
}

HelloAnswer::HelloAnswer( const QString &id, HelloAnswerStatus status, const QString &sessionId, bool isReadOnly,
                          const QString &text, QObject *parent ):
        Generic( parent ), m_id( id ), m_status( status ), m_sessionId( sessionId ), m_isReadOnly( isReadOnly ),
        m_text( text )
{
    qRegisterMetaType<kcollaborate::Message::HelloAnswer>("kcollaborate::Message::HelloAnswer");
}

HelloAnswer::~HelloAnswer()
{}

const QString & HelloAnswer::id() const { return m_id; }
HelloAnswer::HelloAnswerStatus HelloAnswer::status() const { return m_status; }
const QString & HelloAnswer::sessionId() const { return m_sessionId; }
bool HelloAnswer::isReadOnly() const { return m_isReadOnly; }
const QString & HelloAnswer::text() const { return m_text; }

QString HelloAnswer::tagName() const
{
    return "HelloAnswer";
}

void HelloAnswer::toXML( QDomDocument &doc, QDomElement &elt ) const
{
    elt.setAttribute( "id", id() );

    QDomElement element = doc.createElement( status2string( status() ) );
    if ( status() == HelloAnswer::Accepted ) {
        element.setAttribute( "sessionId", sessionId() );
        if ( isReadOnly() ) {
            QDomElement readOnlyElement = doc.createElement( "ReadOnly" );
            element.appendChild( readOnlyElement );
        }
    } else {
        if ( !text().isEmpty() ) {
            QDomCDATASection dataSection = doc.createCDATASection( text() );
            element.appendChild( dataSection );
        }
    }
    elt.appendChild( element );
}

void HelloAnswer::fromXML( QDomElement &elt )
{
    m_id = elt.attribute( "id", "" );
    if ( elt.hasChildNodes() ) {
        QDomElement element = elt.firstChild().toElement();
        if ( !element.isNull() ) {
            m_status = string2status( element.tagName() );
            if ( m_status == HelloAnswer::Accepted ) {
                m_sessionId = element.attribute( "sessionId", "" );
                QDomElement readOnlyElement = element.firstChildElement( "ReadOnly" );
                m_isReadOnly = !readOnlyElement.isNull();
            } else {
                QDomCDATASection dataSection = element.firstChild().toCDATASection();
                m_text = dataSection.data();
            }
        }
    }
}

QString HelloAnswer::status2string( HelloAnswer::HelloAnswerStatus status )
{
    switch ( status ) {
    case HelloAnswer::Rejected:
        return "Rejected";

    case HelloAnswer::Accepted:
        return "Accepted";

    case HelloAnswer::Timeout:
        return "Timeout";

    default:
        //case HelloAnswer::Unsupported:
        return "Unsupported";
    }
}

HelloAnswer::HelloAnswerStatus HelloAnswer::string2status( const QString &string )
{
    if ( "Rejected" == string ) {
        return HelloAnswer::Rejected;
    } else if ( "Accepted" == string ) {
        return HelloAnswer::Accepted;
    } else if ( "Timeout" == string ) {
        return HelloAnswer::Timeout;
    } else {// "Unsupported"
        return HelloAnswer::Unsupported;
    }
}

#include "HelloAnswer.moc"
