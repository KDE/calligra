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
   * Computes the reference depth.
   * Depth means the maximum depth of all cells this cell depends on plus one,
   * while a cell, which do not refer to other cells, has a depth
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
  int computeDepth(Cell* cell) const;

  /**
   * Used in the recalculation events for changed regions.
   * Determines the reference depth for each position in \p region .
   * Calls itself recursively for the regions, that depend on cells
   * in \p region .
   */
  void generateDepths(const Region& region);

  /**
   * \return dependencies of \p cell
   */
  Region getDependencies (const Region::Point &cell);

  /**
   * \return cells depending on \p cell
   */
  Region getDependents(const Cell* cell) const;

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

  /**
   * Helper function for DependencyManager::cellsToCalculate().
   */
  void cellsToCalculate( const Region& region, QSet<Cell*>& cells ) const;

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
  QHash<Cell*, int> depths;
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

    foreach ( Cell* cell, depths.keys() )
    {
        QString cellName = cell->name();
        while ( cellName.count() < 4 ) cellName.prepend( ' ' );
        kDebug(36002) << "depth( " << cellName << " ) = " << depths[cell] << endl;
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

                // remove it from the reference depth list
                d->depths.remove( cell );

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
    {
        ElapsedTime et( "Computing reference depths", ElapsedTime::PrintOnlyTime );
        d->generateDepths(region);
    }
    d->dump();
}

void DependencyManager::areaModified (const QString &name)
{
  d->areaModified (name);
}

void DependencyManager::updateAllDependencies(const Map* map)
{
    // clear the reference depth list
    d->depths.clear();

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
            if (!d->depths.contains(cell))
            {
                int depth = d->computeDepth(cell);
                d->depths.insert(cell, depth);
            }
        }
    }
}

// RangeList DependencyManager::getDependencies (const Region::Point &cell)
// {
//   return d->getDependencies (cell);
// }

KSpread::Region DependencyManager::getDependents(const Cell* cell) const
{
  return d->getDependents(cell);
}

QMap<int, Cell*> DependencyManager::cellsToCalculate( const Region& region ) const
{
    if (region.isEmpty())
        return QMap<int, Cell*>();

    QSet<Cell*> cells;
    d->cellsToCalculate( region, cells );
    QMap<int, Cell*> depths;
    foreach ( Cell* cell, cells )
        depths.insertMulti( d->depths[cell], cell );
    return depths;
}

QMap<int, Cell*> DependencyManager::cellsToCalculate( Sheet* sheet ) const
{
    QMap<int, Cell*> depths;
    foreach ( Cell* cell, d->depths.keys() )
    {
        if ( sheet && cell->sheet() != sheet )
            continue;
        depths.insertMulti( d->depths[cell], cell );
    }
    return depths;
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

    const Formula* formula = cell->formula();
    if ( !formula )
    {
        kDebug(36002) << "Cell at row " << cell->row() << ", col " << cell->column() << " marked as formula, but formula is 0. Formula string: " << cell->inputText() << endl;
        return;
    }

    // Broken formula -> meaningless dependencies
    if ( !formula->isValid() )
        return;

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
            const Region region( sheet->map(), token.text(), sheet );
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

KSpread::Region DependencyManager::Private::getDependents(const Cell* cell) const
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
    // NOTE Stefan: Also store cells without dependencies to avoid an
    //              iteration over all cells in a map/sheet on recalculation.

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
//   if ( cell->value() == Value::errorCIRCLE() )
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

void DependencyManager::Private::generateDepths(const Region& region)
{
    static QSet<Cell*> processedCells;

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
                if ( cell->isDefault() )
                    continue;

                //prevent infinite recursion (circular dependencies)
                if ( processedCells.contains( cell ) || cell->value() == Value::errorCIRCLE() )
                {
                    kDebug(36002) << "Circular dependency at " << cell->fullName() << endl;
                    // don't set anything if the cell already has all these things set
                    // this prevents endless loop for inter-sheet curcular dependencies,
                    // where the usual mechanisms fail doe to having multiple dependency
                    // managers ...
                    if ( cell->value() != Value::errorCIRCLE() )
                        cell->setValue( Value::errorCIRCLE() );
                    depths.insert(cell, 0);
                    // clear the compute reference depth flag
                    processedCells.remove( cell );
                    continue;
                }

                // set the compute reference depth flag
                processedCells.insert( cell );

                int depth = computeDepth(cell);
                depths.insert(cell, depth);

                // Recursion. We need the whole dependency tree of the changed region.
                // An infinite loop is prevented by the check above.
                Region dependentRegion = getDependents(cell);
                if (!dependentRegion.contains(QPoint(col, row), cell->sheet()))
                    generateDepths(dependentRegion);

                // clear the compute reference depth flag
                processedCells.remove( cell );
            }
        }
    }
}

