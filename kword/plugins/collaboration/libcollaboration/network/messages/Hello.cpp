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
using namespace kcollaborate;
using namespace kcollaborate::Message;

Hello::Hello( int id, int protocolVersion, int invitationNumber, const User *user,
              bool isRecoverSession, const QString &sessionId, const QString &lastTimestamp,
              QObject *parent ):
        Generic( parent ), m_id( id ), m_protocolVersion( protocolVersion ), m_invitationNumber( invitationNumber ),
        m_user( user ), m_isRecoverSession( isRecoverSession ), m_sessionId( sessionId ), m_lastTimestamp( lastTimestamp )
{}

Hello::~Hello()
{
    delete m_user;
}

int Hello::id() const { return m_id; }
int Hello::protocolVersion() const { return m_protocolVersion; }
int Hello::invitationNumber() const { return m_invitationNumber; }
const User& Hello::user() const { return *m_user; }

//session related
bool Hello::isRecoverSession() const { return m_isRecoverSession; }
const QString& Hello::sessionId() const { return m_sessionId; }
const QString& Hello::lastTimestamp() const { return m_lastTimestamp; }

//TODO: make this better?
const QString Hello::toMsg() const
{
    QString out;
    out.reserve( 1024 );
    out.append( "<Hello>\n" );
    out.append( "<Id>" ).append( id() ).append( "</Id>\n" );
    out.append( "<ProtocolVersion>" ).append( protocolVersion() ).append( "</ProtocolVersion>\n" );
    out.append( "<InvitationNumber>" ).append( invitationNumber() ).append( "</InvitationNumber>\n" );
    out.append( user().toMsgPart() );
    if ( isRecoverSession() ) {
        out.append( "<Session>\n" );
        out.append( "<Recover />\n" );
        out.append( "<SessionId>" ).append( sessionId() ).append( "</SessionId>\n" );
        out.append( "<LastTimestamp>" ).append( lastTimestamp() ).append( "</LastTimestamp>\n" );
        out.append( "</Session>\n" );
    } else {
        out.append( "<Session><New /></Session>\n" );
    }
    out.append( "</Hello>" );
    return out;
}

#include "Hello.moc"
