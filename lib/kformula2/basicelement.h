/* This file is part of the KDE project
   Copyright (C) 2001 Andrea Rizzi <rizzi@kde.org>
	              Ulrich Kuettler <ulrich.kuettler@mailbox.tu-dresden.de>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#ifndef BASICELEMENT_H
#define BASICELEMENT_H

#include <iostream>

// Qt Include
#include <qlist.h>
#include <qrect.h>
#include <qstring.h>
#include <qdom.h>

// KDE Include


// Formula include
#include "contextstyle.h"
#include "formuladefs.h"

class ComplexElement;
class ElementType;
class FormulaCursor;
class FormulaElement;
class SequenceElement;


/**
 * Basis of every formula element. An element is used basically by
 * other elements and by the @ref FormulaCursor .
 *
 * Each element knows its size (a rect that includes all children)
 * and how to draw itself. See @ref calcSizes and @ref draw .
 *
 * An element might contain valid cursor position. If the cursor
 * enters the element it must find the next valid position
 * depending on the direction in that the cursor moves and the
 * element it comes from. There might also be some flags inside the
 * cursor that tell how it wants to move. See @ref moveLeft ,
 * @ref moveRight , @ref moveUp , @ref moveDown .
 *
 * To build a tree an element must own children. If there are children
 * there must be a main child. This is the child that might be used to
 * replace the element. See @ref getMainChild and @ref setMainChild .
 *
 * If there can be children you might want to @ref insert and @ref remove
 * them. After a removal the element might be senseless.
 * (See @ref isSenseless )
 * If it is it must be removed.
 */
class BasicElement
{
    friend class SequenceElement;
    friend class SequenceParser;

public:

    /*
     * Each element might contain children. Each child needs
     * its own unique number. It is not guaranteed, however,
     * that the number stays the same all the time.
     * (The SequenceElement's children are simply counted.)
     */

    BasicElement(BasicElement* parent = 0);
    virtual ~BasicElement();

    /**
     * Provide fast access to the rootElement for each child.
     */
    virtual FormulaElement* formula() { return parent->formula(); }

    /**
     * @returns the character that represents this element. Used for
     * parsing a sequence.
     */
    virtual QChar getCharacter() const { return QChar::null; }

    /**
     * @returns the type of this element. Used for
     * parsing a sequence.
     */
    virtual TokenType getTokenType() const { return ELEMENT; }

    /**
     * @returns true if we don't want to see the element.
     */
    virtual bool isPhantom() const { return false; }

    /**
     * Sets the cursor and returns the element the point is in.
     * The handled flag shows whether the cursor has been set.
     * This is needed because only the innermost matching element
     * is allowed to set the cursor.
     */
    virtual BasicElement* goToPos(FormulaCursor*, bool& handled,
                                  const QPoint& point, const QPoint& parentOrigin);

    /**
     * Returns our position inside the widget.
     */
    QPoint widgetPos();


    // drawing
    //
    // Drawing depends on a context which knows the required properties like
    // fonts, spaces and such.
    // It is essential to calculate elements size with the same context
    // before you draw.

    /**
     * Calculates our width and height and
     * our children's parentPosition.
     */
    virtual void calcSizes(const ContextStyle& context, ContextStyle::TextStyle tstyle, ContextStyle::IndexStyle istyle) = 0;

    /**
     * Draws the whole element including its children.
     * The `parentOrigin' is the point this element's parent starts.
     * We can use our parentPosition to get our own origin then.
     */
    virtual void draw(QPainter& painter, const QRect& r,
                      const ContextStyle& context,
		      ContextStyle::TextStyle tstyle,
		      ContextStyle::IndexStyle istyle,
		      const QPoint& parentOrigin) = 0;


    // navigation
    //
    // The elements are responsible to handle cursor movement themselves.
    // To do this they need to know the direction the cursor moves and
    // the element it comes from.
    //
    // The cursor might be in normal or in selection mode.

    /**
     * Enters this element while moving to the left starting inside
     * the element `from'. Searches for a cursor position inside
     * this element or to the left of it.
     */
    virtual void moveLeft(FormulaCursor* cursor, BasicElement* from);

    /**
     * Enters this element while moving to the right starting inside
     * the element `from'. Searches for a cursor position inside
     * this element or to the right of it.
     */
    virtual void moveRight(FormulaCursor* cursor, BasicElement* from);

    /**
     * Enters this element while moving up starting inside
     * the element `from'. Searches for a cursor position inside
     * this element or above it.
     */
    virtual void moveUp(FormulaCursor*, BasicElement*) {}

    /**
     * Enters this element while moving down starting inside
     * the element `from'. Searches for a cursor position inside
     * this element or below it.
     */
    virtual void moveDown(FormulaCursor*, BasicElement* ) {}

    /**
     * Moves the cursor to the first position in this sequence.
     * (That is before the first child.)
     */
    virtual void moveHome(FormulaCursor*) {}

    /**
     * Moves the cursor to the last position in this sequence.
     * (That is behind the last child.)
     */
    virtual void moveEnd(FormulaCursor*) {}

