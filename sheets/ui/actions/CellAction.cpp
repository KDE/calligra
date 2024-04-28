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

QAction *CellAction::createAction()
{
    QAction *res;
    if (!m_icon.isNull())
        res = new QAction(m_icon, m_caption, m_actions->tool());
    else
        res = new QAction(m_caption, m_actions->tool());
    if (m_tooltip.length())
        res->setToolTip(m_tooltip);
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
QAction *CellAction::action()
{
    if (!m_action)
        m_action = createAction();
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
    if (m_closeEditor)
        sel->emitCloseEditor(true);
    execute(sel, sel->activeSheet(), m_actions->tool()->canvas()->canvasWidget());
}

bool CellAction::shouldBeEnabled(bool readWrite, Selection *selection, const Cell &activeCell)
{
    if ((!readWrite) && (!enabledIfReadOnly()))
        return false;

    bool prot = selection->isProtected();
    if (prot && (!enabledIfProtected()))
        return false;

    return enabledForSelection(selection, activeCell);
}

bool CellAction::enabledForSelection(Selection *, const Cell &)
{
    return true;
}

QRect CellAction::shrinkToUsedArea(QRect rect, Sheet *sheet)
{
    // If it's a single cell, keep it as it is.
    if ((rect.height() == 1) && (rect.width() == 1))
        return rect;

    QRect used = sheet->usedArea();
    int usex = used.right();
    int usey = used.bottom();
    if (rect.top() > usey)
        rect.setBottom(rect.top());
    else if (rect.bottom() > usey)
        rect.setBottom(usey);
    if (rect.left() > usex)
        rect.setRight(rect.left());
    else if (rect.right() > usex)
        rect.setRight(usex);

    // Remove empty rows/columns
    while (rect.bottom() > rect.top()) {
        int y = rect.bottom();
        bool used = false;
        for (int x = rect.left(); x <= rect.right(); ++x) {
            Value v = CellBase(sheet, x, y).value();
            if (!v.isNull())
                used = true;
        }
        if (used)
            break;
        rect.setBottom(y - 1);
    }
    while (rect.right() > rect.left()) {
        int x = rect.right();
        bool used = false;
        for (int y = rect.top(); y <= rect.bottom(); ++y) {
            Value v = CellBase(sheet, x, y).value();
            if (!v.isNull())
                used = true;
        }
        if (used)
            break;
        rect.setRight(x - 1);
    }

    return rect;
}

QRect CellAction::extendSelectionToColumn(const CellBase &cell, bool numeric)
{
    SheetBase *sheet = cell.sheet();
    int x = cell.column();
    int y = cell.row();
    Value cur = cell.value();
    QRect res(x, y, 1, 1);

    int yy = y;
    // We do not check the current x/y cell - this lets us place the cursor directly under a column and have it extend to it
    while (yy > 1) {
        yy--;
        Value v = CellBase(sheet, x, yy).value();
        if (numeric) {
            if (v.isNumber())
                continue;
        } else {
            if (!v.isEmpty())
                continue;
        }
        res.setTop(yy + 1);
        break;
    }

    // If we are in the middle of a column, expand downwards too
    if ((numeric && cur.isNumber()) || ((!numeric) && (!cur.isEmpty()))) {
        yy = y;
        while (yy < KS_rowMax) {
            yy++;
            Value v = CellBase(sheet, x, yy).value();
            if (numeric) {
                if (v.isNumber())
                    continue;
            } else {
                if (!v.isEmpty())
                    continue;
            }
            res.setBottom(yy - 1);
            break;
        }
    }

    return res;
}

// Same as the above function, just swaps col/row
QRect CellAction::extendSelectionToRow(const CellBase &cell, bool numeric)
{
    SheetBase *sheet = cell.sheet();
    int x = cell.column();
    int y = cell.row();
    Value cur = cell.value();
    QRect res(x, y, 1, 1);

    int xx = x;
    // We do not check the current x/y cell - this lets us place the cursor directly next to a row and have it extend to it
    while (xx > 1) {
        xx--;
        Value v = CellBase(sheet, xx, y).value();
        if (numeric) {
            if (v.isNumber())
                continue;
        } else {
            if (!v.isEmpty())
                continue;
        }
        res.setLeft(xx + 1);
        break;
    }

    // If we are in the middle of a column, expand downwards too
    if ((numeric && cur.isNumber()) || ((!numeric) && (!cur.isEmpty()))) {
        xx = x;
        while (xx < KS_colMax) {
            xx++;
            Value v = CellBase(sheet, xx, y).value();
            if (numeric) {
                if (v.isNumber())
                    continue;
            } else {
                if (!v.isEmpty())
                    continue;
            }
            res.setRight(xx - 1);
            break;
        }
    }

    return res;
}

QRect CellAction::extendSelectionToRange(const CellBase &cell, bool numeric)
{
    QRect r1 = extendSelectionToColumn(cell, numeric);
    CellBase topCell(cell.sheet(), cell.column(), r1.top());
    QRect r2 = extendSelectionToRow(topCell, numeric);
    QRect res(QPoint(r2.left(), r1.top()), QPoint(r2.right(), r1.bottom()));
    return res;
}

// *********** ToggleableCellAction ***********

ToggleableCellAction::ToggleableCellAction(Actions *actions, const QString &actionName, const QString &caption, const QIcon &icon, const QString &tooltip)
    : CellAction(actions, actionName, caption, icon, tooltip)
{
}

ToggleableCellAction::~ToggleableCellAction()
{
    // nothing here
}

QAction *ToggleableCellAction::createAction()
{
    if (!m_icon.isNull())
        m_toggleAction = new KToggleAction(m_icon, m_caption, m_actions->tool());
    else
        m_toggleAction = new KToggleAction(m_caption, m_actions->tool());
    if (m_tooltip.length())
        m_toggleAction->setToolTip(m_tooltip);
    connect(m_toggleAction, &KToggleAction::triggered, this, &ToggleableCellAction::triggered);

    return m_toggleAction;
}

void ToggleableCellAction::trigger()
{
    triggered(!m_toggleAction->isChecked());
}

void ToggleableCellAction::triggered(bool enabled)
{
    Selection *sel = m_actions->tool()->selection();
    if (m_closeEditor)
        sel->emitCloseEditor(true);
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
