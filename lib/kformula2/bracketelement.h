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

#ifndef __BRACKETELEMENT_H
#define __BRACKETELEMENT_H

#include <qpoint.h>
#include <qsize.h>

#include "basicelement.h"

class SequenceElement;


/**
 * A left and/or right bracket around one child.
 */
class BracketElement : public BasicElement {
public:

    enum { contentPos };

    BracketElement(char left, char right, BasicElement* parent = 0);
    ~BracketElement();

    /**
     * Calculates our width and height and
     * our children's parentPosition.
     */
    virtual void calcSizes(ContextStyle& style, int parentSize);

    /**
     * Draws the whole element including its children.
     * The `parentOrigin' is the point this element's parent starts.
     * We can use our parentPosition to get our own origin then.
     */
    virtual void draw(QPainter& painter, ContextStyle& style,
                      int parentSize, const QPoint& parentOrigin);

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
    virtual void moveUp(FormulaCursor* cursor, BasicElement* from);

    /**
     * Enters this element while moving down starting inside
     * the element `from'. Searches for a cursor position inside
     * this element or below it.
     */
    virtual void moveDown(FormulaCursor* cursor, BasicElement* from);

    
    /**
     * Removes all selected children and returns them. Places the
     * cursor to where the children have been.
     *
     * Here we remove ourselve if we are requested to remove our content.
     */
    virtual void remove(FormulaCursor*, QList<BasicElement>&, Direction);

    
    // main child
    //
    // If an element has children one has to become the main one.
    
    virtual SequenceElement* getMainChild();
    virtual void setMainChild(SequenceElement*);

    /**
     * Sets the cursor to select the child. The mark is placed before,
     * the position behind it.
     */
    virtual void selectChild(FormulaCursor* cursor, BasicElement* child);
    
private:

    class Bracket {
    public:
        virtual void calcSizes(ContextStyle& style, int parentSize,
                               int contentHeight, bool right) = 0;
        virtual void draw(QPainter& painter, ContextStyle& style,
                          int parentSize, const QPoint& origin, bool right) = 0;

        int getWidth() const { return size.width(); }
        int getHeight() const { return size.height(); }

        void setX(int x) { point.setX(x); }
        void setY(int y) { point.setY(y); }
        
    protected:
        QSize size;
        QPoint point;
    };

    // '(' and ')'
    class RoundBracket : public Bracket {
        virtual void calcSizes(ContextStyle& style, int parentSize,
                               int contentHeight, bool right);
        virtual void draw(QPainter& painter, ContextStyle& style,
                          int parentSize, const QPoint& origin, bool right);
    };

    // '[' and ']'
    class SquareBracket : public Bracket {
        virtual void calcSizes(ContextStyle& style, int parentSize,
                               int contentHeight, bool right);
        virtual void draw(QPainter& painter, ContextStyle& style,
                          int parentSize, const QPoint& origin, bool right);
    };

    // '{' and '}'
    class CurlyBracket : public Bracket {
        virtual void calcSizes(ContextStyle& style, int parentSize,
                               int contentHeight, bool right);
        virtual void draw(QPainter& painter, ContextStyle& style,
                          int parentSize, const QPoint& origin, bool right);
    };

    // '|'
    class LineBracket : public Bracket {
        virtual void calcSizes(ContextStyle& style, int parentSize,
                               int contentHeight, bool right);
        virtual void draw(QPainter& painter, ContextStyle& style,
                          int parentSize, const QPoint& origin, bool right);
    };

    // '<' and '>'
    class CornerBracket : public Bracket {
        virtual void calcSizes(ContextStyle& style, int parentSize,
                               int contentHeight, bool right);
        virtual void draw(QPainter& painter, ContextStyle& style,
                          int parentSize, const QPoint& origin, bool right);
    };

    // everything else
    class EmptyBracket : public Bracket {
        virtual void calcSizes(ContextStyle&, int, int, bool) {}
        virtual void draw(QPainter&, ContextStyle&, int, const QPoint&, bool) {}
    };

    
    /**
     * Creates a new bracket object that matches the char.
     */
    Bracket* createBracket(char bracket);

    
    /**
     * The brackets we are showing.
     */
    Bracket* left;
    Bracket* right;

    /**
     * Our main child.
     */
    SequenceElement* content;
};


#endif // __BRACKETELEMENT_H