    /**
     * Sets the cursor inside this element to its start position.
     * For most elements that is the main child.
     */
    virtual void goInside(FormulaCursor* cursor);


    // children

    /**
     * Removes the child. If this was the main child this element might
     * request its own removal.
     * The cursor is the one that caused the removal. It has to be moved
     * to the place any user expects the cursor after that particular
     * element has been removed.
     */
    //virtual void removeChild(FormulaCursor* cursor, BasicElement* child) {}

    
    // main child
    //
    // If an element has children one has to become the main one.
    
    virtual SequenceElement* getMainChild() { return 0; }
    //virtual void setMainChild(SequenceElement*) {}
    

    // editing
    //
    // Insert and remove children.

    /**
     * Wether we want to insert to the left of the cursor
     * or right of it.
     * The same for deletion.
     */
    enum Direction { beforeCursor, afterCursor };

    /**
     * Inserts all new children at the cursor position. Places the
     * cursor according to the direction.
     *
     * The list will be emptied but stays the property of the caller.
     */
    virtual void insert(FormulaCursor*, QList<BasicElement>&, Direction) {}

    /**
     * Removes all selected children and returns them. Places the
     * cursor to where the children have been.
     */
    virtual void remove(FormulaCursor*, QList<BasicElement>&, Direction) {}

    /**
     * Moves the cursor to a normal place where new elements
     * might be inserted.
     */
    virtual void normalize(FormulaCursor*, Direction);


    /**
     * Returns wether the element has no more useful
     * children (except its main child) and should therefore
     * be replaced by its main child's content.
     */
    virtual bool isSenseless() { return false; }

    /**
     * Returns the child at the cursor.
     */
    virtual BasicElement* getChild(FormulaCursor*, Direction = beforeCursor) { return 0; }


    /**
     * Sets the cursor to select the child. The mark is placed before,
     * the position behind it.
     */
    virtual void selectChild(FormulaCursor*, BasicElement*) {}


    // basic support

    BasicElement* getParent() { return parent; }
    void setParent(BasicElement* p) { parent = p; }

    int getX() const { return position.x(); }
    int getY() const { return position.y(); }

    void setX(int x) { position.setX(x); }
    void setY(int y) { position.setY(y); }

    //QSize getSize() { return size; }

    int getWidth() const { return size.width(); }
    int getHeight() const { return size.height(); }

    /**
     * Our position inside our parent.
     */
    //QPoint getParentPosition() { return position; }

    int getBaseline() const { return baseline; }
    int getMidline() const { return midline; }


    /**
     * @return a QDomElement that contain as DomChildren the
     * children, and as attribute the attribute of this
     * element.
     */
    QDomElement getElementDom(QDomDocument& doc);

    /**
     * Set this element attribute, build children and
     * call their buildFromDom.
     */
    bool buildFromDom(QDomElement& element);

    // debug
    static int getEvilDestructionCount() { return evilDestructionCount; }

protected:

    QSize& getSize() { return size; }

    void setWidth(int width)   { size.setWidth(width); }
    void setHeight(int height) { size.setHeight(height); }

    void setBaseline(int line) { baseline = line; }
    void setMidline(int mline) { midline = mline; }

    /**
     * Calculates the base line. This is used by all elements
     * those main child determines the position of the elements
     * base line.
     */
    void calcBaseline();

    //Save/load support

    /**
     * Returns the tag name of this element type.
     */
    virtual QString getTagName() const { return "BASIC"; }

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


    /**
     * Returns a SequenceElement constructed from the nodes first child
     * if the nodes name matches the given name.
     *
     * This is a service for all subclasses that contain children.
     */
    SequenceElement* buildChild(QDomNode& node, QString name);

    /**
     * @returns our type. This is an object from our parent's syntax tree
     * or 0 if there was a very bad parsing error.
     */
    ElementType* getElementType() { return elementType; }


    /**
     * @returns the latex representation of the element and
     * of the element's children
     */
    virtual QString toLatex();


private:

    /**
     * Sets a new type. This is done during parsing.
     */
    void setElementType(ElementType* t) { elementType = t; }

    /**
     * Our parent.
     * The parent might not be null except for the FormulaElement
     * that is the top of the element tree.
     */
    BasicElement* parent;

    /**
     * This elements size.
     */
    QSize size;

    /**
     * Our position relative to our parent.
     */
    QPoint position;

    /**
     * The position of our base line from
     * the upper border. A sequence aligns its elements
     * along this line.
     *
     * There are elements (like matrix) that don't have a base line. It is
     * -1 in this case. The alignment is done using the middle line.
     */
    int baseline;

    /**
     * The position of our middle line from
     * the upper border. The strike out position.
     *
     * This will have to go.
     */
    int midline;

    /**
     * The token that describes our type. Please note that we don't
     * own it.
     */
    ElementType* elementType;

    // debug
    static int evilDestructionCount;

    // copying not allowed.
    BasicElement(BasicElement&) {}
    BasicElement& operator= (BasicElement&) { return *this; }
};

#endif // BASICELEMENT_H
