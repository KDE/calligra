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

#include <klocale.h>

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
{
    rootElement = new FormulaElement(this);
    dirty = true;
    testDirty();
}

KFormulaContainer::~KFormulaContainer()
{
    delete rootElement;
}


FormulaCursor* KFormulaContainer::createCursor()
{
    FormulaCursor* cursor = new FormulaCursor(rootElement);
    connect(this, SIGNAL(elementWillVanish(BasicElement*)),
            cursor, SLOT(elementWillVanish(BasicElement*)));
    connect(this, SIGNAL(formulaLoaded(FormulaElement*)),
            cursor, SLOT(formulaLoaded(FormulaElement*)));
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
    rootElement->draw(painter, context);
}


void KFormulaContainer::addText(FormulaCursor* cursor, QChar ch)
{
    setActiveCursor(cursor);
    removeSelection(cursor);
    KFCAdd* command = new KFCAdd(i18n("_:Undo descr.\nAdd text"), this);
    command->addElement(new TextElement(ch));
    execute(command);
}

void KFormulaContainer::addNumber(FormulaCursor* cursor, QChar ch)
{
    setActiveCursor(cursor);
    removeSelection(cursor);
    KFCAdd* command = new KFCAdd(i18n("_:Undo descr.\nAdd number"), this);
    command->addElement(new NumberElement(ch));
    execute(command);
}

void KFormulaContainer::addOperator(FormulaCursor* cursor, QChar ch)
{
    setActiveCursor(cursor);
    removeSelection(cursor);
    KFCAdd* command = new KFCAdd(i18n("_:Undo descr.\nAdd operator"), this);
    command->addElement(new OperatorElement(ch));
    execute(command);
}

void KFormulaContainer::addBracket(FormulaCursor* cursor, char left, char right)
{
    setActiveCursor(cursor);
    KFCAddReplacing* command = new KFCAddReplacing(i18n("_:Undo descr.\nAdd bracket"), this);
    command->setElement(new BracketElement(left, right));
    execute(command);
}

void KFormulaContainer::addFraction(FormulaCursor* cursor)
{
    setActiveCursor(cursor);
    KFCAddReplacing* command = new KFCAddReplacing(i18n("_:Undo descr.\nAdd fraction"), this);
    command->setElement(new FractionElement());
    execute(command);
}


void KFormulaContainer::addRoot(FormulaCursor* cursor)
{
    setActiveCursor(cursor);
    KFCAddReplacing* command = new KFCAddReplacing(i18n("_:Undo descr.\nAdd root"), this);
    command->setElement(new RootElement());
    execute(command);
}


void KFormulaContainer::addSymbol(FormulaCursor* cursor,
                                  Artwork::SymbolType type)
{
    setActiveCursor(cursor);
    KFCAddReplacing* command = new KFCAddReplacing(i18n("_:Undo descr.\nAdd symbol"), this);
    command->setElement(new SymbolElement(type));
    execute(command);
}

void KFormulaContainer::addMatrix(FormulaCursor* cursor, int rows, int columns)
{
    setActiveCursor(cursor);
    removeSelection(cursor);
    KFCAddMatrix* command = new KFCAddMatrix(this, rows, columns);
    execute(command);
}


void KFormulaContainer::addLowerLeftIndex(FormulaCursor* cursor)
{
    setActiveCursor(cursor);
    IndexElement* element = cursor->getActiveIndexElement();
    if (element == 0) {
        element = new IndexElement;

        if (!cursor->isSelection()) {
            //cursor->moveLeft(FormulaCursor::SelectMovement | FormulaCursor::WordMovement);
            cursor->moveLeft(FormulaCursor::SelectMovement);
        }
        KFCAddIndex* command = new KFCAddIndex(this, element, element->getLowerLeft());
        execute(command);
    }
    else {
        addGenericIndex(cursor, element->getLowerLeft());
    }
}

void KFormulaContainer::addUpperLeftIndex(FormulaCursor* cursor)
{
    setActiveCursor(cursor);
    IndexElement* element = cursor->getActiveIndexElement();
    if (element == 0) {
        element = new IndexElement;

        if (!cursor->isSelection()) {
            cursor->moveLeft(FormulaCursor::SelectMovement);
        }
        KFCAddIndex* command = new KFCAddIndex(this, element, element->getUpperLeft());
        execute(command);
    }
    else {
        addGenericIndex(cursor, element->getUpperLeft());
    }
}

void KFormulaContainer::addLowerRightIndex(FormulaCursor* cursor)
{
    setActiveCursor(cursor);
    IndexElement* element = cursor->getActiveIndexElement();
    if (element == 0) {
        element = new IndexElement;

        if (!cursor->isSelection()) {
            cursor->moveLeft(FormulaCursor::SelectMovement);
        }
        KFCAddIndex* command = new KFCAddIndex(this, element, element->getLowerRight());
        execute(command);
    }
    else {
        addGenericIndex(cursor, element->getLowerRight());
    }
}

void KFormulaContainer::addUpperRightIndex(FormulaCursor* cursor)
{
    setActiveCursor(cursor);
    IndexElement* element = cursor->getActiveIndexElement();
    if (element == 0) {
        element = new IndexElement;

        if (!cursor->isSelection()) {
            cursor->moveLeft(FormulaCursor::SelectMovement);
        }
        KFCAddIndex* command = new KFCAddIndex(this, element, element->getUpperRight());
        execute(command);
    }
    else {
        addGenericIndex(cursor, element->getUpperRight());
    }
}


