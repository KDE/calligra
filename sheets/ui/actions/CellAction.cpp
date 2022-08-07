/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2022 Tomas Mecir <mecirt@gmail.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "CellAction.h"
#include "Actions.h"

#include "ui/CellToolBase.h"
#include "ui/Selection.h"

#include <QAction>

using namespace Calligra::Sheets;

CellAction::CellAction(Actions *actions, const QString &actionName, const QString &caption, const QIcon &icon, const QString &tooltip)
    : m_actions(actions)
    , m_name(actionName)
{
    m_tool = actions->tool();
    if (!icon.isNull())
        m_action = new QAction(icon, caption, m_tool);
    else
        m_action = new QAction(caption, m_tool);
    m_tool->addCellAction(this);
    if (tooltip.length()) m_action->setToolTip(tooltip);
    connect(m_action, &QAction::triggered, this, &CellAction::triggered);
}

CellAction::~CellAction()
{
    // We cannot delete this as the tool will do so.
    // TODO - don't delete these in the tool ... we can only do this after everything has been moved out of tool code
    // delete m_action;
}

void CellAction::triggered()
{
    execute(m_tool->selection(), m_tool->selection()->activeSheet());
}

