/* This file is part of the KDE project
   Copyright 2006 Stefan Nikolaus <stefan.nikolaus@kdemail.net>
             2004 Tomas Mecir <mecirt@gmail.com>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/


#include <QLinkedList>
#include <QMap>

#include "Cell.h"
#include "DependencyManager.h"
#include "Formula.h"
#include "Region.h"
#include "Sheet.h"

using namespace KSpread;

/** d-pointer of DependencyManager */
class DependencyManager::Private
{
public:
  /**
   * Clears internal structures
   */
  void reset ();

  /**
   * Generates list of dependencies of \p cell .
   */
  void generateDependencies(const Cell* cell);

  /**
   * \return dependencies of \p cell
   */
  Region getDependencies (const Region::Point &cell);

  /**
   * \return cells depending on \p cell
   */
  Region getDependants(const Cell* cell);

  void areaModified (const QString &name);

  /**
   * Updates structures: \p cell depends on cells in \p region and vice versa.
   */
  void addDependency(const Cell* cell, const Region& region);

  /**
   * Removes all dependencies of \p cell .
   */
  void removeDependencies(const Cell* cell);

  /**
   * \return a list of cells that \p cell depends on
   */
  Region computeDependencies(const Cell* cell) const;

  /** debug */
  void dump();

  /** dependencies of each cell */
  QMap<Region::Point, Region> dependencies;
  /** list of cells, that depend on a cell */
  QMap<Region::Point, Region> dependants;
  /** list of cells referencing a given named area */
  QMap<QString, QMap<Region::Point, bool> > areaDeps;
};

// This is currently not called - but it's really convenient to call it from
// gdb or from debug output to check that everything is set up ok.
void DependencyManager::Private::dump()
{
  QMap<Region::Point, Region>::ConstIterator mend(dependencies.end());
  for ( QMap<Region::Point, Region>::ConstIterator mit(dependencies.begin()); mit != mend; ++mit )
  {
    Region::Point p = mit.key();
    kDebug() << "Cell " << p.name() << " depends on :" << endl;

    Region::ConstIterator rend((*mit).constEnd());
    for ( Region::ConstIterator rit((*mit).constBegin()); rit != rend; ++rit )
    {
      kDebug() << "  cell " << (*rit)->name() << endl;
    }
  }

  mend = dependants.end();
  for ( QMap<Region::Point, Region>::ConstIterator mit(dependants.begin()); mit != mend; ++mit )
  {
    Region::Point p = mit.key();
    kDebug() << "The cells that depend on " << p.name() << " are :" << endl;
    Region::ConstIterator rend((*mit).constEnd());
    for ( Region::ConstIterator rit((*mit).constBegin()); rit != rend; ++rit )
    {
      kDebug() << "  cell " << (*rit)->name() << endl;
    }
  }
}

DependencyManager::DependencyManager()
  : d(new Private)
{
}

DependencyManager::~DependencyManager ()
{
  delete d;
}

void DependencyManager::reset ()
{
  d->reset();
}

void DependencyManager::regionChanged(const Region& region)
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

        // if the cell contains the circle error, we mustn't do anything
        if (cell->testFlag (Cell::Flag_CircularCalculation))
          continue;

        // don't re-generate dependencies if we're updating dependencies
        if ( !(cell->testFlag (Cell::Flag_Progress)))
          d->generateDependencies(cell);
      }
    }
  }
}

void DependencyManager::areaModified (const QString &name)
{
  d->areaModified (name);
}
// RangeList DependencyManager::getDependencies (const Region::Point &cell)
// {
//   return d->getDependencies (cell);
// }

KSpread::Region DependencyManager::getDependants(const Cell* cell)
{
  return d->getDependants(cell);
}

QMap<KSpread::Region::Point, KSpread::Region> DependencyManager::dependencies() const
{
  return d->dependencies;
}

void DependencyManager::Private::reset ()
{
  dependencies.clear();
  dependants.clear();
}

KSpread::Region DependencyManager::Private::getDependencies (const Region::Point &cell)
{
  return dependencies.value(cell);
}

KSpread::Region DependencyManager::Private::getDependants(const Cell* cell)
{
  Region::Point point(QPoint(cell->column(), cell->row()));
  point.setSheet(cell->sheet());

  return dependants.value(point);
}

