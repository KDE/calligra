/* This file is part of the KDE project
   Copyright (C) 2001 Andrea Rizzi <rizzi@kde.org>
	              Ulrich Kuettler <ulrich.kuettler@mailbox.tu-dresden.de>
		 2006 Martin Pfeiffer <hubipete@gmx.net>
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

#ifndef BRACKETELEMENT_H
#define BRACKETELEMENT_H

#include <QPoint>
#include <QSize>
#include <QList>

#include "kformuladefs.h"
#include "BasicElement.h"

class Artwork;
class RowElement;


/**
 * A left and/or right bracket around one child.
 */
class BracketElement : public BasicElement {
public:

    enum { contentPos };

    BracketElement( BasicElement* parent = 0 );
    ~BracketElement();


    /**
     * Obtain a list of all child elements of this element
     * @return a QList with pointers to all child elements
     */
    const QList<BasicElement*> childElements();
	
    /**
     * Render the element to the given QPainter
     * @param painter The QPainter to paint the element to
     * @param am AttributeManager containing style info
     */
    virtual void paint( QPainter& painter, const AttributeManager* am );

    /**
     * Calculate the size of the element and the positions of its children
     * @param am The AttributeManager providing information about attributes values
     */
    virtual void layout( const AttributeManager* am );
    
    /// @return The element's ElementType
    virtual ElementType elementType() const;
    
protected:
    
    /// Read all content from the node - reimplemented from BasicElement
    virtual bool readMathMLContent( const KoXmlElement& element );
    
    /// Write all content to the KoXmlWriter - reimplemented from BasicElement
    virtual void writeMathMLContent( KoXmlWriter* writer) const;

private:

    /**
     * Set left and right types in operator fences
     * @param open if true set SymbolType for open (left) bracket, 
     * otherwise set for close (right) bracket.
     */
    bool operatorType( QDomNode& node, bool open );

    /**
     * Search through the nodes to find the close operator to match current
     * open bracket.
     */
    int searchOperator( const QDomNode& node );

    /**
     * The brackets we are showing.
     */
    Artwork* left;
    Artwork* right;

    SymbolType leftType;
    SymbolType rightType;

    QString m_separators;
    bool m_operator;
    bool m_customLeft;
    bool m_customRight;
};

#endif // BRACKETELEMENT_H
