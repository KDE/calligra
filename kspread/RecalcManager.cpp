/* This file is part of the KDE project
   Copyright 2006 Stefan Nikolaus <stefan.nikolaus@kdemail.net>
             2004 Tomas Mecir <mecirt@gmail.com>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
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

#include <QHash>
#include <QMap>

#include "Cell.h"
#include "DependencyManager.h"
#include "Sheet.h"
#include "Region.h"
#include "Util.h"
#include "ValueFormatter.h"

#include "RecalcManager.h"

using namespace KSpread;

class RecalcManager::Private
{
public:
  /**
   * Stores cells with its reference depth.
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
  QHash<Cell*, int> cells;
  QMap<int, Cell*> depths;
  DependencyManager* depManager;
  bool busy;
};

RecalcManager::RecalcManager(DependencyManager* depManager)
  : d(new Private)
{
  d->depManager = depManager;
  d->busy = false;
}

RecalcManager::~RecalcManager()
{
  delete d;
}

void RecalcManager::regionChanged(const Region& region)
{
  if (d->busy || region.isEmpty())
    return;
  d->busy = true;
  kDebug(36002) << "RecalcManager::regionChanged " << region.name() << endl;
  ElapsedTime et( "Overall region recalculation", ElapsedTime::PrintOnlyTime );
  {
    ElapsedTime et( "Computing reference depths", ElapsedTime::PrintOnlyTime );
    recalcRegion(region);
  }
  recalc();
  d->busy = false;
}

void RecalcManager::recalcSheet(Sheet* const sheet)
{
  if (d->busy)
    return;
  d->busy = true;
  ElapsedTime et( "Overall sheet recalculation", ElapsedTime::PrintOnlyTime );
  {
    ElapsedTime et( "Computing reference depths", ElapsedTime::PrintOnlyTime );
    foreach (Region::Point point, d->depManager->dependencies().keys())
    {
      Cell* const cell = point.cell();
      if (cell->sheet() != sheet)
      {
        // not in the sheet to be recalculated
        continue;
      }
      if (!d->cells.contains(cell))
      {
        int depth = computeDepth(cell);
        d->cells.insert(cell, depth);
        d->depths.insertMulti(depth, cell);
      }
    }
  }
  recalc();
  d->busy = false;
}

void RecalcManager::recalcMap()
{
  if (d->busy)
    return;
  d->busy = true;
  ElapsedTime et( "Overall map recalculation", ElapsedTime::PrintOnlyTime );
  {
    ElapsedTime et( "Computing reference depths", ElapsedTime::PrintOnlyTime );
    foreach (Region::Point point, d->depManager->dependencies().keys())
    {
      Cell* const cell = point.cell();
      if (!d->cells.contains(cell))
      {
        int depth = computeDepth(cell);
        d->cells.insert(cell, depth);
        d->depths.insertMulti(depth, cell);
      }
    }
  }
  recalc();
  d->busy = false;
}

void RecalcManager::recalc()
{
  ElapsedTime et( "Recalculating cells", ElapsedTime::PrintOnlyTime );
  foreach (Cell* cell, d->depths)
  {
    recalcCell(cell);
  }

  d->cells.clear();
  d->depths.clear();
}

int RecalcManager::computeDepth(Cell* cell) const
{
  //prevent infinite recursion (circular dependencies)
  if (cell->testFlag(Cell::Flag_UpdatingDeps) ||
      cell->testFlag (Cell::Flag_CircularCalculation))
  {
    kError(36001) << "Circular dependency at " << cell->fullName() << endl;
    // don't set anything if the cell already has all these things set
    // this prevents endless loop for inter-sheet curcular dependencies,
    // where the usual mechanisms fail doe to having multiple dependency
    // managers ...
    if (!cell->testFlag (Cell::Flag_CircularCalculation))
    {
      Value value;
      cell->setFlag(Cell::Flag_CircularCalculation);
      value.setError(ValueFormatter::errorFormat(cell));
      cell->setValue(value);
    }
    //clear the compute reference depth flag
    cell->clearFlag (Cell::Flag_UpdatingDeps);
    return 0;
  }

  // set the compute reference depth flag
  cell->setFlag(Cell::Flag_UpdatingDeps);

  int depth = 0;

  Region::Point point(cell->column(), cell->row());
  point.setSheet(cell->sheet());
  const Region region = d->depManager->dependencies().value(point);

  Region::ConstIterator end(region.constEnd());
  for (Region::ConstIterator it(region.constBegin()); it != end; ++it)
  {
    const QRect range = (*it)->rect();
    Sheet* sheet = (*it)->sheet();
    const int right = range.right();
    const int bottom = range.bottom();
    for (int col = range.left(); col <= right; ++col)
    {
      for (int row = range.top(); row <= bottom; ++row)
      {
        Region::Point referencedPoint(col, row);
        referencedPoint.setSheet(sheet);
        if (!d->depManager->dependencies().contains(referencedPoint))
        {
          // no further references
          // depth is one at least
          depth = qMax(depth, 1);
          continue;
        }

        Cell* referencedCell = sheet->cellAt(col, row);
        if (d->cells.contains(referencedCell))
        {
          // the referenced cell depth was already computed
          depth = qMax(d->cells[referencedCell] + 1, depth);
          continue;
        }

        // compute the depth of the referenced cell, add one and
        // take it as new depth, if it's greater than the current one
        depth = qMax(computeDepth(referencedCell) + 1, depth);
      }
    }
  }

  //clear the computing reference depth flag
  cell->clearFlag(Cell::Flag_UpdatingDeps);

  return depth;
}

void RecalcManager::recalcRegion(const Region& region)
{
  Region::ConstIterator end(region.constEnd());
  for (Region::ConstIterator it(region.constBegin()); it != end; ++it)
  {
    const QRect range = (*it)->rect();
    const Sheet* sheet = (*it)->sheet();
    const int right = range.right();
    const int bottom = range.bottom();
    for (int col = range.left(); col <= right; ++col)
    {
      for (int row = range.top(); row <= bottom; ++row)
      {
        Cell* const cell = sheet->cellAt(col, row);

        // If we have not processed this cell yet.
        if (!d->cells.contains(cell))
        {
          int depth = computeDepth(cell);
          d->cells.insert(cell, depth);
          d->depths.insertMulti(depth, cell);

          // Recursion. We need the whole dependency tree of the changed region.
          // An infinite loop is prevented by the check above.
          Region dependantRegion = d->depManager->getDependants(cell);
          if (!dependantRegion.contains(QPoint(cell->column(), cell->row()), cell->sheet()))
          {
            recalcRegion(dependantRegion);
          }
        }
      }
    }
  }
}

void RecalcManager::recalcCell(Cell* cell) const
{
  //prevent infinite recursion (circular dependencies)
  if (cell->testFlag (Cell::Flag_CalculatingCell) ||
      cell->testFlag (Cell::Flag_CircularCalculation))
  {
#if 0
    kError(36001) << "ERROR: Circle, cell " << cell->fullName() << endl;
    // don't set anything if the cell already has all these things set
    // this prevents endless loop for inter-sheet curcular dependencies,
    // where the usual mechanisms fail doe to having multiple dependency
    // managers ...
    if (!cell->testFlag (Cell::Flag_CircularCalculation))
    {
      Value value;
      cell->setFlag(Cell::Flag_CircularCalculation);
      value.setError(ValueFormatter::errorFormat(cell));
      cell->setValue(value);
    }
#endif
    //clear the calculation progress flag
    cell->clearFlag (Cell::Flag_CalculatingCell);
    return;
  }
  //set the calculation progress flag
  cell->setFlag(Cell::Flag_CalculatingCell);

  //mark the cell as calc-dirty
  cell->setFlag(Cell::Flag_CalcDirty);

  //recalculate the cell
  cell->calc (false);

  //clear the calculation progress flag
  cell->clearFlag(Cell::Flag_CalculatingCell);
}

void RecalcManager::dump() const
{
  foreach (Cell* cell, d->depths)
  {
    kDebug(36002) << "Cell(" << cell->column() << ", " << cell->row() << "): " << d->cells[cell] << endl;
  }
}
