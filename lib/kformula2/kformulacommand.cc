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
#include "kformulacommand.h"
#include "formulacursor.h"
#include "basicelement.h"
#include "textelement.h"

KFormulaCommand::KFormulaCommand(KFormulaContainer *document,
				 FormulaCursor *cursor)
{
    doc=document;
    if(cursor)
        cursordata=cursor->getCursorData();
    else
        cursordata=0;
}

KFormulaCommand::~KFormulaCommand()
{
    if(cursordata)
        delete cursordata;
}

// ******  Add text command 

KFCAddText::KFCAddText(KFormulaContainer *document,FormulaCursor *cursor,
		       QChar ch) : KFormulaCommand(document,cursor)
{
    
    if (cursor->isSelection()) {
        doc->removeSelection(cursor, BasicElement::beforeCursor);
#warning TODO: get the removed selection !! 
 
    }

    QList<BasicElement> list;
    list.setAutoDelete(true);
    list.append(new TextElement(ch));
    cursor->insert(list);
    cursor->setSelection(false);
}

bool KFCAddText::undo(FormulaCursor *cursor)
{

  cursor->setCursorData(cursordata);
  cursor->remove(removedList,BasicElement::afterCursor);

}


bool KFCAddText::redo(FormulaCursor *cursor)
{

  cursor->setCursorData(cursordata);
  cursor->insert(removedList);


}
