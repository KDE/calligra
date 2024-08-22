/* This file is part of the KDE project
   SPDX-FileCopyrightText: 1998-2023 The Calligra Team <calligra-devel@kde.org>
   SPDX-FileCopyrightText: 2023 Tomas Mecir <mecirt@gmail.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "FindReplace.h"
#include "Actions.h"
#include "Comment.h" // for the command

#include "core/CellStorage.h"
#include "core/Sheet.h"
#include "ui/CellToolBase.h"
#include "ui/commands/DataManipulators.h"

#include "KoCanvasBase.h"
#include "KoDialog.h"

#include <KLocalizedString>
#include <KStandardAction>
#include <kfind.h>
#include <kreplace.h>

using namespace Calligra::Sheets;

FindReplaceAction::FindReplaceAction(Actions *actions)
    : CellAction(actions, "edit_find", QString(), QIcon(), QString())
{
    m_findOptions = 0;
    m_findLeftColumn = 0;
    m_findRightColumn = 0;
    m_findTopRow = 0;
    m_findBottomRow = 0;
    m_typeValue = FindOption::Value;
    m_directionValue = FindOption::Row;
    m_find = nullptr;
    m_replace = nullptr;
    m_replaceCommand = nullptr;

    m_currentSheet = nullptr;
    m_firstSheet = nullptr;
}

FindReplaceAction::~FindReplaceAction()
{
    delete m_find;
    delete m_replace;
}

QAction *FindReplaceAction::createAction()
{
    QAction *action = KStandardAction::find(nullptr, nullptr, m_actions->tool());
    connect(action, &QAction::triggered, this, &FindReplaceAction::triggered);
    return action;
}

void FindReplaceAction::execute(Selection *selection, Sheet *sheet, QWidget *canvasWidget)
{
    m_selection = selection;
    FindDlg *dialog = new FindDlg(canvasWidget, "Find", m_findOptions, m_findStrings);
    dialog->setHasSelection(!selection->isSingular());
    dialog->setHasCursor(true);
    if (KFindDialog::Accepted != dialog->exec()) {
        delete dialog;
        return;
    }

    // Save for next time
    m_findOptions = dialog->options();
    m_findStrings = dialog->findHistory();
    m_typeValue = dialog->searchType();
    m_directionValue = dialog->searchDirection();

    // Create the KFind object
    delete m_find;
    delete m_replace;
    m_find = new KFind(dialog->pattern(), dialog->options(), canvasWidget);
    m_replace = nullptr;
    m_replaceCommand = nullptr;

    m_currentSheet = sheet;
    m_firstSheet = sheet;

    initFindReplace();
    findNext();

    delete dialog;
}

void FindReplaceAction::executeReplace(Selection *selection, Sheet *sheet, QWidget *canvasWidget)
{
    m_selection = selection;
    SearchDlg *dialog = new SearchDlg(canvasWidget, "Replace", m_findOptions, m_findStrings, m_replaceStrings);
    dialog->setHasSelection(!selection->isSingular());
    dialog->setHasCursor(true);
    if (KReplaceDialog::Accepted != dialog->exec()) {
        delete dialog;
        return;
    }

    m_findOptions = dialog->options();
    m_findStrings = dialog->findHistory();
    m_replaceStrings = dialog->replacementHistory();
    m_typeValue = dialog->searchType();

    delete m_find;
    delete m_replace;
    m_find = nullptr;
    m_replace = nullptr;
    // NOTE Stefan: Avoid beginning of line replacements with nothing which
    //              will lead to an infinite loop (Bug #125535). The reason
    //              for this is unclear to me, but who cares and who would
    //              want to do something like this, häh?!
    if (dialog->pattern() == "^" && dialog->replacement().isEmpty())
        return;
    m_replace = new KReplace(dialog->pattern(), dialog->replacement(), dialog->options());

    m_currentSheet = sheet;
    m_firstSheet = sheet;
    initFindReplace();
    connect(m_replace, &KReplace::textReplaced, this, &FindReplaceAction::slotReplace);

    m_replaceCommand = new KUndo2Command(kundo2_i18n("Replace"));

    findNext();
    delete dialog;
}

// Initialize a find or replace operation, using m_find or m_replace,
// and m_findOptions.
void FindReplaceAction::initFindReplace()
{
    KFind *findObj = m_find ? m_find : m_replace;
    Q_ASSERT(findObj);
    connect(findObj, &KFind::textFound, this, &FindReplaceAction::slotHighlight);
    connect(findObj, &KFind::findNext, this, &FindReplaceAction::findNext);

    bool bck = m_findOptions & KFind::FindBackwards;
    Sheet *currentSheet = m_currentSheet;

    QRect region = (m_findOptions & KFind::SelectedText)
        ? m_selection->lastRange()
        : QRect(1, 1, currentSheet->fullCellStorage()->columns(), currentSheet->fullCellStorage()->rows()); // All cells

    int colStart = !bck ? region.left() : region.right();
    int colEnd = !bck ? region.right() : region.left();
    int rowStart = !bck ? region.top() : region.bottom();
    int rowEnd = !bck ? region.bottom() : region.top();

    m_findLeftColumn = region.left();
    m_findRightColumn = region.right();
    m_findTopRow = region.top();
    m_findBottomRow = region.bottom();

    m_findStart = QPoint(colStart, rowStart);
    m_findPos = (m_findOptions & KFind::FromCursor) ? m_selection->cursor() : m_findStart;
    m_findEnd = QPoint(colEnd, rowEnd);
    // debugSheets << m_findPos <<" to" << m_findEnd;
    // debugSheets <<"leftcol=" << m_findLeftColumn <<" rightcol=" << m_findRightColumn;
}

void FindReplaceAction::slotHighlight(const QString & /*text*/, int /*matchingIndex*/, int /*matchedLength*/)
{
    m_selection->initialize(m_findPos);
    QDialog *dialog = nullptr;
    if (m_find)
        dialog = m_find->findNextDialog();
    else
        dialog = m_replace->replaceNextDialog();
    debugSheets << " baseDialog :" << dialog;
    QRect globalRect(m_findPos, m_findEnd);
    globalRect.moveTopLeft(m_selection->canvas()->canvasWidget()->mapToGlobal(globalRect.topLeft()));
    KoDialog::avoidArea(dialog, QRect(m_findPos, m_findEnd));
}

