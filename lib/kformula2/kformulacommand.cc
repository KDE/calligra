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


KFormulaCommand::KFormulaCommand(const QString &name, KFormulaContainer* document)
        : KCommand(name), cursordata(0), undocursor(0), doc(document)
{
    removedList.setAutoDelete(true);

    FormulaCursor* cursor = getActiveCursor();
    cursordata = cursor->getCursorData();
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

KFCAddGenericIndex::KFCAddGenericIndex(KFormulaContainer* document,
                                       ElementIndexPtr index)
        : KFCAdd(i18n("_:Undo descr.\nAdd any index"), document)
{
    addElement(new SequenceElement());
    
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
