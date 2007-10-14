/* This file is part of the KDE project
   Copyright (C) 2001 Andrea Rizzi <rizzi@kde.org>
   Copyright (C) 2001 Ulrich Kuettler <ulrich.kuettler@mailbox.tu-dresden.de>
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

#ifndef MATRIXROWELEMENT_H
#define MATRIXROWELEMENT_H

#include "BasicElement.h"
#include "kformula_export.h"

class MatrixEntryElement;

/**
 * @short Representing the MathML mtr element.
 */
class KOFORMULA_EXPORT MatrixRowElement : public BasicElement {
public:
    /// The standard constructor
    MatrixRowElement( BasicElement* parent = 0 );

    /// The standard destructor
    ~MatrixRowElement();

    /**
     * Obtain a list of all child elements of this element
     * @return a QList with pointers to all child elements
     */
    const QList<BasicElement*> childElements();
 
    /**
     * Insert a new child at the cursor position
     * @param cursor The cursor holding the position where to inser
     * @param child A BasicElement to insert
     */
    void insertChild( FormulaCursor* cursor, BasicElement* child );
   
    /**
     * Remove a child element
     * @param element The BasicElement to remove
     */ 
    void removeChild( BasicElement* element );

    /**
     * Implement the cursor behaviour for the element
     * @param direction Indicates whether the cursor moves up, down, right or left
     * @return A this pointer if the element accepts if not the element to asked instead
     */
    BasicElement* acceptCursor( CursorDirection direction );

    /// @return The position of the given @p entry   
    int positionOfEntry( BasicElement* entry ) const;

    /// @return The MatrixEntryElement at the @p pos position in the MatrixRowElement
    MatrixEntryElement* entryAt( int pos );

    /// @return The element's ElementType
    ElementType elementType() const;

protected:
    /// Read all content from the node - reimplemented by child elements
    bool readMathMLContent( const KoXmlElement& element );

    /// Write all content to the KoXmlWriter - reimplemented by the child elements
    void writeMathMLContent( KoXmlWriter* writer ) const;

private:
    /// The list of entries in this row of the matrix 
    QList<MatrixEntryElement*> m_matrixEntryElements;
};

#endif