void FindReplaceAction::slotReplace(const QString &newText, int, int, int)
{
    if (m_typeValue == FindOption::Value) {
        DataManipulator *command = new DataManipulator(m_replaceCommand);
        command->setParsing(true);
        command->setSheet(m_currentSheet);
        command->setValue(Value(newText));
        command->add(Region(m_findPos, m_currentSheet));
    } else if (m_typeValue == FindOption::Note) {
        CommentCommand *command = new CommentCommand(m_replaceCommand);
        command->setComment(newText);
        command->setSheet(m_currentSheet);
        command->add(Region(m_findPos, m_currentSheet));
    }
}

Cell FindReplaceAction::nextFindValidCell(int col, int row)
{
    Cell cell = Cell(m_currentSheet, col, row);
    if (cell.isDefault() || cell.isPartOfMerged() || cell.isFormula())
        cell = Cell();
    if (m_typeValue == FindOption::Note && !cell.isNull() && cell.comment().isEmpty())
        cell = Cell();
    return cell;
}

Cell FindReplaceAction::findNextCell()
{
    // cellStorage()->firstInRow / cellStorage()->nextInRow would be faster at doing that,
    // but it doesn't seem to be easy to combine it with 'start a column m_find.x()'...

    Sheet *sheet = m_currentSheet;
    Cell cell;
    bool forw = !(m_findOptions & KFind::FindBackwards);
    int col = m_findPos.x();
    int row = m_findPos.y();
    int maxRow = sheet->fullCellStorage()->rows();
    //     warnSheets <<"findNextCell starting at" << col << ',' << row <<"   forw=" << forw;

    if (m_directionValue == FindOption::Row) {
        while (!cell && (row >= m_findTopRow) && (row <= m_findBottomRow) && (forw ? row <= maxRow : row >= 0)) {
            while (!cell && (forw ? col <= m_findRightColumn : col >= m_findLeftColumn)) {
                cell = nextFindValidCell(col, row);
                if (forw)
                    ++col;
                else
                    --col;
            }
            if (!cell.isNull())
                break;
            // Prepare looking in the next row
            if (forw) {
                col = m_findLeftColumn;
                ++row;
            } else {
                col = m_findRightColumn;
                --row;
            }
            // warnSheets <<"next row:" << col << ',' << row;
        }
    } else {
        while (!cell && (forw ? col <= m_findRightColumn : col >= m_findLeftColumn)) {
            while (!cell && (row >= m_findTopRow) && (row <= m_findBottomRow) && (forw ? row <= maxRow : row >= 0)) {
                cell = nextFindValidCell(col, row);
                if (forw)
                    ++row;
                else
                    --row;
            }
            if (!cell.isNull())
                break;
            // Prepare looking in the next col
            if (forw) {
                row = m_findTopRow;
                ++col;
            } else {
                row = m_findBottomRow;
                --col;
            }
            // debugSheets <<"next row:" << col << ',' << row;
        }
    }
    // if (!cell)
    // No more next cell - TODO go to next sheet (if not looking in a selection)
    // (and make m_findEnd(max, max) in that case...)
    //    if (cell.isNull()) warnSheets<<"returning null"<<endl;
    //    else warnSheets <<" returning" << cell;

    return cell;
}

void FindReplaceAction::executeFindNext(Selection *selection, Sheet *sheet, QWidget *canvasWidget)
{
    m_selection = selection;
    KFind *findObj = m_find ? m_find : m_replace;
    if (!findObj) {
        execute(selection, sheet, canvasWidget);
        return;
    }

    findNext();
}

void FindReplaceAction::executeFindPrevious(Selection *selection, Sheet *sheet, QWidget *canvasWidget)
{
    m_selection = selection;
    KFind *findObj = m_find ? m_find : m_replace;
    if (!findObj) {
        execute(selection, sheet, canvasWidget);
        return;
    }

    findPrevious();
}

