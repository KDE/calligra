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
#include <qtextstream.h>
#include <qfile.h>
#include <qpainter.h>

#include <qstring.h>

#include <kdebug.h>
#include <klocale.h>
#include <koprinter.h>

#include "MatrixDialog.h"
#include "bracketelement.h"
#include "contextstyle.h"
#include "formulacursor.h"
#include "formulaelement.h"
#include "fractionelement.h"
#include "indexelement.h"
#include "kformulacommand.h"
#include "kformulacompatibility.h"
#include "kformulacontainer.h"
#include "kformuladocument.h"
#include "kformulamathmlread.h"
#include "kformulamimesource.h"
#include "matrixelement.h"
#include "rootelement.h"
#include "sequenceelement.h"
#include "symbolelement.h"
#include "symboltable.h"
#include "textelement.h"


KFORMULA_NAMESPACE_BEGIN
using namespace std;


struct KFormulaContainer::KFormulaContainer_Impl {

    KFormulaContainer_Impl(KFormulaDocument* doc)
            : document(doc)
    {
    }

    ~KFormulaContainer_Impl()
    {
        delete internCursor;
        delete rootElement;
    }

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
     * The cursor that is used if there is no view.
     */
    FormulaCursor* internCursor;

    /**
     * The document we belong to.
     */
    KFormulaDocument* document;
};


FormulaElement* KFormulaContainer::rootElement() const { return impl->rootElement; }
KFormulaDocument* KFormulaContainer::document() const { return impl->document; }

KFormulaContainer::KFormulaContainer(KFormulaDocument* doc)
{
    impl = new KFormulaContainer_Impl(doc);
    impl->rootElement = new FormulaElement(this);
    impl->activeCursor = impl->internCursor = createCursor();
    connect(this, SIGNAL(commandExecuted()),
            document()->getHistory(), SIGNAL(commandExecuted()));
    recalc();
}

KFormulaContainer::~KFormulaContainer()
{
    document()->formulaDies(this);
    delete impl;
}


FormulaCursor* KFormulaContainer::createCursor()
{
    return new FormulaCursor(rootElement());
}


KCommandHistory* KFormulaContainer::getHistory() const
{
    return document()->getHistory();
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
    impl->dirty = true;
}


FormulaCursor* KFormulaContainer::getActiveCursor()
{
    return impl->activeCursor;
}


/**
 * Tells the formula that a view got the focus and might want to
 * edit the formula.
 */
void KFormulaContainer::setActiveCursor(FormulaCursor* cursor)
{
    document()->activate(this);
    if (cursor != 0) {
        impl->activeCursor = cursor;
    }
    else {
        //FormulaCursor::CursorData* data = activeCursor->getCursorData();
        //internCursor->setCursorData(data);
        //delete data;
        *(impl->internCursor) = *(impl->activeCursor);
        impl->activeCursor = impl->internCursor;
    }
}


bool KFormulaContainer::hasValidCursor() const
{
    return (impl->activeCursor != 0) && !impl->activeCursor->isReadOnly();
}


void KFormulaContainer::testDirty()
{
    if (impl->dirty) {
        recalc();
    }
}

void KFormulaContainer::recalc()
{
    impl->dirty = false;
    rootElement()->calcSizes(document()->getContextStyle());
    emit cursorChanged(getActiveCursor());

    //emit formulaChanged(rootElement->getWidth() + document()->getContextStyle().getEmptyRectWidth() / 2,
    //                    rootElement->getHeight() + document()->getContextStyle().getEmptyRectHeight() / 2);
    emit formulaChanged(rootElement()->getWidth(), rootElement()->getHeight());
    emit cursorMoved(getActiveCursor());
}

bool KFormulaContainer::isEmpty()
{
    return rootElement()->countChildren() == 0;
}


KFormulaDocument* KFormulaContainer::getDocument() const
{
    return document();
}

const SymbolTable& KFormulaContainer::getSymbolTable() const
{
    return document()->getSymbolTable();
}


void KFormulaContainer::draw(QPainter& painter, const QRect& r, const QColorGroup& cg)
{
    painter.fillRect( r, cg.base() );
    draw( painter, r );
}


void KFormulaContainer::draw(QPainter& painter, const QRect& r)
{
    rootElement()->draw(painter, r, document()->getContextStyle(painter.device()->devType() == QInternal::Printer));
}


void KFormulaContainer::addText(QChar ch, bool isSymbol)
{
    if (!hasValidCursor())
        return;
    KFCReplace* command = new KFCReplace(i18n("Add text"), this);
    TextElement* element = new TextElement( ch, isSymbol );
    command->addElement(element);
    execute(command);
}

