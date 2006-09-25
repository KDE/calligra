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

#include <QHash>
#include <QLinkedList>
#include <QMap>

#include <KoRTree.h>

#include "Cell.h"
#include "DependencyManager.h"
#include "Formula.h"
#include "Map.h"
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
  Region getDependents(const Cell* cell);

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

  /**
   * Removes the circular dependency flag from \p region and all their dependencies.
   */
  void removeCircularDependencyFlags(const Region& region);

  /** debug */
  void dump() const;

  /** dependencies of each cell */
  QMap<Region::Point, Region> dependencies;
  /** list of cells, that depend on a cell */
  class PointTree :public KoRTree<Region::Point>
  { public: PointTree() : KoRTree<Region::Point>(8,4) {} };
  QHash<Sheet*, PointTree*> dependents; // FIXME Stefan: Why is a QHash<Sheet*, PointTree> crashing?
  /** list of cells referencing a given named area */
  QMap<QString, QMap<Region::Point, bool> > areaDeps;
};

// This is currently not called - but it's really convenient to call it from
// gdb or from debug output to check that everything is set up ok.
void DependencyManager::Private::dump() const
{
  QMap<Region::Point, Region>::ConstIterator mend(dependencies.end());
  for ( QMap<Region::Point, Region>::ConstIterator mit(dependencies.begin()); mit != mend; ++mit )
  {
    Region::Point p = mit.key();
    kDebug(36002) << "Cell " << p.name() << " depends on:" << endl;

    Region::ConstIterator rend((*mit).constEnd());
    for ( Region::ConstIterator rit((*mit).constBegin()); rit != rend; ++rit )
    {
      kDebug(36002) << "  cell " << (*rit)->name() << endl;
    }
  }

  foreach (Sheet* sheet, dependents.keys())
  {
    QList<QRectF> keys = dependents[sheet]->keys();
    QList<Region::Point> values = dependents[sheet]->values();
    QHash<QString, QString> table;
    for (int i = 0; i < keys.count(); ++i)
    {
      Region::Range tmpRange(keys[i].toRect());
      tmpRange.setSheet(sheet);
      table.insertMulti(tmpRange.name(), values[i].name());
    }
    foreach (QString uniqueKey, table.uniqueKeys())
    {
      QStringList debugStr(table.values(uniqueKey));
      kDebug(36002) << "The cells depending on " << uniqueKey << " are: " << debugStr.join(", ") << endl;
    }
  }
}

DependencyManager::DependencyManager()
  : d(new Private)
{
}

DependencyManager::~DependencyManager ()
{
  qDeleteAll(d->dependents.values());
  delete d;
}

void DependencyManager::reset ()
{
  d->reset();
}

void DependencyManager::regionChanged(const Region& region)
{
  if (region.isEmpty())
    return;
  kDebug(36002) << "DependencyManager::regionChanged " << region.name() << endl;
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

        // empty or default cell or cell without a formula? remove it
        if ( cell->isEmpty() || !cell->isFormula() )
        {
          d->removeDependencies(cell);
          continue;
        }

        // don't re-generate dependencies if we're updating dependencies
//         if ( !(cell->testFlag (Cell::Flag_Progress)))
          d->generateDependencies(cell);
      }
    }
  }
  d->dump();
}

void DependencyManager::areaModified (const QString &name)
{
  d->areaModified (name);
}

void DependencyManager::updateAllDependencies(const Map* map)
{
  foreach (const Sheet* sheet, map->sheetList())
  {
    for (Cell* cell = sheet->firstCell(); cell; cell = cell->nextCell())
    {
      // empty or default cell or cell without a formula? remove it
      if ( cell->isEmpty() || !cell->isFormula() )
      {
        d->removeDependencies(cell);
        continue;
      }

      d->generateDependencies(cell);
    }
  }
}

// RangeList DependencyManager::getDependencies (const Region::Point &cell)
// {
//   return d->getDependencies (cell);
// }

KSpread::Region DependencyManager::getDependents(const Cell* cell)
{
  return d->getDependents(cell);
}

QMap<KSpread::Region::Point, KSpread::Region> DependencyManager::dependencies() const
{
  return d->dependencies;
}

void DependencyManager::regionMoved( const Region& movedRegion, const Region::Point& destination )
{
  Region::Point locationOffset( destination.pos() - movedRegion.boundingRect().topLeft() );

  Region::ConstIterator end( movedRegion.constEnd() );
  for ( Region::ConstIterator it( movedRegion.constBegin() ); it != end; ++it )
  {
    Sheet* const sheet = (*it)->sheet();
    locationOffset.setSheet( ( sheet == destination.sheet() ) ? 0 : destination.sheet() );
    const QRect range = (*it)->rect();

    if ( d->dependents.contains( sheet ) )
    {
      const QRectF rangeF = QRectF( range ).adjusted( 0, 0, -0.1, -0.1 );
      QList<Region::Point> dependentLocations = d->dependents[sheet]->intersects( rangeF );

      for ( int i = 0; i < dependentLocations.count(); ++i )
      {
        const Region::Point location = dependentLocations[i];
        Cell* const cell = location.sheet()->cellAt( location.pos().x(), location.pos().y() );
        updateFormula( cell, (*it), locationOffset );
      }
    }
  }
}

