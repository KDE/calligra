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

#ifndef __KFORMULACOMMAND_H
#define __KFORMULACOMMAND_H

#include "kformulacontainer.h"
#include "formulacursor.h"

class KFormulaCommand 
{
public:
   /**
    * Execute for the first time and create command for undo/redo
    * stacks, default implementation do nothing into document at
    * position cursor.
    * Cursor position is saved.
    */
    KFormulaCommand(KFormulaContainer *document,FormulaCursor *cursor);

    /**
     * Delete the cursor.
     */
    virtual ~KFormulaCommand();

    /**
     * Undo the command
     * @return true if undo is done.
     * Default implementation do nothing.
     */
     virtual bool undo(FormulaCursor *) { return true; } //undo of nothing is nothing
 
    /**
     * Redo the command
     * @return true if redo is done.
     * Default implementation do nothing.
     */
     virtual bool redo(FormulaCursor *) {return true; }  //undo of nothing is nothing

protected:
     
     KFormulaContainer *doc;
     FormulaCursor::CursorData *cursordata;

};

class KFCAdd : public KFormulaCommand
{
public:
   /**
    * generic add command, default implementation do nothing
    */
    KFCAdd(KFormulaContainer *document,FormulaCursor* cursor);
 
    virtual bool undo(FormulaCursor *cursor);
    virtual bool redo(FormulaCursor *cursor);

protected:

     QList<BasicElement> removedList;
   
};

class KFCRemoveSelection : public KFormulaCommand
{
public:
   /**
    * generic add command, default implementation do nothing
    */
    KFCRemoveSelection(KFormulaContainer *document,FormulaCursor* cursor,BasicElement::Direction dir);
 
    virtual bool undo(FormulaCursor *cursor);
    virtual bool redo(FormulaCursor *cursor);

protected:

     QList<BasicElement> removedList;
     BasicElement::Direction dir;   
};

class KFCAddText : public KFCAdd
{
public:
   /**
    * Build a addTextElement command and add
    * at cursor a textelement with content ch
    */
    KFCAddText(KFormulaContainer *document,FormulaCursor* cursor, QChar ch);
   
};

class KFCAddRoot : public KFCAdd
{
public:
   /**
    * Build a addRootElement command and add
    * at cursor a root element
    */
    KFCAddRoot(KFormulaContainer *document,FormulaCursor* cursor);
  
};

class KFCAddMatrix : public KFCAdd
{
public:
   /**
    * Build a addRootElement command and add
    * at cursor a root element
    */
    KFCAddMatrix(KFormulaContainer *document,FormulaCursor* cursor,int r,int c);
  
};


#if 0
   void addOperator(FormulaCursor* cursor, QChar ch);
    void addBracket(FormulaCursor* cursor, char left, char right);
    void addFraction(FormulaCursor* cursor);
    void addRoot(FormulaCursor* cursor);
    void addSymbol(FormulaCursor* cursor, Artwork::SymbolType type);
    void addMatrix(FormulaCursor* cursor, int rows, int columns);
    void addLowerRightIndex(FormulaCursor* cursor);
    void addUpperRightIndex(FormulaCursor* cursor);

    void removeSelection(FormulaCursor* cursor, BasicElement::Direction);
#endif
    

#endif // __KFORMULACONTAINER_H
