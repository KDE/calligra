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

#ifndef EMPTYELEMENT_H
#define EMPTYELEMENT_H

#include "BasicElement.h"
#include "kformula_export.h"

class FormulaCursor;
class QPainterPath;

/**
 * @short Represents a placeholder element
 *
 */

class KOFORMULA_EXPORT EmptyElement : public BasicElement {
public:
    /// The standard constructor
    EmptyElement( BasicElement* parent = 0 );

    /// The standard destructor
    virtual ~EmptyElement();
    
    ///inherited from BasicElement
    virtual void paint ( QPainter& painter, AttributeManager* am );
    
    ///inherited from BasicElement
    virtual ElementType elementType() const;
    
    ///inherited from BasicElement
    virtual QLineF cursorLine ( int position ) const;
};

#endif // ROWELEMENT_H

