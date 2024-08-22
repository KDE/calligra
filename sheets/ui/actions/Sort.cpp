/* This file is part of the KDE project
   SPDX-FileCopyrightText: 1998-2022 The Calligra Team <calligra-devel@kde.org>
   SPDX-FileCopyrightText: 2022 Tomas Mecir <mecirt@gmail.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "Sort.h"
#include "./dialogs/SortDialog.h"
#include "Actions.h"

#include "core/ApplicationSettings.h"
#include "core/CellStorage.h"
#include "core/Map.h"
#include "core/Sheet.h"
#include "engine/CalculationSettings.h"
#include "engine/ValueCalc.h"
#include "engine/ValueConverter.h"

#include <KLocalizedString>

using namespace Calligra::Sheets;

Sort::Sort(Actions *actions)
    : CellAction(actions, "sort", i18n("&Sort..."), QIcon(), i18n("Sort a group of cells"))
    , m_dlg(nullptr)
{
}

Sort::~Sort()
{
    if (m_dlg)
        delete m_dlg;
}

void Sort::execute(Selection *selection, Sheet *sheet, QWidget *canvasWidget)
{
    m_selection = selection;

    QRect range = selection->lastRange();
    CellBase criteriaCell(sheet, range.left(), range.top());
    if ((range.width() == 1) && (range.height() == 1)) {
        range = extendSelectionToRange(criteriaCell, false);
        selection->initialize(range, sheet);
        selection->emitModified();
    }

    QVector<Value> firstRow, firstCol;
    if ((!selection->isRowSelected()) && (range.width() > 1)) {
        // not a full row selection - fill in the first row data
        const int row = range.top();
        int col = range.left();
        while (col <= range.right()) {
            const Value value = CellBase(sheet, col, row).value();
            firstRow.append(value);
            col++;
        }
    }
    if (!selection->isColumnSelected() && (range.height() > 1)) {
        // not a full column selection - fill in the first column data
        const int col = range.left();
        int row = range.top();
        while (row <= range.bottom()) {
            const Value value = CellBase(sheet, col, row).value();
            firstCol.append(value);
            row++;
        }
    }

    ApplicationSettings *sett = sheet->fullMap()->applicationSettings();
    Localization *locale = sheet->map()->calculationSettings()->locale();
    m_dlg = new SortDialog(canvasWidget, range, firstRow, firstCol);
    m_dlg->setCustomLists(sett->sortingList(), locale);

    if (m_dlg->exec()) {
        SortManipulator *const command = new SortManipulator();
        command->setSheet(sheet);

        // set parameters
        command->setSortRows(m_dlg->sortRows());
        command->setSkipFirst(m_dlg->skipFirst());
        command->setCopyFormat(m_dlg->copyFormat());

        const bool horizontal = m_dlg->isHorizontal();
        const int offset = horizontal ? range.top() : range.left();

        // retrieve sorting order
        int count = m_dlg->criterionCount();
        for (int i = 0; i < count; ++i) {
            const int index = m_dlg->criterionIndex(i);
            const Qt::SortOrder order = m_dlg->criterionSortOrder(i);
            const Qt::CaseSensitivity caseSensitivity = m_dlg->criterionCaseSensitivity(i);
            command->addCriterion(index - offset, order, caseSensitivity);
        }

        if (m_dlg->useCustomList()) {
            // add custom list if any
            QStringList clist = m_dlg->customList();
            command->setUseCustomList(true);
            command->setCustomList(clist);
        }
        command->add(range, sheet);
        command->execute(selection->canvas());
    }

    delete m_dlg;
    m_dlg = nullptr;
}

SortInc::SortInc(Actions *actions)
    : CellAction(actions, "sortInc", i18n("Sort &Increasing"), koIcon("view-sort-ascending"), i18n("Sort a group of cells in ascending(first to last) order"))
{
}

SortInc::~SortInc() = default;

void SortInc::execute(Selection *selection, Sheet *sheet, QWidget *)
{
    QRect range = selection->lastRange();
    CellBase criteriaCell(sheet, range.left(), range.top());
    if ((range.width() == 1) && (range.height() == 1)) {
        range = extendSelectionToRange(criteriaCell, false);
        selection->initialize(range, sheet);
        selection->emitModified();
    }

    SortManipulator *command = new SortManipulator();
    command->add(range, sheet);
    command->setSheet(sheet);

    // Entire row(s) selected ? Or just one row ? Sort by rows if yes.
    bool sortCols = selection->isRowSelected();
    sortCols = sortCols || (range.top() == range.bottom());
    command->setSortRows(!sortCols);
    command->setSkipFirst(true);
    command->addCriterion(criteriaCell.column() - range.left(), Qt::AscendingOrder, Qt::CaseInsensitive);
    command->execute(selection->canvas());
}

SortDesc::SortDesc(Actions *actions)
    : CellAction(actions,
                 "sortDesc",
                 i18n("Sort &Decreasing"),
                 koIcon("view-sort-descending"),
                 i18n("Sort a group of cells in decreasing(last to first) order"))
{
}

SortDesc::~SortDesc() = default;

void SortDesc::execute(Selection *selection, Sheet *sheet, QWidget *)
{
    QRect range = selection->lastRange();
    CellBase criteriaCell(sheet, range.left(), range.top());
    if ((range.width() == 1) && (range.height() == 1)) {
        range = extendSelectionToRange(criteriaCell, false);
        selection->initialize(range, sheet);
        selection->emitModified();
    }

    SortManipulator *command = new SortManipulator();
    command->add(range, sheet);
    command->setSheet(sheet);

    // Entire row(s) selected ? Or just one row ? Sort by rows if yes.
    bool sortCols = selection->isRowSelected();
    sortCols = sortCols || (range.top() == range.bottom());
    command->setSortRows(!sortCols);
    command->setSkipFirst(true);
    command->addCriterion(criteriaCell.column() - range.left(), Qt::DescendingOrder, Qt::CaseInsensitive);
    command->execute(selection->canvas());
}

SortManipulator::SortManipulator()
    : AbstractDFManipulator()
    , m_cellStorage(nullptr)
{
    m_changeformat = false;
    m_rows = true;
    m_skipfirst = false;
    m_usecustomlist = false;

    setText(kundo2_i18n("Sort Data"));
}

SortManipulator::~SortManipulator() = default;

bool SortManipulator::process(Element *element)
{
    // process one element - rectangular range

    // here we perform the actual sorting, remember the new ordering and
    // let AbstractDFManipulator::process do the rest of the work
    // the new ordering is used in newValue and newFormat to return proper
    // values

    // sort
    sort(element);

    // set values
    return AbstractDFManipulator::process(element);
}

bool SortManipulator::preProcess()
{
    m_cellStorage = new CellStorage(m_sheet->fullCellStorage()->subStorage(*this));

    Region::ConstIterator endOfList(cells().constEnd());
    for (Region::ConstIterator it = cells().constBegin(); it != endOfList; ++it) {
        QRect range = (*it)->rect();
        for (int col = range.left(); col <= range.right(); ++col)
            for (int row = range.top(); row <= range.bottom(); ++row) {
                Cell cell = Cell(m_sheet, col, row);
                m_styles.insert(cell, cell.style());
                // encode the formula if there is one, so that cell references get updated correctly
                if (cell.isFormula())
                    m_formulas.insert(cell, cell.encodeFormula());
            }
    }

    // to start undo recording
    return AbstractDFManipulator::preProcess();
}

bool SortManipulator::postProcess()
{
    delete m_cellStorage;
    m_cellStorage = nullptr;
    m_styles.clear();
    m_formulas.clear();

    // to stop undo recording
    return AbstractDFManipulator::postProcess();
}

void SortManipulator::addCriterion(int index, Qt::SortOrder order, Qt::CaseSensitivity caseSensitivity)
{
    Criterion criterion;
    criterion.index = index;
    criterion.order = order;
    criterion.caseSensitivity = caseSensitivity;
    m_criteria.append(criterion);
}

void SortManipulator::clearCriteria()
{
    m_criteria.clear();
}

Value SortManipulator::newValue(Element *element, int col, int row, bool *parse, Format::Type *)
{
    QRect range = element->rect();
    int colidx = col - range.left();
    int rowidx = row - range.top();
    if (m_rows) // sort rows
        rowidx = sorted[rowidx];
    else
        colidx = sorted[colidx];
    rowidx += range.top();
    colidx += range.left();

    // If the cell contained a formula, we need to decode it with the -new- coordinates, so that the references remain intact
    Cell orig_cell = Cell(m_sheet, colidx, rowidx);
    Cell new_cell = Cell(m_sheet, col, row);
    if (m_formulas.contains(orig_cell)) {
        *parse = true;
        return Value(new_cell.decodeFormula(m_formulas[orig_cell]));
    }

    *parse = false;
    // have to return the stored value, to prevent the earlier calls from disrupting the latter ones
    return m_cellStorage->value(colidx, rowidx);
}

Style SortManipulator::newFormat(Element *element, int col, int row)
{
    QRect range = element->rect();
    int colidx = col - range.left();
    int rowidx = row - range.top();
    if (m_changeformat) {
        if (m_rows) // sort rows
            rowidx = sorted[rowidx];
        else
            colidx = sorted[colidx];
    }

    // have to return stored format, to avoid earlier calls disrupting latter ones
    return m_styles.value(Cell(m_sheet, colidx + range.left(), rowidx + range.top()));
}

void SortManipulator::sort(Element *element)
{
    // we'll use insert-sort to sort
    QRect range = element->rect();
    range = element->sheet()->cellStorage()->trimToUsedArea(range);
    int max = m_rows ? range.bottom() : range.right();
    int min = m_rows ? range.top() : range.left();
    int count = max - min + 1;
    // initially, all values are at their original positions
    sorted.clear();
    for (int i = 0; i < count; ++i)
        sorted[i] = i;

    // for each position, find the lowest value and move it there
    int start = m_skipfirst ? 1 : 0;
    for (int i = start; i < count - 1; ++i) {
        int lowest = i;
        for (int j = i + 1; j < count; ++j)
            if (shouldReorder(element, sorted[lowest], sorted[j]))
                lowest = j;
        // move lowest to start
        int tmp = sorted[i];
        sorted[i] = sorted[lowest];
        sorted[lowest] = tmp;
    }

    // that's all - process will take care of the rest, together with our
    // newValue/newFormat
}

bool SortManipulator::shouldReorder(Element *element, int first, int second)
{
    // we use ValueCalc::natural* to compare
    // indexes are real indexes, we don't use the sorted array here

    ValueCalc *calc = m_sheet->map()->calc();
    ValueConverter *conv = m_sheet->map()->converter();

    QRect range = element->rect();
    int firstrow = range.top();
    int firstcol = range.left();

    for (int i = 0; i < m_criteria.count(); ++i) {
        int which = m_criteria[i].index;
        bool ascending = m_criteria[i].order == Qt::AscendingOrder;
        bool caseSensitive = m_criteria[i].caseSensitivity == Qt::CaseSensitive;

        // figure out coordinates of the cells
        int row1 = firstrow + (m_rows ? first : which);
        int row2 = firstrow + (m_rows ? second : which);
        int col1 = firstcol + (m_rows ? which : first);
        int col2 = firstcol + (m_rows ? which : second);
        Value val1 = Cell(m_sheet, col1, row1).value();
        Value val2 = Cell(m_sheet, col2, row2).value();
        // empty values always go to the end, so if second value is empty and
        // first one is not, we don't need to reorder
        if (!val1.isEmpty() && val2.isEmpty())
            return false;
        if (val1.isEmpty() && !val2.isEmpty())
            return true;

        // custom list ?
        if (m_usecustomlist) {
            QString s1 = conv->asString(val1).asString().toLower();
            QString s2 = conv->asString(val2).asString().toLower();
            int pos1 = -1, pos2 = -1;
            int pos = 0;
            // Try to locate our two strings in the list. If both are there, assume
            // ordering as specified by the list.
            for (QStringList::ConstIterator it = m_customlist.constBegin(); it != m_customlist.constEnd(); ++it) {
                if ((pos1 == -1) && ((*it).toLower() == s1))
                    pos1 = pos;
                if ((pos2 == -1) && ((*it).toLower() == s2))
                    pos2 = pos;
                pos++;
            }
            if ((pos1 >= 0) && (pos2 >= 0) && (pos1 != pos2))
                // both are in the list, not the same
                return (pos1 > pos2);
        }

        if (calc->naturalGreater(val1, val2, caseSensitive))
            // first one greater - must reorder if ascending, don't reorder if not
            return ascending;
        if (calc->naturalLower(val1, val2, caseSensitive))
            // first one lower - don't reorder if ascending, reorder if not
            return !ascending;
        // equal - don't know yet, continue
    }

    // no difference found, they're the same - no need to reorder
    return false;
}
