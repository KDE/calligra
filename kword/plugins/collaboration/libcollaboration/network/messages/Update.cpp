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

Update::Update():
        Generic()
{
    qRegisterMetaType<kcollaborate::Message::Update>( "kcollaborate::Message::Update" );
}

Update::Update( const Update& update ):
        Generic()
{
    qRegisterMetaType<kcollaborate::Message::Update>( "kcollaborate::Message::Update" );

    m_sessionId = update.sessionId();
    m_id = update.id();
    m_timestamp = update.timestamp();
    m_objectName = update.objectName();
    m_commandName = update.commandName();
    m_data = update.data();
}

Update::Update( const QString &sessionId, QDomElement elt, QObject *parent ):
        Generic( parent ), m_sessionId( sessionId )
{
    qRegisterMetaType<kcollaborate::Message::Update>( "kcollaborate::Message::Update" );

    fromXML( elt );
}

Update::Update( const QString &sessionId, const QString & id, const QString &timestamp, const QString &objectName,
                const QString &commandName, const QString &data, QObject *parent ):
        Generic( parent ), m_sessionId( sessionId ), m_id( id ), m_timestamp( timestamp ),
        m_objectName( objectName ), m_commandName( commandName ), m_data( data )
{
    qRegisterMetaType<kcollaborate::Message::Update>( "kcollaborate::Message::Update" );
}

Update::~Update()
{}

const QString & Update::sessionId() const { return m_sessionId; }
const QString & Update::id() const { return m_id; }
const QString & Update::timestamp() const { return m_timestamp; }
const QString & Update::objectName() const { return m_objectName; }
const QString & Update::commandName() const { return m_commandName; }
const QString & Update::data() const { return m_data; }

QString Update::tagName() const
{
    return "Update";
}

void Update::toXML( QDomDocument &doc, QDomElement &elt ) const
{
    //Note: session id is not set here
    elt.setAttribute( "id", id() );
    elt.setAttribute( "timestamp", timestamp() );
    elt.setAttribute( "objectName", objectName() );
    elt.setAttribute( "commandName", commandName() );
    if ( !data().isEmpty() ) {
        QDomCDATASection dataSection = doc.createCDATASection( data() );
        elt.appendChild( dataSection );
    }
}

void Update::fromXML( QDomElement &elt )
{
    m_id = elt.attribute( "id", "" );
    m_timestamp = elt.attribute( "timestamp", "" );
    m_objectName = elt.attribute( "objectName", "" );
    m_commandName = elt.attribute( "commandName", "" );
    QDomCDATASection dataSection = elt.firstChild().toCDATASection();
    if ( !dataSection.isNull() ) {
        m_data = dataSection.data();
    }
}

#include "Update.moc"
