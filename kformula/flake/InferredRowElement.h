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

#ifndef INFERREDROWELEMENT_H
#define INFERREDROWELEMENT_H

#include "RowElement.h"

/**
 * @short Implementation of an inferred mrow element
 *
 * Some elements are allowed to have a variable number of children.
 * In those cases, elements are supposed to work as if they effectively
 * had an mrow element and those children were children of that inferred
 * mrow element. This class is an specialization of RowElement that handles
 * inferred mrows.
 * Thus, if an element allows inferred mrows should inherit from
 * InferredRowElement instead of RowElement.
 */
class InferredRowElement : public RowElement {
    typedef RowElement inherited;
public:
    /// The standard constructor
    InferredRowElement( BasicElement* parent = 0 );

protected:
    /// Read the element contents from MathML
    virtual bool readMathMLContent( const KoXmlElement& parent );
};
    
#endif // INFERREDROWELEMENT_H