int DependencyManager::Private::computeDepth(Cell* cell) const
{
    // a set of cell, which depth is currently calculated
    static QSet<Cell*> processedCells;

    //prevent infinite recursion (circular dependencies)
    if ( processedCells.contains( cell ) || cell->value() == Value::errorCIRCLE() )
    {
        kDebug(36002) << "Circular dependency at " << cell->fullName() << endl;
        // don't set anything if the cell already has all these things set
        // this prevents endless loop for inter-sheet curcular dependencies,
        // where the usual mechanisms fail doe to having multiple dependency
        // managers ...
        if ( cell->value() != Value::errorCIRCLE() )
            cell->setValue( Value::errorCIRCLE() );
        //clear the compute reference depth flag
        processedCells.remove( cell );
        return 0;
    }

    // set the compute reference depth flag
    processedCells.insert( cell );

    int depth = 0;

    Region::Point point(cell->column(), cell->row());
    point.setSheet(cell->sheet());
    const Region region = dependencies.value(point);

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
                if (!dependencies.contains(referencedPoint))
                {
                    // no further references
                    // depth is one at least
                    depth = qMax(depth, 1);
                    continue;
                }

                Cell* referencedCell = sheet->cellAt(col, row);
                if (depths.contains(referencedCell))
                {
                    // the referenced cell depth was already computed
                    depth = qMax(depths[referencedCell] + 1, depth);
                    continue;
                }

                // compute the depth of the referenced cell, add one and
                // take it as new depth, if it's greater than the current one
                depth = qMax(computeDepth(referencedCell) + 1, depth);
            }
        }
    }

    //clear the computing reference depth flag
    processedCells.remove( cell );

    return depth;
}

KSpread::Region DependencyManager::Private::computeDependencies(const Cell* cell) const
{
  // Not a formula -> no dependencies
    if (!cell->isFormula())
        return Region();

    const Formula* f = cell->formula();
    if (f==0)
    {
        kDebug(36002) << "Cell at row " << cell->row() << ", col " << cell->column() << " marked as formula, but formula is 0. Formula string: " << cell->inputText() << endl;
//     Q_ASSERT(cell->formula());
        return Region();
    }

    // Broken formula -> meaningless dependencies
    if ( !f->isValid() )
        return Region();

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
            Region subRegion(sheet->map(), token.text(), sheet);
            if (subRegion.isValid())
                region.add(subRegion);
        }
    }
    return region;
}

void DependencyManager::Private::removeCircularDependencyFlags(const Region& region)
{
  // a set of cells, which circular dependency flag is currently removed
  static QSet<Cell*> processedCells;

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

        if ( processedCells.contains( cell ) )
          continue;
        processedCells.insert( cell );

        if ( cell->value() == Value::errorCIRCLE() )
            cell->setValue( Value::empty() );

        Region::Point point(col, row);
        point.setSheet(cell->sheet());
        removeCircularDependencyFlags(dependencies.value(point));

        processedCells.remove( cell );
      }
    }
  }
}

void DependencyManager::Private::cellsToCalculate( const Region& region, QSet<Cell*>& cells ) const
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

                // check for already processed cells
                if ( cells.contains( cell ) )
                    continue;

                // add it to the list
                cells.insert( cell );

                // add its dependents to the list
                const Region dependencies = getDependents( cell );
                cellsToCalculate( dependencies, cells );
            }
        }
    }
}
