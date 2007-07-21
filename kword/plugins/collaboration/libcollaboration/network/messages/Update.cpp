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
#include "Update.h"
using namespace kcollaborate;
using namespace kcollaborate::Message;

Update::Update( const QString &sessionId, long timestamp, const QString &objectName, const QString &commandName,
                const QString &data, QObject *parent ):
        Generic( parent ), m_sessionId( sessionId ), m_timestamp( timestamp ),
        m_objectName( objectName ), m_commandName( commandName ), m_data( data )
{}

Update::~Update()
{}

const QString& Update::sessionId() const { return m_sessionId; }
int Update::timestamp() const { return m_timestamp; }
const QString& Update::objectName() const { return m_objectName; }
const QString& Update::commandName() const { return m_commandName; }
const QString& Update::data() const { return m_data; }

const QString Update::toMsg() const
{
    QString out;
    out.reserve( 1024 );
    out.append( "<Update>\n" );
    out.append( "<SessionId>" ).append( sessionId() ).append( "</SessionId>\n" );
    out.append( "<Timestamp>" ).append( timestamp() ).append( "</Timestamp>\n" );
    out.append( "<ObjectName>" ).append( objectName() ).append( "</ObjectName>\n" );
    out.append( "<CommandName>" ).append( commandName() ).append( "</CommandName>\n" );
    if ( !data().isEmpty() ) {
        out.append( "<Data>" ).append( data() ).append( "</Data>\n" );
    }
    out.append( "</Update>" );
    return out;
}

#include "Update.moc"
