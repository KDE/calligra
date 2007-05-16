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

#ifndef GLYPHELEMENT_H
#define GLYPHELEMENT_H

#include "BasicElement.h"
#include <QPainterPath>

class GlyphElement : public BasicElement {
public:
    /// The standart constructor
    GlyphElement( BasicElement* parent = 0 );

    /**
     * Render the element to the given QPainter
     * @param painter The QPainter to paint the element to
     */
    void paint( QPainter& painter, const AttributeManager* am );

    /**
     * Calculate the size of the element and the positions of its children
     * @param am The AttributeManager providing information about attributes values
     */
    void layout( const AttributeManager* am );

    /// @return The element's ElementType
    ElementType elementType() const;

private:
    /// The content as QPainterPath - ready to paint
    QPainterPath m_glyphPath;
};

#endif // GLYPHELEMENT_H
