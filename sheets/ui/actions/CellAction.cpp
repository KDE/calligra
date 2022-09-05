/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2022 Tomas Mecir <mecirt@gmail.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "CellAction.h"
#include "Actions.h"

#include "core/Sheet.h"
#include "ui/CellToolBase.h"
#include "ui/Selection.h"

#include <KoCanvasBase.h>

#include <QAction>

using namespace Calligra::Sheets;

CellAction::CellAction(Actions *actions, const QString &actionName, const QString &caption, const QIcon &icon, const QString &tooltip)
    : m_actions(actions)
    , m_name(actionName)
    , m_closeEditor(false)
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

// Triggerts the action manually. Keeping as a separate call in case we need to do things differently.
void CellAction::trigger()
{
    triggered();
}

void CellAction::triggered()
{
    if (m_closeEditor) m_tool->selection()->emitCloseEditor(true);
    execute(m_tool->selection(), m_tool->selection()->activeSheet(), m_tool->canvas()->canvasWidget());
}

bool CellAction::shouldBeEnabled(bool readWrite, Selection *selection, const Cell &activeCell)
{
    if ((!readWrite) && (!enabledIfReadOnly())) return false;

    bool prot = false;
    if (selection->activeSheet()->isProtected()) {
        prot = true;
        // Unprotected singular cell in a protected sheet?
        // TODO - also allow multi-cell selections consisting solely of protected cells ...
        if (selection->isSingular() && (!activeCell.isNull())) {
            const Style style = activeCell.style();
            // TODO if more cells enabled, 
            if (style.notProtected()) prot = false;
        }
    }
    if (prot && (!enabledIfProtected())) return false;

    return enabledForSelection(selection, activeCell);
}

bool CellAction::enabledForSelection(Selection *, const Cell &)
{
    return true;
}