void KFormulaContainer::addText(const QString& text)
{
    if (!hasValidCursor())
        return;
    KFCReplace* command = new KFCReplace(i18n("Add text"), this);
    for (uint i = 0; i < text.length(); i++) {
        command->addElement(new TextElement(text[i]));
    }
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
    KFCAddReplacing* command = new KFCAddReplacing(i18n("Add matrix"), this);
    command->setElement(new MatrixElement(rows, columns));
    execute(command);
}


void KFormulaContainer::addOneByTwoMatrix()
{
    if (!hasValidCursor())
        return;
    KFCAddReplacing* command = new KFCAddReplacing(i18n("Add 1x2 matrix"), this);
    FractionElement* element = new FractionElement();
    element->showLine(false);
    command->setElement(element);
    execute(command);
}

void KFormulaContainer::addMatrix()
{
    MatrixDialog* dialog = new MatrixDialog(0);
    if (dialog->exec()) {
        uint rows = dialog->h;
        uint cols = dialog->w;
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
        QChar ch = document()->getSymbolTable().unicode( name );
        if (!ch.isNull()) {
            KFCReplace* command = new KFCReplace(i18n("Add symbol"), this);
            TextElement* element = new TextElement( ch, true );
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
        cursor->selectActiveElement();
        const SymbolTable& table = document()->getSymbolTable();
        if (table.greekLetters().find(element->getCharacter()) != -1) {
            KFCReplace* command = new KFCReplace(i18n("Changes the char to a symbol"), this);
            TextElement* symbol = new TextElement( table.unicodeFromSymbolFont( element->getCharacter() ), true );
            command->addElement( symbol );
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
    return QRect(rootElement()->getX(), rootElement()->getY(),
                 rootElement()->getWidth(), rootElement()->getHeight());
}

int KFormulaContainer::baseline() const
{
    return static_cast<int>( rootElement()->getBaseline() );
}

void KFormulaContainer::moveTo(int x, int y)
{
    rootElement()->setX(x);
    rootElement()->setY(y);
}


// This doesn't work.
void KFormulaContainer::setFontSize( int /*pointSize*/, bool /*forPrint*/ )
{
    // taken from KWTextFrameSet::zoom
//     double factor = document()->getContextStyle( forPrint ).getYResolution() *
//                     ( forPrint ? 1.0 : 72.0 / QPaintDevice::x11AppDpiY() );

//     double newSize = factor*pointSize;
//     if ( rootElement()->getBaseSize() != newSize ) {
//         rootElement()->setBaseSize( newSize );
//         kdDebug( 40000 ) << "KFormulaContainer::save " << newSize << endl;
//         recalc();
//     }
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
        kdDebug( 40000 ) << "Error opening file " << file.latin1() << endl;
        return;
    }
    QCString data=domData().toCString();
    //cerr << (const char *)data << endl;

    QTextStream stream(&f);
    stream.setEncoding(QTextStream::Unicode);
    domData().save(stream,4);
    f.close();
}

/**
 * Saves the data into the document.
 */
void KFormulaContainer::save(QDomNode doc)
{
    QDomDocument ownerDoc = doc.ownerDocument();
    doc.appendChild(rootElement()->getElementDom(ownerDoc));
}

void KFormulaContainer::load(QString file)
{
    QFile f(file);
    if (!f.open(IO_ReadOnly)) {
        kdDebug( 40000 ) << "Error opening file " << file.latin1() << endl;
        return;
    }
    QTextStream stream(&f);
    stream.setEncoding(QTextStream::Unicode);
    QString content = stream.read();
    //kdDebug( 40000 ) << content << endl;
    QDomDocument doc;
    if (!doc.setContent(content)) {
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
        kdDebug( 40000 ) << "Error opening file " << file.latin1() << endl;
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
bool KFormulaContainer::load(QDomNode doc)
{
    //cerr << "Loading" << endl;
    QDomElement fe = doc.firstChild().toElement();
    if (!fe.isNull()) {
        FormulaElement* root = new FormulaElement(this);
        if (root->buildFromDom(fe)) {
            delete impl->rootElement;
            impl->rootElement = root;
            emit formulaLoaded(rootElement());

            recalc();
            return true;
        }
        else {
            delete root;
            kdDebug( 40000 ) << "Error constructing element tree." << endl;
        }
    }
    return false;
}


void KFormulaContainer::print(KPrinter& printer)
{
    //printer.setFullPage(true);
    QPainter painter;
    if (painter.begin(&printer)) {
        rootElement()->draw(painter, boundingRect(), document()->getContextStyle(true));
    }
}

QString KFormulaContainer::texString()
{
    return rootElement()->toLatex();
}


bool KFormulaContainer::importOldText(QString text)
{
    KFormulaCompatibility converter;
    QDomDocument doc = converter.buildDOM(text);
    QCString data = doc.toCString();
    cerr << (const char *)data << endl;
    return load(doc);
}

KFORMULA_NAMESPACE_END

using namespace KFormula;
#include "kformulacontainer.moc"
