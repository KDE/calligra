/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2022 Tomas Mecir <mecirt@gmail.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "Actions.h"

#include "AdjustRowCol.h"
#include "InsertSeries.h"
#include "InsertSpecialChar.h"
#include "Link.h"
#include "Sort.h"
#include "Validity.h"


#include "engine/SheetsDebug.h"

using namespace Calligra::Sheets;

Actions::Actions(CellToolBase *tool)
    : m_tool(tool)
{
    createActions();
}

Actions::~Actions()
{
    qDeleteAll(cellActions);
}

void Actions::createActions()
{
    // AdjustRowCol
    addAction(new InsertRemoveRowCol(this, false, false));
    addAction(new InsertRemoveRowCol(this, false, true));
    addAction(new InsertRemoveRowCol(this, true, false));
    addAction(new InsertRemoveRowCol(this, true, true));
    addAction(new ShowHideRowCol(this, false, false));
    addAction(new ShowHideRowCol(this, false, true));
    addAction(new ShowHideRowCol(this, true, false));
    addAction(new ShowHideRowCol(this, true, true));
    addAction(new ShowRowColQuery(this, false));
    addAction(new ShowRowColQuery(this, true));
    addAction(new EqualizeRowCol(this, false));
    addAction(new EqualizeRowCol(this, true));
    addAction(new AdjustRowCol(this, true, true));
    addAction(new AdjustRowCol(this, true, false));
    addAction(new AdjustRowCol(this, false, true));
    addAction(new ResizeRowCol(this, false));
    addAction(new ResizeRowCol(this, true));
    // InsertSpecialChar
    addAction(new InsertSpecialChar(this));
    // InsertSeries
    addAction(new InsertSeries(this));
    // Link
    addAction(new Link(this));
    addAction(new ClearLink(this));
    // Sort
    addAction(new Sort(this));
    addAction(new SortInc(this));
    addAction(new SortDesc(this));
    // Validity
    addAction(new SetValidity(this));
    addAction(new ClearValidity(this));

    // TODO!

}

void Actions::addAction(CellAction *a)
{
    QString name = a->name();
    if (cellActions.contains(name))
        warnSheets << "Duplicated cell action: " << name;

    cellActions[name] = a;
}

CellAction *Actions::cellAction(const QString &name)
{
    if (cellActions.contains(name))
        return cellActions[name];
    return nullptr;
}

QAction *Actions::action(const QString &name)
{
    CellAction *a = cellAction(name);
    return a ? a->action() : nullptr;
}

void Actions::updateOnChange(bool readWrite, Selection *selection, const Cell &activeCell)
{
    for (CellAction *cellAction : cellActions) {
        bool enabled = cellAction->shouldBeEnabled(readWrite, selection, activeCell);
        cellAction->action()->setEnabled(enabled);
    }
}


