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

#include <iostream>
#include <klocale.h>  //This is for undo descriptions

#include <qlist.h>

#include "kformulacommand.h"
#include "formulacursor.h"
#include "bracketelement.h"
#include "matrixelement.h"
#include "basicelement.h"
#include "textelement.h"
#include "rootelement.h"
#include "numberelement.h"
#include "operatorelement.h"
#include "fractionelement.h"
#include "indexelement.h"
#include "sequenceelement.h"
#include "symbolelement.h"


KFormulaCommand::KFormulaCommand(const QString &name, KFormulaContainer* document)
        : KCommand(name), doc(document), cursordata(0), undocursor(0)
{
    removedList.setAutoDelete(true);

    FormulaCursor* cursor = getActiveCursor();
    cursordata = cursor->getCursorData();

    shortText = "";
    longText = i18n("There is no info for this undo/redo");
}

KFormulaCommand::~KFormulaCommand()
{
    delete undocursor;
    delete cursordata;
}


// ******  Generic Add command 

KFCAdd::KFCAdd(const QString &name, KFormulaContainer *document)
        : KFormulaCommand(name, document)
{
    shortText = i18n("Add");
    longText = i18n("Add an element");
}

void KFCAdd::execute()
{
    FormulaCursor* cursor = getActiveCursor();
    cursor->setCursorData(cursordata);
    cursor->insert(removedList, BasicElement::beforeCursor);
    undocursor = cursor->getCursorData();
    cursor->setSelection(false);
}


void KFCAdd::unexecute()
{
    FormulaCursor* cursor = getActiveCursor();
    cursor->setCursorData(undocursor);
    destroyUndoCursor();
    cursor->remove(removedList, BasicElement::beforeCursor);
    //cursor->setSelection(false);
    cursor->normalize();
}



// ******  Remove selection command 

KFCRemoveSelection::KFCRemoveSelection(KFormulaContainer *document,
                                       BasicElement::Direction direction)
        : KFormulaCommand(i18n("_:Undo descr.\nRemove selected text"), document),
          dir(direction)
{
    shortText=i18n("_:Undo descr.\nRemove selected text");
    longText=i18n("Remove the currently selected text");
}

void KFCRemoveSelection::execute()
{
    FormulaCursor* cursor = getActiveCursor();
    cursor->setCursorData(cursordata);
    cursor->remove(removedList, dir);
    undocursor = cursor->getCursorData();
}

void KFCRemoveSelection::unexecute()
{
    FormulaCursor* cursor = getActiveCursor();
    cursor->setCursorData(undocursor);
    destroyUndoCursor();
    cursor->insert(removedList);
    cursor->setSelection(false);
}


KFCRemove::KFCRemove(KFormulaContainer *document,
                     BasicElement::Direction direction)
        : KFormulaCommand(i18n("_:Undo descr.\nRemove selected text"), document),
          element(0), simpleRemoveCursor(0), dir(direction)
{
    shortText=i18n("_:Undo descr.\nRemove selected text");
    longText=i18n("Remove the currently selected text");
}

KFCRemove::~KFCRemove()
{
    delete simpleRemoveCursor;
    delete element;
}

void KFCRemove::execute()
{
    FormulaCursor* cursor = getActiveCursor();
    cursor->setCursorData(cursordata);
    cursor->remove(removedList, dir);
    if (cursor->elementIsSenseless()) {
        simpleRemoveCursor = cursor->getCursorData();
        element = cursor->replaceByMainChildContent();
    }
    undocursor = cursor->getCursorData();
    cursor->normalize();
}

void KFCRemove::unexecute()
{
    FormulaCursor* cursor = getActiveCursor();
    cursor->setCursorData(undocursor);
    destroyUndoCursor();
    if (element != 0) {
        cursor->replaceSelectionWith(element);
        element = 0;
        
        cursor->setCursorData(simpleRemoveCursor);
        delete simpleRemoveCursor;
        simpleRemoveCursor = 0;
    }
    cursor->insert(removedList, dir);
    cursor->setSelection(false);
}


KFCRemoveEnclosing::KFCRemoveEnclosing(KFormulaContainer* document,
                                       BasicElement::Direction dir)
        : KFormulaCommand(i18n("_:Undo descr.\nRemove enclosing element"), document),
          element(0), direction(dir)
{
}

KFCRemoveEnclosing::~KFCRemoveEnclosing()
{
    delete element;
}

void KFCRemoveEnclosing::execute()
{
    FormulaCursor* cursor = getActiveCursor();
    cursor->setCursorData(cursordata);
    element = cursor->removeEnclosingElement(direction);
    undocursor = cursor->getCursorData();
    //cursor->normalize();
    cursor->setSelection(false);
}

void KFCRemoveEnclosing::unexecute()
{
    FormulaCursor* cursor = getActiveCursor();
    cursor->setCursorData(undocursor);
    destroyUndoCursor();
    cursor->replaceSelectionWith(element);
    cursor->normalize();
    element = 0;
}