void FindReplaceAction::findNext()
{
    KFind *findObj = m_find ? m_find : m_replace;
    if (!findObj)
        return;
    KFind::Result res = KFind::NoMatch;
    Cell cell = findNextCell();
    bool forw = !(m_findOptions & KFind::FindBackwards);
    while (res == KFind::NoMatch && !cell.isNull()) {
        if (findObj->needData()) {
            if (m_typeValue == FindOption::Note)
                findObj->setData(cell.comment());
            else
                findObj->setData(cell.userInput());
            m_findPos = QPoint(cell.column(), cell.row());
            // debugSheets <<"setData(cell" << m_findPos << ')';
        }

        // Let KFind inspect the text fragment, and display a dialog if a match is found
        if (m_find)
            res = m_find->find();
        else
            res = m_replace->replace();

        if (res == KFind::NoMatch) {
            // Go to next cell, skipping unwanted cells
            if (m_directionValue == FindOption::Row) {
                if (forw)
                    ++m_findPos.rx();
                else
                    --m_findPos.rx();
            } else {
                if (forw)
                    ++m_findPos.ry();
                else
                    --m_findPos.ry();
            }
            cell = findNextCell();
        }
    }

    if (res == KFind::NoMatch) {
        // emitUndoRedo();
        // removeHighlight();
        if (findObj->shouldRestart()) {
            m_findOptions &= ~KFind::FromCursor;
            m_findPos = m_findStart;
            findObj->resetCounts();
            findNext();
        } else { // done, close the 'find next' dialog
            if (m_find)
                m_find->closeFindNextDialog();
            else {
                m_selection->canvas()->addCommand(m_replaceCommand);
                m_replaceCommand = nullptr;
                m_replace->closeReplaceNextDialog();
            }
        }
    } else if (!cell.isNull()) {
        // move to the cell
        Sheet *sheet = dynamic_cast<Sheet *>(cell.sheet());
        if (sheet != m_selection->activeSheet())
            m_selection->emitVisibleSheetRequested(sheet);
        m_selection->initialize(Region(cell.column(), cell.row(), cell.sheet()), sheet);
        m_actions->tool()->scrollToCell(m_selection->cursor());
    }
}

void FindReplaceAction::findPrevious()
{
    KFind *findObj = m_find ? m_find : m_replace;
    if (!findObj)
        return;
    // debugSheets <<"findPrevious";
    int opt = m_findOptions;
    bool forw = !(opt & KFind::FindBackwards);
    if (forw)
        m_findOptions = (opt | KFind::FindBackwards);
    else
        m_findOptions = (opt & ~KFind::FindBackwards);

    findNext();

    m_findOptions = opt; // restore initial options
}

FindAction::FindAction(Actions *actions, const QString &actionName)
    : CellAction(actions, actionName, QString(), QIcon(), QString())
{
    m_findAction = nullptr;
}

FindAction::~FindAction() = default;

FindReplaceAction *FindAction::findAction()
{
    if (m_findAction)
        return m_findAction;
    CellAction *action = m_actions->cellAction("edit_find");
    if (!action)
        return nullptr; // this shouldn't happen
    m_findAction = dynamic_cast<FindReplaceAction *>(action);
    return m_findAction;
}

FindNext::FindNext(Actions *actions)
    : FindAction(actions, "edit_find_next")
{
}

FindNext::~FindNext() = default;

QAction *FindNext::createAction()
{
    QAction *action = KStandardAction::findNext(nullptr, nullptr, m_actions->tool());
    connect(action, &QAction::triggered, this, &FindNext::triggered);
    return action;
}

void FindNext::execute(Selection *selection, Sheet *sheet, QWidget *canvasWidget)
{
    FindReplaceAction *find = findAction();
    if (!find)
        return;
    find->executeFindNext(selection, sheet, canvasWidget);
}

FindPrevious::FindPrevious(Actions *actions)
    : FindAction(actions, "edit_find_prev")
{
}

FindPrevious::~FindPrevious() = default;

QAction *FindPrevious::createAction()
{
    QAction *action = KStandardAction::findPrev(nullptr, nullptr, m_actions->tool());
    connect(action, &QAction::triggered, this, &FindPrevious::triggered);
    return action;
}

void FindPrevious::execute(Selection *selection, Sheet *sheet, QWidget *canvasWidget)
{
    FindReplaceAction *find = findAction();
    if (!find)
        return;
    find->executeFindPrevious(selection, sheet, canvasWidget);
}

Replace::Replace(Actions *actions)
    : FindAction(actions, "edit_replace")
{
}

Replace::~Replace() = default;

QAction *Replace::createAction()
{
    QAction *action = KStandardAction::replace(nullptr, nullptr, m_actions->tool());
    connect(action, &QAction::triggered, this, &Replace::triggered);
    return action;
}

void Replace::execute(Selection *selection, Sheet *sheet, QWidget *canvasWidget)
{
    FindReplaceAction *find = findAction();
    if (!find)
        return;
    find->executeReplace(selection, sheet, canvasWidget);
}
