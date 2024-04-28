/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2009 Jeremias Epperlein

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef FORMULACOMMANDUPDATE_H
#define FORMULACOMMANDUPDATE_H
#include <kundo2command.h>

class FormulaCommand;
class KoFormulaShape;

/**
 *
 * This class is used to wrap FormulaCommands, which are part of KFormulaLib
 * and therefore can't notify the tool and shape of changes (as they don't know
 * about them). This notification it done by this classes undo / redo methods
 * after calling the respective methods from the wrapped class
 *
 **/

class FormulaCommandUpdate : public KUndo2Command
{
public:
    FormulaCommandUpdate(KoFormulaShape *shape, FormulaCommand *command);

    /// Execute the command
    void redo() override;

    /// Revert the actions done in redo()
    void undo() override;

private:
    /// The BasicElement that owns the newly added Text
    FormulaCommand *m_command;
    KoFormulaShape *m_shape;
};

#endif // FORMULACOMMANDUPDATE_H
