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
#include <koprinter.h>
#include <qstring.h>
#include <qtextstream.h>

#include <klocale.h>

#include "MatrixDialog.h"
#include "bracketelement.h"
#include "contextstyle.h"
#include "formulacursor.h"
#include "formulaelement.h"
#include "fractionelement.h"
#include "indexelement.h"
#include "kformulacommand.h"
#include "kformulacontainer.h"
#include "kformuladocument.h"
#include "kformulamathmlread.h"
#include "kformulamimesource.h"
#include "kformulawidget.h"
#include "matrixelement.h"
#include "rootelement.h"
#include "sequenceelement.h"
#include "symbolelement.h"
#include "textelement.h"


KFormulaContainer::KFormulaContainer(KFormulaDocument* doc)
        : document(doc)
{
    connect(this, SIGNAL(commandExecuted()),
            document->getHistory(), SIGNAL(commandExecuted()));
    rootElement = new FormulaElement(this);
    activeCursor = internCursor = createCursor();
    dirty = true;
    testDirty();
}

KFormulaContainer::~KFormulaContainer()
{
    getDocument()->formulaDies(this);
    delete internCursor;
    delete rootElement;
}


FormulaCursor* KFormulaContainer::createCursor()
{
    return new FormulaCursor(rootElement);
}


