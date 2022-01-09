// This file is part of the KDE project
// SPDX-FileCopyrightText: 2006-2007 Stefan Nikolaus <stefan.nikolaus@kdemail.net>
// SPDX-FileCopyrightText: 2004 Tomas Mecir <mecirt@gmail.com>
// SPDX-License-Identifier: LGPL-2.0-only

// Local
#include "RecalcManager.h"

#include "CellBase.h"
#include "CellBaseStorage.h"
#include "DependencyManager.h"
#include "Formula.h"
#include "FormulaStorage.h"
#include "MapBase.h"
#include "SheetBase.h"
#include "Region.h"
#include "Value.h"
#include "ElapsedTime_p.h"

#include <KoUpdater.h>

#include <QHash>
#include <QMap>

using namespace Calligra::Sheets;

class Q_DECL_HIDDEN RecalcManager::Private
{
public:
    /**
     * Finds all cells in region and their dependents, that need recalculation.
     *
     * \see RecalcManager::regionChanged
     */
    void cellsToCalculate(const Region& region);

    /**
     * Finds all cells in \p sheet , that have got a formula and hence need
     * recalculation.
     * If \p sheet is zero, all cells in the Map a returned.
     *
     * \see RecalcManager::recalcMap
     * \see RecalcManager::recalcSheet
     */
    void cellsToCalculate(SheetBase* sheet = 0);

    /**
     * Helper function for cellsToCalculate(const Region&) and cellsToCalculate(SheetBase*).
     */
    void cellsToCalculate(const Region& region, QSet<Cell>& cells) const;

    /*
     * Stores cells ordered by its reference depth.
     * Depth means the maximum depth of all cells this cell depends on plus one,
     * while a cell which has a formula without cell references has a depth
     * of zero.
     *
     * Examples:
     * \li A1: '=1.0'
     * \li A2: '=A1+A1'
     * \li A3: '=A1+A1+A2'
     *
     * \li depth(A1) = 0
     * \li depth(A2) = 1
     * \li depth(A3) = 2
     */
    QMap<int, Cell> cells;
    const Map* map;
    bool active;
};

void RecalcManager::Private::cellsToCalculate(const Region& region)
{
    if (region.isEmpty())
        return;

    // retrieve the cell depths
    QMap<Cell, int> depths = map->dependencyManager()->depths();

    // create the cell map ordered by depth
    QSet<Cell> cells;
    cellsToCalculate(region, cells);
    const QSet<Cell>::ConstIterator end(cells.end());
    for (QSet<Cell>::ConstIterator it(cells.begin()); it != end; ++it) {
        if ((*it).sheet()->isAutoCalculationEnabled())
            this->cells.insertMulti(depths[*it], *it);
    }
}

void RecalcManager::Private::cellsToCalculate(SheetBase* sheet)
{
    // retrieve the cell depths
    QMap<Cell, int> depths = map->dependencyManager()->depths();

    // NOTE Stefan: It's necessary, that the cells are filled in row-wise;
    //              beginning with the top left; ending with the bottom right.
    //              This ensures, that the value storage is processed the same
    //              way, which boosts performance (using PointStorage) for an
    //              empty storage (on loading). For an already filled value
    //              storage, the speed gain is not that sensible.
    Cell cell;
    if (!sheet) { // map recalculation
        for (int s = 0; s < map->count(); ++s) {
            sheet = map->sheet(s);
            for (int c = 0; c < sheet->formulaStorage()->count(); ++c) {
                cell = CellBase(sheet, sheet->formulaStorage()->col(c), sheet->formulaStorage()->row(c));
                cells.insertMulti(depths[cell], cell);
            }
        }
    } else { // sheet recalculation
        for (int c = 0; c < sheet->formulaStorage()->count(); ++c) {
            cell = CellBase(sheet, sheet->formulaStorage()->col(c), sheet->formulaStorage()->row(c));
            cells.insertMulti(depths[cell], cell);
        }
    }
}

