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
#include <qtextstream.h>
#include <qfile.h>
#include <qdom.h>
#include <qpainter.h>
#include <qevent.h>
#include <qstring.h>

#include "bracketelement.h"
#include "contextstyle.h"
#include "formulacursor.h"
#include "formulaelement.h"
#include "fractionelement.h"
#include "indexelement.h"
#include "kformulacontainer.h"
#include "matrixelement.h"
#include "numberelement.h"
#include "operatorelement.h"
#include "rootelement.h"
#include "sequenceelement.h"
#include "symbolelement.h"
#include "textelement.h"
#include "kformulacommand.h"

KFormulaContainer::KFormulaContainer()
    : rootElement(this)
{
    dirty = true;
    testDirty();
}

KFormulaContainer::~KFormulaContainer()
{
}


FormulaCursor* KFormulaContainer::createCursor()
{
    FormulaCursor* cursor = new FormulaCursor(&rootElement);
    connect(this, SIGNAL(elementWillVanish(BasicElement*)),
            cursor, SLOT(elementWillVanish(BasicElement*)));
    return cursor;
}

void KFormulaContainer::destroyCursor(FormulaCursor* cursor)
{
    delete cursor;
}


/**
 * Gets called just before the child is removed from
 * the element tree.
 */
void KFormulaContainer::elementRemoval(BasicElement* child)
{
    emit elementWillVanish(child);
}

/**
 * Gets called whenever something changes and we need to
 * recalc.
 */
void KFormulaContainer::changed()
{
    dirty = true;
}


/**
 * Draws the whole thing.
 */
void KFormulaContainer::draw(QPainter& painter)
{
    rootElement.draw(painter, context);
}


void KFormulaContainer::addText(FormulaCursor* cursor, QChar ch)
{

KFCAddText *command=new KFCAddText(this,cursor,ch);

pushUndoStack(command);
cleanRedoStack();

/*
    if (cursor->isSelection()) {
        removeSelection(cursor, BasicElement::beforeCursor);
    }
    QList<BasicElement> list;
    list.setAutoDelete(true);
    list.append(new TextElement(ch));
    cursor->insert(list);
    cursor->setSelection(false);
*/
}

void KFormulaContainer::addNumber(FormulaCursor* cursor, QChar ch)
{
    if (cursor->isSelection()) {
        removeSelection(cursor, BasicElement::beforeCursor);
    }
    QList<BasicElement> list;
    list.setAutoDelete(true);
    list.append(new NumberElement(ch));
    cursor->insert(list);
    cursor->setSelection(false);
}

void KFormulaContainer::addOperator(FormulaCursor* cursor, QChar ch)
{
    if (cursor->isSelection()) {
        removeSelection(cursor, BasicElement::beforeCursor);
    }
    QList<BasicElement> list;
    list.setAutoDelete(true);
    list.append(new OperatorElement(ch));
    cursor->insert(list);
    cursor->setSelection(false);
}

void KFormulaContainer::addBracket(FormulaCursor* cursor, char left, char right)
{
    BracketElement* bracket = new BracketElement(left, right);
    if (cursor->isSelection()) {
        cursor->replaceSelectionWith(bracket);
    }
    else {
        cursor->insert(bracket);
        //cursor->setSelection(false);
    }
    cursor->goInsideElement(bracket);
}

void KFormulaContainer::addFraction(FormulaCursor* cursor)
{
    FractionElement* fraction = new FractionElement;
    if (cursor->isSelection()) {
        cursor->replaceSelectionWith(fraction);
    }
    else {
        cursor->insert(fraction);
        //cursor->setSelection(false);
    }
    cursor->goInsideElement(fraction);
}

void KFormulaContainer::addRoot(FormulaCursor* cursor)
{

KFCAddRoot *command=new KFCAddRoot(this,cursor);

pushUndoStack(command);
cleanRedoStack();


/*
    RootElement* root = new RootElement();
    if (cursor->isSelection()) {
        cursor->replaceSelectionWith(root);
    }
    else {
        cursor->insert(root);
        //cursor->setSelection(false);
    }
    cursor->goInsideElement(root);
*/
}


void KFormulaContainer::addSymbol(FormulaCursor* cursor,
                                  Artwork::SymbolType type)
{
    SymbolElement* symbol = new SymbolElement(type);
    if (cursor->isSelection()) {
        cursor->replaceSelectionWith(symbol);
    }
    else {
        cursor->insert(symbol);
        //cursor->setSelection(false);
    }
    cursor->goInsideElement(symbol);
}

void KFormulaContainer::addMatrix(FormulaCursor* cursor, int rows, int columns)
{
    if (cursor->isSelection()) {
        removeSelection(cursor, BasicElement::beforeCursor);
    }
    MatrixElement* matrix = new MatrixElement(rows, columns);
    cursor->insert(matrix);
    cursor->goInsideElement(matrix);
}

void KFormulaContainer::addLowerLeftIndex(FormulaCursor* cursor)
{
    IndexElement* element = cursor->getActiveIndexElement();
    if (element == 0) {
        element = new IndexElement;
        cursor->replaceSelectionWith(element, BasicElement::beforeCursor);
    }
    addIndex(cursor, element->getLowerLeft());
}

void KFormulaContainer::addUpperLeftIndex(FormulaCursor* cursor)
{
    IndexElement* element = cursor->getActiveIndexElement();
    if (element == 0) {
        element = new IndexElement;
        cursor->replaceSelectionWith(element, BasicElement::beforeCursor);
    }
    addIndex(cursor, element->getUpperLeft());
}