KCommandHistory* KFormulaContainer::getHistory() const
{
    return document->getHistory();
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


FormulaCursor* KFormulaContainer::getActiveCursor()
{
    return activeCursor;
}


/**
 * Tells the formula that a view got the focus and might want to
 * edit the formula.
 */
void KFormulaContainer::setActiveCursor(FormulaCursor* cursor)
{
    getDocument()->activate(this);
    if (cursor != 0) {
        activeCursor = cursor;
    }
    else {
        FormulaCursor::CursorData* data = activeCursor->getCursorData();
        internCursor->setCursorData(data);
        delete data;
        activeCursor = internCursor;
    }
}


bool KFormulaContainer::hasValidCursor() const
{
    return (activeCursor != 0) && !activeCursor->isReadOnly();
}


void KFormulaContainer::testDirty()
{
    if (dirty) {
        dirty = false;
        rootElement->calcSizes(getDocument()->getContextStyle());
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
    rootElement->draw(painter, getDocument()->getContextStyle());
}


void KFormulaContainer::addText(QChar ch)
{
    if (!hasValidCursor())
        return;
    KFCReplace* command = new KFCReplace(i18n("Add text"), this);
    command->addElement(new TextElement(ch));
    execute(command);
}

void KFormulaContainer::addLineBreak()
{
    if (!hasValidCursor())
        return;
    // Not supported right now.
}

void KFormulaContainer::addBracket(char left, char right)
{
    if (!hasValidCursor())
        return;
    KFCAddReplacing* command = new KFCAddReplacing(i18n("Add bracket"), this);
    command->setElement(new BracketElement(left, right));
    execute(command);
}

void KFormulaContainer::addDefaultBracket()
{
//     if (!hasValidView())
//         return;
//     addBracket(activeView->getLeftBracket(), activeView->getRightBracket());
}

void KFormulaContainer::addFraction()
{
    if (!hasValidCursor())
        return;
    KFCAddReplacing* command = new KFCAddReplacing(i18n("Add fraction"), this);
    command->setElement(new FractionElement());
    execute(command);
}


void KFormulaContainer::addRoot()
{
    if (!hasValidCursor())
        return;
    KFCAddReplacing* command = new KFCAddReplacing(i18n("Add root"), this);
    command->setElement(new RootElement());
    execute(command);
}


void KFormulaContainer::addSymbol(SymbolType type)
{
    if (!hasValidCursor())
        return;
    KFCAddReplacing* command = new KFCAddReplacing(i18n("Add symbol"), this);
    command->setElement(new SymbolElement(type));
    execute(command);
}

void KFormulaContainer::addMatrix(int rows, int columns)
{
    if (!hasValidCursor())
        return;
    KFCAddMatrix* command = new KFCAddMatrix(this, rows, columns);
    execute(command);
}

void KFormulaContainer::addMatrix()
{
    MatrixDialog* dialog = new MatrixDialog(0);
    if (dialog->exec()) {
        uint rows = dialog->w;
        uint cols = dialog->h;
        addMatrix(rows, cols);
    }
    delete dialog;
}

void KFormulaContainer::addLowerLeftIndex()
{
    if (!hasValidCursor())
        return;
    FormulaCursor* cursor = getActiveCursor();
    IndexElement* element = cursor->getActiveIndexElement();
    if (element == 0) {
        element = createIndexElement();
        KFCAddIndex* command = new KFCAddIndex(this, element, element->getLowerLeft());
        execute(command);
    }
    else {
        addGenericIndex(cursor, element->getLowerLeft());
    }
}

void KFormulaContainer::addUpperLeftIndex()
{
    if (!hasValidCursor())
        return;
    FormulaCursor* cursor = getActiveCursor();
    IndexElement* element = cursor->getActiveIndexElement();
    if (element == 0) {
        element = createIndexElement();
        KFCAddIndex* command = new KFCAddIndex(this, element, element->getUpperLeft());
        execute(command);
    }
    else {
        addGenericIndex(cursor, element->getUpperLeft());
    }
}

void KFormulaContainer::addLowerRightIndex()
{
    if (!hasValidCursor())
        return;
    FormulaCursor* cursor = getActiveCursor();
    IndexElement* element = cursor->getActiveIndexElement();
    if (element == 0) {
        element = createIndexElement();
        KFCAddIndex* command = new KFCAddIndex(this, element, element->getLowerRight());
        execute(command);
    }
    else {
        addGenericIndex(cursor, element->getLowerRight());
    }
}

void KFormulaContainer::addUpperRightIndex()
{
    if (!hasValidCursor())
        return;
    FormulaCursor* cursor = getActiveCursor();
    IndexElement* element = cursor->getActiveIndexElement();
    if (element == 0) {
        element = createIndexElement();
        KFCAddIndex* command = new KFCAddIndex(this, element, element->getUpperRight());
        execute(command);
    }
    else {
        addGenericIndex(cursor, element->getUpperRight());
    }
}

IndexElement* KFormulaContainer::createIndexElement()
{
    IndexElement* element = new IndexElement;
    FormulaCursor* cursor = getActiveCursor();
    if (!cursor->isSelection()) {
        cursor->moveLeft(SelectMovement | WordMovement);
    }
    return element;
}

/**
 * Just search an element that gets an index. Don't create one
 * if there is non.
 */
void KFormulaContainer::addGenericLowerIndex()
{
    if (!hasValidCursor())
        return;
    FormulaCursor* cursor = getActiveCursor();
    SymbolElement* symbol = cursor->getActiveSymbolElement();
    if (symbol != 0) {
        addGenericIndex(cursor, symbol->getLowerIndex());
    }
    else {
        IndexElement* index = cursor->getActiveIndexElement();
        if (index != 0) {
            addGenericIndex(cursor, index->getLowerMiddle());
        }
        else {
            index = createIndexElement();
            KFCAddIndex* command = new KFCAddIndex(this, index, index->getLowerMiddle());
            execute(command);
        }
    }
}

/**
 * Just search an element that gets an index. Don't create one
 * if there is non.
 */
void KFormulaContainer::addGenericUpperIndex()
{
    if (!hasValidCursor())
        return;
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
        else {
            IndexElement* index = cursor->getActiveIndexElement();
            if (index != 0) {
                addGenericIndex(cursor, index->getUpperMiddle());
            }
            else {
                index = createIndexElement();
                KFCAddIndex* command = new KFCAddIndex(this, index, index->getUpperMiddle());
                execute(command);
            }
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
        emit cursorMoved(cursor);
    }
}


void KFormulaContainer::remove(BasicElement::Direction direction)
{
    if (!hasValidCursor())
        return;
    KFCRemove* command = new KFCRemove(this, direction);
    execute(command);
}


void KFormulaContainer::replaceElementWithMainChild(BasicElement::Direction direction)
{
    if (!hasValidCursor())
        return;
    FormulaCursor* cursor = getActiveCursor();
    if (!cursor->isSelection()) {
        KFCRemoveEnclosing* command = new KFCRemoveEnclosing(this, direction);
        execute(command);
    }
}


void KFormulaContainer::compactExpression()
{
    if (!hasValidCursor())
        return;
    FormulaCursor* cursor = getActiveCursor();
    QString name = cursor->getCurrentName();
    if (!name.isNull()) {
        QChar ch = getDocument()->getSymbolTable().getSymbolChar(name);
        if (!ch.isNull()) {
            KFCReplace* command = new KFCReplace(i18n("Add symbol"), this);
            TextElement* element = new TextElement(ch);
            element->setSymbol(true);
            command->addElement(element);
            execute(command);
            return;
        }
    }

    // It might have moved the cursor. So tell them.
    emit cursorMoved(cursor);
}


void KFormulaContainer::makeGreek()
{
    if (!hasValidCursor())
        return;
    FormulaCursor* cursor = getActiveCursor();
    TextElement* element = cursor->getActiveTextElement();
    if ((element != 0) && !element->isSymbol()) {
        const SymbolTable& table = getDocument()->getSymbolTable();
        if (table.getGreekLetters().find(element->getCharacter()) != -1) {
            KFCMakeSymbol* command = new KFCMakeSymbol(this, element);
            execute(command);
        }
    }
}


void KFormulaContainer::paste()
{
    if (!hasValidCursor())
        return;
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
            uint count = list.count();
            // You must not execute an add command that adds nothing.
            if (count > 0) {
                KFCReplace* command = new KFCReplace(i18n("Paste"), this);
                for (uint i = 0; i < count; i++) {
                    command->addElement(list.take(0));
                }
                execute(command);
            }
        }
    }
}

