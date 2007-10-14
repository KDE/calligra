/* This file is part of the KDE project
   Copyright (C) 2001 Andrea Rizzi <rizzi@kde.org>
	              Ulrich Kuettler <ulrich.kuettler@mailbox.tu-dresden.de>
		 2006 Martin Pfeiffer <hubipete@gmx.net>

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

#ifndef MATRIXELEMENT_H
#define MATRIXELEMENT_H

#include "BasicElement.h"
#include "kformula_export.h"
#include <QPainterPath>

class MatrixRowElement;
class MatrixEntryElement;
	
/**
 * @short A matrix or table element in a formula
 *
 * A matrix element contains a list of rows which are of class MatrixRowElement.
 * These rows contain single entries which are of class MatrixEntryElement. The
 * MatrixElement takes care that the different MatrixRowElements are informed how
 * to lay out their children correctly as they need to be synced.
 */
class KOFORMULA_EXPORT MatrixElement : public BasicElement {
public:
    /// The standard constructor
    MatrixElement( BasicElement* parent = 0 );
    
    /// The standard destructor
    ~MatrixElement();

    /**
     * Render the element to the given QPainter
     * @param painter The QPainter to paint the element to
     * @param am AttributeManager containing style info
     */
    void paint( QPainter& painter, AttributeManager* am );

    /**
     * Calculate the size of the element and the positions of its children
     * @param am The AttributeManager providing information about attributes values
     */
    void layout( const AttributeManager* am );

    /**
     * Obtain a list of all child elements of this element
     * @return a QList with pointers to all child elements
     */
    const QList<BasicElement*> childElements();

    /**
     * Implement the cursor behaviour for the element
     * @param direction Indicates whether the cursor moves up, down, right or left
     * @return A this pointer if the element accepts if not the element to asked instead
     */
    BasicElement* acceptCursor( FormulaCursor* cursor );

    /// @return The default value of the attribute for this element
    QString attributesDefaultValue( const QString& attribute ) const;

protected:
    /// Read all content from the node - reimplemented by child elements
    bool readMathMLContent( const KoXmlElement& element );

    /// Write all content to the KoXmlWriter - reimplemented by the child elements
    void writeMathMLContent( KoXmlWriter* writer ) const;

private:
    /// @return The base line computed out of the align attribute
    double parseTableAlign() const;

    Qt::PenStyle parsePenStyle( const QString& value ) const;

    /// @return The index of @p row in m_matrixRowElements
    int indexOfRow( BasicElement* row ) const;
    
    /// The rows a matrix contains
    QList<MatrixRowElement*> m_matrixRowElements;

    /// Path to store borders and grid to be painted
    QPainterPath m_matrixPath;

    /// Buffer for the pen style used for the table's frame
    Qt::PenStyle m_framePenStyle;
};

#endif // MATRIXELEMENT_H
