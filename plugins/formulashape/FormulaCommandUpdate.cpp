/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2009 Jeremias Epperlein <jeeree@web.de>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "FormulaCommandUpdate.h"
#include "FormulaCommand.h"
#include "FormulaCursor.h"
#include "FormulaData.h"
#include <KLocalizedString>

FormulaCommandUpdate::FormulaCommandUpdate(KoFormulaShape *shape, FormulaCommand *command)
    : KUndo2Command()
{
    m_shape = shape;
    m_command = command;
    setText(m_command->text());
}

void FormulaCommandUpdate::redo()
{
    m_shape->update();
    m_command->redo();
    m_shape->updateLayout();
    m_shape->update();
    m_shape->formulaData()->notifyDataChange(m_command, false);
}

void FormulaCommandUpdate::undo()
{
    m_shape->update();
    m_command->undo();
    m_shape->updateLayout();
    m_shape->update();
    m_shape->formulaData()->notifyDataChange(m_command, true);
}
