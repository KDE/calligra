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

#include "IdentifierElement.h"
#include "Entities.h"
#include <algorithm>
#include <kdebug.h>

IdentifierElement::IdentifierElement( BasicElement* parent ) : TokenElement( parent )
{}

QString IdentifierElement::stringToRender( const QString& rawString ) const
{
    const entityMap* begin = entities;
    const entityMap* end = entities + entityMap::size();
    const entityMap* pos = std::lower_bound( begin, end, rawString.toAscii() );

    if ( pos == end || QString( pos->name ) != rawString )
         kWarning() << "Invalid entity refererence: " << rawString << endl;
    else
         return QChar( pos->unicode );

    return rawString;
}
