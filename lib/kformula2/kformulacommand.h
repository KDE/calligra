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

#include <qlist.h>

#include <kcommand.h>

#include "artwork.h"
#include "kformulacontainer.h"
#include "formulacursor.h"


/**
 * Base for all kformula commands.
 *
 * Each command works in the same way. The constructor sets up
 * everything. After the command is created you can execute
 * it. To create a command doesn't mean to execute it. These are
 * two different things.
 *
 * If the command execution fails or has nothing to do in the first place
 * you must not put it in the command history. @ref KFormulaCommand::isSenseless
 * will return <pre>true</pre> then.
 *
 * If you don't like what you've done feel free to unexecute.
 */
class KFormulaCommand : public KCommand
{
public:

    KFormulaCommand(const QString &name, KFormulaContainer* document);
    virtual ~KFormulaCommand();

    /**
     * Executes the command using the KFormulaContainer's active
     * cursor.
     */
    virtual void execute() = 0;

    /**
     * Undoes the command using the KFormulaContainer's active
     * cursor.
     */
    virtual void unexecute() = 0;

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
     * A command might have no effect.
     * @returns true if nothing happened.
     */
    virtual bool isSenseless() { return false; }

protected:

    FormulaCursor* getActiveCursor() { return doc->getActiveCursor(); }

    void destroyUndoCursor() { delete undocursor; undocursor = 0; }
    
    // I would prefer to have private attributes.
    
    /**
     * The container we belong to.
     */
    KFormulaContainer* doc;

    /**
     * Cursor position before the command execution.
     */
    FormulaCursor::CursorData* cursordata;

    /**
     * Cursor position after the command execution.
     */
    FormulaCursor::CursorData* undocursor;

    /**
     * Descriptions.
     */
    QString shortText;
    QString longText;

    /**
     * the list where all elements are stored that are removed
     * from the tree. Nearly each command needs it.
     */
    QList<BasicElement> removedList;
};


/**
 * Base for all commands that want to add a simple element.
 */
class KFCAdd : public KFormulaCommand
{
public:

    KFCAdd(const QString &name, KFormulaContainer* document);
 
    virtual void execute();
    virtual void unexecute();
};


/**
 * Command that is used to remove the current selection
 * if we want to replace it with another element.
 */
class KFCRemoveSelection : public KFormulaCommand
{
public:

    /**
     * generic add command, default implementation do nothing
     */
    KFCRemoveSelection(KFormulaContainer* document, BasicElement::Direction dir);
 
    virtual void execute();
    virtual void unexecute();

protected:
    BasicElement::Direction dir;   
};


/**
 * Command that is used to remove the currently
 * selected element.
 */
class KFCRemove : public KFormulaCommand
{
public:

    /**
     * generic add command, default implementation do nothing
     */
    KFCRemove(KFormulaContainer* document, BasicElement::Direction dir);
    ~KFCRemove();
    
    virtual void execute();
    virtual void unexecute();

private:

    /**
     * The element we might have extracted.
     */
    BasicElement* element;

    /**
     * If this is a complex remove command we need to remember two
     * cursor positions. The one after the first removal (this one)
     * and another at the end.
     */
    FormulaCursor::CursorData* simpleRemoveCursor;

    BasicElement::Direction dir;   
};


/**
 * Command to remove the parent element.
 */
class KFCRemoveEnclosing : public KFormulaCommand
{
public:
    KFCRemoveEnclosing(KFormulaContainer* document, BasicElement::Direction dir);
    ~KFCRemoveEnclosing();

    virtual void execute();
    virtual void unexecute();

    virtual bool isSenseless() { return element == 0; }
    
private:
    BasicElement* element;

    BasicElement::Direction direction;
};


class KFCAddText : public KFCAdd
{
public:
    /**
     * Build a addTextElement command and add
     * at cursor a textelement with content ch
     */
    KFCAddText(KFormulaContainer *document, QChar ch);
};


class KFCAddNumber : public KFCAdd
{
public:
    /**
     * Build a addNumberElement command and add
     * at cursor a numberelement with content ch
     */
    KFCAddNumber(KFormulaContainer *document, QChar ch);
};


class KFCAddOperator : public KFCAdd
{
public:
    /**
     * Build a addOperatorElement command and add
     * at cursor a operatorelement with content ch
     */
    KFCAddOperator(KFormulaContainer *document, QChar ch);
};


/**
 * Base for all commands that want to replace the current
 * selection with a new element and set the replaced elements
 * as main child.
 */
class KFCAddReplacing : public KFormulaCommand
{
public:
    KFCAddReplacing(const QString &name, KFormulaContainer* document);
    ~KFCAddReplacing();

    virtual void execute();
    virtual void unexecute();

protected:
    
    void setElement(BasicElement* e) { element = e; }
    
private:

    /**
     * The element that is to be inserted.
     */
    BasicElement* element;
};


class KFCAddRoot : public KFCAddReplacing
{
public:
    KFCAddRoot(KFormulaContainer* document);
};


class KFCAddFraction : public KFCAddReplacing
{
public:
    KFCAddFraction(KFormulaContainer* document);
};


class KFCAddBracket : public KFCAddReplacing
{
public:
    KFCAddBracket(KFormulaContainer* document, QChar left, QChar right);
};


class KFCAddSymbol : public KFCAddReplacing
{
public:
    KFCAddSymbol(KFormulaContainer* document, Artwork::SymbolType type);
};


class MatrixElement;

class KFCAddMatrix : public KFCAdd
{
public:
    KFCAddMatrix(KFormulaContainer* document, int r, int c);

    virtual void execute();

private:
    MatrixElement* matrix;
};


/**
 * Add an index. The element that gets the index needs to be there
 * already. Else we would not have gotten the ElementIndexPtr.
 */
class KFCAddGenericIndex : public KFCAdd
{
public:

    KFCAddGenericIndex(KFormulaContainer* document, ElementIndexPtr index);

private:
};


class IndexElement;

/**
 * Add an IndexElement.
 *
 * This is an exception as the element itself is not constructed
 * inside the constructor but by the caller.
 */
class KFCAddIndex : public KFCAddReplacing
{
public:

    KFCAddIndex(KFormulaContainer* document, IndexElement* element, ElementIndexPtr index);

    virtual void execute();
    virtual void unexecute();
    
private:
    KFCAddGenericIndex addGenericIndex;
};


/**
 * Command to insert stuff from the clipboard.
 */
class KFCPaste : public KFCAdd
{
public:
    KFCPaste(KFormulaContainer* document, QList<BasicElement>& list);
};

#endif // __KFORMULACONTAINER_H
