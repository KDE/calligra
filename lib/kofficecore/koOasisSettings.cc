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

QString KoOasisSettings::parseConfigItemString( const QString & configName ) const
{
    kdDebug()<<"QString KoOasisSettings::parseConfigItemString( configName :"<<configName<<endl;
    QDomNode item = m_element.firstChild(); //<config:config-item-map-entry>
    kdDebug()<<"item.isNull() :"<<item.isNull()<<endl;
    for ( QDomNode item2 = item.firstChild(); !item2.isNull(); item2 = item2.nextSibling() )
    {
        kdDebug()<<"*******************************************************************\n";
        QDomElement viewItem = item2.toElement();
        kdDebug()<<"viewItem.tagName() :"<<viewItem.tagName()<<endl;
        if ( viewItem.tagName()=="config:config-item" && ( viewItem.attribute("config:name")==configName ) )
        {
            return viewItem.text();
        }
    }
    return QString::null;
}

int KoOasisSettings::parseConfigItemInt( const QString & configName ) const
{
    int value=0;
    QDomNode item = m_element.firstChild(); //<config:config-item-map-entry>
    for ( QDomNode item2 = item.firstChild(); !item2.isNull(); item2 = item2.nextSibling() )
    {
        QDomElement viewItem = item2.toElement();
        //kdDebug()<<"viewItem.tagName() :"<<viewItem.tagName()<<endl;
        if ( viewItem.tagName()=="config:config-item" && ( viewItem.attribute("config:name")==configName ) )
        {
            bool ok;
            QString str = viewItem.text();
            value = str.toInt( &ok );
            if ( ok )
                return value;
            return 0;
        }
    }
    return 0;
}


double KoOasisSettings::parseConfigItemDouble( const QString & configName ) const
{
    double value=0.0;
    QDomNode item = m_element.firstChild(); //<config:config-item-map-entry>
    for ( QDomNode item2 = item.firstChild(); !item2.isNull(); item2 = item2.nextSibling() )
    {
        QDomElement viewItem = item2.toElement();
        //kdDebug()<<"viewItem.tagName() :"<<viewItem.tagName()<<endl;
        if ( viewItem.tagName()=="config:config-item" && ( viewItem.attribute("config:name")==configName ) )
        {
            bool ok;
            QString str = viewItem.text();
            value = str.toDouble( &ok );
            if ( ok )
                return value;
            return 0.0;
        }
    }
    return 0.0;
}


bool KoOasisSettings::parseConfigItemBool( const QString & configName ) const
{
    QDomNode item = m_element.firstChild(); //<config:config-item-map-entry>
    for ( QDomNode item2 = item.firstChild(); !item2.isNull(); item2 = item2.nextSibling() )
    {
        QDomElement viewItem = item2.toElement();
        //kdDebug()<<"viewItem.tagName() :"<<viewItem.tagName()<<endl;
        if ( viewItem.tagName()=="config:config-item" && ( viewItem.attribute("config:name")==configName ) )
        {
            return ( viewItem.text()=="true"  ? true : false );
        }
    }
    return false;
}

