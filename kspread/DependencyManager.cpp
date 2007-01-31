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
#include "CellStorage.h"
#include "Formula.h"
#include "Map.h"
#include "Region.h"
#include "Sheet.h"
#include "Value.h"

#include "DependencyManager.h"

using namespace KSpread;

class DependencyManager::Private
{
public:
    /**
     * Clears internal structures.
     */
    void reset();

    /**
     * Generates the dependencies of \p cell .
     * First, it removes the old providing region. Then, the new providing
     * region is computed. Finally, adds \p cell as consumer and the new
     * providing region to the data structures.
     * \see removeDependencies
     * \see computeDependencies
     * \see addDependencies
     */
    void generateDependencies(const Cell& cell);

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
    int computeDepth(Cell cell) const;

    /**
     * Used in the recalculation events for changed regions.
     * Determines the reference depth for each position in \p region .
     * Calls itself recursively for the regions, that depend on cells
     * in \p region .
     *
     * \see computeDepth
     */
    void generateDepths(const Region& region);

    /**
     * Returns the region, that consumes the value of \p cell.
     * \return region consuming \p cell 's value
     */
    Region consumingRegion(const Cell& cell) const;

    void areaModified (const QString &name);

    /**
     * Updates structures: \p cell depends on cells in \p region and vice versa.
     */
    void addDependencies(const Cell& cell, const Region& region);

    /**
     * Removes all dependencies of \p cell .
     */
    void removeDependencies(const Cell& cell);

    /**
     * \return a list of cells that \p cell depends on
     */
    Region computeDependencies(const Cell& cell) const;

    /**
     * Removes the circular dependency flag from \p region and all their dependencies.
     */
    void removeCircularDependencyFlags(const Region& region);

    /**
     * Helper function for DependencyManager::cellsToCalculate().
     */
    void cellsToCalculate( const Region& region, QSet<Cell>& cells ) const;

    /**
     * For debugging/testing purposes.
     */
    void dump() const;

