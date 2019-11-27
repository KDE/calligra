/* This file is part of the KDE project
   Copyright (C) 2001 Andrea Rizzi <rizzi@kde.org>
                      Ulrich Kuettler <ulrich.kuettler@mailbox.tu-dresden.de>
                 2006 Martin Pfeiffer <hubipete@gmx.net>
                 2009 Jeremias Epperlein <jeeree@web.de>

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

#ifndef ROWELEMENT_H
#define ROWELEMENT_H

#include "BasicElement.h"
#include "koformula_export.h"

class FormulaCursor;

/**
 * @short Implementation of the MathML mrow element
 *
 * The mrow element is specified in the MathML spec section 3.3.1. It primarily
 * serves as container for other elements that are grouped together and aligned
 * in a row.
 * mrow has no own visual representation that is why the paint() method is empty.
 * The handling of background and other global attributes is done generically
 * inside FormulaRenderer.
 * The layout() method implements the layouting and size calculation for the mrow
 * element. The calculations assume that spacing is done per element and therefore
 * do not handle it.
 * At the moment there is no linebreaking implementation in RowElement.
 */
class KOFORMULA_EXPORT RowElement : public BasicElement {
public:
    /// The standard constructor
    explicit RowElement(BasicElement *parent = 0);

    /// The standard destructor
    ~RowElement() override;

    /**
     * Calculate the size of the element and the positions of its children
     * @param am The AttributeManager providing information about attributes values
     */
    void layout( const AttributeManager* am ) override;

    /**
     * Render the element to the given QPainter
     * @param painter The QPainter to paint the element to
     * @param am AttributeManager containing style info
     */
    void paint( QPainter& painter, AttributeManager* am ) override;
    
    /// inherited from BasicElement
    void paintEditingHints ( QPainter& painter, AttributeManager* am ) override;
    
    /**
     * Obtain a list of all child elements of this element
     * @return a QList with pointers to all child elements
     */
    const QList<BasicElement*> childElements() const override;

    /// inherited from BasicElement
    QList< BasicElement* > elementsBetween ( int pos1, int pos2 ) const override;

    /// inherited from BasicElement
    virtual bool insertChild( int position, BasicElement* child );

    /**
     * Remove a child element
     * @param child The BasicElement to remove
     */
    bool removeChild( BasicElement* child);

    ///inherited form BasicElement
    bool replaceChild ( BasicElement* oldelement, BasicElement* newelement ) override;

    /**
     * Implement the cursor behaviour for the element
     * @param cursor the cursor
     * @return A this pointer if the element accepts if not the element to asked instead
     */
    bool acceptCursor( const FormulaCursor& cursor ) override;

    /// inherited from BasicElement
    bool moveCursor(FormulaCursor& newcursor, FormulaCursor& oldcursor) override;

    /// inherited from BasicElement
    bool setCursorTo(FormulaCursor& cursor, QPointF point) override;

    /// @return The element's ElementType
    ElementType elementType() const override;

    /** Reimplemented from parent class
     *
     *  This stretches the children inside, then readjusts their vertical offsets
     */
    void stretch() override;

    /// inherited from BasicElement
    int endPosition() const override;

    /// inherited from BasicElement
    int positionOfChild(BasicElement* child) const override;

    /// inherited from BasicElement
    QLineF cursorLine(int position) const override;

    /// inherited from BasicElement
    BasicElement* elementAfter ( int position ) const override;

    /// inherited from BasicElement
    BasicElement* elementBefore ( int position ) const override;

    /// inherited from BasicElement
    bool isEmpty() const override;

    /// inherited from Basic
    bool isInferredRow() const override;

protected:
    /// Read contents of the row element
    bool readMathMLContent( const KoXmlElement& parent ) override;

    /// Write all content to the KoXmlWriter - reimplemented by the child elements
    void writeMathMLContent( KoXmlWriter* writer, const QString& ns ) const override;

    /// A list of the child elements
    QList<BasicElement*> m_childElements;
};

#endif // ROWELEMENT_H

