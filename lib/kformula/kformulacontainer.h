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

#ifndef KFORMULACONTAINER_H
#define KFORMULACONTAINER_H

#include <qclipboard.h>
#include <qdom.h>
#include <qlist.h>
#include <qobject.h>
#include <qstack.h>
#include <qstring.h>

#include <kcommand.h>

#include "kformuladefs.h"

class QColorGroup;
class QKeyEvent;
class QPainter;

class KCommand;
class KPrinter;

KFORMULA_NAMESPACE_BEGIN

class BasicElement;
class FormulaCursor;
class FormulaElement;
class IndexElement;
class Command;
class Document;
class SymbolTable;


/**
 * The interface the elements expect from its document.
 *
 * Please don't mistake this class for Document.
 * This one represents one formula, the other one provides
 * the context in which the formulae exist.
 */
class FormulaDocument {
public:

    virtual void elementRemoval(BasicElement* child) = 0;
    virtual void changed() = 0;
    virtual const SymbolTable& getSymbolTable() const = 0;
};


/**
 * The document. Actually only one part of the whole.
 * Provides everything to edit the formula.
 */
class Container : public QObject, public FormulaDocument {
    friend class MimeSource;
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
    Container(Document* doc);

    ~Container();

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
    void draw(QPainter& painter, const QRect& r, const QColorGroup& cg);

    /**
     * Draws the whole thing.
     */
    void draw(QPainter& painter, const QRect& r);

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
    void save(QDomNode doc);

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
    bool load(QDomNode doc);

    /**
     * @retruns a tex string for the formula
     */
    QString texString();

    /**
     * Tries to read a formula string from the other lib.
     */
    bool importOldText(QString text);

    /**
     * Prints the formula.
     */
    void print(KPrinter& printer);


    /**
     * @returns the cursor to be used for editing.
     */
    FormulaCursor* activeCursor();
    const FormulaCursor* activeCursor() const;

    /**
     * Sets the cursor that is to be used for any editing.
     *
     * The active cursor might 0. In this case you must not
     * request any change from the formula.
     */
    void setActiveCursor(FormulaCursor* cursor);

    /**
     * @returns the formula's size.
     */
    QRect boundingRect();

    /**
     * @returns the distance between the upper bound and the formulas
     * base line.
     */
    int baseline() const;

    /**
     * Moves the formula to a new location.
     */
    void moveTo( int x, int y );

    /**
     * Recalcs the formula and emits the .*Changed signals if needed.
     */
    void testDirty();

    /**
     * Recalc the formula.
     */
    void recalc();

    /**
     * @returns true if there is no element.
     */
    bool isEmpty();

    /**
     * @returns the document this formula belongs to.
     */
    Document* document() const;

    virtual const SymbolTable& getSymbolTable() const;

    /**
     * Sets the base font size of this formula.
     */
    void setFontSize( int pointSize, bool forPrint = false );

signals:

    /**
     * The cursor has been moved but the formula hasn't been
     * changed. The view that owns the cursor needs to know this.
     */
    void cursorMoved(FormulaCursor* cursor);

    /**
     * The cursor has been moved because of a change at the formula.
     */
    //void cursorChanged(FormulaCursor* cursor);

    /**
     * The formula has changed and needs to be redrawn.
     */
    void formulaChanged( int width, int height );
    void formulaChanged( double width, double height );

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

public:

    /**
     * General input.
     */
    bool input( QKeyEvent* event );

    // There are a lot of thing we can do with the formula.

    void addText(QChar ch, bool isSymbol = false);
    void addText(const QString& text);

    void addNameSequence();
    void addSpace( SpaceWidths space );

    void addLineBreak();

    void addBracket(char left, char right);
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
     * Adds a one by two matrix (one column, two rows)
     **/
    void addOneByTwoMatrix();

    /**
     * Asks for a matrix size and inserts it.
     */
    void addMatrix();

    /**
     * Changes the selected matrixes size.
     */
    void changeMatrix();

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

    void remove(Direction = beforeCursor);

    void replaceElementWithMainChild(Direction = beforeCursor);

    /**
     * Replaces the current name with its text symbol if it has one.
     */
    void compactExpression();

    /**
     * Converts the current character into a greek letter.
     */
    void makeGreek();

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

    /**
     * Creates a new IndexElement to be inserted and selects the elements
     * that should become its main child's content.
     */
    IndexElement* createIndexElement();

    void addGenericIndex(FormulaCursor* cursor, ElementIndexPtr index);

    /**
     * Execute the command if it makes sense.
     */
    void execute(Command *command);

    /**
     * Insert data from the document.
     */
    void paste( QDomDocument document, QString desc );

    /**
     * @returns true if there is a cursor that is allowed to edit the formula.
     */
    bool hasValidCursor() const;

    /**
     * @returns true if the cursor can insert any type of element.
     */
    bool hasMightyCursor() const;

    struct Container_Impl;
    Container_Impl* impl;

    FormulaElement* rootElement() const;

    // debug
    friend class TestFormulaCursor;
    friend class TestFormulaElement;
    friend class TestIndexElement;
    friend class TestCommands;
};

KFORMULA_NAMESPACE_END

#endif // KFORMULACONTAINER_H
