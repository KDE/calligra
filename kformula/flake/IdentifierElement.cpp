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
#include "AttributeManager.h"
#include "Dictionary.h"

#include <kdebug.h>

IdentifierElement::IdentifierElement( BasicElement* parent ) : TokenElement( parent )
{}

void IdentifierElement::renderToPath( const QString& raw, QPainterPath& path )
{
    AttributeManager manager;

    if( raw.startsWith( '&' ) && raw.endsWith( ';' ) ) {
        Dictionary dict;
        QChar mappedEntity = dict.mapEntity( raw );
        kDebug() << "mappenEntity: " << mappedEntity;
        path.addText( path.currentPosition(), manager.font( this ), mappedEntity ); 
    }
    else
        path.addText( path.currentPosition(), manager.font( this ), "hallo" );
}

ElementType IdentifierElement::elementType() const
{ 
    return Identifier;
}

