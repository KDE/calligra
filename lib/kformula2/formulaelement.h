/* This file is part of the KDE project
   Copyright (C) 2001 Andrea Rizzi <rizzi@kde.org>
	              Ulrich Kuettler <ulrich.kuettler@mailbox.tu-dresden.de>

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
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#ifndef __FORMULAELEMENT_H
#define __FORMULAELEMENT_H

// Formula include
#include "sequenceelement.h"

class BasicElement;
class ContextStyle;
class KFormulaContainer;
class SymbolTable;


/**
 * The main element.
 * A formula consists of a FormulaElement and its children.
 * The only element that has no parent.
 */
class FormulaElement : public SequenceElement {
    typedef SequenceElement inherited;
public:

    FormulaElement(KFormulaContainer* container);

    /**
     * Returns the element the point is in.
     */
    BasicElement* goToPos(FormulaCursor*, const QPoint& point);

    /**
     * Provide fast access to the rootElement for each child.
     */
    virtual FormulaElement* formula() { return this; }

    /**
     * Gets called just before the child is removed from
     * the element tree.
     */
    void elementRemoval(BasicElement* child);

    /**
     * Gets called whenever something changes and we need to
     * recalc.
     */
    void changed();

    /**
     * Calculates the formulas sizes and positions.
     */
    void calcSizes(ContextStyle& context);
    
    /**
     * Draws the whole thing.
     */
    void draw(QPainter& painter, ContextStyle& context);

    /**
     * @returns our documents symbol table
     */
    const SymbolTable& getSymbolTable() const;
    
protected:

    //Save/load support
    
    /**
     * Returns the tag name of this element type.
     */
    virtual QString getTagName() const { return "FORMULA"; }
    
    /**
     * Appends our attributes to the dom element.
     */
    virtual void writeDom(QDomElement& element);
    
    /**
     * Reads our attributes from the element.
     * Returns false if it failed.
     */
    virtual bool readAttributesFromDom(QDomElement& element);

    /**
     * Reads our content from the node. Sets the node to the next node
     * that needs to be read.
     * Returns false if it failed.
     */
    virtual bool readContentFromDom(QDomNode& node);
    
private:

    /**
     * The document that owns this formula.
     */
    KFormulaContainer* document;

    /**
     * The root element has to know its size.
     */
    int size;
};

#endif // __FORMULAELEMENT_H