    // stores providing regions ordered by their consuming cell locations
    QMap<Region::Point, Region> providers;
    // stores consuming cell locations ordered by their providing regions
    class PointTree :public KoRTree<Region::Point>
    { public: PointTree() : KoRTree<Region::Point>(8,4) {} };
    QHash<Sheet*, PointTree*> consumers; // FIXME Stefan: Why is a QHash<Sheet*, PointTree> crashing?
    // list of cells referencing a given named area
    QMap<QString, QMap<Region::Point, bool> > areaDeps;
    /*
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
    QHash<Cell, int> depths;
};

// This is currently not called - but it's really convenient to call it from
// gdb or from debug output to check that everything is set up ok.
void DependencyManager::Private::dump() const
{
    QMap<Region::Point, Region>::ConstIterator mend(providers.end());
    for ( QMap<Region::Point, Region>::ConstIterator mit(providers.begin()); mit != mend; ++mit )
    {
        Region::Point p = mit.key();

        QStringList debugStr;
        Region::ConstIterator rend((*mit).constEnd());
        for ( Region::ConstIterator rit((*mit).constBegin()); rit != rend; ++rit )
        {
            debugStr << (*rit)->name();
        }

        kDebug(36002) << p.name() << " consumes values of: " << debugStr.join(", ") << endl;
    }

    foreach (Sheet* sheet, consumers.keys())
    {
        QList<QRectF> keys = consumers[sheet]->keys();
        QList<Region::Point> values = consumers[sheet]->values();
        QHash<QString, QString> table;
        for (int i = 0; i < keys.count(); ++i)
        {
            Region tmpRange(keys[i].toRect(), sheet);
            table.insertMulti(tmpRange.name(), values[i].name());
        }
        foreach (QString uniqueKey, table.uniqueKeys())
        {
            QStringList debugStr(table.values(uniqueKey));
            kDebug(36002) << uniqueKey << " provides values for: " << debugStr.join(", ") << endl;
        }
    }

    foreach ( Cell cell, depths.keys() )
    {
        QString cellName = cell.name();
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
    qDeleteAll(d->consumers.values());
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
                Cell cell( sheet,col, row);

                // remove it from the reference depth list
                d->depths.remove( cell );

                // cell without a formula? remove it
                if ( !cell.isFormula() )
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
    ElapsedTime( "Generating dependencies", ElapsedTime::PrintOnlyTime );

    // clear the reference depth list
    d->depths.clear();

    Cell cell;
    foreach (const Sheet* sheet, map->sheetList())
    {
        for ( int c = 0; c < sheet->formulaStorage()->count(); ++c )
        {
            cell = Cell( sheet, sheet->formulaStorage()->col( c ), sheet->formulaStorage()->row( c ) );

            // empty or default cell or cell without a formula? remove it
            if ( sheet->formulaStorage()->data( c ).expression().isEmpty() )
            {
                d->removeDependencies( cell );
                continue;
            }

            d->generateDependencies( cell );
            if (!d->depths.contains( cell ))
            {
                int depth = d->computeDepth( cell );
                d->depths.insert( cell , depth);
            }
        }
    }
}

KSpread::Region DependencyManager::consumingRegion(const Cell& cell) const
{
    return d->consumingRegion(cell);
}

QMap<int, Cell> DependencyManager::cellsToCalculate( const Region& region ) const
{
    if (region.isEmpty())
        return QMap<int, Cell>();

    QSet<Cell> cells;
    d->cellsToCalculate( region, cells );
    QMap<int, Cell> depths;
    foreach ( Cell cell, cells )
        depths.insertMulti( d->depths[cell], cell );
    return depths;
}

QMap<int, Cell> DependencyManager::cellsToCalculate( Sheet* sheet ) const
{
    QMap<int, Cell> depths;
    foreach ( Cell cell, d->depths.keys() )
    {
        if ( sheet && cell.sheet() != sheet )
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

        if ( d->consumers.contains( sheet ) )
        {
            const QRectF rangeF = QRectF( range ).adjusted( 0, 0, -0.1, -0.1 );
            QList<Region::Point> dependentLocations = d->consumers[sheet]->intersects( rangeF );

            for ( int i = 0; i < dependentLocations.count(); ++i )
            {
                const Region::Point location = dependentLocations[i];
                const Cell cell( location.sheet(), location.pos() );
                updateFormula( cell, (*it), locationOffset );
            }
        }
    }
}

void DependencyManager::updateFormula( const Cell& cell, const Region::Element* oldLocation, const Region::Point& offset )
{
    // Not a formula -> no dependencies
    if (!cell.isFormula())
        return;

    const Formula formula = cell.formula();

    // Broken formula -> meaningless dependencies
    if ( !formula.isValid() )
        return;

    Tokens tokens = formula.tokens();

    //return empty list if the tokens aren't valid
    if (!tokens.valid())
        return;

    QString expression = "=";
    Sheet* sheet = cell.sheet();
    Region region;
    for( int i = 0; i < tokens.count(); i++ )
    {
        Token token = tokens[i];
        Token::Type tokenType = token.type();

        //parse each cell/range and put it to our expression
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
    Cell( cell ).setCellText( expression );
}

void DependencyManager::Private::reset()
{
    providers.clear();
    consumers.clear();
}

KSpread::Region DependencyManager::Private::consumingRegion(const Cell& cell) const
{
    Sheet* const sheet = cell.sheet();

    if (!consumers.contains(sheet))
    {
        kDebug(36002) << "No consumer tree found for the cell's sheet." << endl;
        return Region();
    }

    const KoRTree<Region::Point>* tree = consumers.value(sheet);
    const QList<Region::Point> providers = tree->contains(cell.cellPosition());

    Region region;
    foreach (const Region::Point& point, providers)
        region.add(point.pos(), point.sheet());
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
        Cell cell( it.key().sheet(), it.key().pos() );
        // this forces the cell to regenerate everything - new range dependencies
        // and so on
        cell.setValue (cell.value ());
    }
}

void DependencyManager::Private::addDependencies(const Cell& cell, const Region& region)
{
    // NOTE Stefan: Also store cells without dependencies to avoid an
    //              iteration over all cells in a map/sheet on recalculation.

    Region::Point point(QPoint(cell.column(), cell.row()));
    point.setSheet(cell.sheet());

    // empty region will be created automatically, if necessary
    providers[point].add(region);

    Region::ConstIterator end(region.constEnd());
    for (Region::ConstIterator it(region.constBegin()); it != end; ++it)
    {
        Sheet* sheet = (*it)->sheet();
        QRectF range = QRectF((*it)->rect()).adjusted(0, 0, -0.1, -0.1);

        if (!consumers.contains(sheet)) consumers.insert(sheet, new PointTree);
        consumers[sheet]->insert(range, point);
    }
}

void DependencyManager::Private::removeDependencies(const Cell& cell)
{
    Region::Point point(QPoint(cell.column(), cell.row()));
    point.setSheet(cell.sheet());

    // look if the cell has any providers
    if (!providers.contains (point))
        return;  //it doesn't - nothing more to do

    // first this cell is no longer a provider for all providers
    Region region = providers[point];
    Region::ConstIterator end(region.constEnd());
    for (Region::ConstIterator it(region.constBegin()); it != end; ++it)
    {
        Sheet* const sheet = (*it)->sheet();
        const QRect range = (*it)->rect();

        if (consumers.contains(sheet))
        {
            consumers[sheet]->remove(point);
        }
    }

    // remove information about named area dependencies
    QMap<QString, QMap<Region::Point, bool> >::iterator itr;
    for (itr = areaDeps.begin(); itr != areaDeps.end(); ++itr) {
        if (itr.value().contains (point))
            itr.value().remove (point);
    }

    // clear the circular dependency flags
//   if ( cell.value() == Value::errorCIRCLE() )
    {
        removeCircularDependencyFlags(providers.value(point));
    }

    // finally, remove the entry about this cell
    providers.remove(point);
}

void DependencyManager::Private::generateDependencies(const Cell& cell)
{
    //new dependencies only need to be generated if the cell contains a formula
//     if (cell.isNull())
//         return;
//     if (!cell.isFormula())
//         return;

    //get rid of old dependencies first
    removeDependencies(cell);

    //now we need to generate dependencies
    Region region = computeDependencies(cell);

    //now that we have the new dependencies, we put them into our data structures
    //and we're done
    addDependencies(cell, region);
}

void DependencyManager::Private::generateDepths(const Region& region)
{
    static QSet<Cell> processedCells;

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
                Cell cell( sheet,col, row);

                // skip non-formula cells
                if ( !cell.isFormula() )
                    continue;

                //prevent infinite recursion (circular dependencies)
                if ( processedCells.contains( cell ) || cell.value() == Value::errorCIRCLE() )
                {
                    kDebug(36002) << "Circular dependency at " << cell.fullName() << endl;
                    // don't set anything if the cell already has all these things set
                    // this prevents endless loop for inter-sheet curcular dependencies,
                    // where the usual mechanisms fail doe to having multiple dependency
                    // managers ...
                    if ( cell.value() != Value::errorCIRCLE() )
                        cell.setValue( Value::errorCIRCLE() );
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
                Region dependentRegion = consumingRegion(cell);
                if (!dependentRegion.contains(QPoint(col, row), cell.sheet()))
                    generateDepths(dependentRegion);

                // clear the compute reference depth flag
                processedCells.remove( cell );
            }
        }
    }
}

int DependencyManager::Private::computeDepth(Cell cell) const
{
    // a set of cell, which depth is currently calculated
    static QSet<Cell> processedCells;

    //prevent infinite recursion (circular dependencies)
    if ( processedCells.contains( cell ) || cell.value() == Value::errorCIRCLE() )
    {
        kDebug(36002) << "Circular dependency at " << cell.fullName() << endl;
        // don't set anything if the cell already has all these things set
        // this prevents endless loop for inter-sheet curcular dependencies,
        // where the usual mechanisms fail doe to having multiple dependency
        // managers ...
        if ( cell.value() != Value::errorCIRCLE() )
            cell.setValue( Value::errorCIRCLE() );
        //clear the compute reference depth flag
        processedCells.remove( cell );
        return 0;
    }

    // set the compute reference depth flag
    processedCells.insert( cell );

    int depth = 0;

    Region::Point point(cell.column(), cell.row());
    point.setSheet(cell.sheet());
    const Region region = providers.value(point);

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
                if (!providers.contains(referencedPoint))
                {
                    // no further references
                    // depth is one at least
                    depth = qMax(depth, 1);
                    continue;
                }

                Cell referencedCell = Cell( sheet,col, row);
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

KSpread::Region DependencyManager::Private::computeDependencies(const Cell& cell) const
{
    // Not a formula -> no dependencies
//     if (!cell.isFormula())
//         return Region();

    const Formula formula = cell.formula();

    // Broken formula -> meaningless dependencies
    if ( !formula.isValid() )
        return Region();

    Tokens tokens = formula.tokens();

    //return empty list if the tokens aren't valid
    if (!tokens.valid())
        return Region();

    Sheet* sheet = cell.sheet();
    Region region;
    for( int i = 0; i < tokens.count(); i++ )
    {
        Token token = tokens[i];
        Token::Type tokenType = token.type();

        //parse each cell/range and put it to our Region
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
    static QSet<Cell> processedCells;

    Region::ConstIterator end(region.constEnd());
    for (Region::ConstIterator it(region.constBegin()); it != end; ++it)
    {
        const QRect range = (*it)->rect();
        const Sheet* sheet = (*it)->sheet();
        for (int col = range.left(); col <= range.right(); ++col)
        {
            for (int row = range.top(); row <= range.bottom(); ++row)
            {
                Cell cell( sheet,col, row);

                if ( processedCells.contains( cell ) )
                    continue;
                processedCells.insert( cell );

                if ( cell.value() == Value::errorCIRCLE() )
                    cell.setValue( Value::empty() );

                Region::Point point(col, row);
                point.setSheet(cell.sheet());
                removeCircularDependencyFlags(providers.value(point));

                processedCells.remove( cell );
            }
        }
    }
}

void DependencyManager::Private::cellsToCalculate( const Region& region, QSet<Cell>& cells ) const
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
                Cell cell( sheet,col, row);
                // Even empty cells may act as value
                // providers and need to be processed.

                // check for already processed cells
                if ( cells.contains( cell ) )
                    continue;

                // add it to the list
                if ( cell.isFormula() )
                    cells.insert( cell );

                // add its consumers to the list
                cellsToCalculate( consumingRegion( cell ), cells );
            }
        }
    }
}
