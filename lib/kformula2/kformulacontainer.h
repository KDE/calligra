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
#include "contextstyle.h"
#include "elementindex.h"
#include "formuladefs.h"


class BasicElement;
class FormulaCursor;
class FormulaElement;
class KCommand;
class KFormulaCommand;
class KFormulaDocument;
class KFormulaWidget;
class QKeyEvent;
class QPainter;
class QPrinter;
class QWidget;


/**
 * The document. Actually only one part of the whole.
 * Provides everything to edit the formula.
 */
class KFormulaContainer : public QObject {
    friend class KFormulaMimeSource;
    Q_OBJECT

public:

    /**
     * Construct a new formula.
     *
     * @param doc the document we belong to.
     * @param _history The undo stack we are to store our commands in.
     *                 It must not deleted as long as we exist because
     *                 we only store a reference to it.
     */
    KFormulaContainer(KFormulaDocument* doc);
    
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
     * Saves the data into the document.
     */
    void save(QDomDocument doc);

    /**
     * Load function.
     * Load the formula from the specified file containing MathML .
     */
    void loadMathMl(QString file);

    /**
     * Load function.
     * Load the formula from the specified file.
     */
    void load(QString file);

    /**
     * Loads a formula from the document.
     */
    bool load(QDomDocument doc);

    /**
     * Prints the formula.
     */
    void print(QPrinter& printer);


    /**
     * @returns the cursor to be used for editing.
     */
    FormulaCursor* getActiveCursor() { return activeCursor; }

    /**
     * Tells the formula that a view got the focus and might want to
     * edit the formula.
     */
    void setActiveView(KFormulaWidget* view);
    
    /**
     * @returns the formula's size.
     */
    QRect boundingRect();

    /**
     * Emits a formulaChanged signal if we are dirty.
     */
    void testDirty();

    /**
     * @returns true if there is no element.
     */
    bool isEmpty();

    /**
     * @returns the document this formula belongs to.
     */
    KFormulaDocument* getDocument() const { return document; }

signals:

    /**
     * The cursor has been moved but the formula hasn't been
     * changed. The view that own the cursor needs to know this.
     */
    void cursorMoved(FormulaCursor* cursor);
    
    /**
     * The formula has changed and needs to be redrawn.
     */
    void formulaChanged(int width, int height);

    /**
     * The element is going to leave the formula with and all its children.
     */
    void elementWillVanish(BasicElement* element);

    /**
     * Tells the cursors that we have just loaded a new formula.
     */
    void formulaLoaded(FormulaElement*);
    
    /**
     * A command has been executed.
     */
    void commandExecuted();
    
public slots:    

    // There are a lot of thing we can do with the formula.
    
    void addText(QChar ch);

    void addLineBreak();

    void addBracket(char left, char right);
    void addDefaultBracket();
    void addSquareBracket() { addBracket('[', ']'); }
    void addCurlyBracket()  { addBracket('{', '}'); }
    void addLineBracket()   { addBracket('|', '|'); }
    void addCornerBracket() { addBracket('<', '>'); }
    void addRoundBracket()  { addBracket('(', ')'); }
    
    void addFraction();
    void addRoot();

    void addSymbol(SymbolType type);
    void addIntegral() { addSymbol(Integral); }
    void addProduct()  { addSymbol(Product); }
    void addSum()      { addSymbol(Sum); }

    void addMatrix(int rows, int columns);

    /**
     * Asks for a matrix size and inserts it.
     */
    void addMatrix();

    /**
     * Adds the lower left index of the current IndexElement.
     * Creates and inserts an IndexElement if there is non.
     */
    void addLowerLeftIndex();

    /**
     * Adds the upper left index of the current IndexElement.
     * Creates and inserts an IndexElement if there is non.
     */
    void addUpperLeftIndex();

    /**
     * Adds the lower right index of the current IndexElement.
     * Creates and inserts an IndexElement if there is non.
     */
    void addLowerRightIndex();

    /**
     * Adds the upper right index of the current IndexElement.
     * Creates and inserts an IndexElement if there is non.
     */
    void addUpperRightIndex();

    /**
     * Adds a lower index to the current element if possible.
     */
    void addGenericLowerIndex();

    /**
     * Adds an upper index to the current element if possible.
     */
    void addGenericUpperIndex();
    
    void remove(BasicElement::Direction = BasicElement::beforeCursor);
    
    void replaceElementWithMainChild(BasicElement::Direction = BasicElement::beforeCursor);

    /**
     * Replaces the current name with its text symbol if it has one.
     */
    void compactExpression();
    
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

protected:

    KCommandHistory* getHistory() const;
    
private:

    void addGenericIndex(FormulaCursor* cursor, ElementIndexPtr index);

    /**
     * Execute the command if it makes sense.
     */
    void execute(KFormulaCommand *command);

    /**
     * Sets the cursor that is to be used for any editing.
     *
     * The active cursor might 0. In this case you must not
     * request any change from the formula.
     */
    void setActiveCursor(FormulaCursor* cursor) { activeCursor = cursor; }
    
    /**
     * @returns true if there is a cursor.
     */
    bool hasValidCursor() const { return activeCursor != 0; }

    /**
     * If true we need to recalc the formula.
     */
    bool dirty;
    
    /**
     * The element tree's root.
     */
    FormulaElement* rootElement;

    /**
     * The active cursor is the one that triggered the last command.
     */
    FormulaCursor* activeCursor;

    /**
     * The document we belong to.
     */
    KFormulaDocument* document;
    
    // debug
    friend class TestFormulaCursor;
    friend class TestIndexElement;
    friend class TestCommands;
};


#endif // __KFORMULACONTAINER_H
