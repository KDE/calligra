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
#include <qimage.h>
#include <qptrlist.h>
#include <qobject.h>
#include <qptrstack.h>
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
class Document;
class FormulaCursor;
class FormulaElement;
class IndexElement;
class PlainCommand;
class SymbolTable;


/**
 * The interface the elements expect from its document.
 *
 * Please don't mistake this class for Document.
 * This one represents one formula, the other one provides
 * the context in which the formulae exist.
 */
class FormulaDocument {
    // not allowed
    FormulaDocument( const FormulaDocument& ) {}
    FormulaDocument& operator=( const FormulaDocument& ) { return *this; }
public:

    FormulaDocument() {}
    virtual ~FormulaDocument() {}

    virtual void elementRemoval(BasicElement* child) = 0;
    virtual void changed() = 0;
    virtual void cursorHasMoved( FormulaCursor* ) = 0;
    virtual void moveOutLeft( FormulaCursor* ) = 0;
    virtual void moveOutRight( FormulaCursor* ) = 0;
    virtual void removeFormula( FormulaCursor* ) {}
    virtual void baseSizeChanged( int, bool ) {}
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

    enum ViewActions { EXIT_LEFT, EXIT_RIGHT, REMOVE_FORMULA };

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
     * Gets called when ever something changes and we need to
     * recalc.
     */
    void changed();

    /**
     * Gets called when a request has the side effect of moving the
     * cursor. In the end any operation that moves the cursor should
     * call this.
     */
    void cursorHasMoved( FormulaCursor* );

    void moveOutLeft( FormulaCursor* );
    void moveOutRight( FormulaCursor* );
    void removeFormula( FormulaCursor* );

    /**
     * The base size changed. If not owned it uses the default one now.
     */
    void baseSizeChanged( int size, bool owned );

    /**
     * Draws the whole thing.
     */
    void draw( QPainter& painter, const QRect& r, const QColorGroup& cg, bool edit=false );

    /**
     * Draws the whole thing.
     */
    void draw( QPainter& painter, const QRect& r, bool edit=false );

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

    QString formulaString();

    /**
     * Tries to read a formula string from the other lib.
     */
    bool importOldText(QString text);

    /**
     * Prints the formula.
     */
    void print(KPrinter& printer);

    /**
     * @returns an image that looks like out formula.
     */
    QImage drawImage( int width, int height );

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

    double width() const;
    double height() const;

    /**
     * @returns the distance between the upper bound and the formulas
     * base line.
     */
    double baseline() const;

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

    int fontSize() const;

    /**
     * Sets the base font size of this formula.
     */
    void setFontSize( int pointSize, bool forPrint = false );

    void setFontSizeDirect( int pointSize );

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

    void leaveFormula( FormulaCursor* cursor, int cmd );

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
     * We've got a new base size.
     */
    void baseSizeChanged( int );

public:

    /**
     * General input.
     */
    void input( QKeyEvent* event );

    void performRequest( Request* request );

    // There are a lot of thing we can do with the formula.

    /**
     * Insert data from the clipboard.
     */
    void paste();

    /**
     * Insert data from the document.
     */
    void paste( QDomDocument document, QString desc );

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
     * Execute the command if it makes sense.
     */
    void execute(KCommand *command);

    /**
     * Emits a signal if the cursor had moved.
     */
    void checkCursor();

    /**
     * @returns true if there is a cursor that is allowed to edit the formula.
     */
    bool hasValidCursor() const;

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