//  **** Add text, operator, numbers command

KFCAddText::KFCAddText(KFormulaContainer* document, QChar ch) 
        : KFCAdd(i18n("_:Undo descr.\nAdd text"), document)
{
    removedList.append(new TextElement(ch));
    shortText=i18n("_:Undo descr.\nAdd text");
    longText=i18n("Insertion of a text element");
}


KFCAddNumber::KFCAddNumber(KFormulaContainer* document, QChar ch)
        : KFCAdd(i18n("_:Undo descr.\nAdd number"), document)
{
    removedList.append(new NumberElement(ch));
    shortText=i18n("_:Undo descr.\nAdd number");
    longText=i18n("Insertion of a numberic element");
}

KFCAddOperator::KFCAddOperator(KFormulaContainer* document, QChar ch)
        : KFCAdd(i18n("_:Undo descr.\nAdd operator"), document)
{
    removedList.append(new OperatorElement(ch));
    shortText=i18n("_:Undo descr.\nAdd operator");
    longText=i18n("Insertion of an operator element");
}


// ******  Add root, bracket etc command 

KFCAddReplacing::KFCAddReplacing(const QString &name, KFormulaContainer* document)
        : KFormulaCommand(name, document), element(0)
{
}

KFCAddReplacing::~KFCAddReplacing()
{
    delete element;
}


void KFCAddReplacing::execute()
{
    FormulaCursor* cursor = getActiveCursor();
    cursor->setCursorData(cursordata);
    cursor->replaceSelectionWith(element);
    undocursor = cursor->getCursorData();
    cursor->goInsideElement(element);
    element = 0;
}


void KFCAddReplacing::unexecute()
{
    FormulaCursor* cursor = getActiveCursor();
    cursor->setCursorData(undocursor);
    destroyUndoCursor();
    element = cursor->replaceByMainChildContent();
    cursor->normalize();
}


KFCAddRoot::KFCAddRoot(KFormulaContainer* document)
        : KFCAddReplacing(i18n("_:Undo descr.\nAdd root"), document)
{
    RootElement* root = new RootElement();
    setElement(root);
}


KFCAddFraction::KFCAddFraction(KFormulaContainer* document)
        : KFCAddReplacing(i18n("_:Undo descr.\nAdd fraction"), document)
{
    FractionElement* frac = new FractionElement();
    setElement(frac);
}


KFCAddBracket::KFCAddBracket(KFormulaContainer* document, QChar left, QChar right)
        : KFCAddReplacing(i18n("_:Undo descr.\nAdd bracket"), document)
{
    BracketElement* bra = new BracketElement(left,right);
    setElement(bra);
}

KFCAddSymbol::KFCAddSymbol(KFormulaContainer* document, Artwork::SymbolType type)
        : KFCAddReplacing(i18n("_:Undo descr.\nAdd symbol"), document)
{
    SymbolElement* sym = new SymbolElement(type);
    setElement(sym);
}

// ******  Add matrix command 

KFCAddMatrix::KFCAddMatrix(KFormulaContainer* document, int r, int c)
        : KFCAdd(i18n("_:Undo descr.\nAdd a matrix"), document)
{
    matrix = new MatrixElement(r, c);
    removedList.append(matrix);
}

void KFCAddMatrix::execute()
{
    KFCAdd::execute();
    FormulaCursor* cursor = getActiveCursor();
    cursor->goInsideElement(matrix);
}

// ******  Add index command

KFCAddGenericIndex::KFCAddGenericIndex(KFormulaContainer* document,
                                       ElementIndexPtr index)
        : KFCAdd(i18n("_:Undo descr.\nAdd any index"), document)
{
    removedList.append(new SequenceElement());
    
    FormulaCursor* cursor = getActiveCursor();
    index->setToIndex(cursor);
    delete cursordata;
    cursordata = cursor->getCursorData();
}


KFCAddIndex::KFCAddIndex(KFormulaContainer* document,
                         IndexElement* element, ElementIndexPtr index)
        : KFCAddReplacing(i18n("_:Undo descr.\nAdd index"), document),
          addGenericIndex(document, index)
{
    setElement(element);
}

void KFCAddIndex::execute()
{
    KFCAddReplacing::execute();
    addGenericIndex.execute();
}

void KFCAddIndex::unexecute()
{
    addGenericIndex.unexecute();
    KFCAddReplacing::unexecute();
}


/**
 * Command to insert stuff from the clipboard.
 */
KFCPaste::KFCPaste(KFormulaContainer* document, QList<BasicElement>& list)
        : KFCAdd(i18n("_:Undo descr.\nPaste"), document)
{
    list.setAutoDelete(false);
    uint count = list.count();
    for (uint i = 0; i < count; i++) {
        removedList.append(list.at(i));
    }
    list.clear();
}
