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


#ifndef KOOASISSETTINGS_H
#define KOOASISSETTINGS_H

#include <qdom.h>

/**
 * Class used to parse settings.xml file.
 */

/*
 * By default settings.xml is defineds as :
 *   <office:settings>
 *      <config:config-item-set config:name="view-settings">
 *         <config:config-item-map-indexed config:name="Views">
 *           <config:config-item config:name="SnapLinesDrawing" config:type="string">value</config:config-item>
 *           .....
 *         </config:config-item-map-indexed>
 *         <config:config-item-map-indexed config:name="Interface">
 *         .......
 *         </config:config-item-map-indexed>
 *       </config:config-item-set>
 *       <config:config-item-set config:name="configure-settings">
 *       ....
 *       </config:config-item-set>
 *   </office:settings>
 */


class KoOasisSettings
{
public:
    KoOasisSettings( const QDomDocument &doc );

    /*
     * return true if config item set "configItemName" exist
     */
    bool configItem( const QString &configItemName);

    /*
     * return true if mapItemName exist
     */
    bool mapItem( const QString &mapItemName);


    int parseConfigItemInt( const QString & configName ) const;
    double parseConfigItemDouble( const QString & configName ) const;
    QString parseConfigItemString( const QString & configName ) const;
    bool parseConfigItemBool( const QString & configName ) const;

private:
    //store it to change config item
    QDomDocument m_doc;
    QDomElement m_element;
};

#endif
