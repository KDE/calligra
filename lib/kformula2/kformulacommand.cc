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

KFormulaCommand::KFormulaCommand(KFormulaContainer *document,
				 FormulaCursor *cursor)
{
    doc=document;
    if(cursor)
        cursordata=cursor->getCursorData();
    else
        cursordata=0;
    
    undocursor=0;

    shortText="";
    longText=i18n("There is no info for this undo/redo");
}

KFormulaCommand::~KFormulaCommand()
{
    if(cursordata)
        delete cursordata;
}

// ******  Generic Add command 

KFCAdd::KFCAdd(KFormulaContainer *document,FormulaCursor *cursor) : KFormulaCommand(document,cursor)
{
    removedList.setAutoDelete(true); 
    removedList.clear();
    insideElement=0;
    undocursor=cursor->getCursorData();
    
    shortText=i18n("Add");
    longText=i18n("Add an element of unknown type");

}

bool KFCAdd::undo(FormulaCursor *cursor)
{

    cursor->setCursorData(undocursor);
    cursor->remove(removedList,BasicElement::beforeCursor);

    return true;
}


bool KFCAdd::redo(FormulaCursor *cursor)
{

    cursor->setCursorData(cursordata);
    cursor->insert(removedList);

    if(!insideElement)
        cursor->setSelection(false);
    else
        cursor->goInsideElement(insideElement);
	
    return true;
}



// ******  Remove selection command 

KFCRemoveSelection::KFCRemoveSelection(KFormulaContainer *document,FormulaCursor *cursor,
		    BasicElement::Direction direction) : KFormulaCommand(document,cursor)
{
    dir=direction;
    removedList.setAutoDelete(true); 
    cursor->remove(removedList, direction);
    if (cursor->elementIsSenseless()) {
        BasicElement* element = cursor->replaceByMainChildContent();
        delete element;
    }
    cursor->normalize();
    undocursor=cursor->getCursorData();
    shortText=i18n("_:Undo descr.\nRemove selected text");
    longText=i18n("Remove the currently selected text");

}

bool KFCRemoveSelection::redo(FormulaCursor *cursor)
{

    cursor->setCursorData(cursordata);
    cursor->remove(removedList,dir);
    if (cursor->elementIsSenseless()) {
        BasicElement* element = cursor->replaceByMainChildContent();
        delete element;
    }
    cursor->normalize();
    return true;
}


bool KFCRemoveSelection::undo(FormulaCursor *cursor)
{

    cursor->setCursorData(undocursor);
    cursor->insert(removedList);
    return true;
}



//  **** Add text,operator,numbers command
KFCAddText::KFCAddText(KFormulaContainer *document,FormulaCursor *cursor,QChar ch) 
: KFCAdd(document,cursor)
{
    

    QList<BasicElement> list;
    list.setAutoDelete(true);
    list.append(new TextElement(ch));
    cursor->insert(list);
    cursor->setSelection(false);
    undocursor=cursor->getCursorData();
    shortText=i18n("_:Undo descr.\nAdd text");
    longText=i18n("Insertion of a text element");

}


KFCAddNumber::KFCAddNumber(KFormulaContainer *document,FormulaCursor *cursor,QChar ch) 
: KFCAdd(document,cursor)
{
    

    QList<BasicElement> list;
    list.setAutoDelete(true);
    list.append(new NumberElement(ch));
    cursor->insert(list);
    cursor->setSelection(false);
    undocursor=cursor->getCursorData();
    shortText=i18n("_:Undo descr.\nAdd number");
    longText=i18n("Insertion of a numberic element");

}

KFCAddOperator::KFCAddOperator(KFormulaContainer *document,FormulaCursor *cursor,QChar ch) 
: KFCAdd(document,cursor)
{
    

    QList<BasicElement> list;
    list.setAutoDelete(true);
    list.append(new OperatorElement(ch));
    cursor->insert(list);
    cursor->setSelection(false);
    undocursor=cursor->getCursorData();
    shortText=i18n("_:Undo descr.\nAdd operator");
    longText=i18n("Insertion of an operator element");

}


// ******  Add root,bracket etc command 

KFCAddReplacing::KFCAddReplacing(KFormulaContainer *document,FormulaCursor *cursor)
		        : KFormulaCommand(document,cursor)
{

}


bool KFCAddReplacing::undo(FormulaCursor *cursor)
{


    cursor->setCursorData(undocursor);
    BasicElement* element = cursor->removeEnclosingElement(BasicElement::beforeCursor);
    removedList.clear();
    removedList.append(element);
    
//    cursor->remove(removedList,BasicElement::beforeCursor);

    return true;
}


