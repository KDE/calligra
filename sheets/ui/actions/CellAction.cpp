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

#include <KToggleAction>
#include <QAction>

using namespace Calligra::Sheets;

CellAction::CellAction(Actions *actions, const QString &actionName, const QString &caption, const QIcon &icon, const QString &tooltip)
    : m_actions(actions)
    , m_name(actionName)
    , m_caption(caption)
    , m_tooltip(tooltip)
    , m_icon(icon)
    , m_action(nullptr)
    , m_closeEditor(false)
{
}

QAction *CellAction::createAction() {
    QAction *res;
    if (!m_icon.isNull())
        res = new QAction(m_icon, m_caption, m_actions->tool());
    else
        res = new QAction(m_caption, m_actions->tool());
    if (m_tooltip.length()) res->setToolTip(m_tooltip);
    connect(res, &QAction::triggered, this, &CellAction::triggered);
    return res;
}

CellAction::~CellAction()
{
    // We cannot delete this as the tool will do so.
    // TODO - don't delete these in the tool ... we can only do this after everything has been moved out of tool code
    // delete m_action;
}

// This cannot be in the constructor as it uses virtual functions.
QAction *CellAction::action() {
    if (!m_action) m_action = createAction();
    return m_action;
}

// Triggerts the action manually. Keeping as a separate call in case we need to do things differently.
void CellAction::trigger()
{
    triggered();
}

void CellAction::triggered()
{
    Selection *sel = m_actions->tool()->selection();
    if (m_closeEditor) sel->emitCloseEditor(true);
    execute(sel, sel->activeSheet(), m_actions->tool()->canvas()->canvasWidget());
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


// *********** ToggleableCellAction ***********


ToggleableCellAction::ToggleableCellAction(Actions *actions, const QString &actionName, const QString &caption, const QIcon &icon, const QString &tooltip)
    :
        CellAction(actions, actionName, caption, icon, tooltip)
{
}

ToggleableCellAction::~ToggleableCellAction() {
    // nothing here
}

QAction *ToggleableCellAction::createAction()
{
    if (!m_icon.isNull())
        m_toggleAction = new KToggleAction(m_icon, m_caption, m_actions->tool());
    else
        m_toggleAction = new KToggleAction(m_caption, m_actions->tool());
    if (m_tooltip.length()) m_toggleAction->setToolTip(m_tooltip);
    connect(m_toggleAction, &KToggleAction::triggered, this, &ToggleableCellAction::triggered);

    return m_toggleAction;
}

void ToggleableCellAction::trigger() {
    triggered(!m_toggleAction->isChecked());
}

void ToggleableCellAction::triggered(bool enabled) {
    Selection *sel = m_actions->tool()->selection();
    if (m_closeEditor) sel->emitCloseEditor(true);
    executeToggled(enabled, sel, sel->activeSheet(), m_actions->tool()->canvas()->canvasWidget());
}


bool ToggleableCellAction::shouldBeChecked(Selection *selection, const Cell &activeCell)
{
    return checkedForSelection(selection, activeCell);
}

bool ToggleableCellAction::checkedForSelection(Selection *, const Cell &)
{
    return false;
}