/**
 * Just search an element that gets an index. Don't create one
 * if there is non.
 */
void KFormulaContainer::addGenericLowerIndex(FormulaCursor* cursor)
{
    setActiveCursor(cursor);
    SymbolElement* symbol = cursor->getActiveSymbolElement();
    if (symbol != 0) {
        addGenericIndex(cursor, symbol->getLowerIndex());
    }
}

/**
 * Just search an element that gets an index. Don't create one
 * if there is non.
 */
void KFormulaContainer::addGenericUpperIndex(FormulaCursor* cursor)
{
    setActiveCursor(cursor);
    SymbolElement* symbol = cursor->getActiveSymbolElement();
    if (symbol != 0) {
        addGenericIndex(cursor, symbol->getUpperIndex());
    }
    else {
        RootElement* root = cursor->getActiveRootElement();
        if (root != 0) {
            addGenericIndex(cursor, root->getIndex());
        }
    }
}

/**
 * Helper function that inserts the index it was called with.
 */
void KFormulaContainer::addGenericIndex(FormulaCursor* cursor, ElementIndexPtr index)
{
    setActiveCursor(cursor);
    if (!index->hasIndex()) {
        index->setToIndex(cursor);
        KFCAddGenericIndex* command = new KFCAddGenericIndex(this);
        execute(command);
    }
    else {
        index->moveToIndex(cursor, BasicElement::afterCursor);
        cursor->setSelection(false);
    }
}


void KFormulaContainer::remove(FormulaCursor* cursor,
                               BasicElement::Direction direction)
{
    setActiveCursor(cursor);
    KFCRemove* command = new KFCRemove(this, direction);
    execute(command);
}


void KFormulaContainer::replaceElementWithMainChild(FormulaCursor* cursor,
                                                    BasicElement::Direction direction)
{
    setActiveCursor(cursor);
    if (!cursor->isSelection()) {
        KFCRemoveEnclosing* command = new KFCRemoveEnclosing(this, direction);
        execute(command);
    }
}


void KFormulaContainer::paste(FormulaCursor* cursor, QMimeSource* source)
{
    setActiveCursor(cursor);
    if (source->provides("application/x-kformula")) {
        QByteArray data = source->encodedData("application/x-kformula");
        //cerr << data <<"aaa"<< endl;
        QDomDocument formula;
        formula.setContent(data);

        QList<BasicElement> list;
        list.setAutoDelete(true);
        if (cursor->buildElementsFromDom(formula, list)) {
            removeSelection(cursor);
            KFCAdd* command = new KFCAdd(i18n("_:Undo descr.\nPaste"), this);
            uint count = list.count();
            for (uint i = 0; i < count; i++) {
                command->addElement(list.take(0));
            }
            execute(command);
        }
    }
}


void KFormulaContainer::execute(KFormulaCommand *command)
{
    command->execute();
    if (!command->isSenseless()) {
        history.addCommand(command, false);
        testDirty();
    }
    else {
        delete command;
    }
}


void KFormulaContainer::removeSelection(FormulaCursor* cursor)
{
    if (cursor->isSelection()) {
        KFCRemoveSelection* command = new KFCRemoveSelection(this, BasicElement::beforeCursor);
        history.addCommand(command);
    }
}


void KFormulaContainer::undo()
{
    history.undo();
    testDirty();
}


void KFormulaContainer::undo(FormulaCursor *cursor)
{
    setActiveCursor(cursor);
    undo();
}

void KFormulaContainer::redo()
{
    history.redo();
    testDirty();
}

void KFormulaContainer::redo(FormulaCursor *cursor)
{
    setActiveCursor(cursor);
    redo();
}


QRect KFormulaContainer::boundingRect()
{
    return QRect(0, 0, rootElement->getWidth(), rootElement->getHeight());
}


void KFormulaContainer::testDirty()
{
    if (dirty) {
        dirty = false;
        rootElement->calcSizes(context);
        emit formulaChanged();
    }
}

QDomDocument KFormulaContainer::domData()
{
    QDomDocument doc("KFORMULA");
    doc.appendChild(rootElement->getElementDom(doc));
    return doc;
}

void KFormulaContainer::save(QString file)
{
    QFile f(file);
    if(!f.open(IO_Truncate | IO_ReadWrite)) {
        cerr << "Error" << endl;
        return;
    }
    QCString data=domData().toCString();
    cerr << (const char *)data << endl;    
  
    QTextStream str(&f);
    domData().save(str,4);
    f.close();
}	

void KFormulaContainer::load(QString file)
{
    QFile f(file);
    if (!f.open(IO_ReadOnly)) {
        cerr << "Error" << endl;
        return;
    }
    QDomDocument doc;
    if (!doc.setContent(&f)) {
        f.close();
        return;
    }
    QDomElement fe = doc.firstChild().toElement();
    if (!fe.isNull()) {
        FormulaElement* root = new FormulaElement(this);
        if (root->buildFromDom(fe)) {
            delete rootElement;
            rootElement = root;
            dirty = true;
            testDirty();
            history.clear();

            emit formulaLoaded(rootElement);
        }
        else {
            delete root;
            cerr << "Error constructing element tree." << endl;
        }
    }
    f.close();
}
