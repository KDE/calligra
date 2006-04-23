/* This file is part of the KDE project
 Copyright (C) 2003-2004 Alexander Dymo <adymo@mksat.net>

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
 the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/
#include "pageheader.h"

#include <klocale.h>

#include <qpainter.h>

#include <koproperty/property.h>

namespace Kudesigner
{

PageHeader::PageHeader( int x, int y, int width, int height, Canvas *canvas ) :
        Band( x, y, width, height, canvas )
{
    props.addProperty( new Property( "Height", 50, i18n( "Height" ), i18n( "Height" ), KoProperty::Integer ), "Section" );
    QMap<QString, QString> m;

    m[ i18n( "First Page" ) ] = "0";
    m[ i18n( "Every Page" ) ] = "1";
    m[ i18n( "Last Page" ) ] = "2";

    props.addProperty( new Property( "PrintFrequency", m.values(), m.keys(), "1", i18n( "Print Frequency" ), i18n( "Print Frequency" ) ), "Section" );
}

void PageHeader::draw( QPainter &painter )
{
    painter.drawText( rect(), Qt::AlignVCenter | Qt::AlignLeft,
                      i18n( "Page Header" ) );
    Band::draw( painter );
}

QString PageHeader::getXml()
{
    return "\t<PageHeader" + Band::getXml() + "\t</PageHeader>\n\n";
}

}
