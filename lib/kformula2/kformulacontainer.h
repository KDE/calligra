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

#ifndef __KFORMULACONTAINER_H
#define __KFORMULACONTAINER_H

#include <qobject.h>

#include "basicelement.h"
#include "contextstyle.h"
#include "formulaelement.h"

class BasicElement;
class FormulaCursor;
class QKeyEvent;
class QPainter;


class KFormulaContainer : public QObject {
    Q_OBJECT

public:

    KFormulaContainer();
    ~KFormulaContainer();

    /**
     * Returns a new cursor. It points to the beginning of the
     * formula.
     */
    FormulaCursor* createCursor();

    /**
     * Destroys the cursor. It must not be used afterwards.
     */
    void destroyCursor(FormulaCursor*);

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
     * Draws the whole thing.
     */
    void draw(QPainter& painter);

    /**
     * One of our view got a key.
     */
    void keyPressEvent(FormulaCursor* cursor, QKeyEvent* event);

    
signals:

    /**
     * The formula has changed and needs to be redrawn.
     */
    void formulaChanged();

    /**
     * The element is going to leave the formula with and all its children.
     */
    void elementWillVanish(BasicElement* element);

    
public slots:    

    // There a lot of thing we can do with the formula.
    
    void addText(FormulaCursor* cursor, QChar ch);
    void addOperator(FormulaCursor* cursor, QChar ch);
    void addBracket(FormulaCursor* cursor, char left, char right);
    void addLowerRightIndex(FormulaCursor* cursor);
    void addUpperRightIndex(FormulaCursor* cursor);

    void removeSelection(FormulaCursor* cursor, BasicElement::Direction);

private:
    
    /**
     * Emits a formulaChanged signal if we are dirty.
     */
    void testDirty();
    
    /**
     * The element tree's root.
     */
    FormulaElement rootElement;

    /**
     * The style that should be used to draw the formula.
     */
    ContextStyle context;

    /**
     * If true we need to recalc the formula.
     */
    bool dirty;
};

#endif // __KFORMULACONTAINER_H
