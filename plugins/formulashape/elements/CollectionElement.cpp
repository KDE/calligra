/* This file is part of the KDE project
   Copyright (C) 2006-2007 Alfredo Beaumont Sainz <alfredo.beaumont@gmail.com>

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
   Boston, MA 02110-1301, USA.
*/

#include "QFontMetricsF"
#include "CollectionElement.h"
#include "TokenElement.h"
#include "AttributeManager.h"

CollectionElement::CollectionElement( BasicElement* parent ) : TokenElement( parent )
{

}


bool CollectionElement :: addCharacter(const QString input_text)
{

       static bool space_check =0;

        if(space_check==1)
        {
            m_group="";
            space_check=0;
        }
        if(input_text!=" "){
            m_group.append(input_text);
            return 0;
        }
        else{
            qDebug("Found a space");
            space_check=1;
            m_group.append("\0");
            return 1;
        }
}

QString CollectionElement :: group()
{
    return m_group;
}


QRectF CollectionElement::renderToPath( const QString& rawString, QPainterPath& path ) const
{
    AttributeManager manager;
    QFont font = manager.font(this);
    path.addText( path.currentPosition(), font, rawString );
    QFontMetricsF fm(font);
    return fm.boundingRect(rawString);
}
