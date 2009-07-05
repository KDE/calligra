/* This file is part of the KDE project
   Copyright (C) 2009 Jeremias Epperlein <jeeree@web.de>

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

#include "EmptyElement.h"
#include "FormulaCursor.h"
#include <KoXmlWriter.h>
#include <KoXmlReader.h>
#include <QPainter>

#include <kdebug.h>

EmptyElement::EmptyElement( BasicElement* parent ) : BasicElement( parent )
{
}

EmptyElement::~EmptyElement()
{
}

ElementType EmptyElement::elementType() const
{
        return Empty;
}

void EmptyElement::paint ( QPainter& painter, AttributeManager* am )
{
    painter.save();
    painter.setPen( QPen( Qt::blue ) );
    painter.drawRect( QRectF(0.0, 0.0, width(), height()) );
    painter.restore();
}

QLineF EmptyElement::cursorLine ( int position ) const
{
    QPointF top=absoluteBoundingRect().topLeft()+QPointF( width()/2, 0 );;
    QPointF bottom = top + QPointF( 0.0, height() );
    return QLineF(top,bottom);
}

