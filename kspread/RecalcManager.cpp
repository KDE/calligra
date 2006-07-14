/* This file is part of the KDE project
   Copyright 2006 Stefan Nikolaus <stefan.nikolaus@kdemail.net>

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
   * E.g. A1: '=1.0'; A2: '=A1+A1'; A3: '=A1+A1+A2';
   *      depth(A1) = 0; depth(A2) = 1; depth(A3) = 2
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
  if (d->busy)
    return;
  kDebug() << "RecalcManager::regionChanged " << region.name() << endl;
  {
    Region::ConstIterator end(region.constEnd());
    for (Region::ConstIterator it(region.constBegin()); it != end; ++it)
    {
      const QRect range = (*it)->rect();
      const Sheet* sheet = (*it)->sheet();
      for (int col = range.left(); col <= range.right(); ++col)
      {
        for (int row = range.top(); row <= range.bottom(); ++row)
        {
          Cell* cell = sheet->cellAt(col, row);

          // empty or default cell? do nothing
          if( cell->isDefault() )
            continue;

          recalcRegion(d->depManager->getDependants(cell));
        }
      }
    }
  }
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
  dump();
  ElapsedTime et( "Recalculating cells", ElapsedTime::PrintOnlyTime );
  foreach (Cell* cell, d->depths)
  {
    // FIXME Stefan: zero depths may occur, skip them?!
    recalcCell(cell);
  }

  d->cells.clear();
  d->depths.clear();
}

int RecalcManager::computeDepth(Cell* cell) const
{
  int depth = 0;

  Region::Point point(cell->column(), cell->row());
  point.setSheet(cell->sheet());
  const Region region = d->depManager->dependencies().value(point);

  Region::ConstIterator end(region.constEnd());
  for (Region::ConstIterator it(region.constBegin()); it != end; ++it)
  {
    const QRect range = (*it)->rect();
    Sheet* sheet = (*it)->sheet();
    for (int col = range.left(); col <= range.right(); ++col)
    {
      for (int row = range.top(); row <= range.bottom(); ++row)
      {
        Region::Point referencedPoint(col, row);
        referencedPoint.setSheet(sheet);
        if (!d->depManager->dependencies().contains(referencedPoint))
        {
          // no further references
          // depth stays as is
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
  return depth;
}

void RecalcManager::recalcRegion(const Region& region)
{
  Region::ConstIterator end(region.constEnd());
  for (Region::ConstIterator it(region.constBegin()); it != end; ++it)
  {
    const QRect range = (*it)->rect();
    const Sheet* sheet = (*it)->sheet();
    for (int col = range.left(); col <= range.right(); ++col)
    {
      for (int row = range.top(); row <= range.bottom(); ++row)
      {
        Cell* const cell = sheet->cellAt(col, row);
        recalcCell(cell);
#if 0
        if (!d->cells.contains(cell))
        {
          int depth = computeDepth(cell);
          d->cells.insert(cell, depth);
          d->depths.insertMulti(depth, cell);
        }
#endif
      }
    }
  }
}

void RecalcManager::recalcCell(Cell* cell) const
{
  //prevent infinite recursion (circular dependencies)
  if (cell->testFlag (Cell::Flag_Progress) ||
      cell->testFlag (Cell::Flag_CircularCalculation))
  {
    return;
  }
  //set the computing-dependencies flag
  cell->setFlag (Cell::Flag_Progress);

  //mark the cell as calc-dirty
  cell->setCalcDirtyFlag();

  //recalculate the cell
  cell->calc (false);

  //clear the computing-dependencies flag
  cell->clearFlag (Cell::Flag_Progress);
}

void RecalcManager::dump() const
{
  foreach (Cell* cell, d->depths)
  {
    kDebug() << "Cell(" << cell->column() << ", " << cell->row() << "): " << d->cells[cell] << endl;
  }
}
