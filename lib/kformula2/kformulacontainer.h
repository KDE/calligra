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

#include <qclipboard.h>
#include <qlist.h>
#include <qobject.h>
#include <qstack.h>

#include <kcommand.h>

#include "artwork.h"
#include "basicelement.h"
#include "elementindex.h"
#include "contextstyle.h"


class BasicElement;
class FormulaCursor;
class FormulaElement;
class KCommand;
class KFormulaCommand;
class KFormulaWidget;
class QKeyEvent;
class QPainter;
class QWidget;


/**
 * The document. Provides everything to edit the formula.
 */
class KFormulaContainer : public QObject {
    Q_OBJECT

public:

    KFormulaContainer();
    ~KFormulaContainer();

    /**
     * Returns a new cursor. It points to the beginning of the
     * formula. The cursor gets no messages if the formula changes
     * so use this with care!
     */
    FormulaCursor* createCursor();

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
     * This function return all the data contained in
     * the formula.
     * @return the content of the formula as a
     * QDomDocument.
     */ 
    QDomDocument domData();
    
    /**
     * Save function.
     * Save the formula into the specified file.
     * It overwrite the file if exists.
     */
    void save(QString file);

    /**
     * Load function.
     * Load the formula from the specified file.
     */
    void load(QString file);

    FormulaCursor* getActiveCursor() { return activeCursor; }

    /**
     * Sets the cursor that is to be used for any editing.
     * It's the views responsibility to set its cursor
     * before it triggers an action.
     * Normally a view sets the active cursor if it gets the focus.
     */
    void setActiveCursor(FormulaCursor* cursor) { activeCursor = cursor; }

    /**
     * Emits a formulaChanged signal if we are dirty.
     */
    QRect boundingRect();

    
signals:

    /**
     * The formula has changed and needs to be redrawn.
     */
    void formulaChanged();

    /**
     * The element is going to leave the formula with and all its children.
     */
    void elementWillVanish(BasicElement* element);

    /**
     * Tells the cursors that we have just loaded a new formula.
     */
    void formulaLoaded(FormulaElement*);
    

public slots:    

    // There are a lot of thing we can do with the formula.
    
    void addText(QChar ch);
    void addNumber(QChar ch);
    void addOperator(QChar ch);

    void addBracket(char left, char right);
    void addSquareBracket() { addBracket('[', ']'); }
    void addCurlyBracket()  { addBracket('{', '}'); }
    void addLineBracket()   { addBracket('|', '|'); }
    void addCornerBracket() { addBracket('<', '>'); }
    void addRoundBracket()  { addBracket('(', ')'); }
    
    void addFraction();
    void addRoot();

    void addSymbol(Artwork::SymbolType type);
    void addIntegral() { addSymbol(Artwork::Integral); }
    void addProduct()  { addSymbol(Artwork::Product); }
    void addSum()      { addSymbol(Artwork::Sum); }

    void addMatrix(int rows, int columns);

    void addLowerLeftIndex();
    void addUpperLeftIndex();
    void addLowerRightIndex();
    void addUpperRightIndex();

    void addGenericLowerIndex();
    void addGenericUpperIndex();
    
    void remove(BasicElement::Direction = BasicElement::beforeCursor);
    void removeForward() { remove(BasicElement::afterCursor); }
    
    void replaceElementWithMainChild(BasicElement::Direction = BasicElement::beforeCursor);

    /**
     * Undo and move the undone command to the redo stack
     */
    void undo();

    /**
     * Redo and move the redone command to the undo stack
     */
    void redo();

    /**
     * Insert data from the clipboard.
     */
    void paste();

    /**
     * Copy the current selection to the clipboard.
     */
    void copy();

    /**
     * Copy and remove.
     */
    void cut();

private:

    /**
     * Emits a formulaChanged signal if we are dirty.
     */
    void testDirty();

    void addGenericIndex(FormulaCursor* cursor, ElementIndexPtr index);

    /**
     * Execute the command if it makes sense.
     */
    void execute(KFormulaCommand *command);

    /**
     * Remove the selection if any.
     */
    void removeSelection();

    
    /**
     * The element tree's root.
     */
    FormulaElement* rootElement;

    /**
     * The style that should be used to draw the formula.
     */
    ContextStyle context;

    /**
     * If true we need to recalc the formula.
     */
    bool dirty;

    /**
     * The active cursor is the one that triggered the last command.
     * We need to remember it so we can use the kdelib undo system.
     */
    FormulaCursor* activeCursor;
    
    /**
     * Our undo stack.
     */
    KCommandHistory history;

    // debug
    friend class TestFormulaCursor;
    friend class TestIndexElement;
    friend class TestCommands;
};


#endif // __KFORMULACONTAINER_H