void DependencyManager::Private::areaModified (const QString &name)
{
  // since area names are something like aliases, modifying an area name
  // basically means that all cells referencing this area should be treated
  // as modified - that will retrieve updated area ranges and also update
  // everything as necessary ...
  if (!areaDeps.contains (name))
    return;

  QMap<Region::Point, bool>::iterator it;
  for (it = areaDeps[name].begin(); it != areaDeps[name].end(); ++it)
  {
    Cell *cell = it.key().sheet()->cellAt(it.key().pos());
    // this forces the cell to regenerate everything - new range dependencies
    // and so on
    cell->setValue (cell->value ());
  }
}

void DependencyManager::Private::addDependency(const Cell* cell, const Region& region)
{
  Region::Point point(QPoint(cell->column(), cell->row()));
  point.setSheet(cell->sheet());

  dependencies[point].add(region);

  Region::ConstIterator end(region.constEnd());
  for (Region::ConstIterator it(region.constBegin()); it != end; ++it)
  {
    Sheet* sheet = (*it)->sheet();
    const QRect range = (*it)->rect();
    const int bottom = range.bottom();
    const int right = range.right();
    for (int row = range.top(); row <= bottom; ++row)
    {
      for (int col = range.left(); col <= right; ++col)
      {
        Region::Point point2(QPoint(col, row));
        point2.setSheet(sheet);

        dependants[point2].add(Region(point.pos(), point.sheet()));
      }
    }
  }
}

void DependencyManager::Private::removeDependencies(const Cell* cell)
{
  Region::Point point(QPoint(cell->column(), cell->row()));
  point.setSheet(cell->sheet());

  //look if the cell has any dependencies
  if (!dependencies.contains (point))
    return;  //it doesn't - nothing more to do

  //first we remove cell-dependencies
  Region region = dependencies[point];
  Region::ConstIterator end(region.constEnd());
  for (Region::ConstIterator it(region.constBegin()); it != end; ++it)
  {
    const QRect range = (*it)->rect();
    for (int col = range.left(); col <= range.right(); ++col)
      for (int row = range.top(); row <= range.bottom(); ++row)
    {
      if (!dependants.contains(point))
        continue;  //this should never happen

      //we no longer depend on this cell
      dependants.remove(point);
    }
  }

  // remove information about named area dependencies
  QMap<QString, QMap<Region::Point, bool> >::iterator itr;
  for (itr = areaDeps.begin(); itr != areaDeps.end(); ++itr) {
    if (itr.value().contains (point))
      itr.value().remove (point);
  }

  // finally, remove the entry about this cell
  dependencies.remove(point);
}

void DependencyManager::Private::generateDependencies(const Cell* cell)
{
  //new dependencies only need to be generated if the cell contains a formula
  if( cell->isDefault() )
    return;
  if (!cell->isFormula())
    return;

  //get rid of old dependencies first
  removeDependencies(cell);

  //now we need to generate dependencies
  Region region = computeDependencies(cell);

  //now that we have the new dependencies, we put them into our data structures
  //and we're done
  addDependency(cell, region);
}

KSpread::Region DependencyManager::Private::computeDependencies(const Cell* cell) const
{
  // Not a formula -> no dependencies
  if (!cell->isFormula())
    return Region();

  // Broken formula -> meaningless dependencies
  // (tries to avoid cell->formula() being null)
  if (cell->hasError())
    return Region();

  const Formula* f = cell->formula();
  Q_ASSERT(f);
  if (f==0)
  {
    kDebug() << "Cell at row " << cell->row() << ", col " << cell->column() << " marked as formula, but formula is 0" << endl;
    return Region();
  }

  Tokens tokens = f->tokens();

  //return empty list if the tokens aren't valid
  if (!tokens.valid())
    return Region();

  Sheet* sheet = cell->sheet();
  Region region;
  for( int i = 0; i < tokens.count(); i++ )
  {
    Token token = tokens[i];
    Token::Type tokenType = token.type();

    //parse each cell/range and put it to our RangeList
    if (tokenType == Token::Cell || tokenType == Token::Range)
    {
      Region subRegion(sheet->workbook(), token.text(), sheet);
      if (subRegion.isValid())
        region.add(subRegion);
    }
  }
  return region;
}
