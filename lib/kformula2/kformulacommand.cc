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
#include "indexelement.h"
#include "matrixelement.h"
#include "sequenceelement.h"


int KFormulaCommand::evilDestructionCount = 0;

KFormulaCommand::KFormulaCommand(const QString &name, KFormulaContainer* document)
        : KCommand(name), cursordata(0), undocursor(0), doc(document)
{
    removedList.setAutoDelete(true);

    setExecuteCursor(getActiveCursor());
    evilDestructionCount++;
}

KFormulaCommand::~KFormulaCommand()
{
    evilDestructionCount--;
    delete undocursor;
    delete cursordata;
}

FormulaCursor* KFormulaCommand::getExecuteCursor()
{
    FormulaCursor* cursor = getActiveCursor();
    cursor->setCursorData(cursordata);
    return cursor;
}

void KFormulaCommand::setExecuteCursor(FormulaCursor* cursor)
{
    // assert(cursordata == 0);
    cursordata = cursor->getCursorData();
}

FormulaCursor* KFormulaCommand::getUnexecuteCursor()
{
    FormulaCursor* cursor = getActiveCursor();
    cursor->setCursorData(undocursor);
    destroyUndoCursor();
    return cursor;
}

void KFormulaCommand::setUnexecuteCursor(FormulaCursor* cursor)
{
    // assert(undocursor == 0);
    undocursor = cursor->getCursorData();
}


// ******  Generic Add command 

KFCAdd::KFCAdd(const QString &name, KFormulaContainer *document)
        : KFormulaCommand(name, document)
{
}

void KFCAdd::execute()
{
    FormulaCursor* cursor = getExecuteCursor();
    cursor->insert(removedList, BasicElement::beforeCursor);
    setUnexecuteCursor(cursor);
    cursor->setSelection(false);
}


void KFCAdd::unexecute()
{
    FormulaCursor* cursor = getUnexecuteCursor();
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
}

void KFCRemoveSelection::execute()
{
    FormulaCursor* cursor = getExecuteCursor();
    cursor->remove(removedList, dir);
    setUnexecuteCursor(cursor);
}

void KFCRemoveSelection::unexecute()
{
    FormulaCursor* cursor = getUnexecuteCursor();
    cursor->insert(removedList);
    cursor->setSelection(false);
}


KFCRemove::KFCRemove(KFormulaContainer *document,
                     BasicElement::Direction direction)
        : KFormulaCommand(i18n("_:Undo descr.\nRemove selected text"), document),
          element(0), simpleRemoveCursor(0), dir(direction)
{
}

KFCRemove::~KFCRemove()
{
    delete simpleRemoveCursor;
    delete element;
}

void KFCRemove::execute()
{
    FormulaCursor* cursor = getExecuteCursor();
    cursor->remove(removedList, dir);
    if (cursor->elementIsSenseless()) {
        simpleRemoveCursor = cursor->getCursorData();
        element = cursor->replaceByMainChildContent();
    }
    setUnexecuteCursor(cursor);
    cursor->normalize();
}

void KFCRemove::unexecute()
{
    FormulaCursor* cursor = getUnexecuteCursor();
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
    FormulaCursor* cursor = getExecuteCursor();
    element = cursor->removeEnclosingElement(direction);
    setUnexecuteCursor(cursor);
    //cursor->normalize();
    cursor->setSelection(false);
}

void KFCRemoveEnclosing::unexecute()
{
    FormulaCursor* cursor = getUnexecuteCursor();
    cursor->replaceSelectionWith(element);
    cursor->normalize();
    element = 0;
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
    FormulaCursor* cursor = getExecuteCursor();
    cursor->replaceSelectionWith(element);
    setUnexecuteCursor(cursor);
    cursor->goInsideElement(element);
    element = 0;
}


void KFCAddReplacing::unexecute()
{
    FormulaCursor* cursor = getUnexecuteCursor();
    element = cursor->replaceByMainChildContent();
    cursor->normalize();
}


// ******  Add matrix command 

KFCAddMatrix::KFCAddMatrix(KFormulaContainer* document, int r, int c)
        : KFCAdd(i18n("_:Undo descr.\nAdd a matrix"), document)
{
    matrix = new MatrixElement(r, c);
    addElement(matrix);
}

void KFCAddMatrix::execute()
{
    KFCAdd::execute();
    FormulaCursor* cursor = getActiveCursor();
    cursor->goInsideElement(matrix);
}

// ******  Add index command

KFCAddGenericIndex::KFCAddGenericIndex(KFormulaContainer* document)
        : KFCAdd(i18n("_:Undo descr.\nAdd index"), document)
{
    addElement(new SequenceElement());
}


KFCAddIndex::KFCAddIndex(KFormulaContainer* document,
                         IndexElement* element, ElementIndexPtr index)
        : KFCAddReplacing(i18n("_:Undo descr.\nAdd index"), document)
{
    setElement(element);
    index->setToIndex(getActiveCursor());
    addIndex = new KFCAddGenericIndex(document);
}

KFCAddIndex::~KFCAddIndex()
{
    delete addIndex;
}

void KFCAddIndex::execute()
{
    KFCAddReplacing::execute();
    addIndex->execute();
}

void KFCAddIndex::unexecute()
{
    addIndex->unexecute();
    KFCAddReplacing::unexecute();
}
