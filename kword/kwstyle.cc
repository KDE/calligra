/* This file is part of the KDE project
   Copyright (C) 2001 David Faure <faure@kde.org>

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

#include "kwstyle.h"
#include <qdom.h>

KWStyle::KWStyle( const QString & name )
{
    m_paragLayout.styleName = name;
    m_paragLayout.alignment = Qt::AlignLeft;
    //default ptWidth=0
    m_paragLayout.leftBorder.ptWidth=0;
    m_paragLayout.rightBorder.ptWidth=0;
    m_paragLayout.topBorder.ptWidth=0;
    m_paragLayout.bottomBorder.ptWidth=0;
}

KWStyle::KWStyle( QDomElement & styleElem )
    : m_paragLayout( styleElem ) // Load the paraglayout from the <STYLE> element
{
    m_followingStyle = styleElem.namedItem("FOLLOWING").toElement().attribute("name");

    QDomElement formatElem = styleElem.namedItem( "FORMAT" ).toElement();
    if ( !formatElem.isNull() )
        m_format = KWTextParag::loadFormat( formatElem, 0L );
}

void KWStyle::save( QDomElement parentElem )
{
    QDomDocument doc = parentElem.ownerDocument();
    QDomElement styleElem = doc.createElement( "STYLE" );
    parentElem.appendChild( styleElem );
    QDomElement element = doc.createElement( "FOLLOWING" );
    styleElem.appendChild( element );
    element.setAttribute( "name", m_followingStyle );
    m_paragLayout.save( styleElem );


    KWTextParag::saveFormat( doc, &m_format, 0L, 0, 0 );
}