void RecalcManager::Private::cellsToCalculate(const Region& region, QSet<Cell>& cells) const
{
    Region::ConstIterator end(region.constEnd());
    for (Region::ConstIterator it(region.constBegin()); it != end; ++it) {
        const QRect range = (*it)->rect();
        const SheetBase* sheet = (*it)->sheet();
        for (int col = range.left(); col <= range.right(); ++col) {
            for (int row = range.top(); row <= range.bottom(); ++row) {
                Cell cell(sheet, col, row);
                // Even empty cells may act as value
                // providers and need to be processed.

                // check for already processed cells
                if (cells.contains(cell))
                    continue;

                // add it to the list
                if (cell.isFormula())
                    cells.insert(cell);

                // add its consumers to the list
                cellsToCalculate(map->dependencyManager()->consumingRegion(cell), cells);
            }
        }
    }
}

RecalcManager::RecalcManager(Map *const map)
        : QObject(map)
        , d(new Private)
{
    d->map  = map;
    d->active = false;
}

RecalcManager::~RecalcManager()
{
    delete d;
}

void RecalcManager::regionChanged(const Region& region)
{
    if (d->active || region.isEmpty())
        return;
    d->active = true;
    debugSheetsFormula << "RecalcManager::regionChanged" << region.name();
    ElapsedTime et("Overall region recalculation", ElapsedTime::PrintOnlyTime);
    d->cellsToCalculate(region);
    recalc();
    d->active = false;
}

void RecalcManager::recalcSheet(SheetBase* const sheet)
{
    if (d->active)
        return;
    d->active = true;
    ElapsedTime et("Overall sheet recalculation", ElapsedTime::PrintOnlyTime);
    d->cellsToCalculate(sheet);
    recalc();
    d->active = false;
}

void RecalcManager::recalcMap(KoUpdater *updater)
{
    if (d->active)
        return;
    d->active = true;
    ElapsedTime et("Overall map recalculation", ElapsedTime::PrintOnlyTime);
    d->cellsToCalculate();
    recalc(updater);
    d->active = false;
}

bool RecalcManager::isActive() const
{
    return d->active;
}

void RecalcManager::addSheet(SheetBase *sheet)
{
    // Manages also the revival of a deleted sheet.
    Q_UNUSED(sheet);

    // sebsauer: not recalc every time on loading - bug 284325
    if (!d->map->isLoading()) {
        recalcMap(); // FIXME Stefan: Implement a more elegant solution.
    }
}

void RecalcManager::removeSheet(SheetBase *sheet)
{
    Q_UNUSED(sheet);
    recalcMap(); // FIXME Stefan: Implement a more elegant solution.
}

void RecalcManager::recalc(KoUpdater *updater)
{
    debugSheetsFormula << "Recalculating" << d->cells.count() << " cell(s)..";
    ElapsedTime et("Recalculating cells", ElapsedTime::PrintOnlyTime);

    if (updater)
        updater->setProgress(0);

    const QList<Cell> cells = d->cells.values();
    const int cellsCount = cells.count();
    for (int c = 0; c < cellsCount; ++c) {
        // only recalculate, if no circular dependency occurred
        if (cells.value(c).value() == Value::errorCIRCLE())
            continue;
        // Check for valid formula; parses the expression, if not done already.
        if (!cells.value(c).formula().isValid())
            continue;

        const SheetBase* sheet = cells.value(c).sheet();

        // evaluate the formula and set the result
        Value result = cells.value(c).formula().eval();
        if (result.isArray() && (result.columns() > 1 || result.rows() > 1)) {
            const QRect rect = cells.value(c).lockedCells();
            // unlock
            sheet->cellStorage()->unlockCells(rect.left(), rect.top());
            for (int row = rect.top(); row <= rect.bottom(); ++row) {
                for (int col = rect.left(); col <= rect.right(); ++col) {
                    CellBase(sheet, col, row).setValue(result.element(col - rect.left(), row - rect.top()));
                }
            }
            // relock
            sheet->cellStorage()->lockCells(rect);
        } else {
            CellBase(cells.value(c)).setValue(result);
        }
        if (updater)
            updater->setProgress(int(qreal(c) / qreal(cellsCount) * 100.));
    }

    if (updater)
        updater->setProgress(100);

//     dump();
    d->cells.clear();
}

void RecalcManager::dump() const
{
    QMap<int, Cell>::ConstIterator end(d->cells.constEnd());
    for (QMap<int, Cell>::ConstIterator it(d->cells.constBegin()); it != end; ++it) {
        Cell cell = it.value();
        QString cellName = cell.name();
        while (cellName.count() < 4) cellName.prepend(' ');
        debugSheetsFormula << "depth(" << cellName << " ) =" << it.key();
    }
}
