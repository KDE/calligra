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
#include "UpdateAnswer.h"
using namespace kcollaborate;
using namespace kcollaborate::Message;

UpdateAnswer::UpdateAnswer():
        Generic()
{
    qRegisterMetaType<kcollaborate::Message::UpdateAnswer>( "kcollaborate::Message::UpdateAnswer" );
}

UpdateAnswer::UpdateAnswer( const UpdateAnswer& updateAnswer ):
        Generic()
{
    qRegisterMetaType<kcollaborate::Message::UpdateAnswer>( "kcollaborate::Message::UpdateAnswer" );

    m_id = updateAnswer.id();
    m_status = updateAnswer.status();
    m_sessionId = updateAnswer.sessionId();
}

UpdateAnswer::UpdateAnswer( const QString &sessionId, QDomElement elt, QObject *parent ):
        Generic( parent ), m_sessionId( sessionId )
{
    qRegisterMetaType<kcollaborate::Message::UpdateAnswer>( "kcollaborate::Message::UpdateAnswer" );

    fromXML( elt );
}

UpdateAnswer::UpdateAnswer( const QString &id, UpdateAnswerStatus status, const QString &sessionId, QObject *parent ):
        Generic( parent ), m_id( id ), m_status( status ), m_sessionId( sessionId )
{
    qRegisterMetaType<kcollaborate::Message::UpdateAnswer>( "kcollaborate::Message::UpdateAnswer" );
}

UpdateAnswer::~UpdateAnswer()
{}

const QString & UpdateAnswer::id() const { return m_id; }
UpdateAnswer::UpdateAnswerStatus UpdateAnswer::status() const { return m_status; }
const QString & UpdateAnswer::sessionId() const { return m_sessionId; }

QString UpdateAnswer::tagName() const
{
    return "UpdateAnswer";
}

void UpdateAnswer::toXML( QDomDocument &, QDomElement &elt ) const
{
    //Note: no session id here
    elt.setAttribute( "id", id() );
    elt.setAttribute( "status", status2string( status() ) );
}

void UpdateAnswer::fromXML( QDomElement & elt )
{
    m_id = elt.attribute( "id", "" );
    m_status = string2status( elt.attribute( "status", "" ) );
}

QString UpdateAnswer::status2string( UpdateAnswer::UpdateAnswerStatus status )
{
    switch ( status ) {
    case UpdateAnswer::Rejected:
        return "Rejected";

    case UpdateAnswer::Accepted:
        return "Accepted";

    case UpdateAnswer::Timeout:
        return "Timeout";

    default:
        //case UpdateAnswer::Unsupported:
        return "Unsupported";
    }
}

UpdateAnswer::UpdateAnswerStatus UpdateAnswer::string2status( const QString &string )
{
    if ( "Rejected" == string ) {
        return UpdateAnswer::Rejected;
    } else if ( "Accepted" == string ) {
        return UpdateAnswer::Accepted;
    } else if ( "Timeout" == string ) {
        return UpdateAnswer::Timeout;
    } else {// "Unsupported"
        return UpdateAnswer::Unsupported;
    }
}

#include "UpdateAnswer.moc"
