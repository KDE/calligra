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

#include <qapp.h>
#include <qdom.h>
#include <qevent.h>
#include <qfile.h>
#include <qpainter.h>
#include <qstring.h>
#include <qtextstream.h>

#include <klocale.h>

#include "bracketelement.h"
#include "contextstyle.h"
#include "formulacursor.h"
#include "formulaelement.h"
#include "fractionelement.h"
#include "indexelement.h"
#include "kformulacommand.h"
#include "kformulacontainer.h"
#include "kformulamimesource.h"
#include "kformulawidget.h"
#include "matrixelement.h"
#include "numberelement.h"
#include "operatorelement.h"
#include "rootelement.h"
#include "sequenceelement.h"
#include "symbolelement.h"
#include "textelement.h"
#include "MatrixDialog.h"


KFormulaContainer::KFormulaContainer(KCommandHistory& _history)
        : history(_history)
{
    connect(this, SIGNAL(commandExecuted()), &history, SIGNAL(commandExecuted()));
    rootElement = new FormulaElement(this);
    dirty = true;
    testDirty();
}

KFormulaContainer::~KFormulaContainer()
{
    delete rootElement;
}


FormulaCursor* KFormulaContainer::createCursor(KFormulaWidget* widget)
{
    return new FormulaCursor(widget, rootElement);
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

void KFormulaContainer::testDirty()
{
    if (dirty) {
        dirty = false;
        rootElement->calcSizes(context);
        emit formulaChanged(rootElement->getWidth(), rootElement->getHeight());
    }
}

bool KFormulaContainer::isEmpty()
{
    return rootElement->countChildren() == 0;
}


/**
 * Draws the whole thing.
 */
void KFormulaContainer::draw(QPainter& painter)
{
    rootElement->draw(painter, context);
}


void KFormulaContainer::addText(QChar ch)
{
    removeSelection();
    KFCAdd* command = new KFCAdd(i18n("Add text"), this);
    command->addElement(new TextElement(ch));
    execute(command);
}

void KFormulaContainer::addNumber(QChar ch)
{
    removeSelection();
    KFCAdd* command = new KFCAdd(i18n("Add number"), this);
    command->addElement(new NumberElement(ch));
    execute(command);
}

void KFormulaContainer::addOperator(QChar ch)
{
    removeSelection();
    KFCAdd* command = new KFCAdd(i18n("Add operator"), this);
    command->addElement(new OperatorElement(ch));
    execute(command);
}

void KFormulaContainer::addBracket(char left, char right)
{
    KFCAddReplacing* command = new KFCAddReplacing(i18n("Add bracket"), this);
    command->setElement(new BracketElement(left, right));
    execute(command);
}

void KFormulaContainer::addFraction()
{
    KFCAddReplacing* command = new KFCAddReplacing(i18n("Add fraction"), this);
    command->setElement(new FractionElement());
    execute(command);
}


void KFormulaContainer::addRoot()
{
    KFCAddReplacing* command = new KFCAddReplacing(i18n("Add root"), this);
    command->setElement(new RootElement());
    execute(command);
}


void KFormulaContainer::addSymbol(SymbolType type)
{
    KFCAddReplacing* command = new KFCAddReplacing(i18n("Add symbol"), this);
    command->setElement(new SymbolElement(type));
    execute(command);
}

void KFormulaContainer::addMatrix(int rows, int columns)
{
    removeSelection();
    KFCAddMatrix* command = new KFCAddMatrix(this, rows, columns);
    execute(command);
}

void KFormulaContainer::addMatrix(QWidget* parent)
{
    MatrixDialog* dialog = new MatrixDialog(parent);
    if (dialog->exec()) {
        uint rows = dialog->w;
        uint cols = dialog->h;
        addMatrix(rows, cols);
    }
    delete dialog;
}

void KFormulaContainer::addLowerLeftIndex()
{
    FormulaCursor* cursor = getActiveCursor();
    IndexElement* element = cursor->getActiveIndexElement();
    if (element == 0) {
        element = new IndexElement;

        if (!cursor->isSelection()) {
            //cursor->moveLeft(FormulaCursor::SelectMovement | FormulaCursor::WordMovement);
            cursor->moveLeft(SelectMovement);
        }
        KFCAddIndex* command = new KFCAddIndex(this, element, element->getLowerLeft());
        execute(command);
    }
    else {
        addGenericIndex(cursor, element->getLowerLeft());
    }
}

void KFormulaContainer::addUpperLeftIndex()
{
    FormulaCursor* cursor = getActiveCursor();
    IndexElement* element = cursor->getActiveIndexElement();
    if (element == 0) {
        element = new IndexElement;

        if (!cursor->isSelection()) {
            cursor->moveLeft(SelectMovement);
        }
        KFCAddIndex* command = new KFCAddIndex(this, element, element->getUpperLeft());
        execute(command);
    }
    else {
        addGenericIndex(cursor, element->getUpperLeft());
    }
}

void KFormulaContainer::addLowerRightIndex()
{
    FormulaCursor* cursor = getActiveCursor();
    IndexElement* element = cursor->getActiveIndexElement();
    if (element == 0) {
        element = new IndexElement;

        if (!cursor->isSelection()) {
            cursor->moveLeft(SelectMovement);
        }
        KFCAddIndex* command = new KFCAddIndex(this, element, element->getLowerRight());
        execute(command);
    }
    else {
        addGenericIndex(cursor, element->getLowerRight());
    }
}

void KFormulaContainer::addUpperRightIndex()
{
    FormulaCursor* cursor = getActiveCursor();
    IndexElement* element = cursor->getActiveIndexElement();
    if (element == 0) {
        element = new IndexElement;

        if (!cursor->isSelection()) {
            cursor->moveLeft(SelectMovement);
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
void KFormulaContainer::addGenericLowerIndex()
{
    FormulaCursor* cursor = getActiveCursor();
    SymbolElement* symbol = cursor->getActiveSymbolElement();
    if (symbol != 0) {
        addGenericIndex(cursor, symbol->getLowerIndex());
    }
}

/**
 * Just search an element that gets an index. Don't create one
 * if there is non.
 */
void KFormulaContainer::addGenericUpperIndex()
{
    FormulaCursor* cursor = getActiveCursor();
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
    if (!index->hasIndex()) {
        KFCAddGenericIndex* command = new KFCAddGenericIndex(this, index);
        execute(command);
    }
    else {
        index->moveToIndex(cursor, BasicElement::afterCursor);
        cursor->setSelection(false);
    }
}


void KFormulaContainer::remove(BasicElement::Direction direction)
{
    KFCRemove* command = new KFCRemove(this, direction);
    execute(command);
}


void KFormulaContainer::replaceElementWithMainChild(BasicElement::Direction direction)
{
    FormulaCursor* cursor = getActiveCursor();
    if (!cursor->isSelection()) {
        KFCRemoveEnclosing* command = new KFCRemoveEnclosing(this, direction);
        execute(command);
    }
}


void KFormulaContainer::paste()
{
    QClipboard* clipboard = QApplication::clipboard();
    const QMimeSource* source = clipboard->data();
    if (source->provides("application/x-kformula")) {
        QByteArray data = source->encodedData("application/x-kformula");
        //cerr << data <<"aaa"<< endl;
        QDomDocument formula;
        formula.setContent(data);

        QList<BasicElement> list;
        list.setAutoDelete(true);

        FormulaCursor* cursor = getActiveCursor();
        if (cursor->buildElementsFromDom(formula, list)) {
            removeSelection();
            KFCAdd* command = new KFCAdd(i18n("Paste"), this);
            uint count = list.count();
            for (uint i = 0; i < count; i++) {
                command->addElement(list.take(0));
            }
            execute(command);
        }
    }
}

void KFormulaContainer::copy()
{
    FormulaCursor* cursor = getActiveCursor();
    QDomDocument formula = cursor->copy();
    QClipboard* clipboard = QApplication::clipboard();
    clipboard->setData(new KFormulaMimeSource(formula));
}

void KFormulaContainer::cut()
{
    FormulaCursor* cursor = getActiveCursor();
    if (cursor->isSelection()) {
        copy();
        remove();
    }
}


void KFormulaContainer::execute(KFormulaCommand* command)
{
    command->execute();
    if (!command->isSenseless()) {
        history.addCommand(command, false);
        emit commandExecuted();
    }
    else {
        delete command;
    }
}


void KFormulaContainer::removeSelection()
{
    FormulaCursor* cursor = getActiveCursor();
    if (cursor->isSelection()) {
        KFCRemoveSelection* command = new KFCRemoveSelection(this, BasicElement::beforeCursor);
        history.addCommand(command);
    }
}


void KFormulaContainer::undo()
{
    history.undo();
}


void KFormulaContainer::redo()
{
    history.redo();
}


QRect KFormulaContainer::boundingRect()
{
    return QRect(0, 0, rootElement->getWidth(), rootElement->getHeight());
}


QDomDocument KFormulaContainer::domData()
{
    QDomDocument doc("KFORMULA");
    save(doc);
    return doc;
}

void KFormulaContainer::save(QString file)
{
    QFile f(file);
    if(!f.open(IO_Truncate | IO_ReadWrite)) {
        cerr << "Error opening file " << file.latin1() << endl;
        return;
    }
    QCString data=domData().toCString();
    cerr << (const char *)data << endl;    
  
    QTextStream str(&f);
    domData().save(str,4);
    f.close();
}	

/**
 * Saves the data into the document.
 */
void KFormulaContainer::save(QDomDocument doc)
{
    doc.appendChild(rootElement->getElementDom(doc));
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
    if (load(doc)) {
        history.clear();
    }
    f.close();
}

/**
 * Loads a formula from the document.
 */
bool KFormulaContainer::load(QDomDocument doc)
{
    QDomElement fe = doc.firstChild().toElement();
    if (!fe.isNull()) {
        FormulaElement* root = new FormulaElement(this);
        if (root->buildFromDom(fe)) {
            delete rootElement;
            rootElement = root;
            dirty = true;
            testDirty();

            emit formulaLoaded(rootElement);
            return true;
        }
        else {
            delete root;
            cerr << "Error constructing element tree." << endl;
        }
    }
    return false;
}
