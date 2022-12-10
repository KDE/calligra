/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2022 Tomas Mecir <mecirt@gmail.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "Actions.h"

#include "AdjustRowCol.h"
#include "Align.h"
#include "Angle.h"
#include "AutoSum.h"
#include "Border.h"
#include "Comment.h"
#include "CSVActions.h"
#include "Fill.h"
#include "Indent.h"
#include "InsertSeries.h"
#include "InsertSpecialChar.h"
#include "Link.h"
#include "Merge.h"
#include "NamedAreas.h"
#include "Sort.h"
#include "Style.h"
#include "TextCase.h"
#include "Validity.h"

#include "engine/SheetsDebug.h"
#include "ui/CellToolBase.h"


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
    // Align
    addAction(new AlignLeft(this));
    addAction(new AlignRight(this));
    addAction(new AlignCenter(this));
    addAction(new AlignTop(this));
    addAction(new AlignBottom(this));
    addAction(new AlignMiddle(this));
    // Angle
    addAction(new Angle(this));
    // AutoSum
    addAction(new AutoSum(this));
    // Border
    addAction(new BorderLeft(this));
    addAction(new BorderRight(this));
    addAction(new BorderTop(this));
    addAction(new BorderBottom(this));
    addAction(new BorderAll(this));
    addAction(new BorderNone(this));
    addAction(new BorderOutline(this));
    // Comment
    addAction(new Comment(this));
    addAction(new ClearComment(this));
    // CSVActions
    addAction(new InsertFromFile(this));
    addAction(new InsertFromClipboard(this));
    addAction(new TextToColumns(this));
    // Fill
    addAction(new Fill(this, FillManipulator::Up));
    addAction(new Fill(this, FillManipulator::Down));
    addAction(new Fill(this, FillManipulator::Left));
    addAction(new Fill(this, FillManipulator::Right));
    // Indent
    addAction(new Indent(this, false));
    addAction(new Indent(this, true));
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
    // NamedAreas
    addAction(new AddNamedArea(this));
    addAction(new ManageNamedAreas(this));
    // Sort
    addAction(new Sort(this));
    addAction(new SortInc(this));
    addAction(new SortDesc(this));
    // Style
    addAction(new Bold(this));
    addAction(new Italic(this));
    addAction(new Underline(this));
    addAction(new Strikeout(this));
    addAction(new WrapText(this));
    addAction(new VerticalText(this));
    addAction(new IncreaseFontSize(this));
    addAction(new DecreaseFontSize(this));
    addAction(new IncreasePrecision(this));
    addAction(new DecreasePrecision(this));
    addAction(new PercentFormat(this));
    addAction(new MoneyFormat(this));
    addAction(new DefaultStyle(this));
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
    m_tool->addCellAction(a);
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
        QAction *a = cellAction->action();

        bool enabled = cellAction->shouldBeEnabled(readWrite, selection, activeCell);
        a->setEnabled(enabled);

        bool checked = cellAction->shouldBeChecked(selection, activeCell);
        const bool blocked = a->blockSignals(true);
        a->setChecked(checked);
        a->blockSignals(blocked);
    }
}


