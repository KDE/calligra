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
#include <qstack.h>

#include "artwork.h"
#include "basicelement.h"
#include "elementindex.h"
#include "contextstyle.h"
#include "formulaelement.h"

class BasicElement;
class FormulaCursor;
class QKeyEvent;
class QPainter;
class KFormulaCommand;

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

    // There are a lot of thing we can do with the formula.
    
    void addText(FormulaCursor* cursor, QChar ch);
    void addNumber(FormulaCursor* cursor, QChar ch);
    void addOperator(FormulaCursor* cursor, QChar ch);
    void addBracket(FormulaCursor* cursor, char left, char right);
    void addFraction(FormulaCursor* cursor);
    void addRoot(FormulaCursor* cursor);
    void addSymbol(FormulaCursor* cursor, Artwork::SymbolType type);
    void addMatrix(FormulaCursor* cursor, int rows, int columns);

    void addLowerLeftIndex(FormulaCursor* cursor);
    void addUpperLeftIndex(FormulaCursor* cursor);
    void addLowerRightIndex(FormulaCursor* cursor);
    void addUpperRightIndex(FormulaCursor* cursor);

    void addRootIndex(FormulaCursor* cursor);
    void addSymbolLowerIndex(FormulaCursor* cursor, Artwork::SymbolType type);
    void addSymbolUpperIndex(FormulaCursor* cursor, Artwork::SymbolType type);

    void addGenericLowerIndex(FormulaCursor* cursor);
    void addGenericUpperIndex(FormulaCursor* cursor);
    
    void removeSelection(FormulaCursor* cursor, BasicElement::Direction);

    /**
     * Undo and move the undone command to the redo stack
     */
    void undo(); 

    /**
     * Redo and move the reFdone command to the undo stack
     */
    void redo(); 

    /**
     * Undo and move the undone command to the redo stack
     */
    void undo(FormulaCursor *cursor); 

    /**
     * Redo and move the reFdone command to the undo stack
     * the given cursor is used for redo and will be put
     * in the right place after the redo
     */
    void redo(FormulaCursor *cursor); 
        
    /**
     * Emits a formulaChanged signal if we are dirty.
     */
    void testDirty();

    
private:

    void addIndex(FormulaCursor* cursor, ElementIndexPtr index);

    /**
     * Push the command to the undo stack
     */
     void pushUndoStack(KFormulaCommand *command);

    /**
     * Push the command to the redo stack
     */
     void pushRedoStack(KFormulaCommand *command);
     
    /**
     * Clean redo stack because of a modify.
     */
     void cleanRedoStack() {redoStack.clear();}
     

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

    // debug
    friend class TestFormulaCursor;

    //Undo and redo stack
    QStack<KFormulaCommand> undoStack;
    QStack<KFormulaCommand> redoStack;
 
};


#endif // __KFORMULACONTAINER_H
