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

KoOasisSettings::KoOasisSettings( const QDomDocument &doc )
    :m_doc( doc )
{
}

bool KoOasisSettings::selectItemSet( const QString &itemSetName )
{
    QDomElement contents = m_doc.documentElement();
    QDomElement settingsElement = KoDom::namedItemNS( contents, KoXmlNS::office, "settings" );
    if ( settingsElement.isNull() )
    {
        m_element = QDomElement();
        kdDebug()<<" document doesn't have tag 'office:settings' \n";
        return false;
    }
    QDomElement e;
    forEachElement( e, settingsElement )
    {
        if ( e.localName() == "config-item-set" &&
             e.namespaceURI() == KoXmlNS::config &&
             e.attributeNS( KoXmlNS::config, "name", QString::null ) == itemSetName )
        {
            m_element = e;
            return true;
        }
    }

    return false;
}

bool KoOasisSettings::selectItemMap( const QString &itemMapName )
{
    QDomElement configItem;
    forEachElement( configItem, m_element )
    {
        if ( configItem.localName() == "config-item-map-indexed" &&
             configItem.namespaceURI() == KoXmlNS::config &&
             configItem.attributeNS( KoXmlNS::config, "name", QString::null ) == itemMapName )
        {
            m_element = configItem;
            return true;
        }
    }
    return false;
}


bool KoOasisSettings::selectItemMapNamed( const QString &itemMapName )
{
    if ( m_element.isNull() )
        return false;
    QDomElement element;
    forEachElement( element, m_element )
    {
        if ( element.localName() == "config-item-map-named" &&
             element.namespaceURI() == KoXmlNS::config &&
             element.attributeNS( KoXmlNS::config, "name", QString::null ) == itemMapName )
        {
            m_element = element;
            return true;
        }
    }
    return false;
}

QString KoOasisSettings::parseConfigItemName( const QDomElement & element, const QString &item ) const
{
    QDomElement it;
    forEachElement( it, element )
    {
        if ( it.localName() == "config-item" &&
             it.namespaceURI() == KoXmlNS::config &&
             it.attributeNS( KoXmlNS::config, "name", QString::null ) == item )
        {
            return it.text();
        }
    }
    return QString::null;
}

bool KoOasisSettings::selectItemMapEntry( const QString& entryName )
{
    QDomElement element;
    forEachElement( element, m_element )
    {
        if ( element.localName() == "config-item-map-entry" &&
             element.namespaceURI() == KoXmlNS::config &&
             element.attributeNS( KoXmlNS::config, "name", QString::null ) == entryName )
        {
            m_element = element;
            return true;
        }
    }
    return false;
}

QString KoOasisSettings::parseConfigItem( const QString &item, const QString & /*TODO remove*/ ) const
{
    if ( m_element.isNull() )
        return QString::null;
    return parseConfigItemName( m_element, item );
}


QString KoOasisSettings::parseConfigItemString( const QString & configName, const QString &itemNameEntry ) const
{
    return parseConfigItem( configName, itemNameEntry );
}

int KoOasisSettings::parseConfigItemInt( const QString & configName, const QString &itemNameEntry ) const
{
    int value=0;
    bool ok;
    QString str = parseConfigItem( configName, itemNameEntry );
    value = str.toInt( &ok );
    if ( ok )
        return value;
    return 0;
}

double KoOasisSettings::parseConfigItemDouble( const QString & configName, const QString &itemNameEntry ) const
{
    double value=0.0;
    bool ok;
    QString str = parseConfigItem( configName, itemNameEntry );
    value = str.toDouble( &ok );
    if ( ok )
        return value;
    return 0.0;
}

bool KoOasisSettings::parseConfigItemBool( const QString & configName, const QString &itemNameEntry ) const
{
    QString str = parseConfigItem( configName, itemNameEntry );
    return ( str == "true"  ? true : false );
}

short KoOasisSettings::parseConfigItemShort( const QString & configName, const QString &itemNameEntry ) const
{
    short value=0;
    bool ok;
    QString str = parseConfigItem( configName, itemNameEntry );
    value = str.toShort( &ok );
    if ( ok )
        return value;
    return 0;
}

long KoOasisSettings::parseConfigItemLong( const QString & configName, const QString &itemNameEntry ) const
{
    long value=0;
    bool ok;
    QString str = parseConfigItem( configName, itemNameEntry );
    value = str.toLong( &ok );
    if ( ok )
        return value;
    return 0;
}
