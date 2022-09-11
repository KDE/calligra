/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2022 Tomas Mecir <mecirt@gmail.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "Actions.h"

#include "AdjustRowCol.h"
#include "AutoSum.h"
#include "CSVActions.h"
#include "Fill.h"
#include "InsertSeries.h"
#include "InsertSpecialChar.h"
#include "Link.h"
#include "Merge.h"
#include "Sort.h"
#include "TextCase.h"
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
    // AutoSum
    addAction(new AutoSum(this));
    // CSVActions
    addAction(new InsertFromFile(this));
    addAction(new InsertFromClipboard(this));
    addAction(new TextToColumns(this));
    // Fill
    addAction(new Fill(this, FillManipulator::Up));
    addAction(new Fill(this, FillManipulator::Down));
    addAction(new Fill(this, FillManipulator::Left));
    addAction(new Fill(this, FillManipulator::Right));
    // InsertSpecialChar
    addAction(new InsertSpecialChar(this));
    // InsertSeries
    addAction(new InsertSeries(this));
    // Link
    addAction(new Link(this));
    addAction(new ClearLink(this));
    // Merge
    addAction(new Merge(this, false, false));
    addAction(new Merge(this, true, false));
    addAction(new Merge(this, false, true));
    addAction(new RemoveMerge(this));
    // Sort
    addAction(new Sort(this));
    addAction(new SortInc(this));
    addAction(new SortDesc(this));
    // TextCase
    addAction(new TextCase(this, CaseManipulator::Upper));
    addAction(new TextCase(this, CaseManipulator::Lower));
    addAction(new TextCase(this, CaseManipulator::FirstUpper));
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


