/* This file is part of the KDE project
   Copyright (C) 2004 Laurent Montel <montel@kde.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#include "koOasisSettings.h"
#include "koxmlns.h"
#include "kodom.h"
#include <kdebug.h>

KoOasisSettings::KoOasisSettings( const QDomDocument& doc )
    : m_settingsElement( KoDom::namedItemNS( doc.documentElement(), KoXmlNS::office, "settings" ) )
{
    const QDomElement contents = doc.documentElement();
    if ( m_settingsElement.isNull() )
        kdDebug() << " document doesn't have tag 'office:settings'\n";
}

KoOasisSettings::Items KoOasisSettings::itemSet( const QString& itemSetName ) const
{
    QDomElement e;
    forEachElement( e, m_settingsElement )
    {
        if ( e.localName() == "config-item-set" &&
             e.namespaceURI() == KoXmlNS::config &&
             e.attributeNS( KoXmlNS::config, "name", QString::null ) == itemSetName )
        {
            return Items( e );
        }
    }

    return Items( QDomElement() );
}

KoOasisSettings::IndexedMap KoOasisSettings::Items::indexedMap( const QString& itemMapName ) const
{
    QDomElement configItem;
    forEachElement( configItem, m_element )
    {
        if ( configItem.localName() == "config-item-map-indexed" &&
             configItem.namespaceURI() == KoXmlNS::config &&
             configItem.attributeNS( KoXmlNS::config, "name", QString::null ) == itemMapName )
        {
            return IndexedMap( configItem );
        }
    }
    return IndexedMap( QDomElement() );
}

KoOasisSettings::NamedMap KoOasisSettings::Items::namedMap( const QString& itemMapName ) const
{
    QDomElement configItem;
    forEachElement( configItem, m_element )
    {
        if ( configItem.localName() == "config-item-map-named" &&
             configItem.namespaceURI() == KoXmlNS::config &&
             configItem.attributeNS( KoXmlNS::config, "name", QString::null ) == itemMapName )
        {
            return NamedMap( configItem );
        }
    }
    return NamedMap( QDomElement() );
}

KoOasisSettings::Items KoOasisSettings::IndexedMap::entry( int entryIndex ) const
{
    int i = 0;
    QDomElement entry;
    forEachElement( entry, m_element )
    {
        if ( entry.localName() == "config-item-map-entry" &&
             entry.namespaceURI() == KoXmlNS::config )
        {
            if ( i == entryIndex )
                return Items( entry );
            else
                ++i;
        }
    }
    return Items( QDomElement() );
}

KoOasisSettings::Items KoOasisSettings::NamedMap::entry( const QString& entryName ) const
{
    QDomElement entry;
    forEachElement( entry, m_element )
    {
        if ( entry.localName() == "config-item-map-entry" &&
             entry.namespaceURI() == KoXmlNS::config &&
             entry.attributeNS( KoXmlNS::config, "name", QString::null ) == entryName )
        {
            return Items( entry );
        }
    }
    return Items( QDomElement() );
}

// static helper
QString KoOasisSettings::Items::findConfigItem( const QDomElement& element,
                                                     const QString& item, bool* ok )
{
    QDomElement it;
    forEachElement( it, element )
    {
        if ( it.localName() == "config-item" &&
             it.namespaceURI() == KoXmlNS::config &&
             it.attributeNS( KoXmlNS::config, "name", QString::null ) == item )
        {
            *ok = true;
            return it.text();
        }
    }
    *ok = false;
    return QString::null;
}

QString KoOasisSettings::Items::findConfigItem( const QString& item, bool* ok ) const
{
    return findConfigItem( m_element, item, ok );
}

QString KoOasisSettings::Items::parseConfigItemString( const QString& configName, const QString& defValue ) const
{
    bool ok;
    const QString str = findConfigItem( configName, &ok );
    return ok ? str : defValue;
}

int KoOasisSettings::Items::parseConfigItemInt( const QString& configName, int defValue ) const
{
    bool ok;
    const QString str = findConfigItem( configName, &ok );
    int value;
    if ( ok )
        value = str.toInt( &ok );
    if ( ok )
        return value;
    return defValue;
}

double KoOasisSettings::Items::parseConfigItemDouble( const QString& configName, double defValue ) const
{
    bool ok;
    const QString str = findConfigItem( configName, &ok );
    double value;
    if ( ok )
        value = str.toDouble( &ok );
    if ( ok )
        return value;
    return defValue;
}

bool KoOasisSettings::Items::parseConfigItemBool( const QString& configName, bool defValue ) const
{
    bool ok;
    const QString str = findConfigItem( configName, &ok );
    if ( str == "true" )
        return true;
    else if ( str == "false" )
        return false;
    return defValue;
}

short KoOasisSettings::Items::parseConfigItemShort( const QString& configName, short defValue ) const
{
    bool ok;
    const QString str = findConfigItem( configName, &ok );
    short value;
    if ( ok )
        value = str.toShort( &ok );
    if ( ok )
        return value;
    return defValue;
}

long KoOasisSettings::Items::parseConfigItemLong( const QString& configName, long defValue ) const
{
    bool ok;
    const QString str = findConfigItem( configName, &ok );
    long value;
    if ( ok )
        value = str.toLong( &ok );
    if ( ok )
        return value;
    return defValue;
}

