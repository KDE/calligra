/* This file is part of the KDE project
   Copyright 2006-2007 Stefan Nikolaus <stefan.nikolaus@kdemail.net>
   Copyright 2004 Tomas Mecir <mecirt@gmail.com>

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

#ifndef KSPREAD_DEPENDENCY_MANAGER_P
#define KSPREAD_DEPENDENCY_MANAGER_P

// Local
#include "DependencyManager.h"

#include <QHash>
#include <QList>

#include "Cell.h"
#include "Region.h"
#include "RTree.h"

namespace KSpread
{
class Formula;
class Map;
class Sheet;

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
     */
    void generateDependencies(const Cell& cell, const Formula& formula);

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
     *
     * \see computeDepth
     * \see generateDepths(Cell cell)
     */
    void generateDepths(const Region& region);

    /**
     * Generates the depth of cell and all of its consumers.
     * Calls itself recursively for the cell's consuming cells.
     */
    void generateDepths(Cell cell, QSet<Cell>& computedDepths);

    /**
     * Returns the region, that consumes the value of \p cell.
     * \return region consuming \p cell 's value
     */
    Region consumingRegion(const Cell& cell) const;

    void namedAreaModified(const QString& name);

    /**
     * Removes all dependencies of \p cell .
     */
    void removeDependencies(const Cell& cell);

    /**
     * Removes the depths of \p cell and all its consumers.
     */
    void removeDepths(const Cell& cell);

    /**
     * Computes and stores the dependencies.
     */
    void computeDependencies(const Cell& cell, const Formula& formula);

    enum Direction { Forward, Backward };
    /**
     * Removes the circular dependency flag from \p region and all their dependencies.
     */
    void removeCircularDependencyFlags(const Region& region, Direction direction);

    /**
     * For debugging/testing purposes.
     */
    void dump() const;

    const Map* map;
    // stores providing regions ordered by their consuming cell locations
    QHash<Cell, Region> providers;
    // stores consuming cell locations ordered by their providing regions
    QHash<Sheet*, RTree<Cell>*> consumers;
    // stores consuming cell locations ordered by their providing named area
    QHash<QString, QList<Cell> > namedAreaConsumers;
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

} // namespace KSpread

#endif // KSPREAD_DEPENDENCY_MANAGER_P
