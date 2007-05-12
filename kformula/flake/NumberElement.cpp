/* This file is part of the KDE project
   Copyright (C) 2006-2007 Alfredo Beaumont Sainz <alfredo.beaumont@gmail.com>
                 2007      Martin Pfeiffer <hubipete@gmx.net>

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

#include "NumberElement.h"
#include "ElementFactory.h"

namespace FormulaShape {

NumberElement::NumberElement( BasicElement* parent ) : BasicElement( parent )
{
}

void NumberElement::paint( QPainter& painter, const AttributeManager* am )
{
}
   
void NumberElement::layout( const AttributeManager* am )
{
    // 
}
    
void NumberElement::moveLeft( FormulaCursor* cursor, BasicElement* from )
{
}
    /**
     * Move the FormulaCursor right 
     * @param cursor The FormulaCursor to be moved
     * @param from The BasicElement which was the last owner of the FormulaCursor
     */
    void moveRight( FormulaCursor* cursor, BasicElement* from );

    /**
     * Move the FormulaCursor up 
     * @param cursor The FormulaCursor to be moved
     * @param from The BasicElement which was the last owner of the FormulaCursor
     */
    void moveUp( FormulaCursor* cursor, BasicElement* from );

    /**
     * Move the FormulaCursor down 
     * @param cursor The FormulaCursor to be moved
     * @param from The BasicElement which was the last owner of the FormulaCursor
     */
    void moveDown( FormulaCursor* cursor, BasicElement* from );

    /// @return The element's ElementType
    ElementType elementType() const;

} // namespace FormulaShape
