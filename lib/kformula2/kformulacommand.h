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
 * you must not put it in the command history. @ref isSenseless
 * will return true then.
 *
 * If you don't like what you've done feel free to @ref unexecute .
 */
class KFormulaCommand : public KCommand
{
public:

    /**
     * Sets up the command. Be careful not to change the cursor in
     * the constructor of any command. Each command must use the selection
     * it finds when it is executed for the first time. This way
     * you can use the @ref KMacroCommand .
     *
     * @param name a description to be used as menu entry.
     * @param document the container we are working for.
     */
    KFormulaCommand(const QString& name, KFormulaContainer* document);
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
     * A command might have no effect.
     * @returns true if nothing happened.
     */
    virtual bool isSenseless() { return false; }

    /**
     * debug only.
     */
    static int getEvilDestructionCount() { return evilDestructionCount; }

protected:

    /**
     * @returns the cursor that is to be used to @ref execute the
     * command.
     */
    FormulaCursor* getExecuteCursor();

    /**
     * @returns the cursor that is to be used to @ref unexecute the
     * command.
     */
    FormulaCursor* getUnexecuteCursor();

    /**
     * Sets the cursor that is to be used to @ref unexecute the
     * command. This has to be called by @ref execute after the
     * formula has been changed but before the cursor has been
     * normalized.
     */
    void setUnexecuteCursor(FormulaCursor* cursor);

    /**
     * @returns the cursor that is active. It will be used to @ref execute
     * the command.
     */
    FormulaCursor* getActiveCursor() { return doc->getActiveCursor(); }

    /**
     * Tells the document to check if the formula changed.
     * Needs to be called by each @ref execute and @ref unexecute .
     */
    void testDirty() { doc->testDirty(); }

    /**
     * @returns our document.
     */
    KFormulaContainer* getDocument() const { return doc; }

    // I would prefer to have private attributes.
    
    /**
     * the list where all elements are stored that are removed
     * from the tree. Nearly each command needs it.
     */
    QList<BasicElement> removedList;

private:
    
    void destroyUndoCursor() { delete undocursor; undocursor = 0; }

    /**
     * Saves the cursor that is used to execute the command.
     */
    void setExecuteCursor(FormulaCursor* cursor);
    
    /**
     * Cursor position before the command execution.
     */
    FormulaCursor::CursorData* cursordata;

    /**
     * Cursor position after the command execution.
     */
    FormulaCursor::CursorData* undocursor;

    /**
     * The container we belong to.
     */
    KFormulaContainer* doc;

    // debug
    static int evilDestructionCount;
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

    /**
     * Collects all elements that are to be added.
     */
    void addElement(BasicElement* element) { removedList.append(element); }
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
    KFCRemoveSelection(KFormulaContainer* document,
                       BasicElement::Direction dir = BasicElement::beforeCursor);
 
    virtual void execute();
    virtual void unexecute();

private:
    BasicElement::Direction dir;   
};


/**
 * Removes the current selection and adds the any new elements
 * afterwards.
 */
class KFCReplace : public KFCAdd
{
public:

    KFCReplace(const QString &name, KFormulaContainer* document);
    ~KFCReplace();
    
    virtual void execute();
    virtual void unexecute();

private:

    /**
     * The command that needs to be executed first if there is a selection.
     */ 
    KFCRemoveSelection* removeSelection;
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


/**
 * The command that takes the main child out of the selected
 * element and replaces the element with it.
 */
class KFCAddReplacing : public KFormulaCommand
{
public:
    KFCAddReplacing(const QString &name, KFormulaContainer* document);
    ~KFCAddReplacing();

    virtual void execute();
    virtual void unexecute();

    void setElement(BasicElement* e) { element = e; }
    
private:

    /**
     * The element that is to be inserted.
     */
    BasicElement* element;
};


class MatrixElement;

class KFCAddMatrix : public KFCReplace
{
public:
    KFCAddMatrix(KFormulaContainer* document, int r, int c);

    virtual void execute();

private:
    MatrixElement* matrix;
};


/**
 * Add an index. The element that gets the index needs to be there
 * already.
 */
class KFCAddGenericIndex : public KFCAdd
{
public:

    KFCAddGenericIndex(KFormulaContainer* document, ElementIndexPtr index);

    virtual void execute();

private:
    ElementIndexPtr index;
};


class IndexElement;

/**
 * Add an IndexElement.
 */
class KFCAddIndex : public KFCAddReplacing
{
public:

    KFCAddIndex(KFormulaContainer* document, IndexElement* element, ElementIndexPtr index);
    ~KFCAddIndex();

    virtual void execute();
    virtual void unexecute();
    
private:
    KFCAddGenericIndex addIndex;
};

#endif // __KFORMULACOMMAND_H
