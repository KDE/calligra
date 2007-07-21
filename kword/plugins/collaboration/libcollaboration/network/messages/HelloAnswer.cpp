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

HelloAnswer::HelloAnswer( int id, HelloAnswerStatus status, const QString &sessionId, bool isReadOnly,
                          const QString &text, QObject *parent ):
        Generic( parent ), m_id( id ), m_status( status ), m_sessionId( sessionId ), m_isReadOnly( isReadOnly ),
        m_text( text )
{}

HelloAnswer::~HelloAnswer()
{}

int HelloAnswer::id() const { return m_id; }
HelloAnswer::HelloAnswerStatus HelloAnswer::status() const { return m_status; }
const QString& HelloAnswer::sessionId() const { return m_sessionId; }
bool HelloAnswer::isReadOnly() const { return m_isReadOnly; }
const QString& HelloAnswer::text() const { return m_text; }

const QString HelloAnswer::toMsg() const
{
    QString out;
    out.reserve( 1024 );
    out.append( "<HelloAnswer>\n" );
    out.append( "<Id>" ).append( id() ).append( "</Id>\n" );

    switch ( status() ) {
    case HelloAnswer::Rejected:
        if ( text().isEmpty() ) {
            out.append( "<Rejected />\n" );
        } else {
            out.append( "<Rejected>" ).append( text() ).append( "</Rejected>\n" );
        }
        break;

    case HelloAnswer::Unsupported:
        if ( text().isEmpty() ) {
            out.append( "<Unsupported />\n" );
        } else {
            out.append( "<Unsupported>" ).append( text() ).append( "</Unsupported>\n" );
        }
        break;

    case HelloAnswer::Accepted:
        out.append( "<Accepted><Session>\n" );
        out.append( "<SessionId>" ).append( sessionId() ).append( "</SessionId>\n" );
        if ( isReadOnly() ) {
            out.append( "<ReadOnly />\n" );
        }
        out.append( "</Session></Accepted>\n" );
        break;

    case HelloAnswer::Timeout:
        if ( text().isEmpty() ) {
            out.append( "<Timeout />\n" );
        } else {
            out.append( "<Timeout>" ).append( text() ).append( "</Timeout>\n" );
        }
        break;
    }

    out.append( "</HelloAnswer>" );
    return out;
}

#include "HelloAnswer.moc"