void KFormulaContainer::copy()
{
    // read-only cursors are fine for copying.
    FormulaCursor* cursor = getActiveCursor();
    if (cursor != 0) {
        QDomDocument formula = cursor->copy();
        QClipboard* clipboard = QApplication::clipboard();
        clipboard->setData(new KFormulaMimeSource(formula));
    }
}

void KFormulaContainer::cut()
{
    if (!hasValidCursor())
        return;
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
        getHistory()->addCommand(command, false);
        emit commandExecuted();
    }
    else {
        delete command;
    }
}


QRect KFormulaContainer::boundingRect()
{
    return QRect(0, 0, rootElement->getWidth(), rootElement->getHeight());
}

void KFormulaContainer::moveTo(int x, int y)
{
    rootElement->setX(x);
    rootElement->setY(y);
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
        cerr << "Error opening file" << endl;
        return;
    }
    QDomDocument doc;
    if (!doc.setContent(&f)) {
        f.close();
        return;
    }
    if (load(doc)) {
        getHistory()->clear();
    }
    f.close();
}

void KFormulaContainer::loadMathMl(QString file)
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
    MathMl2KFormula filter(&doc);
    //cerr << "Filtering" << endl;

    filter.startConversion();
    if(filter.isDone())
        if (load(filter.getKFormulaDom())) {
    	    getHistory()->clear();
	}
    f.close();
}

/**
 * Loads a formula from the document.
 */
bool KFormulaContainer::load(QDomDocument doc)
{
    //cerr << "Loading" << endl;
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


void KFormulaContainer::print(KPrinter& printer)
{
    //printer.setFullPage(true);
    QPainter painter;
    if (painter.begin(&printer)) {
        rootElement->draw(painter, getDocument()->getContextStyle());
    }
}
#include "kformulacontainer.moc"
