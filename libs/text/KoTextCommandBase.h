/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2007 Thomas Zander <zander@kde.org>
 * SPDX-FileCopyrightText: 2011 Boudewijn Rempt <boud@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */
#ifndef KO_TEXT_COMMAND_BASE_H
#define KO_TEXT_COMMAND_BASE_H

#include <kundo2command.h>

#include "kotext_export.h"

class KOTEXT_EXPORT KoUndoableTool
{
public:
    virtual ~KoUndoableTool() = default;
    virtual void setAddUndoCommandAllowed(bool allowed) = 0;
};

/**
 * Base class for all commands that work together with a tool that needs to handle undo/redo
 * in a tricky way.
 * Due to the fact that QTextDocument has its own undo queue we need to do some trickery
 * to integrate that into the apps.
 * If your command in some way changes the document, it will create unwanted undo commands in the undoStack
 * unless you inherit from this class and simply implement your undo and redo like this:
@code
void MyCommand::redo() {
    TextCommandBase::redo();
    UndoRedoFinalizer finalizer(m_tool);
    // rest code
}

void MyCommand::undo() {
    TextCommandBase::undo();
    UndoRedoFinalizer finalizer(m_tool);
    // rest code
}
@endcode
 * @see TextTool::addCommand()
 */
class KOTEXT_EXPORT KoTextCommandBase : public KUndo2Command
{
public:
    /// constructor
    explicit KoTextCommandBase(KUndo2Command *parent);
    ~KoTextCommandBase() override;

    /// method called by the tool.
    void setTool(KoUndoableTool *tool);

    // reimplemented from KUndo2Command
    void redo() override;
    // reimplemented from KUndo2Command
    void undo() override;

    /// Sets the m_allowAddUndoCommand of the associated tool
    void setAllow(bool set);

protected:
    class KOTEXT_EXPORT UndoRedoFinalizer
    {
    public:
        explicit UndoRedoFinalizer(KoTextCommandBase *parent)
            : m_parent(parent)
        {
        }
        ~UndoRedoFinalizer();

    private:
        KoTextCommandBase *m_parent;
    };

    KoUndoableTool *m_tool;
};

#endif
