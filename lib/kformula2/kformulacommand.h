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

    /**
     * @return a short i18n text that describes the undo/redo
     * action. This has to be used in menus entries.
     */
     QString getShortDescription() { return shortText; } 

    /**
     * @return a long i18n text that describes the undo/redo
     * action. This can be used as tooltip.
     */
     QString getLongDescription() { return longText; } 

    /**
     * @return true if the command you asked for do nothing relevant,
     * may be it only move the cursor, in this case the constructor will
     * move the cursor but the undo/redo will do nothing.
     */
     
     virtual bool isDoNothing() { return false; }  //this class is doing nothing but this function
                                                   //Is used by many childclasses     

protected:
     
     KFormulaContainer *doc;
     FormulaCursor::CursorData *cursordata;  //Cursor before the command execution
     FormulaCursor::CursorData *undocursor;  //Cursor after the command execution
     QString shortText;
     QString longText;
     
    
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
     BasicElement *insideElement;   
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

class KFCAddNumber : public KFCAdd
{
public:
   /**
    * Build a addNumberElement command and add
    * at cursor a numberelement with content ch
    */
    KFCAddNumber(KFormulaContainer *document,FormulaCursor* cursor, QChar ch);
   
};

class KFCAddOperator : public KFCAdd
{
public:
   /**
    * Build a addOperatorElement command and add
    * at cursor a operatorelement with content ch
    */
    KFCAddOperator(KFormulaContainer *document,FormulaCursor* cursor, QChar ch);
   
};

class KFCAddReplacing : public KFormulaCommand
{
public:
   /**
    * abstract class hinerited by bracket,fraction and root
    */
    KFCAddReplacing(KFormulaContainer *document,FormulaCursor* cursor);

    virtual bool undo(FormulaCursor *cursor);
    virtual bool redo(FormulaCursor *cursor);

protected:

     QList<BasicElement> removedList;
     BasicElement *insideElement;   
  
};


class KFCAddRoot : public KFCAddReplacing
{
public:
   /**
    * Build a addRootElement command and add
    * at cursor a root element
    */
    KFCAddRoot(KFormulaContainer *document,FormulaCursor* cursor);
 
  
};

class KFCAddFraction : public KFCAddReplacing
{
public:
   /**
    * Build a addFractionElement command and add
    * at cursor a fraction element
    */
    KFCAddFraction(KFormulaContainer *document,FormulaCursor* cursor);
 
  
};

class KFCAddBracket : public KFCAddReplacing
{
public:
   /**
    * Build a addBracketElement command and add
    * at cursor a braket element
    */
    KFCAddBracket(KFormulaContainer *document,FormulaCursor* cursor,QChar left,QChar right);
 
};

class KFCAddMatrix : public KFormulaCommand
{
public:
   /**
    * Build a addRootElement command and add
    * at cursor a root element
    */
    KFCAddMatrix(KFormulaContainer *document,FormulaCursor* cursor,int r,int c);

    virtual bool undo(FormulaCursor *cursor);
    virtual bool redo(FormulaCursor *cursor);

protected:

     QList<BasicElement> removedList;
     BasicElement *insideElement;   
  
};

class KFCAddIndex : public KFormulaCommand
{
public:
   /**
    * Add an index at position.
    */
    KFCAddIndex(KFormulaContainer *document,FormulaCursor* cursor,int position);

    virtual bool undo(FormulaCursor *cursor);
    virtual bool redo(FormulaCursor *cursor);

    /**
     * @return true if the command you asked for do nothing relevant,
     * may be it only move the cursor, in this case the constructor will
     * move the cursor but the undo/redo will do nothing.
     */
     
     virtual bool isDoNothing() { return nothing; }  



protected:


     QList<BasicElement> removedList;
     BasicElement *indexelem;   
     FormulaCursor::CursorData *indexcursor;  //Cursor for index insertion (lower orupper,left or right)
     FormulaCursor::CursorData *enclosingcursor;  //Cursor for index insertion (lower orupper,left or right)
              
     bool nothing;
     bool indexExists;
     
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
