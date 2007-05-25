/* This file is part of the KDE project
   Copyright (C) 2006 Martin Pfeiffer <hubipete@gmx.net>
   Copyright (C) 2006 Alfredo Beaumont Sainz <alfredo.beaumont@gmail.com>

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

#ifndef MULTISCRIPTELEMENT_H
#define MULTISCRIPTELEMENT_H

#include "BasicElement.h"
#include "kformula_export.h"

/**
 * @short Implementation of the msub, msup, msubsup and mmultiscript element
 */
class KOFORMULA_EXPORT MultiscriptElement : public BasicElement {
public:
    /// The standard constructor
    MultiscriptElement( BasicElement* parent = 0 );

    /// The destructor
    ~MultiscriptElement();

    /**
     * Obtain a list of all child elements of this element
     * @return a QList with pointers to all child elements
     */
    const QList<BasicElement*> childElements();

    
private:
    /// The BasicElement representing the base element of the multiscript
    BasicElement* m_baseElement;

    /// The BasicElement representing the subscript left to the base element
    BasicElement* m_preSubscript;

    /// The BasicElement representing the superscript left to the base element
    BasicElement* m_preSuperscript;

    /// The BasicElement representing the subscript right to the base element
    BasicElement* m_postSubscript;

    /// The BasicElement representing the superscript right to the base element
    BasicElement* m_postSuperscript;


    virtual QString elementName() const { return "mmultiscript"; }
    virtual void writeMathMLContent( KoXmlWriter* writer ) const;
};

#endif // MULTISCRIPTELEMENT_H
