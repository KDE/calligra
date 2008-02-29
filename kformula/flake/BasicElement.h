/* This file is part of the KDE project
   Copyright (C) 2001 Andrea Rizzi <rizzi@kde.org>
	              Ulrich Kuettler <ulrich.kuettler@mailbox.tu-dresden.de>
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

#ifndef BASICELEMENT_H
#define BASICELEMENT_H

#include "kformula_export.h"
#include "ElementFactory.h"

#include <QHash>
#include <QList>
#include <QString>
#include <QRectF>
class QPainter;
class QVariant;
class KoXmlWriter;
class KoXmlElement;
class AttributeManager;
class FormulaCursor;

#define DEBUGID 40000

/**
 * @short The base class for all elements of a formula
 *
 * The BasicElement class is constructed with a parent and normally an element in a
 * formula has a parent. The only exception is FormulaElement which is the root of
 * the element tree and has no parent element.
 * Most of the elements have children but the number of it can be fixed or variable
 * and the type of child element is not certain. So with the childElements() method you
 * can obtain a list of all direct children of an element. Note that the returned list
 * can be empty when the element is eg a token. This is also the reason why each class
 * inheriting BasicElement has to implement the childElements() method on its own.
 * With the childElementAt method you can test if the given point is in the element.
 * This method is generically implemented for all element types only once in
 * BasicElement.
 * The BasicElement knows its size and position in the formula. This data is normally
 * only used for drawing and stored in the m_boundingRect attribute.
 * To adapt both variables, size and coordinates, to fit in the formula each and every
 * BasicElement derived class has to implement layoutElement() and calculateSize()
 * methods. The former adaptes the position, means the coordinates, when the formula
 * changes and the latter calculates the size of the element. After a formula change
 * first calculateSize is called for all elements then layoutElement().
 */
class KOFORMULA_EXPORT BasicElement {
public:
    /*
     * The standard constructor
     * @param parent pointer to the BasicElement's parent
     */
    BasicElement( BasicElement* parent = 0 );

    /// The standard destructor
    virtual ~BasicElement();

    /**
     * Get the element of the formula at the given point
     * @param p the point to look for 
     * @return a pointer to a BasicElement
     */
    BasicElement* childElementAt( const QPointF& p );

    /**
     * Obtain a list of all child elements of this element - sorted in saving order
     * @return a QList with pointers to all child elements
     */
    virtual const QList<BasicElement*> childElements();

    /**
     * Insert a new child at the cursor position
     * @param cursor The cursor holding the position where to inser
     * @param child A BasicElement to insert
     */
    virtual void insertChild( FormulaCursor* cursor, BasicElement* child );
   
    /**
     * Remove a child element
     * @param element The BasicElement to remove
     */ 
    virtual void removeChild( BasicElement* element );

    /**
     * Render the element to the given QPainter
     * @param painter The QPainter to paint the element to
     * @param am AttributeManager containing style info
     */
    virtual void paint( QPainter& painter, AttributeManager* am );

    /**
     * Calculate the size of the element and the positions of its children
     * @param am The AttributeManager providing information about attributes values
     */
    virtual void layout( const AttributeManager* am );

    /**
     * Implement the cursor behaviour for the element
     * @param cursor The FormulaCursor that is moved around
     * @return A this pointer if the element accepts if not the element to asked instead
     */
    virtual BasicElement* acceptCursor( const FormulaCursor* cursor );

    /// @return The element's ElementType
    virtual ElementType elementType() const;

    /// Set the element's width to @p width
    void setWidth( double width );

    /// @return The width of the element
    double width() const;

    /// Set the element's height to @p height
    void setHeight( double height );

    /// @return The height of the element
    double height() const;
    
    /// @return The bounding rectangle of the element
    const QRectF& boundingRect() const;

    /// Set the element's baseline to @p baseLine
    void setBaseLine( double baseLine );

    /// @return The baseline of the element
    double baseLine() const;

    /// Set the element's origin inside the m_parentElement to @p origin
    void setOrigin( QPointF origin );

    /// @return The element's origin 
    QPointF origin() const;

    /// Set the element's m_parentElement to @p parent
    void setParentElement( BasicElement* parent );

    /// @return The parent element of this BasicElement
    BasicElement* parentElement() const;

    /// Set the element's m_scaleFactor to @p scaleFactor
    void setScaleFactor( double scaleFactor );

    /// @return The elements scale factor
    double scaleFactor() const;

    /**
     * Set an attribute's value
     * @param name The name of the attribute to be set
     * @param value The value to set for the attribute
     */
    void setAttribute( const QString& name, const QVariant& value );

    /// @return The value of the attribute if it is set for this element
    QString attribute( const QString& attribute ) const;

    /// @return The value of the attribute if it is inherited
    virtual QString inheritsAttribute( const QString& attribute ) const;

    /// @return The default value of the attribute for this element
    virtual QString attributesDefaultValue( const QString& attribute ) const;
    
    /// Read the element from MathML
    bool readMathML( const KoXmlElement& element );

    /// Save the element to MathML 
    void writeMathML( KoXmlWriter* writer ) const;

protected:
    /// Read all attributes loaded and add them to the m_attributes map 
    void readMathMLAttributes( const KoXmlElement& element );

    /// Read all content from the node - reimplemented by child elements
    virtual bool readMathMLContent( const KoXmlElement& element );

    /// Write all attributes of m_attributes to @p writer
    void writeMathMLAttributes( KoXmlWriter* writer ) const;

    /// Write all content to the KoXmlWriter - reimplemented by the child elements
    virtual void writeMathMLContent( KoXmlWriter* writer ) const;

private:
    /// The element's parent element - might not be null except of FormulaElement
    BasicElement* m_parentElement;

    /// A hash map of all attributes where attribute name is assigned to a value
    QHash<QString,QString> m_attributes;

    /// The boundingRect storing the element's width, height, x and y
    QRectF m_boundingRect;

    /// The position of our base line from the upper border
    double m_baseLine;
   
    /// Factor with which this element is scaled down
    double m_scaleFactor;

    /// Indicates whether this element has displaystyle set
    bool m_displayStyle;
};

#endif // BASICELEMENT_H
