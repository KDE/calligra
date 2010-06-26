/* This file is part of the KDE project
   Copyright (C) 2006 Tomas Mecir <mecirt@gmail.com>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; only
   version 2 of the License.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/


#include "SortManipulator.h"

#include "Map.h"
#include "Sheet.h"
#include "ValueCalc.h"
#include "ValueConverter.h"

#include <klocale.h>

using namespace KSpread;

SortManipulator::SortManipulator()
        : AbstractDFManipulator()
        , m_cellStorage(0)
{
    m_changeformat = false;
    m_rows = true;
    m_skipfirst = false;
    m_usecustomlist = false;

    setText(i18n("Sort Data"));
}

SortManipulator::~SortManipulator()
{
}

bool SortManipulator::process(Element* element)
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

bool SortManipulator::preProcessing()
{
    // Only on sorting we need to temporarily store the old data.
    // On restoring (undo) we return immediately.
    if (m_reverse)
        return AbstractDFManipulator::preProcessing();

    m_cellStorage = new CellStorage(m_sheet->cellStorage()->subStorage(*this));

    Region::Iterator endOfList(cells().end());
    for (Region::Iterator it = cells().begin(); it != endOfList; ++it) {
        QRect range = (*it)->rect();
        for (int col = range.left(); col <= range.right(); ++col)
            for (int row = range.top(); row <= range.bottom(); ++row) {
                Cell cell = Cell(m_sheet, col, row);
                m_styles.insert(cell, cell.style());
            }
    }

    // to start undo recording
    return AbstractDFManipulator::preProcessing();
}

bool SortManipulator::postProcessing()
{
    delete m_cellStorage;
    m_cellStorage = 0;
    m_styles.clear();

    // to stop undo recording
    return AbstractDFManipulator::postProcessing();
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

Value SortManipulator::newValue(Element *element, int col, int row,
                                bool *parse, Format::Type *)
{
    Q_UNUSED(parse);
    QRect range = element->rect();
    int colidx = col - range.left();
    int rowidx = row - range.top();
    if (m_rows)  // sort rows
        rowidx = sorted[rowidx];
    else
        colidx = sorted[colidx];

    // have to return stored value, to avoid earlier calls disrupting latter ones
    return m_cellStorage->value(colidx + range.left(), rowidx + range.top());
}

Style SortManipulator::newFormat(Element *element, int col, int row)
{
    QRect range = element->rect();
    int colidx = col - range.left();
    int rowidx = row - range.top();
    if (m_changeformat) {
        if (m_rows)  // sort rows
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
    int max = m_rows ? range.bottom() : range.right();
    int min = m_rows ? range.top() : range.left();
    int count = max - min + 1;
    // initially, all values are at their original positions
    sorted.clear();
    for (int i = 0; i < count; ++i) sorted[i] = i;

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
        if ((!val1.isEmpty()) && val2.isEmpty())
            return false;

        // custom list ?
        if (m_usecustomlist) {
            QString s1 = conv->asString(val1).asString().toLower();
            QString s2 = conv->asString(val2).asString().toLower();
            QStringList::iterator it;
            int pos1 = -1, pos2 = -1;
            int pos = 0;
            // Try to locate our two strings in the list. If both are there, assume
            // ordering as specified by the list.
            for (it = m_customlist.begin(); it != m_customlist.end(); ++it) {
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
