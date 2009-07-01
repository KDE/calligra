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

#include "FixedElement.h"
#include "FormulaCursor.h"
#include <KoXmlWriter.h>
#include <KoXmlReader.h>
#include <QPainter>

#include <kdebug.h>

FixedElement::FixedElement( BasicElement* parent ) : BasicElement( parent )
{
}

FixedElement::~FixedElement()
{
}


BasicElement* FixedElement::elementAfter ( int position )
{
    if (position % 2 == 0) {
        return childElements()[position/2];
    } else {
        return 0;
    }
}

BasicElement* FixedElement::elementBefore ( int position )
{
    if (position % 2 == 1) {
        return childElements()[position/2];
    } else {
        return 0;
    }
}


QPainterPath FixedElement::selectionRegion(const int pos1, const int pos2) const 
{
    QPainterPath temp;
    foreach (BasicElement *child, elementsBetween(pos1, pos2)) {
        temp.addRect(child->absoluteBoundingRect());
    }
    return temp;
}





