/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2022 Tomas Mecir <mecirt@gmail.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "Actions.h"

#include "AdjustRowCol.h"
#include "Align.h"
#include "Angle.h"
#include "AutoFilter.h"
#include "AutoFormat.h"
#include "AutoSum.h"
#include "Border.h"
#include "CSVActions.h"
#include "Cells.h"
#include "Clear.h"
#include "Comment.h"
#include "Conditional.h"
#include "Consolidate.h"
#include "Copy.h"
#include "CustomLists.h"
#include "DocumentSettings.h"
#include "Editing.h"
#include "Fill.h"
#include "FindReplace.h"
#include "Font.h"
#include "Formula.h"
#include "GoalSeek.h"
#include "Goto.h"
#include "Indent.h"
#ifndef QT_NO_SQL
#include "InsertFromDatabase.h"
#endif
#include "InsertSeries.h"
#include "InsertSpecialChar.h"
#include "Inspector.h"
#include "Link.h"
#include "ListChoose.h"
#include "Merge.h"
#include "NamedAreas.h"
#include "PageBreak.h"
#include "Paste.h"
// #include "Pivot.h"
#include "SelectAll.h"
#ifndef NDEBUG
#include "ShowTableView.h"
#endif
#include "Sort.h"
#include "SpellCheck.h"
#include "Style.h"
#include "Styles.h"
#include "Subtotals.h"
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
    // AutoFilter
    addAction(new AutoFilter(this));
    // AutoFormat
    addAction(new AutoFormat(this));
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
    addAction(new BorderColor(this));
    // Cells
    addAction(new CellsInsert(this));
    addAction(new CellsRemove(this));
    // Clear
    addAction(new ClearAll(this));
    addAction(new ClearContents(this));
    // Comment
    addAction(new Comment(this));
    addAction(new ClearComment(this));
    // Conditional
    addAction(new SetCondition(this));
    addAction(new ClearCondition(this));
    // Consolidate
    addAction(new Consolidate(this));
    // Copy
    addAction(new Copy(this));
    addAction(new Cut(this));
    // CSVActions
    addAction(new InsertFromFile(this));
    addAction(new InsertFromClipboard(this));
    addAction(new TextToColumns(this));
    // CustomLists
    addAction(new ManageCustomLists(this));
    // DocumentSettings
    addAction(new DocumentSettings(this));
    // Editing
    addAction(new EditCell(this));
    addAction(new PermuteFixation(this));
    // Fill
    addAction(new Fill(this, FillManipulator::Up));
    addAction(new Fill(this, FillManipulator::Down));
    addAction(new Fill(this, FillManipulator::Left));
    addAction(new Fill(this, FillManipulator::Right));
    // FindReplace
    addAction(new FindReplaceAction(this));
    addAction(new FindNext(this));
    addAction(new FindPrevious(this));
    addAction(new Replace(this));
    // Font
    addAction(new Font(this));
    addAction(new FontSize(this));
    addAction(new FontColor(this));
    // Formula
    addAction(new InsertFormula(this));
    addAction(new FormulaSelection(this));
    // GoalSeek
    addAction(new GoalSeek(this));
    // Goto
    addAction(new Goto(this));
    // Indent
    addAction(new Indent(this, false));
    addAction(new Indent(this, true));
#ifndef QT_NO_SQL
    // InsertFromDatabase
    addAction(new InsertFromDatabase(this));
#endif
    // InsertSpecialChar
    addAction(new InsertSpecialChar(this));
    // InsertSeries
    addAction(new InsertSeries(this));
    // Inspector
    addAction(new Inspector(this));
    // Link
    addAction(new Link(this));
    addAction(new ClearLink(this));
    // ListChoose
    addAction(new ListChoose(this));
    // Merge
    addAction(new Merge(this, false, false));
    addAction(new Merge(this, true, false));
    addAction(new Merge(this, false, true));
    addAction(new RemoveMerge(this));
    // NamedAreas
    addAction(new AddNamedArea(this));
    addAction(new ManageNamedAreas(this));
    // PageBreak
    addAction(new PageBreakColumn(this));
    addAction(new PageBreakRow(this));
    // Paste
    addAction(new PasteRegular(this));
    addAction(new PasteSpecial(this));
    addAction(new PasteWithInsert(this));
    // Pivot
    //    addAction(new Pivot(this));
    // SelectAll
    addAction(new SelectAll(this));
#ifndef NDEBUG
    // ShowTableView
    addAction(new ShowTableView(this));
#endif
    // Sort
    addAction(new Sort(this));
    addAction(new SortInc(this));
    addAction(new SortDesc(this));
    // SpellCheck
    addAction(new SpellCheck(this));
    // Style
    addAction(new CellStyle(this));
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
    addAction(new NumberFormat(this));
    addAction(new ScientificFormat(this));
    addAction(new PercentFormat(this));
    addAction(new MoneyFormat(this));
    addAction(new DateFormat(this));
    addAction(new TimeFormat(this));
    addAction(new FillColor(this));
    addAction(new DefaultStyle(this));
    // Styles
    addAction(new StyleFromCell(this));
    addAction(new SetStyle(this));
    addAction(new ShowStyleManager(this));
    // Subtotals
    addAction(new Subtotals(this));
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
        const bool blocked = a->blockSignals(true);

        bool enabled = cellAction->shouldBeEnabled(readWrite, selection, activeCell);
        a->setEnabled(enabled);

        bool checked = cellAction->shouldBeChecked(selection, activeCell);
        a->setChecked(checked);

        a->blockSignals(blocked);

        cellAction->updateOnChange(selection, activeCell);
    }
}

void Actions::onEditorDeleted()
{
    for (CellAction *cellAction : cellActions) {
        cellAction->onEditorDeleted();
    }
}

void Actions::init()
{
    for (CellAction *cellAction : cellActions) {
        cellAction->init();
    }
}