void KFormulaContainer::addLowerRightIndex(FormulaCursor* cursor)
{
    IndexElement* element = cursor->getActiveIndexElement();
    if (element == 0) {
        element = new IndexElement;
        cursor->replaceSelectionWith(element, BasicElement::beforeCursor);
    }
    addIndex(cursor, element->getLowerRight());
}

void KFormulaContainer::addUpperRightIndex(FormulaCursor* cursor)
{
    IndexElement* element = cursor->getActiveIndexElement();
    if (element == 0) {
        element = new IndexElement;
        cursor->replaceSelectionWith(element, BasicElement::beforeCursor);
    }
    addIndex(cursor, element->getUpperRight());
}

void KFormulaContainer::addRootIndex(FormulaCursor* cursor)
{
    RootElement* element = cursor->getActiveRootElement();
    if (element == 0) {
        element = new RootElement;
        cursor->replaceSelectionWith(element, BasicElement::beforeCursor);
    }
    addIndex(cursor, element->getIndex());
}

void KFormulaContainer::addSymbolLowerIndex(FormulaCursor* cursor, Artwork::SymbolType type)
{
    SymbolElement* element = cursor->getActiveSymbolElement();
    if (element == 0) {
        element = new SymbolElement(type);
        cursor->replaceSelectionWith(element, BasicElement::beforeCursor);
    }
    addIndex(cursor, element->getLowerIndex());
}

void KFormulaContainer::addSymbolUpperIndex(FormulaCursor* cursor, Artwork::SymbolType type)
{
    SymbolElement* element = cursor->getActiveSymbolElement();
    if (element == 0) {
        element = new SymbolElement(type);
        cursor->replaceSelectionWith(element, BasicElement::beforeCursor);
    }
    addIndex(cursor, element->getUpperIndex());
}

/**
 * Just search an element that gets an index. Don't create one
 * if there is non.
 */
void KFormulaContainer::addGenericLowerIndex(FormulaCursor* cursor)
{
    SymbolElement* symbol = cursor->getActiveSymbolElement();
    if (symbol != 0) {
        addIndex(cursor, symbol->getLowerIndex());
    }
}

/**
 * Just search an element that gets an index. Don't create one
 * if there is non.
 */
void KFormulaContainer::addGenericUpperIndex(FormulaCursor* cursor)
{
    SymbolElement* symbol = cursor->getActiveSymbolElement();
    if (symbol != 0) {
        addIndex(cursor, symbol->getUpperIndex());
    }
    else {
        RootElement* root = cursor->getActiveRootElement();
        if (root != 0) {
            addIndex(cursor, root->getIndex());
        }
    }
}

/**
 * Helper function that inserts the index it was called with.
 */
void KFormulaContainer::addIndex(FormulaCursor* cursor, ElementIndexPtr index)
{
    if (!index->hasIndex()) {
        SequenceElement* indexContent = new SequenceElement;
        index->setToIndex(cursor);
        cursor->insert(indexContent);
        //cursor->goInsideElement(indexContent);
    }
    else {
        index->moveToIndex(cursor, BasicElement::afterCursor);
        cursor->setSelection(false);
    }
}


void KFormulaContainer::removeSelection(FormulaCursor* cursor,
                                        BasicElement::Direction direction)
{
    QList<BasicElement> list;
    list.setAutoDelete(true);
    cursor->remove(list, direction);
    if (cursor->elementIsSenseless()) {
        BasicElement* element = cursor->replaceByMainChildContent();
        delete element;
    }
    //cursor->normalize(direction);
    cursor->normalize();
}


void KFormulaContainer::undo()
{

    FormulaCursor tmpCursor(&rootElement);
    undo(&tmpCursor);

}


void KFormulaContainer::undo(FormulaCursor *cursor)
{

    KFormulaCommand *command;
    if(!undoStack.isEmpty()) {
    command=undoStack.pop();
        if(command->undo(cursor))
            pushRedoStack(command);
    } 
 
}

void KFormulaContainer::redo()
{

    FormulaCursor tmpCursor(&rootElement);
    redo(&tmpCursor);

}

void KFormulaContainer::redo(FormulaCursor *cursor)
{

    KFormulaCommand *command;

    if(!redoStack.isEmpty()) {        
        command=redoStack.pop();
        if(command->redo(cursor))
            pushUndoStack(command);
    }
//else ??
    
}

void KFormulaContainer::pushUndoStack(KFormulaCommand *command) 
{ 
    undoStack.push(command); 

//emit signals

}

void KFormulaContainer::pushRedoStack(KFormulaCommand *command) 
{ 
    redoStack.push(command); 

//emit signals

}


void KFormulaContainer::testDirty()
{
    if (dirty) {
        dirty = false;
        rootElement.calcSizes(context);
        emit formulaChanged();
    }
}

QDomDocument KFormulaContainer::domData()
{
    QDomDocument doc("KFORMULA");
    doc.appendChild(rootElement.getElementDom(&doc));
    return doc;
}

void KFormulaContainer::save(QString file)
{
	
    QFile f(file);
    if(!f.open(IO_ReadWrite))
    cerr << "Error" << endl;   
   QCString data=domData().toCString();
   fprintf(stderr,"%s\n",(const char *)data);    
  
    QTextStream str(&f);
    domData().save(str,4);
    f.close();
}	