bool KFCAddReplacing::redo(FormulaCursor *cursor)
{

    cursor->setCursorData(cursordata);
    if(cursor->isSelection())
    {        
	cursor->replaceSelectionWith(removedList.first());
    }
    else
	cursor->insert(removedList.first());

//    cursor->insert(removedList);

    if(!insideElement)
        cursor->setSelection(false);
    else
        cursor->goInsideElement(insideElement);
	
    return true;
}


KFCAddRoot::KFCAddRoot(KFormulaContainer *document,FormulaCursor *cursor)
		        : KFCAddReplacing(document,cursor)
{

    RootElement* root = new RootElement();

    if(cursor->isSelection())
    {        
	cursor->replaceSelectionWith(root);
    }
    else
	cursor->insert(root);
        
	//cursor->setSelection(false);
    insideElement=root;
    cursor->goInsideElement(root);
    undocursor=cursor->getCursorData();


}

KFCAddFraction::KFCAddFraction(KFormulaContainer *document,FormulaCursor *cursor)
		        : KFCAddReplacing(document,cursor)
{

    FractionElement* frac = new FractionElement();

    if(cursor->isSelection())
    {        
	cursor->replaceSelectionWith(frac);
    }
    else
	cursor->insert(frac);
        
	//cursor->setSelection(false);
    insideElement=frac;
    cursor->goInsideElement(frac);
    undocursor=cursor->getCursorData();


}


// ******  Add Bracket command 

KFCAddBracket::KFCAddBracket(KFormulaContainer *document,FormulaCursor *cursor,QChar left,QChar right)
		        : KFCAddReplacing(document,cursor)

{

    BracketElement* bra = new BracketElement(left,right);

    if(cursor->isSelection())
    {        
	cursor->replaceSelectionWith(bra);
    }
    else
	cursor->insert(bra);
        
	//cursor->setSelection(false);
    insideElement=bra;
    cursor->goInsideElement(bra);
    undocursor=cursor->getCursorData();


}

// ******  Add matrix command 

KFCAddMatrix::KFCAddMatrix(KFormulaContainer *document,FormulaCursor *cursor,int r,int c)
		        : KFormulaCommand(document,cursor)
{

    MatrixElement* matrix = new MatrixElement(r,c);
    cursor->insert(matrix);
        //cursor->setSelection(false);
    insideElement=matrix;
    cursor->goInsideElement(matrix);


}

bool KFCAddMatrix::undo(FormulaCursor *cursor)
{

    cursor->setCursorData(cursordata);
    cursor->remove(removedList,BasicElement::afterCursor);

    return true;
}


bool KFCAddMatrix::redo(FormulaCursor *cursor)
{

    cursor->setCursorData(cursordata);
    cursor->insert(removedList);
    cursor->goInsideElement(insideElement);
	
    return true;
}

// ******  Add index command 

KFCAddIndex::KFCAddIndex(KFormulaContainer *document,FormulaCursor *cursor,int position)
		        : KFormulaCommand(document,cursor)
{
    indexExists=true;
    IndexElement* element = cursor->getActiveIndexElement();
    if (element == 0) {
        element = new IndexElement;
        cursor->replaceSelectionWith(element, BasicElement::beforeCursor);
	cursor->goInsideElement(element);
	enclosingcursor=cursor->getCursorData();
	indexExists=false;
    } 
    ElementIndexPtr index = element->getIndex(position);

    
    if (!index->hasIndex()) {
        SequenceElement* indexContent = new SequenceElement;
        index->setToIndex(cursor);
        indexcursor=cursor->getCursorData();  	
    
        cursor->insert(indexContent);
            //cursor->goInsideElement(indexContent);
    }
    else {
            //We are doing nothing relevant here!   
        nothing=true;
        index->moveToIndex(cursor, BasicElement::afterCursor);
        cursor->setSelection(false);
    }	
    
    undocursor=cursor->getCursorData();
}

bool KFCAddIndex::undo(FormulaCursor *cursor)
{
    if(nothing) 
	return true;


    cursor->setCursorData(undocursor);

//Remove the inserted index
    cursor->remove(removedList,BasicElement::beforeCursor);
    cerr << removedList.count() <<endl;    
//If we are the command that creates the indexelement we also remove it.    

    indexelem=0;
    if(!indexExists) {
        cursor->setCursorData(enclosingcursor);
	indexelem = cursor->removeEnclosingElement(BasicElement::beforeCursor);
    }
    cursor->setCursorData(cursordata);
    return true;

}


bool KFCAddIndex::redo(FormulaCursor *cursor)
{
    if(nothing) 
	return true;
    
    cursor->setCursorData(cursordata);

    if(!indexExists) {
        cursor->replaceSelectionWith(indexelem, BasicElement::beforeCursor);
    }
    
    cursor->setCursorData(indexcursor);  
    cursor->insert(removedList);

    return true;

}