void DependencyManager::updateFormula( Cell* cell, const Region::Element* oldLocation, const Region::Point& offset )
{
  // Not a formula -> no dependencies
  if (!cell->isFormula())
    return;

  // Broken formula -> meaningless dependencies
  // (tries to avoid cell->formula() being null)
  if (cell->testFlag(Cell::Flag_ParseError))
    return;

  const Formula* formula = cell->formula();
  if ( !formula )
  {
    kDebug(36002) << "Cell at row " << cell->row() << ", col " << cell->column() << " marked as formula, but formula is 0. Formula string: " << cell->text() << endl;
    return;
  }

  Tokens tokens = formula->tokens();

  //return empty list if the tokens aren't valid
  if (!tokens.valid())
    return;

  QString expression = "=";
  Sheet* sheet = cell->sheet();
  Region region;
  for( int i = 0; i < tokens.count(); i++ )
  {
    Token token = tokens[i];
    Token::Type tokenType = token.type();

    //parse each cell/range and put it to our RangeList
    if (tokenType == Token::Cell || tokenType == Token::Range)
    {
      const Region region( sheet->workbook(), token.text(), sheet );
      const Region::Element* element = *region.constBegin();

      kDebug(36002) << region.name() << endl;
      // the offset contains a sheet, only if it was an intersheet move.
      if ( ( oldLocation->sheet() == element->sheet() ) &&
           ( oldLocation->rect().contains( element->rect() ) ) )
      {
        const Region yetAnotherRegion( element->rect().translated( offset.pos() ), offset.sheet() ? offset.sheet() : sheet );
        expression.append( yetAnotherRegion.name( sheet ) );
      }
      else
      {
        expression.append( token.text() );
      }
    }
    else
    {
      expression.append( token.text() );
    }
  }
  cell->setCellText( expression );
}

void DependencyManager::Private::reset ()
{
  dependencies.clear();
  dependents.clear();
}

KSpread::Region DependencyManager::Private::getDependencies (const Region::Point &cell)
{
  return dependencies.value(cell);
}

KSpread::Region DependencyManager::Private::getDependents(const Cell* cell)
{
  Sheet* const sheet = cell->sheet();

  if (!dependents.contains(sheet))
  {
    kDebug(36002) << "No dependent tree found for the cell's sheet." << endl;
    return Region();
  }

  KoRTree<Region::Point>* tree = dependents.value(sheet);

  Region region;
  foreach (Region::Point point, tree->contains(QPoint(cell->column(), cell->row())))
  {
    region.add(point.pos(), point.sheet());
  }
  return region;
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
  if (region.isEmpty())
    return;

  Region::Point point(QPoint(cell->column(), cell->row()));
  point.setSheet(cell->sheet());

  // empty region will be created automatically, if necessary
  dependencies[point].add(region);

  Region::ConstIterator end(region.constEnd());
  for (Region::ConstIterator it(region.constBegin()); it != end; ++it)
  {
    Sheet* sheet = (*it)->sheet();
    QRectF range = QRectF((*it)->rect()).adjusted(0, 0, -0.1, -0.1);

    if (!dependents.contains(sheet)) dependents.insert(sheet, new PointTree);
    dependents[sheet]->insert(range, point);
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
    Sheet* const sheet = (*it)->sheet();
    const QRect range = (*it)->rect();

    if (dependents.contains(sheet))
    {
      dependents[sheet]->remove(point);
    }
  }

  // remove information about named area dependencies
  QMap<QString, QMap<Region::Point, bool> >::iterator itr;
  for (itr = areaDeps.begin(); itr != areaDeps.end(); ++itr) {
    if (itr.value().contains (point))
      itr.value().remove (point);
  }

  // clear the circular dependency flags
//   if (cell->testFlag(Cell::Flag_CircularCalculation))
  {
    removeCircularDependencyFlags(dependencies.value(point));
  }

  // finally, remove the entry about this cell
  dependencies.remove(point);
}

void DependencyManager::Private::generateDependencies(const Cell* cell)
{
  //new dependencies only need to be generated if the cell contains a formula
  if (cell->isDefault())
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
  if (cell->testFlag(Cell::Flag_ParseError))
    return Region();

  const Formula* f = cell->formula();
  if (f==0)
  {
    kDebug(36002) << "Cell at row " << cell->row() << ", col " << cell->column() << " marked as formula, but formula is 0. Formula string: " << cell->text() << endl;
//     Q_ASSERT(cell->formula());
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

void DependencyManager::Private::removeCircularDependencyFlags(const Region& region)
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

        if (cell->testFlag(Cell::Flag_UpdatingDeps))
          continue;
        cell->setFlag(Cell::Flag_UpdatingDeps);

        cell->clearFlag(Cell::Flag_CircularCalculation);

        Region::Point point(col, row);
        point.setSheet(cell->sheet());
        removeCircularDependencyFlags(dependencies.value(point));

        cell->clearFlag(Cell::Flag_UpdatingDeps);
      }
    }
  }
}
