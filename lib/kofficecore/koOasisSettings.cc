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
#include <kdebug.h>

KoOasisSettings::KoOasisSettings( const QDomDocument &doc )
    :m_doc( doc )
{
}

bool KoOasisSettings::configItem( const QString &configItemName)
{
     QDomElement contents = m_doc.documentElement();
     QDomNode tmp = contents.namedItem("office:settings").toElement();
    if (tmp.isNull() )
    {
        m_element =QDomElement();
        kdDebug()<<" document doesn't have tag 'office:settings' \n";
        return false;
    }
    for ( QDomNode n = tmp.firstChild(); !n.isNull(); n = n.nextSibling() )
    {
        QDomElement e = n.toElement();
        //kdDebug()<<"e.tagName() :"<<e.tagName()<<endl;
        if ( e.hasAttribute( "config:name" ) && ( e.attribute( "config:name" )==configItemName ) )
        {
            m_element = e;
            return true;
        }
    }
    return false;
}

bool KoOasisSettings::mapItem( const QString &mapItemName)
{
    for ( QDomNode viewSetting = m_element.firstChild(); !viewSetting.isNull();
          viewSetting = viewSetting.nextSibling() )
    {
        QDomElement configItem = viewSetting.toElement();
        if ( configItem.tagName()== "config:config-item-map-indexed" && ( configItem.attribute( "config:name" )== mapItemName ) )
        {
            m_element = configItem;
            return true;
        }
    }
    return false;
}

QString KoOasisSettings::parseConfigItem( const QString &item ) const
{
    if ( !m_element.isNull() ) {
        QDomNode n = m_element.firstChild();
        for ( ; !n.isNull() ; n = n.nextSibling() ) {
            const QDomElement element = n.toElement();
            if ( element.isNull() ) continue;
            if ( element.tagName() ==  "config:config-item-map-entry" )
            {
                QDomNode tmp = element.firstChild();
                for ( ; !tmp.isNull() ; tmp = tmp.nextSibling() )
                {
                    const QDomElement viewItem = tmp.toElement();
                    if ( viewItem.isNull() ) continue;
                    if ( viewItem.tagName() == "config:config-item" && viewItem.attribute("config:name")==item)
                    {
                        return viewItem.text();
                    }
                }
            }
        }
    }
    return QString::null;
}


QString KoOasisSettings::parseConfigItemString( const QString & configName ) const
{
    return parseConfigItem( configName );
}

int KoOasisSettings::parseConfigItemInt( const QString & configName ) const
{
    int value=0;
    bool ok;
    QString str = parseConfigItem( configName );
    value = str.toInt( &ok );
    if ( ok )
        return value;
    return 0;
}

double KoOasisSettings::parseConfigItemDouble( const QString & configName ) const
{
    double value=0.0;
    bool ok;
    QString str = parseConfigItem( configName );
    value = str.toDouble( &ok );
    if ( ok )
        return value;
    return 0.0;
}

bool KoOasisSettings::parseConfigItemBool( const QString & configName ) const
{
    QString str = parseConfigItem( configName );
    return ( str == "true"  ? true : false );
}

short KoOasisSettings::parseConfigItemShort( const QString & configName ) const
{
    short value=0;
    bool ok;
    QString str = parseConfigItem( configName );
    value = str.toShort( &ok );
    if ( ok )
        return value;
    return 0;
}

long KoOasisSettings::parseConfigItemLong( const QString & configName ) const
{
    long value=0;
    bool ok;
    QString str = parseConfigItem( configName );
    value = str.toLong( &ok );
    if ( ok )
        return value;
    return 0;
}
